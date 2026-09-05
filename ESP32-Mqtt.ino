//  __     ___           _                                 _____                    
//  \ \   / / | __ _  __| | ___  _   _ ___     _ __   ___ |  ___|_ _ _ __ _ __ ___  
//   \ \ / /| |/ _` |/ _` |/ _ \| | | / __|   | '_ \ / _ \| |_ / _` | '__| '_ ` _ \ 
//    \ V / | | (_| | (_| | (_) | |_| \__ \   | | | | (_) |  _| (_| | |  | | | | | |
//     \_/  |_|\__,_|\__,_|\___/ \__,_|___/   |_| |_|\___/|_|  \__,_|_|  |_| |_| |_|
//                                                                                 
// https://github.com/Vladous/ESP32-Mqtt
//
// Deska ESP32 LOLIN S2 MINI WEMOS
//
// MQTT Client IoT
// v1.0 20.03.2020
// v1.1 14.02.2021 Prodloužení času Stisknutí (Extending the press time)
// v1.2 08.04.2021 Prodloužení času Stisknutí , proměné názvu HostName a Svetlo (Extending the press time, HostName and Light variables)
// v2.0 12.06.2021 Oprava pžipojení po výpadku WiFi , Prodloužení času Stisknutí (Fixing the connection after a WiFi failure, Extending the press time)
// v2.1 16.11.2021 Pokus o opravu rozkmitání zpětnou vazbou (An attempt to correct oscillations with feedback)
// v2.3 16.11.2021 Json objekt (Json object)
// v2.3 22.11.2021 Json výběr (Json selection)
// v2.4 14.05.2023 Úprava restartu zažízení při problémech se spojením s WiFi / MQTT podle OpenAI (Modification of device restart for WiFi / MQTT connection problems according to OpenAI)
//                  Přidíní funkce mikrofonu pro ovládání tlesknutím (Added microphone function for clap control)
// v2.5 30.05.2023 Změna na ESP32 (Change to ESP32)
//                  Počáteční nastavení WiFi pomocí WiFimanageru (Initial WiFi setup using WiFimanager)
//                  Úprava velikosti JSON výstupu (Adjusting the size of the JSON output)
// v3.0 21.08.2023 Úprava na ESP32 Deneyap Mini (Modification to ESP32 Deneyap Mini)
//                  Rozdělení světel a relé (Distribution of lights and relays)
// v3.1.28.09.2024 Změna knihovny DHT na ESP32 (Changed DHT library to ESP32)
// v3.2.27.12.2024 Přidání funkce zapnutí mávnutím (Adding activation by waving in front of the distance sensor)
// v3.3.10.01.2025 Rozdělení na kódu na menší části
// v3.4.21.01.2025 Oprava času nočního režimu , dplnění verze do settins get
// v3.5.15.04.2025 Vytvoření release
// v4.0.16.04.2025 Vytvoření OTA pro aktualizace přes WiFi
// v4.1.16.04.2025 Oddělení WiFi Manageru od ino
// v4.2.16.04.2025 Přidání koontroly verze softwaru po startu na samostatný topic "version"
// v4.3.17.04.2025 Ještě drobné rozdělení kódu + bugFix
// v4.4.09.05.2025 Oprava kontroly brokeru. Přihlášení po výpadku elektrického proudu.
// v4.5.09.05.2025 Přidání kontroly připojení k brokeru a WiFi v loopu, pokud není připojeno, pokusí se znovu připojit.
// v4.6.14.05.2025 Přidání watchdog pro restart zařízení při dlouhodobé ztrátě připojení k WiFi nebo MQTT (Adding a watchdog to restart the device in case of prolonged loss of connection to WiFi or MQTT)
// v4.7.14.05.2025 Přidání možnosti nastavení prodlevy mezi pokusy o připojení k WiFi (Adding the ability to set the delay between WiFi connection attempts)
// v4.8.14.05.2025 °C / °F nastavení pro výstup teploty (°C / °F setting for temperature output)
// v4.9.14.06.2025 Přidání možnosti nastavení jednotky pro vzdálenost pro mávnutí (Adding the ability to set the unit for distance for waving)

