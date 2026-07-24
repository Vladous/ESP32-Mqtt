#include "NetConnect.h"
#include <cstring>

extern void requestDbStateRestore();
extern bool isAwaitingDbStateRestore();

static String pendingMqttDiag = "";
static String mqttClientIdCache = "";

static const String& getMqttClientId() {
  if (mqttClientIdCache.length() == 0) {
    uint64_t chipId = ESP.getEfuseMac();
    uint32_t shortId = static_cast<uint32_t>(chipId & 0xFFFFFF);
    char suffix[7];
    snprintf(suffix, sizeof(suffix), "%06lX", static_cast<unsigned long>(shortId));

    mqttClientIdCache = manualConfig.DeskName + "-" + String(suffix);
    mqttClientIdCache.replace(" ", "_");
  }
  return mqttClientIdCache;
}

static void publishNetworkDiag(const String& message) {
  debugMQTT(message);
}

static const char* mqttStateText(int state) {
  switch (state) {
    case -4: return "MQTT_CONNECTION_TIMEOUT";
    case -3: return "MQTT_CONNECTION_LOST";
    case -2: return "MQTT_CONNECT_FAILED";
    case -1: return "MQTT_DISCONNECTED";
    case 0:  return "MQTT_CONNECTED";
    case 1:  return "MQTT_CONNECT_BAD_PROTOCOL";
    case 2:  return "MQTT_CONNECT_BAD_CLIENT_ID";
    case 3:  return "MQTT_CONNECT_UNAVAILABLE";
    case 4:  return "MQTT_CONNECT_BAD_CREDENTIALS";
    case 5:  return "MQTT_CONNECT_UNAUTHORIZED";
    default: return "MQTT_STATE_UNKNOWN";
  }
}

void updateNetworkStatusLed(int ledWiBrightness) {
  static bool ledWiLatchedOn = false;
  static unsigned long ledWiDisconnectSince = 0;
  static int appliedBrightness = -1;
  const unsigned long ledWiOffDelayMs = 1500;

  bool netFullyConnected = (WiFi.status() == WL_CONNECTED) && client.connected();
  if (netFullyConnected) {
    ledWiLatchedOn = true;
    ledWiDisconnectSince = 0;
  } else {
    if (ledWiDisconnectSince == 0) {
      ledWiDisconnectSince = millis();
    }
    if ((millis() - ledWiDisconnectSince) >= ledWiOffDelayMs) {
      ledWiLatchedOn = false;
    }
  }

  int targetBrightness = ledWiLatchedOn ? ledWiBrightness : 0;
  if (targetBrightness < 0) targetBrightness = 0;
  if (targetBrightness > 255) targetBrightness = 255;

  if (targetBrightness != appliedBrightness) {
    analogWrite(LedWi, targetBrightness);
    appliedBrightness = targetBrightness;
  }
}

