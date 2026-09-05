# ESP32-Mqtt: Modulove verze a historie zmen

Tento soubor slouzi jako centralni prehled verzi jednotlivych modulu.

## Pravidla verzovani modulu

- Zakladni verze je aktualni hodnota deklarovana v kodu (`MAIN_VERSION` nebo `MODULE_VERSION_*`).
- Pri kazde funkcni zmene modulu zvys verzi o `0.1` (napr. `5.0` -> `5.1`).
- Do historie zapis:
  - datum,
  - novou verzi,
  - kratky a vystizny popis zmeny,
  - volitelne i technicke detaily (duvod, dopad, rizika).
- Pokud jedna zmena zasahne vice modulu, zapis ji do historie kazdeho z nich.

Doporuceny format zapisu:

- `YYYY-MM-DD | X.Y | Strucny popis zmeny`

---

## ESP32-Mqtt.ino

- Aktualni verze: `5.0`
- Historie:
  - `2026-09-05 | 5.0 | Oprava obnovy Wi-Fi po vypadku napajeni: zarizeni neotevira WiFiManager pri docasne nedostupnem routeru a pokracuje v automatickem pripojovani.`
  - `2026-07-14 | 4.9 | Aktualni stav MAIN_VERSION (beze zmeny cisla verze).`
  - `2025-06-14 | 4.9 | Pridani moznosti nastaveni jednotky vzdalenosti pro mavnuti.`
  - `2025-05-14 | 4.8 | Nastaveni vystupu teploty v jednotkach C/F.`
  - `2025-05-14 | 4.7 | Nastaveni prodlevy mezi pokusy o pripojeni Wi-Fi.`
  - `2025-05-14 | 4.6 | Watchdog pro dlouhodobou ztratu Wi-Fi/MQTT.`
  - `2025-05-09 | 4.5 | Kontrola pripojeni brokeru a Wi-Fi v loopu + reconnect.`
  - `2025-05-09 | 4.4 | Oprava kontroly brokeru a navazani po vypadku proudu.`
  - `2025-04-17 | 4.3 | Drobne rozdeleni kodu + bugfix.`
  - `2025-04-16 | 4.2 | Kontrola verze softwaru po startu na topicu version.`
  - `2025-04-16 | 4.1 | Oddeleni WiFi manageru od hlavniho ino.`
  - `2025-04-16 | 4.0 | OTA aktualizace pres Wi-Fi.`
  - `2025-04-15 | 3.5 | Vytvoreni release vetve/stavu.`
  - `2025-01-21 | 3.4 | Oprava casu nocniho rezimu + doplneni verze do settings get.`
  - `2025-01-10 | 3.3 | Rozdeleni kodu na mensi casti.`
  - `2024-12-27 | 3.2 | Zapnuti mavnutim (distance senzor).`
  - `2024-09-28 | 3.1 | Zmena knihovny DHT na variantu pro ESP32.`
  - `2023-08-21 | 3.0 | Uprava na ESP32 Deneyap Mini + rozdeleni svetel a rele.`
  - `2023-05-30 | 2.5 | Prechod na ESP32 + pocatecni WiFi setup pres WiFiManager.`
  - `2023-05-14 | 2.4 | Restart logika pri problemech Wi-Fi/MQTT + mikrofon (tlesknuti).`
  - `2021-11-22 | 2.3 | Json vyber dat.`
  - `2021-11-16 | 2.2 | Json objekt (puvodni zapis 2.3 v komentarich).`
  - `2021-11-16 | 2.1 | Pokus o potlaceni rozkmitani zpetnou vazbou.`
  - `2021-06-12 | 2.0 | Oprava pripojeni po vypadku Wi-Fi + uprava casu stisku.`
  - `2021-04-08 | 1.2 | Delsi stisk + upravy HostName/Svetlo promennych.`
  - `2021-02-14 | 1.1 | Prodlouzeni casu stisknuti.`
  - `2020-03-20 | 1.0 | Prvni verze MQTT IoT klienta.`

## config (config.cpp, config.h)

- Aktualni verze: `4.9`
- Historie:
  - `2026-07-14 | 4.9 | Inicialni zapis podle MODULE_VERSION_CONFIG.`

## deviceControl (deviceControl.cpp, deviceControl.h)

- Aktualni verze: `4.9`
- Historie:
  - `2026-07-14 | 4.9 | Inicialni zapis podle MODULE_VERSION_DEVICE_CONTROL.`

## devicePins (devicePins.cpp, devicePins.h)

