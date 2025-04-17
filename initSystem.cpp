#include "initSystem.h"
#include "config.h"
#include "devicePins.h"
#include "stateControl.h"
#include "sensors.h"
#include "wifiManagerHandler.h"
#include "mqttHandler.h"
#include "ota.h"
#include <Ticker.h>
#include <WiFi.h>
#include <Preferences.h>
#include <Arduino.h>

extern Ticker TimerOdeslat, TimerMereni;
extern WiFiClient espClient;
extern PubSubClient client;
extern Preferences preferences;
extern const char* PREF_NAMESPACE;
extern char mqtt_server[40];
extern char mqtt_port[6];
extern const char* WIFI_HOSTNAME;
extern char ssid[32];
extern char password[32];
extern String resetReasonMessage;
extern ManualConfig manualConfig;
extern DefaultConfig defaultConfig;
extern char SvetloChr[50];

// Deklarace funkcí definovaných jinde
extern void pushInterrupt();
extern void connectToNetwork();
//extern void tempAndAmpMeter();
extern void Poslat();

void initPins() {
  analogSetAttenuation(ADC_11db);
  analogReadResolution(12);
  pinMode(LedPWR, OUTPUT);
  pinMode(LedWi, OUTPUT);
  pinMode(PwrSw, OUTPUT);
  pinMode(Re, OUTPUT);
  pinMode(PwrRed, OUTPUT);
  pinMode(PwrGreen, OUTPUT);
  pinMode(PwrBlue, OUTPUT);
  pinMode(ClapSensor, INPUT);
  pinMode(AmpPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  analogWrite(LedPWR, LedL);
}

void initSerial() {
  Serial.begin(9600);
  delay(1500);
}

void initSensors() {
  if (manualConfig.useTemp) {
    initTempSensor(DHTPin);
  }
}

void initInputs() {
  if (manualConfig.useTlac) {
    pinMode(Sw, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Sw), pushInterrupt, FALLING);
  }
}

void initConnection() {
  setupWiFi();
  initOTA(WIFI_HOSTNAME);

  if (strcmp(mqtt_server, "") == 0) {
    strlcpy(mqtt_server, "192.168.1.1", sizeof(mqtt_server));
    Serial.println("Chyba načtení MQTT serveru z EEPROM");
  }
  if (strcmp(mqtt_port, "") == 0) {
    strlcpy(mqtt_port, "1883", sizeof(mqtt_port));
    Serial.println("Chyba načtení MQTT portu z EEPROM");
  }
  Serial.print("MQTT server: "); Serial.println(mqtt_server);
  Serial.print("MQTT port: "); Serial.println(mqtt_port);

  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);

  manualConfig.DeskName.toCharArray(SvetloChr, manualConfig.DeskName.length() + 1);
  connectToNetwork();
}

void initTimers() {
  if (manualConfig.useTemp or manualConfig.useAmpMeter) {
    TimerMereni.attach(defaultConfig.CekejMereni, tempAndAmpMeter);
  }
  TimerOdeslat.attach(defaultConfig.CekejOdeslat, []() { Poslat(); });
}

void reportStatus() {
  reportFirmwareVersion();
  Serial.println("Moje IP adresa je:");
  Serial.println(WiFi.localIP());
  debugMQTT("✅ Zařízení " + String(manualConfig.DeskName.c_str()) + " se úspěšně spustilo.");
}