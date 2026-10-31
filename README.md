# uhr4 (Version 4) – Uhr auf runden und eckigen Displays

*English version below.*

uhr4 zeigt die Uhrzeit als Zifferblatt mit Zeigern auf einem TFT-Display. Die Hardware ist ein ESP32-S2 (Lolin S2 Pico) mit rundem (GC9A01, GC9D01) oder rechteckigem Display (ILI9341), ein ESP32-C6 von Waveshare mit eingebautem ST7789 ein ESP32-S3 von Waveshare mit eingebautem rundem GC9A01 oder ein ESP32-S3-Touch-LCD-4 von Waveshare mit eingebautem quadratischem ST7701 (480 × 480, Touch) bzw. ein Adafruit Qualia ESP32-S3 mit einem runden 2,1"-Display (480 × 480, Touch, ungetestet). Auf den rechteckigen Displays (ILI9341, ST7789 172 × 320) zeigt ein Streifen neben der Uhr Uhrzeit und Datum, wahlweise über oder unter der Uhr (quer: links oder rechts). Die Zeit kommt per NTP (WLAN), DCF77 oder RTC. Zifferblätter, Zeiger und alle Einstellungen werden über die Weboberfläche angepasst.

**Download:** Firmware mit Flash-Tool unter [Releases](https://github.com/holgiw/ESP32-Station-Clock/releases/latest) unter *Assets* (Anleitung: Abschnitt „Fertige Firmware flashen“ und `build_uhr4/readme.txt`). Auch das ZIP von „Download ZIP“ ist flashbar: Die Firmware-Dateien in `build_uhr4/esp32*` (je Chip vier `.bin`) entstehen mit `buildAll.bat` und liegen im Repository. Beim Update von uhr3 (Version 3) übernimmt uhr4 den Displaytyp.

---

## 1. Unterstützung mehrerer TFT-Displays

Eine Firmware je ESP. Den Displaytyp wählt man im Tab „Uhr Einstellungen“ oder beim Flashen mit `flashESP`; beim Update von uhr3 wird er übernommen. Grafikbibliothek: LovyanGFX.

- **ESP32-S2** (Lolin S2 Pico, Build `build_uhr4/esp32s2`):
  - GC9A01 – rund, 240 × 240; ohne oder mit geregelter Hintergrundbeleuchtung (BL)
  - GC9D01 – rund, 160 × 160; BL geregelt
  - ILI9341 – rechteckig, 240 × 320; Uhr 240 × 240 (Zifferblätter und Zeiger wie beim GC9A01) und Streifen mit Uhrzeit und Datum; feste Beleuchtung
- **ESP32-C6** (Waveshare, Display fest verbaut, Build `build_uhr4/esp32c6`, ohne PSRAM; BL geregelt; eigene Partitionstabelle mit 2,25 MB Programm und 1,6 MB Dateisystem – beim Selbstbauen `boards.local.txt` und `uhr4_c6.csv` in den Core-Ordner kopieren, siehe `boards.local.txt`; beim Wechsel von einer älteren Firmware vorher eine Sicherung ziehen, das Dateisystem wird neu angelegt):
  - ESP32-C6-LCD-1.47: ST7789 – rechteckig, 172 × 320; Uhr 172 × 172 und Streifen mit Uhrzeit und Datum
  - ESP32-C6-LCD-1.3: ST7789_240 – quadratisch, 240 × 240
- **ESP32-S3** (Waveshare ESP32-S3-LCD-1.28, Display fest verbaut, Build `build_uhr4/esp32s3`, 16 MB Flash, 2 MB PSRAM; BL geregelt; noch nicht am Gerät erprobt):
  - GC9A01 – rund, 240 × 240
  - Flash mit zwei App-Partitionen und 9,9 MB für Zifferblätter, Zeiger und Schriften
  - Firmware-Update über WLAN (siehe Abschnitt 10)
  - Rotation von Hand oder „automatisch“ über den eingebauten Lagesensor (QMI8658): Das Zifferblatt steht aufrecht, egal wie die Uhr hängt; liegt sie flach, bleibt die Lage. Vorher die passende Rotation von Hand wählen und speichern, dann auf „automatisch“ stellen – die Uhr nimmt diese Lage als Bezug.
- **ESP32-S3-Touch-LCD-4** (Waveshare, Display fest verbaut, Build `build_uhr4/esp32s3lcd4`, 16 MB Flash, 8 MB PSRAM):
  - ST7701_480 – quadratisch, 480 × 480, am RGB-Bus; Touch GT911, Port-Expander CH32V003 (Beleuchtung per PWM, Beeper)
  - Das Bild liegt in einem eigenen Puffer im PSRAM; nach Flash-Schreibzugriffen (Firmware-Update, Sicherung einspielen) synchronisiert die Uhr das Panel neu. Während Update und Sicherung zeigt das Display kurz den Text und danach eine blaue Fläche.
  - Touch-Zonen: Die Ecke rechts oben zeigt 5 Sekunden lang WLAN und IP-Adresse (Status), die Ecke links unten lädt das vorherige, die Ecke rechts unten das nächste Preset (Reihenfolge wie auf der Seite „Presets“); jede Zone ist 160 × 160 Pixel groß. Helligkeit über die PWM-Beleuchtung („Hintergrundbeleuchtung PWM“).
  - Bekannte Grenzen: etwa 22 Bilder pro Sekunde beim Sekundenzeiger, selten kurzes Flimmern oder ein Neustart des Panels (Zähler `/api/lcdstats`). Eigene Firmware: Das GitHub-Update lädt `uhr4-s3lcd4.bin`.
- **Adafruit Qualia ESP32-S3** mit dem 2,1" runden Display TL021WVC02 (Build `build_uhr4/esp32s3qualia`, 16 MB Flash, 8 MB PSRAM; **EXPERIMENTELL und NICHT AM GERÄT GETESTET** – nach den Unterlagen von Adafruit geschrieben):
  - TL021_480 – rund, 480 × 480, am RGB-Bus (Controller der ST7701-Familie, Startfolge über den Port-Expander PCA9554A); Touch CST826 (I2C 0x15)
  - Die Beleuchtung lässt sich nur ein- und ausschalten (PWM nur mit Lötjumper am Board): die Helligkeit regelt das Abdunkeln der Pixel. Die Tasten UP und DOWN am Expander zählen wie der Taster.
  - Berührung zeigt wie beim Waveshare-Board 5 Sekunden lang WLAN und IP-Adresse; Firmware-Update und Sicherung zeigen die ruhige blaue Fläche. Das GitHub-Update lädt `uhr4-s3qualia.bin`.
- Zweites, baugleiches Display (nur ESP32-S2) mit eigener Rotation (0°, 90°, 180°, 270°). Steht ein Display auf „nicht angeschlossen (n.a.)“, bleibt es schwarz, und die Uhr zeichnet und berechnet nichts dafür. Standard: Display 1 mit 0°, Display 2 n.a. Status- und Startmeldungen (Start, Access-Point-Modus) erscheinen bis zum Uhrstart trotzdem auf beiden Displays; die Einstellungsseite weist darauf hin.

---

## 2. Anpassbare Zeiger und Zifferblätter

- Stunden-, Minuten- und Sekundenzeiger und Zifferblätter lassen sich als BMP hochladen, z. B. im Layout der Deutschen Bahn oder anderer Gesellschaften, oder im Designer erstellen.
- Startpaket: Was davon fehlt, zeichnet die Uhr bei jedem Start selbst neu (nach dem ersten Flashen, einem Werksreset oder wenn es gelöscht wurde); Vorhandenes bleibt
  - vier Zifferblätter, weiß mit Rand, Stunden- und Minutenstrichen: `face_default.bmp` (12, 3, 6, 9), `face_numbers.bmp` (1–12), `face_roman.bmp` (I–XII), `face_subdial.bmp` (12, 3, 9 und ein Sekundenfeld im unteren Drittel),
  - vier Zeigersätze: Satz 0 mit schwarzen Balken und rotem Sekundenzeiger, Satz 1 ebenso mit schwarzem Sekundenzeiger, Satz 2 geschwungen, ein Satz wie Satz 0 mit kurzem rotem Sekundenzeiger samt Scheibe fürs Sekundenfeld,
  - die Uhren Sets „Standard“, „1-12“, „I-XII“ und „Sekundenfeld“, sofern es keins mit diesem Namen gibt (I-XII ohne Sekundenzeiger; auf Displays mit Streifen – ILI9341, ST7789 172 × 320 – mit weißem Streifen und schwarzer Schrift).

  Die Werksreset-Aktionen „Zifferblätter löschen“, „Zeigersätze löschen“ und „Uhren Sets löschen“ stellen diesen Zustand wieder her. Den Sekundenfeld-Satz legt die Uhr unter der ersten freien Nummer ab 3 an – auf älteren Uhren kann 3 ein eigener Satz sein. Beim Start nummeriert die Uhr die Zeigersätze fortlaufend neu (0, 1, 2, dann der Sekundenfeld-Satz als 3, die übrigen ab 4 in ihrer Reihenfolge); Lücken schließen sich, Uhren Sets und der aktive Satz ziehen mit. Zeiger im alten Format speichert sie dabei im neuen Format des Displays (transparent aufgefüllt, Anzeige unverändert). Ein vorhandener Zeigersatz mit der Nummer eines Start-Satzes wird nicht ergänzt. Jeder Zeigersatz hat alle drei Zeiger: Fehlt einem einer (z. B. nach dem Hochladen nur einer `1_second.bmp`), kopiert die Uhr ihn aus Satz 0 – beim Start, beim Aufrufen der Zeigersatz-Übersicht, beim Aktivieren des Satzes und nach dem Wiederherstellen. Danach ist er ein eigener Zeiger des Satzes.
  Die erzeugten Zifferblätter und Zeigersätze lassen sich weder löschen noch umbenennen noch überschreiben (Weboberfläche, Upload, Designer, Wiederherstellen); sie stehen in den Übersichten zuerst und sind nicht in der Sicherung. Im Designer geht für sie nur „Als neues … speichern“ unter einem anderen Namen. Die Werksreset-Aktionen „Zifferblätter löschen“ und „Zeigersätze löschen“ lassen sie stehen.
- Uhrzeit, Datum und Wochentag lassen sich im Zifferblatt-Designer einblenden: bei ILI9341 und ST7789 172 × 320 im Streifen, bei allen anderen Displays (rund, 240 × 240, 480 × 480) direkt auf dem Zifferblatt hinter den Zeigern – ab Werk aus, je Zifferblatt gespeichert, ohne Sekunden und ohne PSRAM (ESP32-C6) nicht verfügbar.
- Alle Zeiger sind kantengeglättet (LovyanGFX). Stunden- und Minutenzeiger liegen in einem zwischengespeicherten Bild, zum Display geht nur der Bereich um den Sekundenzeiger. Deshalb läuft der schwingende Sekundenzeiger flüssig.
- Zeigerformat 25 × 166 px mit Drehpunkt 12 / 120 (160er-Display: 15 × 115, Drehpunkt 7 / 80): Zeiger reichen vom Drehpunkt bis zum Displayrand, unter dem Drehpunkt sind 15 Zeilen mehr Platz als früher. Zeigersätze im bisherigen Format 25 × 151 (15 × 100) und im älteren Format 21 × 131 (13 × 86) funktionieren weiter; die Uhr füllt sie beim Laden transparent auf, der Drehpunkt bleibt an derselben Stelle. Gültig sind alle Kombinationen aus Breite 21 oder 25 und Höhe 131, 151 oder 166. Beim 480×480 ist die Zeigerbreite 51 (ungerade, die Symmetrieachse liegt auf der mittleren Pixelspalte); Zeiger der Breite 50 und 42 bleiben gültig. Diese Größen bleiben beim Hochladen unverändert, alle anderen skaliert die Uhr auf 21 × 131 (13 × 86). Der Zeiger-Designer speichert im alten Maß, solange kein Zeiger darüber hinausragt – solche Sätze laufen auch auf älterer Firmware.
- Zeiger-Designer im Browser (Seite „Zeiger“ → Link „Designer“ unter dem Zeigersatz; der Satz wird dabei aktiviert): neue Zeiger auf Basis des aktiven Satzes gestalten. Speichern als neuer Zeigersatz (wahlweise gleich aktiviert) oder über den aktiven Satz, der dann sofort gilt. Werkzeuge: Stift, Linie, Rechteck, Ellipse, Polygon, Füllen, Pipette, Farbwahl (Palette, Farbwähler, Hex-Eingabe), Spiegeln an der Mittelachse, Rückgängig, Formgenerator (Länge, Breiten, Gegengewicht, Scheibe) und Live-Vorschau auf dem aktuellen Zifferblatt. Zeigergröße und Drehpunkt passt die Uhr an ihr Display an.
- Zifferblatt-Designer im Browser (Seite „Zifferblatt“ → Link „Designer“ unter dem Zifferblatt; es wird dabei aktiviert): ein neues Zifferblatt auf Basis des aktiven gestalten oder das aktive überschreiben und sofort anwenden. Werkzeuge: Generator (Hintergrund, Rand, Stunden- und Minutenstriche, Ziffern 1–12, 12/3/6/9 oder römisch), Pixel-Werkzeuge mit Stiftbreite, Text (gerade, gedreht oder im Bogen, mit installierten oder geladenen Schriften), Logo-Stempel, Spiegel- und Dreh-Symmetrie (4-, 12- oder 60-fach), Farbwahl, Bild laden (PNG, JPG, BMP …), Zoom und Live-Vorschau mit den aktiven Zeigern. Auf runden Displays ist der unsichtbare Bereich markiert.
- Die Nabe (Mittelpunkt über den Zeigern) wird ebenfalls im Zifferblatt-Designer eingestellt: Radius und Farbe; die Farbe kommt aus dem Werkzeugkasten (Pipette, Palette, Farbfeld) und wird mit „Aktuelle Farbe“ übernommen. Die Uhr zeigt Änderungen sofort.
- Sekundenfeld: Im Zifferblatt-Designer lässt sich der Drehpunkt des Sekundenzeigers aus der Mitte verschieben, z. B. ins untere Drittel. Der Sekundenzeiger liegt dann unter Stunden- und Minutenzeiger; einen passend kurzen Sekundenzeiger mit eigener Scheibe zeichnet man im Zeiger-Designer. Der Drehpunkt gehört zum Zifferblatt.

---

## 3. Sanfter Minutenzeiger und Bahnhofsuhr-Modus

- Sanfter Minutenzeiger: läuft gleichmäßig, statt jede Minute zu springen.
- Bahnhofsuhr-Modus: Der Sekundenzeiger umrundet das Zifferblatt in 58,5 Sekunden und hält kurz auf der 12.
- Zeitsprünge (Start, erste gültige Uhrzeit, Zeitumstellung, Rocrail): Die Zeiger laufen in 3 Sekunden auf dem kürzesten Weg zur neuen Zeit, auch rückwärts.

---

## 4. Helligkeitssteuerung

- Automatische Helligkeit über einen Fotowiderstand mit einstellbaren Schwellwerten, oder manuell.
- Hintergrundbeleuchtung per PWM: Die Uhr dimmt dann die Beleuchtung statt die Pixel. Der Helligkeits-Tab zeigt dafür den Haken „Hintergrundbeleuchtung PWM“ mit der Pinnummer.
  - ESP32-S2: Pin 3, bei den Displaytypen „GC9A01 mit BL“ und GC9D01 ab Werk an.
  - ESP32-C6: Pin 22, auf dem Waveshare-Board fest verdrahtet, ab Werk an.
  - ESP32-S3: Pin 40, auf dem Waveshare-Board fest verdrahtet, ab Werk an.
  - ESP32-S3-Touch-LCD-4: kein Pin, die PWM sitzt im Port-Expander CH32V003 des Boards; ab Werk an.
- Fotowiderstand als Spannungsteiler mit 10 kΩ: ESP32-S2 an GPIO 1 (3 V), 2 (Messung), 4 (GND); ESP32-C6 an GPIO 1 (3 V), 2 (Messung), 3 (GND) der Stiftleiste; ESP32-S3 an GPIO 2 (3 V), 4 (Messung), 5 (GND) der Stiftleiste; ESP32-S3-Touch-LCD-4: nicht möglich (außer Display und I2C sind keine GPIOs frei – auch kein DCF77-Empfänger und kein externer Taster; der BOOT-Taster bleibt). Ohne Fotowiderstand gilt die manuelle Helligkeit.
- Während der Einrichtung (Access Point oder WPS aktiv) leuchtet das Display mit der maximalen Helligkeit, mindestens mit 100 (von 255). Beim normalen Start leuchtet es mit PWM-Beleuchtung zuerst mit der maximalen Helligkeit (Startmeldungen), geht kurz vor dem ersten Zifferblatt auf die minimale Helligkeit, solange noch keine Uhrzeit da ist, und wird danach langsam (etwa 5–8 s) auf den Wert der Regelung (z. B. Zeitfenster) hochgedimmt.
- Die Gamma-Korrektur gibt es nur mit Lichtsensor und PWM-Beleuchtung. ESP32-C6: Über 128 warnt die Helligkeitsseite vor Überhitzung der Beleuchtung (Hinweis von Waveshare).
- Mit Rocrail kann der Server die Helligkeit vorgeben (Abschnitt 11).

---

## 5. WLAN- und NTP-Integration

- Bis zu 15 WLAN-Netze, Einrichtung auch per WPS, automatischer Reconnect, eigener Hostname.
- Einrichtung beim Flashen: `flashESP.bat`/`flashESP.sh` erkennen ESP32-S2, ESP32-C6, ESP32-S3, ESP32-S3-Touch-LCD-4 und Adafruit Qualia (experimentell, ungetestet) am USB-Port, fragen Displaytyp (nicht bei den beiden ESP32-S3) und WLAN ab (Auswahl aus allen sichtbaren 2,4-GHz-Netzen, Passwort verdeckt) und senden beides per USB an die Uhr, zuletzt die Uhrzeit des PCs. Den Build des erkannten Chips nehmen sie aus den Ordnern `build_uhr4/esp32*` neben dem Skript (im Repository, im Repository-ZIP und im Werkzeug-ZIP, ohne Internet); fehlen die Ordner, laden sie ihn vom GitHub-Release (`uhr4-esp32s2.zip`, `-esp32c6.zip`, `-esp32s3.zip`, `-esp32s3lcd4.zip`, `-esp32s3qualia.zip`, mit SHA-256-Prüfung); `clocksetup.ps1 -Download` erzwingt den Download. Nach dem Flashen bleibt die COM-Schnittstelle frei (serieller Monitor nur mit `clocksetup.ps1 -Flash -Monitor`). Für die Entwicklung flasht `clocksetup.ps1 -Local` bzw. `UHR4_LOCAL=1` die Ordner `build_uhr4/esp32*`; die Pakete erzeugt `build_uhr4/publish_builds.ps1`.
- Werkzeuge im Projektordner: `buildAll.bat` baut die vier Ziele (s3, s2, c6, s3lcd4), das Qualia (`s3qualia`) nur auf Zuruf mit arduino-cli und kopiert sie nach `build_uhr4` (ohne Benutzernamen im Binary, `-ffile-prefix-map`; `publish_builds.ps1` bricht sonst ab); `build_uhr4/cleanup_history.ps1` entfernt die Firmware nach einem neuen Build-Commit aus allen älteren Commits; `checkClocks.bat` listet alle uhr4 im Netz mit Chip, Display und Firmware-Stand; `flashAll.bat` spielt veraltete Uhren ein (ESP32-S3 und ESP32-S3-Touch-LCD-4 per WLAN, jede mit ihrem eigenen Build; ESP32-S2 und ESP32-C6 nur am USB). Für jede am USB geflashte Uhr fragt `flashAll.bat` danach wie `flashESP.bat` das WLAN ab (Enter = unverändert); mit `-Yes` entfällt die Frage.
- Nur die Uhrzeit setzen, z. B. bei einer Uhr ohne WLAN, DCF77 und RTC: `setTime.bat`/`setTime.sh` oder der Knopf „Uhrzeit übernehmen“ in der Weboberfläche (Zeit des Handys bzw. PCs, auch im Access-Point-Modus).
- Weitere Wege ins WLAN: WPS oder der Einrichtungs-Access-Point (SSID `clock123`, Passwort `clocksetup`). Den Access Point startet die Uhr nur beim Start, wenn kein WLAN gespeichert oder keins erreichbar ist (mit RTC oder mit Zeit aus einem offenen WLAN nur ohne gespeichertes WLAN). Bricht das WLAN im Betrieb weg, bleibt die Uhr im WLAN-Modus und versucht es mit „WLAN neu verbinden“ erneut – zuerst 5 Minuten nach dem Start, danach stündlich, im AP-Modus nur stündlich – der AP bleibt dabei erreichbar.
- Änderungen an den WLAN-Netzen (hinzufügen, überschreiben, wechseln, löschen) gelten sofort. Die Weboberfläche unterscheidet nicht zwischen Zugriff aus dem Heimnetz und von außen – die Uhr deshalb nicht per Port-Weiterleitung oder DMZ aus dem Internet erreichbar machen.
- NTP, bei Ausfall DCF77. Sobald die Uhr eine gültige Uhrzeit hat, ist sie selbst NTP-Server für andere Geräte im Netz.
- Uhrzeit aus offenen WLANs (Kästchen im Tab „Uhr Einstellungen“, ab Werk an): Kann sich die Uhr beim Start mit keinem gespeicherten WLAN verbinden (außer Reichweite oder kein Zugang, z. B. falsches Passwort) und ist keine RTC eingebaut, verbindet sich die Uhr kurz mit bis zu sechs offenen WLANs (gleiche Namen nur einmal, je 15 s, insgesamt höchstens 60 s) und holt die Zeit per NTP oder aus dem `Date:`-Kopf einer HTTP- bzw. HTTPS-Antwort, auch der Anmeldeseite eines Hotspots. Danach trennt sie wieder und läuft mit dieser Zeit normal weiter, ohne WPS und Access Point. Jeder Schritt steht im Log (Logging einschalten). Das sind fremde Netze – nur nutzen, wenn erlaubt.
- Bis zu 15 eigene NTP-Server. Ohne eigenen Server nutzt die Uhr `pool.ntp.org` und `ptbtime1.ptb.de`; sind die eigenen Server nicht erreichbar, versucht sie diese beiden zuletzt.

---

## 6. Weboberfläche

- Einstellungen in Tabs mit dunklem Design: WLAN, Uhr Einstellungen, Helligkeit, NTP Zeitzone, Status, Log und Rocrail (bei aktivierter Anbindung). Weitere Seiten über die Navigation: Vorschau, Uhren Sets, Zifferblatt, Zeiger, Dateimanager, DCF77, Sicherung, Werkseinstellungen.
- Deutsch und Englisch.
- Zifferblätter und Zeigersätze kommen aus den Designern, über die Sicherung und über das Upload-Formular der Seite „Zifferblatt“ auf die Uhr; es nimmt neben `face_*.bmp` auch Zeigersatz-Dateien (`hand_set<N>_hour/minute/second.bmp`) an. Die Seite „Zeiger“ und der Dateimanager haben kein Upload-Formular. Einen Download von GitHub gibt es nur für die Firmware (Seite „Sicherung“, ESP32-S3 und ESP32-S3-Touch-LCD-4), nicht für Zifferblätter oder Zeiger.
- Rocrail-Tab für die Modellzeit (Abschnitt 11).

---

## 7. Dateiverwaltung mit LittleFS

- Zifferblätter und Zeiger liegen komprimiert im Dateisystem. Dateien lassen sich in der Weboberfläche hoch- und herunterladen, umbenennen und löschen.
- Logging optional, ab Werk aus; es schaltet sich nach 24 Stunden oder nach 9 Logdateien selbst ab. Im Log-Tab mit Auswahl der Logdatei (neueste vorausgewählt), Auto-Refresh und Knopf „Herunterladen“; jede neue Logdatei beginnt mit einer Kopfzeile (Uhr, Hostname, Chip, Display, Build).

---

## 8. Zeitzonen-Anpassung

- Automatische Sommerzeit oder dauerhaft Sommer- bzw. Winterzeit.
- Ohne Zeitzone (leeres Feld) oder bei einem ungültigen POSIX-TZ-String nutzt die Uhr `CET-1CEST,M3.5.0,M10.5.0/3` (Mitteleuropäische Zeit).

---

## 9. Hardware-Integration

- **ESP32-S2** (Lolin S2 Pico):
  - GC9A01 (rund, 240 × 240); Bezugsquelle (Beispiel, keine Empfehlung): [Amazon](https://www.amazon.de/dp/B0CFXW8TTH)
  - GC9D01 (rund, 160 × 160); Bezugsquelle (Beispiel, keine Empfehlung): [Amazon](https://www.amazon.de/dp/B0HJ2SLTK8)
  - ILI9341 (rechteckig, 240 × 320)
  - Verdrahtung siehe `build_uhr4/readme.txt`, Platine im Ordner `PCB`
  - Auch ein ESP32-S2 Mini läuft mit dieser Firmware (sie ist für das Lolin S2 Pico kompiliert); Bezugsquelle (Beispiel, keine Empfehlung): [Amazon](https://www.amazon.de/dp/B0DDC49WWJ)
- **ESP32-C6** (Waveshare, Display fest verbaut):
  - [ESP32-C6-LCD-1.47](https://docs.waveshare.com/ESP32-C6-LCD-1.47/Resources-And-Documents) mit ST7789 (172 × 320); Bezugsquelle (Beispiel, keine Empfehlung): [Amazon](https://www.amazon.de/dp/B0DHTMYTCY)
  - [ESP32-C6-LCD-1.3](https://docs.waveshare.com/ESP32-C6-LCD-1.3/Resources-And-Documents) mit ST7789 (240 × 240); Bezugsquelle (Beispiel, keine Empfehlung): [Amazon](https://www.amazon.de/dp/B0GGXWCQ79)
- **ESP32-S3** (Waveshare, Display fest verbaut):
  - [ESP32-S3-LCD-1.28](https://docs.waveshare.com/ESP32-S3-LCD-1.28) mit GC9A01 (240 × 240, rund); Bezugsquelle (Beispiel, keine Empfehlung): [Amazon](https://www.amazon.de/dp/B0CSFFHNWT)
- **ESP32-S3-Touch-LCD-4** (Waveshare, Display fest verbaut):
  - [ESP32-S3-Touch-LCD-4](https://docs.waveshare.com/ESP32-S3-Touch-LCD-4) mit ST7701 (480 × 480, quadratisch, Touch); Bezugsquelle (Beispiel, keine Empfehlung): [Amazon](https://www.amazon.de/dp/B0DCZGB5LH)
- **Adafruit Qualia ESP32-S3** (Display ohne Controller-Platine, **experimentell, ungetestet**):
  - [Qualia ESP32-S3 for RGB666 Displays](https://learn.adafruit.com/adafruit-qualia-esp32-s3-for-rgb666-displays) mit dem 2,1" runden TL021WVC02 (480 × 480, Touch CST826)
- Fotowiderstand für die Helligkeit; die Uhr erkennt ihn beim Start.
- Optional: zweites, baugleiches Display (nur ESP32-S2, eigener Chip-Select), RTC DS3231 (hält die Uhrzeit über Stromausfälle, auch ohne WLAN; Bezugsquelle, Beispiel ohne Empfehlung: [Amazon](https://www.amazon.de/dp/B07LH592FM)), DCF77-Empfänger (Funkzeit ohne Internet; Bezugsquelle, Beispiel ohne Empfehlung: [ELV DCF-2](https://de.elv.com/p/elv-dcf-empfangsmodul-dcf-2-P091610/), ein Bausatz).
- Taster (oder der eingebaute Boot-Taster), je nach Haltedauer:
  - kurz: zeigt das verbundene WLAN,
  - 10–20 Sekunden: gelber Countdown „WiFi Reset“, Loslassen bricht ab,
  - 20–30 Sekunden: roter Countdown „Factory Reset“; jetzt loslassen löscht alle gespeicherten WLANs und startet neu – die Uhr geht in WPS/Access Point, alles andere bleibt,
  - ab 30 Sekunden: vollständiger Werksreset.

---

## 10. Weitere Funktionen

- Anzeige der Laufzeit seit dem letzten Neustart.
- Neustart über die Weboberfläche.
- Firmware-Update über WLAN (ESP32-S3 und ESP32-S3-Touch-LCD-4): auf der Seite „Sicherung“ die Datei `uhr4.ino.bin` aus `esp32s3` bzw. `esp32s3lcd4` hochladen (jedes Board nur seine eigene), mit Fortschrittsbalken; oder aus Visual Micro bzw. der Arduino IDE über den Netzwerk-Port der Uhr (ArduinoOTA, Port 3232, ohne Passwort). Die neue Firmware landet in der zweiten App-Partition und startet erst, wenn sie vollständig und für den ESP32-S3 ist; Einstellungen, Zifferblätter und Zeigersätze bleiben. Hängt ein Update, startet die Uhr nach 3 Minuten mit der alten Firmware neu. Außerdem kann man auf der Seite „Sicherung“ → „Firmware von GitHub“ per „Jetzt prüfen“ nachsehen, ob das neueste Release eine neuere Firmware hat (die Uhr sucht nie von selbst); eingespielt wird nur per Klick auf „Update einspielen“, die Datei wird per Zertifikat und SHA-256 geprüft.
- Hochgeladene BMP-Dateien lassen sich auf die Displaygröße skalieren.
- HTTP-API, z. B. `/api/setMode` (Zifferblatt, Zeigersatz, Zeigermodus, Nabe); die Uhren Sets sind solche URLs.
- Bis zu 50 Uhren Sets (Presets) mit Zifferblatt, Zeigersatz, Nabenfarbe und -größe, Sekundenzeiger (sichtbar, Stil), Bahnhofsmodus, sanftem Minutenzeiger, Zeitzone und automatischer Helligkeit (die Helligkeitswerte gehören zur Uhr und stehen nicht im Preset). Einzeln umbenennen und löschen, alphabetisch sortiert; alle zusammen in eine Datei sichern und wiederherstellen. Sind alle 50 Plätze belegt, erscheint eine Warnung.
- Komplettsicherung (Seite „Sicherung“):
  - Eine TAR-Datei mit allen Einstellungen, Uhren Sets, Zifferblättern, Zeigersätzen und Schriften; Zifferblätter und Zeiger darin als normale BMP-Bilder. Der Dateiname enthält Hostname, Chip (S2, S3, C6), Displaytyp, Datum und Uhrzeit.
  - Wiederherstellen auf dieser oder einer anderen Uhr; Sichern und Wiederherstellen zeigen einen Fortschrittsbalken. Vorhandene Zifferblätter, Zeigersätze und Uhren Sets bleiben; gleichnamige aus der Sicherung ersetzen sie. Zeigersätze der Sicherung mit einer Nummer, die hier die Firmware belegt (0–2, Sekundenfeld-Satz), speichert die Uhr unter der nächsten freien Nummer ab 3 und stellt die Uhren Sets darauf um; sind sie pixelgleich mit dem Satz der Firmware, entfallen sie.
  - WLAN-Zugangsdaten nur auf Wunsch, verschlüsselt mit einem Schlüssel, der in jeder uhr4-Firmware gleich ist – also nicht sicher.
  - Displaytyp, Rotation, Hintergrundbeleuchtung und Lichtsensor der Ziel-Uhr bleiben. Eine falsche oder beschädigte Datei ändert nichts.
  - Helligkeitswerte (Min./Max. Helligkeit, Schwellwerte, Gamma, Hell-/Dunkelzeiten) stehen in keiner Sicherung; die Uhr behält ihre eigenen, auch beim Einspielen älterer Sicherungen. Eine ganz neu geflashte Uhr startet mit der Min. Helligkeit 75.
  - Sicherung von einem anderen Displaytyp: Die Uhr weist darauf hin und übernimmt nur Zifferblätter, Zeiger (auf ihre Größe skaliert), Uhren Sets und allgemeine Einstellungen. Die Nabengröße (auch in den Uhren Sets) rechnet sie um, die Helligkeit bleibt. Das geht nur mit Zifferblättern von mindestens 240 × 240, sonst übernimmt sie nichts. Hat die Sicherung keinen Streifen, bekommt eine Uhr mit Streifen den Standard-Streifen.
- Wird ein Zifferblatt oder Zeigersatz gelöscht, löscht die Uhr auch die Uhren Sets, die darauf verweisen. War der gelöschte Zeigersatz aktiv, schaltet sie auf Satz 0.
- DCF77: Empfang auch bei schwachem oder gestörtem Signal. Die Uhr ordnet die Impulse einem Sekundenraster zu, ein fehlender Impuls verschiebt also nicht die folgenden Bits. Die Signalpolarität spielt keine Rolle. Ein gestörtes Telegramm setzt nie eine falsche Zeit.
- Live-Seite `/dcf77`: Bit-Fortschritt des aktuellen und das zuletzt dekodierte Telegramm.
- Optional blinkt eine LED während der Synchronisation, aber nur, wenn ein DCF77-Empfänger erkannt wurde.

---

## 11. Rocrail-Modellzeit

- Optionale Verbindung zu einem [Rocrail](https://wiki.rocrail.net/)-Server (Modellbahn-Steuerung): Die Zeiger zeigen dann die „Fast Clock“ (Modellzeit) des Servers. Einschalten mit dem Haken „Rocrail“ im Tab „Uhr Einstellungen“.
- Bis zu 15 Serveradressen; wie bei den NTP-Servern erscheint nach dem letzten Eintrag ein leerer Platz. Ein Radio-Button je Zeile wählt den aktiven Server. Die Uhr verbindet sich sofort beim Speichern, Aktivieren oder Neustart, nicht erst im minütlichen Zeitfenster.
- Kommen länger als 2 Minuten keine Updates, läuft die Uhr mit der NTP-/RTC-/DCF77-Zeit weiter. Die Modellzeit läuft mit dem Beschleunigungsfaktor von Rocrail („Divider“), die Bahnhofsuhr-Animation des Sekundenzeigers entsprechend schneller. Oberhalb eines Schwellwerts blendet die Uhr den Sekundenzeiger aus.
- Der Tab zeigt Verbindungsstatus, Divider und Modellzeit live. Zu jedem Server lässt sich ein Anlagenname eintragen.
- Meldet der Server einen Helligkeitswert, übernimmt die Uhr ihn (Abschnitt 4). Die Seite „Vorschau“ zeigt Modellzeit und Divider ebenso.

---

## Fertige Firmware flashen

Die fertigen `.bin`-Dateien liegen im Repository (`build_uhr4/esp32*`, auch im ZIP von *Code → Download ZIP*) und im [neuesten Release](https://github.com/holgiw/ESP32-Station-Clock/releases/latest) unter *Assets*; die Werkzeug-ZIP `uhr4_flash.zip` enthält die Builds aller Chips. Nur der neueste Commit behält die Firmware, ältere Commits verlieren sie.

- **Weg A, Werkzeug-ZIP:** `uhr4_flash.zip` aus dem Release komplett auspacken und `flashESP.bat` doppelklicken. Das Skript erkennt das Board am USB-Port und flasht den passenden Build, der im Werkzeug-ZIP mitliegt (wie im Repository-ZIP) (Ordner `esp32s2`, `esp32c6`, `esp32s3`, `esp32s3lcd4`, `esp32s3qualia`; kein Internet nötig). `esptool.exe` liegt ebenfalls im ZIP. `clocksetup.ps1 -Download` lädt den Build stattdessen vom Release (`uhr4-esp32<chip>.zip`, SHA-256 geprüft).
- **Weg B, Repository-ZIP:** `build_uhr4/flashESP.bat` genügt; das Repository enthält die Builds, das Skript flasht den passenden ohne Internet. Findet das Skript weder neben sich noch im ESP32-Core der Arduino IDE ein `esptool.exe`, lädt es dieses aus `uhr4_flash.zip` des Releases (SHA-256 geprüft). `clocksetup.ps1 -FetchTools` erzwingt das.
- **Weg C, von Hand:** `uhr4-v4-esp32<chip>-merged.bin` (je Chip eine Datei, ein Abbild für Adresse 0x0), z. B. für den ESP32-S3: `esptool.exe --chip esp32s3 --port COM5 --baud 921600 write_flash 0x0 uhr4-v4-esp32s3-merged.bin`. Das Abbild überschreibt den Bereich ab Adresse 0x0 bis zum Ende der App, dabei gehen die gespeicherten Einstellungen (WLAN, Displaytyp) verloren; sie müssen danach neu eingerichtet werden (Browser oder `flashESP.bat`).
- Meldet der Virenscanner `esptool.exe`, ist das ein bekannter Fehlalarm (gepacktes Python-Programm): Ausnahme für den Ordner einrichten und neu auspacken.

uhr4 steht unter der GNU General Public License v3.0 (siehe [LICENSE](LICENSE)): Du darfst es nutzen, ändern und
weitergeben – geänderte Versionen, auch als fertige Firmware, nur zusammen mit ihrem Quellcode und unter derselben
Lizenz. Die Lizenzen der enthaltenen Bibliotheken und Schriften (LovyanGFX, RTClib, arduino-esp32, esptool, DejaVu,
FreeSans, Orbitron) stehen in [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md).

Die HTTP-Schnittstellen der Uhr (Zeit, Steuerung, Firmware, Diagnose) stehen in [API.md](API.md).

## Kontakt

E-Mail: [carleaner@gmx.de](mailto:carleaner@gmx.de)  
GitHub: <https://github.com/holgiw/ESP32-Station-Clock>

---
---

# English Version

uhr4 shows the time as a clock face with hands on a TFT display. The hardware is an ESP32-S2 (Lolin S2 Pico) with a round (GC9A01, GC9D01) or rectangular display (ILI9341), a Waveshare ESP32-C6 with a built-in ST7789, a Waveshare ESP32-S3 with a built-in round GC9A01, or a Waveshare ESP32-S3-Touch-LCD-4 with a built-in square ST7701 (480 × 480, touch) or an Adafruit Qualia ESP32-S3 with a round 2.1" display (480 × 480, touch, untested). On the rectangular displays (ILI9341, ST7789 172 × 320) a strip next to the clock shows time and date, either above or below the clock (landscape: left or right). The time comes via NTP (WiFi), DCF77 or RTC. Clock faces, hands and all settings are adjusted in the web interface.

**Download:** firmware with flash tool under [Releases](https://github.com/holgiw/ESP32-Station-Clock/releases/latest) under *Assets* (instructions: section "Flashing the Ready-Made Firmware" and `build_uhr4/readme.txt`). The "Download ZIP" archive is flashable as well: the firmware files in `build_uhr4/esp32*` (four `.bin` per chip) are created by `buildAll.bat` and are in the repository. When updating from uhr3 (version 3), uhr4 keeps the display type.

## 1. Support for Multiple TFT Displays

One firmware per ESP. The display type is chosen in the "Clock Setup" tab or when flashing with `flashESP`; an update from uhr3 keeps it. Graphics library: LovyanGFX.

- **ESP32-S2** (Lolin S2 Pico, build `build_uhr4/esp32s2`):
  - GC9A01 – round, 240 × 240; without or with controlled backlight (BL)
  - GC9D01 – round, 160 × 160; BL controlled
  - ILI9341 – rectangular, 240 × 320; clock 240 × 240 (clock faces and hands as for the GC9A01) and a strip with time and date; fixed backlight
- **ESP32-C6** (Waveshare, display built in, build `build_uhr4/esp32c6`, without PSRAM; BL controlled; own partition table with 2.25 MB program and 1.6 MB file system – when building yourself copy `boards.local.txt` and `uhr4_c6.csv` into the core folder, see `boards.local.txt`; when switching from an older firmware take a backup first, the file system is created anew):
  - ESP32-C6-LCD-1.47: ST7789 – rectangular, 172 × 320; clock 172 × 172 and a strip with time and date
  - ESP32-C6-LCD-1.3: ST7789_240 – square, 240 × 240
- **ESP32-S3** (Waveshare ESP32-S3-LCD-1.28, display built in, build `build_uhr4/esp32s3`, 16 MB flash, 2 MB PSRAM; BL controlled; not yet tried on the device):
  - GC9A01 – round, 240 × 240
  - flash with two app partitions and 9.9 MB for clock faces, hands and fonts
  - firmware update over WiFi (see section 10)
  - rotation by hand or "automatic" via the built-in motion sensor (QMI8658): the clock face stays upright however the clock hangs; lying flat, the orientation stays. First select and save the fitting rotation by hand, then switch to "automatic" – the clock takes this position as its reference.
- **ESP32-S3-Touch-LCD-4** (Waveshare, display built in, build `build_uhr4/esp32s3lcd4`, 16 MB flash, 8 MB PSRAM):
  - ST7701_480 – square, 480 × 480, on the RGB bus; GT911 touch, CH32V003 port expander (PWM backlight, buzzer)
  - The image is in its own buffer in PSRAM; after flash writes (firmware update, restoring a backup) the clock re-synchronises the panel. During update and backup the display briefly shows the text and then a blue area.
  - Touch zones: the top right corner shows WiFi and IP address for 5 seconds (status), the bottom left corner loads the previous and the bottom right corner the next preset (order as on the "Presets" page); each zone is 160 × 160 pixels. Brightness via the PWM backlight ("Backlight PWM").
  - Known limits: about 22 frames per second for the second hand, rarely a short flicker or a panel restart (counter in `/api/lcdstats`). Own firmware: the GitHub update downloads `uhr4-s3lcd4.bin`.
- **Adafruit Qualia ESP32-S3** with the 2.1" round display TL021WVC02 (build `build_uhr4/esp32s3qualia`, 16 MB flash, 8 MB PSRAM; **EXPERIMENTAL and NOT TESTED ON THE DEVICE** – written from Adafruit's documents):
  - TL021_480 – round, 480 × 480, on the RGB bus (controller of the ST7701 family, start-up sequence via the PCA9554A port expander); touch CST826 (I2C 0x15)
  - The backlight can only be switched on and off (PWM only with a solder jumper on the board): brightness is done by darkening the pixels. The buttons UP and DOWN on the expander count like the button.
  - A touch shows WiFi and IP address for 5 seconds as on the Waveshare board; firmware update and backup show the calm blue area. The GitHub update downloads `uhr4-s3qualia.bin`.
- A second, identical display (ESP32-S2 only) with its own rotation (0°, 90°, 180°, 270°). A display set to "not connected (n.a.)" stays black, and the clock neither draws nor calculates anything for it. Default: display 1 at 0°, display 2 n.a. Status and boot messages (boot, access point mode) still appear on both displays until the clock starts; the settings page notes this.

---

## 2. Customizable Clock Hands and Faces

- Hour, minute and second hands and clock faces can be uploaded as BMP files, e.g. in the layout of Deutsche Bahn or other companies, or created in the designer.
- Starter set: whatever of it is missing, the clock draws anew at every start (after the first flash, a factory reset or if it was deleted); existing items stay
  - four clock faces, white with a rim, hour and minute marks: `face_default.bmp` (12, 3, 6, 9), `face_numbers.bmp` (1–12), `face_roman.bmp` (I–XII), `face_subdial.bmp` (12, 3, 9 and a seconds subdial in the lower third),
  - four hand sets: set 0 with black bars and a red second hand, set 1 the same with a black second hand, set 2 curved, a set like set 0 with a short red second hand with its disc for the subdial,
  - the presets "Standard", "1-12", "I-XII" and "Sekundenfeld", unless one with that name exists (I-XII without a second hand; on displays with a strip – ILI9341, ST7789 172 × 320 – with a white strip and black text).

  The factory reset actions "Delete Clock Faces", "Delete Hand Sets" and "Delete Presets" restore this state. The clock creates the subdial set under the first free number from 3 – on older clocks 3 can be an own set. At start the clock numbers the hand sets consecutively again (0, 1, 2, then the subdial set as 3, the others from 4 in their order); gaps close, presets and the active set follow. Hands in the old format are stored in the display's new format (padded transparent, display unchanged). An existing hand set with the number of a starter set is not completed. Every hand set has all three hands: if one is missing (e.g. after uploading only a `1_second.bmp`), the clock copies it from set 0 – at start, when the hand set overview is opened, when the set is activated and after a restore. It is then a hand of its own in the set.
  The generated clock faces and hand sets can be neither deleted nor renamed nor overwritten (web interface, upload, designer, restore); they come first in the overviews and are not part of the backup. In the designer only "Save as new …" under another name works for them. The factory reset actions "Delete Clock Faces" and "Delete Hand Sets" leave them in place.
- Time, date and weekday can be shown in the clock face designer: in the strip on the ILI9341 and ST7789 172 × 320, on all other displays (round, 240 × 240, 480 × 480) directly on the clock face behind the hands – off by default, stored per clock face, without seconds and not available without PSRAM (ESP32-C6).
- All hands are anti-aliased (LovyanGFX). Hour and minute hands live in a cached image, and only the area around the second hand goes to the display. That keeps the sweeping second hand smooth.
- Hand format 25 × 166 px with pivot 12 / 120 (160 display: 15 × 115, pivot 7 / 80): hands reach from the pivot to the display edge, below the pivot there are 15 more rows than before. Hand sets in the previous format 25 × 151 (15 × 100) and in the older format 21 × 131 (13 × 86) keep working; the clock pads them transparent when loading, the pivot stays at the same place. All combinations of width 21 or 25 and height 131, 151 or 166 are valid. On the 480×480 the hand width is 51 (odd, the symmetry axis lies on the middle pixel column); hands of width 50 and 42 stay valid. These sizes stay unchanged on upload, the clock scales all others to 21 × 131 (13 × 86). The hand designer saves at the old size as long as no hand extends beyond it – such sets also run on older firmware.
- In-browser hand designer ("Hand Set" page → "Designer" link below the hand set, which also activates it): design new hands based on the active set. Save as a new hand set (optionally activated right away) or over the active set, which then applies immediately. Tools: pen, line, rectangle, ellipse, polygon, fill, picker, colour choice (palette, colour picker, hex input), mirroring at the centre axis, undo, a shape generator (length, widths, counterweight, disc) and a live preview on the current clock face. The clock adapts hand size and pivot to its display.
- In-browser clock face designer ("Clock Face" page → "Designer" link below the clock face, which also activates it): design a new clock face based on the active one, or overwrite the active one and apply it right away. Tools: generator (background, rim, hour and minute marks, numerals 1-12, 12/3/6/9 or Roman), pixel tools with pen width, text (straight, rotated or on an arc, with installed or loaded fonts), logo stamp, mirror and rotation symmetry (4, 12 or 60 times), colour choice, image import (PNG, JPG, BMP ...), zoom and a live preview with the active hands. On round displays the invisible area is marked.
- The hub (centre over the hands) is also set in the clock face designer: radius and colour; the colour comes from the toolbox (picker, palette, colour field) and is taken over with "Current colour". The clock shows changes right away.
- Seconds subdial: in the clock face designer the second hand's pivot can be moved out of the centre, e.g. into the lower third. The second hand then lies below the hour and minute hand; a fittingly short second hand with its own disc is drawn in the hand designer. The pivot belongs to the clock face.

---

## 3. Smooth Minute and Train Station Modes

- Smooth minute hand: moves evenly instead of jumping every minute.
- Train station mode: the second hand goes round in 58.5 seconds and pauses briefly at the 12.
- Time jumps (boot, first valid time, DST change, Rocrail): the hands move to the new time in 3 seconds along the shortest path, backwards too.

---

## 4. Brightness Control

- Automatic brightness via a photoresistor with configurable thresholds, or manual.
- Backlight via PWM: the clock then dims the backlight instead of the pixels. The Brightness tab shows the "Backlight control" checkbox with the pin number for this.
  - ESP32-S2: pin 3, on by default for the display types "GC9A01 with BL" and GC9D01.
  - ESP32-C6: pin 22, hard-wired on the Waveshare board, on by default.
  - ESP32-S3: pin 40, hard-wired on the Waveshare board, on by default.
  - ESP32-S3-Touch-LCD-4: no pin, the PWM sits in the board's CH32V003 port expander; on by default.
- Photoresistor as a voltage divider with 10 kΩ: ESP32-S2 on GPIO 1 (3 V), 2 (measurement), 4 (GND); ESP32-C6 on GPIO 1 (3 V), 2 (measurement), 3 (GND) of the pin header; ESP32-S3 on GPIO 2 (3 V), 4 (measurement), 5 (GND) of the pin header; ESP32-S3-Touch-LCD-4: not possible (apart from display and I2C no GPIOs are free – no DCF77 receiver and no external button either; the BOOT button stays). Without a photoresistor the manual brightness applies.
- During setup (access point or WPS active) the display runs at the maximum brightness, at least 100 (of 255). At a normal start with a PWM backlight it first runs at the maximum brightness (boot messages), goes to the minimum brightness shortly before the first clock face as long as there is no time yet, and is then dimmed up slowly (about 5–8 s) to the value of the control (e.g. time window).
- Gamma correction only exists with a light sensor and PWM backlight. ESP32-C6: above 128 the brightness page warns of the backlight overheating (note from Waveshare).
- With Rocrail, the server can set the brightness (section 11).

---

## 5. WiFi and NTP Integration

- Up to 15 WiFi networks, setup via WPS as well, automatic reconnect, custom hostname.
- Setup while flashing: `flashESP.bat`/`flashESP.sh` recognize the ESP32-S2, ESP32-C6, ESP32-S3, ESP32-S3-Touch-LCD-4 and Adafruit Qualia (experimental, untested) by the USB port, ask for the display type (not on the two ESP32-S3) and a WiFi network (choice of all visible 2.4 GHz networks, password hidden) and send both to the clock via USB, finally the PC's time. They take the build of the detected chip from the folders `build_uhr4/esp32*` next to the script (in the repository, the repository ZIP and the tools ZIP, without internet); if the folders are missing they download it from the GitHub release (`uhr4-esp32s2.zip`, `-esp32c6.zip`, `-esp32s3.zip`, `-esp32s3lcd4.zip`, `-esp32s3qualia.zip`, with a SHA-256 check); `clocksetup.ps1 -Download` forces the download. After flashing the COM port stays free (serial monitor only with `clocksetup.ps1 -Flash -Monitor`). For development `clocksetup.ps1 -Local` or `UHR4_LOCAL=1` flashes the folders `build_uhr4/esp32*`; the packages are created by `build_uhr4/publish_builds.ps1`.
- Tools in the project folder: `buildAll.bat` builds the four targets (s3, s2, c6, s3lcd4), the Qualia (`s3qualia`) only when named with arduino-cli and copies them to `build_uhr4` (no user name in the binary, `-ffile-prefix-map`; `publish_builds.ps1` aborts otherwise); `build_uhr4/cleanup_history.ps1` removes the firmware from all older commits after a new build commit; `checkClocks.bat` lists all uhr4 on the network with chip, display and firmware state; `flashAll.bat` flashes outdated clocks (ESP32-S3 and ESP32-S3-Touch-LCD-4 over WiFi, each with its own build; ESP32-S2 and ESP32-C6 only on USB). For every clock flashed on USB, `flashAll.bat` then asks for the WiFi like `flashESP.bat` (Enter = unchanged); with `-Yes` the question is skipped.
- To only set the time, e.g. on a clock without WiFi, DCF77 and RTC: `setTime.bat`/`setTime.sh` or the "Use device time" button in the web interface (time of the phone or PC, also in access point mode).
- Other ways into WiFi: WPS or the setup access point (SSID `clock123`, password `clocksetup`). The clock only starts the access point at boot when no WiFi is stored or none is reachable (with an RTC or with a time from an open WiFi only without a stored WiFi). If WiFi drops during operation, the clock stays in WiFi mode and retries with "Reconnect WiFi" – first 5 minutes after boot, then hourly, in AP mode only hourly – the AP stays reachable meanwhile.
- Changes to the WiFi networks (add, overwrite, switch, delete) apply immediately. The web interface does not distinguish between access from the home network and from outside – so do not expose the clock to the internet via a port forward or DMZ.
- NTP, with DCF77 as a fallback. Once the clock has a valid time, it is an NTP server for other devices on the network.
- Time from open WiFis (checkbox in the "Clock Setup" tab, on by default): if the clock cannot connect to any stored WiFi at boot (out of range or no access, e.g. a wrong password) and there is no RTC, the clock briefly connects to up to six open WiFis (each name once, 15 s each, 60 s in total at most) and gets the time via NTP or from the `Date:` header of an HTTP or HTTPS reply, including the login page of a hotspot. Then it disconnects again and runs normally with that time, without WPS and access point. Every step is in the log (enable logging). These are networks of others – only use it if allowed.
- Up to 15 custom NTP servers. Without a custom server the clock uses `pool.ntp.org` and `ptbtime1.ptb.de`; if the custom servers are unreachable, it tries these two last.

---

## 6. Web Interface

- Settings in tabs with a dark theme: WiFi Settings, Clock Setup, Brightness, NTP Timezone, Status, Log and Rocrail (when the connection is enabled). Further pages via the navigation bar: Preview, Presets, Clock Face, Hand Set, File Manager, DCF77, Backup, Factory Reset.
- German and English.
- Clock faces and hand sets come onto the clock from the designers, via the backup and via the upload form of the "Clock Face" page; besides `face_*.bmp` it also accepts hand set files (`hand_set<N>_hour/minute/second.bmp`). The "Hand Set" page and the file manager have no upload form. A download from GitHub exists only for the firmware ("Backup" page, ESP32-S3 and ESP32-S3-Touch-LCD-4), not for clock faces or hands.
- Rocrail tab for the model time (section 11).

---

## 7. File Management with LittleFS

- Clock faces and hands are stored compressed in the file system. Files can be uploaded, downloaded, renamed and deleted in the web interface.
- Optional logging, off by default; it switches itself off after 24 hours or 9 log files. In the Log tab with a log file selector (newest preselected), auto-refresh and a "Download" button; every new log file starts with a header line (clock, host name, chip, display, build).

---

## 8. Time Zone Customization

- Automatic daylight saving time, or permanent summer or winter time.
- Without a time zone (empty field) or with an invalid POSIX TZ string the clock uses `CET-1CEST,M3.5.0,M10.5.0/3` (Central European Time).

---

## 9. Hardware Integration

- **ESP32-S2** (Lolin S2 Pico):
  - GC9A01 (round, 240 × 240); source (example, not a recommendation): [Amazon](https://www.amazon.de/dp/B0CFXW8TTH)
  - GC9D01 (round, 160 × 160); source (example, not a recommendation): [Amazon](https://www.amazon.de/dp/B0HJ2SLTK8)
  - ILI9341 (rectangular, 240 × 320)
  - Wiring see `build_uhr4/readme.txt`, PCB in the `PCB` folder
  - An ESP32-S2 Mini also runs this firmware (it is compiled for the Lolin S2 Pico); source (example, not a recommendation): [Amazon](https://www.amazon.de/dp/B0DDC49WWJ)
- **ESP32-C6** (Waveshare, display built in):
  - [ESP32-C6-LCD-1.47](https://docs.waveshare.com/ESP32-C6-LCD-1.47/Resources-And-Documents) with ST7789 (172 × 320); source (example, not a recommendation): [Amazon](https://www.amazon.de/dp/B0DHTMYTCY)
  - [ESP32-C6-LCD-1.3](https://docs.waveshare.com/ESP32-C6-LCD-1.3/Resources-And-Documents) with ST7789 (240 × 240); source (example, not a recommendation): [Amazon](https://www.amazon.de/dp/B0GGXWCQ79)
- **ESP32-S3** (Waveshare, display built in):
  - [ESP32-S3-LCD-1.28](https://docs.waveshare.com/ESP32-S3-LCD-1.28) with GC9A01 (240 × 240, round); source (example, not a recommendation): [Amazon](https://www.amazon.de/dp/B0CSFFHNWT)
- **ESP32-S3-Touch-LCD-4** (Waveshare, display built in):
  - [ESP32-S3-Touch-LCD-4](https://docs.waveshare.com/ESP32-S3-Touch-LCD-4) with ST7701 (480 × 480, square, touch); source (example, not a recommendation): [Amazon](https://www.amazon.de/dp/B0DCZGB5LH)
- **Adafruit Qualia ESP32-S3** (display without controller board, **experimental, untested**):
  - [Qualia ESP32-S3 for RGB666 Displays](https://learn.adafruit.com/adafruit-qualia-esp32-s3-for-rgb666-displays) with the 2.1" round TL021WVC02 (480 × 480, CST826 touch)
- Photoresistor for brightness; the clock detects it at startup.
- Optional: a second, identical display (ESP32-S2 only, own chip select), RTC DS3231 (keeps the time across power loss, also without WiFi; source, example without recommendation: [Amazon](https://www.amazon.de/dp/B07LH592FM)), DCF77 receiver (radio time without internet; source, example without recommendation: [ELV DCF-2](https://de.elv.com/p/elv-dcf-empfangsmodul-dcf-2-P091610/), a kit).
- Button (or the built-in Boot button), depending on how long it is held:
  - short: shows the connected WiFi,
  - 10–20 seconds: yellow "WiFi Reset" countdown, releasing aborts,
  - 20–30 seconds: red "Factory Reset" countdown; releasing now deletes all stored WiFi networks and restarts – the clock goes into WPS/access point, everything else stays,
  - from 30 seconds: full factory reset.

---

## 10. More Features

- Uptime since the last restart.
- Restart via the web interface.
- Firmware update over WiFi (ESP32-S3 and ESP32-S3-Touch-LCD-4): upload the file `uhr4.ino.bin` from `esp32s3` or `esp32s3lcd4` on the "Backup" page (each board only its own), with a progress bar; or from Visual Micro or the Arduino IDE via the clock's network port (ArduinoOTA, port 3232, without password). The new firmware goes to the second app partition and only starts if it is complete and for the ESP32-S3; settings, clock faces and hand sets stay. If an update hangs, the clock restarts with the old firmware after 3 minutes. On the "Backup" page → "Firmware from GitHub", "Check now" shows whether the latest release has a newer firmware (the clock never checks by itself); it is installed only by clicking "Install update", the file is checked by certificate and SHA-256. The files `uhr4-s3.bin`/`uhr4-s3.txt` (ESP32-S3-LCD-1.28) and `uhr4-s3lcd4.bin`/`uhr4-s3lcd4.txt` (ESP32-S3-Touch-LCD-4) of the release are created by `build_uhr4/publish_s3_update.ps1`.
- Uploaded BMP files can be scaled to the display size.
- HTTP API, e.g. `/api/setMode` (clock face, hand set, hand mode, hub); presets are such URLs.
- Up to 50 presets with clock face, hand set, hub colour and size, second hand (visible, style), station mode, smooth minute hand, time zone and automatic brightness (the brightness values belong to the clock and are not in the preset). Rename and delete them individually, sorted alphabetically; back up all of them to one file and restore them. A warning appears once all 50 slots are full.
- Full backup (Backup page):
  - One TAR file with all settings, presets, clock faces, hand sets and fonts; clock faces and hands inside as normal BMP images. The file name contains host name, chip (S2, S3, C6), display type, date and time.
  - Restore on this or another clock; backup and restore show a progress bar. Existing clock faces, hand sets and presets stay; same-named ones from the backup replace them. Hand sets of the backup whose number the firmware occupies here (0–2, subdial set) are stored under the next free number from 3 and the presets are switched to it; if they are pixel-identical to the firmware's set they are dropped.
  - WiFi credentials only on request, encrypted with a key that is the same in every uhr4 firmware – so not secure.
  - Display type, rotation, backlight and light sensor of the target clock stay. A wrong or damaged file changes nothing.
  - Brightness values (min./max. brightness, thresholds, gamma, bright/dark times) are in no backup; the clock keeps its own, also when restoring older backups. A completely freshly flashed clock starts with the min. brightness 75.
  - Backup from another display type: the clock points this out and only takes over clock faces, hands (scaled to its size), presets and general settings. It converts the hub size (also in the presets), the brightness stays. This only works with clock faces of at least 240 × 240, otherwise it takes over nothing. If the backup has no strip, a clock with a strip gets the standard strip.
- Deleting a clock face or hand set also deletes the presets that refer to it. If the deleted hand set was active, the clock switches to set 0.
- DCF77: reception even with a weak or disturbed signal. The clock places the pulses on a one-second grid, so a missing pulse does not shift the following bits. The signal polarity does not matter. A disturbed telegram never sets a wrong time.
- Live page `/dcf77`: bit progress of the current telegram and the last decoded one.
- Optional LED blink during synchronization, only if a DCF77 receiver was detected.

---

## 11. Rocrail Model Time

- Optional connection to a [Rocrail](https://wiki.rocrail.net/) server (model railway control): the hands then show the server's "fast clock" (model time). Enable it with the "Rocrail" checkbox in the "Clock Setup" tab.
- Up to 15 server addresses; as with the NTP servers, an empty slot appears after the last entry. A radio button per row selects the active server. The clock connects right away on save, enable or restart, not only in the once-a-minute window.
- If no updates arrive for more than 2 minutes, the clock continues with the NTP/RTC/DCF77 time. Model time runs at Rocrail's acceleration factor (the "divider"), the station-clock second-hand animation correspondingly faster. Above a threshold the clock hides the second hand.
- The tab shows connection status, divider and model time live. Each server can get a layout name.
- If the server reports a brightness value, the clock takes it over (section 4). The Preview page shows model time and divider the same way.

---

## Flashing the Ready-Made Firmware

The ready-made `.bin` files are in the repository (`build_uhr4/esp32*`, also in the ZIP from *Code → Download ZIP*) and in the [latest release](https://github.com/holgiw/ESP32-Station-Clock/releases/latest) under *Assets*; the tools ZIP `uhr4_flash.zip` contains the builds of all chips. Only the newest commit keeps the firmware, older commits lose it.

- **Way A, tools ZIP:** unpack `uhr4_flash.zip` from the release completely and double-click `flashESP.bat`. The script recognizes the board on the USB port, flashes the matching build, which is in the tools ZIP (as in the repository ZIP) (folders `esp32s2`, `esp32c6`, `esp32s3`, `esp32s3lcd4`, `esp32s3qualia`; no internet needed). `esptool.exe` is in the ZIP as well. `clocksetup.ps1 -Download` downloads the build from the release instead (`uhr4-esp32<chip>.zip`, SHA-256 checked).
- **Way B, repository ZIP:** `build_uhr4/flashESP.bat` is enough; the repository contains the builds, the script flashes the matching one without internet. If the script finds no `esptool.exe` next to itself nor in the ESP32 core of the Arduino IDE, it downloads one from `uhr4_flash.zip` of the release (SHA-256 checked). `clocksetup.ps1 -FetchTools` forces this.
- **Way C, by hand:** `uhr4-v4-esp32<chip>-merged.bin` (one file per chip, one image for address 0x0), e.g. for the ESP32-S3: `esptool.exe --chip esp32s3 --port COM5 --baud 921600 write_flash 0x0 uhr4-v4-esp32s3-merged.bin`. The image overwrites the area from address 0x0 to the end of the app, which loses the saved settings (WiFi, display type); set them up again afterwards (browser or `flashESP.bat`).
- If the virus scanner reports `esptool.exe`, that is a known false alarm (packed Python program): add an exception for the folder and unpack again.

uhr4 is licensed under the GNU General Public License v3.0 (see [LICENSE](LICENSE)): you may use, modify and
share it – modified versions, including ready-made firmware, only together with their source code and under the
same license. The licenses of the included libraries and fonts (LovyanGFX, RTClib, arduino-esp32, esptool, DejaVu,
FreeSans, Orbitron) are listed in [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md).

The clock's HTTP interfaces (time, control, firmware, diagnostics) are described in [API.md](API.md).

## Contact

E-mail: [carleaner@gmx.de](mailto:carleaner@gmx.de)  
GitHub: <https://github.com/holgiw/ESP32-Station-Clock>
