#pragma once

#include <WiFi.h>
#include <WebServer.h>

#include "prefs_keys.h"

#include "config.h"        // Board-/Display-Auswahl, Pins, Timing-Makros
                           // board/display selection, pins, timing macros
#include "lgfx_config.h"   // LovyanGFX: Bus, Panel, Init-Sequenz (Pins aus config.h)
                           // LovyanGFX: bus, panel, init sequence (pins from config.h)
#include <Preferences.h>
#include <LittleFS.h>
#include "fs_cache.h"   // Zwischenspeicher fuer LittleFS-Abfragen, ersetzt LittleFS per Makro / cache for LittleFS queries, replaces LittleFS via macro
#include <set>
#include "nvs_flash.h"
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <map>
#include <esp_wps.h>
#include <esp_wifi.h>

// ESP_ARDUINO_VERSION_STR: arduino-esp32-Core-Version, mit der kompiliert
// wurde (Anzeige im Status, siehe webserver_routes.cpp).

// ESP_ARDUINO_VERSION_STR: arduino-esp32 core version this was compiled
// with (shown in Status, see webserver_routes.cpp).

#include <esp_arduino_version.h>

// Fuer esp_reset_reason() - Grund des letzten Neustarts (Power-On, Watchdog,
// Panic, Brownout, ...), Anzeige im Status (siehe webserver_routes.cpp).

// For esp_reset_reason() - reason for the last restart (power-on, watchdog,
// panic, brownout, ...), shown in Status (see webserver_routes.cpp).

#include <esp_system.h>
#include <esp_mac.h> // esp_read_mac()
#include <Wire.h>
#include <RTClib.h>
#include <WiFiUdp.h>

// Fuer sntp_set_sync_mode()/sntp_set_sync_interval() - siehe setup() und
// den Kommentar dort zum Sekundenzeiger-Ruecksprung-Fix.

// For sntp_set_sync_mode()/sntp_set_sync_interval() - see setup() and the
// comment there about the second-hand jump-back fix.

#include <esp_sntp.h>

    // Globale Objekte/Variablen, nach Modul sortiert - als extern deklariert, definiert in globals.cpp.
    // Semaphoren-Header explizit fuer logBufferMutex (kommt nicht sicher transitiv mit).

    // Global objects/variables, sorted by module - declared extern, defined in globals.cpp.
    // Semaphore header explicitly for logBufferMutex (not reliably included transitively).

#include <freertos/semphr.h>

    // Aktiver Displaytyp (Index in DISPLAY_GEOMETRY) - CLOCK_WIDTH, HAND_WIDTH usw. lesen ueber displayGeom.
    // loadDisplayType() setzt ihn ganz am Anfang von setup(), vor allem, was Masse braucht.

    // Active display type (index into DISPLAY_GEOMETRY) - CLOCK_WIDTH, HAND_WIDTH etc. read via displayGeom.
    // loadDisplayType() sets it at the very start of setup(), before anything that needs dimensions.

    extern uint8_t displayType;
    extern const DisplayGeometry* displayGeom;

    // System / Allgemein
    // System / General

    extern String currentLanguage; // Standardmäßig Deutsch
                                   // Default: German

    extern char version[20]; // Build-Version ("YYYY-MM-DD HH:MM:SS" = 19 Zeichen + Nullterminator)
                      // Build version ("YYYY-MM-DD HH:MM:SS" = 19 chars + null terminator)

    extern bool loggingEnabled;

    // Laufendes Firmware-Update (ota_update.cpp): loop() pausiert, WLAN-Scans unterbleiben. Bleibt auf
    // S2/C6 (ohne OTA) immer false.

    // Running firmware update (ota_update.cpp): loop() pauses, WiFi scans are skipped. Always stays false on
    // S2/C6 (without OTA).

    extern bool otaInProgress;
    extern unsigned long otaStartMillis;
    extern String otaUploadError; // Fehler beim Hochladen ueber die Weboberflaeche, leer = ok
                                // error while uploading via the web interface, empty = ok

    // Kennung der Build-Zeit in der .bin (wie version, nur als Text "Oct  1 2026 12:00:00"): publish_s3_update.ps1
    // und checkClocks.ps1 lesen sie aus der Datei. Alle Chips; sie wird beim Start ins Log geschrieben (sonst wirft
    // der Linker sie weg). Datum und Uhrzeit des Builds einzeln: fwBuildDate ("Oct  1 2026"), fwBuildTime. Alle drei
    // stehen in build_info.cpp.

    // Marker of the build time in the .bin (like version, only as text "Oct  1 2026 12:00:00"): publish_s3_update.ps1
    // and checkClocks.ps1 read it from the file. All chips; it is written to the log at start (otherwise the linker
    // drops it). Date and time of the build separately: fwBuildDate ("Oct  1 2026"), fwBuildTime. All three are in
    // build_info.cpp.

    extern const char fwBuildMarker[];
    extern const char fwBuildDate[];
    extern const char fwBuildTime[];

    // Zeiger fahren auf ihre Position (animateHand() in display.cpp, Start und Zeitkorrektur, 3 bis 10 s): in dieser Zeit
    // ruht alle Hintergrundarbeit, die den Takt stoeren koennte (Log in den Flash schreiben, Webserver, NTP-Antworten,
    // Rocrail). Die Pause dauert so lange wie die Fahrt plus 1 s (handsDeferMs), mindestens HANDS_ANIMATION_MAX_DEFER_MS,
    // damit auch die lange Startfahrt ungestoert bleibt und nie etwas haengen bleibt.

    // Hands moving to their position (animateHand() in display.cpp, start and time correction, 3 to 10 s): during that
    // time all background work that could disturb the rhythm rests (writing the log to flash, web server, NTP replies,
    // Rocrail). The pause lasts as long as the move plus 1 s (handsDeferMs), at least HANDS_ANIMATION_MAX_DEFER_MS, so
    // the long start move stays undisturbed too and nothing ever hangs.

    // Nach einem Neustart (OTA, Browser, WPS) behaelt der Chip seine Systemzeit - sie kann falsch sein (hier 28 Minuten).
    // Ohne RTC/DCF77/NTP/manuell gesetzte Zeit bleiben die Zeiger deshalb bis zu TIME_TRUST_WAIT_MS auf 12, statt
    // erst zu dieser Zeit und dann nochmal zur richtigen zu fahren (timeTrusted() in display.cpp).

    // After a restart (OTA, browser, WPS) the chip keeps its system time - it may be wrong (here 28 minutes). Without
    // RTC/DCF77/NTP/manually set time the hands therefore stay at 12 for up to TIME_TRUST_WAIT_MS, instead of first
    // moving to that time and then again to the right one (timeTrusted() in display.cpp).

#define TIME_TRUST_WAIT_MS 15000
    extern bool timeSetByUser; // setClockTime() erfolgreich / setClockTime() succeeded

#define HANDS_ANIMATION_MAX_DEFER_MS 5000
    extern bool handsAnimating;
    extern unsigned long handsAnimatingSinceMillis;
    extern unsigned long handsDeferMs; // gilt fuer die laufende Fahrt, siehe renderClockFrame() / for the running move, see renderClockFrame()

    inline bool backgroundWorkAllowed() {
        return !handsAnimating || millis() - handsAnimatingSinceMillis > handsDeferMs;
    }

    // Ergebnis der letzten Update-Pruefung auf GitHub (ota_update.cpp): Build-Zeit des Releases (leer = unbekannt) und
    // Fehlertext (leer = ok); fwLastCheckMillis = Zeitpunkt des Versuchs

    // Result of the last update check on GitHub (ota_update.cpp): build time of the release (empty = unknown) and
    // error text (empty = ok); fwLastCheckMillis = time of the attempt

    extern String fwRemoteBuild;
    extern String fwCheckError;
    extern unsigned long fwLastCheckMillis;

    // Log-Zeilen werden gesammelt statt einzeln auf Flash geschrieben - jeder Flash-Zugriff haelt kurz alles
    // an (auch die Displayuebertragung). logBufferMutex schuetzt logLineBuffer, da auch NTP-/Rocrail-Tasks
    // hineinschreiben.

    // Log lines are collected instead of written to flash one by one - every flash access briefly stalls
    // everything (including the display transfer). logBufferMutex protects logLineBuffer, since NTP/Rocrail
    // tasks write into it too.

    extern String logLineBuffer;
    extern SemaphoreHandle_t logBufferMutex; // erzeugt in setup() via xSemaphoreCreateMutex()
                                                // created in setup() via xSemaphoreCreateMutex()
    extern unsigned long lastLogFlushMillis;
#define LOG_FLUSH_INTERVAL_MS 3000 // spaetestens nach dieser Zeit leeren, auch bei nur einer Zeile
                                  // flush at the latest after this long, even for just one line
