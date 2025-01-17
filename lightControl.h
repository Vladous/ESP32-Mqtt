#ifndef LIGHT_CONTROL_H
#define LIGHT_CONTROL_H

#include <Arduino.h>
#include "config.h"

// Deklarace externích funkcí, které se používají v `pushInterrupt`
void changeState();
void updateZap();
void aktivaceZarizeni();
void ledKontolaZapnuti();
void Poslat();
void Poslat(String from);

// Deklarace globální proměnné
extern bool firstReading;
extern float average;
extern unsigned long lastClapTime;
extern bool firstClapDetected;
extern const unsigned long doubleClapWindow;
extern DefaultConfig defaultConfig;
extern bool PoslatOnOff;

// Deklarace přerušovací funkce
void IRAM_ATTR pushInterrupt();
void detectClap(int ClapSensor);
void checkWave(int trigPin, int echoPin, DefaultConfig* config);

#endif