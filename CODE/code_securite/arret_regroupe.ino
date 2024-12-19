int CurrentStop() {

  CurrentR = 0;
  for (int i = 0; i < 9; i++) {
    CurrentR += analogRead(A4);
  }
  CurrentR = CurrentR / 10;
  if (map(CurrentR, 0, 1023, -4.955, 5.621) >= 0.5) {
    Arret = 1;
  }
  CurrentL = 0;
  for (int i = 0; i < 9; i++) {
    CurrentL += analogRead(A5);
  }
  CurrentL = CurrentL / 10;
  if (map(CurrentL, 0, 1023, -4.955, 5.621) >= 0.5) {
    Arret = 1;
  }
  if (Arret == 1) {
    Serial.println("Arret ELEC");
  }
}

int TimeStop() {

  currentTime = millis();
  if ((millis() > 300000)) {
    MoteurGD(Stop, Stop);
    lcd.print("Temps ecoule");
  }
}

int BumperStop() {

  bool leftPressed = digitalRead(bumperLeft) == LOW;    // Bumper gauche à l'état bas
  bool rightPressed = digitalRead(bumperRight) == LOW;  // Bumper droit à l'état bas

  if (leftPressed || rightPressed) {
    MoteurG(Stop);
    MoteurD(Stop);
    lcd.setCursor(0, 0);
    lcd.print("arret cause");
    lcd.setCursor(1, 0);
    lcd.print("bumpers");
  }
}

int RotStop() {
  gx = IMU.readFloatGyroX();
  gy = IMU.readFloatGyroY();
  gz = IMU.readFloatGyroZ();
  lcd.print(gx);
  lcd.print(gy);
  lcd.print(gz);

  long RangeG = ultrasonicG.MeasureInCentimeters();  //mesure de la distance sur le capteur à ultrasons gauche
  long RangeD = ultrasonicD.MeasureInCentimeters();  //mesure de la distance sur le capteur à ultrasons droit
  long RangeF = ultrasonicF.MeasureInCentimeters();  //mesure de la distance sur le capteur ultrasons avant

  lcd.setCursor(0, 0);

  // Vérification de la vitesse de rotation
  if (abs(gx) > 200 || abs(gy) > 200 || abs(gz) > 200) {
    lcd.print("Vrot trop grande");
    MoteurGD(Stop, Stop);
  }
}

int InfraStop() {
  if (!stopped && digitalRead(infrared)) {  //si le capteur ne détecte rien
    MoteurG(600);
    MoteurD(600);
  } else if (!digitalRead(infrared)) {  //si le capteur détecte quelque chose
    MoteurG(400);
    MoteurD(400);
    // stopped = true; // Marquer l'arrêt pour ne plus redémarrer
  }
}

int Arret5cm() {
  // Lire l'état des bumpers
  bool leftPressed = digitalRead(bumperLeft) == LOW; // Bumper gauche pressé
  bool rightPressed = digitalRead(bumperRight) == LOW; // Bumper droit pressé

  // Si l'un des bumpers est pressé, arrêter les moteurs
 if (leftPressed || rightPressed) {
    MoteurG(400);
    MoteurD(400);
    lcd.setCursor(0, 0);
    lcd.print("arret cause");
    lcd.setCursor(1, 0);
    lcd.print("bumpers");
  }
  else {
     MoteurG(600);
    MoteurD(600);
  }
}