#define LOG_FLUSH_MAX_BUFFER_BYTES 1024 // frueher leeren, wenn der Puffer waechst, statt RAM zu verschwenden
                                        // flush earlier if the buffer grows, instead of wasting RAM

    extern bool initial;

    extern String ipAddress;


    // Kern-Hardwareobjekte (TFT, Webserver, Preferences, RTC, ...)
    // Core hardware objects (TFT, web server, preferences, RTC, ...)

    extern UhrLGFX tft; // ein Geraet fuer beide Displays, siehe lgfx_config.h
                 // one device for both displays, see lgfx_config.h

    // false bis tft.init() gelaufen ist - vorher duerfen setCS1()/setCS2()
    // (display.cpp) noch keine Rotation an den Chip senden.

    // false until tft.init() has run - before that setCS1()/setCS2()
    // (display.cpp) must not send a rotation to the chip yet.

    extern bool tftInitialized;
    extern WebServer webserver;
    extern Preferences preferences;
    extern DNSServer dnsServer;
    extern WiFiUDP udp;

    // Eigene WiFiUDP-Instanz fuer die R2RNet-Multicast-Diagnose - unabhaengig von 'udp' (eigener NTP-Server).
    // Separate WiFiUDP instance for the R2RNet multicast diagnostics - independent of 'udp' (own NTP server).

    extern WiFiUDP r2rnetDebugUdp;
    extern uint8_t r2rnetDebugLogCount; // geloggte R2RNet-Pakete seit dem letzten Beitritt (siehe R2RNET_DEBUG_LOG_LIMIT)
                                     // R2RNet packets logged since the last join (see R2RNET_DEBUG_LOG_LIMIT)
    extern bool r2rnetDebugListening; // true, sobald der Multicast-Gruppe erfolgreich beigetreten wurde
                                       // true once the multicast group has been joined successfully

    extern RTC_DS3231 rtc;

    // WLAN
    // WiFi

#define MAX_WLAN 15
    extern String wifiSsid[MAX_WLAN];
    extern String wifiPass[MAX_WLAN];

#define NOT_CONNECTED 0
#define CONNECTED 1

    extern bool wifiActive;

    // Zustand fuer eine per Web-Button ausgeloeste WPS-Anfrage (siehe loop() in
    // main.cpp, /api/startWPS in webserver_routes.cpp) - laeuft asynchron und
    // event-basiert ueber WiFi.onEvent(), um den Webserver nicht zu blockieren.

    // State for a web-button-triggered WPS request (see loop() in main.cpp,
    // /api/startWPS in webserver_routes.cpp) - runs asynchronously and event-based
    // via WiFi.onEvent(), so the web server isn't blocked.

    extern bool wpsPending;
    extern unsigned long wpsStartMillis;
    extern String wpsPreviousSsid; // Verbindung vor dem WPS-Start, um danach ggf. dorthin zurueckzuwechseln
                                 // Connection before the WPS start, to switch back to it afterward if needed
    extern volatile bool wpsSuccessEvent; // wird im WiFi-Event-Callback gesetzt (anderer Kontext!)
                                           // set in the WiFi event callback (different context!)
#ifdef WIFI_TX_AUTO
    extern volatile int8_t wifiTxPowerCurrent; // gerade gewaehlte Sendeleistung (wifiTxPowerCurrent wird im Start-Ereignis angewendet)
                                                            // currently chosen transmit power (applied in the start event)
