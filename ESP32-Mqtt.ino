//  __     ___           _                                 _____                    
//  \ \   / / | __ _  __| | ___  _   _ ___     _ __   ___ |  ___|_ _ _ __ _ __ ___  
//   \ \ / /| |/ _` |/ _` |/ _ \| | | / __|   | '_ \ / _ \| |_ / _` | '__| '_ ` _ \ 
//    \ V / | | (_| | (_| | (_) | |_| \__ \   | | | | (_) |  _| (_| | |  | | | | | |
//     \_/  |_|\__,_|\__,_|\___/ \__,_|___/   |_| |_|\___/|_|  \__,_|_|  |_| |_| |_|
//                                                                                 
// https://github.com/Vladous/ESP32-Mqtt
//
// Deska Deneyap Mini (Board Deneyap Mini)
// Deska ESP32 LOLIN S2 MINI WEMOS
//
// MQTT Client IoT
// v1.0 20.03.2020
// v1.1 14.02.2021 Prodloužení času stisknutí (Extending the press time)
// v1.2 08.04.2021 Prodloužení času stisknutí , proměné názvu HostName a Svetlo (Extending the press time, HostName and Light variables)
// v2.0 12.06.2021 Oprava pžipojení po výpadku WiFi , Prodloužení času stisknutí (Fixing the connection after a WiFi failure, Extending the press time)
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
#define VERSION "3.2"
//
// ESP32 desky - https://dl.espressif.com/dl/package_esp32_index.json
//
//
// ESP 32 Wroom mini S2 - Deneyap Mini
// Mikrofon (KY-038) VCC → 3.3V , GNG → GND , A0 → GPIO34
//
//
//                          Komponenty (Components)                ESP 32 Wroom mini S2                  Komponenty (Components)
//                       +--------------------------+             +---------------------+             +--------------------------+             +------+
//                       |                          |             |                     |             |                  VCC     |-------------| VCC  |
//                       |                          |-------------|-------       -------|-------------|  Měření proudu   DATA    |             |      |
//                       |                          |             |      |       |      |             | Current measure  GND     |-------------| GND  |
//                       |                          |             |      |       |      |             +--------------------------+             +------+
//                       |                          |-------------| RES  1      40  39  |-------------|       Relé  Relay        |-------------| GND  |
//  +------+             +--------------------------+             |                     |             +--------------------------+             +------+
//  | GND  |-------------|Tlač. zapnutí (Pwr button)|-------------|-------       -------|             |                          |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//  | VCC  |-------------|               VCC        |             |      |       |      |             |                          |
//  |      |             |      DHT 11 - DATA       |-------------|  3   2      38  37  |             |                          |
//  | GND  |-------------|               GND        |             |                     |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+
//  | GND  |-------------| Sv.červená (1) Light Red |-------------|-------       -------|             |                          |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//                       |                          |             |  5   4      36  35  |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+             +------+
//  | GND  |-------------|Sv.zelená (2) Light Green |-------------|-------       -------|-------------| Echo               VCC   |-------------| VCC  |
//  +------+             +--------------------------+             |      |       |      |             +         HC-SR4           +             |      |
//  | GND  |-------------|      Ledka WiFi Led      |-------------|  7   6      34  33  |-------------| Trigger            GND   |-------------| GND  |
//  +------+             +--------------------------+             |                     |             +--------------------------+             +------+
//  | GND  |-------------| Sv.modrá (3) Light Blue  |-------------|-------       -------|             |                          |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//  | GND  |-------------|     Ledka on/off Led     |-------------|  9   8       21  18 |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+
//                       |                          |             |-------       -------|             |                          |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//  | GND  |-------------| Ledka napájení Led pwr   |-------------| 11   10     17  16  |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+
//                       |                          |             |-------       -------|-------------|           GND            |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//  | GND  |-------------|  Mikrofon Microphone     |-------------| 13   12     GND GND |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+
//                       |                          |             |-------       -------|             |                          |
//                       +--------------------------+             |      |       |      |             +--------------------------+
//                       |           VCC            |-------------| 3V2  14     15 VBUS |             |                          |
//                       +--------------------------+             +---------------------+             +--------------------------+
//
//
//
// JSON payload for sending
// ------------------------
// "devices": [                         // array, Pole objektů s informacemi o zařízeních (An array of objects with device information)
//   {
//     "device": "LED1",                // string, Typ zařízení LED1, LED2, LED3 (Device type)
//     "state": "on",                   // string, Stav zařízení "on" nebo "off" (Device state "on" or "off")
//     "brightness": 128                // integer (volitelně), Jas zařízení, 0 - 255 (Device brightness)
//   },
//   {
//     "device": "RGB",
//     "state": "on",
//     "spectrumRGB": [255, 0, 0]       // integer array (volitelně), Hodnoty RGB, 0 - 255 (RGB values)
//   },
//   {
//     "device": "RELAY",               // string, Typ zařízení (Device type)
//     "state": "on"                    // string, Stav zařízení "on" nebo "off" (Device state "on" or "off")
//   }
// ],
// "temp": 21.2,                        // float (volitelně), Naměřená teplota ve stupních Celsia (Measured temperature in degrees Celsius)
// "hum": 54.6,                         // float (volitelně), Naměřená relativní vlhkost v procentech (Measured relative humidity in percent)
// "Amp": 512,                          // integer (volitelně), Naměřená hodnota z ampérmetru (Measured value from the ammeter)
// "signal": -60,                       // integer, Síla signálu WiFi, RSSI v dBm (WiFi signal strength, RSSI in dBm)
// "bssid": "00:11:22:33:44:55",        // string, MAC adresa WiFi hotspotu (The MAC address of the WiFi hotspot)
//
// "settings": "get",
//  "ip": "192.168.1.1",                // string, IP adresa zařízení (IP address of the device)
//  "host": "ESP_HOST",                 // string, Hostname zařízení (Hostname device)
//  "ClapThreshold": 900,               // integer, Práh pro detekci tlesknutí (Threshold for clap detection)
//  "CekejOdeslat": 20.0,               // float, Časový interval čekání na odeslání (v sekundách) (Time interval waiting for sending (in seconds))
//  "CekejMereni": 4.0,                 // float, Časový interval čekání na měření (v sekundách) (Time interval waiting for measurement (in seconds))
//  "CekejDetectClap": 50,              // integer, Časový interval pro detekci tlesknutí (v milisekundách) (Time interval for clap detection (in milliseconds))
//  "KalibrT": 1.33,                    // float, Kalibrace teplotního senzoru (Temperature sensor calibration)
//  "KalibrV": 0.70                     // float, Kalibrace vlhkostního senzoru (Calibration of the humidity sensor)
//  "DistanceSet": 5                    // int, Kalibrace vzdálenosti mávnutí (Calibration distance of wave detection)
//  "TeplotaChip" : 50                  // float, Teplota čipu ESP ve °C (ESP chip temperature in °C)
//
// JSON callback
// -------------
// "device": "DEVICE_NAME",             // string, Typ zařízení ("LED1", "LED2", "LED3", "RGB", "RELAY") (Device type)
// "state": "on",                       // string, Požadovaný stav zařízení, "on" nebo "off" (Desired device status)
// "brightness": 128,                   // integer (volitelně), Hodnota jasu LED světla, 0 - 255 (Brightness value of the LED light)
// "spectrumRGB": [255, 0, 0],          // integer array (volitelně), Pole tří hodnot RGB, 0 - 255 (An array of three RGB values)
//
// "settings": "set",                   // string, Určuje akci, "set" pro nastavení, "get" pro získání nastavení (Specifies the action, "set" for setting, "get" for getting the setting)
//  "ClapThreshold": 900,               // integer (volitelně), Prah pro detekci tlesknutí (Threshold for clap detection)
//  "CekejOdeslat": 20.0,               // float   (volitelně), Časový interval čekání na odeslání (v sekundách) (Time interval waiting for sending (in seconds))
//  "CekejMereni": 4.0,                 // float   (volitelně), Časový interval čekání na měření (v sekundách) (Time interval waiting for measurement (in seconds))
//  "CekejDetectClap": 50,              // integer (volitelně), Časový interval pro detekci tlesknutí (v milisekundách) (Time interval for clap detection (in milliseconds))
//  "KalibrT": 1.33,                    // float   (volitelně), Kalibrace teplotního senzoru  (Temperature sensor calibration)
//  "KalibrV": 0.70                     // float   (volitelně), Kalibrace vlhkostního senzoru (Calibration of the humidity sensor)
//  "DistanceSet": 5                    // int     (volitelně), Kalibrace vzdálenosti mávnutí (Calibration distance of wave detection)
//
//
// Přidat mikrotlačítko reset
// Výber výstupu teploty °C / °F
// Při ovládání vzdálenosti použít cm i inch
// Doplnit podmínky na všechny dostupná místa
//

