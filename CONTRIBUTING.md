# Mitwirken / Contributing – ESP32-Station-Clock (uhr4)

*English version below.*

---

Danke für dein Interesse an diesem Projekt! Es ist ein Hobbyprojekt, das in der Freizeit gepflegt wird – bitte halte
Pull Requests deshalb fokussiert und möglichst klein, dann lassen sie sich viel leichter prüfen und übernehmen.

## Hardware und Build-Umgebung

- Ziel: nur **ESP32-S2** (wenig RAM – siehe „Speichergrenzen“ unten)
- Board: **LOLIN S2 PICO** (esp32-Arduino-Core, gebaut mit 3.3.12), Partition Scheme
  **„No OTA (2MB APP/2MB SPIFFS)“**, USB CDC On Boot „Enabled“. PSRAM ist bei diesem Board immer an
  (`BOARD_HAS_PSRAM` in der Board-Definition, kein Menüpunkt).
- Der Compiler-Schalter `-mtext-section-literals` ist nötig: Funktion und Literal sollen auch in den großen
  Quelldateien (`display.cpp`, `webserver_routes.cpp`) nebeneinander liegen, sonst bricht der Linker mit „dangerous
  relocation: l32r: literal target out of range“ ab. Visual Micro liest ihn aus
  `board.txt` im Projektordner; für Arduino IDE/arduino-cli `platform.local.txt` aus dem Projektordner neben die
  `platform.txt` des ESP32-Cores kopieren – unter Windows
  `%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\3.3.12\` (Pfade für Linux/macOS stehen in der Datei) – und
  die IDE neu starten, oder den Schalter per `--build-property "compiler.cpp.extra_flags=-mtext-section-literals"`
  übergeben.
- Release-Builds (`uhr4.ino.bin`, `.bootloader.bin`, `.partitions.bin`, `.merged.bin`) kommen direkt nach
  `build_uhr4/` – `flashESP.bat`/`flashESP.sh` flashen genau diese Dateien.
- Displays: GC9A01 (240x240, ohne oder mit Hintergrundbeleuchtung an Pin 3), GC9D01 (160x160) oder ILI9341 (240x320,
  Uhr 240 oben, darunter der Streifen mit Uhrzeit/Datum; feste Hintergrundbeleuchtung) – eine Firmware für alle, der
  Typ ist eine Einstellung in der Weboberfläche (Tab „Uhr Einstellungen“, wirkt nach einem Neustart). Die Maße je Typ
  stehen in `DISPLAY_GEOMETRY` (`config.h`); `CLOCK_WIDTH`, `HAND_WIDTH` usw. lesen den aktiven Typ zur Laufzeit,
  deshalb nie in Array-Größen, `static_assert` oder `#if` verwenden. `TFT_WIDTH`/`TFT_HEIGHT` sind die Panelgröße, die
  nur beim ILI9341 größer als die Uhr ist (`drawInfoStrips()` in `display.cpp`).
- Grafikbibliothek: **LovyanGFX** (getestet mit 1.2.31), unverändert aus dem Bibliotheksverwalter installiert – Bus,
  Panel und Init-Sequenz stehen in `lgfx_config.h`.
- Weitere Bibliotheken aus dem Bibliotheksverwalter: **RTClib** 2.1.4 (Adafruit) mit ihrer Abhängigkeit **Adafruit
  BusIO** 1.17.4. Alles andere (WiFi, WebServer, LittleFS, Preferences, DNSServer, ESPmDNS, Wire) gehört zum Core.
  Dieselbe Liste, die Board-Einstellungen und der Linker-Schalter stehen auch oben in `uhr4.ino`.

## Projektaufbau

Die Firmware besteht aus einem dünnen Sketch (`uhr4.ino`) und je einer `.cpp` pro Aufgabe. Der zugehörige Header
enthält nur Deklarationen (Prototypen, `extern`-Variablen, Typen, Makros); die Funktionen und Variablen selbst stehen
in der `.cpp`. Jede `.cpp` beginnt mit `#include "uhr4.h"`.

