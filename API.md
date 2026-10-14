# HTTP-Schnittstellen der Uhr / HTTP interfaces of the clock

*English version below.*

Alle Adressen gelten für `http://<IP-Adresse der Uhr>/…`. Es gibt keine Anmeldung. Adressen, die Zugangsdaten, Dateien oder den Neustart betreffen, antworten nur im **privaten Netz** (192.168.x.x, 10.x.x.x, 172.16–31.x.x) und sonst mit `403` oder einer leeren Antwort. Die Liste ist eine Auswahl der Adressen, die sich von außen sinnvoll nutzen lassen; alle Routen stehen in `webserver_routes.h` (`webserver.on(...)`), die Firmware-Routen in `ota_update.h`.

Wahrheitswerte sind `1` oder `true` (Groß-/Kleinschreibung egal), alles andere gilt als aus.

## Zeit und Status

| Adresse | Methode | Parameter | Antwort |
|---|---|---|---|
| `/api/currentTime` | GET | – | JSON: `hour`, `minute`, `second`, `ms`, `rocrail`, `divider`, `frozen`, `previewSig` (Kennwert aus Zifferblatt und Zeigern) |
| `/api/topbarStatus` | GET | – | JSON: Zeitquelle, RTC, DCF77, Rocrail, Lichtwert, Datum, Firmware-Version. Außerhalb des privaten Netzes nur `"na"`. |
| `/api/dcf77status` | GET | – | Status und Bits des DCF77-Empfängers |
| `/api/rocrailStatus` | GET | – | Status der Rocrail-Verbindung und Modellzeit |
| `/api/setTime` | POST | `t` = Unix-Sekunden UTC, auch mit Nachkommastellen (2024–2099) | `{"ok":true,"time":"JJJJ-MM-TT hh:mm:ss"}`; setzt Systemzeit und RTC; privates Netz |
| `/api/testNtp` | GET | `server` = Hostname | `OK\|<Zeit>` oder `FAILED` |
| `/api/currentLog` | GET | – | Aktuelles Log als Text (privates Netz, nur wenn die Protokollierung an ist) |
| `/api/logFileList` | GET | – | JSON-Liste der Logdateien (privates Netz) |

## Steuerung

| Adresse | Methode | Parameter | Wirkung |
|---|---|---|---|
| `/api/setMode` | GET | siehe unten | Stellt Zifferblatt, Zeigersatz und weitere Einstellungen um; nicht genannte Werte bleiben |
| `/api/setPreset` | GET | `name` | Wendet ein gespeichertes Preset an (Leerzeichen werden zu `_`); `404`, wenn es das Preset nicht gibt |
| `/api/createPreset` | POST | `name` (optional) | Legt aus den aktuellen Einstellungen ein Preset an, solange ein Platz frei ist |
| `/api/partialUpdate` | GET | `enabled=0\|1` (optional) | Teilbilder ein oder aus; ohne Parameter nur Abfrage |
| `/api/testBrightness` | POST | `value=0..255` | Zeigt den Helligkeitswert eine Weile auf der Uhr |
| `/api/setbrightness` | GET, POST | `value=0..255` oder `value=auto` | Setzt die Helligkeit fest (bis zum Neustart); die PWM-Beleuchtung gleitet langsam hin (etwa 10 ms je Stufe), ohne PWM gilt der Wert sofort; `auto` (oder `-1`) kehrt langsam zur normalen Regelung zurück. Antwort `{"ok":true,"brightness":N}`; bei Einrichtung (Access Point, WPS) gilt deren Helligkeit weiter |
| `/api/setPreviewSize` | GET | `size` | Größe der Vorschau auf der Hauptseite |
| `/api/reboot` | GET | – | Startet die Uhr neu (privates Netz) |
| `/setbackground` | GET | `file`, `ajax=1` (Antwort `ok` statt Weiterleitung) | Zifferblatt wählen, z. B. `file=face_db.bmp` |
| `/sethandset` | GET | `set`, `ajax=1` | Zeigersatz wählen |

**Parameter von `/api/setMode`:**

