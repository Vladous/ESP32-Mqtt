#include "config.h"

// Definice globálních proměnných
extern DefaultConfig defaultConfig;
extern Preferences preferences;
extern void debugMQTT(const String& message);
extern ManualConfig manualConfig;

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

  // Načtení nastavení nočního režimu kontrolních LED
  if (!preferences.isKey("NightKontrolLed")) {
    preferences.putBool("NightKontrolLed", defaultConfig.NightKontrolLed);
  }
  defaultConfig.NightKontrolLed = preferences.getBool("NightKontrolLed", defaultConfig.NightKontrolLed);

  if (!preferences.isKey("NightKontrolLedEnable")) {
    preferences.putBool("NightKontrolLedEnable", defaultConfig.NightKontrolLedEnable);
  }
  defaultConfig.NightKontrolLedEnable = preferences.getBool("NightKontrolLedEnable", defaultConfig.NightKontrolLedEnable);

  if (!preferences.isKey("NightStartHour")) {
    preferences.putInt("NightStartHour", defaultConfig.NightStartHour);
  }
  defaultConfig.NightStartHour = preferences.getInt("NightStartHour", defaultConfig.NightStartHour);

  if (!preferences.isKey("NightStartMin")) {
    preferences.putInt("NightStartMin", defaultConfig.NightStartMin);
  }
  defaultConfig.NightStartMin = preferences.getInt("NightStartMin", defaultConfig.NightStartMin);

  if (!preferences.isKey("NightEndHour")) {
    preferences.putInt("NightEndHour", defaultConfig.NightEndHour);
  }
  defaultConfig.NightEndHour = preferences.getInt("NightEndHour", defaultConfig.NightEndHour);

  if (!preferences.isKey("NightEndMin")) {
    preferences.putInt("NightEndMin", defaultConfig.NightEndMin);
  }
  defaultConfig.NightEndMin = preferences.getInt("NightEndMin", defaultConfig.NightEndMin);

  // TempUnit (výstupní jednotka teploty)
  if (!preferences.isKey("TempUnit")) {
    preferences.putString("TempUnit", defaultConfig.TempUnit);
  }
  defaultConfig.TempUnit = preferences.getString("TempUnit", defaultConfig.TempUnit);

  // DistanceUnit (výstupní jednotka vzdálenosti)
  if (!preferences.isKey("DistanceUnit")) {
    preferences.putString("DistanceUnit", defaultConfig.DistanceUnit);
  }
  defaultConfig.DistanceUnit = preferences.getString("DistanceUnit", defaultConfig.DistanceUnit);

  preferences.end();  // Ukončení práce s Preferences
}

// Funkce na vymazání uložených dat z EEPROM (Function to erase stored data from EEPROM)
void resetCalibreData() {
 preferences.begin(PREF_NAMESPACE, false);
  // Vymažte všechny záznamy v Preferences
  preferences.clear();
  preferences.end();
}

String deviceList() {
  String result = "";
  if (manualConfig.DeviceType != DEVICE_NONE) {
    result += "DeviceType:" + String(manualConfig.DeviceType) + " ";
  }
  if (manualConfig.useTlac) {
    result += "tlacitko ";
  }
  if (manualConfig.Stisk != DEVICE_NONE) {
    result += "Stisk:" + String(manualConfig.Stisk) + " ";
  }
  if (manualConfig.useClap) {
    result += "tlesknuti ";
  }
  if (manualConfig.useWave) {
    result += "mavnutim ";
  }
  if (manualConfig.useTemp) {
    result += "teplomer ";
  }
  if (manualConfig.useAmpMeter) {
    result += "proudovy_senzor ";
  }
  if (manualConfig.useLedBrightnessControl) {
    result += "jas_kontrolky ";
  }
  if (defaultConfig.NightKontrolLed) {
    result += "noční_režim_kontrolky od " + String(defaultConfig.NightStartHour) + ":" + String(defaultConfig.NightStartMin) + " do " + String(defaultConfig.NightEndHour) + ":" + String(defaultConfig.NightEndMin) + " ";
  }
  return result;
}