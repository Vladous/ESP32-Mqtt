#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h> // Přidání závislosti na Preferences

#define LED_WHITE1    0x01
#define LED_WHITE2    0x02
#define LED_WHITE3    0x04
#define LED_RGB       0x08
#define DEVICE_RELAY  0x10

// Konfigurace nastavení zařízení (Configuration settings device)
struct ManualConfig {    
  const String DeskName = "Test_Board";                 // !! CHANGE !!  Topic název zařízení (Topic device name)
  const uint8_t DeviceType = LED_WHITE1;                // !! CHANGE !!  LED_WHITE1 | LED_WHITE2 | LED_WHITE3 | LED_RGB | DEVICE_RELAY
  const bool useTlac = true;                            // !! CHANGE !!  Použití tlačítka (Using the button)
  const uint8_t Stisk = LED_WHITE1;                     // !! CHANGE !!  Nastavení tlačítka ( LED_WHITE1 | LED_WHITE2 | LED_WHITE3 | LED_RGB | DEVICE_RELAY ) (Button settings)
  const bool useClap = false;                           // !! CHANGE !!  Použití mikrofonu (Using the microphone)
  const bool useWave = false;                            // !! CHANGE !!  Aktivace funkce zapnutí mávnutím (Activation by waving in front of the distance sensor)
  const bool useTemp = true;                            // !! CHANGE !!  Použití DHT sezoru měření teploty (Using a DHT sensor to measure temperature)
  const bool useAmpMeter = false;                       // !! CHANGE !!  Použití měření odběru (Using current draw measurement)
  const bool useLedBrightnessControl = true;            // !! CHANGE !!  Použití ovládání jasu kontrolních led
  const String LedBrightnessTopic = "Led_brightness";   // !! CHANGE !!  Topic ovládání jasu kontrolních led
};

// Konfigurace defaultníh hodnot (Configuration defaults variables)
struct DefaultConfig {
  float KalibrT = 1.33;     
  float KalibrV = 0.70;     
  int ClapThreshold = 15;                               // Výchozí nastavení hladiny detekce tlesknutí (Default clap detection level setting)
  float CekejOdeslat = 20.0f;                           // Výchozí nastavení prodlevy mezi odesláním dat (Default setting for the delay between sending data)
  float CekejMereni = 4.0f;                             // Výchozí nastavení prodlevy mezi měřením DHT (Default setting for the delay between DHT measurements)
  int CekejDetectClap = 50;                             // Výchozí nastavení prodlevy mezi detekcí tlesknutí (Default setting for the delay between clap detection)
  int DistanceSet = 10;                                 // Nastavení vzdálenosti pro sepnutí
  // Kalibrace jasu kontrolek
  float KalibrKontrolRed = 4.8f;
  float KalibrKontrolGreen = 1.0f;
  float KalibrKontrolBlue = 4.8f;
  // Noční režim led kontrolek
  bool NightKontrolLed = false;
  bool NightKontrolLedEnable = false;
  int NightStartHour = 21;
  int NightStartMin = 0;
  int NightEndHour = 6;
  int NightEndMin = 0;
};

// Deklarace externích proměnných
extern DefaultConfig defaultConfig;
extern Preferences preferences;
extern const char* PREF_NAMESPACE;
extern char mqtt_server[40];
extern char mqtt_port[6];


// Deklarace funkcí
void loadDefaultConfig();

#endif