void connectToNetwork() {
  static bool hadNetDrop = false;
  static unsigned long netDropStartedAt = 0;
  static unsigned long lastAttemptTime = 0;
  static unsigned long wifiRetryInterval = 5000;
  const unsigned long wifiRetryMin = 5000;
  const unsigned long wifiRetryMax = 60000;

  static bool wifiConnecting = false;
  static unsigned long wifiStartTime = 0;
  const unsigned long wifiTimeout = 45000;

  static unsigned long mqttConnNext = 0;   // kdy zkusit další client.connect

  // Periodický roaming mezi AP se stejným SSID (pokud je nalezen výrazně lepší signál)
  static bool roamScanRunning = false;
  static unsigned long lastRoamScanAt = 0;
  static unsigned long connectedSince = 0;
  const bool enableRoaming = true;
  const unsigned long roamScanIntervalMs = 300000UL;  // 5 min
  const unsigned long roamMinConnectedMs = 180000UL;  // minimálně 3 minuty na aktuálním AP
  const int roamMinImprovementDb = 14;                // přepínat jen při výrazně lepším signálu
  const int roamWeakRssiThreshold = -90;              // scan/přepnutí jen při opravdu slabém signálu

  const unsigned long now = millis();

  bool wifiOkNow = (WiFi.status() == WL_CONNECTED);
  bool mqttOkNow = client.connected();

  if (!(wifiOkNow && mqttOkNow)) {
    if (!hadNetDrop) {
      hadNetDrop = true;
      netDropStartedAt = now;
      int state = client.state();
      pendingMqttDiag = "NET drop: wifi=" + String((int)WiFi.status()) +
                        ", mqttState=" + String(state) + " (" + String(mqttStateText(state)) + ")" +
                        ", rssi=" + String(WiFi.RSSI()) +
                        ", broker=" + String(mqtt_server) + ":" + String(mqtt_port) +
                        ", deviceId=" + String(manualConfig.DeskName.c_str()) +
                        ", clientId=" + getMqttClientId();
    }
  }

  // Watchdog: pokud jsme dlouho offline (WiFi nebo MQTT), restartujeme zařízení
  static unsigned long offlineStart = 0;
  const unsigned long offlineRestartMs = 10UL * 60UL * 1000UL; // 10 minut

  // 0) už jsme OK
  if (WiFi.status() == WL_CONNECTED && client.connected()) {
    IsConnected = true;
    if (hadNetDrop) {
      unsigned long downtimeMs = now - netDropStartedAt;
      String lastDropReason = pendingMqttDiag;
      if (pendingMqttDiag.length() > 0) {
        publishNetworkDiag(pendingMqttDiag);
      }
      pendingMqttDiag = "";
      hadNetDrop = false;
      netDropStartedAt = 0;
    }
    offlineStart = 0; // reset watchdog
    wifiRetryInterval = wifiRetryMin;

    if (connectedSince == 0) {
      connectedSince = now;
    }

    if (enableRoaming) {
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
              debugMQTT("WiFi roaming: RSSI " + String(currentRssi) + " -> " + String(bestRoamRssi) + ", switching AP");
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

      // Spuštění periodického roaming scanu pouze při slabším signálu.
      int currentRssiForScan = WiFi.RSSI();
      if (!wifiConnecting && !roamScanRunning &&
          currentRssiForScan <= roamWeakRssiThreshold &&
          (now - connectedSince) >= roamMinConnectedMs &&
          (now - lastRoamScanAt) >= roamScanIntervalMs) {
        lastRoamScanAt = now;
        WiFi.scanDelete();
        WiFi.scanNetworks(true);  // async
        publishNetworkDiag("Roam scan start: rssi=" + String(currentRssiForScan));
        roamScanRunning = true;
      }
    } else if (roamScanRunning) {
      // Bezpečně ukončit případný rozběhnutý scan po vypnutí roamingu.
      WiFi.scanDelete();
      roamScanRunning = false;
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

    // Start pokusu: přímý reconnect na uložené SSID/password.
    // Vyhýbáme se aktivnímu scanování, které může způsobovat nestabilitu.
    if (!wifiConnecting) {
      lastAttemptTime = now;     // ✅ chybělo v původním kódu
      wifiConnecting = true;
      wifiStartTime = now;
      WiFi.begin(ssid, password);
      return;
    }

    // timeout WiFi pokusu
    if (wifiConnecting && (now - wifiStartTime > wifiTimeout)) {
      wifiConnecting = false;
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

  // 3) WiFi je OK, ale MQTT není připojen -> zkus connect (jednou za 2s)
  if (!client.connected()) {
    if (now < mqttConnNext) return;
    mqttConnNext = now + 2000;  // další pokus za 2s

    if (client.connect(getMqttClientId().c_str())) {
      // Úspěšně připojeno k MQTT
      client.subscribe(SvetloChr);
      client.subscribe(manualConfig.LedBrightnessTopic.c_str());
      IsConnected = true;

      if (isAwaitingDbStateRestore()) {
        requestDbStateRestore();
      }

      Serial.println(WIFI_HOSTNAME);
      Serial.println(manualConfig.DeskName.c_str());

      if (resetReasonMessage.length() > 0) {
        debugMQTT("Restart důvod: " + resetReasonMessage);
        resetReasonMessage = "";
      }
    } else {
      // Připojení selhalo - zkusí se znovu za 2s
      IsConnected = false;

      int state = client.state();
      String msg = "MQTT connect fail: state=" + String(state) +
                   " (" + String(mqttStateText(state)) + ")" +
                   ", wifi=" + String((int)WiFi.status()) +
                   ", rssi=" + String(WiFi.RSSI()) +
                   ", broker=" + String(mqtt_server) + ":" + String(mqtt_port) +
                   ", deviceId=" + String(manualConfig.DeskName.c_str()) +
                   ", clientId=" + getMqttClientId();

      // Pokud zrovna nejsme připojení, odešleme diagnostiku při prvním dalším úspěšném connectu.
      pendingMqttDiag = msg;
    }
  }
}

// Backwards-compatible wrapper used by mqttHandler
void reconnect() {
  connectToNetwork();
}