#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <Arduino.h>
#include "config.h"

// Externí proměnné potřebné pro funkce v deviceControl.cpp
extern const int PwrSw;               // Pin pro LED kontrolku on/off
extern const int PwrRed;              // Pin pro červenou LED
extern const int PwrGreen;            // Pin pro zelenou LED
extern const int PwrBlue;             // Pin pro modrou LED
extern const int Re;                  // Pin pro relé
extern volatile int Zap;              // Proměnná pro kontrolu stavu zařízení
extern volatile bool led1State;       // Stav LED1
extern volatile int led1Brightness;   // Jas LED1
extern volatile bool led2State;       // Stav LED2
extern volatile int led2Brightness;   // Jas LED2
extern volatile bool led3State;       // Stav LED3
extern volatile int led3Brightness;   // Jas LED3
extern volatile bool ledRGBState;     // Stav RGB LED
extern volatile int Red;              // Hodnota červené složky RGB
extern volatile int Green;            // Hodnota zelené složky RGB
extern volatile int Blue;             // Hodnota modré složky RGB
extern volatile bool relayState;      // Stav relé
extern volatile int LedL;             // Jas hlavní LED

// Deklarace funkcí
void updateZap();
void changeState();
void ledKontolaZapnuti();
void aktivaceZarizeni();

#endif // DEVICE_CONTROL_H