#endif
    extern volatile int lastTxPowerResult; // Ergebnis von esp_wifi_set_max_tx_power() im Start-Ereignis (-1 = nie aufgerufen)
                                         // result of esp_wifi_set_max_tx_power() in the start event (-1 = never called)
    extern volatile uint8_t lastWifiDisconnectReason; // Grund des letzten Verbindungsabbruchs (WiFi-Event, anderer Kontext!)
                                                   // reason of the last disconnect (WiFi event, other context!)
    extern volatile bool wpsFailedEvent;

    // Verzoegerter WPS-Start: der HTTP-Handler darf nicht per delay()
    // blockieren, sonst kann der Webserver die Folge-Anfrage fuer die
    // Redirect-Zielseite nicht mehr annehmen - loop() startet WPS stattdessen zeitgesteuert etwas spaeter.

    // Deferred WPS start: the HTTP handler must not block with delay(),
    // otherwise the web server can't accept the redirect target page's
    // follow-up request - loop() starts WPS on a timer a bit later instead.

    extern bool wpsStartRequested;
    extern unsigned long wpsStartRequestedAtMillis;

    // MAC Adresse
    // MAC address

    extern uint8_t mac[6];
    extern char hostname[32];

    // Zur Laufzeit erzeugtes AP-Passwort (aus MAC-Bytes, siehe startAP() in
    // wifi_manager.cpp) - als Puffer, damit Display und Web-UI-Statuszeile es
    // beide anzeigen koennen. Leer, solange der AP nie lief.

    // AP password generated at runtime (from MAC bytes, see startAP() in
    // wifi_manager.cpp) - as a buffer so both the display and web UI status line
    // can show it. Empty as long as the AP has never run.

    extern char apPassword[64]; // AP_PASSWORD (config.h), max. 63 Zeichen + NUL / max. 63 chars + NUL
    extern bool pingHostname;

    extern bool softAPIP; // Flag für SoftAP IP
                            // Flag for SoftAP IP
    extern long softAPIPstart; // Startzeit für SoftAP IP
                             // Start time for SoftAP IP

    // Bestaetigungscode fuer Werksreset-Aktionen und Aenderungen am AKTUELL VERBUNDENEN WLAN: "" = nichts
    // angefordert, sonst der auf dem Display gezeigte 3-stellige Code - verhindert solche Aktionen aus der
    // Ferne ohne Zugriff auf die Uhr. Andere WLAN-Netze bleiben ohne Code aenderbar.

    // Confirmation code for factory reset actions and changes to the CURRENTLY CONNECTED WiFi: "" = nothing
    // requested, otherwise the 3-digit code shown on the display - prevents such actions remotely without
    // access to the clock. Other WiFi networks stay changeable without a code.

    extern String factoryResetCode;
    extern unsigned long factoryResetCodeStartMillis;
    extern bool factoryResetCodeShown; // wurde der Code schon gezeichnet? (nur einmal, nicht jeden Frame)
                                       // has the code already been drawn? (only once, not every frame)
    extern String factoryResetPendingAction; // welche Aktion nach Code-Bestaetigung ausgefuehrt wird:
                                           // "all"/"wifi"/"faces"/"hands"/"presets"/"wlanDeleteActive"/
                                           // "wlanOverwriteActive" (siehe /factoryReset/confirm)
    extern uint8_t factoryResetCodeAttempts; // Fehlversuche fuer den AKTUELLEN Code (siehe
                                         // FACTORY_RESET_MAX_ATTEMPTS in config.h) - ohne diese Bremse waere
                                         // der Code per Brute-Force erratbar, da 3 Stellen nur 900
                                         // Kombinationen sind und sonst beliebig oft geraten werden koennte.
                                         // failed attempts for the CURRENT code (see FACTORY_RESET_MAX_ATTEMPTS
                                         // in config.h) - without this brake the code would be brute-forceable,
                                         // since 3 digits are only 900 combinations and could otherwise be
                                         // guessed an unlimited number of times.
                                           // which action runs once the code is confirmed:
                                           // "all"/"wifi"/"faces"/"hands"/"presets"/"wlanDeleteActive"/
                                           // "wlanOverwriteActive" (see /factoryReset/confirm)

    // Nutzlast der WLAN-Bestaetigungsaktionen, nur gueltig bei passender factoryResetPendingAction:
    // pendingWifiChangeIndex = zu loeschender Slot ("wlanDeleteActive"), pendingWifiSsid[]/pendingWifiPass[]
    // = neue WLAN-Liste aus dem Formular ("wlanOverwriteActive").

    // Payload of the WiFi confirmation actions, only valid with a matching factoryResetPendingAction:
    // pendingWifiChangeIndex = slot to delete ("wlanDeleteActive"), pendingWifiSsid[]/pendingWifiPass[] = new
    // WiFi list from the form ("wlanOverwriteActive").

    extern int pendingWifiChangeIndex;
    extern String pendingWifiSsid[MAX_WLAN];
    extern String pendingWifiPass[MAX_WLAN];

    struct WifiNetwork {
        String ssid;
        int rssi;
        int enc;
    };
    extern WifiNetwork availableNetworks[MAX_WLAN];
    extern int foundNetworkCount;
    extern bool isScanning;

    // Zeit, NTP, DCF77, RTC
    // Time, NTP, DCF77, RTC

    // NTP-Server-Port
    // NTP server port

    const int NTP_PORT = 123;

    // NTP-Paketgröße
    // NTP packet size

    const int NTP_PACKET_SIZE = 48;
    extern byte ntpPacket[NTP_PACKET_SIZE];


    // Interrupt auf CHANGE; processDcf77Bits() wertet nur die Dauer zwischen zwei Flanken aus - der Pegel
    // wird nie gelesen.

    // Interrupt on CHANGE; processDcf77Bits() only evaluates the duration between two edges - the level is
    // never read.

    extern volatile uint16_t dcf77Count; // Anzahl der empfangenen DCF77-Signale (wird in der ISR verändert)
                                      // Number of received DCF77 signals (modified in the ISR)

    extern volatile bool dcfTimeFound; // wird in loop()/updateDcf77Status() gesetzt, nicht mehr in der ISR gelesen
                                        // set in loop()/updateDcf77Status(), no longer read in the ISR

    // Gesetzt in processDcf77Bits(), nicht in der ISR: setLedOn/Off() liegen im Flash (Panic-Risiko bei
    // abgeschaltetem Flash-Cache), und nur processDcf77Bits() weiss ueber dcf77Confirmed, ob wirklich DCF77
    // erkannt wurde.

    // Set in processDcf77Bits(), not the ISR: setLedOn/Off() live in flash (panic risk with the flash cache
    // disabled), and only processDcf77Bits() knows via dcf77Confirmed whether DCF77 was actually recognized.

    extern volatile bool dcfLedTogglePending;

    // Zeitpunkt (millis()), zu dem der aktuelle Einmal-Blitz der LED wieder
    // ausgeschaltet werden soll; 0 = kein Blitz aktiv (siehe
    // DCF77_LED_BLINK_MS in config.h und die Abarbeitung in loop()).

    // Time (millis()) at which the LED's current one-shot flash is to be
    // switched off again; 0 = no flash active (see DCF77_LED_BLINK_MS in
    // config.h and the handling in loop()).

    extern unsigned long dcfLedOffAtMillis;

    extern bool dcfSyncLedEnabled; // per Checkbox abschaltbar (Default: an) - steuert nur, ob loop() das per
                                    // dcfLedTogglePending angeforderte Blinken ausfuehrt (siehe PK_DCF_SYNC_LED)
                                    // switchable via checkbox (default: on) - only controls whether loop() executes
                                    // the blink requested via dcfLedTogglePending (see PK_DCF_SYNC_LED)

    // Onboard-LED (blau, LED_BOARD) ganz abschaltbar (Default: an): aus = setLedOn() schaltet nie ein - weder beim
    // Start, bei HTTP-Zugriffen, beim DCF77-Empfang noch bei WPS (siehe PK_BOARD_LED).

    // Onboard LED (blue, LED_BOARD) switchable off completely (default: on): off = setLedOn() never switches it on -
    // neither at boot, on HTTP access, on DCF77 reception nor on WPS (see PK_BOARD_LED).

    extern bool boardLedEnabled;

    // Touch-Bedienung des Displays (ESP32-S3-Touch-LCD-4) - aus = alle Touch-Funktionen und die Hinweise in den Ecken entfallen
    // Touch control of the display (ESP32-S3-Touch-LCD-4) - off = all touch functions and the corner hints are disabled

    extern bool touchEnabled;
    extern bool cornerHintsActive; // Hinweise in den Ecken im Uhrbild zeigen (system_utils.cpp) / show the corner hints in the clock image (system_utils.cpp)

    extern time_t lastDcfSyncTime; // Unix-Zeitstempel der letzten erfolgreichen DCF77-Synchronisation (0 = noch nie)
                                // Unix timestamp of the last successful DCF77 sync (0 = never)

    extern unsigned long lastDcf77PulseChangeMillis; // millis()-Zeitpunkt der letzten beobachteten Aenderung von dcf77Count (siehe checkDcf77Health() in time_sync.cpp) -
                                                   // erkennt einen kompletten Empfangsausfall waehrend des Betriebs, da dcf77Count selbst nie auf 0 zurueckgesetzt wird
                                                   // millis() timestamp of the last observed change in dcf77Count (see checkDcf77Health() in time_sync.cpp) -
                                                   // detects a complete reception failure during operation, since dcf77Count itself is never reset to 0

    extern uint8_t dcf77PlausiblePulseStreak; // Zahl aufeinanderfolgender dcf77Count-Aenderungen mit plausiblem Abstand
                                            // (siehe DCF77_PRESENCE_MAX_GAP_MS, checkDcf77Health()) - filtert Rauschen
                                            // number of consecutive dcf77Count changes with a plausible gap (see
                                            // DCF77_PRESENCE_MAX_GAP_MS, checkDcf77Health()) - filters out noise

    extern bool dcf77Confirmed; // wird einmalig true bei DCF77_PRESENCE_MIN_STREAK (config.h) - bestimmt, ob der
                                 // DCF77-Eintrag in der Topbar angezeigt wird; danach nie zurueckgesetzt
                                 // becomes true exactly once at DCF77_PRESENCE_MIN_STREAK (config.h) - decides
                                 // whether the DCF77 topbar entry is shown; never reset afterwards

    // Eigener Bit-Fortschritt fuer /dcf77 und die Zeituebernahme: die ISR schreibt nur Zeitstempel in den
    // Ringpuffer, dekodiert wird in loop(). 64 Plaetze (256 Byte) ueberbruecken auch laengere Blockaden durch
    // NTP/Web/LittleFS.

    // Own bit progress for /dcf77 and the time takeover: the ISR only writes timestamps to the ring buffer,
    // decoding happens in loop(). 64 slots (256 bytes) also bridge longer stalls from NTP/web/LittleFS.

#define DCF77_EDGE_BUFFER_SIZE 64
    extern volatile unsigned long dcf77EdgeMillis[DCF77_EDGE_BUFFER_SIZE];

    extern volatile uint8_t dcf77EdgeHead; // naechster freier Schreibindex - NUR von der ISR veraendert
                                        // next free write index - ONLY changed by the ISR
    extern volatile uint32_t dcf77EdgeDropped; // Anzahl verworfener Flanken bei vollem Puffer - uint32_t, da uint8_t nach 256 ueberlaufen wuerde (Diagnose).
                                            // number of edges dropped when the buffer was full - uint32_t, since uint8_t would wrap after 256 (diagnostic).
    extern uint8_t dcf77EdgeTail; // naechster zu lesende Index - NUR im Hauptthread (loop()) veraendert
                               // next index to read - ONLY changed on the main thread (loop())

#define DCF77_TELEGRAM_BITS 59

    // dcf77Bits ist nach Rasterposition (dcf77Phase, 0..59) indiziert, nicht
    // nach Sekunde - die Zuordnung Position->Sekunde steht erst nach Fund der
    // Minutenmarke fest, sonst bliebe die /dcf77-Seite bis dahin leer.

    // dcf77Bits is indexed by grid position (dcf77Phase, 0..59), not by
    // second - the mapping position->second is only fixed once the minute
    // marker is found, otherwise the /dcf77 page would stay empty until then.

