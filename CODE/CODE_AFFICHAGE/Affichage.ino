#include "affichage.h"
#include <Ultrasonic.h>
//#include <TimerOne.h>

Ultrasonic ultrasonicG(10);
Ultrasonic ultrasonicD(8);
Ultrasonic ultrasonicF(12);

void securite();


#define Thash 800
#define Stop 400
#define Vmax Thash
// Macros
#define LedToggle digitalWrite(13, !digitalRead(13))
#define MoteurG(Vg) OCR5A=Vg // Vg in [0... 1999]
#define MoteurD(Vd) OCR5B=Vd // VD in [0... 1999]
#define MoteurGD(Vg,Vd) MoteurG(Vg);MoteurD(Vd)
#define StopMoteurGD MoteurGD(Stop,Stop)

void initMoteurs() {  // MoteurG :OC5A=PIN46-PL3, MoteurD : OC5B=PIN45-PL4
  DDRL = 0x18 ; // PL3 et PL4
  DDRB = 0x80 ; // PB7 LedToggle
  // COM5B_1:0 = 10   -> clear sur egalite++, set sur egalite--
  // WGM5_3:1 = 1000  -> mode 8 => ICR5 defini le TOP
  TCCR5A = (1 << COM5A1) + (1 << COM5B1);
  TCCR5B = (1 << ICNC5) + (1 << WGM53) + (1 << CS50); // CS_12:10  = 001  -> prediv par 1
  ICR5 = Thash; // 1999 correspond a f = 4khz
  StopMoteurGD;
  // Interrution de débordement du timer
  TIMSK5 = 1 << TOIE5;
}

ISR (TIMER5_OVF_vect) { // Pour la lecture du courant 
  LedToggle;

}


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(A2, INPUT);
  //Timer1.initialize(100*1000); // Vérifie toutes les 100 ms
  //Timer1.attachInterrupt(securite);
  
  pinMode(43,OUTPUT);
  digitalWrite(43,0);
  initMoteurs();
  sei();
  digitalWrite(43,1);
  MoteurG(800);
  MoteurD(800);
  lcd.setCursor(0, 0);
}

void loop() {
  //AFFICHAGE
  //affichage(1);

  //ARRET
  securite();
}

void securite(){
  if (ultrasonicF.MeasureInCentimeters() < 5 || ultrasonicF.MeasureInCentimeters() < 5 ultrasonicF.MeasureInCentimeters() < 5){
    MoteurGD(400,400);
  }
  else{
  MoteurG(800);
  MoteurD(800);
  }
}