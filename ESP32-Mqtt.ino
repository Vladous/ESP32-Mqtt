// https://github.com/Vladous/ESP32-Mqtt

// Deska Deneyap Mini
//
// MQTT Client IoT
// v1.0 20.03.2020
// v1.1 14.02.2021 Prodloužení času stisknutí
// v1.2 08.04.2021 Prodloužení času stisknutí , proměné názvu HostName a Svetlo
// v2.0 12.06.2021 Oprava pžipojrní po výpadku WiFi , Prodloužení času stisknutí
// v2.1 16.11.2021 Pokus o opravu rozkmitání zpětnou vazbou
// v2.3 16.11.2021 Json objekt
// v2.3 22.11.2021 Json výběr
// v2.4 14.05.2023 Úprava restartu zažízení při problémech se spojením s WiFi / MQTT podle OpenAI
//                  Přidíní funkce mikrofonu pro ovládání tlesknutím
// v2.5 30.05.2023 Změna na ESP32
//                  Počáteční nastavení WiFi pomocí WiFimanageru
//                  Úprava velikosti JSON výstupu
// v3.0 21.08.2023 Úprava na ESP32 Deneyap Mini
//                  Rozdělení světel a relé
//
// ESP32 desky - https://dl.espressif.com/dl/package_esp32_index.json
//
//
// ESP 32 Wroom mini S2 - Deneyap Mini
// Mikrofon (KY-038) VCC → 3.3V , GNG → GND , A0 → GPIO34
//
//
//                                Komponenty                       ESP 32 Wroom mini S2                       Komponenty
//                       +--------------------------+             +---------------------+             +--------------------------+             +------+
//                       |                          |             |                     |             |                  VCC     |-------------| VCC  |
//                       |                          |-------------|-------       -------|-------------|   Měření proudu  DATA    |             |      |
//                       |                          |             |      |       |      |             |                  GND     |-------------| GND  |
//                       |                          |             |      |       |      |             +--------------------------+             +------+
//                       |                          |-------------| RES  1      40  39  |-------------|          Relé            |-------------| GND  |
//  +------+             +--------------------------+             |                     |             +--------------------------+             +------+
//  | GND  |-------------|   Tlačítko k zapnutí     |-------------|-------       -------|             |                          |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//  | VCC  |-------------|               VCC        |             |      |       |      |             |                          |
//  |      |             |      DHT 11 - DATA       |-------------|  3   2      38  37  |             |                          |
//  | GND  |-------------|               GND        |             |                     |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+
//  | GND  |-------------|  Led světlo červená (1)  |-------------|-------       -------|             |                          |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//                       |                          |             |  5   4      36  35  |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+
//  | GND  |-------------|  Led světlo zelená (2)   |-------------|-------       -------|             |                          |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//  | GND  |-------------|  Ledka připojení WiFi    |-------------|  7   6      34  33  |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+
//  | GND  |-------------|  Led světlo modrá (3)    |-------------|-------       -------|             |                          |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//  | GND  |-------------| Ledka zapnuto / vypnuto  |-------------|  9   8       21  18 |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+
//                       |                          |             |-------       -------|             |                          |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//  | GND  |-------------|     Ledka napájení       |-------------| 11   10     17  16  |             |                          |
//  +------+             +--------------------------+             |                     |             +--------------------------+
//                       |                          |             |-------       -------|-------------|           GND            |
//  +------+             +--------------------------+             |      |       |      |             +--------------------------+
//  | GND  |-------------|        Mikrofon          |-------------| 13   12     GND GND |             |                          |
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
// "on": true,                      // boolean, Určuje, zda je zařízení zapnuto nebo vypnuto
// "signal": -60,                   // integer, Síla signálu WiFi, RSSI v dBm
// "bssid": "00:11:22:33:44:55",    // string, MAC adresa WiFi hotspotu
// "brightness": 100,               // integer, Jas kontrolek v procentech (0-100)
// "spectrumRGB": [255, 0, 0],      // integer array, Pole tří hodnot, které reprezentují barvy v RGB spektru (0 - 255)
// "temp": 21.2,                    // float, Naměřená teplota ve stupních Celsia
// "hum": 54.6,                     // float, Naměřená relativní vlhkost v procentech
//
// "settings": "get",
//  "ip": "192.168.1.1"             // string, IP adresa zařízení
//  "host": "ESP_HOST"              // string, Hostname zařízení
//  "ClapThreshold": 10,            // integer, Prah pro detekci tlesknutí
//  "CekejOdeslat": 5000,           // integer, Časový interval čekání na odeslání
//  "CekejMereni": 3000,            // integer, Časový interval čekání na měření
//  "CekejDetectClap": 2000,        // integer, Časový interval čekání na detekci tlesknutí
//  "KalibrT": 1.23,                // float, Kalibrace teplotního senzoru
//  "KalibrV": 0.98                 // float, Kalibrace vlhkostního senzoru
//
// JSON callback
// -------------
// "device": "DEVICE_NAME",          // string, Název zařízení ("LED1", "LED2", "LED3", "RGB", "RELAY")
// "state": "on",                    // string, Požadovaný stav zařízení, "on" nebo "off"
// "brightness": 128,                // integer (volitelně), Hodnota jasu LED světla, 0 - 255
// "spectrumRGB": [255, 0, 0],       // integer array (volitelně), Pole tří hodnot RGB, 0 - 255
//
// "settings": "set",                // string, Určuje akci, "set" pro nastavení, "get" pro získání nastavení
//  "ClapThreshold": 900,            // integer (volitelně), Prah pro detekci tlesknutí
//  "CekejOdeslat": 20.0,            // float (volitelně), Časový interval čekání na odeslání (v sekundách)
//  "CekejMereni": 4.0,              // float (volitelně), Časový interval čekání na měření (v sekundách)
//  "CekejDetectClap": 50,           // integer (volitelně), Časový interval pro detekci tlesknutí (v milisekundách)
//  "KalibrT": 1.33,                 // float (volitelně), Kalibrace teplotního senzoru
//  "KalibrV": 0.70                  // float (volitelně), Kalibrace vlhkostního senzoru
//
// JSON payload for sending
// ------------------------
// "devices": [                      // array, Pole objektů s informacemi o zařízeních
//   {
//     "device": "LED1",             // string, Název zařízení LED1, LED2, LED3
//     "state": "on",                // string, Stav zařízení "on" nebo "off"
//     "brightness": 128             // integer (volitelně), Jas zařízení, 0 - 255
//   },
//   {
//     "device": "RGB",
//     "state": "on",
//     "spectrumRGB": [255, 0, 0]    // integer array (volitelně), Hodnoty RGB, 0 - 255
//   },
//   {
//     "device": "RELAY",            // string, Název zařízení
//     "state": "on"                 // string, Stav zařízení "on" nebo "off"
//   }
// ],
// "temp": 21.2,                     // float (volitelně), Naměřená teplota ve stupních Celsia
// "hum": 54.6,                      // float (volitelně), Naměřená relativní vlhkost v procentech
// "Amp": 512,                       // integer (volitelně), Naměřená hodnota z ampérmetru
// "signal": -60,                    // integer, Síla signálu WiFi, RSSI v dBm
// "bssid": "00:11:22:33:44:55",     // string, MAC adresa WiFi hotspotu
//
// "settings": "get",
//  "ip": "192.168.1.1",             // string, IP adresa zařízení
//  "host": "ESP_HOST",              // string, Hostname zařízení
//  "ClapThreshold": 900,            // integer, Prah pro detekci tlesknutí
//  "CekejOdeslat": 20.0,            // float, Časový interval čekání na odeslání (v sekundách)
//  "CekejMereni": 4.0,              // float, Časový interval čekání na měření (v sekundách)
//  "CekejDetectClap": 50,           // integer, Časový interval pro detekci tlesknutí (v milisekundách)
//  "KalibrT": 1.33,                 // float, Kalibrace teplotního senzoru
//  "KalibrV": 0.70                  // float, Kalibrace vlhkostního senzoru
//
//
// Přidat mikrotlačítko reset
//

