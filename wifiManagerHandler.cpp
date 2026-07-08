#include <WiFiManager.h>
#include <Preferences.h>
#include "wifiManagerHandler.h"
#include "mqttHandler.h"    // kvůli WIFI_HOSTNAME a MQTT param
#include "config.h"         // kvůli globálním proměnným jako mqtt_server, mqtt_port, atd.

// Externí proměnné
extern char ssid[32];
extern char password[64];
extern char mqtt_server[40];
extern char mqtt_port[6];
// extern char mqtt_username[32];
// extern char mqtt_password[32];

WiFiManager wifiManager;

bool setupWiFi() {
  WiFi.setHostname(WIFI_HOSTNAME);

  if (WiFi.isConnected()) {
    // WiFi už je připojeno, načti uložené MQTT údaje z Preferences
    Preferences preferences;
    preferences.begin(PREF_NAMESPACE, true); // pouze pro čtení
    String savedServer = preferences.getString("mqtt_server", "");
    String savedPort = preferences.getString("mqtt_port", "");
    preferences.end();

    savedServer.toCharArray(mqtt_server, sizeof(mqtt_server));
    savedPort.toCharArray(mqtt_port, sizeof(mqtt_port));

    return true;
  }

  // 1) Nejprve zkoušej připojení na již uložené WiFi údaje (STA) s postupným zpomalováním.
  //    Teprve po delší době bez úspěchu přepneme do WiFiManager portálu.
  const unsigned long stationTryWindowMs = 180000UL;   // 3 minuty
  const unsigned long backoffStartMs = 4000UL;
  const unsigned long backoffMaxMs = 30000UL;

  unsigned long backoffMs = backoffStartMs;
  unsigned long startedAt = millis();

  WiFi.mode(WIFI_STA);
  WiFi.begin();  // použije uložené credentials z NVS

  while (WiFi.status() != WL_CONNECTED) {
    if ((millis() - startedAt) >= stationTryWindowMs) {
      break;
    }

    Serial.print("WiFi reconnect in ");
    Serial.print(backoffMs / 1000);
    Serial.println("s...");
    delay(backoffMs);

    if (WiFi.status() == WL_CONNECTED) {
      break;
    }

    WiFi.disconnect(false);
    WiFi.begin();

    if (backoffMs < backoffMaxMs) {
      backoffMs = min(backoffMs * 2, backoffMaxMs);
    }
  }

  if (WiFi.isConnected()) {
    // Připojeno přes uložené údaje, WiFiManager není potřeba.
    strcpy(ssid, WiFi.SSID().c_str());
    strcpy(password, WiFi.psk().c_str());

    Preferences preferences;
    preferences.begin(PREF_NAMESPACE, true); // pouze pro čtení
    String savedServer = preferences.getString("mqtt_server", "");
    String savedPort = preferences.getString("mqtt_port", "");
    preferences.end();

    savedServer.toCharArray(mqtt_server, sizeof(mqtt_server));
    savedPort.toCharArray(mqtt_port, sizeof(mqtt_port));

    return true;
  }

  // Parametry pro MQTT
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  // WiFiManagerParameter custom_mqtt_username("user", "mqtt user", mqtt_username, 32);
  // WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 32);

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  // wifiManager.addParameter(&custom_mqtt_username);
  // wifiManager.addParameter(&custom_mqtt_password);

  String apName = String(WIFI_HOSTNAME) + "_AP";
  wifiManager.setConfigPortalTimeout(300); // 5 minut na ruční zásah

  if (wifiManager.autoConnect(apName.c_str())) {
    // Připojeno OK → načti hodnoty
    strcpy(ssid, WiFi.SSID().c_str());
    strcpy(password, WiFi.psk().c_str());
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    // strcpy(mqtt_username, custom_mqtt_username.getValue());
    // strcpy(mqtt_password, custom_mqtt_password.getValue());

    // Ulož do Preferences
    Preferences preferences;
    preferences.begin(PREF_NAMESPACE, false);
    preferences.putString("mqtt_server", mqtt_server);
    preferences.putString("mqtt_port", mqtt_port);
    preferences.end();

    return true;
  } else {
    Serial.println("❌ Nepodařilo se připojit ve WiFiManageru");
    return false;
  }
}

// Funkce na vymazání uložených dat z WiFiManageru
void resetWifiManager() {
  wifiManager.resetSettings();
}