#define DCF77_GRID_SLOTS 60
    extern int8_t dcf77Bits[DCF77_GRID_SLOTS]; // 0/1 je Rasterposition der laufenden Minute, -1 = (noch) unbekannt - NUR Hauptthread
                                        // 0/1 per grid position of the running minute, -1 = unknown (yet) - main thread only
    extern uint8_t dcf77BitIndex; // Position (Sekunde) des naechsten erwarteten Impulses - bei einer verlorenen Sekunde
                               // bleibt dort eine Luecke (-1), der Index zeigt trotzdem korrekt weiter (siehe processDcf77Bits())
                               // position (second) of the next expected pulse - a lost second leaves a hole (-1)
                               // there, the index still points correctly onward (see processDcf77Bits())

    extern bool dcf77Synced; // true, sobald Minutenmarke erkannt und seither jeder Abstand ins Sekundenraster passte;
                               // Impulse werden nur dann als Bits abgelegt. Faellt bei Rasterbruch oder unmoeglichem
                               // Telegramm (Bit 0/20, siehe decodeDcf77Telegram()) wieder auf false zurueck.

                               // true once the minute marker is detected and every distance since fit the second
                               // grid; pulses are only then stored as bits. Falls back to false on a grid break or
                               // an impossible telegram (bit 0/20, see decodeDcf77Telegram()).

    // dcf77Phase ist eine freilaufende Rasterposition 0..59. Die Minutenmarke wird statistisch erkannt (die
    // einzige Position, an der IMMER ein Impuls fehlt) - robust auch bei Stoerungen mit haeufigen 2s-Luecken.

    // dcf77Phase is a free-running grid position 0..59. The minute marker is detected statistically (the only
    // position where a pulse is ALWAYS missing) - robust even with interference causing frequent 2s gaps.

    extern uint8_t dcf77Phase;

    extern uint8_t dcf77MarkerMiss[60]; // wie oft an dieser Rasterposition ein Impuls fehlte
                                         // how often a pulse was missing at this grid position
    extern uint8_t dcf77MarkerHit[60]; // wie oft an dieser Rasterposition ein Impuls ankam
                                         // how often a pulse arrived at this grid position

    extern int8_t dcf77MarkerPos; // erkannte Rasterposition der Minutenmarke, -1 = noch unbekannt
                                // detected grid position of the minute marker, -1 = not known yet

    extern int8_t dcf77LastSecond; // zuletzt belegte Sekunde der Minute, fuer die Erkennung des Minutenwechsels
                                 // last second of the minute filled in, used to detect the minute change

    extern uint8_t dcf77StructFails; // aufeinanderfolgende Telegramme mit unmoeglichen Festbits (Bit 0 / Bit 20) -
                                  // ab DCF77_STRUCT_FAIL_LIMIT gilt die erkannte Marke als falsch und wird verworfen
                                  // consecutive telegrams with impossible fixed bits (bit 0 / bit 20) - from
                                  // DCF77_STRUCT_FAIL_LIMIT on, the detected marker counts as wrong and is discarded

    // Diagnosewerte fuer die /dcf77-Seite: machen sichtbar, was der Empfaenger
    // tatsaechlich liefert - ohne Oszilloskop sonst nicht zu unterscheiden, ob
    // der Dekoder falsch rechnet oder keine brauchbaren Impulse ankommen.

    // Diagnostic values for the /dcf77 page: make visible what the receiver
    // actually delivers - without an oscilloscope there's no other way to
    // tell whether the decoder computes wrongly or no usable pulses arrive.

    extern uint32_t dcf77PulsesSeen; // erkannte Impulse seit dem Start / pulses detected since start
    extern uint32_t dcf77PulsesMissed; // uebersprungene Rasterpositionen / grid positions skipped
    extern uint32_t dcf77PhaseBreaks; // wie oft das Sekundenraster verlorenging / how often the second grid was lost

