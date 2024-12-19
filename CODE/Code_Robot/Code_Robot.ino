#include "Moteur.h"
#include "affichage.h"
#include "Arret.h"
#include <Encoder.h>

moteur moteur;

int code_arret;

//Encoder
static Encoder codeur_L(18, 29);
static Encoder codeur_R(19, 27);

int nb_mur;

ISR(TIMER5_OVF_vect) {
}

void setup() {
  lcd.begin(16, 2);
  moteur.init();
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void loop() {
  while (1){//code_arret == 0) {
  code_arret = Arret(moteur.CO_L, moteur.CO_R, codeur_L.read(), codeur_R.read());
    moteur.POLICE();
    detect_mur();
    //CODE ICI
    moteur.MoteurGD(600,600);
  }
  while (1) {
    delay(100);
    affichage(code_arret, 0, 0, nb_mur, codeur_L.read(), codeur_R.read());
    moteur.Stop();
  }
}

void detect_mur() {
  if (UltraF.MeasureInCentimeters() <= 20 || UltraD.MeasureInCentimeters() <= 20 || UltraG.MeasureInCentimeters() <= 20) nb_mur++;
}