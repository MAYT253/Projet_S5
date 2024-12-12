#include "Error.h"

int Arret(bool OMG, bool OMD) {

  //Arret Electrique
  int CurrentR = 0;
  for (int i = 0; i < 9; i++) CurrentR += analogRead(A4);
  CurrentR = CurrentR / 10;
  if (map(CurrentR, 0, 1023, MIN_CURRENT, MAX_CURRENT) >= MAX_CURRENT_MOTOR) code_Arret = 2;

  int CurrentL = 0;
  for (int i = 0; i < 9; i++) CurrentL += analogRead(A5);
  CurrentL = CurrentL / 10;
  if (map(CurrentL, 0, 1023, MIN_CURRENT, MAX_CURRENT) >= MAX_CURRENT_MOTOR) code_Arret = 2;

  //Arret Mécanique
  if (abs(IMU.readFloatGyroZ()) > 200) code_Arret = 3;

  //Arret Roue(s) Bloquée(s)
  long positionLeft, positionRight = 0;
  long newLeft = knobLeft.read();
  long newRight = knobRight.read();
  if (((newLeft == positionLeft) && OMG) || ((newRight == positionRight) && OMG)) return 4;
  positionLeft = newLeft;
  positionRight = newRight;

  //Arret Distance<5cm
  if (UltraF.MeasureInCentimeters() <= 5 || UltraD.MeasureInCentimeters() <= 5 || UltraG.MeasureInCentimeters() <= 5) code_Arret = 5;

  //Temps écoulé
  long currentTime = millis();
  if ((millis() > 10*1000)) code_Arret = 6;

  //Bumper taper
  bool leftPressed = digitalRead(bumperLeft) == LOW;    // Bumper gauche à l'état bas
  bool rightPressed = digitalRead(bumperRight) == LOW;  // Bumper droit à l'état bas
  if (leftPressed || rightPressed) code_Arret = 7;

  //Fin de course
  if (digitalRead(infrared)) code_Arret = 8;

  //Envoie du code d'arret
  return code_Arret;
}