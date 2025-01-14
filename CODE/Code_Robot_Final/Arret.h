#ifndef ERROR_H
#define ERROR_H

#include <Arduino.h>
#include "SparkFunLSM6DS3.h"
//#include "moteur.h"

//Valeur de courant
#define MIN_CURRENT -1.239
#define MAX_CURRENT 1.263
#define MAX_CURRENT_MOTOR 1.6
//Bumper
#define bumperLeft 2
#define bumperRight 3
//Capteur Infrarouge
#define infrared 6
//Accelerometre
static LSM6DS3 IMU(I2C_MODE, 0x6A);
//Pas d'arret
static int code_Arret = 0;
static long positionLeft, positionRight = -100;
static long prevmillis;

int Arret(long, long, bool, bool, int, int, int);

#endif