#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <Arduino.h>
#include "config.h"

// Externí proměnné potřebné pro funkce v deviceControl.cpp
extern volatile int Zap;        // Proměnná pro kontrolu stavu zařízení
extern const int PwrSw;         // Pin pro LED kontrolku on/off
extern const int PwrRed;        // Pin pro červenou LED
extern const int PwrGreen;      // Pin pro zelenou LED
extern const int PwrBlue;       // Pin pro modrou LED
extern const int Re;            // Pin pro relé
extern bool led1State;          // Stav LED1
extern int led1Brightness;      // Jas LED1
extern bool led2State;          // Stav LED2
extern int led2Brightness;      // Jas LED2
extern bool led3State;          // Stav LED3
extern int led3Brightness;      // Jas LED3
extern bool ledRGBState;        // Stav RGB LED
extern int Red;                 // Hodnota červené složky RGB
extern int Green;               // Hodnota zelené složky RGB
extern int Blue;                // Hodnota modré složky RGB
extern bool relayState;         // Stav relé
extern int LedL;                // Jas hlavní LED

// Deklarace funkcí
void updateZap();
void changeState();
void ledKontolaZapnuti();
void aktivaceZarizeni();

#endif // DEVICE_CONTROL_H