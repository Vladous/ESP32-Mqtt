#include "sensors.h"
#include "config.h"
#include <DHTesp.h>                     // https://github.com/adafruit/DHT-sensor-library

//DefaultConfig defaultConfig;
DHTesp dht;

volatile float Teplota = 0.0;
volatile float Vlhkost = 0.0;
volatile int16_t PwrAmp = 0;


void initTempSensor(uint8_t DHTPin) {
  dht.setup(DHTPin, DHTesp::DHT11);                                    // Inicializace senzoru na GPIO4
  Serial.println("Sensors initialized.");
}

int dhtErrorCounter = 0;                                               // Počítadlo chybných měření
void updateTempSensor(DefaultConfig* config) {  
  TempAndHumidity newValues = dht.getTempAndHumidity();                // Načtení dat z DHT senzoru
  if (!isnan(newValues.temperature) && !isnan(newValues.humidity)) {   // Kalibrace
    Teplota = (Teplota + newValues.temperature / config->KalibrT) / 2;
    Vlhkost = (Vlhkost + newValues.humidity / config->KalibrV) / 2;    
    dhtErrorCounter = 0;                                                // Reset počítadla chyb
  } 
  else {
    dhtErrorCounter++;                                                  // Zvýšení počtu chyb
    if (dhtErrorCounter >= 2) {  
      debugMQTT("❌ DHT Chyba: Opakované selhání načtení teploty.");
      dhtErrorCounter = 0;                                              // Reset počítadla po odeslání chyby
    }
  }
}

void updateMeasureAmp(int AmpPin) {
  uint16_t a = analogRead(AmpPin);                                     // Načtení dat z ampermetru (Reading data from the ammeter)
  if (!isnan(a)) {
    PwrAmp = (PwrAmp + a) / 2;                                         // Zkalibrování výstupních hodnot (Calibration of output values)
  }
  else {
    debugMQTT("❌ Chyba: Nepodařilo se načíst hodnotu z ampérmetru.");
  }
}