| Datei | Aufgabe |
|---|---|
| `uhr4.ino` | Sketch-Einstieg (Arduino und Visual Micro verlangen eine `.ino`): Kopfkommentar mit Bibliotheken und Board-Einstellungen, bindet `uhr4.h` ein |
| `main.cpp` | `setup()`, `loop()` und der WLAN-Aufbau beim Start (`connectWiFiAtBoot()`) |
| `uhr4.h` | Sammelheader: bindet `globals.h` und die Header aller Module ein |
| `build_info.cpp` | Build-Zeit der Firmware (`__DATE__`, `__TIME__`): `buildAll.bat` löscht vor jedem Lauf ihre Objektdatei, damit die Zeit zum jeweiligen Build passt |
| `config.h` | Display-Geometrie je Typ, Hardware-Pins, Konstanten, Board-Auswahl (nur Makros und Konstanten) |
| `lgfx_config.h` | LovyanGFX-Gerät: SPI-Bus, GC9A01- und GC9D01-Panel (eigene Init-Sequenzen), ILI9341 (Standardtreiber von LovyanGFX); bleibt Header, da die Klassen im Header stehen müssen |
| `prefs_keys.h` | Konstanten für die Schlüsselnamen in `Preferences` (NVS) |
| `fs_cache.h` | Zwischenspeicher für LittleFS-Abfragen (Klasse `CachedFs`) |
| `globals.h`, `globals.cpp` | Globale Variablen und gemeinsamer Zustand: `extern`-Deklarationen und Typen im Header, Definitionen mit Startwerten in der `.cpp` |
| `translation.cpp`, `.h` | Übersetzungen der Oberfläche (DE) |
| `wifi_manager.cpp`, `.h` | WLAN-Verbindung, AP-Modus, Scan, WPS |
| `time_sync.cpp`, `.h` | Zeitabgleich per NTP und DCF77 |
| `open_wifi_time.cpp`, `.h` | Uhrzeit aus offenen WLANs beim Start |
| `display.cpp`, `.h` | Zeichnen, BMP-Verarbeitung, RLE-Kompression, Einrichtungsbefehle über USB |
| `presets_manager.cpp`, `.h` | Presets speichern und laden |
| `backup.cpp`, `.h` | Komplettsicherung/-wiederherstellung (TAR), WLAN-Verschlüsselung |
| `webserver_routes.cpp`, `.h` | Alle HTTP-Routen und das erzeugte HTML |
| `face_designer_html.cpp`, `.h` | Zifferblatt-Designer (Browserseite) – **erzeugt**, siehe unten |
| `hand_designer_html.cpp`, `.h` | Zeiger-Designer (Browserseite) – **erzeugt**, siehe unten |
| `designer_common_js.cpp`, `.h` | Gemeinsame Funktionen beider Designer – **erzeugt**, siehe unten |
| `web/` | Quellen beider Designer (`*.css`, `*.html`, `*.js`, gemeinsam `designer_common.js`) und der Generator `build_web.py` |
| `rocrail_client.cpp`, `.h` | Verbindung für die Rocrail-Modellzeit |
| `ota_update.cpp`, `.h` | Firmware-Update über WLAN (nur ESP32-S3) |
| `imu_rotation.cpp`, `.h` | Automatische Rotation per Lagesensor (nur ESP32-S3-LCD-1.28) |
| `mqtt_status.cpp`, `.h` | MQTT-Meldung des Status (z. B. für openHAB) |
| `system_utils.cpp`, `.h` | Tasten, Touch, Logging, Reset, Neustart, Heap-Überwachung, sonstige Hilfsfunktionen |

**Neue Funktionen und Variablen:** Eine Funktion, die eine andere Datei aufruft, braucht einen Prototyp im Header ihres
Moduls (`display.h` usw.); Standardwerte von Parametern stehen nur dort, nicht in der Definition. Variablen, die mehrere
Dateien nutzen, stehen als `extern` im Header und mit Definition in der `.cpp` (die globalen in `globals.h` und
`globals.cpp`). Eine neue `.cpp` bekommt einen Header, den `uhr4.h` einbindet, und wird in der Projektdatei von Visual
Micro (`uhr4.vcxproj`) ergänzt; Arduino IDE und `arduino-cli` finden sie von selbst.

### Designer-Seiten (`web/`)

Zifferblatt- und Zeiger-Designer werden in `web/face_designer.*` und `web/hand_designer.*` bearbeitet, Funktionen für
beide (BMP lesen, Zeigerwinkel, Farbhilfen) in `web/designer_common.js` – nie in den erzeugten Dateien. Nach einer
Änderung aus dem Projektordner aufrufen:

```bash
python web/build_web.py
```

