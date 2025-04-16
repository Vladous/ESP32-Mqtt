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
const char* VERSION = "4.2";
//
// ESP32 desky - https://dl.espressif.com/dl/package_esp32_index.json
//
//
// ESP 32 Wroom mini S2
//
//
//                          Komponenty (Components)                ESP 32 Wroom mini S2                  Komponenty (Components)
//                                                                ┏━━━━━━━━━━━━━━━━━━━━━┓             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             ┏━━━━━━┓
//                                                                |                     |             |                  VCC     |━━━━━━━━━━━━━|🔋VCC |
//                                                                |-------       -------|━━━━━━━━━━━━━|  Měření proudu   DATA    |             +------+
//                                                                |      |       |      |             | Current measure  GND     |━━━━━━━━━━━━━|⏚ GND |
//                                                                |      |       |      |             +--------------------------+             +------+
//                                                                | RES  1      40  39  |━━━━━━━━━━━━━|       Relé  Relay        |━━━━━━━━━━━━━|⏚ GND |
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
//                                                                |-------       -------|
//  ┏━━━━━━┓             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             |      |       |      |
//  |⏚ GND |━━━━━━━━━━━━━| Ledka napájení Led pwr   |━━━━━━━━━━━━━| 11   10     17  16  |
//  ┗━━━━━━┛             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             |                     |             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//                                                                |-------       -------|━━━━━━━━━━━━━|          ⏚ GND           |
//  ┏━━━━━━┓             ┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓             |      |       |      |             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛
//  |⏚ GND |━━━━━━━━━━━━━|  🎤 Mikrofon Microphone  |━━━━━━━━━━━━| 13   12     GND GND |
//  ┗━━━━━━┛             ┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛             |                     |
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


#include "config.h"
#include "sensors.h"
#include "lightControl.h"
#include "deviceControl.h"
#include "mqttHandler.h"
#include "wifiManagerHandler.h"
#include "ota.h"
#include <PubSubClient.h>                                              // https://github.com/knolleary/pubsubclient
#include <WiFi.h>                                                      // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
#include <ArduinoJson.h>                                               // https://github.com/bblanchon/ArduinoJson
#include <Preferences.h>                                               // https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences
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
// Nastavení vstupů ESP
const int Sw = 2;                                                      // Nastavení pinu pro tlačítko (Setting the pin for the button)
const int ClapSensor = 12;                                             // Nastavení pinu pro zvukový senzor - mikrofon (Pin settings for sound sensor - microphone)
const int trigPin = 33;                                                //
const int echoPin = 34;                                                //
uint8_t DHTPin = 3;
const int AmpPin = 40;                                                 // Nastavení pinu pro ampérmetr (Pin settings for ammeter)
// Výstupy kontrolky led
const int LedPWR = 11;                                                 // Nastavení pinu pro led kontrolku napájení (Setting the pin for the power LED)
const int LedWi = 7;                                                   // Nastavení pinu pro led kontrolku připojení WiFI (Setting the pin for the WiFI connection led indicator)
const int PwrSw = 9;                                                   // Nastavení pinu pro led kontrolku on/off (Setting the pin for the on/off led indicator)
// Výstupy ovládání
const int Re = 39;                                                     // Nastavení pinu pro relé (Pin settings for the relay)
const int PwrRed = 4;                                                  // Nastavení pinu pro led (červená) / Světlo 1 (Pin settings for LED (Red)   / Light 1)
const int PwrGreen = 6;                                                // Nastavení pinu pro led (zelená)  / Světlo 2 (Pin settings for LED (Green) / Light 2)
const int PwrBlue = 8;                                                 // Nastavení pinu pro led (modrá)   / Světlo 3 (Pin settings for Led (Blue)  / Light 3)
int Value = 0;
char SvetloChr[50];
volatile int OZap;                                                     // Proměnná pro kontrolu stavu zařízení (A variable to check the status of the device)
volatile int Zap;                                                      // Temp proměnná pro kontrolu stavu zařízení (Temp Variable to check device status)
char Pwr[50];
// Definice zařízení (Device definition)
volatile bool led1State = false;                                       // Led svělto 1 (Led light 1)
volatile int led1Brightness = 255;
volatile bool led2State = false;                                       // Led světlo 2 (Led light 2)
volatile int led2Brightness = 255;
volatile bool led3State = false;                                       // Led světlo 3 (Led light 3)
volatile int led3Brightness = 255;
volatile bool ledRGBState = false;                                     // Led světlo RGB (Led light RGB)
volatile int Red = 254;
volatile int Green = 254;
volatile int Blue = 254;
volatile bool relayState = false;                                      // Relé (Relay)
volatile int LedL = 254;
bool IsConnected = false;
Ticker TimerOdeslat, TimerMereni;                                      // Proměnné přerušení (Interrupts variables)

