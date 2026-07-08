#pragma once
#include <Arduino.h>

inline constexpr const char* MODULE_VERSION_DEVICE_PINS = "4.9";

// Vstupy
extern const int Sw;
extern const int ClapSensor;
extern const int trigPin;
extern const int echoPin;
extern uint8_t DHTPin;
extern const int AmpPin;

// Výstupy kontrolky
extern const int LedPWR;
extern const int LedWi;
extern const int PwrSw;

// Výstupy ovládání
extern const int Re;
extern const int PwrRed;
extern const int PwrGreen;
extern const int PwrBlue;