#define DCF77_DIAG_SLOTS 12
    extern uint16_t dcf77DiagWidth[DCF77_DIAG_SLOTS]; // Impulsdauer in ms / pulse width in ms
    extern uint16_t dcf77DiagGap[DCF77_DIAG_SLOTS]; // Abstand zum vorigen Impulsanfang in ms / distance to the previous pulse start in ms
    extern uint8_t dcf77DiagIdx;
    extern uint8_t dcf77DiagCount;

    // Ergebnis der letzten vollstaendig dekodierten Minute - bleibt bei
    // Paritaetsfehler erhalten (valid=false statt verwerfen), damit die
    // Live-Seite auch fehlerhaften Empfang zeigen kann.

    // Result of the last fully decoded minute - kept on a parity error
    // (valid=false instead of discarding it), so the live page can also
    // show faulty reception.

    struct Dcf77Decoded {
        bool valid = false;       // alle drei Paritaeten (Minute/Stunde/Datum) korrekt
                                  // all three parities (minute/hour/date) correct
        uint8_t minute = 0, hour = 0, day = 0, month = 0, weekday = 0;
        uint16_t year = 0;
        bool dst = false;         // Sommerzeit aktiv (Bit 17)
                                  // daylight saving time active (bit 17)
        bool callBit = false;     // Bit 15 - Anrufbit / unregelmaessige Aussendung
                                  // bit 15 - call bit / irregular transmission
        bool parityMinOk = false, parityHourOk = false, parityDateOk = false;
        uint8_t repairedBits = 0; // wie viele fehlende Bits aus Paritaet/Festwerten rekonstruiert wurden
                                  // how many missing bits were reconstructed from parity/fixed values
        unsigned long decodedAtMillis = 0; // millis() beim Minutenanfang dieses Telegramms, 0 = noch nie
                                           // millis() at this telegram's minute start, 0 = never
    };
    extern Dcf77Decoded dcf77LastDecoded;

    // Letzte bestaetigte Dekodierung als Referenz: mit aus der Paritaet ergaenzten Bits muss die Zeit exakt
    // zur vorherigen plus verstrichenen Minuten passen - ein vollstaendiges Telegramm braucht das nicht.

    // Last confirmed decoding as a reference: with bits filled from parity, the time must exactly match the
    // previous one plus elapsed minutes - a complete telegram does not need this.

    extern time_t dcf77PrevEpoch;
    extern unsigned long dcf77PrevAtMillis;


    // Ist der eigene NTP-Server (siehe startNtpServer() in time_sync.cpp) an
    // Port 123 gebunden - fuer korrektes Logging/Statusanzeige, statt den
    // Erfolg von udp.begin() blind anzunehmen.

    // Is the own NTP server (see startNtpServer() in time_sync.cpp) bound to
    // port 123 - for correct logging/status display, instead of blindly
    // assuming udp.begin() succeeded.

    extern bool ntpServerRunning;

    // Diagnosezaehler fuer den eigenen NTP-Server (siehe /status): unterscheiden,
    // ob Anfragen ankommen (Netz/Firewall) oder nur unbeantwortet bleiben
    // (keine gueltige Systemzeit).

    // Diagnostic counters for the own NTP server (see /status): distinguish
    // whether requests arrive at all (network/firewall) or just go unanswered
    // (no valid system time).

    extern uint32_t ntpRequestsReceived;
    extern uint32_t ntpRepliesSent;

    extern unsigned long lastNTPUpdate; // Zeitpunkt des letzten RTC-Updates
                                     // Timestamp of the last RTC update
    extern unsigned long lastDCFUpdate; // Wartezeit nach DCF77-Update, bevor RTC aktualisiert wird (ms)
                                     // Wait time after a DCF77 update before the RTC is updated (ms)
    extern unsigned long lastRTCUpdate; // Zeitpunkt des letzten RTC-Updates
                                     // Timestamp of the last RTC update
    extern bool serialTimeSet; // Uhrzeit per USB gesetzt (handleSerialTime()) - beendet das Warten auf DCF77 beim Start
                                     // time set via USB (handleSerialTime()) - ends the wait for DCF77 at boot
    extern volatile unsigned long lastNtpSuccessMillis; // Zeitpunkt (millis()) der letzten erfolgreichen NTP-Sync (0 = nie) -
                                            // unterscheidet echten Erfolg vom irrefuehrenden true-Rueckgabewert von
                                            // setupNTP() ohne WLAN; bleibt er unveraendert, springt DCF77 ein (time_sync.cpp).
                                            // volatile seit der Async-Umstellung (startNtpSyncTask()): wird jetzt aus der
                                            // Sync-Task heraus geschrieben und von pollNtpSyncTask() auf dem Hauptthread gelesen.

    // lastNtpSuccessMillis unterscheidet echten NTP-Erfolg vom irrefuehrenden true von setupNTP() ohne WLAN.
    // Die NTP-Sync laeuft in einer eigenen kurzlebigen Task, damit ihre DNS-/UDP-Wartezeiten weder loop()
    // noch den Webserver blockieren.

    // lastNtpSuccessMillis tells a real NTP success from setupNTP()'s misleading true without WiFi. The NTP
    // sync runs in its own short-lived task, so its DNS/UDP wait times block neither loop() nor the web
    // server.

    extern TaskHandle_t ntpSyncTaskHandle;
    extern volatile bool ntpSyncTaskRunning; // Task laeuft gerade
                                              // task currently running
    extern volatile bool ntpSyncTaskDone; // Task fertig, Ergebnis in ntpSyncTaskResult
                                              // task finished, result in ntpSyncTaskResult
    extern volatile bool ntpSyncTaskResult; // Rueckgabewert von setupNTP() des letzten Laufs (siehe Hinweis oben: bei
                                              // fehlendem WLAN irrefuehrend true - pollNtpSyncTask() prueft daher zusaetzlich lastNtpSuccessMillis)
                                              // return value of the last setupNTP() run (see note above: misleadingly
                                              // true without WiFi - pollNtpSyncTask() therefore also checks lastNtpSuccessMillis)
    extern unsigned long ntpSyncCheckBeforeMillis; // millis() unmittelbar VOR dem Start der laufenden Sync-Task -
                                                // Referenzwert fuer den lastNtpSuccessMillis-Vergleich in pollNtpSyncTask()
                                                // millis() immediately BEFORE starting the current sync task -
                                                // reference value for the lastNtpSuccessMillis comparison in pollNtpSyncTask()
    extern String ntpSyncCheckLabel; // Label fuer die Logausgabe ("Initial sync"/"Periodic sync"/"Manual sync"), von pollNtpSyncTask() ausgewertet
                                   // label for the log output ("Initial sync"/"Periodic sync"/"Manual sync"), evaluated by pollNtpSyncTask()

    extern struct tm timeinfo;

    // Rocrail-Modellzeit (siehe rocrail_client.cpp): eigenstaendige,
    // ggf. beschleunigte Zeitstruktur, komplett getrennt von "timeinfo"
    // oben. renderClockFrame() liest je nach rocrailEnabled aus der einen oder anderen Struktur.

    // Rocrail model time (see rocrail_client.cpp): an independent, possibly
    // accelerated time struct, fully separate from "timeinfo" above.
    // renderClockFrame() reads from whichever struct applies, depending on rocrailEnabled.

    extern struct tm rocrailTimeinfo;
    extern bool rocrailEnabled; // per Tab-Schalter/Preferences aktiviert
                                    // enabled via the tab switch/preferences
    extern bool rocrailConnected; // TCP-Verbindung zum Server aktuell offen
                                    // TCP connection to the server currently open
    extern String rocrailServerHost; // aktuell aktiver Server - siehe rocrailServerList[] unten;
                                    // wird beim Speichern aus dem angehakten Listeneintrag uebernommen.
                                    // currently active server - see rocrailServerList[] below;
                                    // taken over from the checked list entry when saving.
    extern uint16_t rocrailServerPort;

    // Kopie von rocrailServerHost/-Port fuer die Connect-Task: ein Arduino String ist nicht thread-sicher,
    // und die Einstellungsseite kann ihn jederzeit neu setzen, waehrend die Task gerade verbindet.

    // Copy of rocrailServerHost/-port for the connect task: an Arduino String is not thread-safe, and the
    // settings page can reassign it at any time while the task is connecting.

    extern char rocrailServerHostSnapshot[64];
    extern uint16_t rocrailServerPortSnapshot;

    extern char rocrailServerList[MAX_WLAN][64]; // Hostname/IP je Listenplatz (siehe pkRocrailServerHost())
                                                 // hostname/IP per list slot (see pkRocrailServerHost())
    extern uint16_t rocrailServerPortList[MAX_WLAN]; // Port je Listenplatz (siehe pkRocrailServerPort())
                                                 // port per list slot (see pkRocrailServerPort())

    // Anlagenname je Listenplatz - rein manuell, vom Nutzer frei editierbar
    // zur eigenen Orientierung (z.B. bei mehreren Servern). RCP meldet
    // keinen Anlagennamen, daher keine automatische Uebernahme.

    // Layout name per list slot - purely manual, freely editable by the
    // user for their own reference (e.g. with several servers). RCP does
    // not report a layout name, so there is no automatic takeover.

    extern char rocrailServerNameList[MAX_WLAN][40];

    extern int rocrailActiveServerIndex; // 0-basierter Index des per Haekchen ausgewaehlten
                                                 // Listenplatzes, -1 = keiner ausgewaehlt
                                                 // 0-based index of the list slot selected via the
                                                 // checkmark, -1 = none selected
    extern uint8_t rocrailDivider; // Modellzeit-Beschleunigungsfaktor vom Server
                                    // model-time acceleration factor from the server
    extern bool rocrailFrozen; // per <clock state="freeze"/> angehalten (siehe processRocrailClockPayload())
                                    // paused via <clock state="freeze"/> (see processRocrailClockPayload())
    extern uint8_t rocrailBrightness; // zuletzt vom Server per <clock bri="..."/> gemeldeter
                                        // Helligkeitswert (0-255, siehe processRocrailClockPayload())
    extern bool rocrailBrightnessKnown; // true, sobald einmal ein bri-Wert empfangen wurde - erst
                                         // dann uebernimmt updateBrightness() ihn; sendet der Server
                                         // nie bri, bleibt die lokale Helligkeitssteuerung dauerhaft aktiv.

                                        // true once a bri value has been received - only then does
                                        // updateBrightness() take it over; if the server never sends
                                        // bri, local brightness control stays active permanently.
    extern WiFiClient rocrailClient;
    extern String rocrailRxBuffer; // Empfangspuffer fuer XML-Tag-Bruchstuecke
                                    // receive buffer for XML tag fragments
    extern uint8_t rocrailLogChunkCount; // Zaehler fuer die Diagnose-Logeintraege der ersten
                                      // 5 empfangenen RCP-Rohdaten-Haeppchen (siehe pollRocrailClient())
                                      // counter for the diagnostic log entries of the first 5
                                      // received RCP raw data chunks (see pollRocrailClient())
    extern uint8_t rocrailClockLogCount; // Zaehler fuer die routinemaessige "[ROCRAIL] clock ..."-Zeile
                                      // (siehe processRocrailClockPayload()) - nur die ersten 2 Updates,
                                      // danach nur noch bei Divider-/Pause-Wechsel
                                      // counter for the routine "[ROCRAIL] clock ..." line (see
                                      // processRocrailClockPayload()) - only the first 2 updates,
                                      // afterwards only on divider/pause changes
    extern uint8_t rocrailConnectFailLogCount; // Zaehler fuer "Connecting to.."/"did not respond"
                                            // (siehe connectRocrailClient()/pollRocrailConnectTask()) -
                                            // nur die ersten 2 Fehlschlaege einer Ausfall-Serie loggen,
                                            // die Versuche selbst laufen unabhaengig davon stumm weiter;
                                            // wird bei erfolgreicher Verbindung zurueckgesetzt
                                            // counter for "Connecting to.."/"did not respond" (see
                                            // connectRocrailClient()/pollRocrailConnectTask()) - only
                                            // log the first 2 failures of a failure streak, the actual
                                            // attempts keep running silently regardless; reset on a
                                            // successful connection
    extern unsigned long rocrailLastClockMillis; // millis() beim letzten <clock>-Update (0 = noch keins)
                                               // millis() at the last <clock> update (0 = none yet)
    extern float rocrailDisplaySeconds; // aktuell angezeigte Modellzeit in Sekunden seit
                                               // Mitternacht (fliessend, siehe advanceRocrailTime())
                                               // currently displayed model time in seconds since
                                               // midnight (continuous, see advanceRocrailTime())
    extern unsigned long rocrailLastAdvanceMillis; // millis() beim letzten Fortschreiben (advanceRocrailTime())
                                                // millis() at the last advance step (advanceRocrailTime())
    extern float rocrailDriftSeconds; // verbleibende, sanft auszugleichende Abweichung zur
                                               // zuletzt gemeldeten Server-Zeit (siehe advanceRocrailTime()/
                                               // processRocrailClockPayload()) - 0 = keine Korrektur noetig

                                               // remaining deviation to the last reported server time,
                                               // eased in smoothly (see advanceRocrailTime()/
                                               // processRocrailClockPayload()) - 0 = no correction needed
    extern float rocrailSecFrac; // Sekunde mit Nachkommastellen (0.0-59.999) fuer die
                                               // glatte, nicht tickende Zeigerbewegung im Rocrail-
                                               // Modus (siehe advanceRocrailTime()/renderClockFrame())

                                               // second with a fractional part (0.0-59.999) for the
                                               // smooth, non-ticking hand motion in Rocrail mode (see
                                               // advanceRocrailTime()/renderClockFrame())
    extern unsigned long rocrailLastConnectAttemptMillis;
    extern TaskHandle_t rocrailConnectTaskHandle; // eigene, kurzlebige Task fuer den
                                                  // (blockierenden) Connect-Versuch, damit
                                                  // loop() dabei nicht blockiert (siehe rocrail_client.cpp)

                                                  // own short-lived task for the (blocking) connect
                                                  // attempt, so loop() doesn't block during it (see rocrail_client.cpp)
    extern volatile bool rocrailConnectTaskRunning; // Task laeuft gerade
                                                     // task is currently running
    extern volatile bool rocrailConnectTaskDone; // Task fertig, Ergebnis in rocrailConnectTaskResult
                                                     // task finished, result in rocrailConnectTaskResult
    extern volatile bool rocrailConnectTaskResult; // Ergebnis des letzten Connect-Versuchs
                                                     // result of the last connect attempt

    extern String timezone;

    // Kopie von timezone fuer die NTP-Sync-Task: der String wird auch von Web-Handlern geschrieben und ist
    // nicht thread-sicher - die Task liest nur diese auf dem Hauptthread angelegte Kopie.

    // Copy of timezone for the NTP sync task: the String is also written by web handlers and is not
    // thread-safe - the task only reads this copy created on the main thread.

    extern char timezoneSnapshot[64];

    extern char ntpServers[MAX_WLAN][64];

    // Kopie von ntpServers[] fuer die NTP-Sync-Task, auf dem Hauptthread angelegt - die Weboberflaeche
    // koennte das Array sonst gleichzeitig beschreiben.

    // Copy of ntpServers[] for the NTP sync task, created on the main thread - the web UI could otherwise
    // write to the array at the same time.

    extern char ntpServersSnapshot[MAX_WLAN][64];

