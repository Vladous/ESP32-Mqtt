#ifndef SENSORS_H
#define SENSORS_H
#include <config.h>

extern volatile float Teplota;
extern volatile float Vlhkost;
extern volatile int16_t PwrAmp;

extern void debugMQTT(const String& message);

// Inicializace senzorů
void initTempSensor(uint8_t DHTPin);

// Aktualizace hodnot globálních proměnných
void updateTempSensor(DefaultConfig* config);
void updateMeasureAmp(int AmpPin);

#endif