#include <PubSubClient.h>               // https://github.com/knolleary/pubsubclient
#include <WiFi.h>                       // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
#include <WiFiManager.h>                // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>                // https://github.com/bblanchon/ArduinoJson
#include <DHTesp.h>                     // https://github.com/adafruit/DHT-sensor-library
#include <Preferences.h>                // https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences
#include <Ticker.h>                     // https://github.com/espressif/arduino-esp32/blob/master/libraries/Ticker
#include <esp_system.h>                 // Zobrazení příčiny restartu (Display the reason for the restart)

#define PREF_NAMESPACE "mqtt-app"       // Jmenný prostor EEPROM (EEPROM namespace)

#define LED_WHITE1    0x01
#define LED_WHITE2    0x02
#define LED_WHITE3    0x04
#define LED_RGB       0x08
#define DEVICE_RELAY  0x10

uint8_t DHTPin = 3;                     // Nastavení datového pinu DHT (Setting the DHT data pin)
DHTesp dht;                             // Inicializace DHT senzoru (DHT sensor initialization)

WiFiClient espClient;                   // Inicializace WiFi   (Wi-Fi initializacion)
PubSubClient client(espClient);         // Inicializace MQTT   (MQTT initializacion)
Preferences preferences;                // Inicializace EEprom (EEprom initializacion)
WiFiManager wifiManager;

const String Svetlo = "Test_Board";     // !! CHANGE !!  Topic název zařízení (Topic device name)
const uint8_t DeviceType = LED_RGB;     // !! CHANGE !!  LED_WHITE1 | LED_WHITE2 | LED_WHITE3 | LED_RGB | DEVICE_RELAY
const bool Tlac = true;                 // !! CHANGE !!  Použití tlačítka (Using the button)
const uint8_t Stisk = LED_RGB;          // !! CHANGE !!  Nastavení tlačítka ( LED_WHITE1 | LED_WHITE2 | LED_WHITE3 | LED_RGB | DEVICE_RELAY ) (Button settings)
const bool Clap = false;                // !! CHANGE !!  Použití mikrofonu (Using the microphone)
const bool Wave = true;                 // !! CHANGE !!  Aktivace funkce zapnutí mávnutím (Activation by waving in front of the distance sensor)
const bool Temp = true;                 // !! CHANGE !!  Použití DHT sezoru měření teploty (Using a DHT sensor to measure temperature)
const bool AmpMeter = false;            // !! CHANGE !!  Použití měření odběru (Using current draw measurement)
const char mqtt_ip[] = "192.168.1.1";   // Defaultní adresa MQTT serveru (Lze nastavit přes WiFiManager) (Default MQTT server address (Can be set via WiFiManager))

