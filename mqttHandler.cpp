//
// PubSubClient.h
// #define MQTT_MAX_PACKET_SIZE 1024
//
// Maximální délka MQTT zprávy 952 b ! (Maximální délka MQTT zprávy bez úpravy PubSubClient.h - 238 b !)
//

#include "mqttHandler.h"
#include "config.h"
#include <Preferences.h>

// Globální proměnné (pokud jsou potřeba)
extern Preferences preferences;

static bool awaitingDbStateRestore = true;
static unsigned long dbSyncStartedAt = 0;
static unsigned long lastDbSyncRequestAt = 0;
static unsigned char dbSyncRetryCount = 0;

static constexpr unsigned long DB_SYNC_RETRY_INTERVAL_MS = 5000;
static constexpr unsigned long DB_SYNC_TIMEOUT_MS = 30000;
static constexpr unsigned char DB_SYNC_MAX_RETRIES = 6;

static int getSmoothedRssi() {
  static bool initialized = false;
  static float rssiFiltered = -100.0f;

  auto absf = [](float value) -> float {
    return (value < 0.0f) ? -value : value;
  };

  const int rawRssi = WiFi.RSSI();
  if (!initialized) {
    rssiFiltered = static_cast<float>(rawRssi);
    initialized = true;
    return rawRssi;
  }

  float delta = static_cast<float>(rawRssi) - rssiFiltered;
  float alpha = 0.25f;
  if (absf(delta) > 8.0f) {
    alpha = 0.45f;
  } else if (absf(delta) < 2.0f) {
    alpha = 0.15f;
  }

  rssiFiltered += alpha * delta;
  return static_cast<int>(rssiFiltered + (rssiFiltered >= 0.0f ? 0.5f : -0.5f));
}

static void publishDbSyncFallback(const char* syncStatus, const char* reason) {
  String out = "{\"device\":\"" + String(SvetloChr) +
               "\",\"action\":\"dbSyncResult\",\"status\":\"" + String(syncStatus) +
               "\",\"reason\":\"" + String(reason) +
               "\",\"fallback\":\"localDefault\",\"brightness\":\"NaN\",\"note\":\"Vim o tobe, ale nic pro tebe nemam.\"}";

  String resultTopic = String(SvetloChr) + "/db-sync-result";
  client.publish(resultTopic.c_str(), out.c_str(), false);
  client.publish(SvetloChr, out.c_str(), false);
}

static void finishDbSyncAwait(const String& reason) {
  if (!awaitingDbStateRestore) {
    return;
  }

  awaitingDbStateRestore = false;
  dbSyncStartedAt = 0;
  lastDbSyncRequestAt = 0;
  dbSyncRetryCount = 0;
  debugMQTT(reason);
}

// Helper funkce pro aktualizaci konfigurace
template<typename TValue, typename TPref>
void updateConfig(JsonDocument& doc, const char* key, TValue& configValue,
                  size_t (Preferences::*putFunc)(const char*, TPref)) {
  if (doc.containsKey(key)) {
    configValue = doc[key].as<TValue>();
    (preferences.*putFunc)(key, static_cast<TPref>(configValue));
  }
}

// Specializace pro String s validací
void updateConfigString(JsonDocument& doc, const char* key, String& configValue, 
                        const char** validValues = nullptr, int validCount = 0) {
  if (doc.containsKey(key)) {
    const char* value = doc[key];
    if (value != nullptr) {
      String strValue = String(value);
      // Pokud je validace, kontroluj
      if (validValues != nullptr) {
        for (int i = 0; i < validCount; i++) {
          if (strValue == validValues[i]) {
            configValue = strValue;
            preferences.putString(key, configValue);
            return;
          }
        }
      } else {
        // Bez validace
        configValue = strValue;
        preferences.putString(key, configValue);
      }
    }
  }
}

// Helper pro update stavu LED s brightness
void updateLEDState(JsonDocument& doc, volatile bool& state, volatile int& brightness) {
  if (doc.containsKey("state")) {
    String stateStr = doc["state"].as<String>();
    state = (stateStr == "on");
  }
  if (doc.containsKey("brightness")) {
    brightness = doc["brightness"].as<int>();
  }
}

// Helper pro přidání LED do JSON
void addLEDToJson(JsonArray& devices, const char* name, bool state, int brightness) {
  JsonObject led = devices.createNestedObject();
  led["device"] = name;
  led["state"] = state ? "on" : "off";
  led["brightness"] = brightness;
}

