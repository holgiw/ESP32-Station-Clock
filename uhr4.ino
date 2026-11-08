    // carleaner@gmx.de - Stationsuhr uhr4: ESP32-S2 Mini (Lolin S2 Pico), LittleFS, TFT
    // GC9A01/GC9D01/ILI9341, ESP32-C6 mit ST7789 oder ESP32-S3 mit GC9A01. DCF77-Modul: https://de.elv.com/p/elv-dcf-empfangsmodul-dcf-2-P091610/

    // Bibliotheken (Bibliotheksverwalter): LovyanGFX 1.3.0, RTClib 2.1.4 mit Adafruit BusIO 1.17.4. WiFi,
    // WebServer, LittleFS, Preferences, DNSServer, ESPmDNS und Wire bringt der Core "esp32" 3.3.12 von
    // Espressif mit.

    // Board: "LOLIN S2 PICO", Partition Scheme "No OTA (2MB APP/2MB SPIFFS)", USB CDC On Boot "Enabled"
    // (Vorgabe); PSRAM ist bei diesem Board immer an.

    // Waveshare ESP32-C6-LCD-1.47 / -1.3 (eigenes Build): Board "ESP32C6 Dev Module", Partition Scheme "uhr4 C6
    // (2.25MB APP/1.6MB SPIFFS)" (boards.local.txt und uhr4_c6.csv aus dem Projektordner in den Core-Ordner
    // kopieren, siehe boards.local.txt), USB CDC On Boot "Enabled". Pins stellt config.h ein
    // (BOARD_WAVESHARE_C6_ST7789), das Display der Displaytyp (ST7789 / ST7789_240).

    // Waveshare ESP32-S3-LCD-1.28 (eigenes Build): Board "ESP32S3 Dev Module", Flash Size "16MB", PSRAM "QSPI
    // PSRAM", Partition Scheme "16M Flash (3MB APP/9.9MB FATFS)", USB CDC On Boot "Disabled" (USB ueber CH343P).
    // Pins stellt config.h ein (BOARD_WAVESHARE_S3_GC9A01).

    // build_opt.h (nur "-g0", Kommentare darf die Datei nicht enthalten): ohne Debug-Infos - schnelleres
    // Uebersetzen und Linken, gleiche Firmware; ein Absturz-Backtrace zeigt dann nur Funktionsnamen.

    // ESP32-S2 und -S3 brauchen beim Linken -mtext-section-literals (sonst "dangerous relocation: l32r"). Visual Micro
    // liest board.txt. Arduino IDE: platform.local.txt aus dem Projektordner nach
    // %LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\3.3.12\ kopieren (Linux/macOS: siehe Datei).

    // carleaner@gmx.de - station clock uhr4: ESP32-S2 Mini (Lolin S2 Pico), LittleFS, TFT
    // GC9A01/GC9D01/ILI9341, ESP32-C6 with ST7789 or ESP32-S3 with GC9A01. DCF77 module: https://de.elv.com/p/elv-dcf-empfangsmodul-dcf-2-P091610/

    // Libraries (Library Manager): LovyanGFX 1.3.0, RTClib 2.1.4 with Adafruit BusIO 1.17.4. WiFi,
    // WebServer, LittleFS, Preferences, DNSServer, ESPmDNS and Wire come with the "esp32" core 3.3.12 by
    // Espressif.

    // Board: "LOLIN S2 PICO", Partition Scheme "No OTA (2MB APP/2MB SPIFFS)", USB CDC On Boot "Enabled"
    // (default); PSRAM is always on for this board.

    // Waveshare ESP32-C6-LCD-1.47 / -1.3 (separate build): board "ESP32C6 Dev Module", Partition Scheme "uhr4 C6
    // (2.25MB APP/1.6MB SPIFFS)" (copy boards.local.txt and uhr4_c6.csv from the project folder into the core
    // folder, see boards.local.txt), USB CDC On Boot "Enabled". config.h sets the pins
    // (BOARD_WAVESHARE_C6_ST7789), the display type the display (ST7789 / ST7789_240).

    // Waveshare ESP32-S3-LCD-1.28 (separate build): board "ESP32S3 Dev Module", Flash Size "16MB", PSRAM "QSPI
    // PSRAM", Partition Scheme "16M Flash (3MB APP/9.9MB FATFS)", USB CDC On Boot "Disabled" (USB via CH343P).
    // config.h sets the pins (BOARD_WAVESHARE_S3_GC9A01).

    // build_opt.h (only "-g0", the file must not contain comments): without debug info - faster compiling
    // and linking, same firmware; a crash backtrace then only shows function names.

    // The ESP32-S2 and -S3 need -mtext-section-literals when linking (else "dangerous relocation: l32r"). Visual
    // Micro reads board.txt. Arduino IDE: copy platform.local.txt from the project folder to
    // %LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\3.3.12\ (Linux/macOS: see the file).

#include "uhr4.h"
