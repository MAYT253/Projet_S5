#include "Moteur.h"

moteur::moteur() {
}

void moteur::init() {
  pinMode(43, OUTPUT);
  digitalWrite(43, 0);
  // MoteurG :OC5A=PIN46-PL3, MoteurD : OC5B=PIN45-PL4
  DDRL = 0x18;  // PL3 et PL4
  DDRB = 0x80;  // PB7 LedToggle
  // COM5B_1:0 = 10   -> clear sur egalite++, set sur egalite--
  // WGM5_3:1 = 1000  -> mode 8 => ICR5 defini le TOP
  TCCR5A = (1 << COM5A1) + (1 << COM5B1);
  TCCR5B = (1 << ICNC5) + (1 << WGM53) + (1 << CS50);  // CS_12:10  = 001  -> prediv par 1
  ICR5 = THASH;                                        // 1999 correspond a f = 4khz
  StopGD;
  // Interrution de débordement du timer
  TIMSK5 = 1 << TOIE5;
  digitalWrite(43, 1);
  sei();
}

void moteur::MoteurGD(int SL, int SR) {
  if (!moteur::ARRET) {
    WriteLR(SL, SR);
    moteur::SL = SL;
    moteur::SR = SR;
    moteur::CO_L = true;
    moteur::CO_R = true;
  } else moteur::Stop();
}

void moteur::MoteurG(int SL) {
  if (!moteur::ARRET) {
    WriteL(SL);
    moteur::SL = SL;
    moteur::CO_L = true;
  } else moteur::Stop();
}

void moteur::MoteurD(int SR) {
  if (!moteur::ARRET) {
    WriteR(SR);
    moteur::SR = SR;
    moteur::CO_R = true;
  } else moteur::Stop();
}

void moteur::Stop() {
  StopGD;
  moteur::CO_L = false;
  moteur::CO_R = false;
}

void moteur::POLICE() {
  if (moteur::prevmillis + 1000 < millis()) {
    moteur::couleur = !moteur::couleur;
    moteur::prevmillis = millis();
  }
  if (couleur) police.setRGB(255, 0, 0);
  else police.setRGB(0, 0, 255);
}
