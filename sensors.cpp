#include <Arduino.h>
#include "sensors.h"
#include "config.h"
#include <DHTesp.h>                     // https://github.com/adafruit/DHT-sensor-library

extern ManualConfig manualConfig;
extern DefaultConfig defaultConfig;
extern int AmpPin;
DHTesp dht;

volatile float Teplota = 0.0;
volatile float Vlhkost = 0.0;
volatile float PwrAmp = 0.0f;
volatile bool shouldMeasure = false;
static float PwrAmpRaw = 0.0f;


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

void updateMeasureAmp(int AmpPin)
{
    // ADC hodnota pro 0 A (naměřený střed)
    const int ZERO = 2400;          // uprav, až si to někdy znovu přeměříš
    // maximální rozumný rozdíl v ADC krocích
    // 2000 kroků ≈ 2000 * 0.01221 A ≈ 24 A
    const int MAX_DIFF = 2000;
    // Převodní konstanta: A na jeden ADC krok (ACS712-30A, 66 mV/A, 3.3 V, 12bit)
    const float K_ADC_TO_A = 0.01221f;
    // 1) načtení syrové hodnoty 0–4095
    int raw = analogRead(AmpPin);
    // 2) rozdíl od středu (může být záporný)
    int diff = raw - ZERO;
    // 3) vyhození úplných nesmyslů (glitchů)
    if (abs(diff) > MAX_DIFF) {
        // Bordel ignorujeme, necháme poslední platnou hodnotu
        return;
    }
    // 4) zajímá nás velikost proudu (kladné číslo)
    float valRaw = (float)abs(diff);    // v ADC krocích
    // 5) jednoduchý low-pass filtr (EMA) na surovou hodnotu
    const float alpha = 0.3f;           // 0–1, vyšší = rychlejší, nižší = hladší
    PwrAmpRaw = (1.0f - alpha) * PwrAmpRaw + alpha * valRaw;
    // 6) přepočet na ampéry
    PwrAmp = PwrAmpRaw * K_ADC_TO_A;
}

void tempAndAmpMeter() {
  if (manualConfig.useTemp) {
    updateTempSensor(&defaultConfig);
  }
  if (manualConfig.useAmpMeter) {
    updateMeasureAmp(AmpPin);
  }
}