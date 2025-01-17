#ifndef SENSORS_H
#define SENSORS_H
#include <config.h>

extern float Teplota;
extern float Vlhkost;
extern int PwrAmp;

// Inicializace senzorů
void initTempSensor(uint8_t DHTPin);

// Aktualizace hodnot globálních proměnných
void updateTempSensor(DefaultConfig* config);
void updateMeasureAmp(int AmpPin);

#endif