void setup() {
  analogSetAttenuation(ADC_11db);                                      // Nastavení attenuace pro širší rozsah měření (Attenuation setting for wider measurement range)
  analogReadResolution(12);                                            // Nastavení rozlišení ADC na 12 bitů (Setting the ADC resolution to 12 bits)
  pinMode(LedPWR, OUTPUT);                                             // Výstup aktivace pinu pro kontrolku led (červená) power (Pin activation output for led (red) power)
  pinMode(LedWi, OUTPUT);                                              // Výstup aktivace pinu pro kontrolku led (modrá) připojení k WiFi a MQTT (Pin activation output for led (blue) connection to WiFi and MQTT)
  pinMode(PwrSw, OUTPUT);                                              // Výstup aktivace pinu pro kontrolku led (zelená) zapnutí/vypnutí (Pin activation output for led (green) on/off)
  pinMode(Re, OUTPUT);                                                 // Výstup aktivace pinu pro relé (Pin enable output for relay)
  pinMode(PwrRed, OUTPUT);                                             // Výstup aktivace pinu světla (červená / světlo 1) (Light Pin Enable Output (Red   / Light 1))
  pinMode(PwrGreen, OUTPUT);                                           // Výstup aktivace pinu světla (zelená  / světlo 2) (Light Pin Enable Output (Green / Light 2))
  pinMode(PwrBlue, OUTPUT);                                            // Výstup aktivace pinu světla (modrá   / světlo 3) (Light Pin Enable Output (Blue  / Light 3))
  pinMode(ClapSensor, INPUT);                                          // Vstup aktivace pinu Mikrofon (Microphone pin enable input)
  pinMode(AmpPin, INPUT);                                              // Vstup aktivace pinu Ampermetr (Ammeter pin activation input)
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  analogWrite(LedPWR, LedL);                                           // Zapnutí kontrolky (červená) připojení ke zdroji (Turning on the light (red) connecting to the source)
  Serial.begin(9600);
  delay(1500);
  printResetReason();                                                  // Vypsání příčiny restartu (Listing the cause of the restart)
  if (manualConfig.useTlac) {
    pinMode(Sw, INPUT_PULLUP);                                         // PullUp výstup nastavení pinu pro Tlačítko (PullUp output setting pin for Button)
    attachInterrupt(digitalPinToInterrupt(Sw), pushInterrupt, FALLING);// Aktivace přerušení na spadání hrany pro tlačítko (Activating break on falling edge for button)
  }
  if (manualConfig.useTemp) {
    initTempSensor(DHTPin);
  }
  loadDefaultConfig();

  // Pokud chcete vymazat všechny uložené informace wifiManageru, odkomentujte následující řádku a spusťte jej znovu
  // (If you want to clear all saved wifiManager information, uncomment the following line and run it again)
  // resetWifiManager();

  setupWiFi();
  initOTA(WIFI_HOSTNAME);
  // Pokud se dostanete až sem, jste připojeni k WiFi (If you get this far, you're connected to WiFi)
  Serial.println("Připojeno k WiFi");
  if (strcmp(mqtt_server, "") == 0) {
    strlcpy(mqtt_server, mqtt_ip, sizeof(mqtt_server));                // Pokud se nenačte z EEPROM nastaví default (If it is not read from the EEPROM, the default will be set)
    Serial.println("Chyba načtení MQTT serveru z EEPROM");
  }
  if (strcmp(mqtt_port, "") == 0) {
    strlcpy(mqtt_port, "1883", sizeof(mqtt_port));                     // Pokud se nenačte z EEPROM, nastaví default (If it is not read from the EEPROM, the default will be set)
    Serial.println("Chyba načtení MQTT portu z EEPROM");
  }
  Serial.print("MQTT server: ");
  Serial.println(mqtt_server);
  Serial.print("MQTT port: ");
  Serial.println(mqtt_port);

  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);

  manualConfig.DeskName.toCharArray(SvetloChr, manualConfig.DeskName.length() + 1);
  connectToNetwork();                                                  // Volání nové funkce pro připojení k nastavené WiFi a MQTT (Calling a new function to connect to the set WiFi and MQTT)
  reportFirmwareVersion();
  Serial.println("Moje IP adresa je:");
  Serial.println(WiFi.localIP());
  if (manualConfig.useTemp or manualConfig.useAmpMeter) {              // Pokud je zapnuto měřění teploty, nebo ampermetr aktivuj timer (If the temperature measurement or the ammeter is switched on, activate the timer)
    TimerMereni.attach(defaultConfig.CekejMereni, tempAndAmpMeter);
  }
  TimerOdeslat.attach(defaultConfig.CekejOdeslat, []() { Poslat(); }); // Aktivuj timer pro odesílání dat (Activate timer for sending data)
  debugMQTT("✅ Zařízení " + String(manualConfig.DeskName.c_str()) + " se úspěšně spustilo.");
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

// 
void extendedSwitchDispatcher() {
  if (manualConfig.useClap) {
    detectClap(ClapSensor);
  }
  if (manualConfig.useWave) {
    checkWave(trigPin, echoPin, &defaultConfig);
  }
}

// Načítání hodnot ze senzorů (Reading values ​​from sensors)
void tempAndAmpMeter() {
  if (manualConfig.useTemp) {
    updateTempSensor(&defaultConfig);                                  // Volání funkce seznoru DHT (DHT seznor function call)
  }
  if (manualConfig.useAmpMeter) {
    updateMeasureAmp(AmpPin);                                          // Volání funkce seznoru Ampermetru (CCalling the Ammeter sensor function)
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

      while (!client.connected()) {
        if (client.connect(WIFI_HOSTNAME)) {
          analogWrite(LedWi, LedL);
          client.subscribe(SvetloChr);                                 // Přihlášení topic kanálu zařízení
          client.subscribe(manualConfig.LedBrightnessTopic.c_str());   // Přihlášení univerzal topic kanálu jasu led
          IsConnected = true;
          if (resetReasonMessage != "") {
            debugMQTT("Restart důvod: " + resetReasonMessage);
            resetReasonMessage = "";
          }
          return;                                                      // MQTT připojeno, ukončit funkci (MQTT connected, quit function)
        } else {
          analogWrite(LedWi, LedL);
          delay(250);
          analogWrite(LedWi, 0);
          delay(250);
        }
      }
    } else {
      IsConnected = false;                                             // Připojení síti nenavázáno (Network connection not established)
    }
  } else {
    IsConnected = false;                                               // Připojení síti nenavázáno (Network connection not established)
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
}