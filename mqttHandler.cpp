//
// Maximální délka MQTT zprávy 238 b !
//

#include "mqttHandler.h"
#include "config.h"
#include <Preferences.h>

// Globální proměnné (pokud jsou potřeba)
extern Preferences preferences;

// Implementace funkcí
void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  DynamicJsonDocument doc(512);
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
    if (doc.containsKey("settings")) {
      const char* settingAction = doc["settings"];
      if (strcmp(settingAction, "set") == 0) {
        callbackSettingsSet(doc);
      } else if (strcmp(settingAction, "get") == 0) {
        callbackSettingsGet();
      }
    } else if (doc.containsKey("reset")) {
      resetCalibreData();
      resetWifiManager();
      restartDevice();
    } else if (doc.containsKey("boardVersion")) {
      reportBoardVersion();
    } else if (doc.containsKey("restart")) {
      restartDevice();
    } else if (doc.containsKey("help")) {
      sendHelpResponse();
    } else {
      callbackDevice(doc);
    }
  }
}

void callbackSettingsSet(JsonDocument& doc) {
  preferences.begin(PREF_NAMESPACE, false);
  if (doc.containsKey("ClapThreshold")) {
    defaultConfig.ClapThreshold = doc["ClapThreshold"].as<int>();
    preferences.putInt("ClapThreshold", defaultConfig.ClapThreshold);
  }

  if (doc.containsKey("DistanceSet")) {
    defaultConfig.DistanceSet = doc["DistanceSet"].as<int>();
    preferences.putInt("DistanceSet", defaultConfig.DistanceSet);
  }

  // Jednotka vzdálenosti ("cm" nebo "inch")
  if (doc.containsKey("DistanceUnit")) {
    const char* du = doc["DistanceUnit"];
    if (du != nullptr) {
      String unit = String(du);
      if (unit == "cm" || unit == "inch") {
        defaultConfig.DistanceUnit = unit;
        preferences.putString("DistanceUnit", defaultConfig.DistanceUnit);
      }
    }
  }

  if (doc.containsKey("CekejOdeslat")) {
    defaultConfig.CekejOdeslat = doc["CekejOdeslat"].as<float>();
    preferences.putFloat("CekejOdeslat", defaultConfig.CekejOdeslat);
  }
  if (doc.containsKey("CekejMereni")) {
    defaultConfig.CekejMereni = doc["CekejMereni"].as<float>();
    preferences.putFloat("CekejMereni", defaultConfig.CekejMereni);
  }
  if (doc.containsKey("CekejDetectClap")) {
    defaultConfig.CekejDetectClap = doc["CekejDetectClap"].as<int>();
    preferences.putInt("CekejDetectClap", defaultConfig.CekejDetectClap);
  }
  if (doc.containsKey("KalibrT")) {
    defaultConfig.KalibrT = doc["KalibrT"].as<float>();
    preferences.putFloat("KalibrT", defaultConfig.KalibrT);
  }
  if (doc.containsKey("KalibrV")) {
    defaultConfig.KalibrV = doc["KalibrV"].as<float>();
    preferences.putFloat("KalibrV", defaultConfig.KalibrV);
  }
  // Noční režim kontrolních LED
  if (doc.containsKey("NightKontrolLed")) {
    defaultConfig.NightKontrolLed = doc["NightKontrolLed"].as<bool>();
    preferences.putBool("NightKontrolLed", defaultConfig.NightKontrolLed);
  }
  if (doc.containsKey("NightKontrolLedEnable")) {
    defaultConfig.NightKontrolLedEnable = doc["NightKontrolLedEnable"].as<bool>();
    preferences.putBool("NightKontrolLedEnable", defaultConfig.NightKontrolLedEnable);
  }
  if (doc.containsKey("NightStartHour")) {
    defaultConfig.NightStartHour = doc["NightStartHour"].as<int>();
    preferences.putInt("NightStartHour", defaultConfig.NightStartHour);
  }
  if (doc.containsKey("NightStartMin")) {
    defaultConfig.NightStartMin = doc["NightStartMin"].as<int>();
    preferences.putInt("NightStartMin", defaultConfig.NightStartMin);
  }
  if (doc.containsKey("NightEndHour")) {
    defaultConfig.NightEndHour = doc["NightEndHour"].as<int>();
    preferences.putInt("NightEndHour", defaultConfig.NightEndHour);
  }
  if (doc.containsKey("NightEndMin")) {
    defaultConfig.NightEndMin = doc["NightEndMin"].as<int>();
    preferences.putInt("NightEndMin", defaultConfig.NightEndMin);
  }
  // Výstupní jednotka teploty ("C" nebo "F")
  if (doc.containsKey("TempUnit")) {
    const char* tu = doc["TempUnit"];
    if (tu != nullptr) {
      String unit = String(tu);
      if (unit == "C" || unit == "F") {
        defaultConfig.TempUnit = unit;
        preferences.putString("TempUnit", defaultConfig.TempUnit);
      }
    }
  }
  preferences.end();
}