#include <PubSubClient.h>  // https://github.com/knolleary/pubsubclient
#include <WiFi.h>          // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
#include <WiFiManager.h>   // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>   // https://github.com/bblanchon/ArduinoJson
#include <DHTesp.h>        // https://github.com/adafruit/DHT-sensor-library
#include <Preferences.h>   // https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences
#include <Ticker.h>        // https://github.com/espressif/arduino-esp32/blob/master/libraries/Ticker
#include <esp_system.h>    // Dočasné testování příčiny restartu

#define DHTTYPE DHT11              // Typ DHT sezoru teploty a vlhkosti
#define PREF_NAMESPACE "mqtt-app"  // Jmenný prostor EEPROM

#define LED_WHITE1    0x01
#define LED_WHITE2    0x02
#define LED_WHITE3    0x04
#define LED_RGB       0x08
#define DEVICE_RELAY  0x10

uint8_t DHTPin = 3;                // Nastavení datového pinu DHT
DHTesp dht;                        // Inicializace DHT senzoru

WiFiClient espClient;
PubSubClient client(espClient);
Preferences preferences;

const String Svetlo = "Test_Board";                 // !! CHANGE !!  Topic název zařízení
const uint8_t DeviceType = LED_RGB;                 // !! CHANGE !!  LED_WHITE1 | LED_WHITE2 | LED_WHITE3 | LED_RGB | DEVICE_RELAY
const uint8_t Stisk = LED_RGB;                      // !! CHANGE !!  Použití tlačítka ( LED_WHITE1 | LED_WHITE2 | LED_WHITE3 | LED_RGB | DEVICE_RELAY )
const bool Clap = false;                            // !! CHANGE !!  Použití mikrofonu
const bool Temp = true;                             // !! CHANGE !!  Použití DHT sezoru měření teploty
const bool AmpMeter = false;                        // !! CHANGE !!  Zapnutí měření odběru

