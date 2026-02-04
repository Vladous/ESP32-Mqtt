#include "NetConnect.h"

void connectToNetwork() {
  static unsigned long lastAttemptTime = 0;
  const unsigned long retryInterval = 2000;

  static bool wifiConnecting = false;
  static unsigned long wifiStartTime = 0;
  const unsigned long wifiTimeout = 20000;

  static bool mqttWaiting = false;
  static unsigned long mqttStartTime = 0;
  const unsigned long mqttTimeout = 30000;

  static unsigned long mqttProbeNext = 0;   // kdy zkusit další test brokeru
  static unsigned long mqttConnNext  = 0;   // kdy zkusit další client.connect

#if defined(ESP32)
  // ESP32: async scan (neblokuje)
  static bool scanRunning = false;
  static bool bestBssidValid = false;
  static uint8_t bestBssid[6] = {0};
#endif

  const unsigned long now = millis();

  // Watchdog: pokud jsme dlouho offline (WiFi nebo MQTT), restartujeme zařízení
  static unsigned long offlineStart = 0;
  const unsigned long offlineRestartMs = 5UL * 60UL * 1000UL; // 5 minut

  // 0) už jsme OK
  if (WiFi.status() == WL_CONNECTED && client.connected()) {
    IsConnected = true;
    analogWrite(LedWi, LedL);
    offlineStart = 0; // reset watchdog
    return;
  }

  // zahájit měření doby offline
  if (offlineStart == 0) offlineStart = now;
  else if ((now - offlineStart) > offlineRestartMs) {
    // dlouhodobá neschopnost připojení -> restart
    Serial.println("Network watchdog: restarting device due to prolonged offline state");
    // ukažme to i do MQTT logu pokud to jde
    debugMQTT("Network watchdog: restarting device due to prolonged offline state");
    // reset pomocí restartu
    ESP.restart();
    return;
  }

  // default: nejsme OK
  IsConnected = false;

  // 1) WiFi není připojená -> řeš WiFi (neblokující)
  if (WiFi.status() != WL_CONNECTED) {

    // nový pokus až po intervalu (pokud zrovna neprobíhá connect)
    if (!wifiConnecting && (now - lastAttemptTime < retryInterval)) return;

    // start pokusu (jednou)
    if (!wifiConnecting) {
      lastAttemptTime = now;     // ✅ chybělo v původním kódu
      wifiConnecting = true;
      wifiStartTime = now;

#if defined(ESP32)
      WiFi.scanDelete();
      WiFi.scanNetworks(true);   // async start
      scanRunning = true;
      bestBssidValid = false;
      return;
#else
      // ESP8266: scan může blokovat, takže ho vynecháme = fakt neblokující
      WiFi.begin(ssid, password);
      return;
#endif
    }

#if defined(ESP32)
    // čekej na dokončení async scanu
    if (scanRunning) {
      int n = WiFi.scanComplete();
      if (n == WIFI_SCAN_RUNNING) return;

      scanRunning = false;

      int bestRSSI = -9999;
      bestBssidValid = false;

      for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == ssid) {
          int rssi = WiFi.RSSI(i);
          if (rssi > bestRSSI) {
            bestRSSI = rssi;
            const uint8_t* b = WiFi.BSSID(i);
            if (b) {
              memcpy(bestBssid, b, 6);
              bestBssidValid = true;
            }
          }
        }
      }

      WiFi.scanDelete();

      if (bestBssidValid) WiFi.begin(ssid, password, 0, bestBssid);
      else                WiFi.begin(ssid, password);

      return;
    }
#endif

    // timeout WiFi pokusu
    if (wifiConnecting && (now - wifiStartTime > wifiTimeout)) {
    #if defined(ESP32)
      wifiConnecting = false;
      scanRunning = false;
      // Nevymazávat uložené WiFi údaje — použít disconnect(false)
      WiFi.disconnect(false);
    #else
      wifiConnecting = false;
      WiFi.disconnect(false);
    #endif
    
    }

    return;
  }

  // 2) WiFi připojena -> reset WiFi stavu
  wifiConnecting = false;
#if defined(ESP32)
  scanRunning = false;
#endif

  // 3) čekání na broker (neblokující)
  if (!mqttWaiting) {
    mqttWaiting = true;
    mqttStartTime = now;
    mqttProbeNext = 0;
    mqttConnNext = 0;
  }

  if (mqttWaiting) {
    if (now - mqttStartTime > mqttTimeout) {
      mqttWaiting = false;
      return;
    }

    if (now < mqttProbeNext) return;
    mqttProbeNext = now + 1000;

    WiFiClient testClient;
    testClient.setTimeout(150); // minimalizace blokace uvnitř connect()

    if (testClient.connect(mqtt_server, (uint16_t)atoi(mqtt_port))) {
      testClient.stop();
      mqttWaiting = false;
    } else {
      testClient.stop();
      debugMQTT("🕓 Čekám na spuštění MQTT brokeru...");
      return;
    }
  }

  // 4) broker dostupný -> MQTT connect (jednou za 2s)
  if (!client.connected()) {
    if (now < mqttConnNext) return;
    mqttConnNext = now + 2000;

    if (client.connect(manualConfig.DeskName.c_str())) {
      client.subscribe(SvetloChr);
      client.subscribe(manualConfig.LedBrightnessTopic.c_str());
      IsConnected = true;

#ifdef WIFI_HOSTNAME
      Serial.println(WIFI_HOSTNAME);
#endif
      Serial.println(manualConfig.DeskName.c_str());

      if (resetReasonMessage.length() > 0) {
        debugMQTT("Restart důvod: " + resetReasonMessage);
        resetReasonMessage = "";
      }

      analogWrite(LedWi, LedL);
    } else {
      IsConnected = false;
    }
  }
}

// Backwards-compatible wrapper used by mqttHandler
void reconnect() {
  connectToNetwork();
}