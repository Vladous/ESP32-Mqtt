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
const char* VERSION = "4.5";
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
//   "temp": 21.2,                          // float (volitelně), Teplota ze senzoru (°C)
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
//   "DistanceSet": 5                       // integer (volitelně), Vzdálenost pro mávnutí
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
// Výber výstupu teploty °C / °F
// Při ovládání vzdálenosti použít cm i inch
// Doplnit podmínky na všechny dostupná místa
// Zkontrolovat jestli relé je vždy digitalOut


#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "lightControl.h"
#include "deviceControl.h"
#include "mqttHandler.h"
#include "wifiManagerHandler.h"
#include "ota.h"
#include "initSystem.h"
#include "devicePins.h"
#include "stateControl.h"
#include <PubSubClient.h>                                              // https://github.com/knolleary/pubsubclient
#include <WiFi.h>                                                      // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
#include <Ticker.h>                                                    // https://github.com/espressif/arduino-esp32/blob/master/libraries/Ticker
#include <esp_system.h>                                                // Zobrazení příčiny restartu (Display the reason for the restart)

ManualConfig manualConfig;                                             // Použije výchozí hodnoty definované v `ManualConfig`      (config.h)
DefaultConfig defaultConfig;                                           // Použije výchozí hodnoty definované v `DefaultConfig`     (config.h)
WiFiClient espClient;                                                  // Inicializace WiFi   (Wi-Fi initializacion)
PubSubClient client(espClient);                                        // Inicializace MQTT   (MQTT initializacion)
Preferences preferences;                                               // Inicializace EEprom (EEprom initializacion)

const char* PREF_NAMESPACE = "mqtt-app";                               // Jmenný prostor EEPROM (EEPROM namespace)
const char mqtt_ip[] = "192.168.1.1";                                  // Defaultní adresa MQTT serveru (Lze nastavit přes WiFiManager) (Default MQTT server address (Can be set via WiFiManager))
const char* WIFI_HOSTNAME = manualConfig.DeskName.c_str();
char ssid[32];                                                         // Proměnná pro SSID (Variable for SSID)
char password[32];                                                     // Proměnná pro heslo (Variable for password)
// Mqtt proměnné nastavení
char mqtt_server[40];                                                  // Proměnná pro MQTT IP adress (Variable for MQTT IP adress)
char mqtt_port[6] = "1883";                                            // Proměnná pro MQTT port (A variable for the MQTT port)
char mqtt_username[32];                                                // Proměnná pro MQTT User name (Variable for MQTT User name)
char mqtt_password[32];                                                // Proměnná pro MQTT Password (Variable for MQTT Password)
String resetReasonMessage;
int Value = 0;
char SvetloChr[50];
char Pwr[50];
Ticker TimerOdeslat, TimerMereni;                                      // Proměnné přerušení (Interrupts variables)

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
  if (!IsConnected) {
    connectToNetwork();                                                // Opětovné připojení k WiFi (Reconnect to WiFi)
  }  
  client.loop();
  extendedSwitchDispatcher();
  if (PoslatOnOff) {
    Poslat();
    PoslatOnOff = false;
  }  
  int LedBright = LedL;
  if (defaultConfig.NightKontrolLedEnable) {
    LedBright = LedBright /10;
  }
  // Úprava nstavení jasu kontrolek (Adjusting the brightness setting of the lights)
  analogWrite(LedPWR, LedBright / defaultConfig.KalibrKontrolRed);
  if (IsConnected) {
    analogWrite(LedWi, LedBright / defaultConfig.KalibrKontrolBlue);
  }  else {
    analogWrite(LedWi, 0);
  }
  
  if (Zap > 0) {
    analogWrite(PwrSw, LedBright / defaultConfig.KalibrKontrolGreen);
  }  
}

// Kontrola připojení k WiFi a MQTT (WiFi and MQTT connection check)
void reconnect() {
 if (!client.connected() || WiFi.status() != WL_CONNECTED) {
    IsConnected = false;
  }
}