const char* WIFI_HOSTNAME = Svetlo.c_str();
char ssid[32];                      // Proměnná pro SSID
char password[32];                  // Proměnná pro heslo
// Mqtt proměnné nastavení
char mqtt_server[40];               // MQTT IP adress
char mqtt_port[6] = "1883";         // MQTT port
char mqtt_username[32];             // MQTT User name
char mqtt_password[32];             // MQTT Password
// Nastavení vstupů ESP
const int Sw = 2;                   // Tlačítko
const int ClapSensor = 12;          // Zvukový senzor připojený na analogový vstup GPIO34
const int AmpPin = 40;              // Vstup ampérmetru
// Výstupy kontrolky led
const int LedPWR = 11;              // Ledka power
const int LedWi = 7;                // Ledka připojení
const int PwrSw = 9;                // Ledka / transistor zapnuti
// Výstupy ovládání
const int Re = 39;                  // Relé
const int PwrRed = 4;               // Ledka power (red)   / Světlo 1
const int PwrGreen = 6;             // Ledka power (green) / Světlo 2
const int PwrBlue = 8;              // Ledka power (blue)  / Světlo 3
// Kalibrační hodnoty
int ClapThreshold = 900;            // Nastavitelná hladina detekce tlesknutí
float CekejOdeslat = 20.0f;         // Prodleva mezi odesláním naměřených hodnot
float CekejMereni = 4.0f;           // Prodleva mezi měřením DHT
int CekejDetectClap = 50;           // Prodleva mezi detekcí tlesknutí
long LastMsg = 0;
int Value = 0;
char SvetloChr[50];
// bool Tlac;
String Zap_str;
volatile bool Rep;
volatile int OZap;                  // Led světlo 1 - 1 , Led svěetlo 2 - 2 , Led světlo 3 - 4 , RGB - 8 , Relé - 16
volatile int Zap;                   // Led světlo 1 - 1 , Led svěetlo 2 - 2 , Led světlo 3 - 4 , RGB - 8 , Relé - 16
float Teplota;
float Vlhkost;
char Pwr[50];

bool led1State = false;
int led1Brightness = 255;

bool led2State = false;
int led2Brightness = 255;

bool led3State = false;
int led3Brightness = 255;

bool ledRGBState = false;
int Red = 254;
int Green = 254;
int Blue = 254;

bool relayState = false;

int LedL = 254;
//int Bright = 254;
double KalibrT = 1.33;
double KalibrV = 0.70;
int PwrAmp;
bool IsConnected = false;
unsigned long lastClapTime = 0;
bool firstClapDetected = false;
const unsigned long doubleClapWindow = 500;
Ticker TimerOdeslat, TimerMereni;

