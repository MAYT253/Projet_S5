#include "affichage.h"

void affichage(int arret) {
  if (analogRead(A2) >= SEUIL_HAUT && millis() - prevmillis_Joystick >= TEMPO_JOYSTICK && !(analogRead(A2) >= 750)) {
    prevmillis_Arret = millis();
    menu++;
    prevmillis_Joystick = millis();
  } else if (analogRead(A2) <= SEUIL_BAS && millis() - prevmillis_Joystick >= TEMPO_JOYSTICK && !(analogRead(A2) >= 750)) {
    prevmillis_Arret = millis();
    menu--;
    prevmillis_Joystick = millis();
  }

  if (analogRead(A2) >= 800 || affiche) {
    Serial.println("test");
    switch (menu) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Temps de parcour");
        lcd.setCursor(0, 1);
        lcd.print("TEMPS");
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Energie Consomme");
        lcd.setCursor(0, 1);
        lcd.print("ENERGIE");
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Detection mur");
        lcd.setCursor(0, 1);
        lcd.print("NB FOIS MUR");
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print("Tour Roue");
        lcd.setCursor(0, 1);
        lcd.print("TOUR R/L");
        break;
      default:
        menu = 0;
        break;
    }
    if (!affiche) {
      affiche = 1;
    }
    if (millis() - prevmillis_Arret >= TEMPO_AFFICHAGE_ARRET) {
      affiche = 0;
    }
  } else {
    switch (arret) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Robot leve");
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Arret ELEC");
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("ARRET MEC");
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print("ARRET RB");
        break;
      case 4:
        lcd.setCursor(0, 0);
        lcd.print("ARRET DIST");
        break;
      default:
        arret = 0;
        break;
    }
    prevmillis_Arret = millis();
  }
  if (millis() - prevmillis_Clear >= TEMPO_CLEAR) {
    lcd.clear();
    prevmillis_Clear = millis();
  }
}