Das Skript schreibt `face_designer_html`, `hand_designer_html` und `designer_common_js` (je eine `.h` mit den
`extern`-Deklarationen und eine `.cpp` mit den Daten): das Markup bleibt Text,
CSS und Skripte werden ohne Kommentarzeilen gzip-komprimiert abgelegt (ausgeliefert unter `/facedesigner.css|.js`,
`/handdesigner.css|.js` und `/designer.js`, der Browser entpackt). Das spart rund 90 KB Flash. Die neu erzeugten
Dateien zusammen mit den Quellen in `web/` committen – zum Bauen der Firmware selbst ist kein zusätzlicher Schritt
nötig.

## Code-Stil

- **Einrückung:** Präprozessor-Direktiven (`#define`, `#include`, `#if`, `#endif`, …) stehen ganz links (Spalte 0);
  alles andere folgt der normalen verschachtelten Einrückung.
- **Namen:** camelCase für Variablen und Funktionen.
- Zusammengehörende Logik beieinander lassen und kurz kommentieren, *warum* etwas so ist, nicht nur *was* – vor allem
  bei allem, was nicht offensichtlich ist (Timing, Speicheraufteilung, Eigenheiten der Hardware).

## Übersetzungen (`translation.cpp`)

- Die Übersetzungen liegen in einem Array im Flash, `static const TranslationEntry translationTable[]` (ein einfaches
  Array, kein `std::map`) – das ist Absicht, siehe „Speichergrenzen“ unten. Neue Einträge als
  `{ "English key", "German value" }` ergänzen.
- **Für Umlaute und Sonderzeichen HTML-Entities verwenden** (`&auml;`, `&ouml;`, `&uuml;`, `&szlig;`, …) statt roher
  UTF-8-Zeichen. Der HTML-Kopf sendet keine Zeichensatz-Angabe, rohe UTF-8-Bytes stellt der Browser deshalb falsch dar
  (Zeichensalat) – HTML-Entities erscheinen in jedem Fall richtig.
- **Kein Layout-Markup in Übersetzungen** (`<br>`, rein gestalterisches `&nbsp;`). Wenn ein Zeilenumbruch oder ein
  geschütztes Leerzeichen fürs Layout nötig ist, gehört es in den umgebenden C++/HTML-Code, nicht in den übersetzten
  Text. (`&nbsp;` *innerhalb* eines übersetzten Ausdrucks ist in Ordnung, wenn es genau diesen Ausdruck vor einem
  unschönen Umbruch bewahrt, z. B. bei schmalen Menüpunkten.)
- **Jeder `translate("...")`-Aufruf braucht einen deutschen Eintrag – außer der deutsche Text ist gleich** (z. B.
  „Status“, „Log“, „Version“): `translate()` gibt ohne Eintrag den Schlüssel zurück, gleiche Einträge werden deshalb
  bewusst weggelassen. Vor einem PR prüfen, dass keine Übersetzung fehlt (alle in den `.ino`/`.cpp`/`.h`-Dateien verwendeten
  `translate("...")`-Schlüssel sammeln und sicherstellen, dass jeder in `translationTable` steht oder auf Deutsch
  gleich lautet), und Einträge entfernen, deren Schlüssel nirgends mehr verwendet wird.
- Der Schlüssel selbst bleibt Englisch und entspricht dem Text, der bei englischer Oberfläche angezeigt wird.

## Speichergrenzen (bitte vor neuen Abhängigkeiten lesen)

Der ESP32-S2 hat wenig internen RAM, und das Projekt hatte schon echte Fehler, weil das unterschätzt wurde:

- **`<algorithm>`/`std::sort` vermeiden.** Das Projekt verwendet stattdessen kleine, ausgeschriebene Insertion-Sorts
  (siehe `naturalSortNames()`), um die Abhängigkeit für ohnehin kleine Listen (Dateien, Presets) nicht hereinzuholen.
- **Große Konstruktionen über `std::initializer_list` vermeiden**, vor allem in Funktionen, die von HTTP-Handlern
  erreicht werden. Ein `std::map` aus einer Initialisierungsliste mit rund 200 Einträgen hat einmal auf `/setLanguage`
  einen Stack-Überlauf ausgelöst – deshalb sind die Übersetzungen jetzt ein flaches Array.
- **Schreibzugriffe auf `Preferences` (NVS) gering halten.** Flash verträgt nur begrenzt viele Schreibvorgänge;
  `preferences.put...()` nur aufrufen, wenn sich ein Wert tatsächlich geändert hat (Muster siehe `savePresets()`).