- Aktualni verze: `4.9`
- Historie:
  - `2026-07-14 | 4.9 | Inicialni zapis podle MODULE_VERSION_DEVICE_PINS.`

## initSystem (initSystem.cpp, initSystem.h)

- Aktualni verze: `4.9`
- Historie:
  - `2026-07-14 | 4.9 | Inicialni zapis podle MODULE_VERSION_INIT_SYSTEM.`

## lightControl (lightControl.cpp, lightControl.h)

- Aktualni verze: `4.9`
- Historie:
  - `2026-07-14 | 4.9 | Inicialni zapis podle MODULE_VERSION_LIGHT_CONTROL.`

## mqttHandler (mqttHandler.cpp, mqttHandler.h)

- Aktualni verze: `5.1`
- Historie:
  - `2026-07-21 | 5.1 | Doplneno chytre vyhlazovani RSSI pri telemetrii a stabilizace signalu bez zmeny topicu nebo payloadu.`
  - `2026-07-17 | 5.0 | Pridano rizeni DB sync po restartu (request, retry, timeout, fallback no-data/no-response) a ochrana proti predcasnemu prepisu stavu.`
  - `2026-07-14 | 4.9 | Inicialni zapis podle MODULE_VERSION_MQTT_HANDLER.`

## NetConnect (NetConnect.cpp, NetConnect.h)

- Aktualni verze: `5.5`
- Historie:
  - `2026-09-05 | 5.5 | Ochrana proti pripojovani s prazdnymi Wi-Fi udaji a pravidelne hledani silnejsiho AP se stejnym SSID.`
  - `2026-07-21 | 5.4 | Obnoven rizeny roaming jen pri opravdu slabem signalu s delsim intervalem scanu a prisnejsimi podminkami pro prepinani AP.`
  - `2026-07-17 | 5.3 | Presun rizeni modre sitove LED z hlavniho ino do NetConnect a pridani zpozdeneho zhasnuti, aby kratke WiFi/MQTT reconnecty nezpusobovaly probliky.`
  - `2026-07-17 | 5.2 | Po MQTT connectu vyvolani requestu na DB sync obnovu stavu, pokud zarizeni ceka na inicialni data.`
  - `2026-07-14 | 5.1 | Prubezna kontrola silnejsiho AP a podminky pro bezpecne prepnuti (lokalni zmena, zatim bez samostatneho commitu verze v .h).`
  - `2026-07-08 | 5.0 | Oprava pripadu vypadku energie (git commit 41f0f39, 4.9 -> 5.0).`
  - `2026-07-08 | 4.9 | Zavedeni moduloveho verze v NetConnect.h (git commit e5b8a39).`

## ota (ota.cpp, ota.h)

- Aktualni verze: `4.0`
- Historie:
  - `2026-07-14 | 4.0 | Inicialni zapis podle MODULE_VERSION_OTA.`

## sensors (sensors.cpp, sensors.h)

- Aktualni verze: `5.0`
- Historie:
  - `2026-07-21 | 5.0 | Chytre vyrovnavani teploty a vlhkosti: adaptivni EMA filtr s omezenim skoku pro stabilnejsi telemetrii.`
  - `2026-07-14 | 4.9 | Inicialni zapis podle MODULE_VERSION_SENSORS.`

## stateControl (stateControl.cpp, stateControl.h)

- Aktualni verze: `4.9`
- Historie:
  - `2026-07-14 | 4.9 | Inicialni zapis podle MODULE_VERSION_STATE_CONTROL.`

## wifiManagerHandler (wifiManagerHandler.cpp, wifiManagerHandler.h)

- Aktualni verze: `5.1`
- Historie:
  - `2026-09-05 | 5.1 | WiFiManager se spousti pouze pri chybejicich ulozenych credentials; docasny vypadek routeru se resi opakovanym pripojovanim.`
  - `2026-07-14 | 5.0 | Aktualni stav dle MODULE_VERSION_WIFI_MANAGER.`
  - `2026-07-08 | 5.0 | Oprava pripadu vypadku energie (git commit 41f0f39, 4.9 -> 5.0).`
  - `2026-07-08 | 4.9 | Zavedeni modulove verze v wifiManagerHandler.h (git commit e5b8a39).`

---

## Poznamka k historii

Historie je kombinovana ze 2 zdroju:

- explicitni verze a komentare v `ESP32-Mqtt.ino`,
- dohledatelne zmeny v Git commitech (hlavne pro `NetConnect` a `wifiManagerHandler`).

U modulu, kde v historii zatim chybi starsi verze, zustava zatim jen aktualni stav.
