# Parts List for ESP32-MQTT Project

[<img src="https://raw.githubusercontent.com/lipis/flag-icons/main/flags/4x3/cz.svg" width="16"> Česky](./SeznamSoucastek.md) | [<img src="https://raw.githubusercontent.com/lipis/flag-icons/main/flags/4x3/gb.svg" width="16"> English](./Parts-List.md)

## Contents
- [Main Unit](#main-unit)
- [Optional Components](#optional-components)
- [Notes](#notes)

---

## 🔌 Main Unit

### 🧠 Microcontroller
| Component  | Quantity | Description         | Datasheet |
|------------|:--------:|---------------------|:----------|
| ESP32      |    1     | Wemos S2 Mini S2    | [ESP32 datasheet](https://www.wemos.cc/en/latest/s2/s2_mini.html) |

### 💡 LEDs
| Component  | Quantity | Description         | Datasheet |
|------------|:--------:|---------------------|:----------|
| LED        |    1     | Red 5 mm            ||
| LED        |    1     | Blue 5 mm           | [LED 5mm datasheet](https://www.tme.eu/Document/e17ac5a5d91f1e843e5681cfa4a5ba4b/HLMP-HG64-VY0DD.pdf) |
| LED        |    1     | Green 5 mm          ||

### ⚡ Power Supply
| Component       | Quantity | Description                         | Datasheet |
|-----------------|:--------:|-------------------------------------|:----------|
| AC-DC Converter |    1     | 230V to 12V Power Supply Board       | <img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/AC-DC Converter.png" width="50"> |
| Voltage Regulator|    1     | 78S05 +5V 2A                       | [78S05 datasheet](https://www.farnell.com/datasheets/1815527.pdf) |

---

## 🔧 Optional Components

### 🔘 Button
| Component       | Quantity | Description            | Preview |
|-----------------|:--------:|------------------------|:--------|
| Membrane Switch |    1     | Single key             | <img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/MembraneButton.png" width="50"> |

### 🔌 230V Output
| Component       | Quantity | Description                         | Datasheet |
|-----------------|:--------:|-------------------------------------|:----------|
| Relay           |    1     | SRD-12VDC-SL-C Mini Power Relay 10A | [SRD-12VDC-SL-C datasheet](http://www.songlerelay.com/Public/Uploads/20161104/581c81ac16e36.pdf) |
| Optocoupler     |    1     | PC817C                              | [PC817C datasheet](https://www.farnell.com/datasheets/73758.pdf) |
| Terminal Block  |    1     | 3-Pin Universal Screw Terminal 2.54mm PCB | <img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/UniversalniSvorka.png" width="50"> |
| Transistor      |    1     | TIP120 60V, 5A, NPN, TO-220         | [TIP120 datasheet](https://www.farnell.com/datasheets/2877030.pdf) |
| Resistor        |    1     | 10K                                ||

### 💡 12V Output
*Optionally up to 3, i.e., three separate LED lights, RGB, or other 12V devices*
| Component       | Quantity | Description                         | Datasheet |
|-----------------|:--------:|-------------------------------------|:----------|
| Transistor      | 1-3      | TIP120 60V, 5A, NPN, TO-220         | [TIP120 datasheet](https://www.farnell.com/datasheets/2877030.pdf) |
| Resistor        | 1-3      | 10K                                ||
| Terminal Block  |    1     | 4-Pin Universal Screw Terminal 2.54mm PCB | <img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/UniversalniSvorka.png" width="50"> |

### 👏 Clap Control
| Component       | Quantity | Description                         | Datasheet |
|-----------------|:--------:|-------------------------------------|:----------|
| Microphone      |    1     | CZN-15E 2-pin Electret Microphone    | [CZN-15E datasheet](https://eva.fing.edu.uy/file.php/1241/materiales/HD/CZN-15E.pdf) |
| Capacitor       |    1     | Electrolytic 1uF/50V               ||

### ✋ Ovládání mávnutím
| Component       | Quantity | Description                         | Datasheet |
|-----------|:--------:|:-------|:-------|
| Ultrasonic senzor | 1 | HC-SR4 |[HC-SR4 datasheet](www.handsontec.com/dataspecs/HC-SR04-Ultrasonic.pdf)|

### 📊 Power Consumption Measurement
| Component       | Quantity | Description                         | Datasheet |
|-----------------|:--------:|-------------------------------------|:----------|
| Current Sensor  |    1     | ACS712 20A                          ||
| Terminal Block  |    1     | 3-Pin Universal Screw Terminal 2.54mm PCB | <img src="https://github.com/Vladous/ESP32-Mqtt/raw/main/.pictures/UniversalniSvorka.png" width="50"> |

---

## 📝 Notes
- All resistors are 1/4W unless stated otherwise.
- For the 12V output, you can use 1 to 3 sets of transistors and resistors as needed.
- When wiring, pay attention to the correct voltage and current values for individual components.

### ⚠️ Warning: Working with High Voltage
**WARNING:** This project involves working with high voltage (230V AC). Working with high voltage can be extremely dangerous and potentially lethal if not done correctly. If you lack the necessary experience and qualifications, we strongly recommend seeking the help of a qualified electrician or expert.

### 🛑 Disclaimer
The author of this project assumes no responsibility for any damage, injury, or death resulting from the use of this information. Use this information at your own risk. The user assumes full responsibility for ensuring safety while implementing this project and for adhering to all applicable safety regulations and standards.

Before starting work on this project, please thoroughly review all safety guidelines and regulations regarding working with electrical devices and high voltage.
