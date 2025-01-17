#include "lightControl.h"
#include "config.h"

// Externí proměnná
bool PoslatOnOff = false;
bool firstReading = true;               // Příznak pro první tlesknutí (Flag for first clap)
float average = 0.0;                    // Proměnná pro průměr hlasitosti detekovaného zvuku (A variable for the volume average of the detected sound)
unsigned long lastClapTime = 0;
bool firstClapDetected = false;
const unsigned long doubleClapWindow = 500;

void IRAM_ATTR pushInterrupt() {
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();

    // Debouncing: ignorovat přerušení, pokud k němu došlo příliš brzy po předchozím
    if (interruptTime - lastInterruptTime > 200) {
        changeState();
        updateZap();
        aktivaceZarizeni();
        ledKontolaZapnuti();
        PoslatOnOff = true;
    }

    lastInterruptTime = interruptTime;
}

// Detekce tlesknutí (mock implementace)
void detectClap(int ClapSensor) {
  int currentReading = analogRead(ClapSensor);
  unsigned long currentTime = millis();  
  if (firstReading) {
    average = currentReading;
    firstReading = false;
  } else {
    // Aktualizace průměru pouze pokud aktuální čtení není více než nastavená hodnota "ClapThreshold" nad průměrem
    // (Update the average only if the current reading is not more than the "ClapThreshold" set value above the average)
    if (currentReading <= average + (defaultConfig.ClapThreshold * 2)) {
      average = (currentReading + average) / 2.0;
    }
  }
  if (currentReading > average + defaultConfig.ClapThreshold) {
    if (!firstClapDetected) {
      // První tlesknutí (The first clap)
      firstClapDetected = true;
      lastClapTime = currentTime;
    } else {
      // Potenciální druhé tlesknutí (A potential second clap)
      if (currentTime - lastClapTime <= doubleClapWindow) {
        // Dvojtlesk detekován (Double clap detected)
        changeState();                                                 // Změna stavů zařízení (Changing devices states)
        Serial.println("Detekováno tlesknutí .. .. .. .. ..");
        firstClapDetected = false;                                     // Reset detekce (Reset detection)
        aktivaceZarizeni();                                            // Aktualizace stavu zařízení (Devices status update)
        Poslat();                                                      // Odeslání stavu přes MQTT (Sending status via MQTT)
      } else {
        // Příliš dlouhý interval, považujeme za nový první tlesk (Too long an interval, we consider the new first clap)
        lastClapTime = currentTime;
      }
    }
    // delay(defaultConfig.CekejDetectClap);                                         // Zpoždění pro zabránění falešných detekcí (Delay to prevent false detections)
  } else if (firstClapDetected && (currentTime - lastClapTime > doubleClapWindow)) {
    // Vypršelo časové okno pro druhé tlesknutí (The time window for the second clap has expired)
    firstClapDetected = false;
  }
}

// Kontrola mávnutí (mock implementace)
void checkWave(int trigPin,int echoPin, DefaultConfig* config) {
static bool isInRange = false; // Sledování, zda jsme aktuálně v rozsahu
  static unsigned long lastMeasurementTime = 0;
  const unsigned long measurementInterval = 100; // Interval mezi měřeními v milisekundách
  long duration;
  int distance;

  if (millis() - lastMeasurementTime >= measurementInterval) {
    lastMeasurementTime = millis();

    // Spuštění měření
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    // Ověření rozsahu měření
    if (distance >= 1 && distance <= 400) {
      if (distance < config->DistanceSet) {
        // Pokud jsme v rozsahu, nastavíme isInRange na true
        isInRange = true;
      } else if (isInRange) {
        // Pokud jsme opustili rozsah a dříve jsme v něm byli
        isInRange = false;

        // Akce po opuštění rozsahu
        changeState();                                                     
        aktivaceZarizeni();                                               
        String info = "Wave : " + String(distance) + " cm";
        Poslat(info);
      }
    } else {
      // Pokud je vzdálenost mimo měřitelný rozsah
      isInRange = false;
    }
  }
}
