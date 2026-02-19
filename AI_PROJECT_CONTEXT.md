# AI Project Context (ESP32-Mqtt)

Tento soubor je **trvalý projektový kontext** pro AI asistenci.
Je určený pro opakované použití nad celým projektem, ne pro jeden konkrétní úkol.

---

## 1) Projekt v kostce
- **Název:** ESP32-Mqtt
- **Účel:** ESP32 zařízení řízené přes MQTT (stavy, senzory, ovládání relé/světel, OTA)
- **HW:** ESP32-WROOM
- **Build systém:** PlatformIO
- **Framework:** Arduino

## 2) Architektura a moduly
- **Main entrypoint:** `ESP32-Mqtt.ino`
- **Síť / Wi-Fi:** `NetConnect.*`, `wifiManagerHandler.*`
- **MQTT vrstva:** `mqttHandler.*`
- **Logika stavů:** `stateControl.*`
- **Ovládání zařízení:** `deviceControl.*`, `lightControl.*`, `devicePins.*`
- **Senzory:** `sensors.*`
- **Inicializace systému:** `initSystem.*`
- **OTA update:** `ota.*`
- **Konfigurace:** `config.*`

## 3) Trvalá pravidla pro změny
- Udržovat změny minimální a cílené.
- Nerefaktorovat nesouvisející části.
- Neměnit veřejné chování/API bez jasného důvodu.
- Držet styl existujícího kódu.
- Nepřidávat nové knihovny, pokud není nutné.

## 4) Standardní ověření po úpravách
- **Build:** `pio run`
- **Upload:** `pio run -t upload`
- **Monitor:** `pio device monitor`
- **Kontrola funkce (dle typu změny):**
  - MQTT connect/reconnect
  - publikace/odběr relevantních topiců
  - reakce zařízení (relé/světla)
  - OTA chování (pokud relevantní)

## 5) Jak má AI odpovídat
- Stručně shrnout, co změnila a proč.
- Vypsat upravené soubory.
- Uvést, co bylo ověřeno (build/test) a výsledek.
- Uvést rizika nebo otevřené body.

## 6) Potvrzené projektové požadavky (globální)

Tato sekce je **živý seznam** rozhodnutí, která platí napříč projektem.
Jakmile uživatel požadavek výslovně potvrdí (např. „ano, tak to chci“), má se přiměřeně promítnout sem i do kódu.

### Pravidla údržby
- Po potvrzení přidat/aktualizovat položku v této sekci.
- Držet formulace stručné, testovatelné a dlouhodobě platné.
- Pokud nové rozhodnutí mění staré, staré nahradit (ne duplikovat).
- Při implementaci vždy ověřit i dříve potvrzené globální požadavky, pokud s nimi změna souvisí.

### Aktivní požadavky
- _(zatím žádné trvale potvrzené požadavky)_
- **Příklad formulace:** „Systém musí průběžně kontrolovat dostupnost Wi-Fi a při výpadku provést bezpečný reconnect bez blokování hlavní smyčky.“

## Mini-prompt (projektový)

> Pracuj v projektu ESP32-Mqtt podle pravidel v tomto souboru. Nejdřív rychle zkontroluj relevantní moduly, pak navrhni a proveď minimální potřebné změny. Pokud v konverzaci potvrdím nové globální pravidlo, přiměřeně ho doplň i do sekce „Potvrzené projektové požadavky“. Nakonec napiš stručné shrnutí, seznam změněných souborů a výsledek ověření.
