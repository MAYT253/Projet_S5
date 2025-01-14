#include "Moteur.h"
#include "affichage.h"
#include "Arret.h"
#include "Ultrasonic.h"
#include <Encoder.h>

int vg = 800;
int vd = 800;

float Kp = 5.0;
int flage = 0;

float Kp_min = 2.0;            // Minimum allowed Kp
float Kp_max = 4.0;            // Maximum allowed Kp
float error_threshold = 10.0;  // Error threshold for adjusting Kp
float mod_rate = 0.008;         // Rate of Kp adjustment
float mod_rate2 = 0.008;

unsigned long previousMillis_PID = 0;    // Stores the last time the action was performed
const unsigned long interval_PID = 400;  // 400 milliseconds

//Regulation
float e = 0.0, e2 = 0.0;
volatile int commande = 0, commande2 = 0;
volatile int vistess_const2 = 782, vistess_const1 = 800;
volatile int RangeF, RangeG, RangeD;

//variable menu PID
int X;
float Coff_Corr_Roue_D = 8.0;
float Coff_Corr_Roue_G = 12.0;
int enter = 0;

const int desired_distance = 60;  // Desired distance in cm
const int tolerance = 1;

int code_arret = 0;
int compteur = 0;
float courant, tension, energie;
int nb_mur;
long temps_start;
long temps_final;

moteur moteur;

//Ultrason
Ultrasonic UltraD(8);
Ultrasonic UltraG(10);
Ultrasonic UltraF(12);

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
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setRGB(200, 200, 200);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  moteur.init();

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
}

void loop() {
  temps_start = millis();
  while (code_arret == 0) {
    RangeD = UltraD.MeasureInCentimeters();
    RangeF = UltraF.MeasureInCentimeters();
    RangeG = UltraG.MeasureInCentimeters();
    code_arret = Arret(codeur_L.read(), codeur_R.read(), moteur.CO_L, moteur.CO_R, RangeD, RangeG, RangeF);
    detect_mur();
    if (RangeF <= 90) {
      moteur.MoteurD(520);
      moteur.MoteurG(400);
      delay(500);
    } else if (RangeD < desired_distance - tolerance) {
      zone_1();

    } else if (RangeD > desired_distance + tolerance) {
      zone_1();
    }
    if (RangeF < 250 && RangeD < 100 && RangeG < 80) {
      vistess_const2 = 660, vistess_const1 = 600;
    }
    temps_final = millis() - temps_start;
    lcd.setCursor(0, 0);
    lcd.print(temps_final);
  }
  while (1) {
    affichage(code_arret, temps_final / 1000, energie, nb_mur, codeur_L.read() / 1200, codeur_R.read() / 1200);
    moteur.Stop();
  }
}


void zone_1() {
  int error = 60 - RangeG;
  if (error > error_threshold) {
    Coff_Corr_Roue_G += mod_rate;
  } else if (error == 1) {
    Coff_Corr_Roue_G -= mod_rate;
  }

  if (error < -5) {
    Coff_Corr_Roue_D += mod_rate2;
  } else if (error == -1) {
    Coff_Corr_Roue_D -= mod_rate2;
  }

  Coff_Corr_Roue_G = constrain(Coff_Corr_Roue_G, Kp_min, Kp_max);
  Coff_Corr_Roue_D = constrain(Coff_Corr_Roue_D, Kp_min, Kp_max);

  moteur.MoteurGD(vistess_const2, vistess_const1);

  if (RangeD < 59) {
    lcd.setRGB(200, 0, 0);
    e2 = abs(60 - RangeD) * Coff_Corr_Roue_G / 10.0;
    e = constrain(e2, 0, 50);
    commande = e;
    moteur.MoteurD(vistess_const1);
    moteur.MoteurG(vistess_const2 - commande);
  } else if (RangeD > 61) {
    lcd.setRGB(0, 0, 200);
    float e3 = abs(60 - RangeD) * (Coff_Corr_Roue_D / 10.0);
    float e1 = constrain(e3, 0, 100);
    commande2 = e1;
    moteur.MoteurG(vistess_const2);
    moteur.MoteurD(vistess_const1 - commande2);
  }
}

void detect_mur() {
  if (RangeF <= 20 || RangeD <= 20 || RangeG <= 20) nb_mur++;
}

void mesure_energie() {
  for (int i = 0; i < 10; i++) courant = abs(map(analogRead(A5), 0, 1023, MIN_CURRENT, MAX_CURRENT)) + abs(map(analogRead(A4), 0, 1023, MIN_CURRENT, MAX_CURRENT));
  courant = courant / 10;
  tension = (3.0 * analogRead(A0) * 498.0 / 1023.0);
  energie = energie + courant * tension / 1000;
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