// Implementace funkcí
void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    String message = "❌ Chyba deserializace callback: " + String(error.c_str()) + "\n";
    message += topicStr + " - ";    
    // Převod payload na String
    for (unsigned int i = 0; i < length; i++) {
      message += (char)payload[i];
    }
    debugMQTT(message);
    //Serial.print("Deserialization failed: ");
    //Serial.println(error.c_str());
    return;
  }

  if (topicStr == manualConfig.LedBrightnessTopic) {
    if (!doc["brightArd"].isNull()) {
      LedL = doc["brightArd"];
    }
    // Přidat noční režim
    if (doc.containsKey("localTime")) {
      const char* localTime = doc["localTime"];
      if (localTime) {
        // Extrahování hodin a minut
        int hours, minutes;
        sscanf(localTime, "%*d. %*d. %*d %d:%d", &hours, &minutes);
        if (hours >= defaultConfig.NightStartHour || hours < defaultConfig.NightEndHour) {
          defaultConfig.NightKontrolLedEnable = true;
        } else {
          defaultConfig.NightKontrolLedEnable = false;
        }
      }
    }
  } else {
    if (doc.containsKey("dbSyncDone")) {
      finishDbSyncAwait("DB sync potvrzen, periodicke odesilani stavu povoleno.");
    }
    if (doc.containsKey("dbSyncNoData")) {
      publishDbSyncFallback("noData", "DB nema ulozena data pro toto zarizeni.");
      finishDbSyncAwait("DB sync bez dat, pouzivam lokalni vychozi stav.");
      return;
    }

    if (doc.containsKey("settings")) {
      const char* settingAction = doc["settings"];
      if (strcmp(settingAction, "set") == 0) {
        callbackSettingsSet(doc);
      } else if (strcmp(settingAction, "get") == 0) {
        callbackSettingsGet();
      }
    } else if (doc.containsKey("reset")) {

      const char* resetAction = doc["reset"];
      if (strcmp(settingAction, "calibre") == 0) {
        resetCalibreData();  
      } else if (strcmp(settingAction, "wifi") == 0) {
        resetWifiManager();  
      }

      //resetCalibreData();
      //resetWifiManager();


    } else if (doc.containsKey("boardVersion")) {
      reportBoardVersion();
    } else if (doc.containsKey("reportFirmwareVersion") || doc.containsKey("firmwareVersion")) {
      reportFirmwareVersion();
    } else if (doc.containsKey("restart")) {
      restartDevice();
    } else if (doc.containsKey("help")) {
      sendHelpResponse();
    } else {
      if (awaitingDbStateRestore && doc.containsKey("device")) {
        finishDbSyncAwait("Dorazil device stav, periodicke odesilani stavu povoleno.");
      }
      callbackDevice(doc);
    }
  }
}

void callbackSettingsSet(JsonDocument& doc) {
  preferences.begin(PREF_NAMESPACE, false);
  
  // Int hodnoty
  updateConfig(doc, "ClapThreshold", defaultConfig.ClapThreshold, &Preferences::putInt);
  updateConfig(doc, "DistanceSet", defaultConfig.DistanceSet, &Preferences::putInt);
  updateConfig(doc, "CekejDetectClap", defaultConfig.CekejDetectClap, &Preferences::putInt);
  updateConfig(doc, "NightStartHour", defaultConfig.NightStartHour, &Preferences::putInt);
  updateConfig(doc, "NightStartMin", defaultConfig.NightStartMin, &Preferences::putInt);
  updateConfig(doc, "NightEndHour", defaultConfig.NightEndHour, &Preferences::putInt);
  updateConfig(doc, "NightEndMin", defaultConfig.NightEndMin, &Preferences::putInt);
  
  // Float hodnoty
  updateConfig(doc, "CekejOdeslat", defaultConfig.CekejOdeslat, &Preferences::putFloat);
  updateConfig(doc, "CekejMereni", defaultConfig.CekejMereni, &Preferences::putFloat);
  updateConfig(doc, "KalibrT", defaultConfig.KalibrT, &Preferences::putFloat);
  updateConfig(doc, "KalibrV", defaultConfig.KalibrV, &Preferences::putFloat);
  
  // Bool hodnoty
  updateConfig(doc, "NightKontrolLed", defaultConfig.NightKontrolLed, &Preferences::putBool);
  updateConfig(doc, "NightKontrolLedEnable", defaultConfig.NightKontrolLedEnable, &Preferences::putBool);
  
  // String s validací
  const char* distanceUnits[] = {"cm", "inch"};
  updateConfigString(doc, "DistanceUnit", defaultConfig.DistanceUnit, distanceUnits, 2);
  
  const char* tempUnits[] = {"C", "F"};
  updateConfigString(doc, "TempUnit", defaultConfig.TempUnit, tempUnits, 2);
  
  preferences.end();
}

