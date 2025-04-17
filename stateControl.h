#pragma once

#include <Arduino.h>

// Stav světel
extern volatile bool led1State;
extern volatile int led1Brightness;
extern volatile bool led2State;
extern volatile int led2Brightness;
extern volatile bool led3State;
extern volatile int led3Brightness;
extern volatile bool ledRGBState;
extern volatile int Red;
extern volatile int Green;
extern volatile int Blue;
extern volatile bool relayState;
extern volatile int LedL;

// Ostatní
extern volatile int OZap;
extern volatile int Zap;
extern bool IsConnected;