| Parameter | Werte |
|---|---|
| `face` | Dateiname des Zifferblatts, z. B. `face_sbb.bmp` (muss auf der Uhr liegen) |
| `handSet` | Kennung des Zeigersatzes |
| `timeZone` | Zeitzonenangabe (löst eine NTP-Abfrage aus) |
| `hubSize`, `hubColor` | Nabe: Größe in Pixel, Farbe als `RRGGBB` (hex) |
| `stationMode` | Sekundenzeiger wartet auf der 12 (Bahnhofsuhr) |
| `showSecondHand`, `smoothSecond`, `smoothMinute` | Sekundenzeiger zeigen; Sekunde und Minute gleitend statt springend |
| `rotation` | `0`, `90`, `180`, `270` (Grad), Index `0–3` oder `na` (Display nicht angeschlossen) |
| `rocrail`, `wifiReconnect` | Rocrail-Zeit an/aus, WLAN-Wiederverbindung an/aus |
| `autoBrightness` | Automatische Helligkeit; die Werte für Minimum, Maximum, Schwellen, Gamma und Zeiten werden **nicht** übernommen |
| `source=preset`, `ajax=1` | Nur intern für Presets; `ajax=1` antwortet mit `ok` |

## Firmware

| Adresse | Methode | Parameter | Wirkung |
|---|---|---|---|
| `/firmware/check` | GET | `force=1` (prüft sofort neu) | JSON: `state` (`older`, `same`, `newer`, `unknown`), `remote`, `installed`, `error`; privates Netz |
| `/firmware/github` | POST | `force=1` (auch gleiche oder ältere Version) | Lädt die Firmware aus dem GitHub-Release, prüft SHA-256 und startet neu |
| `/firmware/update` | POST | Formularfeld `firmware` = `.bin` | OTA-Upload; Beispiel: `curl -F "firmware=@uhr4.ino.bin" http://<IP>/firmware/update` |

## Dateien

| Adresse | Methode | Parameter | Wirkung |
|---|---|---|---|
| `/file` | GET | `name` | Liefert eine Datei der Uhr (BMP als normales BMP, Texte als Text), z. B. `name=/stripcfg_sbb.txt` |
| `/upload` | POST | Formular mit Datei | Lädt ein Zifferblatt (`face_*.bmp`) hoch |
| `/delete` | GET | `file` | Löscht eine Datei |

## Zifferblatt-Designer

| Adresse | Methode | Parameter | Wirkung |
|---|---|---|---|
| `/save_strip` | POST | `st`, `sd`, `sw` (Uhrzeit, Datum, Wochentag an), `fg` (`#rrggbb`), `font`, `tfmt`, `dfmt`, `wfmt`, Größen, Positionen `tx ty dx dy wx wy` (`-1` = automatisch), `save=0\|1`, `face` | Stellt Uhrzeit/Datum/Wochentag ein; `save=1` speichert zum Zifferblatt (`stripcfg_*.txt`) |
| `/api/stripimg` | GET | `text=3` (nur Zifferblatt-Anzeige), `h`, `m`, `s` (feste Uhrzeit), `tx ty dx dy wx wy` (nur für diese Vorschau) | Vorschaubild für den Designer: 4 Byte Kopf (Anfangszeile, Zeilenzahl), danach RGB565 big-endian, weiße Schrift auf Schwarz |

## Waveshare ESP32-S3-Touch-LCD-4 (480 × 480), zur Fehlersuche

| Adresse | Methode | Parameter | Antwort |
|---|---|---|---|
| `/api/touchlog` | GET | – | JSON: die letzten 8 Berührungen als `[x, y, sx, sy]` (Touch-Chip, Zifferblatt), `rotation`, `max` = Auflösung des Touch-Chips (nur Waveshare ESP32-S3-Touch-LCD-4) |
| `/api/lcdstats` | GET | – | JSON: Panel-Neustarts, Bilder je Fenster (`partial`, `full`) mit Zeit je Bild in µs (`prep`, `comp`, `hands`, `send`), `why` (Gründe für Vollbilder), `beep`. Das Lesen setzt die Zähler zurück. |
| `/api/frameimg` | GET | – | Zuletzt gezeichnetes Bild, 480 × 480, RGB565 big-endian (460 800 Byte). Das Senden hält die Uhr etwa 1 s an. |
| `/api/fbimg` | GET | – | Der vom Panel gerade angezeigte Bildspeicher (Doppelpuffer), 480 × 480, RGB565 big-endian; sollte bis auf den Sekundenzeiger gleich `/api/frameimg` sein |
| `/api/faceimg` | GET | – | Zifferblatt-Cache mit Uhrzeit und Datum, RGB565 |
| `/api/lcdresync` | GET | – | Erzwingt eine neue Panel-Synchronisation |
| `/api/beeptest` | GET | `pin=4..7`, `ms=50..1500`, `tone=0\|1` | Setzt einen Expander-Ausgang für `ms` Millisekunden auf high (`tone=1`: Rechtecksignal); der Beeper hängt an Pin 6; privates Netz |
| `/api/imu` | GET | – | Lagesensor: `x`, `y`, `z`, `quadrant`, `auto`, `rotation` (nur Boards mit IMU) |

## Absichtlich nicht aufgeführt