- **Nicht darauf bauen, dass PSRAM bei TLS/HTTPS hilft.** Die internen Puffer von mbedTLS brauchen für den Handshake
  zusammenhängenden *internen* RAM und weichen auf diesem Chip nicht automatisch ins PSRAM aus – eine Funktion zum
  Abruf von GitHub über HTTPS wurde genau deshalb wieder entfernt. Wer HTTPS braucht, sollte beim Verbindungsaufbau
  mit 40 KB oder mehr freiem internen Heap rechnen und auf echter Hardware testen, nicht nach „sollte theoretisch
  gehen“.

## Vor einem Pull Request

Bitte prüfen:

- [ ] Geschweifte Klammern `{`/`}` sind in jeder geänderten Datei ausgeglichen
- [ ] Präprozessor-Direktiven (`#if`/`#ifdef`/`#ifndef`/`#endif`) sind ausgeglichen
- [ ] Jeder `translate("...")`-Aufruf hat einen deutschen Eintrag in `translation.cpp` (außer der deutsche Text ist
      gleich)
- [ ] In `translation.cpp` sind keine rohen Nicht-ASCII-Zeichen hinzugekommen (HTML-Entities verwenden – siehe oben)
- [ ] Der Sketch lässt sich für das oben beschriebene ESP32-S2-Board und Partitionsschema weiterhin kompilieren
- [ ] Neue Routen, die Zustand senden oder ändern, verwenden `HTTP_POST` (nicht `HTTP_GET`), wenn sie Nebenwirkungen
      haben, und lassen zerstörende Aktionen vorher im Browser bestätigen (`onclick="return confirm(...)"`), wie die
      vorhandenen Lösch- und Rücksetz-Muster

## Fehler melden

Bei einem Fehlerbericht bitte angeben:
- Board-Variante und Displaytyp (GC9A01 ohne Hintergrundbeleuchtung, GC9A01 mit Hintergrundbeleuchtung an Pin 3,
  GC9D01 oder ILI9341) und die Firmware-Version bzw. das Build-Datum
- Schritte, mit denen sich der Fehler nachstellen lässt
- Ausgabe des seriellen Monitors, falls das Gerät abgestürzt ist oder sich unerwartet verhalten hat

---
---

# English Version

Thanks for your interest in this project! It's a hobby project maintained
in spare time, so please keep pull requests focused and reasonably small -
that makes them much easier to review and merge.

## Hardware & Build Environment

- Target: **ESP32-S2** only (limited RAM - see "Memory constraints" below)
- Board: **LOLIN S2 PICO** (esp32 Arduino core, built with 3.3.12), partition
  scheme: **"No OTA (2MB APP/2MB SPIFFS)"**, USB CDC On Boot "Enabled". PSRAM is always on for this
  board (`BOARD_HAS_PSRAM` in the board definition, no menu option).
- Compiler flag `-mtext-section-literals` is required: function and literal should lie next to each other even in the
  large source files (`display.cpp`, `webserver_routes.cpp`), otherwise the linker fails with "dangerous relocation:
  l32r: literal target out of range". Visual Micro picks it up from
  `board.txt` in the project folder; for Arduino IDE/arduino-cli copy `platform.local.txt` from the project
  folder next to the ESP32 core's `platform.txt` - on Windows
  `%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\3.3.12\` (Linux/macOS paths are in the file) - and
  restart the IDE, or pass the flag via `--build-property "compiler.cpp.extra_flags=-mtext-section-literals"`.
- Release builds (`uhr4.ino.bin`, `.bootloader.bin`, `.partitions.bin`,
  `.merged.bin`) go directly into `build_uhr4/` - `flashESP.bat`/`flashESP.sh`
  flash exactly those files.
- Displays: GC9A01 (240x240, without or with backlight on pin 3), GC9D01
  (160x160) or ILI9341 (240x320, 240 clock on top, time/date strip below; fixed backlight) -
  one firmware for all, the type is a setting in the web UI ("Clock Setup" tab, takes effect after a restart).
  Dimensions per type live in `DISPLAY_GEOMETRY` (`config.h`); `CLOCK_WIDTH`,
  `HAND_WIDTH` etc. read the active type at runtime, so never use them in array
  sizes, `static_assert` or `#if`. `TFT_WIDTH`/`TFT_HEIGHT` are the panel size, which is only larger
  than the clock on the ILI9341 (`drawInfoStrips()` in `display.cpp`).
- Graphics library: **LovyanGFX** (tested with 1.2.31), installed unchanged from
  the Library Manager - bus, panel and init sequence live in `lgfx_config.h`