const char* MAIN_VERSION = "5.0";
//
// ESP32 desky - https://dl.espressif.com/dl/package_esp32_index.json
//
//
//
//
//                          Komponenty (Components)                ESP 32 Wroom mini S2                  Komponenty (Components)
//                                                                ┏━━━━━━━━━━━━━━━━━━━━━┓
//                                                                |                     |             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             ┏━━━━━━┓
//                                                                |-------       -------|             |       Relé  Relay        |━━━━━━━━━━━━━|🔋VCC |
//                                                                |      |       |      |             ┗━━━━━━━━━━━━━|━━━━━━━━━━━━┛             ┗━━━━━━┛
//                                                                |      |       |      |             ┏━━━━━━━━━━━━━|━━━━━━━━━━━━┓             ┏━━━━━━┓
//                                                                | RES  1      40  39  |━━━━━━━━━━━━━|         TIP 120          |━━━━━━━━━━━━━|⏚ GND |
//  ┏━━━━━━┓             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             |                     |             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             ┗━━━━━━┛
//  |⏚ GND |━━━━━━━━━━━━━|Tlač. zapnutí (Pwr button)|━━━━━━━━━━━━━|-------       -------| 
//  +------+             +--------------------------+             |      |       |      |
//  |🔋VCC |━━━━━━━━━━━━━|               VCC        |             |      |       |      |
//  +------+             |      DHT 11 - DATA       |━━━━━━━━━━━━━|  3   2      38  37  |
//  |⏚ GND |━━━━━━━━━━━━━|               GND        |             |                     |
//  +------+             +--------------------------+             |                     |
//  |⏚ GND |━━━━━━━━━━━━━| 🔴 Sv.červená (1) Red   |━━━━━━━━━━━━━|-------       -------| 
//  ┗━━━━━━┛             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             |      |       |      |
//                                                                |  5   4      36  35  |
//  ┏━━━━━━┓             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             |                     |             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             ┏━━━━━━┓
//  |⏚ GND |━━━━━━━━━━━━━| 🟢 Sv.zelená (2)  Green |━━━━━━━━━━━━━|-------       -------|━━━━━━━━━━━━━| Echo       📡      VCC   |━━━━━━━━━━━━━|🔋VCC |
//  +------+             +--------------------------+             |      |       |      |             +         HC-SR4           +             +------+
//  |⏚ GND |━━━━━━━━━━━━━|      Ledka WiFi Led      |━━━━━━━━━━━━━|  7   6      34  33  |━━━━━━━━━━━━━| Trigger            GND   |━━━━━━━━━━━━━|⏚ GND |
//  +------+             +--------------------------+             |                     |             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             ┗━━━━━━┛
//  |⏚ GND |━━━━━━━━━━━━━| 🔵 Sv.modrá (3)   Blue  |━━━━━━━━━━━━━|-------       -------|
//  +------+             +--------------------------+             |      |       |      |
//  |⏚ GND |━━━━━━━━━━━━━|     Ledka on/off Led     |━━━━━━━━━━━━━|  9   8       21  18 |
//  ┗━━━━━━┛             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             |                     |
//                                                                |-------       -------|             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             ┏━━━━━━┓
//  ┏━━━━━━┓             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             |      |       |      |             |                  VCC     |━━━━━━━━━━━━━|🔋VCC |
//  |⏚ GND |━━━━━━━━━━━━━| Ledka napájení Led pwr   |━━━━━━━━━━━━━| 11   10     17  16  |━━━━━━━━━━━━━|  Měření proudu   DATA    |             +------+
//  ┗━━━━━━┛             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             |                     |             | Current measure  GND     |━━━━━━━━━━━━━|⏚ GND |
//                                                                |-------       -------|             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             ┗━━━━━━┛
//  ┏━━━━━━┓             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             |      |       |      |             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//  |⏚ GND |━━━━━━━━━━━━━|  🎤 Mikrofon Microphone  |━━━━━━━━━━━━| 13   12     GND GND |━━━━━━━━━━━━━|          ⏚ GND           |
//  ┗━━━━━━┛             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             |                     |             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛
//                                                                |-------       -------|
//                       ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             |      |       |      |
//                       |        🔋 VCC           |━━━━━━━━━━━━━━| 3V2  14     15 VBUS |
//                       ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             ┗━━━━━━━━━━━━━━━━━━━━━┛
//
//
//
// ****************************************************************************************
// MQTT JSON DOKUMENTACE – přehled struktury pro komunikaci s ESP zařízeními
// Autor: Generováno na základě aktuální implementace mqttHandler.cpp
// ****************************************************************************************
//
// 📤 1. ODESÍLÁNÍ DAT Z ESP (funkce Poslat())
// --------------------------------------------
//
// Odesíláno automaticky nebo po vyžádání ("settings": "get").
// Hodnoty závisí na aktivovaných modulech (manualConfig).
//
// {
//   "devices": [                           // array, Pole zařízení se stavem
//     {
//       "device": "LED1",                  // string, Název zařízení (LED1, LED2, LED3, RGB, RELAY)
//       "state": "on",                     // string, Stav zařízení ("on"/"off")
//       "brightness": 128                  // integer (volitelně), Jas (0–255)
//     },
//     {
//       "device": "RGB",
//       "state": "on",
//       "spectrumRGB": [255, 0, 0]         // integer[3] (volitelně), RGB složky (0–255)
//     },
//     {
//       "device": "RELAY",
//       "state": "off"
//     }
//   ],
//   "temp": 21.2,                          // float (volitelně), Teplota ze senzoru (podle "TempUnit" — °C nebo °F)
//   "hum": 54.6,                           // float (volitelně), Vlhkost ze senzoru (%)
//   "Amp": 512,                            // integer (volitelně), Proud z ampérmetru
//   "signal": -60,                         // integer, Síla WiFi signálu (RSSI v dBm)
//   "bssid": "00:11:22:33:44:55",          // string, MAC adresa přístupového bodu
//   "ip": "192.168.1.10",                  // string, Lokální IP adresa ESP
//   "host": "ESP_HOSTNAME",                // string, Název zařízení
//   "TeplotaChip": 48.2,                   // float, Interní teplota čipu ESP
//   "Verze": "1.0.0",                      // string, Verze firmware
//   "settings": "get"                      // string, Typ zprávy – informativní
// }
//
// 📥 2. PŘÍCHOZÍ DATA (MQTT callback())
// --------------------------------------------
//
// A) Ovládání zařízení
// --------------------
// {
//   "device": "LED1",                      // string, Název zařízení
//   "state": "on",                         // string, Požadovaný stav ("on"/"off")
//   "brightness": 128                      // integer (volitelně), Jas (0–255)
// }
//
// {
//   "device": "RGB",
//   "state": "on",
//   "spectrumRGB": [255, 0, 0]             // integer[3], RGB barvy
// }
//
// B) Nastavení konfigurace ("settings": "set")
// --------------------------------------------
// {
//   "settings": "set",                     // string, Příkaz pro nastavení
//   "ClapThreshold": 900,                  // integer (volitelně), Práh pro tlesknutí
//   "CekejOdeslat": 20.0,                  // float (volitelně), Interval odesílání (s)
//   "CekejMereni": 4.0,                    // float (volitelně), Interval měření (s)
//   "CekejDetectClap": 50,                 // integer (volitelně), Prodleva po tlesknutí (ms)
//   "KalibrT": 1.33,                       // float (volitelně), Kalibrace teploty
//   "KalibrV": 0.70,                       // float (volitelně), Kalibrace vlhkosti
//   "DistanceSet": 5,                      // integer (volitelně), Vzdálenost pro mávnutí
//   "DistanceUnit": "cm",                  // string (volitelně), Jednotka pro DistanceSet ("cm" nebo "inch")
//   "TempUnit": "C",                       // string (volitelně), Výstupní jednotka teploty ("C" nebo "F")
//   "NightKontrolLed": false               // bool (volitelně), Zapnout noční režim kontrolních LED (sníží jas)
//   "NightKontrolLedEnable": false         // bool (volitelně), Automatické povolení nočního režimu podle času
//   "NightStartHour": 21                   // integer (volitelně), Hodina začátku nočního režimu (0-23)
//   "NightStartMin": 0                     // integer (volitelně), Minuta začátku nočního režimu (0-59)
//   "NightEndHour": 6                      // integer (volitelně), Hodina konce nočního režimu (0-23)
//   "NightEndMin": 0                       // integer (volitelně), Minuta konce nočního režimu (0-59)
// }
//
// C) Vyžádání nastavení ("settings": "get")
// --------------------------------------------
// {
//   "settings": "get"                      // string, Žádost o zaslání aktuálních hodnot
// }
//
// Odpověď ESP:
// {
//   "ip": "192.168.1.10",
//   "host": "ESP_HOST",
//   "deviceList": [...],                   // seznam zařízení (např. ["LED1", "RGB"])
//   "CekejOdeslat": 20.0,
//   "CekejMereni": 4.0,
//   "KalibrT": 1.33,
//   "KalibrV": 0.70,
//   "ClapThreshold": 900,
//   "CekejDetectClap": 50,
//   "DistanceSet": 5,
//   "NightKontrolLed": false,
//   "NightKontrolLedEnable": false,
//   "NightStartHour": 21,
//   "NightStartMin": 0,
//   "NightEndHour": 6,
//   "NightEndMin": 0,
//   "TeplotaChip": 48.2,
//   "Verze": "1.0.0"
// }
//
// D) Systémové příkazy
// --------------------------------------------
// { "reset": true }                        // Reset kalibrací a WiFi
// { "restart": true }                      // Restart zařízení
// { "help": true }                         // Vrací JSON s nápovědou možných nastavení
//
// 🪵 3. LOGY (debugMQTT())
// --------------------------------------------
// {
//   "device": "SvetloChr",                 // string, MQTT identifikátor zařízení
//   "message": "❌ Chyba deserializace..." // string, Popis chyby nebo stavu
// }
//
//
// 📦 4. VERZE FIRMWARE (reportFirmwareVersion())
// --------------------------------------------
// Odesíláno jednorázově (např. při spuštění) do topicu "version"
// s parametrem retained = true (zachováno na brokeru)
//
// {
//   "device": "ESP_HOST",                  // string, Název zařízení (hostname)
//   "version": "1.0"                       // string, Verze firmwaru
// }
//
// Používá se pro zobrazení verze na centrálním řídicím serveru
//
// ****************************************************************************************

