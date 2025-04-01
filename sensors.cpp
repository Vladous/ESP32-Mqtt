#include "sensors.h"
#include "config.h"
#include <DHTesp.h>                     // https://github.com/adafruit/DHT-sensor-library

//DefaultConfig defaultConfig;
DHTesp dht;

volatile float Teplota = 0.0;
volatile float Vlhkost = 0.0;
volatile int16_t PwrAmp = 0;


void initTempSensor(uint8_t DHTPin) {
  dht.setup(DHTPin, DHTesp::DHT11);                                     // Inicializace senzoru na GPIO4
}

int dhtErrorCounterT = 0;  // Počítadlo chybných měření teploty
int dhtErrorCounterV = 0;  // Počítadlo chybných měření vlhkosti

void updateTempSensor(DefaultConfig* config) {  
  TempAndHumidity newValues = dht.getTempAndHumidity();  // Načtení dat z DHT senzoru
  int pocetChybnychPokusu = 3;
  // Kontrola teploty
  if (!isnan(newValues.temperature)) {  
    Teplota = (Teplota + (newValues.temperature / config->KalibrT)) / 2;
    dhtErrorCounterT = 0;  // Reset počítadla chyb pro teplotu
  } else {  
    dhtErrorCounterT++;  // Zvýšení počtu chyb
    if (dhtErrorCounterT >= pocetChybnychPokusu) {  
      debugMQTT("❌ DHT Chyba: Opakované selhání načtení teploty.");
      dhtErrorCounterT = 0;  
    }
  }

  // Kontrola vlhkosti
  if (!isnan(newValues.humidity)) {  
    Vlhkost = (Vlhkost + (newValues.humidity / config->KalibrV)) / 2;
    dhtErrorCounterV = 0;  // Reset počítadla chyb pro vlhkost
  } else {  
    dhtErrorCounterV++;  // Zvýšení počtu chyb
    if (dhtErrorCounterV >= pocetChybnychPokusu) {  
      debugMQTT("❌ DHT Chyba: Opakované selhání načtení vlhkosti.");
      dhtErrorCounterV = 0;  
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