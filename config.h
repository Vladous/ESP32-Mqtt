// Automaticky vygenerovaný config.h pro Test_Board 2025-04-25 16:58
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h> // Přidání závislosti na Preferences

#define DEVICE_NONE   0x00
#define LED_WHITE1    0x01
#define LED_WHITE2    0x02
#define LED_WHITE3    0x04
#define LED_RGB       0x08
#define DEVICE_RELAY  0x10

// Konfigurace nastavení zařízení (Configuration settings device)
struct ManualConfig {    
  const String DeskName = "Tepl_6";                     // !! CHANGE !!  Topic název zařízení (Topic device name)
  const uint8_t DeviceType = DEVICE_NONE;               // !! CHANGE !!  LED_WHITE1 | LED_WHITE2 | LED_WHITE3 | LED_RGB | DEVICE_RELAY
  const bool useTlac = false;                           // !! CHANGE !!  Použití tlačítka (Using the button)
  const uint8_t Stisk = DEVICE_NONE;                    // !! CHANGE !!  Nastavení tlačítka ( LED_WHITE1 | LED_WHITE2 | LED_WHITE3 | LED_RGB | DEVICE_RELAY ) (Button settings)
  const bool useClap = false;                           // !! CHANGE !!  Použití mikrofonu (Using the microphone)
  const bool useWave = false;                           // !! CHANGE !!  Aktivace funkce zapnutí mávnutím (Activation by waving in front of the distance sensor)
  const bool useTemp = true;                            // !! CHANGE !!  Použití DHT sezoru měření teploty (Using a DHT sensor to measure temperature)
  const bool useAmpMeter = false;                       // !! CHANGE !!  Použití měření odběru (Using current draw measurement)
  const bool useLedBrightnessControl = true;            // !! CHANGE !!  Použití ovládání jasu kontrolních led
  const String LedBrightnessTopic = "Led_brightness";
};

// Konfigurace defaultníh hodnot (Configuration defaults variables)
struct DefaultConfig {
  // Noční režim led kontrolek
  bool NightKontrolLed = false;                          // !! CHANGE !!  Noční režim kontrolních led
  bool NightKontrolLedEnable = false;
  int NightStartHour = 21;
  int NightStartMin = 0;
  int NightEndHour = 6;
  int NightEndMin = 0;
  // Kalibrační hodnoty
  float KalibrT = 1.56;     
  float KalibrV = 0.58;     
  int ClapThreshold = 15;                               // Výchozí nastavení hladiny detekce tlesknutí (Default clap detection level setting)
  float CekejOdeslat = 20.0f;                           // Výchozí nastavení prodlevy mezi odesláním dat (Default setting for the delay between sending data)
  float CekejMereni = 2.23f;                            // Výchozí nastavení prodlevy mezi měřením DHT (Default setting for the delay between DHT measurements)
  int CekejDetectClap = 50;                             // Výchozí nastavení prodlevy mezi detekcí tlesknutí (Default setting for the delay between clap detection)
  int DistanceSet = 10;                                 // Nastavení vzdálenosti pro sepnutí
  // Výstupní jednotka vzdálenosti: "cm" nebo "inch"
  String DistanceUnit = "cm";
  // Kalibrace jasu kontrolek
  float KalibrKontrolRed = 4.4f;                        // Výchozí korekce jasu červené led kontrolky
  float KalibrKontrolGreen = 1.0f;                      // Výchozí korekce jasu zelené  led kontrolky
  float KalibrKontrolBlue = 4.4f;                       // Výchozí korekce jasu modré   led kontrolky
  // Výstupní jednotka teploty: "C" nebo "F"
  String TempUnit = "C";
};

// Deklarace externích proměnných
extern Preferences preferences;
extern const char* PREF_NAMESPACE;
extern char mqtt_server[40];
extern char mqtt_port[6];


// Deklarace funkcí
void loadDefaultConfig();
void resetCalibreData();
String deviceList();

#endif