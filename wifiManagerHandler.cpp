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

  // Parametry pro MQTT
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  // WiFiManagerParameter custom_mqtt_username("user", "mqtt user", mqtt_username, 32);
  // WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 32);

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  // wifiManager.addParameter(&custom_mqtt_username);
  // wifiManager.addParameter(&custom_mqtt_password);

  WiFi.setHostname(WIFI_HOSTNAME);
  String apName = String(WIFI_HOSTNAME) + "_AP";

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
    Serial.println("❌ Nepodařilo se připojit - restartuju");
    ESP.restart();
    return false; // Tohle už se neprovede, ale pro jistotu
  }
}

// Funkce na vymazání uložených dat z WiFiManageru
void resetWifiManager() {
  wifiManager.resetSettings();
}