// ToDo
// Přidat mikrotlačítko reset
// Doplnit podmínky na všechny dostupná místa



#include "config.h"
#include "sensors.h"
#include "lightControl.h"
#include "deviceControl.h"
#include "NetConnect.h"
#include "mqttHandler.h"
#include "wifiManagerHandler.h"
#include "ota.h"
#include "initSystem.h"
#include "devicePins.h"
#include "stateControl.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>                                              // https://github.com/knolleary/pubsubclient
#include <WiFi.h>                                                      // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
#include <Ticker.h>                                                    // https://github.com/espressif/arduino-esp32/blob/master/libraries/Ticker
#include <esp_system.h>                                                // Zobrazení příčiny restartu (Display the reason for the restart)

struct ModuleVersionEntry {
  const char* key;
  const char* version;
};

ManualConfig manualConfig;                                             // Použije výchozí hodnoty definované v `ManualConfig`      (config.h)
DefaultConfig defaultConfig;                                           // Použije výchozí hodnoty definované v `DefaultConfig`     (config.h)
WiFiClient espClient;                                                  // Inicializace WiFi   (Wi-Fi initializacion)
PubSubClient client(espClient);                                        // Inicializace MQTT   (MQTT initializacion)
Preferences preferences;                                               // Inicializace EEprom (EEprom initializacion)

