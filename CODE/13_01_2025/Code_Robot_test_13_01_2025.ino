#include "Moteur.h"
#include "affichage.h"
#include "Arret.h"
#include <Encoder.h>

moteur moteur;

#define Jstk_seuil_max 600;
#define Jstk_seuil_min 400;

unsigned long previousMillis_PID = 0;    // Stores the last time the action was performed
const unsigned long interval_PID = 400;  // 400 milliseconds

//Regulation           
float e = 0.0, e2 = 0.0;
volatile int commande = 0, commande2 = 0;
volatile int vistess_const2 = 800, vistess_const1 = 710;
volatile int RangeF, RangeG, RangeD;
int vg_global=0;
int vd_global=0;

float Kp_min = 2.0; // Minimum allowed Kp
float Kp_max = 10.0; // Maximum allowed Kp
float error_threshold = 50.0; // Error threshold for adjusting Kp
float mod_rate = 0.01; // Rate of Kp adjustment

//variable menu PID
int X;
float Coff_Corr_Roue_D=10.0;
float Coff_Corr_Roue_G=4.0;
int enter=0;

const int desired_distance = 60; // Desired distance in cm
const int tolerance = 1;

int nb_mur;
int code_arret;
float energie;
int compteur = 0;
long temps_start;
long temps_final;
float courant, tension;

//Encoder
Encoder codeur_L(18, 29);
Encoder codeur_R(19, 27);

ISR(TIMER5_OVF_vect) {
  if (compteur++ >= 244 && code_arret == 0) {
    mesure_energie();
    compteur = 0;
  }
}

void setup() {
  lcd.begin(16, 2);
  moteur.init();
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  while (enter != 3) {
    //lcd.setCursor(0,0);
    //lcd.print(analogRead(A2));
    if (analogRead(A2) > 800) {
      unsigned long currentMillis_PID = millis();

      if (currentMillis_PID - previousMillis_PID >= interval_PID) {
        previousMillis_PID = currentMillis_PID;
        lcd.clear();
        enter++;
        if (enter > 3) enter = 0;
      }
    }
    if (enter == 1) {
      menu_err_D();
    }
    if (enter == 2) {
      menu_err_G();
    }
  }
  delay(5000);
}

void loop() {
  while (code_arret == 0) {
    code_arret = Arret(moteur.CO_L, moteur.CO_R, codeur_L.read(), codeur_R.read());
    //moteur.POLICE();
    detect_mur();
    //CODE ICI
    //moteur.MoteurGD(600, 600);
    temps_final = millis() - temps_start;
  
    RangeF = UltraF.MeasureInCentimeters();
    RangeD = UltraD.MeasureInCentimeters();
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(RangeF);
    
    // Check conditions
    if (RangeF <= 70) {
        moteur.MoteurD(520);
        moteur.MoteurG(400);
        delay(500);
    } else if(RangeD<desired_distance-tolerance){
    correction_trajectoire();
    } else if(RangeD>desired_distance+tolerance){
    correction_trajectoire();
    }
  
  }
  while (1) {
    delay(100);
    affichage(code_arret, temps_final / 1000, energie, nb_mur, codeur_L.read(), codeur_R.read());
    moteur.Stop();
  }
}

void detect_mur() {
  if (UltraF.MeasureInCentimeters() <= 20 || UltraD.MeasureInCentimeters() <= 20 || UltraG.MeasureInCentimeters() <= 20) nb_mur++;
}

void mesure_energie() {
  for (int i = 0; i < 10; i++) courant = abs(map(analogRead(A5), 0, 1023, MIN_CURRENT, MAX_CURRENT)) + abs(map(analogRead(A4), 0, 1023, MIN_CURRENT, MAX_CURRENT));
  courant = courant / 10;
  tension = (3.0 * analogRead(A0) * 4980.0 / 1023.0);
  energie = energie + courant * tension / 1000;
}

void correction_trajectoire() {
    RangeD = UltraD.MeasureInCentimeters();
    RangeF = UltraF.MeasureInCentimeters();
    /*lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(RangeD);
    lcd.setCursor(3,2);
    lcd.print (RangeF);*/
    int error=60-RangeG;
    if (error > error_threshold) {
      Coff_Corr_Roue_G += mod_rate;
    } else {
      Coff_Corr_Roue_G -= mod_rate; 
    }

    if (error < -5) {
      Coff_Corr_Roue_D += mod_rate;
    } else {
      Coff_Corr_Roue_D -= mod_rate; 
    }
  
    Coff_Corr_Roue_G = constrain(Coff_Corr_Roue_G, Kp_min, Kp_max);

    moteur.MoteurGD(vistess_const2, vistess_const1);

    if (RangeD <= 59) 
    {
      lcd.setRGB(200,0,0);
      e2 = abs(60 - RangeD)*Coff_Corr_Roue_G/10.0;
      e = constrain(e2, 0, 25);
      commande = e;
      vd_global=vistess_const1;//ajouter
      vg_global=vistess_const2 - commande;//ajouter
      moteur.MoteurD(vistess_const1);
      moteur.MoteurG(vistess_const2 - commande); 
    } else if (RangeD >= 61) { 
      lcd.setRGB(0,0,200);
        float e3 = abs(60 - RangeD) * (Coff_Corr_Roue_D/10.0);
        float e1 = constrain(e3, 0, 100);
        commande2 = e1;
        vd_global=vistess_const1 - commande2;//ajouter
        vg_global=vistess_const2;//ajouter
        moteur.MoteurG(vistess_const2);
        moteur.MoteurD(vistess_const1 - commande2);
    }
}

void menu_err_D() {
  X = (analogRead(A2) > 600 && analogRead(A2) < 800) ? 1 : ((analogRead(A2) < 400) ? -1 : 0);
  delay(400);
  Coff_Corr_Roue_D = Coff_Corr_Roue_D + X;
  Coff_Corr_Roue_D = constrain(Coff_Corr_Roue_D, 0, 20);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Coff Roue D:");
  lcd.setCursor(13, 0);
  lcd.print(Coff_Corr_Roue_D);
}

void menu_err_G() {

  X = (analogRead(A2) > 600 && analogRead(A2) < 800) ? 1 : ((analogRead(A2) < 400) ? -1 : 0);

  delay(400);
  Coff_Corr_Roue_G = Coff_Corr_Roue_G + X;
  Coff_Corr_Roue_G = constrain(Coff_Corr_Roue_G, 0, 10);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Coff Roue G:");
  lcd.setCursor(13, 0);
  lcd.print(Coff_Corr_Roue_G);
}