- Other libraries from the Library Manager: **RTClib** 2.1.4 (Adafruit) with its dependency **Adafruit BusIO**
  1.17.4. Everything else (WiFi, WebServer, LittleFS, Preferences, DNSServer, ESPmDNS, Wire) is part of the core.
  The same list, board settings and the linker flag are also at the top of `uhr4.ino`.

## Project Structure

The firmware is a thin sketch (`uhr4.ino`) plus one `.cpp` per responsibility. The matching header only holds
declarations (prototypes, `extern` variables, types, macros); the functions and variables themselves live in the
`.cpp`. Every `.cpp` starts with `#include "uhr4.h"`.

| File | Responsibility |
|---|---|
| `uhr4.ino` | Sketch entry (Arduino and Visual Micro require an `.ino`): header comment with libraries and board settings, includes `uhr4.h` |
| `main.cpp` | `setup()`, `loop()` and the WiFi connection at boot (`connectWiFiAtBoot()`) |
| `uhr4.h` | Umbrella header: includes `globals.h` and the headers of all modules |
| `build_info.cpp` | Build time of the firmware (`__DATE__`, `__TIME__`): `buildAll.bat` deletes its object file before every run so that the time matches the build |
| `config.h` | Display geometry per type, hardware pins, constants, board selection (macros and constants only) |
| `lgfx_config.h` | LovyanGFX device: SPI bus, GC9A01 and GC9D01 panels (own init sequences), ILI9341 (LovyanGFX default driver); stays a header because the classes have to be in a header |
| `prefs_keys.h` | `Preferences` (NVS) key name constants |
| `fs_cache.h` | Cache for LittleFS queries (class `CachedFs`) |
| `globals.h`, `globals.cpp` | Global variables and shared state: `extern` declarations and types in the header, definitions with initial values in the `.cpp` |
| `translation.cpp`, `.h` | UI translations (DE) |
| `wifi_manager.cpp`, `.h` | WiFi connection, AP mode, scanning, WPS |
| `time_sync.cpp`, `.h` | NTP and DCF77 time synchronization |
| `open_wifi_time.cpp`, `.h` | Time from open WiFis at boot |
| `display.cpp`, `.h` | Rendering, BMP handling, RLE compression, USB setup commands |
| `presets_manager.cpp`, `.h` | Preset storage/retrieval logic |
| `backup.cpp`, `.h` | Full backup/restore (TAR), WiFi encryption |
| `webserver_routes.cpp`, `.h` | All HTTP routes and generated HTML |
| `face_designer_html.cpp`, `.h` | Clock face designer (browser page) - **generated**, see below |
| `hand_designer_html.cpp`, `.h` | Hand designer (browser page) - **generated**, see below |
| `designer_common_js.cpp`, `.h` | Functions shared by both designers - **generated**, see below |
| `web/` | Sources of both designers (`*.css`, `*.html`, `*.js`, shared `designer_common.js`) and the generator `build_web.py` |
| `rocrail_client.cpp`, `.h` | Rocrail model time connection |
| `ota_update.cpp`, `.h` | Firmware update over WiFi (ESP32-S3 only) |
| `imu_rotation.cpp`, `.h` | Automatic rotation via the motion sensor (ESP32-S3-LCD-1.28 only) |
| `mqtt_status.cpp`, `.h` | MQTT status report (e.g. for openHAB) |
| `system_utils.cpp`, `.h` | Buttons, touch, logging, reset, restart, heap monitoring, misc helpers |

**New functions and variables:** a function that another file calls needs a prototype in its module's header
(`display.h` etc.); default values of parameters live only there, not in the definition. Variables used by several files
are `extern` in the header and defined in the `.cpp` (the global ones in `globals.h` and `globals.cpp`). A new `.cpp`
gets a header that `uhr4.h` includes and is added to the Visual Micro project file (`uhr4.vcxproj`); the Arduino IDE
and `arduino-cli` find it on their own.

### Designer pages (`web/`)

The clock face and hand designers are edited in `web/face_designer.*` and `web/hand_designer.*`, functions used by
both (BMP reading, hand angles, colour helpers) in `web/designer_common.js` - never in the generated files. After a
change, run from the project folder:

```bash
python web/build_web.py
```