void IRAM_ATTR pushInterrupt() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  // Debouncing: ignorovat přerušení, pokud k němu došlo příliš brzy po předchozím
  if (interruptTime - lastInterruptTime > 200) {
    changeState();
    updateZap();
    aktivaceZarizeni();
    ledKontolaZapnuti();
    Rep = !Rep;  // Přepnout stav tlačítka
    Poslat();
  }
  lastInterruptTime = interruptTime;
}

void setup() {
  pinMode(Sw, INPUT_PULLUP);                                           // PullUp výstup nastavení pinu pro Tlačítko
  pinMode(LedPWR, OUTPUT);                                             // Výstup nastavení pinu pro kontrolku led (červená) power
  pinMode(LedWi, OUTPUT);                                              // Výstup nastavení pinu pro kontrolku led (modrá)   připojení k WiFi a MQTT
  pinMode(PwrSw, OUTPUT);                                              // Výstup nastavení pinu pro kontrolku led (zelená)  zapnutí/vypnutí led/relé
  pinMode(Re, OUTPUT);                                                 // Výstup nastavení pinu pro relé
  pinMode(PwrRed, OUTPUT);                                             // Výstup pro nastavení pinu světla led (red   / white)
  pinMode(PwrGreen, OUTPUT);                                           // Výstup pro nastavení pinu světla led (green / white)
  pinMode(PwrBlue, OUTPUT);                                            // Výstup pro nastavení pinu světla led (blue  / white)
  pinMode(ClapSensor, INPUT);                                          // Vstup pro nastavení pinu Mikrofon
  pinMode(AmpPin, INPUT);                                              // Vstup pro nastavení pinu Ampermetr
  attachInterrupt(digitalPinToInterrupt(Sw), pushInterrupt, FALLING);  // Přerušení na spadání hrany
  analogWrite(LedPWR, LedL);                                           // Zapnutí kontrolky (červená) připojení ke zdroji
  Serial.begin(115200);
  delay(1500);
  printResetReason();     // Dočasné testování příčiny restartu
  if (Temp) {
    dht.setup(DHTPin, DHTesp::DHT11);
  }
  preferences.begin(PREF_NAMESPACE, false);

  // Kontrola nastavených kalibračních hodnot v EEPROM
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

  // Načtení uložených hodnot
  String savedServer = preferences.getString("mqtt_server", "");
  String savedPort = preferences.getString("mqtt_port", "");

  savedServer.toCharArray(mqtt_server, sizeof(mqtt_server));
  savedPort.toCharArray(mqtt_port, sizeof(mqtt_port));

  WiFiManager wifiManager;

  // Pokud chcete vymazat všechny uložené informace wifiManageru, odkomentujte následující řádku a spusťte jej jednou
  //wifiManager.resetSettings();

  // Přidání parametrů do WiFiManager portálu.
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
    // Připojení proběhlo úspěšně, teď uložíme hodnoty do proměnných
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
  // Pokud se dostanete až sem, jste připojeni k WiFi
  Serial.println("Připojeno k WiFi");
  if (strcmp(mqtt_server, "") == 0) {
    strlcpy(mqtt_server, "192.168.10.6", sizeof(mqtt_server));  // Pokud se nenačte z EEPROM nastaví default
    Serial.println("Chyba načtení MQTT serveru z EEPROM");
  }
  if (strcmp(mqtt_port, "") == 0) {
    strlcpy(mqtt_port, "1883", sizeof(mqtt_port));  // Pokud se nenačte z EEPROM, nastaví defaultní hodnotu
    Serial.println("Chyba načtení MQTT portu z EEPROM");
  }
  Serial.print("MQTT server: ");
  Serial.println(mqtt_server);
  Serial.print("MQTT port: ");
  Serial.println(mqtt_port);

  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);

  Svetlo.toCharArray(SvetloChr, Svetlo.length() + 1);
  connectToNetwork();  // Volání nové funkce pro připojení k nastavené WiFi a MQTT
  Serial.println("Moje IP adresa je:");
  Serial.println(WiFi.localIP());
  if (Temp or AmpMeter) {  // Pokud je zapnuto měřění teploty, nebo ampermetr aktivuj timer
    TimerMereni.attach(CekejMereni, tempAndAmpMeter);
  }
  TimerOdeslat.attach(CekejOdeslat, Poslat);
}

