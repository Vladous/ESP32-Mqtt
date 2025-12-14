#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "config.h"
#include <Preferences.h>

// Deklarace externích konstant
extern const char* PREF_NAMESPACE;
extern const char* WIFI_HOSTNAME;
extern const char* VERSION;

// Externí proměnné
extern PubSubClient client; // MQTT klient, definovaný jinde
extern WiFiManager wifiManager;
extern char SvetloChr[50];
extern ManualConfig manualConfig;
extern DefaultConfig defaultConfig;
extern volatile int LedL;
extern volatile bool led1State;
extern volatile bool led2State;
extern volatile bool led3State;
extern volatile bool ledRGBState;
extern volatile bool relayState;
extern volatile int led1Brightness;
extern volatile int led2Brightness;
extern volatile int led3Brightness;
extern volatile int Red;
extern volatile int Green;
extern volatile int Blue;
extern volatile float Teplota;
extern volatile float Vlhkost;
extern volatile float PwrAmp;

// Funkce
void callback(char* topic, byte* payload, unsigned int length);
void callbackSettingsSet(JsonDocument& doc);
void callbackSettingsGet();
void callbackDevice(JsonDocument& doc);
void Poslat();
void Poslat(String from);
void sendHelpResponse();
void aktivaceZarizeni();
void reconnect();
void resetCalibreData();
void resetWifiManager();
void restartDevice();
void debugMQTT(const String& message);
void reportFirmwareVersion();
void reportBoardVersion();