// Připojení k WiFi a MQTT (Connection to WiFi and MQTT)
void connectToNetwork() {
  static unsigned long lastAttemptTime = 0;  // Čas posledního pokusu o připojení
  const unsigned long retryInterval = 2000; // 2 sekundy mezi pokusy

  if (millis() - lastAttemptTime < retryInterval) {
    return;                                                            // Pokud ještě neuplynul interval, neprováděj další pokus
  }
  
  // Vyhledání nejsilnějšího připojení (Finding the strongest connection)
  int n = WiFi.scanNetworks();
  int bestNetworkIndex = -1;
  int bestRSSI = -9999;                                                // Nízká výchozí hodnota pro porovnání (Low default value for comparison)
  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == ssid) {                                        // Hledání sítě s požadovaným SSID (Searching for a network with the desired SSID)
      int rssi = WiFi.RSSI(i);
      if (rssi > bestRSSI) {                                           // Pokud je signál silnější, ulož index a RSSI (If the signal is stronger, save the index and RSSI)
        bestNetworkIndex = i;
        bestRSSI = rssi;
      }
    }
  }
  if (bestNetworkIndex != -1) {                                        // Připojit pokud byla nalezena vhodná síť (Connect if a suitable network is found)
    WiFi.begin(WiFi.SSID(bestNetworkIndex).c_str(), password, 0, WiFi.BSSID(bestNetworkIndex));
    int timeout = 20000;
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
      analogWrite(LedWi, HIGH);
      delay(250);
      analogWrite(LedWi, LOW);
      delay(250);
    }
    if (WiFi.status() == WL_CONNECTED) {                               // Pokud je aktivní WiFi, připojit k MQTT (If WiFi is active, connect to MQTT)
      analogWrite(LedWi, LedL);

      // ✅ Nové: čekej na MQTT broker max 30s
      WiFiClient testClient;
      unsigned long mqttTimeout = millis() + 30000;
      while (!testClient.connect(mqtt_server, atoi(mqtt_port)) && millis() < mqttTimeout) {
        debugMQTT("🕓 Čekám na spuštění MQTT brokeru...");
        delay(1000);
      }
      testClient.stop();

      // Pokud broker stále není dostupný, nevstupuj do připojení
      if (!client.connected()) {
        if (client.connect(manualConfig.DeskName.c_str())) {
          client.subscribe(SvetloChr);
          client.subscribe(manualConfig.LedBrightnessTopic.c_str());
          IsConnected = true;
          Serial.println(WIFI_HOSTNAME);
          Serial.println(manualConfig.DeskName.c_str());
          if (resetReasonMessage != "") {
            debugMQTT("Restart důvod: " + resetReasonMessage);
            resetReasonMessage = "";
          }
        } else {
          IsConnected = false;
        }
      }
    } else {
      IsConnected = false;
    }
  } else {
    IsConnected = false;
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
        case ESP_RST_SW:        resetReasonMessage = " ℹ️  Softwarový reset (restart vyvolaný softwarem)."; break;
        case ESP_RST_PANIC:     resetReasonMessage = "❌ Panický reset (systémová chyba)."; break;
        case ESP_RST_INT_WDT:   resetReasonMessage = "⚠️ Reset způsobený watchdog časovačem přerušení."; break;
        case ESP_RST_TASK_WDT:  resetReasonMessage = "⚠️ Reset způsobený watchdog časovačem úlohy."; break;
        case ESP_RST_WDT:       resetReasonMessage = "⚠️ Reset způsobený watchdog časovačem."; break;
        case ESP_RST_DEEPSLEEP: resetReasonMessage = " ℹ️  Reset po probuzení z hlubokého spánku."; break;
        case ESP_RST_BROWNOUT:  resetReasonMessage = "❌ Reset kvůli poklesu napájecího napětí."; break;
        case ESP_RST_SDIO:      resetReasonMessage = "⚠️ Reset související s rozhraním SDIO (Bezpečné digitální vstupně-výstupní rozhraní)."; break;
        default:                resetReasonMessage = "❓ Neznámý důvod restartu."; break;
  }
}