#define RTC_NOT_AVAILABLE 0
#define RTC_AVAILABLE 1
#define RTC_AVAILABLE_BUT_INVALID 2

    extern int rtcOk;

    extern String i2cAddr;

    // Zifferblatt / Display
    // Clock face / Display

    extern String tftType;

    extern LGFX_Sprite backgroundSprite;
    extern LGFX_Sprite hourHandSprite;
    extern LGFX_Sprite minuteHandSprite;
    extern LGFX_Sprite secondHandSprite;

    // Sprites fuer Status-/Boot-Text, nur fuer den GC9D01-Software-Rotations-
    // Workaround noetig (dort wird die HW-Rotation uebersprungen) - auf
    // anderen Boards ungenutzt. Werden erst bei Bedarf angelegt (kein (P)SRAM-Verschwenden).

    // Sprites for status/boot text, only needed for the GC9D01 software
    // rotation workaround (there, HW rotation is skipped) - unused on other
    // boards. Created lazily on first use (avoids wasting (P)SRAM).

    extern LGFX_Sprite statusSprite1;
    extern LGFX_Sprite statusSprite2;
    extern bool statusSprite1Created;
    extern bool statusSprite2Created;

    extern String selectedBackground;

    extern bool stationMode; // "wartet auf 12": Sekundenzeiger eilt in komprimierter Zeit voraus und pausiert oben (siehe display.cpp)
                      // "waits at 12": second hand races ahead in compressed time and pauses at the top (see display.cpp)
    extern bool smoothMinute;
    extern bool smoothSecond; // Darstellungsstil des Sekundenzeigers: schwingend (true) oder tickend (false) -
                       // unabhaengig von stationMode (siehe renderClockFrame() in display.cpp)
                       // second hand rendering style: smooth/swinging (true) or ticking (false) -
                       // independent of stationMode (see renderClockFrame() in display.cpp)
    extern bool showSecondHand;

    extern int hourHandWidth;
    extern int minuteHandWidth;
    extern int secondHandWidth;

    // nabe
    // hub

    extern uint16_t hubColor;
    extern uint8_t hubSize;

    extern bool firstRun;
    extern bool firstRun2; // wie firstRun, aber fuer Display 2 (CS2) - siehe renderClockFrame() in display.cpp
                           // like firstRun, but for Display 2 (CS2) - see renderClockFrame() in display.cpp

    // Rotation 0-3 (0/90/180/270 Grad) oder TFT_ROTATION_NA (Display nicht angeschlossen,
    // siehe config.h) - fuer n.a. wird kein Zifferblatt/Zeiger berechnet, Status-Meldungen
    // laufen aber weiter. Abfrage ueber isDisplayConnected()/effectiveRotation(), nie direkt gegen den Wert.

    // Rotation 0-3 (0/90/180/270 degrees) or TFT_ROTATION_NA (display not connected,
    // see config.h) - for n.a. no face/hands are calculated, but status messages still
    // run. Query via isDisplayConnected()/effectiveRotation(), never compare the value directly.

    extern uint8_t tftRotation1;
    extern bool autoRotation; // Display 1 dreht sich nach dem Lagesensor (imu_rotation.cpp, nur S3)
                               // display 1 turns according to the motion sensor (imu_rotation.cpp, S3 only)
    extern uint8_t tftRotation2; // Rotation von Display 2 (CS2) - eigener Wert, damit beide Displays
                                                  // unterschiedlich ausgerichtet montiert sein koennen (siehe main.cpp/webserver_routes.cpp)
                                                  // rotation of Display 2 (CS2) - its own value, so both displays can be
                                                  // mounted with a different orientation (see main.cpp/webserver_routes.cpp)


    extern uint16_t rowBuffer[CLOCK_MAX]; // Groesse fuer den groessten Displaytyp, genutzt werden CLOCK_WIDTH Eintraege
                                   // sized for the largest display type, CLOCK_WIDTH entries are used

    // Steuert nur den GC9D01-Software-Rotations-Workaround (nicht "ist PSRAM
    // vorhanden" allgemein - dafuer wird ueberall psramFound() aufgerufen).
    // Nur bei Displaytypen mit swRotation (DISPLAY_GEOMETRY) moeglich.

    // Controls only the GC9D01 software rotation workaround (not "is PSRAM
    // available" in general - psramFound() is called directly for that).
    // Only possible for display types with swRotation (DISPLAY_GEOMETRY).

    extern bool gc9d01SwRotation;

    extern uint16_t* clockFaceBuffer;

    // Ohne PSRAM (ESP32-C6) kein Rohbild: das Zifferblatt als RLE-Daten im RAM (clockFaceRle, nur kleine
    // Dateien) oder direkt aus der Datei (clockFaceRlePath), "" = erzeugtes Standard-Zifferblatt. Siehe
    // prepareClockFaceRle() in display.cpp.

    // Without PSRAM (ESP32-C6) no raw image: the clock face as RLE data in RAM (clockFaceRle, small files only)
    // or straight from the file (clockFaceRlePath), "" = generated default clock face. See prepareClockFaceRle()
    // in display.cpp.

    extern uint8_t* clockFaceRle;
    extern size_t clockFaceRleSize;
    extern String clockFaceRlePath;
    extern bool clockFaceRleReady;

    // Fertiges "Zifferblatt + Stunden-/Minutenzeiger" je Display als Sprite: nur bei Aenderung neu aufgebaut
    // (Zeiger per pushRotatedWithAA()), sonst per pushSprite() kopiert - spart Zeit fuer den Sekundenzeiger.

    // Finished "clock face + hour/minute hand" per display as a sprite: rebuilt only on change (hands via
    // pushRotatedWithAA()), otherwise copied via pushSprite() - frees time for the second hand.

    struct HandComposite {
        LGFX_Sprite* sprite = nullptr;
        bool valid = false;
        float hourAngle = 0.0f;
        float minuteAngle = 0.0f;
        uint8_t rotation = 0xFF;
        uint8_t brightness = 0xFF;
        uint32_t assetGeneration = 0xFFFFFFFF;
        bool allocationFailed = false; // nach einem Fehlschlag nicht bei jedem Tick erneut versuchen
                                       // don't retry on every tick after a failure
        bool hands = true;             // mit Stunden-/Minutenzeiger (false: nur Zifferblatt, Sekundenfeld)
                                       // with hour/minute hand (false: clock face only, seconds subdial)
    };
    extern HandComposite handComposite[2]; // [0] = Display 1, [1] = Display 2

    // Zuletzt tatsaechlich gesendeter Frame pro Display - renderClockFrame()
    // ueberspringt Zeichnen + SPI-Push, solange sich keiner dieser Werte
    // aendert (tickender Zeiger: Bild aendert sich nur ~1x pro Sekunde).

    // Last frame actually pushed per display - renderClockFrame() skips
    // drawing + SPI push as long as none of these values change (ticking
    // hand: the image only changes ~once per second).

    struct ClockFrameKey {
        bool valid = false;
        float hourAngle = 0.0f;
        float minuteAngle = 0.0f;
        float secondAngle = 0.0f;
        bool drawSecond = false;
        bool drawHub = false;
        uint8_t hubSize = 0;
        uint16_t hubColor = 0;
        uint8_t brightness = 0;
        uint8_t rotation = 0xFF;
        bool smoothSecond = false;
        uint32_t assetGeneration = 0;
    };
    extern ClockFrameKey lastClockFrame[2]; // [0] = Display 1, [1] = Display 2

    // Teil-Aktualisierung: aendert sich nur der Sekundenzeiger, sendet renderClockFrame() nur das Rechteck um
    // alten und neuen Zeiger samt Nabe. Rechteck des letzten Frames - der Spiegelbetrieb schickt Display 2
    // dasselbe.

    // Partial update: if only the second hand changes, renderClockFrame() sends just the rectangle around old
    // and new hand plus hub. Rectangle of the last frame - mirror mode sends display 2 the same one.

    extern bool lastRenderPartial;
    extern bool partialUpdateEnabled; // zur Diagnose abschaltbar: /api/partialUpdate?enabled=0 (nicht gespeichert)
                                      // switchable for diagnosis: /api/partialUpdate?enabled=0 (not stored)
    extern uint32_t compositeBuildCount; // zaehlt Neuaufbauten des Zwischenbilds (buildHandComposite())
                                      // counts composite rebuilds (buildHandComposite())

    // Bildrate/Zeichenzeit fuer die Statusseite (recordRenderFrame() in
    // display.cpp): nur tatsaechlich gesendete Frames, alle 5 s ausgewertet.

    // Frame rate/render time for the status page (recordRenderFrame() in
    // display.cpp): only frames actually sent, evaluated every 5 s.

    struct RenderStats {
        uint32_t windowStartMillis = 0;
        uint32_t frames = 0, partialFrames = 0;
        uint64_t sumMicros = 0;
        uint32_t maxMicros = 0;
        float fps = 0.0f, avgMs = 0.0f, maxMs = 0.0f, partialPercent = 0.0f; // zuletzt ausgewertetes Fenster
                                                                             // last evaluated window
    };
    extern RenderStats renderStats;
    extern int32_t lastRenderRect[4]; // x, y, w, h

    // Etwas anderes (Status-/Bootmeldung, Bestaetigungscode) hat aufs Display
    // gezeichnet - naechster Uhr-Frame muss voll neu gesendet werden, auch
    // wenn sich die Zeigerwinkel nicht geaendert haben (siehe beginStatusDraw()).

    // Something else (status/boot message, confirmation code) drew on the
    // display - the next clock frame must be sent in full, even if the hand
    // angles didn't change (see beginStatusDraw()).

    extern bool clockFrameDirty[2];

    // Je Display: Streifen fuer Uhrzeit/Datum neben der Uhr (nur ILI9341, drawInfoStrips()) komplett neu
    // zeichnen - gesetzt, wenn eine Meldung das Display ueberschrieben hat.

    // Per display: redraw the time/date strip next to the clock (ILI9341 only, drawInfoStrips()) completely -
    // set when a message drew over the display.

    extern bool infoStripDirty[2];

    // Streifen fuer Uhrzeit/Datum, eingestellt im Zifferblatt-Designer: Farben als RGB888, Schriftart (Index in
    // STRIP_FONTS, display.cpp) und Mitte der Zeilen im Streifen (hochkant) - -1 = automatisch. stripAuto* sind die
    // zuletzt berechneten automatischen Positionen fuer den Designer.

    // Time/date strip, set in the clock face designer: colors as RGB888, font (index into STRIP_FONTS,
    // display.cpp) and centre of the lines in the strip (portrait) - -1 = automatic. stripAuto* are the last
    // computed automatic positions for the designer.

    extern uint32_t stripBgRgb;
    extern uint32_t stripFgRgb;
    extern uint8_t stripFont;
    extern bool stripBlink; // Doppelpunkt der Uhrzeit "H:MM" blinkt im Sekundentakt
                              // colon of the time "H:MM" blinks every second
    extern bool stripBefore; // Streifen ueber der Uhr (quer: links davon) statt darunter (quer: rechts)
                              // strip above the clock (landscape: left of it) instead of below (landscape: right)
    extern int16_t stripTimeX;
    extern int16_t stripTimeY;
    extern int16_t stripDateX;
    extern int16_t stripDateY;
    extern int16_t stripAutoTimeY;
    extern int16_t stripAutoDateY;
    extern LGFX_Sprite infoStripSprite; // Streifen wird hier gezeichnet und in einem Stueck gesendet (kein Flackern)
                                       // the strip is drawn here and sent in one piece (no flicker)
    extern bool infoStripSpriteCreated;

    // Streifen-Grafik des aktiven Zifferblatts (strip_<Name>.bmp, hochkant, RGB565) - stripImageFor ist der
    // geladene Pfad, "?" = neu laden (nach Upload, Loeschen, Umbenennen).

    // Strip graphic of the active clock face (strip_<name>.bmp, portrait, RGB565) - stripImageFor is the loaded
    // path, "?" = reload (after upload, delete, rename).

    extern uint16_t* stripImage;
    extern String stripImageFor;

    // Zeit- und Datumsformat des Streifens (PK_STRIP_TIME_FMT/PK_STRIP_DATE_FMT) und die VLW-Schriften aus dem
    // Designer (stripfont_time.vlw/stripfont_date.vlw im PSRAM, stripVlwStale = neu laden). stripVlwName und die
    // Groessen merkt sich nur der Designer.

    // Time and date format of the strip (PK_STRIP_TIME_FMT/PK_STRIP_DATE_FMT) and the VLW fonts from the
    // designer (stripfont_time.vlw/stripfont_date.vlw in PSRAM, stripVlwStale = reload). stripVlwName and the
    // sizes are only remembered for the designer.

    extern uint8_t stripTimeFmt;
    extern uint8_t stripSeconds; // Sekunden in der Uhrzeit: 0 = automatisch, 1 = ohne, 2 = mit
                              // seconds in the time: 0 = automatic, 1 = without, 2 = with
    extern uint8_t stripDateFmt;
    extern uint8_t stripWeekdayFmt; // Wochentag: 0 = ausgeschrieben, 1 = kurz (Mo) / weekday: 0 = written out, 1 = short (Mon)
    extern uint8_t* stripVlwTime;
    extern uint8_t* stripVlwDate;
    extern bool stripVlwStale;
    extern uint32_t stripVlwGeneration;
    extern String stripVlwName;
    extern uint8_t stripVlwTimeSize;
    extern uint8_t stripVlwDateSize;

    // Groesse der eingebauten Streifen-Schrift in Prozent (STRIP_SCALE_MIN..MAX), getrennt fuer Uhrzeit und Datum.
    // VLW-Schriften haben stattdessen ihre Pixelgroesse (stripVlwTimeSize/stripVlwDateSize).

    // Size of the built-in strip font in percent (STRIP_SCALE_MIN..MAX), separate for time and date. VLW fonts
    // have their pixel size instead (stripVlwTimeSize/stripVlwDateSize).

    // Drehpunkt des Sekundenzeigers im Zifferblatt (Anhang der Zifferblatt-Datei, siehe readFaceSecPivot()), -1 = Mitte.
    // faceSettingsFor: Zifferblatt, dessen Einstellungen geladen sind ("?" = neu laden).

    // Pivot of the second hand on the clock face (trailer of the clock face file, see readFaceSecPivot()), -1 =
    // centre. faceSettingsFor: clock face whose settings are loaded ("?" = reload).

    extern int16_t secPivotX;
    extern int16_t secPivotY;

    // Drehpunkt von Stunden- und Minutenzeiger (zusammen) im Zifferblatt, -1 = Mitte, und Lage des Sekundenzeigers
    // (0 = automatisch: ueber den anderen Zeigern, wenn sein Drehpunkt gleich ist, sonst darunter; 1 = immer ueber,
    // 2 = immer unter den anderen Zeigern) - ebenfalls im Anhang der Zifferblatt-Datei.

    // Pivot of the hour and minute hand (together) on the clock face, -1 = centre, and position of the second hand
    // (0 = automatic: over the other hands if its pivot is the same, otherwise below; 1 = always over, 2 = always
    // below the other hands) - also in the trailer of the clock face file.

    extern int16_t hmPivotX;
    extern int16_t hmPivotY;
    extern uint8_t secOrder;
    extern String faceSettingsFor;

    // Drehpunkte und Reihenfolge eines Zifferblatts (Anhang der Datei) / pivots and order of a clock face (file trailer)
    struct FacePivots {
        int16_t secX = -1, secY = -1; // Sekundenzeiger / second hand
        int16_t hmX = -1, hmY = -1;   // Stunden- und Minutenzeiger / hour and minute hand
        uint8_t order = 0;            // Lage des Sekundenzeigers / position of the second hand
    };

    // Wo welcher Zeiger sitzt (in Zifferblatt-Pixeln, ohne Rotation): makeHandPlan() in display.cpp
    // Where which hand sits (in clock face pixels, without rotation): makeHandPlan() in display.cpp
    struct HandPlan {
        float hmX, hmY, secX, secY;
        bool hmCustom;  // Drehpunkt von Stunde/Minute nicht in der Mitte / pivot of hour/minute not in the centre
        bool secOwn;    // Sekundenzeiger mit eigenem Drehpunkt / second hand with a pivot of its own
        bool secUnder;  // Sekundenzeiger unter den anderen Zeigern / second hand below the other hands
    };

    extern String stripSettingsFor; // Zifferblatt, dessen Streifen-Einstellungen geladen sind ("?" = neu laden)
                                   // clock face whose strip settings are loaded ("?" = reload)
    extern uint8_t stripTimeScale;
    extern uint8_t stripDateScale;
    extern bool stripShowTime; // Uhrzeit bzw. Datum im Streifen ein-/ausgeschaltet
    extern bool stripShowDate; // time or date in the strip switched on/off

    // Wochentag ausgeschrieben (Sprache der Weboberflaeche): ab Werk aus, eigene Groesse und Position. Die
    // VLW-Datei hat zusaetzlich die Buchstaben der Wochentage (stripVlwPath(size, true)).

    // Weekday written out (language of the web interface): off by default, own size and position. The VLW
    // file additionally holds the letters of the weekdays (stripVlwPath(size, true)).

    extern bool stripShowWeekday;
    extern uint8_t stripWeekdayScale;
    extern uint8_t stripVlwWeekdaySize;
    extern int16_t stripWeekdayX;
    extern int16_t stripWeekdayY;
    extern int16_t stripAutoWeekdayY;
    extern uint8_t* stripVlwWeekday;
    extern int8_t stripWeekdayLine; // quer: Zeile des Wochentags (stripContent()), -1 = keine
                                  // landscape: line of the weekday (stripContent()), -1 = none

    // Uhrzeit, Datum und Wochentag direkt auf dem Zifferblatt (Displays ohne Streifen): infoOverlayRevision zaehlt
    // jede Aenderung der Einstellungen, infoOverlayAppliedKey ist der Stand, der im Zifferblatt-Cache steckt.

    // Time, date and weekday directly on the clock face (displays without a strip): infoOverlayRevision counts
    // every change of the settings, infoOverlayAppliedKey is the state contained in the clock face cache.

    extern uint32_t infoOverlayRevision;
    extern uint32_t infoOverlayAppliedKey;
    extern uint32_t infoOverlayRevApplied;

    // Rechtecke (x0, y0, x1, y1 im Zifferblatt, x1 < x0 = leer): Text im Cache, eben gezeichneter Text, und die
    // Flaeche, die der naechste Frame neu aufbauen muss (alter plus neuer Text).

    // Rectangles (x0, y0, x1, y1 in the clock face, x1 < x0 = empty): text in the cache, text just drawn, and the
    // area the next frame has to rebuild (old plus new text).

    extern int16_t infoOverlayBox[4];
    extern int16_t infoOverlayBoxNew[4];
    extern int16_t infoOverlayPending[4];
    extern int16_t stripPreviewBox[4]; // Grenzen des Textes der letzten Vorschau / bounds of the text of the last preview

    // Wird hochgezaehlt, sobald sich Zifferblatt, Zeigersatz oder Zeigerbreiten
    // aendern - macht jedes Zwischenbild ungueltig, ohne dass jede einzelne
    // Aenderungsstelle das Zwischenbild selbst kennen muss.

    // Incremented whenever the clock face, hand set or hand widths change -
    // invalidates every composite image without each individual change site
    // having to know about the composite itself.

    extern uint32_t clockAssetGeneration;

    // Cache: clockFaceBuffer bereits mit currentBrightness vorberechnet (siehe
    // loadClockFace()) - vermeidet die teure Pixel-Helligkeitsanpassung bei
    // jedem Tick, obwohl sich die Helligkeit dazwischen fast nie aendert.

    // Cache: clockFaceBuffer is pre-adjusted for currentBrightness (see
    // loadClockFace()) - avoids the expensive per-pixel brightness adjustment
    // on every tick, even though brightness rarely changes in between.

    extern uint16_t* clockFaceBrightBuffer;

    // Je Display true, solange noch nicht passender Inhalt (Uhr/Meldung) darauf steht - ein "n.a."-Display
    // wird dann einmalig schwarz geloescht. Startwert true, da der Inhalt nach tft.init() undefiniert ist.

    // Per display true while content (clock/message) that no longer fits is still shown - an "n.a." display
    // is then cleared to black once. Initially true, since the content is undefined after tft.init().

    extern bool displayNeedsBlank[2];

    // Display 2 (am CS2-Pin) wird ueber seine Rotation "n.a." abgeschaltet, kein eigener Schalter. Danach:
    // Helligkeit / Fotowiderstand (ADC).

    // Display 2 (on the CS2 pin) is switched off via its rotation "n.a.", no separate toggle. Below:
    // brightness / photoresistor (ADC).

    extern bool adcInverted; // Standardmäßig nicht invertiert
                              // Not inverted by default

    extern bool useAdc;
    extern bool photoresistorFound;

    extern uint8_t currentBrightness;
    extern uint8_t brightnessTestValue; // Wert des Helligkeitstests / value of the brightness test
    extern unsigned long brightnessTestUntil; // millis() am Ende des Tests, 0 = kein Test / millis() at the end of the test, 0 = no test
    extern int16_t brightnessOverride; // /api/setbrightness: fester Wert 0-255 bis zum Neustart oder "auto", -1 = normale Regelung / fixed value 0-255 until restart or "auto", -1 = normal control
    extern uint8_t lastAppliedBrightness; // gehoert zum Zifferblatt-Cache und wird von loadClockFace() gepflegt
                                         // belongs to the clock face cache and is maintained by loadClockFace()

    // Eigener Vergleichswert fuer die Zeiger-Sprites (siehe updateBrightness()) -
    // darf nicht lastAppliedBrightness teilen, sonst wuerde loadClockFace()
    // die Neueinfaerbung nie ausloesen.

    // Own comparison value for the hand sprites (see updateBrightness()) -
    // must not share lastAppliedBrightness, or loadClockFace() would never
    // trigger the re-tinting.

    extern uint8_t lastHandBrightness;
    extern uint8_t targetBrightness;
    extern int lowThreshold;
    extern int highThreshold;
    extern uint8_t minBrightness; //
    extern uint8_t maxBrightness; // Obergrenze
                                  // Upper limit

    // Wird wahr, sobald das Zifferblatt zum ersten Mal gezeichnet wird. Davor (Start, Meldungen) gilt mit
    // Beleuchtungsregelung maxBrightness, danach minBrightness bis die Uhrzeit bekannt ist.

    // Becomes true as soon as the clock face is drawn for the first time. Before (start, messages) maxBrightness
    // applies with a backlight control, afterwards minBrightness until the time is known.

    extern bool startupFaceDrawn;

    // Zeitabhängige Helligkeit
    // Time-dependent brightness

    extern uint8_t brightStartHour; // inkl. (z.B. 8)
                                       // inclusive (e.g. 8)
    extern uint8_t brightEndHour; // exkl. (z.B. 20)
                                       // exclusive (e.g. 20)

    extern float gammaBrightness; // Gamma-Korrektur für Helligkeit (nur mit Backlight wirksam)
                                   // Gamma correction for brightness (only effective with a backlight)

    // Helligkeit per PWM auf TFT_Backlight (Pin 3) statt dunklerer Pixel - Einstellung PK_USE_BACKLIGHT. Ab
    // Werk beim GC9D01 an, beim GC9A01 aus (BACKLIGHT_DEFAULT). Umschalten: setBacklightMode().

    // Brightness via PWM on TFT_Backlight (pin 3) instead of darker pixels - setting PK_USE_BACKLIGHT.
    // Factory: on for GC9D01, off for GC9A01 (BACKLIGHT_DEFAULT). Switch via setBacklightMode().

    extern bool useBacklight; // wird in setup() aus den Preferences geladen
                               // loaded from preferences in setup()
    extern bool backlightAttached; // PWM-Kanal an TFT_Backlight angehaengt (ledcAttach())
                                    // PWM channel attached to TFT_Backlight (ledcAttach())

