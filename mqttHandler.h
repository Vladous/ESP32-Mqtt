#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

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

// Externí proměnné
extern PubSubClient client; // MQTT klient, definovaný jinde
extern WiFiManager wifiManager;
extern char SvetloChr[50];
extern ManualConfig manualConfig;
extern DefaultConfig defaultConfig;
extern int LedL;
extern bool led1State;
extern bool led2State;
extern bool led3State;
extern bool ledRGBState;
extern bool relayState;
extern int led1Brightness;
extern int led2Brightness;
extern int led3Brightness;
extern int Red;
extern int Green;
extern int Blue;
extern float Teplota;
extern float Vlhkost;
extern int PwrAmp;

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

#endif // MQTT_HANDLER_H