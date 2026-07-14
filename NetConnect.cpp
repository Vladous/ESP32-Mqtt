#include "NetConnect.h"
#include <cstring>

void connectToNetwork() {
  static unsigned long lastAttemptTime = 0;
  static unsigned long wifiRetryInterval = 5000;
  const unsigned long wifiRetryMin = 5000;
  const unsigned long wifiRetryMax = 60000;

  static bool wifiConnecting = false;
  static unsigned long wifiStartTime = 0;
  const unsigned long wifiTimeout = 45000;

  static unsigned long mqttConnNext = 0;   // kdy zkusit další client.connect

  // ESP32: async scan (neblokuje)
  static bool scanRunning = false;
  static bool bestBssidValid = false;
  static unsigned char bestBssid[6] = {0};

  // Periodický roaming mezi AP se stejným SSID (pokud je nalezen výrazně lepší signál)
  static bool roamScanRunning = false;
  static unsigned long lastRoamScanAt = 0;
  static unsigned long connectedSince = 0;
  const unsigned long roamScanIntervalMs = 90000UL;   // 90 s
  const unsigned long roamMinConnectedMs = 120000UL;  // minimálně 2 minuty na aktuálním AP
  const int roamMinImprovementDb = 12;                // přepínat jen při jasně lepším signálu
  const int roamWeakRssiThreshold = -75;              // pokud jsme už slabí, stačí i menší rozdíl

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

    if (connectedSince == 0) {
      connectedSince = now;
    }

    // Dokončení async roaming scanu během normálního provozu.
    if (roamScanRunning) {
      int n = WiFi.scanComplete();
      if (n != WIFI_SCAN_RUNNING) {
        roamScanRunning = false;

        int currentRssi = WiFi.RSSI();
        const uint8_t* currentBssid = WiFi.BSSID();
        bool currentBssidValid = (currentBssid != nullptr);

        int bestRoamRssi = -9999;
        bool bestRoamBssidValid = false;
        uint8_t bestRoamBssid[6] = {0};

        for (int i = 0; i < n; i++) {
          if (WiFi.SSID(i) == ssid) {
            int rssi = WiFi.RSSI(i);
            const uint8_t* b = WiFi.BSSID(i);
            if (b && rssi > bestRoamRssi) {
              bestRoamRssi = rssi;
              memcpy(bestRoamBssid, b, 6);
              bestRoamBssidValid = true;
            }
          }
        }

        if (bestRoamBssidValid) {
          bool differentAp = true;
          if (currentBssidValid) {
            differentAp = (memcmp(bestRoamBssid, currentBssid, 6) != 0);
          }

          int improvement = bestRoamRssi - currentRssi;
          bool strongImprovement = (improvement >= roamMinImprovementDb);
          bool currentlyWeakAndBetter = (currentRssi <= roamWeakRssiThreshold) && (improvement > 0);

          if (differentAp && (strongImprovement || currentlyWeakAndBetter)) {
            Serial.printf("WiFi roaming: RSSI %d -> %d, switching AP\n", currentRssi, bestRoamRssi);
            WiFi.disconnect(false, false);
            WiFi.begin(ssid, password, 0, bestRoamBssid, true);

            wifiConnecting = true;
            wifiStartTime = now;
            lastAttemptTime = now;
            mqttConnNext = 0;
            connectedSince = 0;
          }
        }

        WiFi.scanDelete();
      }
    }

    // Spuštění periodického roaming scanu.
    if (!wifiConnecting && !roamScanRunning && (now - connectedSince) >= roamMinConnectedMs && (now - lastRoamScanAt) >= roamScanIntervalMs) {
      lastRoamScanAt = now;
      WiFi.scanDelete();
      WiFi.scanNetworks(true);  // async
      roamScanRunning = true;
    }

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
  connectedSince = 0;
  roamScanRunning = false;

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

      WiFi.scanDelete();
      WiFi.scanNetworks(true);   // async start
      scanRunning = true;
      bestBssidValid = false;
      return;
    }

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

    // timeout WiFi pokusu
    if (wifiConnecting && (now - wifiStartTime > wifiTimeout)) {
      wifiConnecting = false;
      scanRunning = false;
      roamScanRunning = false;
      // Nevymazávat uložené WiFi údaje — použít disconnect(false)
      WiFi.disconnect(false);

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
  scanRunning = false;

  // 3) WiFi je OK, ale MQTT není připojen -> zkus connect (jednou za 2s)
  if (!client.connected()) {
    if (now < mqttConnNext) return;
    mqttConnNext = now + 2000;  // další pokus za 2s

    if (client.connect(manualConfig.DeskName.c_str())) {
      // Úspěšně připojeno k MQTT
      client.subscribe(SvetloChr);
      client.subscribe(manualConfig.LedBrightnessTopic.c_str());
      IsConnected = true;

      Serial.println(WIFI_HOSTNAME);
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