void callbackSettingsGet() {
  DynamicJsonDocument responseDoc(512);
  responseDoc["ip"] = WiFi.localIP().toString();
  responseDoc["host"] = WIFI_HOSTNAME;
  responseDoc["deviceList"] = manualConfig.DeviceType;
  responseDoc["CekejOdeslat"] = defaultConfig.CekejOdeslat;
  if (manualConfig.useTemp || manualConfig.useAmpMeter) {
    responseDoc["CekejMereni"] = defaultConfig.CekejMereni;
    if (manualConfig.useTemp) {
      responseDoc["KalibrT"] = defaultConfig.KalibrT;
      responseDoc["KalibrV"] = defaultConfig.KalibrV;
    }
  }
  if (manualConfig.useClap) {
    responseDoc["ClapThreshold"] = defaultConfig.ClapThreshold;
    responseDoc["CekejDetectClap"] = defaultConfig.CekejDetectClap;
  }
  if (manualConfig.useWave) {
    responseDoc["DistanceSet"] = defaultConfig.DistanceSet;
  }
  // Jednotka vzdálenosti
  responseDoc["DistanceUnit"] = defaultConfig.DistanceUnit;
  responseDoc["TeplotaChip"] = (int)temperatureRead();
  responseDoc["Verze"] = VERSION;
  // Výstupní jednotka teploty
  responseDoc["TempUnit"] = defaultConfig.TempUnit;
  // Noční režim kontrolních LED
  responseDoc["NightKontrolLed"] = defaultConfig.NightKontrolLed;
  responseDoc["NightKontrolLedEnable"] = defaultConfig.NightKontrolLedEnable;
  responseDoc["NightStartHour"] = defaultConfig.NightStartHour;
  responseDoc["NightStartMin"] = defaultConfig.NightStartMin;
  responseDoc["NightEndHour"] = defaultConfig.NightEndHour;
  responseDoc["NightEndMin"] = defaultConfig.NightEndMin;
  char responseOut[512];
  serializeJson(responseDoc, responseOut);

  //Serial.print("Velikost zprávy (strlen): ");
  //Serial.println(strlen(responseOut));
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
    if (doc.containsKey("state")) {
      String state = doc["state"].as<String>();
      led1State = (state == "on");  // Změna stavu pouze pokud je přijat
    }
    if (doc.containsKey("brightness")) {
      led1Brightness = doc["brightness"].as<int>();  // Změna jasu pouze pokud je přijat
    }
  } else if ((deviceName == "LED2") && (manualConfig.DeviceType & LED_WHITE2)) {
    if (doc.containsKey("state")) {
      String state = doc["state"].as<String>();
      led2State = (state == "on");
    }
    if (doc.containsKey("brightness")) {
      led2Brightness = doc["brightness"].as<int>();
    }
  } else if ((deviceName == "LED3") && (manualConfig.DeviceType & LED_WHITE3)) {
    if (doc.containsKey("state")) {
      String state = doc["state"].as<String>();
      led3State = (state == "on");
    }
    if (doc.containsKey("brightness")) {
      led3Brightness = doc["brightness"].as<int>();
    }
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
  reconnect();                                           // Volání funkce pro kontrolu připojení k WiFi a MQTT (WiFi and MQTT connection check)
  DynamicJsonDocument doc(512);                          // Deklarace proměnné doc pro Json (Declaration of doc variable for Json)
  JsonArray devices = doc.createNestedArray("devices");  // Vytvoření pole devices k odeslání (Creating the devices array to send)
  // Kontrola a přidání LED1 (Checking and adding LED1)
  if (manualConfig.DeviceType & LED_WHITE1) {
    JsonObject led1 = devices.createNestedObject();  // Vytvoření Json objektu pro LED_WHITE1 (Creating a Json object for LED_WHITE1)
    led1["device"] = "LED1";                         // Přidání device LED1 do výstupu k odeslání (Adding device LED1 to output to send)
    led1["state"] = led1State ? "on" : "off";        // Přidání state LED1 do výstupu k odeslání (Adding state LED1 to output to send)
    led1["brightness"] = led1Brightness;             // Přidání jasu LED1 do výstupu k odeslání (Adding brightness of LED1 to the output to send)
  }
  // Kontrola a přidání LED2 (Checking and adding LED2)
  if (manualConfig.DeviceType & LED_WHITE2) {
    JsonObject led2 = devices.createNestedObject();  // Vytvoření Json objektu pro LED_WHITE2 (Creating a Json object for LED_WHITE2)
    led2["device"] = "LED2";                         // Přidání device LED2 do výstupu k odeslání (Adding device LED2 to output to send)
    led2["state"] = led2State ? "on" : "off";        // Přidání state LED2 do výstupu k odeslání (Adding state LED2 to output to send)
    led2["brightness"] = led2Brightness;
  }
  // Kontrola a přidání LED3 (Checking and adding LED3)
  if (manualConfig.DeviceType & LED_WHITE3) {
    JsonObject led3 = devices.createNestedObject();  // Vytvoření Json objektu pro LED_WHITE3 (Creating a Json object for LED_WHITE3)
    led3["device"] = "LED3";                         // Přidání device LED3 do výstupu k odeslání (Adding device LED3 to output to send)
    led3["state"] = led3State ? "on" : "off";        // Přidání state LED3 do výstupu k odeslání (Adding state LED3 to output to send)
    led3["brightness"] = led3Brightness;
  }
  // Kontrola a přidání RGB LED (Checking and adding RGB LED)
  if (manualConfig.DeviceType & LED_RGB) {
    JsonObject ledRGB = devices.createNestedObject();               // Vytvoření Json objektu pro RGB LED (Creating a Json object for RGB LED)
    ledRGB["device"] = "RGB";                                       // Přidání device RGB LED do výstupu k odeslání (Adding device RGB LED to output to send)
    ledRGB["state"] = ledRGBState ? "on" : "off";                   // Přidání state RGB LED do výstupu k odeslání (Adding state RGB LED to output to send)
    JsonArray rgbValues = ledRGB.createNestedArray("spectrumRGB");  // Vytvoření Json objektu pro spectrumRGB (Creating a Json object for spectrumRGB)
    rgbValues.add(Red);                                             // Přidání jasu červené do výstupu k odeslání (Adding brightness red to the output to send)
    rgbValues.add(Green);                                           // Přidání jasu zelené do výstupu k odeslání (Adding brightness green to the output to send)
    rgbValues.add(Blue);                                            // Přidání jasu modré do výstupu k odeslání (Adding brightness blue to the output to send)
  }
  // Přidání relé
  if (manualConfig.DeviceType & DEVICE_RELAY) {
    JsonObject relay = devices.createNestedObject();  // Vytvoření Json objektu pro relé (Creating a Json object for the relay)
    relay["device"] = "RELAY";                        // Přidání device relé do výstupu k odeslání (Adding device relay to output to send)
    relay["state"] = relayState ? "on" : "off";       // Přidání state relé do výstupu k odeslání (Adding state relay to output to send)
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
  doc["signal"] = WiFi.RSSI();  // Přidání aktuální síly signálu do výstupu k odeslání (Adding the current signal strength to the output to send)
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
  String payload = "{\"device\":\"" + String(WIFI_HOSTNAME) + "\",\"version\":\"" + String(VERSION) + "\"}";
  client.publish("version", payload.c_str());
}

void reportBoardVersion() {
  StaticJsonDocument<256> doc;

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

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(SvetloChr, buffer);
}