WLAN (`/api/scanwifi`, `/api/rescanwifi`, `/api/connectWifi`, `/api/startWPS`) und Sicherung (`/backup/download`, `/backup/restore`) enthalten oder verändern Zugangsdaten und gehören nicht in Skripte ohne Not.

## USB (serielle Schnittstelle)

Zeilen über die USB-Schnittstelle (die Baudrate ist ohne Bedeutung, der ESP32 nutzt sein eigenes USB): `UHR4 INFO` (Chip, Displaytyp, Version), `UHR4 DISPLAY <Name>`, `UHR4 WIFI <Name-Hex> <Passwort-Hex>`, `UHR4 TIME <Unix-Sekunden>`, `UHR4 RESTART`. `flashESP` und `setTime` benutzen sie; Einzelheiten in `build_uhr4/readme.txt` und in `handleSerialCommands()` in `display.h`.

## MQTT

Wenn MQTT eingeschaltet ist (Seite „MQTT“), meldet die Uhr einmal nach dem Start, um Mitternacht und bei neuer IP-Adresse ein JSON nach `<Basis-Topic>/<Hostname>/info` (Status, Firmware, Adresse).

---

# English

All addresses are `http://<IP address of the clock>/…`. There is no login. Addresses that concern credentials, files or the restart answer only in the **private network** (192.168.x.x, 10.x.x.x, 172.16–31.x.x) and otherwise with `403` or an empty answer. The list is a selection of the addresses that are useful from outside; all routes are in `webserver_routes.h` (`webserver.on(...)`), the firmware routes in `ota_update.h`.

Truth values are `1` or `true` (case does not matter), anything else counts as off.

## Time and status

| Address | Method | Parameters | Response |
|---|---|---|---|
| `/api/currentTime` | GET | – | JSON: `hour`, `minute`, `second`, `ms`, `rocrail`, `divider`, `frozen`, `previewSig` (check value of clock face and hands) |
| `/api/topbarStatus` | GET | – | JSON: time source, RTC, DCF77, Rocrail, light value, date, firmware version. Outside the private network only `"na"`. |
| `/api/dcf77status` | GET | – | Status and bits of the DCF77 receiver |
| `/api/rocrailStatus` | GET | – | Status of the Rocrail connection and model time |
| `/api/setTime` | POST | `t` = Unix seconds UTC, fractions allowed (2024–2099) | `{"ok":true,"time":"YYYY-MM-DD hh:mm:ss"}`; sets system time and RTC; private network |
| `/api/testNtp` | GET | `server` = host name | `OK\|<time>` or `FAILED` |
| `/api/currentLog` | GET | – | Current log as text (private network, only if logging is on) |
| `/api/logFileList` | GET | – | JSON list of the log files (private network) |

## Control

| Address | Method | Parameters | Effect |
|---|---|---|---|
| `/api/setMode` | GET | see below | Changes clock face, hand set and further settings; values not named stay |
| `/api/setPreset` | GET | `name` | Applies a stored preset (spaces become `_`); `404` if there is no such preset |
| `/api/createPreset` | POST | `name` (optional) | Creates a preset from the current settings while a slot is free |
| `/api/partialUpdate` | GET | `enabled=0\|1` (optional) | Partial frames on or off; without a parameter only a query |
| `/api/testBrightness` | POST | `value=0..255` | Shows the brightness value on the clock for a while |
| `/api/setbrightness` | GET, POST | `value=0..255` or `value=auto` | Sets the brightness fixed (until restart); the PWM backlight glides slowly (about 10 ms per step), without PWM the value applies at once; `auto` (or `-1`) returns slowly to the normal control. Answer `{"ok":true,"brightness":N}`; during setup (access point, WPS) its brightness stays |
| `/api/setPreviewSize` | GET | `size` | Size of the preview on the main page |
| `/api/reboot` | GET | – | Restarts the clock (private network) |
| `/setbackground` | GET | `file`, `ajax=1` (answer `ok` instead of a redirect) | Choose the clock face, e.g. `file=face_db.bmp` |
| `/sethandset` | GET | `set`, `ajax=1` | Choose the hand set |

**Parameters of `/api/setMode`:**

| Parameter | Values |
|---|---|
| `face` | File name of the clock face, e.g. `face_sbb.bmp` (must be on the clock) |
| `handSet` | Identifier of the hand set |
| `timeZone` | Time zone setting (triggers an NTP request) |
| `hubSize`, `hubColor` | Hub: size in pixels, colour as `RRGGBB` (hex) |
| `stationMode` | Second hand waits at 12 (station clock) |
| `showSecondHand`, `smoothSecond`, `smoothMinute` | Show the second hand; second and minute gliding instead of jumping |
| `rotation` | `0`, `90`, `180`, `270` (degrees), index `0–3` or `na` (display not connected) |
| `rocrail`, `wifiReconnect` | Rocrail time on/off, WiFi reconnect on/off |
| `autoBrightness` | Automatic brightness; the values for minimum, maximum, thresholds, gamma and times are **not** taken over |
| `source=preset`, `ajax=1` | Internal use for presets only; `ajax=1` answers `ok` |

