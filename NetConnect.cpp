#include "NetConnect.h"

void connectToNetwork() {
  static unsigned long lastAttemptTime = 0;
  static unsigned long wifiRetryInterval = 5000;
  const unsigned long wifiRetryMin = 5000;
  const unsigned long wifiRetryMax = 60000;

  static bool wifiConnecting = false;
  static unsigned long wifiStartTime = 0;
  const unsigned long wifiTimeout = 45000;

  static unsigned long mqttConnNext = 0;   // kdy zkusit další client.connect

#if defined(ESP32)
  // ESP32: async scan (neblokuje)
  static bool scanRunning = false;
  static bool bestBssidValid = false;
  static unsigned char bestBssid[6] = {0};
#endif

  const unsigned long now = millis();

  // Watchdog: pokud jsme dlouho offline (WiFi nebo MQTT), restartujeme zařízení
  static unsigned long offlineStart = 0;
  const unsigned long offlineRestartMs = 10UL * 60UL * 1000UL; // 10 minut

  // 0) už jsme OK
  if (WiFi.status() == WL_CONNECTED && client.connected()) {
    IsConnected = true;
    analogWrite(LedWi, LedL);
    offlineStart = 0; // reset watchdog
    wifiRetryInterval = wifiRetryMin;
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
    // Reset MQTT stavu když WiFi vypadne
    mqttConnNext = 0;

    // nový pokus až po intervalu (pokud zrovna neprobíhá connect)
    if (!wifiConnecting && (now - lastAttemptTime < wifiRetryInterval)) return;

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
            const auto* b = WiFi.BSSID(i);
            if (b) {
              for (int j = 0; j < 6; j++) {
                bestBssid[j] = b[j];
              }
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

      if (wifiRetryInterval < wifiRetryMax) {
        unsigned long nextRetryInterval = wifiRetryInterval * 2;
        wifiRetryInterval = (nextRetryInterval > wifiRetryMax) ? wifiRetryMax : nextRetryInterval;
      }
    
    }

    return;
  }

  // 2) WiFi připojena -> reset WiFi stavu
  wifiConnecting = false;
  wifiRetryInterval = wifiRetryMin;
#if defined(ESP32)
  scanRunning = false;
#endif

  // 3) WiFi je OK, ale MQTT není připojen -> zkus connect (jednou za 2s)
  if (!client.connected()) {
    if (now < mqttConnNext) return;
    mqttConnNext = now + 2000;  // další pokus za 2s

    if (client.connect(manualConfig.DeskName.c_str())) {
      // Úspěšně připojeno k MQTT
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
      // Připojení selhalo - zkusí se znovu za 2s
      IsConnected = false;
    }
  }
}

// Backwards-compatible wrapper used by mqttHandler
void reconnect() {
  connectToNetwork();
}