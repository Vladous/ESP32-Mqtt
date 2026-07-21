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
  static bool tempInitialized = false;
  static bool humInitialized = false;

  const float tempJumpClamp = 2.5f;  // max skok teploty za jeden cyklus (°C)
  const float humJumpClamp = 6.0f;   // max skok vlhkosti za jeden cyklus (%)

  auto adaptiveAlpha = [](float delta, float smallDelta, float mediumDelta) -> float {
    if (delta <= smallDelta) return 0.20f;
    if (delta <= mediumDelta) return 0.35f;
    return 0.55f;
  };

  auto absf = [](float value) -> float {
    return (value < 0.0f) ? -value : value;
  };

  auto clampStep = [](float current, float target, float maxStep) -> float {
    float delta = target - current;
    if (delta > maxStep) return current + maxStep;
    if (delta < -maxStep) return current - maxStep;
    return target;
  };

  // Kontrola teploty
  if (!isnan(newValues.temperature)) {  
    float measuredTemp = newValues.temperature / config->KalibrT;
    if (!tempInitialized) {
      Teplota = measuredTemp;
      tempInitialized = true;
    } else {
      float limitedTarget = clampStep(Teplota, measuredTemp, tempJumpClamp);
      float alpha = adaptiveAlpha(absf(limitedTarget - Teplota), 0.4f, 1.2f);
      Teplota = Teplota + alpha * (limitedTarget - Teplota);
    }
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
    float measuredHum = newValues.humidity / config->KalibrV;
    if (!humInitialized) {
      Vlhkost = measuredHum;
      humInitialized = true;
    } else {
      float limitedTarget = clampStep(Vlhkost, measuredHum, humJumpClamp);
      float alpha = adaptiveAlpha(absf(limitedTarget - Vlhkost), 1.0f, 3.5f);
      Vlhkost = Vlhkost + alpha * (limitedTarget - Vlhkost);
    }
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