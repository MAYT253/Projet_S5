#ifndef ERROR_H
#define ERROR_H

#include <Arduino.h>
#include "SparkFunLSM6DS3.h"
#include "Ultrasonic.h"
#include <Encoder.h>
//#include "moteur.h"

//Valeur de courant
#define MIN_CURRENT -4.955
#define MAX_CURRENT 5.621
#define MAX_CURRENT_MOTOR 1.6
//Bumper
#define bumperLeft 2
#define bumperRight 3
//Capteur Infrarouge
#define infrared 6
//Accelerometre
static LSM6DS3 IMU(I2C_MODE, 0x6A);
//Ultrason
static Ultrasonic UltraD(8);
static Ultrasonic UltraG(10);
static Ultrasonic UltraF(12);
//Encoder
static Encoder knobLeft(18, 29);
static Encoder knobRight(19, 27);
//Pas d'arret
static int code_Arret = 0;

int Arret(bool, bool);

#endif