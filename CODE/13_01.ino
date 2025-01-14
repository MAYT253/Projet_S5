#include "Ultrasonic.h"
#include "rgb_lcd.h"
#define MoteurG(Vg) OCR5A = Vg // Vg in [0...1999]
#define MoteurD(Vd) OCR5B = Vd // Vd in [0...1999]
#define MoteurGD(Vg, Vd) MoteurG(Vg); MoteurD(Vd)
#define StopMoteurGD MoteurGD(Stop, Stop)
#define Thash 800
#define Stop 400
#define Vmax Thash
#define EncAD 19
#define EncBD 27
#define EncAG 18
#define EncBG 29

int vg=800;
int vd=800;
int vg_global=0;
int vd_global=0;

float Kp = 5.0;
int flage=0;

float Kp_min = 2.0; // Minimum allowed Kp
float Kp_max = 7.0; // Maximum allowed Kp
float error_threshold = 10.0; // Error threshold for adjusting Kp
float mod_rate = 0.05; // Rate of Kp adjustment
float mod_rate2 = 0.008;

//Regulation           
float e = 0.0, e2 = 0.0;
volatile int commande = 0, commande2 = 0;
volatile int vistess_const2 = 800, vistess_const1 = 710;
volatile int RangeF, RangeG, RangeD;

//variable menu PID
int X;
float Coff_Corr_Roue_D=10.0;
float Coff_Corr_Roue_G=4.0;
int enter=0;

const int desired_distance = 60; // Desired distance in cm
const int tolerance = 1;

rgb_lcd lcd;
Ultrasonic ultrasonicG(10);
Ultrasonic ultrasonicD(8);
Ultrasonic ultrasonicF(12);

void initMoteurs() {
    DDRL = 0x18; // PL3 and PL4
    TCCR5A = (1 << COM5A1) + (1 << COM5B1);
    TCCR5B = (1 << ICNC5) + (1 << WGM53) + (1 << CS50); // Prediv by 1
    ICR5 = Thash; // 1999 corresponds to f = 4kHz
    StopMoteurGD;
//    TIMSK5 = 1 << TOIE5; // Timer overflow interrupt
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(EncAD,INPUT);
  pinMode(EncBD,INPUT);
  pinMode(EncAG,INPUT);
  pinMode(EncBG,INPUT);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(EncAD),readEncoderD,RISING);
  //attachInterrupt(digitalPinToInterrupt(EncAG),readEncoderG,RISING);

  lcd.begin(16,2);
  lcd.setRGB(200,200,200);

  pinMode(43, OUTPUT);
  digitalWrite(43, 0);
  initMoteurs();
  digitalWrite(43, 1);
}

void loop() {
  
  RangeD = ultrasonicD.MeasureInCentimeters();
  RangeF = ultrasonicF.MeasureInCentimeters();
  RangeG = ultrasonicG.MeasureInCentimeters();
  bool leftPressed = digitalRead(2) == LOW;    // Bumper gauche à l'état bas
  bool rightPressed = digitalRead(3) == LOW;  // Bumper droit à l'état bas
  
  //Serial.print(RangeD);
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(RangeF);
  if (digitalRead(6)||RangeG<5||RangeF<5||RangeD<5||leftPressed || rightPressed){
    while (1){
    MoteurGD(400, 400);
    if(digitalRead(6)&& flage==0){
     lcd.setCursor(0, 0);
      lcd.print("fin course");
    }
    else if(RangeG<5||RangeF<5||RangeD<5){
      lcd.setCursor(0, 0);
      lcd.print("ARRET Distance");
      flage=1;
    }else if (leftPressed || rightPressed){
      lcd.setCursor(0, 0);
      lcd.print("ARRET Bumper");
    }
    }
  }
  else if (RangeF <= 90) {
        MoteurD(520);
        MoteurG(400);
        delay(500);
    }
  else if(RangeD<desired_distance-tolerance){
    zone_1();
 
  } else if(RangeD>desired_distance+tolerance){
    zone_1();
  }

  if(RangeF<250 && RangeD<100 && RangeG<80){
    vistess_const2 = 660, vistess_const1 = 600;
  }
}


void zone_1() {
    RangeD = ultrasonicD.MeasureInCentimeters();
    RangeF = ultrasonicF.MeasureInCentimeters();
    /*lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(RangeD);
    lcd.setCursor(3,2);
    lcd.print (RangeF);*/
    int error=60-RangeG;
    if (error > error_threshold) {
      Coff_Corr_Roue_G += mod_rate;
    } else if (error ==1){
      Coff_Corr_Roue_G -= mod_rate; 
    }

    if (error < -5) {
      Coff_Corr_Roue_D += mod_rate2;
    } else if (error ==-1) {
      Coff_Corr_Roue_D -= mod_rate2; 
    }
  
    Coff_Corr_Roue_G = constrain(Coff_Corr_Roue_G, Kp_min, Kp_max);
    Coff_Corr_Roue_D = constrain(Coff_Corr_Roue_D, Kp_min, Kp_max);

    MoteurGD(vistess_const2, vistess_const1);

    if (RangeD <= 59) 
    {
      lcd.setRGB(200,0,0);
      e2 = abs(60 - RangeD)*Coff_Corr_Roue_G/10.0;
      e = constrain(e2, 0, 25);
      commande = e;
      vd_global=vistess_const1;//ajouter
      vg_global=vistess_const2 - commande;//ajouter
      MoteurD(vistess_const1);
      MoteurG(vistess_const2 - commande); 
    } else if (RangeD >= 61) { 
      lcd.setRGB(0,0,200);
        float e3 = abs(60 - RangeD) * (Coff_Corr_Roue_D/10.0);
        float e1 = constrain(e3, 0, 100);
        commande2 = e1;
        vd_global=vistess_const1 - commande2;//ajouter
        vg_global=vistess_const2;//ajouter
        MoteurG(vistess_const2);
        MoteurD(vistess_const1 - commande2);
    }
}