void detectClap() {
  int clapValue = analogRead(ClapSensor);
  unsigned long currentTime = millis();
  if (clapValue > ClapThreshold) {
    if (!firstClapDetected) {
      // První tlesknutí
      firstClapDetected = true;
      lastClapTime = currentTime;
    } else {
      // Potenciální druhé tlesknutí
      if (currentTime - lastClapTime <= doubleClapWindow) {
        // Dvojtlesk detekován
        changeState();
        Serial.println("Detekováno kliknutí .. .. .. .. ..");
        firstClapDetected = false;  // Reset detekce
        aktivaceZarizeni();         // Aktualizace stavu zařízení
        Poslat();                   // Odeslání stavu přes MQTT
      } else {
        // Příliš dlouhý interval, považujeme za nový první tlesk
        lastClapTime = currentTime;
      }
    }
    // delay(CekejDetectClap);      // Zpoždění pro zabránění falešných detekcí
  } else if (firstClapDetected && (currentTime - lastClapTime > doubleClapWindow)) {
    // Vypršelo časové okno pro druhé tlesknutí
    firstClapDetected = false;
  }
}

void loop() {
  OZap = Zap;
  if (!IsConnected) {
    connectToNetwork();
  }  
  client.loop();

  if (Clap) {
    detectClap();
  }

  // Úprava nstavení jasu kontrolek
  analogWrite(LedPWR, LedL);
  analogWrite(LedWi, LedL);
  if (Zap > 0) {
    analogWrite(PwrSw, LedL);
  }  
}

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

