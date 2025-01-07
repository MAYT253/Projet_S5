#include "Moteur.h"
#include "affichage.h"
#include "Arret.h"
#include <Encoder.h>

moteur moteur;

int nb_mur;
int code_arret;
long temps_parcours;
float energie;
int compteur = 0;
long temps_start;

//Encoder
Encoder codeur_L(18, 29);
Encoder codeur_R(19, 27);

ISR(TIMER5_OVF_vect) {
  if (compteur++ >=244 && code_arret == 0) mesure_energie();
}

void setup() {
  lcd.begin(16, 2);
  moteur.init();
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void loop() {
  temps_start = millis();
  while (code_arret == 0) {
  code_arret = Arret(moteur.CO_L, moteur.CO_R, codeur_L.read(), codeur_R.read());
    moteur.POLICE();
    detect_mur();
    lcd.setCursor(0, 0);
    lcd.print(code_arret);
    //CODE ICI
    moteur.MoteurGD(600,600);

  }
  while (1) {
    delay(100);
    affichage(code_arret, (millis()-temps_start)/1000, energie, nb_mur, codeur_L.read(), codeur_R.read());
    moteur.Stop();
  }
}

void detect_mur() {
  if (UltraF.MeasureInCentimeters() <= 20 || UltraD.MeasureInCentimeters() <= 20 || UltraG.MeasureInCentimeters() <= 20) nb_mur++;
}

void mesure_energie(){
  float courant;
  for (int i = 0;i < 10; i++) courant = map(analogRead(A5), 0, 1023, MIN_CURRENT, MAX_CURRENT) + map(analogRead(A4), 0, 1023, MIN_CURRENT, MAX_CURRENT);
  courant = courant/10;
	float tension = (3 * analogRead(A0) * 2490 / 1023.00);
  energie = energie + courant*tension;
}
