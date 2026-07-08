#pragma once
#include <config.h>

inline constexpr const char* MODULE_VERSION_SENSORS = "4.9";

extern volatile float Teplota;
extern volatile float Vlhkost;
extern volatile float PwrAmp;
extern volatile bool shouldMeasure;

extern void debugMQTT(const String& message);

// Inicializace senzorů
void initTempSensor(uint8_t DHTPin);

// Aktualizace hodnot globálních proměnných
void updateTempSensor(DefaultConfig* config);
void updateMeasureAmp(int AmpPin);
void tempAndAmpMeter();