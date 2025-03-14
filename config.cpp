#include "config.h"

// Definice globálních proměnných
extern DefaultConfig defaultConfig;
extern Preferences preferences;
extern void debugMQTT(const String& message);

void loadDefaultConfig() {
  preferences.begin(PREF_NAMESPACE, false);  // Inicializace Preferences s jmenným prostorem


  // Načtení uložených hodnot (Retrieving stored values)
  String savedServer = preferences.getString("mqtt_server", "");
  String savedPort = preferences.getString("mqtt_port", "");
  savedServer.toCharArray(mqtt_server, sizeof(mqtt_server));
  savedPort.toCharArray(mqtt_port, sizeof(mqtt_port));
 
  if (!preferences.isKey("KalibrT")) {
    preferences.putFloat("KalibrT", defaultConfig.KalibrT );
  }
  defaultConfig.KalibrT = preferences.getFloat("KalibrT", defaultConfig.KalibrT);
  
  if (!preferences.isKey("KalibrV")) {
    preferences.putFloat("KalibrV", defaultConfig.KalibrV );
  }
  defaultConfig.KalibrV = preferences.getFloat("KalibrV", defaultConfig.KalibrV );

  if (!preferences.isKey("ClapThreshold")) {
    preferences.putInt("ClapThreshold", defaultConfig.ClapThreshold );
  }
  defaultConfig.ClapThreshold = preferences.getInt("ClapThreshold", defaultConfig.ClapThreshold);

  if (!preferences.isKey("DistanceSet")) {
    preferences.putInt("DistanceSet", defaultConfig.DistanceSet);
  }
  defaultConfig.DistanceSet = preferences.getInt("DistanceSet", defaultConfig.DistanceSet);

  if (!preferences.isKey("CekejOdeslat")) {
    preferences.putFloat("CekejOdeslat", defaultConfig.CekejOdeslat);
  }
  defaultConfig.CekejOdeslat = preferences.getFloat("CekejOdeslat", defaultConfig.CekejOdeslat);

  if (!preferences.isKey("CekejMereni")) {
    preferences.putFloat("CekejMereni", defaultConfig.CekejMereni);
  }
  defaultConfig.CekejMereni = preferences.getFloat("CekejMereni", defaultConfig.CekejMereni);

  if (!preferences.isKey("CekejDetectClap")) {
    preferences.putInt("CekejDetectClap", defaultConfig.CekejDetectClap);
  }
  defaultConfig.CekejDetectClap = preferences.getInt("CekejDetectClap", defaultConfig.CekejDetectClap);

  preferences.end();  // Ukončení práce s Preferences
}