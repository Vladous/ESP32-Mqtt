#include "sensors.h"
#include "config.h"
#include <DHTesp.h>                     // https://github.com/adafruit/DHT-sensor-library

//DefaultConfig defaultConfig;
DHTesp dht;

float Teplota = 0.0;
float Vlhkost = 0.0;
//int PwrAmp;


void initTempSensor(uint8_t DHTPin) {
    dht.setup(DHTPin, DHTesp::DHT11); // Inicializace senzoru na GPIO4
    Serial.println("Sensors initialized.");
}

void updateTempSensor(DefaultConfig* config) {
    TempAndHumidity newValues = dht.getTempAndHumidity(); // Načtení dat z DHT senzoru
    if (!isnan(newValues.temperature) && !isnan(newValues.humidity)) { // Kalibrace
        Teplota = (Teplota + newValues.temperature / config->KalibrT) / 2;
        Vlhkost = (Vlhkost + newValues.humidity / config->KalibrV) / 2;
    }
}

void updateMeasureAmp(int AmpPin) {
  float a = analogRead(AmpPin);                                        // Načtení dat z ampermetru (Reading data from the ammeter)
  if (!isnan(a)) {
      PwrAmp = (PwrAmp + a) / 2;                                       // Zkalibrování výstupních hodnot (Calibration of output values)
    }
}