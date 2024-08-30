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
// JSON callback
// -------------
// "on": true,                      // boolean, Určuje, zda se má zařízení zapnout nebo vypnout, true / false
// "spectrumRGB": [255, 0, 0],      // integer array, Pole tří hodnot, které nastaví barvy v RGB spektru, 0 - 255
// "brightArd": 128,                // integer, Hodnota jasu LED světla, 0 - 255
// "brightness": 100,               // integer, Jas kontrolek v procentech, 0 - 100
// "stav": true,                    // boolean, Indikuje, zda je třeba odeslat aktualizovaný stav zpět, true / false
//
// "settings": "set",               // string, Určuje akci, "set" pro nastavení, "get" pro získání nastavení
//  "ClapThreshold": 10,            // integer, Prah pro detekci tlesknutí
//  "CekejOdeslat": 5000,           // integer, Časový interval čekání na odeslání
//  "CekejMereni": 3000,            // integer, Časový interval čekání na měření
//  "CekejDetectClap": 2000,        // integer, Časový interval čekání na detekci tlesknutí
//  "KalibrT": 1.23,                // float, Kalibrace teplotního senzoru
//  "KalibrV": 0.98                 // float, Kalibrace vlhkostního senzoru
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
// "JSONsize": 123                  // integer, Velikost JSON dokumentu
//
//
//
// Přidat mikrotlačítko reset
// Přidat podmínky boolean na všechna dostupná místa
//


#include <PubSubClient.h>  // https://github.com/knolleary/pubsubclient
#include <WiFi.h>          // https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
#include <WiFiManager.h>   // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>   // https://github.com/bblanchon/ArduinoJson
#include <DHT.h>           // https://github.com/adafruit/DHT-sensor-library
#include <Preferences.h>   // https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences
#include <Ticker.h>        // https://github.com/espressif/arduino-esp32/blob/master/libraries/Ticker
#include <esp_system.h>    // Dočasné testování příčiny restartu

#define DHTTYPE DHT11              // Typ DHT sezoru teploty a vlhkosti
#define PREF_NAMESPACE "mqtt-app"  // Jmenný prostor EEPROM
uint8_t DHTPin = 3;                // Nastavení datového pinu DHT
DHT dht(DHTPin, DHTTYPE);          // Inicializace DHT senzoru

WiFiClient espClient;
PubSubClient client(espClient);
Preferences preferences;

// Rozdělit světla a relé
const int LedType = 8;              // !! CHANGE !!  Typ led světel   ( White(1) - 1 / Wihe(2) - 2 / White (3) - 4 / RGB - 8 )
const String Svetlo = "Svetlo_05";  // !! CHANGE !!  Topic název zařízení
const bool Relay = false;           // !! CHANGE !!  Relé (Zásuvka)                    //////// Přesunout do Stisk
const bool Clap = false;            // !! CHANGE !!  Použití mikrofonu
const bool Temp = false;             // !! CHANGE !!  Použití DHT sezoru měření teploty
const int Stisk = 8;                // !! CHANGE !!  Použití tlačítka ( Led světlo 1 - 1 , Led světlo 2 - 2 , Led světlo 4, RGB - 8 , Relé - 16 )
const bool AmpMeter = false;        // !! CHANGE !!  Zapnutí měření odběru

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
int Red = 254;
int Green = 254;
int Blue = 254;
int LedL = 254;
int Bright = 254;
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
    if (Zap == 0) {
      Zap = Stisk;                  // Nastavit novou hodnotu
    } else {
      Zap = 0;                      // Resetovat hodnotu
    }
    Rep = !Rep;                     // Přepnout stav tlačítka
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
    dht.begin();
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
        Zap = !Zap;
        firstClapDetected = false;  // Reset detekce
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
  if (OZap != Zap) {
    aktivaceSvetel();
    Poslat();
  }
  if (Relay) {
    if (Zap & 16) {
      digitalWrite(Re, HIGH);
      ledKontolaZapnuti();
    } else {
      digitalWrite(Re, LOW);
      ledKontolaZapnuti();
    }
  }
  
  // Úprava nstavení jasu kontrolek
  analogWrite(LedPWR, LedL);
  analogWrite(LedWi, LedL);
  
}

void ledKontolaZapnuti() {
  if (Zap > 0) {
    analogWrite(PwrSw, LedL);
  } else {
    analogWrite(PwrSw, 0);
  }
}