void changeState() {
  if (Zap == 0) {
      // Zapnout zařízení podle proměnné Stisk
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
    } else {
      // Vypnout všechna zařízení
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

void ledKontolaZapnuti() {
  if (Zap == 0) {
    analogWrite(PwrSw, 0);
  } else {
    analogWrite(PwrSw, LedL);
  }
}

void aktivaceZarizeni() {
  // LED1
  if (DeviceType & LED_WHITE1) {
    if (led1State) {
      analogWrite(PwrRed, led1Brightness);
    } else {
      analogWrite(PwrRed, 0);
    }
  }
  // LED2
  if (DeviceType & LED_WHITE2) {
    if (led2State) {
      analogWrite(PwrGreen, led2Brightness);
    } else {
      analogWrite(PwrGreen, 0);
    }
  }
  // LED3
  if (DeviceType & LED_WHITE3) {
    if (led3State) {
      analogWrite(PwrBlue, led3Brightness);
    } else {
      analogWrite(PwrBlue, 0);
    }
  }
  // RGB LED
  if (DeviceType & LED_RGB) {
    if (ledRGBState) {
      analogWrite(PwrRed, Red);
      analogWrite(PwrGreen, Green);
      analogWrite(PwrBlue, Blue);
    } else {
      // Pokud RGB LED není zapnuta, vypneme její piny
      analogWrite(PwrRed, 0);
      analogWrite(PwrGreen, 0);
      analogWrite(PwrBlue, 0);
    }
  }
  if (DeviceType & DEVICE_RELAY) {
    if (relayState) {
      digitalWrite(Re, HIGH); // Zapnout relé
    } else {
      digitalWrite(Re, LOW);  // Vypnout relé
    }
  }
  updateZap();
  ledKontolaZapnuti();
}

void tempAndAmpMeter() {
  if (Temp) {
    senzorTemp();  // Načtení hodnoty ze seznoru DHT
  }
  if (AmpMeter) {
    measureAmp();  // Načtení hodnoty Ampermetru
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, payload, length);

  if (error) {
    Serial.print("Deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc.containsKey("settings")) {
    const char* settingAction = doc["settings"];
    if (strcmp(settingAction, "set") == 0) {
      callbackSettingsSet(doc);
    } else if (strcmp(settingAction, "get") == 0) {
      callbackSettingsGet();
    }
  } else {
    callbackDevice(doc);
  }
}

void callbackSettingsSet(JsonDocument& doc) {
  preferences.begin(PREF_NAMESPACE, false);
  if (doc.containsKey("ClapThreshold")) {
    ClapThreshold = doc["ClapThreshold"].as<int>();
    preferences.putInt("ClapThreshold", ClapThreshold);
  }
  if (doc.containsKey("CekejOdeslat")) {
    CekejOdeslat = doc["CekejOdeslat"].as<float>();
    preferences.putInt("CekejOdeslat", CekejOdeslat);
  }
  if (doc.containsKey("CekejMereni")) {
    CekejMereni = doc["CekejMereni"].as<float>();
    preferences.putInt("CekejMereni", CekejMereni);
  }
  if (doc.containsKey("CekejDetectClap")) {
    CekejDetectClap = doc["CekejDetectClap"].as<int>();
    preferences.putInt("CekejDetectClap", CekejDetectClap);
  }
  if (doc.containsKey("KalibrT")) {
    KalibrT = doc["KalibrT"].as<float>();
    preferences.putFloat("KalibrT", KalibrT);
  }
  if (doc.containsKey("KalibrV")) {
    KalibrV = doc["KalibrV"].as<float>();
    preferences.putFloat("KalibrV", KalibrV);
  }
  preferences.end();
}

void callbackSettingsGet() {
  DynamicJsonDocument responseDoc(512);
  responseDoc["ip"] = WiFi.localIP().toString();
  responseDoc["host"] = WIFI_HOSTNAME;
  responseDoc["CekejOdeslat"] = CekejOdeslat;  
  if (Temp || AmpMeter) {
    responseDoc["CekejMereni"] = CekejMereni;
    if (Temp) {
      responseDoc["KalibrT"] = KalibrT;
      responseDoc["KalibrV"] = KalibrV;
    }
  }  
  if (Clap) {
    responseDoc["ClapThreshold"] = ClapThreshold;
    responseDoc["CekejDetectClap"] = CekejDetectClap;
  }
  char responseOut[512];

  serializeJson(responseDoc, responseOut);
  client.publish(SvetloChr, responseOut);
}

void callbackDevice(JsonDocument& doc) {
  if (doc["brightArd"] != nullptr) {
      LedL = doc["brightArd"];
    }
  if (doc.containsKey("device") && doc.containsKey("state")) {
    String deviceName = doc["device"].as<String>();
    String state = doc["state"].as<String>();
    bool isOn = (state == "on");

    if ((deviceName == "LED1") && (DeviceType & LED_WHITE1)) {
      led1State = isOn;
      if (doc.containsKey("brightness")) {
        led1Brightness = doc["brightness"].as<int>();
      }
    } else if ((deviceName == "LED2") && (DeviceType & LED_WHITE2)) {
      led2State = isOn;
      if (doc.containsKey("brightness")) {
        led2Brightness = doc["brightness"].as<int>();
      }
    } else if ((deviceName == "LED3") && (DeviceType & LED_WHITE3)) {
      led3State = isOn;
      if (doc.containsKey("brightness")) {
        led3Brightness = doc["brightness"].as<int>();
      }
    } else if ((deviceName == "RGB") && (DeviceType & LED_RGB)) {
      ledRGBState = isOn;
      if (doc.containsKey("spectrumRGB")) {
        JsonArray rgbValues = doc["spectrumRGB"].as<JsonArray>();
        if (rgbValues.size() == 3) {
          Red = rgbValues[0];
          Green = rgbValues[1];
          Blue = rgbValues[2];
        }
      }
    } else if ((deviceName == "RELAY") && (DeviceType & DEVICE_RELAY)) {
      relayState = isOn;
    }
    // Aktualizujeme světla podle nového stavu
    aktivaceZarizeni();
  }
}

void reconnect() {
  if (!client.connected()) {
    IsConnected = false;
  }
}

void Poslat() {
  reconnect();
  DynamicJsonDocument doc(512);  

  
  // Vytvoříme pole "devices" pouze pokud existují nějaké LED k odeslání
  JsonArray devices = doc.createNestedArray("devices");
  
  // Kontrola a přidání LED1
  if (DeviceType & LED_WHITE1) {
    JsonObject led1 = devices.createNestedObject();
    led1["device"] = "LED1";
    led1["state"] = led1State ? "on" : "off";
    led1["brightness"] = led1Brightness;
  }

  // Kontrola a přidání LED2
  if (DeviceType & LED_WHITE2) {
    JsonObject led2 = devices.createNestedObject();
    led2["device"] = "LED2";
    led2["state"] = led2State ? "on" : "off";
    led2["brightness"] = led2Brightness;
  }

  // Kontrola a přidání LED3
  if (DeviceType & LED_WHITE3) {
    JsonObject led3 = devices.createNestedObject();
    led3["device"] = "LED3";
    led3["state"] = led3State ? "on" : "off";
    led3["brightness"] = led3Brightness;
  }

  // Kontrola a přidání RGB LED
  if (DeviceType & LED_RGB) {
    JsonObject ledRGB = devices.createNestedObject();
    ledRGB["device"] = "RGB";
    ledRGB["state"] = ledRGBState ? "on" : "off";
    JsonArray rgbValues = ledRGB.createNestedArray("spectrumRGB");
    rgbValues.add(Red);
    rgbValues.add(Green);
    rgbValues.add(Blue);
  }

  // Přidání relé
  if (DeviceType & DEVICE_RELAY) {
    JsonObject relay = devices.createNestedObject();
    relay["device"] = "RELAY";
    relay["state"] = relayState ? "on" : "off";
  }

  // Pokud pole "devices" je prázdné, odstraníme jej z JSON zprávy
  if (devices.size() == 0) {
    doc.remove("devices");
  }

  if (Temp) {
    doc["temp"] = Teplota;
    doc["hum"] = Vlhkost;
  }

  if (AmpMeter) {
    doc["Amp"] = PwrAmp;
  }

  // Základní informace o zařízení
  doc["signal"] = WiFi.RSSI();
  // Přidání MAC adresy WiFi hotspotu
  byte bssid[6];
  WiFi.BSSID(bssid);
  char bssidStr[18];
  sprintf(bssidStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
  doc["bssid"] = bssidStr;

  char out[256];

  serializeJson(doc, out);  
  client.publish(SvetloChr, out);

  float teplotaCipu = temperatureRead();
  Serial.print("Teplota čipu: ");
  Serial.print(teplotaCipu);
  Serial.println(" °C");

}

void senzorTemp() {
  TempAndHumidity newValues = dht.getTempAndHumidity();
  if (!isnan(newValues.temperature) && !isnan(newValues.humidity)) {
      Teplota = (Teplota + newValues.temperature / KalibrT) / 2;
      Vlhkost = (Vlhkost + newValues.humidity / KalibrV) / 2;
    }
}

void measureAmp() {  // Měření hodnoty z ampermetru
  float a = analogRead(AmpPin);
  if (!isnan(a)) {
      PwrAmp = (PwrAmp + a) / 2;
    }
}

void connectToNetwork() {
  int n = WiFi.scanNetworks();
  int bestNetworkIndex = -1;
  int bestRSSI = -9999;  // Nízká výchozí hodnota pro porovnání
  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == ssid) {  // Hledání sítě s požadovaným SSID
      int rssi = WiFi.RSSI(i);
      if (rssi > bestRSSI) {  // Pokud je signál silnější, ulož index a RSSI
        bestNetworkIndex = i;
        bestRSSI = rssi;
      }
    }
  }
  if (bestNetworkIndex != -1) {  // Pokud byla nalezena vhodná síť
    WiFi.begin(WiFi.SSID(bestNetworkIndex).c_str(), password);

    int timeout = 10000;  // 10 sekund timeout
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
      analogWrite(LedWi, HIGH);
      delay(50);
      analogWrite(LedWi, LOW);
      delay(50);
    }

    if (WiFi.status() == WL_CONNECTED) {
      analogWrite(LedWi, LedL);

      while (!client.connected()) {
        if (client.connect(WIFI_HOSTNAME)) {
          analogWrite(LedWi, LedL);
          client.subscribe(SvetloChr);
          IsConnected = true;
          return;  // MQTT připojeno, ukončit funkci
        } else {
          analogWrite(LedWi, LedL);
          delay(2500);
          analogWrite(LedWi, 0);
          delay(2500);
        }
      }
    } else {
      IsConnected = false;
    }
  } else {
    IsConnected = false;
  }
}

// Zatím nepoužitá funkce
void resetCalibreData() {
 preferences.begin(PREF_NAMESPACE, false);
  // Vymažte všechny záznamy v Preferences
  preferences.clear();
  preferences.end();
}

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