const char* WIFI_HOSTNAME = Svetlo.c_str();
char ssid[32];                          // Proměnná pro SSID (Variable for SSID)
char password[32];                      // Proměnná pro heslo (Variable for password)
// Mqtt proměnné nastavení
char mqtt_server[40];                   // Proměnná pro MQTT IP adress (Variable for MQTT IP adress)
char mqtt_port[6] = "1883";             // Proměnná pro MQTT port (A variable for the MQTT port)
char mqtt_username[32];                 // Proměnná pro MQTT User name (Variable for MQTT User name)
char mqtt_password[32];                 // Proměnná pro MQTT Password (Variable for MQTT Password)
// Nastavení vstupů ESP
const int Sw = 2;                       // Nastavení pinu pro tlačítko (Setting the pin for the button)
const int ClapSensor = 12;              // Nastavení pinu pro zvukový senzor - mikrofon (Pin settings for sound sensor - microphone)
const int trigPin = 33;                 //
const int echoPin = 34;                 //
const int AmpPin = 40;                  // Nastavení pinu pro ampérmetr (Pin settings for ammeter)
// Výstupy kontrolky led
const int LedPWR = 11;                  // Nastavení pinu pro led kontrolku napájení (Setting the pin for the power LED)
const int LedWi = 7;                    // Nastavení pinu pro led kontrolku připojení WiFI (Setting the pin for the WiFI connection led indicator)
const int PwrSw = 9;                    // Nastavení pinu pro led kontrolku on/off (Setting the pin for the on/off led indicator)
// Výstupy ovládání
const int Re = 39;                      // Nastavení pinu pro relé (Pin settings for the relay)
const int PwrRed = 4;                   // Nastavení pinu pro led (červená) / Světlo 1 (Pin settings for LED (Red)   / Light 1)
const int PwrGreen = 6;                 // Nastavení pinu pro led (zelená)  / Světlo 2 (Pin settings for LED (Green) / Light 2)
const int PwrBlue = 8;                  // Nastavení pinu pro led (modrá)   / Světlo 3 (Pin settings for Led (Blue)  / Light 3)
// Kalibrační hodnoty
int ClapThreshold = 15;                 // Výchozí nastavení hladiny detekce tlesknutí (Default clap detection level setting)
float CekejOdeslat = 20.0f;             // Výchozí nastavení prodlevy mezi odesláním dat (Default setting for the delay between sending data)
float CekejMereni = 4.0f;               // Výchozí nastavení prodlevy mezi měřením DHT (Default setting for the delay between DHT measurements)
int CekejDetectClap = 50;               // Výchozí nastavení prodlevy mezi detekcí tlesknutí (Default setting for the delay between clap detection)
int DistanceSet = 5;                    // Nastavení vzdálenosti pro sepnutí
int Value = 0;
char SvetloChr[50];
volatile int OZap;                      // Proměnná pro kontrolu stavu zařízení (A variable to check the status of the device)
volatile int Zap;                       // Temp proměnná pro kontrolu stavu zařízení (Temp Variable to check device status)
float Teplota;
float Vlhkost;
char Pwr[50];
// Definice zařízení (Device definition)
bool led1State = false;                 // Led svělto 1 (Led light 1)
int led1Brightness = 255;
bool led2State = false;                 // Led světlo 2 (Led light 2)
int led2Brightness = 255;
bool led3State = false;                 // Led světlo 3 (Led light 3)
int led3Brightness = 255;
bool ledRGBState = false;               // Led světlo RGB (Led light RGB)
int Red = 254;
int Green = 254;
int Blue = 254;
bool relayState = false;                // Relé (Relay)
bool PoslatOnOff = false;
int LedL = 254;
double KalibrT = 1.33;
double KalibrV = 0.70;
int PwrAmp;
bool IsConnected = false;
unsigned long lastClapTime = 0;
bool firstClapDetected = false;
const unsigned long doubleClapWindow = 500;
float average = 0.0;                    // Proměnná pro průměr hlasitosti detekovaného zvuku (A variable for the volume average of the detected sound)
bool firstReading = true;               // Příznak pro první tlesknutí (Flag for first clap)
Ticker TimerOdeslat, TimerMereni;       // Proměnné přerušení (Interrupts variables)

