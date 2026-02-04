#include "lightControl.h"
#include "config.h"
#include "devicePins.h"

extern ManualConfig manualConfig;
extern DefaultConfig defaultConfig;
extern const int ClapSensor;
extern const int trigPin;
extern const int echoPin;
// Externí proměnná
bool PoslatOnOff = false;
bool firstReading = true;               // Příznak pro první tlesknutí (Flag for first clap)
float average = 0.0;                    // Proměnná pro průměr hlasitosti detekovaného zvuku (A variable for the volume average of the detected sound)
unsigned long lastClapTime = 0;
bool firstClapDetected = false;
const unsigned long doubleClapWindow = 500;
static volatile bool pushInterruptTriggered = false;
static volatile unsigned long lastInterruptTime = 0;

void IRAM_ATTR pushInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 200) {
    pushInterruptTriggered = true;
    lastInterruptTime = now;
  }
}

// Detekce tlesknutí (mock implementace)
void detectClap(int ClapSensor) {
  static unsigned long currentTime;
  static int currentReading;

  currentTime = millis();
  currentReading = analogRead(ClapSensor);

  if (firstReading) {
    average = currentReading;
    firstReading = false;
    return;
  }

  // Průměrování (filtrované)
  if (currentReading <= average + (defaultConfig.ClapThreshold * 2)) {
    average = (currentReading + average) / 2.0;
  }

  // Detekce nadprahového impulsu
  if (currentReading > average + defaultConfig.ClapThreshold) {
    if (!firstClapDetected) {
      firstClapDetected = true;
      lastClapTime = currentTime;
    } else if (currentTime - lastClapTime <= doubleClapWindow) {
      // Detekován dvojtlesk
      zpracujZmenuStavu();
      firstClapDetected = false;
    } else {
      // První tlesk byl moc dávno → resetuj okno
      lastClapTime = currentTime;
    }
  }

  // Timeout okna
  if (firstClapDetected && (currentTime - lastClapTime > doubleClapWindow)) {
    firstClapDetected = false;
  }
}

// Kontrola mávnutí (mock implementace)
void checkWave(int trigPin, int echoPin, DefaultConfig* config) {
  static unsigned long lastMeasurementTime = 0;
  static unsigned long waveLockTime = 0;
  static bool wasDetected = false;
  const unsigned long measurementInterval = 100;
  const unsigned long lockDuration = 1000;
  if (millis() - lastMeasurementTime < measurementInterval) return;
  lastMeasurementTime = millis();
  // Spuštění měření
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 15000);  // 15 ms timeout
  if (duration == 0) return;  // neplatná hodnota
  // Výpočet vzdálenosti v cm
  float distanceCm = (duration * 0.034) / 2.0;
  // Převod podle nastavené jednotky
  float measuredDistance = distanceCm;
  if (config->DistanceUnit == "inch") {
    measuredDistance = distanceCm / 2.54; // 1 inch = 2.54 cm
  }
  if (measuredDistance > 1 && measuredDistance < config->DistanceSet) {
    if (!wasDetected && millis() - waveLockTime > lockDuration) {
      wasDetected = true;
      waveLockTime = millis();
      zpracujZmenuStavu();
    }
  } else {
    wasDetected = false;
  }
}

// 
void extendedSwitchDispatcher() {
  // Tlesknutí
  if (manualConfig.useClap) {
    detectClap(ClapSensor);
  }
  // Mávnutí
  if (manualConfig.useWave) {
    checkWave(trigPin, echoPin, &defaultConfig);
  }
  // Tlačítko
  if (pushInterruptTriggered) {
    pushInterruptTriggered = false;
    zpracujZmenuStavu();
  }
}

// Provede kompletní změnu stavu výstupů a nastaví příznak pro odeslání stavu
void zpracujZmenuStavu() {
  changeState();
  updateZap();
  aktivaceZarizeni();
  ledKontolaZapnuti();
  PoslatOnOff = true;
}