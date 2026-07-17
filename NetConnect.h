#pragma once
#include <Arduino.h>
#include "config.h"          // mqtt_server[], mqtt_port[], ManualConfig

inline constexpr const char* MODULE_VERSION_NET_CONNECT = "5.3";

#include <WiFi.h>

#include <PubSubClient.h>

// === Tvoje existující globály (typy musí sedět) ===
// - používám "extern char []" schválně: neřešíme velikosti [32]/[50]
extern char ssid[];
extern char password[];

extern PubSubClient client;

extern const int LedWi;          // u tebe: extern const int LedWi;
extern volatile int LedL;        // u tebe: extern volatile int LedL;

extern bool IsConnected;

extern char SvetloChr[];         // u tebe: char SvetloChr[50];

extern ManualConfig manualConfig; // u tebe: ManualConfig manualConfig;
extern const char* WIFI_HOSTNAME;

extern String resetReasonMessage;

void debugMQTT(const String& msg);

void connectToNetwork();
void updateNetworkStatusLed(int ledWiBrightness);