void IRAM_ATTR pushInterrupt() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  // Debouncing: ignorovat přerušení, pokud k němu došlo příliš brzy po předchozím (ignore the interrupt if it occurred too soon after the previous one)
  if (interruptTime - lastInterruptTime > 200) {
    changeState();
    updateZap();
    aktivaceZarizeni();
    ledKontolaZapnuti();
    PoslatOnOff = true;
  }
  lastInterruptTime = interruptTime;
}

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
  Serial.begin(115200);
  delay(1500);
  printResetReason();                                                  // Vypsání příčiny restartu (Listing the cause of the restart)
  if (Tlac) {
    pinMode(Sw, INPUT_PULLUP);                                         // PullUp výstup nastavení pinu pro Tlačítko (PullUp output setting pin for Button)
    attachInterrupt(digitalPinToInterrupt(Sw), pushInterrupt, FALLING);// Aktivace přerušení na spadání hrany pro tlačítko (Activating break on falling edge for button)
  }
  if (Temp) {
    dht.setup(DHTPin, DHTesp::DHT11);                                  // Aktivace senzoru DHT (DHT sensor activation)
  }
  preferences.begin(PREF_NAMESPACE, false);                            // Inicializace Namespace v EEPROM (Initialize namespace in EEPROM)
  // Kontrola nastavených kalibračních hodnot v EEPROM (Checking the set calibration values ​​in the EEPROM)
  if (!preferences.isKey("KalibrT")) {
    preferences.putDouble("KalibrT", 1.33);
  }
  KalibrT = preferences.getDouble("KalibrT", 1.33);

  if (!preferences.isKey("KalibrV")) {
    preferences.putDouble("KalibrV", 0.70);
  }
  KalibrV = preferences.getDouble("KalibrV", 0.70);

  if (!preferences.isKey("ClapThreshold")) {
    preferences.putInt("ClapThreshold", 900);
  }
  ClapThreshold = preferences.getInt("ClapThreshold", 900);

  if (!preferences.isKey("DistanceSet")) {
    preferences.putInt("DistanceSet", 5);
  }
  ClapThreshold = preferences.getInt("DistanceSet", 5);

  if (!preferences.isKey("CekejOdeslat")) {
    preferences.putFloat("CekejOdeslat", 20);
  }
  CekejOdeslat = preferences.getFloat("CekejOdeslat", 20);

  if (!preferences.isKey("CekejMereni")) {
    preferences.putFloat("CekejMereni", 4);
  }
  CekejMereni = preferences.getFloat("CekejMereni", 4);

  if (!preferences.isKey("CekejDetectClap")) {
    preferences.putInt("CekejDetectClap", 50);
  }
  CekejDetectClap = preferences.getInt("CekejDetectClap", 50);

  // Načtení uložených hodnot (Retrieving stored values)
  String savedServer = preferences.getString("mqtt_server", "");
  String savedPort = preferences.getString("mqtt_port", "");

  savedServer.toCharArray(mqtt_server, sizeof(mqtt_server));
  savedPort.toCharArray(mqtt_port, sizeof(mqtt_port));

  // Pokud chcete vymazat všechny uložené informace wifiManageru, odkomentujte následující řádku a spusťte jej znovu
  // (If you want to clear all saved wifiManager information, uncomment the following line and run it again)
  // resetWifiManager();

  // Přidání parametrů do WiFiManager portálu. (Adding parameters to the WiFiManager portal.)
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  //WiFiManagerParameter custom_mqtt_username("user", "mqtt user", mqtt_username, 32);
  //WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 32);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  //wifiManager.addParameter(&custom_mqtt_username);
  //wifiManager.addParameter(&custom_mqtt_password);
  WiFi.setHostname(WIFI_HOSTNAME);
  String apName = String(WIFI_HOSTNAME) + "_AP";
  if (wifiManager.autoConnect(apName.c_str())) {
    // Připojení proběhlo úspěšně, teď uložíme hodnoty do proměnných (The connection was successful, now we will store the values ​​in the variables)
    strcpy(ssid, WiFi.SSID().c_str());
    strcpy(password, WiFi.psk().c_str());
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    //strcpy(mqtt_username, custom_mqtt_username.getValue());
    //strcpy(mqtt_password, custom_mqtt_password.getValue());
    preferences.putString("mqtt_server", mqtt_server);
    preferences.putString("mqtt_port", mqtt_port);
  } else {
    Serial.println("Nepodařilo se připojit - vypršel časový limit");
    ESP.restart();
  }
  preferences.end();
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

  Svetlo.toCharArray(SvetloChr, Svetlo.length() + 1);
  connectToNetwork();                                                  // Volání nové funkce pro připojení k nastavené WiFi a MQTT (Calling a new function to connect to the set WiFi and MQTT)
  Serial.println("Moje IP adresa je:");
  Serial.println(WiFi.localIP());
  if (Temp or AmpMeter) {                                              // Pokud je zapnuto měřění teploty, nebo ampermetr aktivuj timer (If the temperature measurement or the ammeter is switched on, activate the timer)
    TimerMereni.attach(CekejMereni, tempAndAmpMeter);
  }
  TimerOdeslat.attach(CekejOdeslat, Poslat);                           // Aktivuj timer pro odesílání dat (Activate timer for sending data)
}

// Funkce pro detekci tlesknutí (Clap detection function)
void detectClap() {
  int currentReading = analogRead(ClapSensor);
  unsigned long currentTime = millis();  
  if (firstReading) {
    average = currentReading;
    firstReading = false;
  } else {
    // Aktualizace průměru pouze pokud aktuální čtení není více než nastavená hodnota "ClapThreshold" nad průměrem
    // (Update the average only if the current reading is not more than the "ClapThreshold" set value above the average)
    if (currentReading <= average + (ClapThreshold * 2)) {
      average = (currentReading + average) / 2.0;
    }
  }
  if (currentReading > average + ClapThreshold) {
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
    // delay(CekejDetectClap);                                         // Zpoždění pro zabránění falešných detekcí (Delay to prevent false detections)
  } else if (firstClapDetected && (currentTime - lastClapTime > doubleClapWindow)) {
    // Vypršelo časové okno pro druhé tlesknutí (The time window for the second clap has expired)
    firstClapDetected = false;
  }
}

void loop() {
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
  // Úprava nstavení jasu kontrolek (Adjusting the brightness setting of the lights)
  analogWrite(LedPWR, LedL);
  analogWrite(LedWi, LedL);
  if (Zap > 0) {
    analogWrite(PwrSw, LedL);
  }  
}

// 
void extendedSwitchDispatcher() {
  if (Clap) {
    detectClap();
  }
  if (Wave) {
    checkWave();
  }
}

// Update proměnné zap (Update variable zap)
void updateZap() {
  Zap = 0;
  if ((DeviceType & LED_WHITE1) && led1State) {
    Zap |= LED_WHITE1;
  }
  if ((DeviceType & LED_WHITE2) && led2State) {
    Zap |= LED_WHITE2;
  }
  if ((DeviceType & LED_WHITE3) && led3State) {
    Zap |= LED_WHITE3;
  }
  if ((DeviceType & LED_RGB) && ledRGBState) {
    Zap |= LED_RGB;
  }
  if ((DeviceType & DEVICE_RELAY) && relayState) {
    Zap |= DEVICE_RELAY;
  }
}

// Změna stavu zařízení při stisku tlačítka (Changing the state of the devices when the button is pressed)
void changeState() {
  if (Zap == 0) {                                                      // Zapnout zařízení podle proměnné Stisk (Turn on the devices according to the variable Stisk)
      if (Stisk & LED_WHITE1) {
        led1State = true;
      }
      if (Stisk & LED_WHITE2) {
        led2State = true;
      }
      if (Stisk & LED_WHITE3) {
        led3State = true;
      }
      if (Stisk & LED_RGB) {
        ledRGBState = true;
      }
      if (Stisk & DEVICE_RELAY) {
        relayState = true;
      }
    } else {                                                           // Vypnout všechna zařízení (Turn off all devices)
      if (DeviceType & LED_WHITE1) {
        led1State = false;
      }
      if (DeviceType & LED_WHITE2) {
        led2State = false;
      }
      if (DeviceType & LED_WHITE3) {
        led3State = false;
      }
      if (DeviceType & LED_RGB) {
        ledRGBState = false;
      }
      if (DeviceType & DEVICE_RELAY) {
        relayState = false;
      }
    }
}