const char* PREF_NAMESPACE = "mqtt-app";                               // Jmenný prostor EEPROM (EEPROM namespace)
const char mqtt_ip[] = "192.168.1.1";                                  // Defaultní adresa MQTT serveru (Lze nastavit přes WiFiManager) (Default MQTT server address (Can be set via WiFiManager))
const char* WIFI_HOSTNAME = manualConfig.DeskName.c_str();
char ssid[32];                                                         // Proměnná pro SSID (Variable for SSID)
char password[64];                                                     // Proměnná pro heslo (Variable for password)
// Mqtt proměnné nastavení
char mqtt_server[40];                                                  // Proměnná pro MQTT IP adress (Variable for MQTT IP adress)
char mqtt_port[6] = "1883";                                            // Proměnná pro MQTT port (A variable for the MQTT port)
char mqtt_username[32];                                                // Proměnná pro MQTT User name (Variable for MQTT User name)
char mqtt_password[64];                                                // Proměnná pro MQTT Password (Variable for MQTT Password)
String resetReasonMessage;
int Value = 0;
char SvetloChr[50];
char Pwr[50];
Ticker TimerOdeslat, TimerMereni;                                      // Proměnné přerušení (Interrupts variables)

void fillModuleVersions(JsonObject versionsObject) {
  static const ModuleVersionEntry MODULE_VERSIONS[] = {
    {"main", MAIN_VERSION},
    {"config", MODULE_VERSION_CONFIG},
    {"sensors", MODULE_VERSION_SENSORS},
    {"lightControl", MODULE_VERSION_LIGHT_CONTROL},
    {"deviceControl", MODULE_VERSION_DEVICE_CONTROL},
    {"netConnect", MODULE_VERSION_NET_CONNECT},
    {"mqttHandler", MODULE_VERSION_MQTT_HANDLER},
    {"wifiManager", MODULE_VERSION_WIFI_MANAGER},
    {"ota", MODULE_VERSION_OTA},
    {"initSystem", MODULE_VERSION_INIT_SYSTEM},
    {"devicePins", MODULE_VERSION_DEVICE_PINS},
    {"stateControl", MODULE_VERSION_STATE_CONTROL}
  };

  for (const auto& entry : MODULE_VERSIONS) {
    versionsObject[entry.key] = entry.version;
  }
}