// PubSubClient.h
// #define MQTT_MAX_PACKET_SIZE 1024
void callbackSettingsGet() {
  DynamicJsonDocument responseDoc(1024);
  responseDoc["ip"] = WiFi.localIP().toString();
  responseDoc["host"] = WIFI_HOSTNAME;
  responseDoc["deviceList"] = manualConfig.DeviceType;
  responseDoc["CekejOdeslat"] = defaultConfig.CekejOdeslat;
  if (manualConfig.useTemp || manualConfig.useAmpMeter) {
    responseDoc["CekejMereni"] = defaultConfig.CekejMereni;
    if (manualConfig.useTemp) {
      responseDoc["KalibrT"] = defaultConfig.KalibrT;
      responseDoc["KalibrV"] = defaultConfig.KalibrV;
      responseDoc["TempUnit"] = defaultConfig.TempUnit;
    }
  }
  if (manualConfig.useClap) {
    responseDoc["ClapThreshold"] = defaultConfig.ClapThreshold;
    responseDoc["CekejDetectClap"] = defaultConfig.CekejDetectClap;
  }
  if (manualConfig.useWave) {
    responseDoc["DistanceSet"] = defaultConfig.DistanceSet;
    responseDoc["DistanceUnit"] = defaultConfig.DistanceUnit;
  }
  responseDoc["TeplotaChip"] = (int)temperatureRead();
  responseDoc["Verze"] = MAIN_VERSION;
  
  // Noční režim kontrolních LED
  if (defaultConfig.NightKontrolLed) {
    responseDoc["NightKontrolLed"] = defaultConfig.NightKontrolLed;
    responseDoc["NightKontrolLedEnable"] = defaultConfig.NightKontrolLedEnable;
    responseDoc["NightStartHour"] = defaultConfig.NightStartHour;
    responseDoc["NightStartMin"] = defaultConfig.NightStartMin;
    responseDoc["NightEndHour"] = defaultConfig.NightEndHour;
    responseDoc["NightEndMin"] = defaultConfig.NightEndMin;
  }
  
  char responseOut[1024];
  serializeJson(responseDoc, responseOut);
  
  Serial.print("Velikost zprávy (strlen): ");
  Serial.println(strlen(responseOut));

  client.publish(SvetloChr, responseOut);
}

void callbackDevice(JsonDocument& doc) {
  if (!doc.containsKey("device")) {
    // Pokud není definováno zařízení, funkci ukončíme
    return;
  }

  String deviceName = doc["device"].as<String>();  // Povinné: Vybrané zařízení

  // Zpracování jednotlivých zařízení
  if ((deviceName == "LED1") && (manualConfig.DeviceType & LED_WHITE1)) {
    updateLEDState(doc, led1State, led1Brightness);
  } else if ((deviceName == "LED2") && (manualConfig.DeviceType & LED_WHITE2)) {
    updateLEDState(doc, led2State, led2Brightness);
  } else if ((deviceName == "LED3") && (manualConfig.DeviceType & LED_WHITE3)) {
    updateLEDState(doc, led3State, led3Brightness);
  } else if ((deviceName == "RGB") && (manualConfig.DeviceType & LED_RGB)) {
    if (doc.containsKey("state")) {
      String state = doc["state"].as<String>();
      ledRGBState = (state == "on");
    }
    if (doc.containsKey("spectrumRGB")) {
      JsonArray rgbValues = doc["spectrumRGB"].as<JsonArray>();
      if (rgbValues.size() == 3) {
        Red = rgbValues[0];
        Green = rgbValues[1];
        Blue = rgbValues[2];
      }
    }
  } else if ((deviceName == "RELAY") && (manualConfig.DeviceType & DEVICE_RELAY)) {
    if (doc.containsKey("state")) {
      String state = doc["state"].as<String>();
      relayState = (state == "on");
    }
  }
  // Aktualizujeme stav zařízení
  aktivaceZarizeni();
}

