#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <Arduino.h>
#include <rgb_lcd.h>

#define SEUIL_HAUT 600
#define SEUIL_BAS 400

#define TEMPO_JOYSTICK 1000
#define TEMPO_CLEAR 250
#define TEMPO_AFFICHAGE_ARRET 5000

static rgb_lcd lcd;

static int menu = 0;
static bool affiche;
static long prevmillis_Joystick;
static long prevmillis_Clear;
static long prevmillis_Arret;

void affichage(int, int, float, int, long, long);

#endif