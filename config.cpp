#include "config.h"

// Definice globálních proměnných
extern DefaultConfig defaultConfig;
extern Preferences preferences;

void loadDefaultConfig() {
  preferences.begin(PREF_NAMESPACE, false);  // Inicializace Preferences s jmenným prostorem

  // Načtení uložených hodnot (Retrieving stored values)
  String savedServer = preferences.getString("mqtt_server", "");
  String savedPort = preferences.getString("mqtt_port", "");
  savedServer.toCharArray(mqtt_server, sizeof(mqtt_server));
  savedPort.toCharArray(mqtt_port, sizeof(mqtt_port));

  if (!preferences.isKey("KalibrT")) {
    preferences.putDouble("KalibrT", 1.33);
  }
  defaultConfig.KalibrT = preferences.getDouble("KalibrT", 1.33);

  if (!preferences.isKey("KalibrV")) {
    preferences.putDouble("KalibrV", 0.70);
  }
  defaultConfig.KalibrV = preferences.getDouble("KalibrV", 0.70);

  if (!preferences.isKey("ClapThreshold")) {
    preferences.putInt("ClapThreshold", 15);
  }
  defaultConfig.ClapThreshold = preferences.getInt("ClapThreshold", 15);

  if (!preferences.isKey("DistanceSet")) {
    preferences.putInt("DistanceSet", 5);
  }
  defaultConfig.DistanceSet = preferences.getInt("DistanceSet", 5);

  if (!preferences.isKey("CekejOdeslat")) {
    preferences.putFloat("CekejOdeslat", 20.0f);
  }
  defaultConfig.CekejOdeslat = preferences.getFloat("CekejOdeslat", 20.0f);

  if (!preferences.isKey("CekejMereni")) {
    preferences.putFloat("CekejMereni", 4.0f);
  }
  defaultConfig.CekejMereni = preferences.getFloat("CekejMereni", 4.0f);

  if (!preferences.isKey("CekejDetectClap")) {
    preferences.putInt("CekejDetectClap", 50);
  }
  defaultConfig.CekejDetectClap = preferences.getInt("CekejDetectClap", 50);

  preferences.end();  // Ukončení práce s Preferences
}