void Poslat() {
  Poslat("");
}

void Poslat(String from = "") {
  if (awaitingDbStateRestore && from != "force") {
    requestDbStateRestore();

    if (dbSyncStartedAt != 0 && (millis() - dbSyncStartedAt) >= DB_SYNC_TIMEOUT_MS) {
      publishDbSyncFallback("timeout", "DB sync timeout, odpoved neprisla.");
      finishDbSyncAwait("DB sync timeout, prechod na lokalni vychozi stav.");
    } else if (dbSyncRetryCount >= DB_SYNC_MAX_RETRIES) {
      publishDbSyncFallback("noResponse", "DB sync bez odpovedi po max poctu pokusu.");
      finishDbSyncAwait("DB sync bez odpovedi, prechod na lokalni vychozi stav.");
    }

    return;
  }

  reconnect();                                           // Volání funkce pro kontrolu připojení k WiFi a MQTT (WiFi and MQTT connection check)
  DynamicJsonDocument doc(1024);                         // Deklarace proměnné doc pro Json (Declaration of doc variable for Json)
  JsonArray devices = doc.createNestedArray("devices");  // Vytvoření pole devices k odeslání (Creating the devices array to send)
  
  // Přidání bílých LED
  if (manualConfig.DeviceType & LED_WHITE1) {
    addLEDToJson(devices, "LED1", led1State, led1Brightness);
  }
  if (manualConfig.DeviceType & LED_WHITE2) {
    addLEDToJson(devices, "LED2", led2State, led2Brightness);
  }
  if (manualConfig.DeviceType & LED_WHITE3) {
    addLEDToJson(devices, "LED3", led3State, led3Brightness);
  }
  
  // Přidání RGB LED
  if (manualConfig.DeviceType & LED_RGB) {
    JsonObject ledRGB = devices.createNestedObject();
    ledRGB["device"] = "RGB";
    ledRGB["state"] = ledRGBState ? "on" : "off";
    JsonArray rgbValues = ledRGB.createNestedArray("spectrumRGB");
    rgbValues.add(Red);
    rgbValues.add(Green);
    rgbValues.add(Blue);
  }
  
  // Přidání relé
  if (manualConfig.DeviceType & DEVICE_RELAY) {
    JsonObject relay = devices.createNestedObject();
    relay["device"] = "RELAY";
    relay["state"] = relayState ? "on" : "off";
  }
  // Pokud pole "devices" je prázdné, odstraníme jej z JSON zprávy (If the devices field is empty, we remove it from the JSON message)
  if (devices.size() == 0) {
    doc.remove("devices");
  }

  if (manualConfig.useTemp) {
    float tempOut = Teplota;
    if (defaultConfig.TempUnit == "F") {
      tempOut = (Teplota * 9.0f / 5.0f) + 32.0f;
    }
    doc["temp"] = tempOut;  // Přidání naměřené teploty (podle nastavené jednotky)
    doc["tempUnit"] = defaultConfig.TempUnit;
    doc["hum"] = Vlhkost;   // Přidání naměřené vlhkosti do výstupu k odeslání (Adding the measured humidity to the output for sending)
  }
  if (manualConfig.useAmpMeter) {
    doc["Amp"] = PwrAmp;  // Přidání naměřeného odběru proudu do výstupu k odeslání (Adding the measured current draw to the output to send)
  }
  // Základní informace o zařízení
  doc["signal"] = getSmoothedRssi();  // Vyhlazená síla signálu pro stabilnější telemetrii
  uint8_t* bssid = WiFi.BSSID();            // (Retrieve the MAC address of the WiFi hotspot)
  char bssidStr[18];
  sprintf(bssidStr, "%02X:%02X:%02X:%02X:%02X:%02X",
          bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
  doc["bssid"] = bssidStr;  // Přidání aktuální MAC adresy WiFi hotspotu do výstupu k odeslání (Adding the current MAC address of the WiFi hotspot to the output to send)
  char out[256];
  serializeJson(doc, out);         // Serializace dat do Json (Serializing data to Json)
  client.publish(SvetloChr, out);  // Odeslání dat přes MQTT (Sending data via MQTT)
}

void sendHelpResponse() {
  DynamicJsonDocument helpDoc(1024);
  // Přidání seznamu příkazů
  JsonArray commands = helpDoc.createNestedArray("commands");
  commands.add("reset");
  commands.add("restart");
  commands.add("settings-set");
  commands.add("settings-get");
  commands.add("help");
  
  // Detailní možnosti pro "settings-set"
  JsonArray settingsSetOptions = helpDoc.createNestedArray("settings-set-options");
  settingsSetOptions.add("CekejOdeslat");
  if (manualConfig.useTemp || manualConfig.useAmpMeter) {
    settingsSetOptions.add("CekejMereni");    
    if (manualConfig.useTemp) {
      settingsSetOptions.add("KalibrT");
      settingsSetOptions.add("KalibrV");
      settingsSetOptions.add("TempUnit");
    }
  }
  if (manualConfig.useClap) {
    settingsSetOptions.add("ClapThreshold");
    settingsSetOptions.add("CekejDetectClap");
  }
  if (manualConfig.useWave) {
    settingsSetOptions.add("DistanceSet");
  }
  // DistanceUnit možnost: "cm" nebo "inch"
  settingsSetOptions.add("DistanceUnit");
  // Noční režim kontrolních LED
  settingsSetOptions.add("NightKontrolLed");
  settingsSetOptions.add("NightKontrolLedEnable");
  settingsSetOptions.add("NightStartHour");
  settingsSetOptions.add("NightStartMin");
  settingsSetOptions.add("NightEndHour");
  settingsSetOptions.add("NightEndMin");
  // Serializace a odeslání zprávy
  char helpOut[1024];
  serializeJson(helpDoc, helpOut);
  client.publish(SvetloChr, helpOut);  // Odeslání zprávy na MQTT topic
}

void debugMQTT(const String& message) {
  String messageJson = "{\"device\":\"" + String(SvetloChr) + "\",\"message\":\"" + message + "\"}";
  client.publish("logs", messageJson.c_str());
}

void reportFirmwareVersion() {
  String payload = buildModuleVersionsJson();
  // Publikuj do centrálního topicu verzí a současně i na device topic
  // (snazší testování bez nutnosti extra subscribe na "version").
  client.publish("version", payload.c_str(), true);
  client.publish(SvetloChr, payload.c_str());
}

void requestDbStateRestore() {
  if (!awaitingDbStateRestore) {
    return;
  }

  const unsigned long now = millis();
  if (dbSyncStartedAt == 0) {
    dbSyncStartedAt = now;
  }

  if (dbSyncRetryCount >= DB_SYNC_MAX_RETRIES) {
    return;
  }

  if (lastDbSyncRequestAt != 0 && (now - lastDbSyncRequestAt) < DB_SYNC_RETRY_INTERVAL_MS) {
    return;
  }

  DynamicJsonDocument requestDoc(256);
  requestDoc["device"] = String(SvetloChr);
  requestDoc["action"] = "dbSyncRequest";
  requestDoc["message"] = "Zarizeni nacist hodnoty z DB";
  requestDoc["replyTopic"] = String(SvetloChr);

  String requestTopic = String(SvetloChr) + "/db-sync";

  char requestOut[256];
  serializeJson(requestDoc, requestOut);
  client.publish(requestTopic.c_str(), requestOut, false);
  lastDbSyncRequestAt = now;
  dbSyncRetryCount++;
}

bool isAwaitingDbStateRestore() {
  return awaitingDbStateRestore;
}

void reportBoardVersion() {
  StaticJsonDocument<1024> doc;

#if defined(ESP32)
  doc["chip_model"] = ESP.getChipModel();
  doc["chip_rev"] = ESP.getChipRevision();
  doc["mac"] = String((uint64_t)ESP.getEfuseMac(), HEX);
  doc["sdk"] = ESP.getSdkVersion();
  doc["board"] = ARDUINO_BOARD;
  doc["variant"] = ARDUINO_VARIANT;

#elif defined(ESP8266)
  doc["chip_model"] = "ESP8266";
  doc["chip_id"] = ESP.getChipId();
  doc["core"] = ESP.getCoreVersion();
  doc["sdk"] = ESP.getSdkVersion();
  doc["boot_ver"] = ESP.getBootVersion();
#endif

  char buffer[1024];
  serializeJson(doc, buffer);
  client.publish(SvetloChr, buffer);
}