// Změna aktivace ledky on/off (Enable/disable LED activation)
void ledKontolaZapnuti() {
  if (Zap == 0) {
    analogWrite(PwrSw, 0);
  } else {
    analogWrite(PwrSw, LedL);
  }
}

// Změna stavu zařízení (Changing devices status)
void aktivaceZarizeni() {
  // LED1
  if (DeviceType & LED_WHITE1) {
    if (led1State) {
      analogWrite(PwrRed, led1Brightness);                             // Zapnout LED1 (Turn on  LED1)
    } else {
      analogWrite(PwrRed, 0);                                          // Vypnout LED1 (Turn off LED1)
    }
  }
  // LED2
  if (DeviceType & LED_WHITE2) {
    if (led2State) {
      analogWrite(PwrGreen, led2Brightness);                           // Zapnout LED2 (Turn on  LED2)
    } else {
      analogWrite(PwrGreen, 0);                                        // Vypnout LED2 (Turn off LED2)
    }
  }
  // LED3
  if (DeviceType & LED_WHITE3) {
    if (led3State) {
      analogWrite(PwrBlue, led3Brightness);                            // Zapnout LED3 (Turn on  LED3)
    } else {
      analogWrite(PwrBlue, 0);                                         // Vypnout LED3 (Turn off LED3)
    }
  }
  // RGB LED
  if (DeviceType & LED_RGB) {
    if (ledRGBState) {
      analogWrite(PwrRed, Red);                                        // Zapnout červenou (Turn on red)
      analogWrite(PwrGreen, Green);                                    // Zapnout zelenou  (Turn on green)
      analogWrite(PwrBlue, Blue);                                      // Zapnout modrou   (Turn on blue)
    } else {
      analogWrite(PwrRed, 0);                                          // Vypnout červenou (Turn off red)
      analogWrite(PwrGreen, 0);                                        // Vypnout zelenou  (Turn off green)
      analogWrite(PwrBlue, 0);                                         // Vypnout modrou   (Turn off blue)
    }
  }
  if (DeviceType & DEVICE_RELAY) {
    if (relayState) {
      digitalWrite(Re, HIGH);                                          // Zapnout relé (Turn on  relay)
    } else {
      digitalWrite(Re, LOW);                                           // Vypnout relé (Turn off relay)
    }
  }
  updateZap();
  ledKontolaZapnuti();
}

void checkWave() {
  long duration;
  int distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < DistanceSet) {
    changeState();                                                     // Změna stavů zařízení (Changing devices states)
    aktivaceZarizeni();                                                // Aktualizace stavu zařízení (Devices status update)
    Poslat();                                                          // Odeslání stavu přes MQTT (Sending status via MQTT)
  }
}

// Načítání hodnot ze senzorů (Reading values ​​from sensors)
void tempAndAmpMeter() {
  if (Temp) {
    senzorTemp();                                                      // Volání funkce seznoru DHT (DHT seznor function call)
  }
  if (AmpMeter) {
    measureAmp();                                                      // Volání funkce seznoru Ampermetru (CCalling the Ammeter sensor function)
  }
}

