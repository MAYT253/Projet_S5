#ifndef MOTEUR_H
#define MOTEUR_H

#include <Arduino.h>
#include <rgb_lcd.h>

#define THASH 800
#define STOP_VALUE 400
#define VMAX Thash
// Macros
#define LedToggle digitalWrite(13, !digitalRead(13))
#define WriteL(Vg) OCR5A = Vg  // Vg in [0... 1999]
#define WriteR(Vd) OCR5B = Vd  // VD in [0... 1999]
#define WriteLR(Vg, Vd) WriteL(Vg); WriteR(Vd);
#define StopGD WriteLR(STOP_VALUE, STOP_VALUE);

static rgb_lcd police;

class moteur {
public:
  moteur();

  void init(void);
  void MoteurGD(int SL, int SR);
  void MoteurG(int SL);
  void MoteurD(int SR);
  void Stop(void);
  void POLICE(void);

  bool CO_L;
  bool CO_R;
  bool ARRET;
  int SR;
  int SL;
private:
  long prevmillis;
  bool couleur;
};
#endif
