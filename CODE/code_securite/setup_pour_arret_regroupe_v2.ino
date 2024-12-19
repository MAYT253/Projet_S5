
#include "rgb_lcd.h"
#include <SparkFunLSM6DS3.h>

#define Thash 800
#define Stop 400
#define Vmax Thash
#define LedToggle digitalWrite(13, !digitalRead(13))
#define MoteurG(Vg) OCR5A = Vg
#define MoteurD(Vd) OCR5B = Vd
#define MoteurGD(Vg, Vd)
#define StopMoteurGD MoteurGD(Stop, Stop)

int CurrentR;
int CurrentL;
float ax, ay, az;
float gx, gy, gz;
const byte bumperLeft = 2;
const byte bumperRight = 3;
LSM6DS3 IMU(I2C_MODE, 0x6A);
const byte infrared = 6;

const byte bumperLeft = 2;
const byte bumperRight = 3;

int setup() {

  lcd.begin(16, 2);
  pinMode(bumperLeft, INPUT_PULLUP);
  pinMode(bumperRight, INPUT_PULLUP);
  pinMode(43, OUTPUT);
  digitalWrite(43, 0);
  initMoteurs();
  sei();
  digitalWrite(43, 1);
  MoteurG(800);
  MoteurD(800);
  lcd.clear();

  if (!IMU.begin()) {
    lcd.print("Erreur init IMU !");
    while (1)
      ;  // Arrêt du programme en cas d'erreur
  }

  lcd.print("Device OK!");
  MoteurGD(800, 0);  //démarrage des moteurs

  pinMode(bumperLeft, INPUT_PULLUP);  // Utiliser une résistance pull-up interne
  pinMode(bumperRight, INPUT_PULLUP);  // Utiliser une résistance pull-up interne
}