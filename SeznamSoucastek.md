# Seznam součástek pro ESP32-MQTT projekt

[<img src="https://raw.githubusercontent.com/lipis/flag-icons/main/flags/4x3/cz.svg" width="16"> Česky](./SeznamSoucastek.md) | [<img src="https://raw.githubusercontent.com/lipis/flag-icons/main/flags/4x3/gb.svg" width="16"> English](./Parts-List.md)

## Obsah
- [Základní jednotka](#základní-jednotka)
- [Volitelné součásti](#volitelné-součásti)
- [Poznámky](#poznámky)

---

## 🔌 Základní jednotka

### 🧠 Mikrokontrolér
| Součástka | Množství |Popis| Datasheet |
|-----------|:--------:|:-------------|:---------------------|
| ESP32     |    1     | Wemos S2 Mini S2| [ESP32 datasheet](https://www.wemos.cc/en/latest/s2/s2_mini.html) |

### 💡 LED diody
| Součástka | Množství | Popis | Datasheet |
|-----------|:--------:|:------------|:------------|
| LED dioda |    1     | Červená 5 mm ||
| LED dioda |    1     | Modrá 5 mm |[LED 5mm datasheet](https://www.tme.eu/Document/e17ac5a5d91f1e843e5681cfa4a5ba4b/HLMP-HG64-VY0DD.pdf)|
| LED dioda |    1     | Zelená 5 mm ||

### ⚡ Napájení
| Součástka | Množství | Popis | Datasheet |
|-----------|:--------:|:-------|:-------|
| AC-DC Converter | 1 | 230V na 12V Power Supply Board |<img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/AC-DC Converter.png" width="50"> |
| Stabilizátor napětí | 1 | 78S05 +5V 2A |[78S05 datasheet](https://www.farnell.com/datasheets/1815527.pdf)|

---

## 🔧 Volitelné součásti

### 🔘 Tlačítko
| Součástka | Množství | Popis | Náhled |
|-----------|:--------:|:------|:------|
| Membránový spínač | 1 | 1 klávesa | <img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/MembraneButton.png" width="50"> |

### 🔌 Výstup 230V
| Součástka | Množství | Popis | Datasheet |
|-----------|:--------:|:-------|:-------|
| Relé | 1 | SRD-12VDC-SL-C Mini výkonové relé 10A |[SRD-12VDC-SL-C datasheet](http://www.songlerelay.com/Public/Uploads/20161104/581c81ac16e36.pdf)|
| Optočlen | 1 | PC817C |[PC817C datasheet](https://www.farnell.com/datasheets/73758.pdf)|
| Svorkovnice | 1 | 3Pin Universální šroubovací svorka 2.54mm PCB | <img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/UniversalniSvorka.png" width="50"> |
| Tranzistor | 1 | TIP120 60V, 5A, NPN, TO-220 |[TIP120 datasheet](https://www.farnell.com/datasheets/2877030.pdf)|
| Rezistor | 1 | 10K |

### 💡 Výstup 12V 
*Možnost až 3. Tedy tři oddělená led světla, RGB, nebo jiné zařízení na 12V*
| Součástka | Množství | Popis | Datasheet |
|-----------|:--------:|:-------|:-------|
| Tranzistor | 1-3 | TIP120 60V, 5A, NPN, TO-220 |[TIP120 datasheet](https://www.farnell.com/datasheets/2877030.pdf)|
| Rezistor | 1-3 | 10K ||
| Svorkovnice | 1 | 4Pin Universální šroubovací svorka 2.54mm PCB | <img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/UniversalniSvorka.png" width="50"> |

### 👏 Ovládání tlesknutím
| Součástka | Množství | Popis | Datasheet |
|-----------|:--------:|:-------|:-------|
| Mikrofon | 1 | CZN-15E 2pin kondenzátorový mikrofon |[CZN-15E datasheet](https://eva.fing.edu.uy/file.php/1241/materiales/HD/CZN-15E.pdf)|
| Kondenzátor | 1 | Elektrolitický 1uF/50V ||

### ✋ Ovládání mávnutím
| Součástka | Množství | Popis | Datasheet |
|-----------|:--------:|:-------|:-------|
| Senzor vzdálenosti | 1 | HC-SR4 |[HC-SR4 datasheet](www.handsontec.com/dataspecs/HC-SR04-Ultrasonic.pdf)|

### 📊 Měření odběru
| Součástka | Množství | Popis | Datasheet |
|-----------|:--------:|:-------|:-------|
| Proudový senzor | 1 | ACS712 20A |
| Svorkovnice | 1 | 3Pin Universální šroubovací svorka 2.54mm PCB | <img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/UniversalniSvorka.png" width="50"> |
---

## 📝 Poznámky
- Všechny rezistory jsou 1/4W, pokud není uvedeno jinak.
- Pro výstup 12V je možné použít 1 až 3 sady tranzistorů a rezistorů podle potřeby.
- Při zapojování dbejte na správné hodnoty napětí a proudu pro jednotlivé komponenty.

### ⚠️ Varování: Práce s vysokým napětím
**POZOR:** Tento projekt zahrnuje práci s vysokým napětím (230V AC). Práce s vysokým napětím může být extrémně nebezpečná a potenciálně smrtelná, pokud není prováděna správně. Pokud nemáte odpovídající zkušenosti a kvalifikaci, důrazně doporučujeme vyhledat pomoc kvalifikovaného elektrikáře nebo odborníka.

### 🛑 Zřeknutí se odpovědnosti
Autor tohoto projektu nenese žádnou odpovědnost za jakékoli škody, zranění nebo úmrtí způsobené použitím těchto informací. Použití těchto informací je na vlastní nebezpečí. Uživatel přebírá plnou odpovědnost za zajištění bezpečnosti při realizaci tohoto projektu a dodržování všech příslušných bezpečnostních předpisů a norem.

Před zahájením práce na tomto projektu si prosím důkladně prostudujte všechny bezpečnostní pokyny a předpisy týkající se práce s elektrickými zařízeními a vysokým napětím.