#define ADC_SMOOTHING 20
    extern int adcHistory[ADC_SMOOTHING];
    extern int adcIndex;
    extern int currentAdcAvg; // global definieren
                            // define globally
    extern int currentLightPercent; // global speichern für Anzeige
                                  // store globally for display

    // Mindestabstand zwischen zwei ADC-Abtastungen (siehe updateBrightness()) -
    // ohne das wuerde das ADC_SMOOTHING-Fenster bei schnellen loop()-Durchlaeufen
    // nur wenige Millisekunden abdecken statt eine Sekunde.

    // Minimum spacing between two ADC samples (see updateBrightness()) -
    // without it, the ADC_SMOOTHING window would span only a few
    // milliseconds on fast loop() iterations instead of one second.

#define ADC_SAMPLE_INTERVAL_MS 50
    extern unsigned long lastAdcSampleMillis;

    // So lange muss eine Schwellwert-Ueberschreitung anhalten, bevor targetBrightness springt - filtert kurze
    // Einbrueche (z.B. WLAN-Sendeburst), die das Zifferblatt sonst fuer ein Bild umfaerben wuerden.

    // How long a threshold crossing must persist before targetBrightness jumps - filters brief dips (e.g. a
    // WiFi TX burst) that would otherwise re-tint the clock face for a frame.

#define BRIGHTNESS_DEBOUNCE_MS 1500
    extern int pendingBrightnessState; // -1 = Kandidat fuer minBrightness, 1 = fuer maxBrightness, 0 = im Hysterese-Band
                                    // -1 = candidate for minBrightness, 1 = for maxBrightness, 0 = inside the hysteresis band
    extern unsigned long brightnessThresholdSinceMillis;


    // Presets
    // Presets

#define MAX_PRESETS 50
    struct Preset {
        String name;
        String url;
    };
    extern Preset presets[MAX_PRESETS];

    // Datei-Upload / Wartung
    // File upload / maintenance

    extern File uploadFile;
    extern String uploadFilePath;
    extern bool uploadSuccess;


    extern int lastResetWeek;
    extern int currentWeek;

    // Uebersetzungen fuer verschiedene Sprachen
    // Translations for various languages

#include "translation.h"