void aktivaceSvetel() {
    // Nastavujeme světla podle stavu v proměnné Zap
    if (Zap & 1) {
        analogWrite(PwrRed, Red);
    } else {
        analogWrite(PwrRed, 0);
    }
    if (Zap & 2) {
        analogWrite(PwrGreen, Green);
    } else {
        analogWrite(PwrGreen, 0);
    }
    if (Zap & 4) {
        analogWrite(PwrBlue, Blue);
    } else {
        analogWrite(PwrBlue, 0);
    }
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
    CekejOdeslat = doc["CekejOdeslat"].as<int>();
    preferences.putInt("CekejOdeslat", CekejOdeslat);
  }
  if (doc.containsKey("CekejMereni")) {
    CekejMereni = doc["CekejMereni"].as<int>();
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
  int jsonSize = serializeJson(responseDoc, responseOut);
  responseDoc["JSONsize"] = jsonSize;
  serializeJson(responseDoc, responseOut);
  client.publish(SvetloChr, responseOut);
}

void callbackDevice(JsonDocument& doc) {
  // Pomocná funkce pro zpracování hodnot
  auto processValue = [](JsonVariant variant, int& value) {
    if (variant != nullptr) {
      value = round(variant.as<int>() * 2.54);
    }
  };

  if (doc["on"] != nullptr) {
    Zap = doc["on"];
  }

  if (doc["brightArd"] != nullptr) {
    LedL = doc["brightArd"];
  }

  switch (LedType) {

    case 1:
      {
        processValue(doc["brightness"], Bright);
      }
      break;

    case 2:
      {
        if (doc["2Lights"][0] != nullptr) {
          processValue(doc["2Lights"][0], Red);
        }
        if (doc["2Lights"][1] != nullptr) {
          processValue(doc["2Lights"][1], Green);
        }
      }
      break;

    case 4:
      {
        if (doc["3Lights"][0] != nullptr) {
          processValue(doc["3Lights"][0], Red);
        }
        if (doc["3Lights"][1] != nullptr) {
          processValue(doc["3Lights"][1], Green);
        }
        if (doc["3Lights"][2] != nullptr) {
          processValue(doc["3Lights"][2], Blue);
        }
      }
      break;

    case 8:
      {
        if (doc["spectrumRGB"][0] != nullptr) {
          Red = doc["spectrumRGB"][0];
        }
        if (doc["spectrumRGB"][1] != nullptr) {
          Green = doc["spectrumRGB"][1];
        }
        if (doc["spectrumRGB"][2] != nullptr) {
          Blue = doc["spectrumRGB"][2];
        }
      }
      break;
  }
  aktivaceSvetel();
}

void reconnect() {
  if (!client.connected()) {
    IsConnected = false;
  }
}

void Poslat() {
  DynamicJsonDocument doc(512);
  

  // Základní informace o zařízení
  doc["on"] = Zap;
  doc["signal"] = WiFi.RSSI();

  // Přidání MAC adresy WiFi hotspotu
  byte bssid[6];
  WiFi.BSSID(bssid);
  char bssidStr[18];
  sprintf(bssidStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
  doc["bssid"] = bssidStr;

  switch (LedType) {
    case 1:
      {
        doc["brightness"] = round(Bright / 2.54);
      }
      break;
    case 2:
      {
        JsonArray data = doc.createNestedArray("2Lights");
        data.add(round(Red / 2.54));
        data.add(round(Green / 2.54));
      }
      break;
    case 4:
      {
        JsonArray data = doc.createNestedArray("3Lights");
        data.add(round(Red / 2.54));
        data.add(round(Green / 2.54));
        data.add(round(Blue / 2.54));
      }
      break;
    case 8:
      {
        JsonArray data = doc.createNestedArray("spectrumRGB");
        data.add(Red);
        data.add(Green);
        data.add(Blue);
      }
      break;
    }

  if (Temp) {
    doc["temp"] = Teplota;
    doc["hum"] = Vlhkost;
  }

  if (AmpMeter) {
    doc["Amp"] = PwrAmp;
  }

  char out[256];
  int jsonSize = serializeJson(doc, out);
  doc["JSONsize"] = jsonSize;
  serializeJson(doc, out);
  
  client.publish(SvetloChr, out);
}

void senzorTemp() {
  float t = dht.readTemperature();
  Teplota = (Teplota + t / KalibrT) / 2;
  float h = dht.readHumidity();
  Vlhkost = (Vlhkost + h / KalibrV) / 2;
}

void measureAmp() {  // Měření hodnoty z ampermetru
  PwrAmp = (PwrAmp + analogRead(AmpPin)) / 2;
  delay(10);
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

// Dočasné testování příčiny restartu

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