It writes `face_designer_html`, `hand_designer_html` and `designer_common_js` (each a `.h` with the `extern`
declarations and a `.cpp` with the data): the markup stays text, CSS and
scripts are stored gzip-compressed without comment lines (served at `/facedesigner.css|.js`, `/handdesigner.css|.js`
and `/designer.js`, unpacked by the browser). This saves ~90 KB of flash. Commit the regenerated files
together with the `web/` sources - building the firmware itself needs no extra step.

## Code Style

- **Indentation:** preprocessor directives (`#define`, `#include`, `#if`,
  `#endif`, ...) stay flush left (column 0); everything else follows normal
  nested indentation.
- **Naming:** camelCase for variables and functions.
- Keep related logic together and add a short comment explaining *why*,
  not just *what*, especially for anything non-obvious (timing, memory
  layout, hardware quirks).

## Translations (`translation.cpp`)

- Translations live in a flash-resident `static const TranslationEntry
  translationTable[]` (a plain array, not `std::map`) - this is
  intentional, see "Memory constraints" below. Add new entries as
  `{ "English key", "German value" }`.
- **Use HTML entities for accented characters** (`&auml;`, `&ouml;`,
  `&uuml;`, `&szlig;`, ...)
  instead of raw UTF-8 umlauts/accents. The HTML head sends no charset
  declaration, so raw UTF-8 bytes get mis-rendered by the browser
  (mojibake) - HTML entities render correctly regardless.
- **Never embed layout markup** (`<br>`, purely-structural `&nbsp;`) inside
  a translation string. If you need a line break or non-breaking space for
  layout, add it in the surrounding C++/HTML code, not the translated
  text. (`&nbsp;` *inside* a translated phrase is fine when it's part of
  keeping that specific phrase from wrapping awkwardly, e.g. narrow nav
  labels.)
- **Every `translate("...")` call needs a German entry - unless the German
  text is identical** (e.g. "Status", "Log", "Version"): `translate()` returns
  the key when there is no entry, so identical entries are deliberately left
  out. Before submitting a PR, verify there are no missing translations
  (extract every `translate("...")` key used across the `.ino`/`.cpp`/`.h` files and
  confirm each exists in `translationTable` or reads the same in German), and
  remove entries whose key is no longer used anywhere.
- Keep the key itself in English, matching the fallback text shown when
  the interface language is English.

## Memory Constraints (please read before adding dependencies)

The ESP32-S2 has limited internal RAM, and this project has already hit
real bugs from underestimating that:

- **Avoid `<algorithm>`/`std::sort`.** The project uses small, explicit
  insertion sorts instead (see `naturalSortNames()`) to avoid pulling in
  the dependency for what are always small (file/preset count) lists.
- **Avoid large `std::initializer_list`-based constructions**, especially
  inside functions reachable from HTTP request handlers. A `std::map`
  built from a ~200-entry initializer list once caused a stack overflow
  crash on `/setLanguage` - that's why translations are now a flat array
  instead.
- **Minimize `Preferences` (NVS) writes.** Flash has a limited write
  endurance; only call `preferences.put...()` when a value actually
  changed (see `savePresets()` for the pattern).
- **Don't assume PSRAM helps with TLS/HTTPS.** mbedTLS's internal buffers
  need contiguous *internal* RAM for the handshake and do not
  automatically spill into PSRAM on this chip - a GitHub-fetch-over-HTTPS
  feature was reverted for exactly this reason. If you need HTTPS, budget
  for ~40 KB+ of free internal heap at connection time and test on real
  hardware, not just "should work in theory."

## Before Submitting a Pull Request

Please check:

- [ ] Braces `{`/`}` balance in every file you touched
- [ ] Preprocessor directives (`#if`/`#ifdef`/`#ifndef`/`#endif`) balance
- [ ] Every `translate("...")` call has a matching German entry in
      `translation.cpp` (unless the German text is identical)
- [ ] No raw non-ASCII characters were introduced into `translation.cpp`
      (use HTML entities - see above)
- [ ] The sketch still compiles for the ESP32-S2 board/partition scheme
      described above
- [ ] New routes that send/modify state use `HTTP_POST` (not `HTTP_GET`)
      if they have side effects, and confirm destructive actions
      client-side (`onclick="return confirm(...)"`) before submitting,
      matching the existing delete/reset patterns

## Reporting Issues

When reporting a bug, please include:
- Board variant and display type (GC9A01 without backlight, GC9A01 with
  backlight on pin 3, GC9D01 or ILI9341) and the firmware version/build date
- Steps to reproduce
- Serial monitor output if the device crashed or behaved unexpectedly