// Přijetí zprávy z MQTT serveru (Receive message from MQTT server)
void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  DynamicJsonDocument doc(512);                                        // Deklarace proměnné doc pro Json (Declaration of doc variable for Json)
  DeserializationError error = deserializeJson(doc, payload, length);  // Deserializace Json zprávy (Deserialize a Json message)
  // Ukončení funkce pri chybě deserializace (Function termination on deserialization error)
  if (error) {
    Serial.print("Deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Zpracování podle topicu
  if (topicStr == "Led_brightness") {
    if (doc["brightArd"] != nullptr) {
      LedL = doc["brightArd"];                                         // Nastavení hodnoty jasu kontrolek (Setting the value of the brightness of the lights)
    }
  } else {
    // Roztřídění přijatých zpráv (settings-set, settings-get, device) (Sorting received messages)
    if (doc.containsKey("settings")) {
      const char* settingAction = doc["settings"];                       // Načtení z příznaku settings (Loaded from the settings)
      if (strcmp(settingAction, "set") == 0) {
        callbackSettingsSet(doc);                                        // Pokud je settings set, zavolej funkci pro nastavení prarametrů (If settings is set, call the parameter setting function)
      } else if (strcmp(settingAction, "get") == 0) {
        callbackSettingsGet();                                           // Pokud je settings set, zavolej funkci pro odeslání nastavení parametrů (If settings is set, call the function to send the parameter settings)
      }
    } else if(doc.containsKey("reset")) {
      resetCalibreData();
      resetWifiManager();
      restartDevice();    
    } else if(doc.containsKey("restart")) {
      restartDevice();
    } else if (doc.containsKey("help")) {
        sendHelpResponse(); // Volání funkce pro odeslání seznamu příkazů
    } else {
      callbackDevice(doc);                                               // Pokud není příznak settings, zavolej funkci ovládání zařízení (If there is no settings flag, call the device control function)
    }
  }
}

// Funkce pro nastavení parametrů (Function for setting parameters)
void callbackSettingsSet(JsonDocument& doc) {
  preferences.begin(PREF_NAMESPACE, false);                            // Inicializace Namespace v EEPROM (Initialize namespace in EEPROM)
  if (doc.containsKey("ClapThreshold")) {
    ClapThreshold = doc["ClapThreshold"].as<int>();                    // Nastavení práhu tlesknutí (Setting the clap threshold)
    preferences.putInt("ClapThreshold", ClapThreshold);                // Uložení práhu tlesknutí do EEPROM (Saving the clap threshold to EEPROM)
  }
  if (doc.containsKey("CekejOdeslat")) {
    CekejOdeslat = doc["CekejOdeslat"].as<float>();                    // Nastavení frekvence odesílání dat (Setting the data sending frequency)
    preferences.putInt("CekejOdeslat", CekejOdeslat);                  // Uložení frekvence odesílání dat do EEPROM (Saving data sending frequency to EEPROM)
  }
  if (doc.containsKey("CekejMereni")) {
    CekejMereni = doc["CekejMereni"].as<float>();                      // Nastavení frekvence čtení čidel (Setting the sensors reading frequency)
    preferences.putInt("CekejMereni", CekejMereni);                    // Uložení frekvence čtení čidel do EEPROM (Saving the reading frequency of the sensors in the EEPROM)
  }
  if (doc.containsKey("CekejDetectClap")) {
    CekejDetectClap = doc["CekejDetectClap"].as<int>();                // Prodleva mezi kontrolou tlesknutí (Delay between clap checks)
    preferences.putInt("CekejDetectClap", CekejDetectClap);            // Uložení prodlevy mezi kontrolou tlesknutí (Saving the delay between clap checks)
  }
  if (doc.containsKey("DistanceSet")) {
    CekejDetectClap = doc["DistanceSet"].as<int>();                    // Vzdálenost detekce mávnutí (Distance wave checks)
    preferences.putInt("DistanceSet", DistanceSet);                    // Uložení vzdálenosti detekce mávnutí (Saving the distance wave checks)
  }
  if (doc.containsKey("KalibrT")) {
    KalibrT = doc["KalibrT"].as<float>();                              // Kalibrační konstanta pro měření teploty (Calibration constant for temperature measurement)
    preferences.putFloat("KalibrT", KalibrT);                          // Uložení kalibrační konstanty pro měření teploty do EEPROM (Saving the calibration constant for temperature measurement in the EEPROM)
  }
  if (doc.containsKey("KalibrV")) {
    KalibrV = doc["KalibrV"].as<float>();                              // Kalibrační konstanta pro měření vlhkosti (Calibration constant for humidity measurement)
    preferences.putFloat("KalibrV", KalibrV);                          // Uložení kalibrační konstanty pro měření vlhkosti (Saving the calibration constant for humidity measurement)
  }
  preferences.end();                                                   // Ukončení Namespace v EEPROM (End namespace in EEPROM)
}

// Funkce pro odeslání nastavených parametrů (Function for sending set parameters)
void callbackSettingsGet() {
  DynamicJsonDocument responseDoc(512);                                // Deklarace proměnné responseDoc pro Json (Declaration of responseDoc variable for Json)
  responseDoc["ip"] = WiFi.localIP().toString();                       // Přidání IP adresy zařízení do výstupu k odeslání (Adding the IP address of the device to the output to send)
  responseDoc["host"] = WIFI_HOSTNAME;                                 // Přidání názvu host do výstupu k odeslání (Adding the host name to the output to send)
  responseDoc["CekejOdeslat"] = CekejOdeslat;                          // Přidání nastavené prodlevy odesílání do výstupu k odeslání (Adding a set send delay to the output to send)
  if (Temp || AmpMeter) {
    responseDoc["CekejMereni"] = CekejMereni;                          // Přidání nastavené prodlevy měření senzorů do výstupu k odeslání (Adding the set sensors measurement delay to the output to be sent)
    if (Temp) {
      responseDoc["KalibrT"] = KalibrT;                                // Přidání nastavené kalibrační hodnoty teploměru do výstupu k odeslání (Adding the set thermometer calibration value to the output to be sent)
      responseDoc["KalibrV"] = KalibrV;                                // Přidání nastavené kalibrační hodnoty vlhkoměru do výstupu k odeslání (Adding the set hygrometer calibration value to the output to be sent)
    }
  }  
  if (Clap) {
    responseDoc["ClapThreshold"] = ClapThreshold;                      // Přidání nastavené hodnoty prahu tlesknutí do výstupu k odeslání (Adding a set clap threshold value to the output to send)
    responseDoc["CekejDetectClap"] = CekejDetectClap;                  // Přidání nastavené hodnoty prodlevy kontroly tlesknutí do výstupu k odeslání (Adding a set clap check delay value to the output to send)
  }
  if (Wave) {
    responseDoc["DistanceSet"] = DistanceSet;                          // Přidání nastavené hodnoty vzdálenosti detekce mávnutí do výstupu k odeslání (Adding a set wave check delay value to the output to send)
  }
  float teplotaCipu = temperatureRead();
  responseDoc["TeplotaChip"] = teplotaCipu;                            // Přidání aktuální teploty ESP do výstupu k odeslání (Adding the current ESP temperature to the output to send)
  responseDoc["Verze"] = VERSION;                                      // Přidání aktuální verze softwaru do výstupu k odeslání (Adding the current version of the software to the output to send)
  char responseOut[512];
  serializeJson(responseDoc, responseOut);                             // Serializace dat do Json (Serializing data to Json)
  client.publish(SvetloChr, responseOut);                              // Odeslání dat přes MQTT (Sending data via MQTT)
}

// Funkce pro nastavení zařízení z MQTT (Function to set up device from MQTT)
void callbackDevice(JsonDocument& doc) { 
  if (doc.containsKey("device") && doc.containsKey("state")) {
    String deviceName = doc["device"].as<String>();                    // Vybrané zařízení (Selected device)
    String state = doc["state"].as<String>();                          // Nastavený parametr (Set parameter)
    bool isOn = (state == "on");
    if ((deviceName == "LED1") && (DeviceType & LED_WHITE1)) {
      led1State = isOn;
      if (doc.containsKey("brightness")) {
        led1Brightness = doc["brightness"].as<int>();                  // Nastavení hodnoty jasu na LED_WHITE1 (Setting the brightness value to LED_WHITE1)
      }
    } else if ((deviceName == "LED2") && (DeviceType & LED_WHITE2)) {
      led2State = isOn;
      if (doc.containsKey("brightness")) {
        led2Brightness = doc["brightness"].as<int>();                  // Nastavení hodnoty jasu na LED_WHITE2 (Setting the brightness value to LED_WHITE2)
      }
    } else if ((deviceName == "LED3") && (DeviceType & LED_WHITE3)) {
      led3State = isOn;
      if (doc.containsKey("brightness")) {
        led3Brightness = doc["brightness"].as<int>();                  // Nastavení hodnoty jasu na LED_WHITE3 (Setting the brightness value to LED_WHITE3)
      }
    } else if ((deviceName == "RGB") && (DeviceType & LED_RGB)) {
      ledRGBState = isOn;
      if (doc.containsKey("spectrumRGB")) {
        JsonArray rgbValues = doc["spectrumRGB"].as<JsonArray>();
        if (rgbValues.size() == 3) {
          Red = rgbValues[0];                                          // Nastavení hodnoty jasu červené (Setting the brightness value red)
          Green = rgbValues[1];                                        // Nastavení hodnoty jasu zelené  (Setting the brightness value green)
          Blue = rgbValues[2];                                         // Nastavení hodnoty jasu modré   (Setting the brightness value blue)
        }
      }
    } else if ((deviceName == "RELAY") && (DeviceType & DEVICE_RELAY)) {
      relayState = isOn;                                               // Nastavení stavu relé (Relay status setting)
    }
    // Aktualizujeme světla podle nového stavu
    aktivaceZarizeni();                                                // Volání funkce změny stavu zařízení (Calling the device state change function)
  }
}

// Kontrola připojení k WiFi a MQTT (WiFi and MQTT connection check)
void reconnect() {
 if (!client.connected() || WiFi.status() != WL_CONNECTED) {
    IsConnected = false;
  }
}

// Odeslání dat na MQTT server (Sending data to MQTT server)
void Poslat() {
  reconnect();                                                         // Volání funkce pro kontrolu připojení k WiFi a MQTT (WiFi and MQTT connection check)
  DynamicJsonDocument doc(512);                                        // Deklarace proměnné doc pro Json (Declaration of doc variable for Json)
  JsonArray devices = doc.createNestedArray("devices");                // Vytvoření pole devices k odeslání (Creating the devices array to send)
  // Kontrola a přidání LED1 (Checking and adding LED1)
  if (DeviceType & LED_WHITE1) {
    JsonObject led1 = devices.createNestedObject();                    // Vytvoření Json objektu pro LED_WHITE1 (Creating a Json object for LED_WHITE1)
    led1["device"] = "LED1";                                           // Přidání device LED1 do výstupu k odeslání (Adding device LED1 to output to send)
    led1["state"] = led1State ? "on" : "off";                          // Přidání state LED1 do výstupu k odeslání (Adding state LED1 to output to send)
    led1["brightness"] = led1Brightness;                               // Přidání jasu LED1 do výstupu k odeslání (Adding brightness of LED1 to the output to send)
  }
  // Kontrola a přidání LED2 (Checking and adding LED2)
  if (DeviceType & LED_WHITE2) {
    JsonObject led2 = devices.createNestedObject();                    // Vytvoření Json objektu pro LED_WHITE2 (Creating a Json object for LED_WHITE2)
    led2["device"] = "LED2";                                           // Přidání device LED2 do výstupu k odeslání (Adding device LED2 to output to send)
    led2["state"] = led2State ? "on" : "off";                          // Přidání state LED2 do výstupu k odeslání (Adding state LED2 to output to send)
    led2["brightness"] = led2Brightness;
  }
  // Kontrola a přidání LED3 (Checking and adding LED3)
  if (DeviceType & LED_WHITE3) {
    JsonObject led3 = devices.createNestedObject();                    // Vytvoření Json objektu pro LED_WHITE3 (Creating a Json object for LED_WHITE3)
    led3["device"] = "LED3";                                           // Přidání device LED3 do výstupu k odeslání (Adding device LED3 to output to send)
    led3["state"] = led3State ? "on" : "off";                          // Přidání state LED3 do výstupu k odeslání (Adding state LED3 to output to send)
    led3["brightness"] = led3Brightness;
  }
  // Kontrola a přidání RGB LED (Checking and adding RGB LED)
  if (DeviceType & LED_RGB) {
    JsonObject ledRGB = devices.createNestedObject();                  // Vytvoření Json objektu pro RGB LED (Creating a Json object for RGB LED)
    ledRGB["device"] = "RGB";                                          // Přidání device RGB LED do výstupu k odeslání (Adding device RGB LED to output to send)
    ledRGB["state"] = ledRGBState ? "on" : "off";                      // Přidání state RGB LED do výstupu k odeslání (Adding state RGB LED to output to send)
    JsonArray rgbValues = ledRGB.createNestedArray("spectrumRGB");     // Vytvoření Json objektu pro spectrumRGB (Creating a Json object for spectrumRGB)
    rgbValues.add(Red);                                                // Přidání jasu červené do výstupu k odeslání (Adding brightness red to the output to send)
    rgbValues.add(Green);                                              // Přidání jasu zelené do výstupu k odeslání (Adding brightness green to the output to send)
    rgbValues.add(Blue);                                               // Přidání jasu modré do výstupu k odeslání (Adding brightness blue to the output to send)
  }
  // Přidání relé
  if (DeviceType & DEVICE_RELAY) {
    JsonObject relay = devices.createNestedObject();                   // Vytvoření Json objektu pro relé (Creating a Json object for the relay)
    relay["device"] = "RELAY";                                         // Přidání device relé do výstupu k odeslání (Adding device relay to output to send)
    relay["state"] = relayState ? "on" : "off";                        // Přidání state relé do výstupu k odeslání (Adding state relay to output to send)
  }
  // Pokud pole "devices" je prázdné, odstraníme jej z JSON zprávy (If the devices field is empty, we remove it from the JSON message)
  if (devices.size() == 0) {
    doc.remove("devices");
  }
  if (Temp) {
    doc["temp"] = Teplota;                                             // Přidání naměřené teploty do výstupu k odeslání (Adding the measured temperature to the output for sending)
    doc["hum"] = Vlhkost;                                              // Přidání naměřené vlhkosti do výstupu k odeslání (Adding the measured humidity to the output for sending)
  }
  if (AmpMeter) {
    doc["Amp"] = PwrAmp;                                               // Přidání naměřeného odběru proudu do výstupu k odeslání (Adding the measured current draw to the output to send)
  }  
  // Základní informace o zařízení
  doc["signal"] = WiFi.RSSI();                                         // Přidání aktuální síly signálu do výstupu k odeslání (Adding the current signal strength to the output to send)
  byte bssid[6];                                                       // Načtení MAC adresy WiFi hotspotu
  WiFi.BSSID(bssid);                                                   // (Retrieve the MAC address of the WiFi hotspot)
  char bssidStr[18];
  sprintf(bssidStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
  doc["bssid"] = bssidStr;                                             // Přidání aktuální MAC adresy WiFi hotspotu do výstupu k odeslání (Adding the current MAC address of the WiFi hotspot to the output to send) 
  char out[256];
  serializeJson(doc, out);                                             // Serializace dat do Json (Serializing data to Json)
  client.publish(SvetloChr, out);                                      // Odeslání dat přes MQTT (Sending data via MQTT)
}

// Funkce měření teploty a vlhkosti (Temperature and humidity measurement function)
void senzorTemp() {
  TempAndHumidity newValues = dht.getTempAndHumidity();                // Načtení dat z DHT senzoru (Loading data from the DHT sensor)
  if (!isnan(newValues.temperature) && !isnan(newValues.humidity)) {   // Zkalibrování výstupních hodnot (Calibration of output values)
      Teplota = (Teplota + newValues.temperature / KalibrT) / 2;
      Vlhkost = (Vlhkost + newValues.humidity / KalibrV) / 2;
    }
}

// Funkce měření odběru proudu (Current consumption measurement function)
void measureAmp() {
  float a = analogRead(AmpPin);                                        // Načtení dat z ampermetru (Reading data from the ammeter)
  if (!isnan(a)) {
      PwrAmp = (PwrAmp + a) / 2;                                       // Zkalibrování výstupních hodnot (Calibration of output values)
    }
}

// Připojení k WiFi a MQTT (Connection to WiFi and MQTT)
void connectToNetwork() {
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
    WiFi.begin(WiFi.SSID(bestNetworkIndex).c_str(), password);
    int timeout = 10000;
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
          client.subscribe("Led_brightness");                          // Přihlášení univerzal topic kanálu jasu led
          IsConnected = true;
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

void sendHelpResponse() {
  DynamicJsonDocument helpDoc(1024);                                   // Zvýšení velikosti bufferu pro více dat
  // Přidání seznamu příkazů
  helpDoc["commands"] = JsonArray();
  helpDoc["commands"].add("reset");                                    // Reset zařízení
  helpDoc["commands"].add("restart");                                  // Restart zařízení
  helpDoc["commands"].add("settings-set");                             // Nastavení parametrů
  helpDoc["commands"].add("settings-get");                             // Získání aktuálních parametrů
  helpDoc["commands"].add("help");                                     // Získání nápovědy

  // Detailní možnosti pro "settings-set"
  helpDoc["settings-set-options"] = JsonObject();                      // Objekt pro možnosti nastavení
  helpDoc["settings-set-options"]["CekejOdeslat"] = "float - časový interval mezi odesláním dat (v sekundách)";
  if (Temp || AmpMeter) {
    helpDoc["settings-set-options"]["CekejMereni"] = "float - nastavené prodlevy měření senzorů (v sekundách)";
    if (Temp) {
      helpDoc["settings-set-options"]["KalibrT"] = "double - nastavené kalibrační hodnoty teploměru";
      helpDoc["settings-set-options"]["KalibrV"] = "double - nastavené kalibrační hodnoty vlhkoměru";
    }
  }  
  if (Clap) {
    helpDoc["settings-set-options"]["ClapThreshold"] = "int - nastavené hodnoty prahu tlesknutí";
    helpDoc["settings-set-options"]["CekejDetectClap"] = "int - nastavené hodnoty prodlevy kontroly tlesknutí";
  }
  if (Wave) {
    helpDoc["settings-set-options"]["DistanceSet"] = "int - nastavené hodnoty vzdálenosti kontroly mávnutí";
  }
  // Serializace a odeslání zprávy
  char helpOut[1024];
  serializeJson(helpDoc, helpOut);
  client.publish(SvetloChr, helpOut);                                  // Odeslání zprávy na MQTT topic
}

// Funkce na vymazání uložených dat z WiFiManageru (Function to delete saved data from WiFiManager)
void resetWifiManager() {
  wifiManager.resetSettings();
}

void restartDevice() {
    ESP.restart();                                                     // Restart zařízení
}

// Funkce na vymazání uložených dat z EEPROM (Function to erase stored data from EEPROM)
void resetCalibreData() {
 preferences.begin(PREF_NAMESPACE, false);
  // Vymažte všechny záznamy v Preferences
  preferences.clear();
  preferences.end();
}

// Funkce na vypsání výjimek restartu (Function to list restart exceptions)
void printResetReason() {
  esp_reset_reason_t reason = esp_reset_reason();
  switch (reason) {
    case ESP_RST_POWERON:   Serial.println("Power on reset"); break;
    case ESP_RST_EXT:       Serial.println("External reset"); break;
    case ESP_RST_SW:        Serial.println("Software reset"); break;
    case ESP_RST_PANIC:     Serial.println("Panic reset"); break;
    case ESP_RST_INT_WDT:   Serial.println("Interrupt watchdog reset"); break;
    case ESP_RST_TASK_WDT:  Serial.println("Task watchdog reset"); break;
    case ESP_RST_WDT:       Serial.println("Watchdog reset"); break;
    case ESP_RST_DEEPSLEEP: Serial.println("Deep sleep reset"); break;
    case ESP_RST_BROWNOUT:  Serial.println("Brownout reset"); break;
    case ESP_RST_SDIO:      Serial.println("SDIO reset"); break;
    default:                Serial.println("Unknown reset reason"); break;
  }
}