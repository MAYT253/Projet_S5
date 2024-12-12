#include "Moteur.h"
#include "Error.h"
#include <Encoder.h>

moteur moteur;

static Encoder codeur_L(18, 29);
static Encoder codeur_R(19, 27);

int nb_mur;

ISR(TIMER5_OVF_vect) {

}

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("test");
  moteur.init();
  moteur.MoteurGD(450, 450);
  delay(1000);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
}

void loop() {
  while (Arret(moteur.CO_L,moteur.CO_R) == 0){
    moteur.MoteurGD(430, 430);
    moteur.POLICE();
    detect_mur();
  }
  while(1){
    delay(100);
    affichage(Arret(moteur.CO_L,moteur.CO_R),0,0,nb_mur,codeur_L.read(),codeur_R.read());
    moteur.Stop();
  }
}

void detect_mur(){
  if (UltraF.MeasureInCentimeters() <= 20 || UltraD.MeasureInCentimeters() <= 20 || UltraG.MeasureInCentimeters() <= 20) nb_mur++;
}