## Firmware

| Address | Method | Parameters | Effect |
|---|---|---|---|
| `/firmware/check` | GET | `force=1` (checks again right away) | JSON: `state` (`older`, `same`, `newer`, `unknown`), `remote`, `installed`, `error`; private network |
| `/firmware/github` | POST | `force=1` (also the same or an older version) | Downloads the firmware from the GitHub release, checks SHA-256 and restarts |
| `/firmware/update` | POST | form field `firmware` = `.bin` | OTA upload; example: `curl -F "firmware=@uhr4.ino.bin" http://<IP>/firmware/update` |

## Files

| Address | Method | Parameters | Effect |
|---|---|---|---|
| `/file` | GET | `name` | Delivers a file of the clock (BMP as a normal BMP, texts as text), e.g. `name=/stripcfg_sbb.txt` |
| `/upload` | POST | form with a file | Uploads a clock face (`face_*.bmp`) |
| `/delete` | GET | `file` | Deletes a file |

## Clock face designer

| Address | Method | Parameters | Effect |
|---|---|---|---|
| `/save_strip` | POST | `st`, `sd`, `sw` (time, date, weekday on), `fg` (`#rrggbb`), `font`, `tfmt`, `dfmt`, `wfmt`, sizes, positions `tx ty dx dy wx wy` (`-1` = automatic), `save=0\|1`, `face` | Sets time/date/weekday; `save=1` stores it with the clock face (`stripcfg_*.txt`) |
| `/api/stripimg` | GET | `text=3` (clock face display only), `h`, `m`, `s` (fixed time), `tx ty dx dy wx wy` (for this preview only) | Preview image for the designer: 4-byte header (first row, row count), then RGB565 big-endian, white text on black |

## Waveshare ESP32-S3-Touch-LCD-4 (480 × 480), for troubleshooting

| Address | Method | Parameters | Response |
|---|---|---|---|
| `/api/touchlog` | GET | – | JSON: the last 8 touches as `[x, y, sx, sy]` (touch chip, clock face), `rotation`, `max` = resolution of the touch chip (Waveshare ESP32-S3-Touch-LCD-4 only) |
| `/api/lcdstats` | GET | – | JSON: panel restarts, frames per window (`partial`, `full`) with time per frame in µs (`prep`, `comp`, `hands`, `send`), `why` (reasons for full frames), `beep`. Reading resets the counters. |
| `/api/frameimg` | GET | – | Last drawn frame, 480 × 480, RGB565 big-endian (460,800 bytes). Sending stalls the clock for about 1 s. |
| `/api/fbimg` | GET | – | The frame buffer the panel currently displays (double buffer), 480 × 480, RGB565 big-endian; should equal `/api/frameimg` except for the second hand |
| `/api/faceimg` | GET | – | Clock face cache with time and date, RGB565 |
| `/api/lcdresync` | GET | – | Forces a new panel synchronisation |
| `/api/beeptest` | GET | `pin=4..7`, `ms=50..1500`, `tone=0\|1` | Sets an expander output high for `ms` milliseconds (`tone=1`: square wave); the buzzer is on pin 6; private network |
| `/api/imu` | GET | – | Motion sensor: `x`, `y`, `z`, `quadrant`, `auto`, `rotation` (only boards with an IMU) |

## Deliberately not listed

WiFi (`/api/scanwifi`, `/api/rescanwifi`, `/api/connectWifi`, `/api/startWPS`) and backup (`/backup/download`, `/backup/restore`) contain or change credentials and do not belong in scripts without need.

## USB (serial interface)

Lines over the USB interface (the baud rate does not matter, the ESP32 uses its own USB): `UHR4 INFO` (chip, display type, version), `UHR4 DISPLAY <name>`, `UHR4 WIFI <name hex> <password hex>`, `UHR4 TIME <Unix seconds>`, `UHR4 RESTART`. `flashESP` and `setTime` use them; details in `build_uhr4/readme.txt` and in `handleSerialCommands()` in `display.h`.

## MQTT

If MQTT is switched on (page "MQTT"), the clock reports a JSON once after the start, at midnight and on a new IP address to `<base topic>/<host name>/info` (status, firmware, address).