String buildModuleVersionsJson() {
  StaticJsonDocument<768> payloadDoc;
  payloadDoc["device"] = WIFI_HOSTNAME;
  payloadDoc["version"] = MAIN_VERSION;

  JsonObject versionsObject = payloadDoc.createNestedObject("versions");
  fillModuleVersions(versionsObject);

  String payload;
  serializeJson(payloadDoc, payload);
  return payload;
}

void setup() {  
  initPins();  
  initSerial();  
  printResetReason();                                                  // Vypsání příčiny restartu (Listing the cause of the restart)  
  initInputs();  
  initSensors();  
  loadDefaultConfig();
  initConnection();  
  initTimers();
  reportStatus();
}

void loop() {
  handleOTA();
  OZap = Zap;
  connectToNetwork();
  if (client.connected()) client.loop();
  extendedSwitchDispatcher();
  if (shouldMeasure) {
    shouldMeasure = false;
    tempAndAmpMeter();
  }
  if (PoslatOnOff) {
    Poslat();
    PoslatOnOff = false;
  } 
  int LedBright = LedL;
  if (defaultConfig.NightKontrolLed && defaultConfig.NightKontrolLedEnable) {
    LedBright = LedBright /10;
  }
  // Úprava nstavení jasu kontrolek (Adjusting the brightness setting of the lights)
  analogWrite(LedPWR, LedBright / defaultConfig.KalibrKontrolRed);
  updateNetworkStatusLed(LedBright / defaultConfig.KalibrKontrolBlue);
  
  if (Zap > 0) {
    analogWrite(PwrSw, LedBright / defaultConfig.KalibrKontrolGreen);
  }  
}

void restartDevice() {
    ESP.restart();                                                     // Restart zařízení
}

// Funkce na vypsání výjimek restartu (Function to list restart exceptions)
void printResetReason() {
  esp_reset_reason_t reason = esp_reset_reason();
  switch (reason) {
        case ESP_RST_POWERON:   resetReasonMessage = "✅ Reset po zapnutí napájení."; break;
        case ESP_RST_EXT:       resetReasonMessage = "⚠️ Externí reset (například tlačítkem)."; break;
        case ESP_RST_SW:        resetReasonMessage = "ℹ️  Softwarový reset (restart vyvolaný softwarem)."; break;
        case ESP_RST_PANIC:     resetReasonMessage = "❌ Panický reset (systémová chyba)."; break;
        case ESP_RST_INT_WDT:   resetReasonMessage = "⚠️ Reset způsobený watchdog časovačem přerušení."; break;
        case ESP_RST_TASK_WDT:  resetReasonMessage = "⚠️ Reset způsobený watchdog časovačem úlohy."; break;
        case ESP_RST_WDT:       resetReasonMessage = "⚠️ Reset způsobený watchdog časovačem."; break;
        case ESP_RST_DEEPSLEEP: resetReasonMessage = "ℹ️  Reset po probuzení z hlubokého spánku."; break;
        case ESP_RST_BROWNOUT:  resetReasonMessage = "❌ Reset kvůli poklesu napájecího napětí."; break;
        case ESP_RST_SDIO:      resetReasonMessage = "⚠️ Reset související s rozhraním SDIO (Bezpečné digitální vstupně-výstupní rozhraní)."; break;
        default:                resetReasonMessage = "❓ Neznámý důvod restartu."; break;
  }
}