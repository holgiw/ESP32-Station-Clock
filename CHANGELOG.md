# Changelog

*English version below.*

## 2026-10-12

### Hinzugefügt
- Neues Ziel (EXPERIMENTELL): Waveshare ESP32-S3-Touch-LCD-4 (480x480, ST7701 am RGB-Bus, Touch GT911, Port-Expander CH32V003). Eigener
  Build (`esp32s3lcd4`, PSRAM octal). Das Bild kommt aus einem eigenen Puffer im PSRAM; nach Flash-Schreibzugriffen
  (Firmware-Update, Backup einspielen) wird das Panel neu synchronisiert. Beim Firmware-Update zeigt das
  Display nur eine ruhige blaue Fläche (kein Text), bei der Backup-Wiederherstellung kurz den Text und danach Blau.
- Neues Ziel (EXPERIMENTELL, NICHT AM GERÄT GETESTET): Adafruit Qualia ESP32-S3 mit dem 2,1" runden Display TL021WVC02 (480x480, Touch CST826,
  Port-Expander PCA9554A). Eigener Build (`esp32s3qualia`, Option `-DUHR4_BOARD_QUALIA`), Displaytyp `TL021_480`; Pins, Zeiten und Startfolge
  nach den Unterlagen von Adafruit. Beleuchtung nur an/aus (Helligkeit durch Abdunkeln der Pixel); die Tasten UP/DOWN am Expander zählen wie der
  Taster. `buildAll.bat`, `flashESP`, `flashAll`, `checkClocks` und `publish_*.ps1` kennen das Board; `flashESP` erkennt ein fabrikneues
  Qualia (Adafruit-Kennung 239A) und erklärt den Weg in den Download-Modus (BOOT0 halten, Reset drücken).
- 480x480: Berührung des Displays zeigt 5 Sekunden lang die Seite mit WLAN und IP-Adresse.
- 480x480: Die Helligkeit wird über das PWM-Register des Port-Expanders geregelt („Hintergrundbeleuchtung PWM“, ab
  Werk an); der Test-Knopf in den Helligkeitseinstellungen funktioniert wie bei den anderen Uhren.
- 480x480: Der Beeper wird beim Start und regelmäßig im Betrieb ausgeschaltet.
- Firmware-Update von GitHub: Das 480x480-Board lädt `uhr4-s3lcd4.bin` (die Datei `uhr4-s3.bin` gehört zum ESP32-S3-LCD-1.28).
- buildAll.bat baut vier Ziele (s3, s2, c6, s3lcd4) und kopiert den Build des 480x480-Boards nach `build_uhr4\esp32s3lcd4`.
- flashESP (Windows und Linux), flashAll und checkClocks kennen das 480x480-Board: Es wird am Displaytyp ST7701_480 bzw.
  per esptool (chip_id) vom ESP32-C6 unterschieden (beide melden sich als USB-Serial-JTAG), erhält nur seinen eigenen
  Build und wird nicht nach dem Displaytyp gefragt. publish_builds.ps1 und publish_s3_update.ps1 veröffentlichen auch
  `uhr4-esp32s3lcd4` bzw. `uhr4-s3lcd4`.

- Zifferblatt-Designer: Knopf „Schärfen“ (Unschärfemaske) mit einstellbarer Stärke (10 bis 300 %); schärft das ganze Zifferblatt, ist mehrfach anwendbar und mit „Rückgängig“ zurückzunehmen.
- Zifferblatt-Designer: Pfeilknöpfe „Verschieben“ schieben das ganze Zifferblatt um 1 Pixel (mit Umschalt um 10) nach links, rechts, oben oder unten; frei werdende Ränder erhalten die gewählte Farbe. Rückgängig ist möglich.
- Zifferblatt- und Zeigersatz-Seite: Das aktive Zifferblatt bzw. der aktive Zeigersatz hat einen roten Rahmen; er wandert mit der Auswahl.
- Beim Umbenennen eines Zifferblatts wird „face_“ (und „.bmp“) automatisch ergänzt, wenn es im neuen Namen fehlt; Speichern im Designer und Hochladen taten das schon.
- Dateimanager: Alle Dateien außer denen des Startpakets lassen sich umbenennen (bisher nur .bmp). Schriften behalten ihr Präfix („font_“, „stripfont_“), fehlt die Endung, bleibt die alte.
- README: Bezugsquellen (Beispiele, keine Empfehlung) für Boards und Displays; THIRD_PARTY_LICENSES um FreeSerif und die Qualia-Startfolge ergänzt.
- Uhrzeit, Datum und Wochentag lassen sich jetzt auch bei Displays ohne Streifen anzeigen (rund, 240x240, 480x480): im Zifferblatt-Designer unter
  „Uhrzeit, Datum, Wochentag auf dem Zifferblatt“ (gleiche Einstellungen wie der Streifen: Schrift, Farbe, Größe, Format, Position; je Zifferblatt in
  `stripcfg_*.txt`). Ab Werk aus. Der Text liegt hinter den Zeigern und steckt im Zifferblatt-Cache, ohne Sekunden und ohne blinkenden Doppelpunkt; er
  ändert sich einmal pro Minute (auf dem 480er-Board kostet das ein kurzes Aussetzen des Sekundenzeigers). Ohne PSRAM (ESP32-C6) nicht verfügbar.
- Neue Adressen zur Kontrolle: `/api/faceimg` (Zifferblatt-Cache, wie er zum Display geht) und `/api/frameimg` (zuletzt gezeichnetes Bild mit Zeigern), beide RGB565.
- Die Seite „Vorschau“ und die Vorschaubilder der Presets zeigen Uhrzeit, Datum und Wochentag auf dem Zifferblatt (Displays ohne Streifen), wenn das Zifferblatt sie anzeigt; in den Presets mit der Demo-Zeit 10:10:30.
- Waveshare ESP32-S3-Touch-LCD-4: Beim Start piept der Beeper 500 ms lang (Beeper an Expander-Ausgang EXIO6); Prüfadresse `/api/beeptest`.
- 480x480: Der Minutenzeiger gleitet 300 ms statt 600 ms.
- Zifferblatt-Designer: Das Werkzeug „Anzeige“ sendet die neue Position erst beim Speichern; Erfolgsmeldungen verschwinden nach 4 s und sind höchstens 420 px breit.
- Sicherung von einem anderen Displaytyp: Die Zifferblätter der Sicherung müssen mindestens 240 × 240 groß sein (bisher genau 240 × 240).
- 480x480: Sekundenzeiger, Stunden-, Minutenzeiger und neuer Text werden als getrennte kleine Rechtecke gezeichnet und gesendet statt als ein großes; Rechtecke, die zusammen nicht teurer sind als einzeln, werden zusammengefasst.
- Beim Minutenwechsel wird nur das Zeilenband mit dem Text kopiert und neu gezeichnet (24 statt 100 ms), auch bei gedrehtem Zifferblatt als Teilbild.
- Datumsformat „T“ (nur der Tag); Wochentag wahlweise ausgeschrieben oder kurz (Mo / Mon). Beides je Zifferblatt gespeichert.
- Zifferblatt-Designer: Pipette neben der Schriftfarbe; Uhrzeit, Datum und Wochentag erscheinen auch in der Zeichenfläche und lassen sich dort mit dem Werkzeug „Anzeige“ mit der Maus verschieben; die Vorschau (240 px breit) steht oben in der rechten Spalte und scrollt nicht weg; beide Designer nutzen die ganze Bildschirmbreite.
- Beim Minutenwechsel wird nur der Text im Zifferblatt-Cache neu gezeichnet (statt den Cache ganz neu zu rechnen); die Vorschau des Designers überträgt für die Anzeige auf dem Zifferblatt nur die Zeilen mit Text (100 KB statt 920 KB), die Uhr hält dabei kaum noch an. Zuvor blieb der Sekundenzeiger nach jedem Minutenwechsel 2–3 s auf der 0 stehen, solange der Designer offen war.
- Abfragen des Dateisystems werden zwischengespeichert (`fs_cache.h`: freier Platz, Existenz von Dateien, Dateiliste): Seiten wie „Zeiger“ (vorher 1,7 s) und der Zeigerdesigner (0,5 s) laden in etwa 0,5 s bzw. 0,1 s, der Sekundenzeiger steht beim Seitenaufruf kürzer still. Nach jedem Schreibzugriff werden die Werte neu gelesen.
- 480x480: Beim Speichern eines Zifferblatts oder Zeigers und beim Hochladen zeigt das Display eine ruhige blaue Fläche statt zu flackern (bis 1,5 s nach dem letzten Schreibzugriff).

### Geändert
- Zeigersatz „Sekundenfeld“ (Satz 3): Die Nabe des Sekundenzeigers ist noch einmal halbiert.
- ESP32-S3-Touch-LCD-4: Der Sekundenzeiger wird in doppelter Auflösung gehalten und mit Kantenglättung auf halbe Größe gedreht (Deckung aus 2 × 2 Unterpixeln); hochgerechnete Zeiger wirken dadurch weniger pixelig.
- `flashESP.bat` lässt nach dem Flashen die COM-Schnittstelle frei (kein serieller Monitor mehr); den Monitor gibt es mit
  `clocksetup.ps1 -Flash -Monitor`.
- Eine ganz neu geflashte Uhr startet mit der Helligkeit „dunkel“ (Min. Helligkeit) 75 statt 100 bzw. 5; Updates ändern
  gespeicherte Werte nicht.
- Sicherungen enthalten keine Helligkeitswerte mehr (Min./Max. Helligkeit, Schwellwerte, Gamma, Hell-/Dunkelzeiten);
  beim Wiederherstellen behält die Uhr ihre eigenen Werte, auch aus älteren Sicherungen.
  Presets speichern und übernehmen sie ebenfalls nicht mehr (es gilt der Wert der Uhr); nur „Automatische Helligkeit“
  bleibt Teil eines Presets.

## 2026-10-08

### Hinzugefügt
- Seite „Log“: Der Knopf „Herunterladen“ lädt die angezeigte (standardmäßig die aktuelle) Logdatei herunter.
- Jede neue Logdatei beginnt mit einer Kopfzeile: Uhr (uhr4), Hostname, ESP32-Chip (Modell, Revision, Arduino-Core),
  Display und Kompilierdatum/-zeit.
- Zifferblatt-Designer: Auch Stunden- und Minutenzeiger (zusammen) lassen sich über das ganze Zifferblatt verschieben
  („Drehpunkt Stunde und Minute“, X/Y oder Mitte). Die Nabe sitzt auf diesem Drehpunkt. Ein Sekundenzeiger ohne eigenen
  Drehpunkt sitzt auf dem der anderen Zeiger. Der Haken „Sekundenzeiger über den anderen Zeigern“ legt die Lage fest;
  ohne eigene Wahl gilt die Regel: über den anderen, wenn alle denselben Drehpunkt haben, sonst darunter (Sekundenfeld).
  Drehpunkte und Lage stehen im Anhang der Zifferblatt-Datei (jetzt 14 statt 8 Byte, ältere Dateien bleiben lesbar).
- Helligkeitsseite: Neben „Minimale“ und „Maximale Helligkeit“ gibt es je einen Knopf „Test 10 s“. Er zeigt den Wert im
  Feld (auch ungespeichert) 10 Sekunden lang auf der Uhr, danach gilt wieder die normale Helligkeit.
- Das Kästchen „Uhrzeit aus offenen WLANs holen“ steht jetzt im Tab „Uhr Einstellungen“ (unter „WLAN neu verbinden“).
- MQTT-Meldung (alle Chips, ab Werk aus, Seite „MQTT“ in der Navigation): Die Uhr veröffentlicht ihren Status als
  retained-Nachrichten an einen MQTT-Broker, z. B. den von openHAB - einmal nach dem Start, einmal um Mitternacht und wenn
  sich ihre IP-Adresse ändert: `<Basis-Topic>/<Hostname>/` mit `info` (JSON mit `host`, `ip`, `chip`, `display`, `version`; daraus kann openHAB eine
  Uhrenliste mit Links bauen), `ip`, `host`, `chip`, `display`, `version`, `time_ok`, `heap_kb`, `rssi` und `uptime_s`
  (ohne Uhrzeit). Ein schlanker Client ohne Bibliothek (MQTT 3.1.1, QoS 0); das Passwort
  steht nicht in der Sicherung. Schlägt eine Meldung fehl, folgt der nächste Versuch nach 2, 4, 8 und dann alle
  15 Minuten. Die Statusseite zeigt das letzte Ergebnis.
- `flashAll.bat` (Projektordner, ruft `flashAll.ps1` auf) prüft alle Uhren im Netz und bringt veraltete auf den Stand
  der Builds in `build_uhr4`: ESP32-S3 per WLAN (OTA), ESP32-S2 und -C6 (kein OTA) nur, wenn sie am USB hängen.
  Vor jedem Flash wird gefragt; `-CheckOnly` prüft nur, `-Yes` fragt nicht, `-Force` spielt auch aktuelle S3 neu ein.
  `checkClocks.ps1` hat dafür den Schalter `-PassThru`.
- `checkClocks.ps1` (Projektordner) sucht alle uhr4-Uhren im Netz und zeigt Adresse, Name, Chip, Display, laufende
  Firmware, ob sie dem neuesten Build in `build_uhr4` entspricht, ob die Uhrzeit stimmt und den freien Speicher.
  Mit `-All` (auch über `checkClocks.bat`) listet es vorher alle Geräte im Netz auf, auch Nicht-Uhren (Adresse, MAC,
  Name, Hinweis wie Seitentitel oder Espressif).
- Steht ein Update auf GitHub bereit, zeigt die Seite „Sicherung“ einen Link zum Changelog (neueste Einträge oben):
  Was seit der installierten Version neu ist.
- Update von GitHub (nur ESP32-S3): Auf der Seite „Sicherung“ → „Firmware von GitHub“
  zeigt „Jetzt prüfen“, ob das neueste Release eine neuere Firmware hat (die Uhr sucht nie von selbst; Zeile
  „Firmware on GitHub“ auf der Statusseite nach einer Prüfung). Eingespielt wird nur per Klick auf „Update einspielen“; die Uhr lädt
  `uhr4-s3.bin`, prüft Zertifikat (eingebautes Zertifikatspaket, dafür muss die Uhrzeit stimmen), Chip-Kopf und
  SHA-256 aus `uhr4-s3.txt` und aktiviert die Firmware erst danach. Die beiden Dateien legt
  `build_uhr4/publish_s3_update.ps1` in das neueste Release.
- Eigener Build für den Waveshare ESP32-S3-LCD-1.28 (rundes GC9A01 fest verbaut, 16 MB Flash, 2 MB PSRAM),
  Ordner `build_uhr4/esp32s3`; noch nicht am Gerät erprobt:
  - Board „ESP32S3 Dev Module“, Flash Size 16MB, PSRAM „QSPI PSRAM“, Partition Scheme „16M Flash (3MB APP/9.9MB
    FATFS)“, USB CDC On Boot „Disabled“ (USB läuft über den Wandler CH343P). Falsche Einstellungen brechen den
    Build mit einer Meldung ab.
  - Zwei App-Partitionen, LittleFS mit 9,9 MB auf der Partition „ffat“.
  - Firmware-Update über WLAN: Seite „Sicherung“ → „Firmware-Update“ (`uhr4.ino.bin` hochladen, mit
    Fortschrittsbalken) oder ArduinoOTA aus Visual Micro/Arduino IDE (Port 3232, ohne Passwort). Die Uhr
    prüft Kopf und Chip der Datei, lehnt merged-, Bootloader- und Partitionsdateien ab und startet die neue
    Firmware erst nach vollständiger Prüfung. Während des Updates zeigt das Display den Fortschritt, die
    übrige Uhr pausiert, WLAN-Scans unterbleiben, das Funk-Energiesparen ist aus; hängt ein Update, startet
    die Uhr nach 3 Minuten neu.
  - Beleuchtung an GPIO 40, ab Werk geregelt. Fotowiderstand an GPIO 2/4/5, Taster 16, DCF77 17, RTC am I2C
    6/7 des eingebauten Lagesensors. Das Board hat keine LED, die Option „DCF77 Sync LED Blink“ entfällt.
  - Rotation „automatisch“ (Tab „Uhr Einstellungen“, nur dieses Board): Der Lagesensor QMI8658 dreht das
    Zifferblatt aufrecht, sobald eine neue Lage 1,5 s anliegt; flach liegend bleibt die Rotation. Beim
    Einschalten nimmt die Uhr die aktuelle Lage als Bezug für die gerade eingestellte Rotation. Die
    Statusseite zeigt Lage und Bezug.
  - `flashESP.bat`/`flashESP.sh` erkennen das Board am CH343P (USB-Kennung 1A86:55D3), fragen keinen
    Displaytyp ab und flashen den Build aus `esp32s3`. Antwortet am Wandler keine uhr4, fragen sie vorher
    nach.
- Uhrzeit aus offenen WLANs (alle Builds, Tab „NTP Zeitzone“, ab Werk an): Verbindet sich die Uhr beim Start
  mit keinem gespeicherten WLAN (außer Reichweite oder kein Zugang, z. B. falsches Passwort), verbindet sie sich
  ohne RTC kurz mit bis zu sechs offenen WLANs (ab -85 dBm, gleiche Namen nur einmal, je 15 s, insgesamt höchstens 60 s) und holt die
  Zeit: NTP, dann der `Date:`-Kopf einer HTTP-Antwort (erst ohne DNS: Gateway, 1.1.1.1; dann per Name),
  zuletzt einmal der Weiterleitung zur Anmeldeseite folgen, per HTTPS (TLS ohne Zertifikatsprüfung) und HTTP.
  Danach trennt sie wieder und geht wie mit RTC in den normalen Uhrenbetrieb, ohne WPS und Access Point; das
  gespeicherte WLAN bleibt und wird mit „WLAN neu verbinden“ stündlich wieder versucht. Das Log nennt jeden
  Schritt: Grund, Netz, IP, Weg und Ergebnis. Kostet etwa 100 KB Flash (TLS).
- Zifferblatt-Designer, Streifen Uhrzeit/Datum: Die Einstellungen stehen in drei Gruppen Uhrzeit, Datum und
  Wochentag. Jede Gruppe hat „anzeigen“, einen Regler „Größe“ und die Position (automatisch oder X/Y); dazu
  Zeitformat, Sekunden und Blinken bei der Uhrzeit, das Datumsformat beim Datum.
  - Neu: Wochentag ausgeschrieben in der Sprache der Weboberfläche (Deutsch, sonst Englisch), ab Werk aus.
    Automatisch stehen die eingeschalteten Zeilen in der Reihenfolge Uhrzeit, Wochentag, Datum mit gleichen
    Abständen; quer steht der Wochentag vor dem Datum.
  - Größe bei eingebauten Schriften 50–200 %, bei Designer-Schriften (VLW) 8–120 px (bisher zwei Zahlenfelder,
    nur VLW). Eine VLW-Größe geht erst beim Loslassen an die Uhr, weil jede Größe eine eigene Datei ist. Der
    Wochentag bekommt eine eigene VLW-Datei mit Buchstaben (`stripfont_<Name>_<Größe>_wd.vlw`); fehlt sie, nimmt
    er DejaVu.
  - Neues Datumsformat T.MM.JJ.
  - Der Streifen bleibt leer, bis eine aktuelle Zeit vorliegt (NTP, RTC, DCF77, offenes WLAN oder
    Rocrail-Modellzeit) – vorher stand dort „0:00“.
- Sekundenfeld: Der Drehpunkt des Sekundenzeigers lässt sich im Zifferblatt-Designer verschieben (Bereich
  „Nabe und Sekundenzeiger“, „Mitte“ oder X/Y), z. B. ins untere Drittel. Der Sekundenzeiger liegt dann unter
  Stunden- und Minutenzeiger; die Zeigergrafik bleibt, einen passend kurzen Sekundenzeiger mit eigener Scheibe
  zeichnet man im Zeiger-Designer. Der Drehpunkt gehört zum Zifferblatt (`facecfg_<Name>.txt`, mit Löschen,
  Umbenennen und Sicherung, von einem anderen Displaytyp umgerechnet) und gilt auch in allen Vorschauen.
  Das Startpaket enthält dafür `face_subdial.bmp` (12, 3, 9, Sekundenfeld statt der 6), einen Zeigersatz wie
  Satz 0 mit kurzem rotem Sekundenzeiger samt Scheibe und das Uhren Set „Sekundenfeld“.
- Die von der Firmware erzeugten Zifferblätter (Startpaket) und Zeigersätze (0, 1, 2 und der Sekundenfeld-Satz)
  lassen sich weder löschen noch umbenennen noch überschreiben – in der Weboberfläche, beim Hochladen, im
  Designer und beim Wiederherstellen. Sie stehen in den Übersichten „Zifferblätter“ und „Zeigersätze“ zuerst und
  tragen den Hinweis „von der Firmware“ statt „Umbenennen“ und „Löschen“. Im Designer ist „Aktuelles … speichern
  und anwenden“ für sie gesperrt, nur „Als neues … speichern“ geht, und deren Namen sind nicht wählbar. Sie
  gehören nicht in die Sicherung; die Uhr erzeugt sie bei Bedarf selbst. Die Werksreset-Aktionen „Zifferblätter
  löschen“ und „Zeigersätze löschen“ lassen sie stehen.
- Startpaket: Fehlende Zifferblätter, Zeigersätze und Uhren Sets des Startpakets erzeugt die Uhr bei jedem Start
  neu, nicht mehr nur ohne jedes Zifferblatt und jeden Zeigersatz – so bekommen auch bestehende Uhren das
  Sekundenfeld. Vorhandenes bleibt; den Sekundenfeld-Satz legt die Uhr unter der ersten freien Nummer ab 3 an.
- Bis eine Zeit vorliegt, stehen Stunden-, Minuten- und Sekundenzeiger auf 12 (bisher liefen sie ab 10:10:30).
  Kommt die Zeit, laufen sie zur Uhrzeit.
- Helligkeit mit PWM (Hintergrundbeleuchtung): Solange die Uhr beim Start auf Daten wartet (noch keine Zeit,
  WPS, Access Point), leuchtet sie mit 50 % statt voll.
- Die Gamma-Korrektur (Eingabe und Kurve) erscheint nur noch mit Lichtsensor - ohne ihn wirkt sie nicht.
- ESP32-C6: Liegt die maximale Helligkeit über 128, warnt die Helligkeitsseite vor Überhitzung der
  Beleuchtung (Hinweis von Waveshare), schon beim Eintippen.
- Der Streifen gehört zum Zifferblatt: Neben der Grafik (`strip_<Name>.bmp`) hat jedes Zifferblatt seine
  Streifen-Einstellungen in `stripcfg_<Name>.txt` (Lage, Farben, Schrift, Größen, Formate, Positionen,
  ein/aus). „Streifen speichern“ legt sie zum aktiven Zifferblatt ab, „Speichern“ im Designer zum
  gespeicherten. Ein Zifferblatt ohne eigene Einstellungen bekommt den Standard (weiß, schwarze Schrift
  FreeSans Bold, Uhrzeit und Datum automatisch); „Standard“ im Designer setzt dieselben Werte.
  - Löschen und Umbenennen eines Zifferblatts nehmen Grafik und Einstellungen mit. Beim Start löscht die Uhr
    Streifen ohne Zifferblatt.
  - Die Sicherung enthält die Einstellungen. Von einem anderen Displaytyp mit Streifen rechnet die Uhr sie um:
    Positionen im Verhältnis von Breite bzw. Höhe, Größen mit dem kleineren Verhältnis, VLW-Größen auf die
    nächste vorhandene Datei. Die Grafiken lässt sie dabei aus (anderes Seitenverhältnis). Displays ohne
    Streifen übernehmen keine Streifen-Dateien, auch keine Schriften.
  - Uhren Sets speichern keine Streifen-Werte mehr, sie kommen über das Zifferblatt. Beim ersten Start
    übernimmt die Uhr die bisherigen Einstellungen einmal zum aktiven Zifferblatt und die Streifen-Werte
    vorhandener Uhren Sets zu deren Zifferblättern.

### Geändert
- WLAN-Verbindung beim Start: Statt des drehenden Zeichens (/ - \ -) zeigt die Uhr ein animiertes WLAN-Symbol (Punkt und drei Bögen, die nacheinander aufleuchten).
- Zeigersatz „Sekundenfeld“ (erzeugt, meist Satz 3): Der Sekundenzeiger ist filigraner (halbe Nadelbreite, Scheibe am Drehpunkt nur halb so groß).
  Die Uhr legt seine Datei einmalig neu an; eigene Zeigersätze bleiben unberührt.
- Logging ist ab Werk aus und schaltet sich nach 24 Stunden oder nach 9 Logdateien selbst ab (die Logdateien bleiben bei der
  24-Stunden-Abschaltung erhalten). Es wird auch nach einem Firmware-Update nicht mehr von selbst eingeschaltet.
- Zifferblätter und Zeiger werden beim Vergrößern (z. B. eine Sicherung mit 240er-Zifferblättern auf einem größeren Display
  oder ein kleineres hochgeladenes Bild) weich interpoliert statt in Pixelblöcken; bei Zeigern ohne Saum zur Transparenzfarbe.
- Helligkeitsseite: Das Kästchen „Hintergrundbeleuchtung regeln“ heißt jetzt „Hintergrundbeleuchtung PWM“.
- Zifferblatt-Designer: Der Knopf „Speichern“ im Kasten „Nabe und Sekundenzeiger“ entfällt. Das Speichern oben legt
  Bild, Nabe, Drehpunkte und Streifen zusammen ab; bei den von der Firmware erzeugten Zifferblättern speichert
  „Aktuelles Zifferblatt speichern“ jetzt Nabe, Drehpunkte und Streifen (das Bild bleibt).
- Der Drehpunkt des Sekundenzeigers im Sekundenfeld steckt jetzt in der Zifferblatt-Datei selbst (8 Byte hinter den
  Bilddaten) statt in einer eigenen Datei `facecfg_<Name>.txt`. Ältere Firmware liest solche Dateien weiter. Vorhandene
  `facecfg_*.txt` werden beim Start einmalig ins Zifferblatt übernommen und gelöscht, auch nach dem Wiederherstellen einer
  älteren Sicherung; beim Umrechnen einer Sicherung auf einen anderen Displaytyp wird der Drehpunkt mit skaliert. Ein
  neu hochgeladenes Zifferblatt hat wieder die Mitte als Drehpunkt.
- Lagesensor (ESP32-S3): Liegt die Uhr flach, beginnt die Zeitmessung für die stabile Lage von vorn; ein einzelner
  Ausreißer nach langem Liegen dreht das Display nicht mehr sofort. Die Seite `/api/imu` zeigt Beschleunigung, Lage und
  Rotation zur Diagnose.
- ESP32-S3 (Waveshare ESP32-S3-LCD-1.28): Der SPI-Takt zum Display steigt von 40 auf 80 MHz. Die Startanimation
  läuft damit etwas flüssiger (in einer Messung 55 statt 47 Bilder in 3 s); die übrigen Chips bleiben bei 40 MHz.
- Der ESP32-S2-Build ist kleiner: Mit „Core Debug Level: None“ (Visual Micro) bzw. `DebugLevel=none` sind es 56 KB
  weniger, dazu liest das Datum der offenen WLANs ohne `sscanf`. Die App belegt statt 98,7 % nur noch etwa 95 % des
  2-MB-Platzes. Beim Selbstbauen mit Visual Micro die Option für den S2 auf „None“ stellen.
- Solange die Zeit nach dem Start noch nicht bestätigt ist (Zeiger auf 12), steht ein kleiner Hinweis
  „warte auf Zeit“ bzw. „waiting for time“ im unteren Teil des Zifferblatts.
- Hochgeladene Zifferblätter und Streifengrafiken werden beim Skalieren gemittelt (jedes Zielpixel ist der Durchschnitt
  der Quellpixel, die es überdeckt) statt nur ein Quellpixel zu übernehmen: Große Bilder (z. B. 480 × 480) werden
  ohne Aliasing verkleinert, einfarbige Flächen bleiben exakt gleich. Gilt für alle Chips, auch ohne PSRAM. Zeiger
  skalieren wie bisher (gemittelte Ränder würden einen Saum zur Transparenzfarbe ergeben).
- `flashESP.bat`/`flashESP.sh` laden den Build des erkannten Chips beim Start vom GitHub-Release
  (`uhr4-esp32s2.zip`, `-esp32c6.zip`, `-esp32s3.zip`, je ca. 1,3 MB, mit SHA-256-Prüfung vor dem Flashen) statt ihn
  aus dem ZIP zu nehmen. Das Flash-ZIP enthält nur noch die Werkzeuge (7 statt 15 MB). Der PC braucht dafür Internet;
  einen Rückfall auf lokale Dateien gibt es nicht, bei einem Fehler wird nichts geflasht. Für die Entwicklung flasht
  `clocksetup.ps1 -Local` bzw. `UHR4_LOCAL=1` die Ordner `esp32s2`/`esp32c6`/`esp32s3`. Die Pakete erzeugt
  `build_uhr4/publish_builds.ps1`.
- Im Dateimanager stehen die Dateien des Startpakets (Zifferblätter und Zeigersätze der Firmware) in ihrer Gruppe
  vorn und lassen sich wie auf den Seiten „Zifferblatt“ und „Zeiger“ weder löschen noch umbenennen noch skalieren;
  „Alle auswählen“ lässt sie aus.
- Nach einem Neustart (OTA, Browser, WPS) bleiben die Zeiger bis zu 15 s auf 12, solange nur die vom Chip behaltene
  Systemzeit da ist (sie kann falsch sein). Sie fahren dann einmal zur bestätigten Zeit (NTP, RTC, DCF77, Rocrail
  oder von Hand gesetzt), statt erst zur falschen und mitten in der Fahrt zur richtigen.
- Während die Zeiger auf ihre Position fahren (Start, Zeitkorrektur, ca. 3 s), ruht die Hintergrundarbeit: kein
  Schreiben des Logs in den Flash, kein Webserver, keine NTP-Antworten, kein Rocrail. Das Log nennt danach Dauer,
  Bildzahl und die längste Pause zwischen zwei Bildern.
- Beim Löschen eines Zifferblatts oder Zeigersatzes erscheint oben ein Fortschrittsbalken („Wird gelöscht - bitte
  warten“), bis die Seite neu geladen ist.
- Zeiger laufen auf ihre Position (Start, Zeitkorrektur) ohne Ruckler in der Mitte: Sprünge über 10° gehen
  unverändert an die 3-Sekunden-Fahrt, statt vorher schrittweise geglättet zu werden und die Fahrt neu zu starten.
- Im Log stehen Adresse und Zeitquelle jetzt je in einer Zeile (I2C-Suche, Zeitsynchronisation).
- Auf der Seite „Presets“ entfallen „Presets als Datei speichern“ und „Presets aus Datei laden“ (samt Routen
  `/exportpresets` und `/importpresets`); Presets sichert die Seite „Sicherung“ mit.
- Preset anklicken lädt die Seite „Presets“ nicht mehr neu: Das Preset wird angewendet, oben erscheint die Meldung
  „Preset angewendet“ (wie bei Zifferblättern und Zeigersätzen).
- Der Chip (S2, S3, C6) steht jetzt neben dem Displaytyp in der Kopfzeile der Weboberfläche („S3 GC9A01_WITH_BACKLIGHT“),
  im Dateinamen der Sicherung (`uhr4-backup-<Hostname>-<Chip>-<Displaytyp>-<Datum>.tar`) und im Kopf der mitgesicherten
  Statusdatei.
- flashESP.bat: Nach dem Flashen und dem Senden von WLAN und Uhrzeit bleibt ein serieller Monitor offen. Er zeigt die
  Ausgabe der Uhr, verbindet sich nach einem Neustart selbst wieder und endet mit Q oder Esc. Die Uhr schreibt nur,
  wenn „Logging aktivieren“ an ist. Ohne Monitor: `clocksetup.ps1 -Flash -NoMonitor`.
- Zifferblatt- und Zeigersatz-Seite: Ein Klick auf ein Zifferblatt oder einen Zeigersatz wählt es aus, ohne die Seite
  neu zu laden; nur der Hinweis „(aktiv)“ wandert, die Meldung erscheint oben. Die Vorschaubilder tragen die
  Dateiversion in der Adresse und bleiben im Browser zwischengespeichert (ändert sich eine Datei, ändert sich die
  Adresse). Die Seiten laden dadurch auch beim erneuten Öffnen deutlich schneller. Schlägt der Aufruf fehl, folgt der
  Browser dem Link wie bisher. Der Streifen der Zifferblatt-Vorschau (zeigt Uhrzeit/Datum) wird nicht zwischengespeichert.
- Zifferblatt-Designer: Die Nabenfarbe hat kein eigenes Farbfeld mehr. Sie kommt aus dem Werkzeugkasten (Pipette,
  Palette, Farbfeld): Farbe wählen und „Aktuelle Farbe“ im Bereich „Nabe und Sekundenzeiger“ drücken; daneben zeigt
  ein Farbfeld die Nabenfarbe.
- Alle Zeigerdateien speichert die Uhr beim Start (und nach dem Wiederherstellen) im neuen Format des Displays.
  Zeiger im alten Format (schmaler und/oder kürzer) füllt sie transparent auf wie beim Laden; die Anzeige bleibt
  gleich, die Übersicht zeigt „neues Format“. Dateien anderer Größe bleiben unberührt.
- Die Zeigersätze nummeriert die Uhr beim Start fortlaufend neu: 0, 1 und 2 bleiben, der Sekundenfeld-Satz wird 3,
  die übrigen folgen in ihrer bisherigen Reihenfolge ab 4. Lücken von gelöschten Sätzen oder einer
  Wiederherstellung schließen sich. Uhren Sets, der aktive Zeigersatz und die gemerkte Nummer des
  Sekundenfeld-Satzes ziehen mit. Fällt beim Umnummerieren der Strom aus, setzt die Uhr beim nächsten Start fort.
- Jeder Zeigersatz hat jetzt alle drei Zeiger: Fehlt einem ein Zeiger, kopiert die Uhr ihn aus Satz 0 – beim Start,
  in der Zeigersatz-Übersicht, beim Aktivieren und nach dem Wiederherstellen. Bisher zeigte sie den fehlenden
  Zeiger aus Satz 0 nur ersatzweise an und folgte damit späteren Änderungen von Satz 0; jetzt ist die Kopie ein
  eigener Zeiger des Satzes.
- ESP32-C6: eigene Partitionstabelle `uhr4_c6` mit 2,25 MB Programm (bisher 2 MB) und 1,625 MB Dateisystem (bisher
  1,875 MB) – das Programm passte nicht mehr in 2 MB. Beim Selbstbauen `boards.local.txt` und `uhr4_c6.csv` in den
  Core-Ordner kopieren und das Schema „uhr4 C6 (2.25MB APP/1.6MB SPIFFS)“ wählen; mit „No OTA“ bricht der Build
  mit einer Meldung ab. **Beim ersten Flashen einer vorhandenen C6 vorher eine Sicherung ziehen:** Das Dateisystem
  liegt an anderer Stelle und wird neu angelegt, Zifferblätter, Zeiger, Streifen und Schriften sind danach leer;
  Einstellungen, WLAN und Uhren Sets bleiben. Danach die Sicherung wiederherstellen.
- Wiederherstellen einer Sicherung löscht nichts mehr: Vorhandene Zifferblätter, Zeigersätze, Streifen,
  Schriften und Uhren Sets bleiben, gleichnamige aus der Sicherung ersetzen sie. Uhren Sets der Sicherung mit
  neuem Namen kommen in freie Plätze. Die übrigen Einstellungen übernimmt die Uhr wie bisher aus der
  Sicherung. Zeigersätze der Sicherung mit einer Nummer, die die Firmware belegt (0–2, Sekundenfeld-Satz –
  etwa in einer älteren Sicherung), bekommen die nächste freie Nummer ab 3, und die Uhren Sets sowie der aktive
  Zeigersatz der Sicherung ziehen mit. Sind alle Zeiger pixelgleich mit dem Satz der Firmware (die erzeugten
  Sätze), entfallen sie und die Uhren Sets zeigen auf den Satz der Firmware.
- Jeder Start beginnt ohne Uhrzeit: Die Systemzeit überstand bisher einen Neustart per Reset-Taste oder
  Software und wird jetzt am Anfang verworfen. Die Uhr holt sie wie nach einem Stromausfall neu (NTP, RTC,
  DCF77, offenes WLAN, USB).
- Displayauswahl (Tab „Uhr Einstellungen“): ILI9341 und ST7789 172 × 320 heißen „mit Streifen für Uhrzeit und
  Datum“ statt „mit Uhrzeit und Datum unter der Uhr“ – der Streifen kann auch über der Uhr liegen.
- Taster (oder Boot-Taster) bekommt eine Stufe für das WLAN: ab 10 s läuft ein gelber Countdown „WiFi Reset“
  über 10 s (Loslassen bricht ab), ab 20 s ein roter Countdown „Factory Reset“ über 10 s. Wer in dieser Zeit
  loslässt, löscht alle gespeicherten WLANs und startet neu; der vollständige Werksreset kommt erst nach
  30 s (bisher 15 s). Jede Stufe steht im Log.

### Entfernt
- „Weitere Zifferblätter von GitHub laden“ (Zifferblatt-Seite) und „Weitere Zeigersätze von GitHub laden“ sowie
  das Formular „Neuen Zeigersatz hochladen“ (Zeigersatz-Seite) entfallen. Zifferblätter und Zeiger entstehen in den
  Designern oder kommen über den Dateimanager und die Sicherung auf die Uhr; das Hochladen-Formular der
  Zifferblatt-Seite bleibt.

### Behoben
- Zeiger-Designer: Die Zeiger des aktiven Designs werden nacheinander geladen (je bis zu dreimal versucht) statt parallel.
  Zuvor konnte eine abgebrochene Anfrage einen Zeiger leer lassen, der dann als leerer Zeiger in ein neues Design gespeichert
  wurde. Konnte ein Zeiger nicht geladen werden, zeigt der Designer das an und speichert nicht.
- Uhrzeit aus offenen WLANs auf dem ESP32-S3: Die Uhr verbindet sich mit den offenen Hotspots mit voller Sendeleistung (sie sind meist
  weit weg), danach gilt wieder die kleinste Stufe. Neuer Diagnosebefehl `UHR4 OPENWIFI` (USB): sucht die Zeit gezielt aus
  einem offenen WLAN (auch ab -95 dBm), jeder Schritt steht im Log, danach Neustart.
- ESP32-S3-Build: Bricht jetzt mit einer Meldung ab, wenn PSRAM nicht auf „QSPI PSRAM“ steht. Der Chip hat 2 MB PSRAM;
  ohne sie blieben nur etwa 30 KB freier Heap (größter Block 8 KB), und Seiten der Weboberfläche liefen in die
  Zeitüberschreitung.
- Die Uhr wählt die WLAN-Sendeleistung selbst (`WIFI_TX_AUTO` in `config.h`). Mit voller Leistung (20 dBm) antworteten
  Router und Hotspots auf die Anmeldung der ESP32-S3 nicht (Abbruchgrund „2 AUTH_EXPIRE“). Die S3 beginnt bei 8,5 dBm
  und geht bei Misserfolg über 13 und 17 auf 19,5 dBm hoch; S2 und C6 beginnen mit der vollen Standardleistung und gehen
  nur bei Misserfolg herunter (17, 13, 8,5 dBm). Je Stufe ein Versuch nur mit WPA2, zuletzt einer mit WPA3. Bei einem
  neuen WLAN prüft die Uhr nach dem Verbinden das Gateway (TCP- und DNS-Antwort): zu hohe Leistung lässt kurze
  Verwaltungspakete durch, Daten aber nicht. Besteht die Prüfung auf keiner Stufe, gilt die erste, auf der sie
  verbunden war. Die Stufe, die klappt, merkt sie sich je WLAN, ebenso, ob das Netz WPA3 braucht (dann beginnt sie dort gleich mit WPA3), und
  nimmt sie beim nächsten Start zuerst (ohne Prüfung); sie steht nicht in der Sicherung. Nach einem Misserfolg gilt wieder die Startstufe (Access Point, offene
  WLANs). Die Statusseite zeigt die Sendeleistung.
- WLAN-Log: Die Zeile zum Verbindungsversuch nennt die Länge des Passworts (nicht das Passwort) und die Sendeleistung.
- Uhrzeit aus offenen WLANs: Die TLS-Verbindung zur Anmeldeseite eines Hotspots bekommt 10 s statt 4 s, und schlägt sie
  fehl, nennt das Log den mbedTLS-Fehler. Hotspots wie „Vodafone Hotspot“ leiten auf eine Anmeldeseite um, deren
  Antwort ohne `Date:`-Kopf kommt; die Uhr braucht dann die HTTPS-Antwort der Anmeldeseite.
- WLAN: Der erste Verbindungsversuch läuft nur mit WPA2 (ohne geschützte Verwaltungsframes, 15 s), erst der zweite
  mit WPA3. Router im Mischbetrieb WPA2/WPA3 antworteten auf die WPA3-Anmeldung der ESP32-S3 nicht (Abbruchgrund
  „2 AUTH_EXPIRE“). Ein Router nur mit WPA3 verbindet damit erst im zweiten Versuch.
- WLAN: Die Uhr durchsucht beim Verbinden alle Kanäle und nimmt den stärksten Access Point mit dem gespeicherten
  Namen. Bisher nahm sie den ersten gefundenen; bei mehreren Access Points mit gleichem Namen (Repeater, Mesh) konnte
  das der schwache sein und die Verbindung mit „2 AUTH_EXPIRE“ scheitern. Das Log nennt bei Erfolg Kanal, BSSID und
  Signalstärke.
- WLAN: Schlägt eine Verbindung fehl, nennt das Log jetzt den Grund des Abbruchs (z. B. „202 AUTH_FAIL“ bei falschem
  Passwort, „201 NO_AP_FOUND“, „203 ASSOC_FAIL“ wenn der Router das Gerät ablehnt).
- Der Hostname aus der MAC-Adresse (`clock_XXXXXX`) war auf dem ESP32-S3 `clock_000000`, weil die Adresse zu früh
  gelesen wurde. Die Uhr liest sie jetzt direkt aus dem Chip, unabhängig vom WLAN-Zustand. Ein bereits gespeicherter
  eigener Hostname bleibt unverändert.
- Meldungen auf dem Display (WPS-Hinweis „then Access Point“, Access-Point-Anzeige mit „http://…“): Passte ein Text
  in keine der größeren Schriften, nahm die Uhr die winzige Notschrift (6 × 8 Pixel). Jetzt verkleinert sie die
  letzte größere Schrift bis auf 75 % und vergrößert erst danach die Notschrift (bis 3-fach). Das betrifft vor allem
  runde Displays, wo die untere Zeile schmal ist.
- Taster und Boot-Taste reagierten nicht, solange die Uhr auf eine WLAN-Verbindung wartete (je 30 s, beim
  Start und bei jedem Reconnect), offene WLANs abfragte, im WPS-Countdown stand oder auf DCF77 wartete. Sie
  werden jetzt auch dort abgefragt. Außerdem versuchte die Uhr direkt nach einem gescheiterten Start sofort
  noch einmal 60 s lang, sich zu verbinden; der erste Reconnect-Versuch kommt jetzt nach 5 Minuten, danach
  stündlich. Neuer USB-Befehl `UHR4 PINS` (Diagnose: Pegel von Taster und Boot-Taste).
- Access-Point-Modus nach dem Start (gespeichertes WLAN nicht erreichbar): 2 Minuten nach Beginn schaltete ein
  Verbindungsversuch den AP ab – scheiterte er, war die Uhr bis zum nächsten stündlichen Versuch weder im
  WLAN noch per AP erreichbar. Jetzt prüft die Uhr auch im AP-Modus nur stündlich (mit „WLAN neu
  verbinden“) und startet den AP nach einem gescheiterten Versuch sofort wieder; klappt die Verbindung,
  beendet sie den AP-Modus. Im laufenden Betrieb geht die Uhr weiterhin nie in den AP-Modus.

## 2026-10-07

### Geändert
- Die Nabe (Mittelpunkt über den Zeigern) wird im Zifferblatt-Designer eingestellt: Radius und Farbe
  (Farbwähler) mit Live-Vorschau. Die Uhr zeigt Änderungen sofort, „Nabe speichern“ legt sie ab. Das Formular
  „Mittelpunkt“ auf der Seite „Zeiger“ entfällt.

## 2026-10-06

### Hinzugefügt
- Knopf „Uhrzeit übernehmen“: Hat die Uhr keine Uhrzeit (kein WLAN/NTP, keine RTC, kein DCF77, z. B. im
  Access-Point-Modus), zeigt ihn die Statusleiste. Er stellt die Uhr auf die Zeit des Handys bzw. PCs, eine
  vorhandene RTC gleich mit. Derselbe Knopf steht im Tab „NTP Zeitzone“.

### Geändert
- GitHub-Repository umbenannt in `holgiw/ESP32-Station-Clock` (vorher `TFT-Clock-GC9A01`; der Name passte nicht
  mehr zu den verschiedenen Displays und Boards). GitHub leitet alte Links weiter, Firmware und Doku verweisen
  auf den neuen Namen.
- Startpaket einer neuen Uhr:
  - `face_default.bmp` hat die Ziffern 12, 3, 6 und 9.
  - Ohne Zifferblätter und Zeigersätze erzeugt die Uhr dazu `face_numbers.bmp` (1–12), `face_roman.bmp`
    (I–XII), die Zeigersätze 1 (Balken, schwarzer Sekundenzeiger) und 2 (geschwungen) und, wenn keine Presets
    da sind, die Presets „Standard“, „1-12“ und „I-XII“ (ohne Sekundenzeiger).
  - Die Werksreset-Aktionen für Zifferblätter, Zeigersätze und Presets stellen diesen Zustand wieder her.
  - Passen die erzeugten Dateien nach einem Wechsel des Displaytyps nicht zur Uhrgröße, erzeugt die Uhr sie
    beim Start neu.
  - Displays mit Streifen (ILI9341, ST7789 172 × 320): Die drei Presets haben einen weißen Streifen mit
    schwarzer Schrift in FreeSans Bold, DejaVu bzw. der Designer-Schrift „Sans“ (falls auf der Uhr, sonst
    GLCD). Eine neue Uhr startet mit dem Streifen von „Standard“.
- Sicherung von einem anderen Displaytyp wiederherstellen: Die Uhr weist darauf hin und übernimmt nur
  Zifferblätter, Zeiger (auf ihre Größe skaliert), Uhren Sets und allgemeine Einstellungen. Die Nabengröße
  (auch in den Uhren Sets) rechnet sie um, die Nabenfarbe kommt mit. Displaytyp, Rotation,
  Hintergrundbeleuchtung und Helligkeit bleiben. Sind die Zifferblätter der Sicherung nicht 240 × 240 groß,
  übernimmt sie nichts und meldet das. Hat die Sicherung keinen Streifen, bekommt eine Uhr mit Streifen den
  Standard-Streifen (weiß, schwarze Schrift, FreeSans Bold). Bisher lehnte die Uhr eine andere Uhrgröße ab und
  übernahm bei gleicher Größe (z. B. GC9A01 und ILI9341) alles.
- Sicherung: Der Displaytyp steht immer darin, auch wenn er nie gespeichert wurde (Werkseinstellung). Sonst
  nahm die Zieluhr beim Wiederherstellen ihre eigene Werkseinstellung an. Ältere Sicherungen ohne diesen
  Eintrag gelten als GC9A01 (Werkseinstellung des ESP32-S2).
- Sicherung: Der Dateiname enthält die Uhrzeit mit Sekunden (z. B.
  `uhr4-backup-clock_23F6A4-ST7789-20261006-175812.tar`), damit sich mehrere Sicherungen eines Tages nicht
  überschreiben.
- `flashESP.sh` (Linux) kennt den ESP32-C6: erkennt ihn am USB-Port (303a:1001), bietet ST7789 und ST7789_240
  an und flasht den Build aus `esp32c6` mit `boot_app0.bin`, ohne Download-Modus.
- ST7789 172 × 320: Start- und Statusmeldungen stehen quer (90° gedreht) in doppelter Schriftgröße.
- Hochladen:
  - Fehlt im Dateinamen `face_` (Zifferblatt) oder `hand_set` (Zeiger), ergänzt die Uhr es, z. B.
    `bahnhof.bmp` → `face_bahnhof.bmp`, `set3_hour.bmp` oder `3_hour.bmp` → `hand_set3_hour.bmp`. `.BMP` wird
    klein geschrieben.
  - Die Hinweise unter den Upload-Formularen nennen, was die Uhr annimmt: BMP mit 16, 24 oder 32 Bit,
    höchstens so groß wie der freie Speicher (angezeigt). Die Uhr skaliert und wandelt in RGB565 um.
- Presets speichern, ob die Rocrail-Modellzeit an ist und ob die Uhr das WLAN neu verbindet (`rocrail=` und
  `wifiReconnect=true/false` in `/api/setMode`). Serveradressen und WLAN-Daten bleiben Einstellungen der Uhr.
- Sichern und Wiederherstellen zeigen einen Fortschrittsbalken. Solange die Uhr beschäftigt ist, pausiert die
  Statusleiste, statt „Verbindung verloren“ zu melden. Nach dem Wiederherstellen wartet die Seite, bis die Uhr
  nach dem Neustart wieder antwortet.

### Entfernt
- Knopf „Presets von GitHub laden“ auf der Seite Uhren Sets, die Nachfrage bei leerer Liste und die nur
  dafür genutzte Route `/importpresetsmerge`.
- Link auf die ZIP-Dateien (`faces_handsets_240.zip` / `_160.zip`) auf der Seite Zifferblätter.

### Behoben
- Hochladen: 16-Bit-BMPs im Format RGB555 haben die richtigen Farben. Nicht unterstützte BMPs (Palette,
  8/4/1 Bit, komprimiert) lehnt die Uhr ab, statt sie als schwarzes Bild zu speichern; die abgelehnte Datei
  bleibt nicht liegen.
- Zeigersätze hochladen: Ein dort hochgeladenes Zifferblatt (`face_…`) landete trotz Fehlermeldung auf der
  Uhr. Die Zeiger-Seite nimmt nur noch Zeiger an.
- Wiederherstellen: Die Warnung bei anderem Displaytyp im Dateinamen der Sicherung greift (das Datum mit
  Bindestrich wurde nicht erkannt) und kennt die ST7789-Typen.
- ESP32-C6: Nach einem Neustart durch die Software (Wiederherstellen, Neustart-Knopf, flashESP) blieb das
  Display schwarz, nur Strom aus/an half. CS liegt nicht mehr dauerhaft auf LOW, LovyanGFX schaltet es je
  Übertragung.
- Uhren Sets: Vorschaubilder fehlten oft. Der Browser forderte alle gleichzeitig an, und der Uhr (vor allem
  ohne PSRAM) ging der Speicher aus. Die Seite lädt sie nacheinander mit bis zu drei Versuchen; die Uhr sendet
  jedes Bild zeilenweise statt als zweite Kopie im RAM.
- Zifferblatt-Designer (Displays mit Streifen): Füllen bleibt im angeklickten Bereich, Zifferblatt oder
  Streifen. Bisher lief z. B. der Rand um ein rundes Zifferblatt in den Streifen weiter und färbte ihn mit.
- ESP32-C6 (ohne PSRAM): Die Streifen-Grafik eines Zifferblatts (`strip_….bmp`) fehlte, weil für ihre Kopie im
  RAM der Speicher nicht reichte; der Streifen blieb einfarbig. Die Uhr liest sie zeilenweise aus der Datei,
  auf dem Display und in den Vorschauen.

## 2026-10-05

### Hinzugefügt
- Erste Unterstützung für Waveshare ESP32-C6-LCD-1.47 (ST7789, 172 × 320) und ESP32-C6-LCD-1.3 (ST7789,
  240 × 240) als eigener Build:
  - Displaytypen ST7789 (Uhr 172 × 172 mit Streifen für Uhrzeit und Datum) und ST7789_240.
  - Pinbelegung in `config.h` (`BOARD_WAVESHARE_C6_ST7789`, am Chip erkannt; beide Boards gleich belegt), kein
    zweites Display.
  - Zifferblätter von GitHub kommen aus dem 240er-Ordner und werden beim Hochladen verkleinert.
  - Der Linker-Schalter `-mtext-section-literals` gilt nur für den ESP32-S2 (`board.txt`,
    `platform.local.txt`).
  - Ohne PSRAM hält die Uhr das Zifferblatt nur komprimiert (oder liest es aus der Datei) und zeichnet jedes
    Bild daraus neu. Hochladen skaliert zeilenweise ohne Vollbild im RAM; Vorschauen (Uhren Sets,
    Vorschau-Seite) verkleinern bzw. streamen zeilenweise.
  - `flashESP.bat` erkennt den C6 am USB-Port und flasht den Build aus `build_uhr4\esp32c6`.
  - Der Build braucht „USB CDC On Boot: Enabled“, sonst bricht er mit einer Meldung ab.

## 2026-10-04

### Behoben
- README: Hinweis auf die englische Fassung direkt unter dem Titel; den Verweis auf das nicht mehr vorhandene
  Release „Version 3“ gibt es nicht mehr.

## 2026-10-02

### Geändert
- Sicherung: Zifferblätter, Zeiger und Streifen-Grafiken liegen darin als normale BMP-Bilder (vorher im
  komprimierten Format der Uhr) und öffnen in jedem Bildprogramm. Beim Wiederherstellen packt die Uhr sie wie
  beim Hochladen; ältere Sicherungen lassen sich weiter einspielen.
- Screenshots (`screenshots/de`, `screenshots/en`) neu aufgenommen: die Seite DCF77 von einer Uhr mit
  Empfänger, wieder der Zifferblatt-Designer einer ILI9341-Uhr (`zifferblatt_designer_ili.png`) und neu deren
  Vorschau mit dem Uhrzeit-/Datumsstreifen (`vorschau_ili.png`).
- Auswahl der mitgelieferten Zifferblätter, Zeiger und Presets überarbeitet (`graphic/` mit
  `faces_handsets_160.zip` und `faces_handsets_240.zip`, `presets.txt`).

### Behoben
- Selbst gezeichnetes Standard-Zifferblatt: Auf runden Displays bleibt alles außerhalb des Kreises weiß wie bei
  hochgeladenen Zifferblättern. Es übersteht Sichern und Wiederherstellen Pixel für Pixel unverändert.
- `presets.txt`: Das Preset „Antik“ wird beim Herunterladen von GitHub wieder übernommen. Name und Adresse
  waren durch ein Leerzeichen statt eines Tabs getrennt, deshalb wurde die Zeile übersprungen.

---

# English Version

## 2026-10-12

### Added
- New target (EXPERIMENTAL): Waveshare ESP32-S3-Touch-LCD-4 (480x480, ST7701 on the RGB bus, GT911 touch, CH32V003 port expander). Own
  build (`esp32s3lcd4`, octal PSRAM). The image comes from its own buffer in PSRAM; after flash writes (firmware update,
  restoring a backup) the panel is re-synchronised. During a firmware update the display shows only a calm blue
  area (no text), during a backup restore briefly the text and then blue.
- New target (EXPERIMENTAL, NOT TESTED ON THE DEVICE): Adafruit Qualia ESP32-S3 with the 2.1" round display TL021WVC02 (480x480, CST826 touch,
  PCA9554A port expander). Own build (`esp32s3qualia`, option `-DUHR4_BOARD_QUALIA`), display type `TL021_480`; pins, timings and start-up
  sequence according to Adafruit's documents. Backlight only on/off (brightness by darkening the pixels); the buttons UP/DOWN on the expander
  count like the button. `buildAll.bat`, `flashESP`, `flashAll`, `checkClocks` and `publish_*.ps1` know the board; `flashESP` recognizes a
  factory-new Qualia (Adafruit id 239A) and explains how to enter download mode (hold BOOT0, press reset).
- 480x480: touching the display shows the page with WiFi and IP address for 5 seconds.
- 480x480: brightness is controlled via the PWM register of the port expander ("Backlight PWM", on by default); the test
  button in the brightness settings works as on the other clocks.
- 480x480: the buzzer is switched off at start-up and regularly during operation.
- Firmware update from GitHub: the 480x480 board downloads `uhr4-s3lcd4.bin` (the file `uhr4-s3.bin` belongs to the
  ESP32-S3-LCD-1.28).
- buildAll.bat builds four targets (s3, s2, c6, s3lcd4) and copies the build of the 480x480 board to
  `build_uhr4\esp32s3lcd4`.
- flashESP (Windows and Linux), flashAll and checkClocks know the 480x480 board: it is told apart from the ESP32-C6 (both
  report as USB serial JTAG) by the display type ST7701_480 or by esptool (chip_id), gets only its own build and is not
  asked for the display type. publish_builds.ps1 and publish_s3_update.ps1 also publish `uhr4-esp32s3lcd4` and
  `uhr4-s3lcd4`.

- Clock face designer: "Sharpen" button (unsharp mask) with adjustable strength (10 to 300 %); sharpens the whole clock face, can be applied repeatedly and undone with "Undo".
- Clock face designer: arrow buttons "Shift" move the whole clock face by 1 pixel (10 with Shift) left, right, up or down; edges that become free get the selected colour. Undo is possible.
- Clock face and hand set pages: the active clock face or hand set has a red frame; it follows the selection.
- When a clock face is renamed, "face_" (and ".bmp") is added automatically if it is missing in the new name; saving in the designer and uploading already did that.
- File manager: all files except those of the starter set can be renamed (before only .bmp). Fonts keep their prefix ("font_", "stripfont_"), if the extension is missing the old one stays.
- README: sources (examples, not a recommendation) for boards and displays; THIRD_PARTY_LICENSES extended by FreeSerif and the Qualia start-up sequence.
- Time, date and weekday can now also be shown on displays without a strip (round, 240x240, 480x480): in the clock face designer under
  "Time, date, weekday on the clock face" (same settings as the strip: font, colour, size, format, position; per clock face in
  `stripcfg_*.txt`). Off by default. The text lies behind the hands and is part of the clock face cache, without seconds and without a blinking
  colon; it changes once a minute (on the 480 board that costs a short pause of the second hand). Not available without PSRAM (ESP32-C6).
- New addresses for checking: `/api/faceimg` (clock face cache as it goes to the display) and `/api/frameimg` (last drawn frame with the hands), both RGB565.
- The "Preview" page and the preview images of the presets show time, date and weekday on the clock face (displays without a strip) if the clock face displays them; in the presets with the demo time 10:10:30.
- Waveshare ESP32-S3-Touch-LCD-4: the buzzer beeps for 500 ms at start (buzzer on expander output EXIO6); test address `/api/beeptest`.
- 480x480: the minute hand glides for 300 ms instead of 600 ms.
- Clock face designer: the tool "Display" sends the new position only when saving; success messages disappear after 4 s and are at most 420 px wide.
- Backup from another display type: the backup's clock faces must be at least 240 x 240 (before exactly 240 x 240).
- 480x480: second hand, hour hand, minute hand and new text are drawn and sent as separate small rectangles instead of one big one; rectangles that are no more expensive together than apart are merged.
- On a minute change only the band of rows with the text is copied and redrawn (24 instead of 100 ms), also as a partial frame with a rotated clock face.
- Date format "D" (day only); weekday written out or short (Mo / Mon). Both stored per clock face.
- Clock face designer: pipette next to the text colour; time, date and weekday also appear in the drawing area and can be moved there with the mouse using the tool "Display"; the preview (240 px wide) sits at the top of the right column and does not scroll away; both designers use the full screen width.
- On a minute change only the text in the clock face cache is redrawn (instead of recalculating the whole cache); for the display on the clock face the designer's preview transfers only the rows with text (100 KB instead of 920 KB), so the clock hardly stalls. Before, the second hand stayed at 0 for 2-3 s after every minute change while the designer was open.
- File system queries are cached (`fs_cache.h`: free space, existence of files, file list): pages such as "Hand Set" (before 1.7 s) and the hand designer (0.5 s) load in about 0.5 s and 0.1 s, the second hand stands still for a shorter time when a page is opened. After every write access the values are read again.
- 480x480: when saving a clock face or hand and when uploading, the display shows a calm blue area instead of flickering (until 1.5 s after the last write access).

### Changed
- Hand set "seconds subdial" (set 3): the hub of the second hand is halved once more.
- ESP32-S3-Touch-LCD-4: the second hand is held in double resolution and rotated with anti-aliasing at half size (coverage from 2 × 2 sub-pixels); scaled-up hands look less pixelated.
- `flashESP.bat` leaves the COM port free after flashing (no serial monitor any more); the monitor is available with
  `clocksetup.ps1 -Flash -Monitor`.
- A completely freshly flashed clock starts with the "dark" brightness (min. brightness) 75 instead of 100 or 5;
  updates do not change stored values.
- Backups no longer contain brightness values (min./max. brightness, thresholds, gamma, bright/dark times); when
  restoring, the clock keeps its own values, also from older backups.
  Presets no longer save or apply them either (the clock's own value applies); only "Automatic brightness" stays part
  of a preset.

## 2026-10-08

### Added
- "Log" page: the button "Download" (German: "Herunterladen") downloads the displayed (by default the current) log file.
- Every new log file starts with a header line: clock (uhr4), hostname, ESP32 chip (model, revision, Arduino core),
  display and compile date/time.
- Clock face designer: the hour and minute hand (together) can also be moved over the whole clock face ("Hour and
  minute hand pivot", X/Y or centre). The hub sits on this pivot. A second hand without a pivot of its own sits on that
  of the other hands. The tick "Second hand over the other hands" sets the position; without a choice of its own the
  rule applies: over the others if all share the same pivot, otherwise below (seconds subdial). Pivots and position are
  in the trailer of the clock face file (now 14 instead of 8 bytes, older files stay readable).
- Brightness page: next to "Min Brightness" and "Max Brightness" there is a "Test 10 s" button each. It shows the value
  in the field (even unsaved) on the clock for 10 seconds, then the normal brightness applies again.
- The checkbox "Get the time from open WiFis" is now in the tab "Clock Setup" (below "Reconnect WiFi").
- MQTT report (all chips, off by default, page "MQTT" in the navigation): the clock publishes its status as retained
  messages to an MQTT broker, e.g. that of openHAB - once after the start, once at midnight and when its IP address
  changes: `<base topic>/<host name>/` with `info` (JSON with `host`, `ip`, `chip`, `display`, `version`; openHAB can build a
  clock list with links from it), `ip`, `host`, `chip`, `display`, `version`, `time_ok`, `heap_kb`, `rssi` and
  `uptime_s` (no time of day). A slim client without a library (MQTT 3.1.1, QoS 0); the password is not part of the backup.
  If a report fails, the next attempt follows after 2, 4, 8 and then every 15 minutes. The status page shows the last
  result.
- `flashAll.bat` (project folder, calls `flashAll.ps1`) checks all clocks on the network and brings outdated ones to
  the state of the builds in `build_uhr4`: ESP32-S3 over WiFi (OTA), ESP32-S2 and -C6 (no OTA) only if they are
  connected via USB. It asks before every flash; `-CheckOnly` only checks, `-Yes` does not ask, `-Force` also
  re-flashes up-to-date S3. `checkClocks.ps1` got the switch `-PassThru` for it.
- `checkClocks.ps1` (project folder) finds all uhr4 clocks on the network and shows address, name, chip, display,
  running firmware, whether it matches the newest build in `build_uhr4`, whether the time is right and the free memory.
  With `-All` (also via `checkClocks.bat`) it first lists all devices on the network, also non-clocks (address, MAC,
  name, note such as page title or Espressif).
- If an update is available on GitHub, the "Backup" page shows a link to the changelog (newest entries on top):
  what is new since the installed version.
- Update from GitHub (ESP32-S3 only): on the "Backup" page → "Firmware from GitHub"
  "Check now" shows whether the latest release has a newer firmware (the clock never checks by itself; line
  "Firmware on GitHub" on the status page after a check). It is installed only by clicking "Install update"; the clock downloads
  `uhr4-s3.bin`, checks the certificate (built-in certificate bundle, so the time must be right), chip header and
  SHA-256 from `uhr4-s3.txt` and only then activates the firmware. `build_uhr4/publish_s3_update.ps1` puts the two
  files into the latest release.
- Separate build for the Waveshare ESP32-S3-LCD-1.28 (round GC9A01 built in, 16 MB flash, 2 MB PSRAM), folder
  `build_uhr4/esp32s3`; not yet tried on the device:
  - Board "ESP32S3 Dev Module", Flash Size 16MB, PSRAM "QSPI PSRAM", Partition Scheme "16M Flash (3MB APP/9.9MB
    FATFS)", USB CDC On Boot "Disabled" (USB runs via the CH343P converter). Wrong settings stop the build with
    a message.
  - Two app partitions, LittleFS with 9.9 MB on the "ffat" partition.
  - Firmware update over WiFi: "Backup" page → "Firmware Update" (upload `uhr4.ino.bin`, with a progress
    bar) or ArduinoOTA from Visual Micro/Arduino IDE (port 3232, without password). The clock checks header
    and chip of the file, rejects merged, bootloader and partition files and only starts the new firmware
    after a complete check. During the update the display shows the progress, the rest of the clock pauses,
    WiFi scans are skipped, radio power saving is off; if an update hangs, the clock restarts after 3
    minutes.
  - Backlight on GPIO 40, controlled by default. Photoresistor on GPIO 2/4/5, button 16, DCF77 17, RTC on the
    I2C 6/7 of the built-in motion sensor. The board has no LED, the "DCF77 Sync LED Blink" option is omitted.
  - Rotation "automatic" ("Clock Setup" tab, this board only): the QMI8658 motion sensor turns the clock
    face upright once a new position lasts 1.5 s; lying flat, the rotation stays. When switching it on, the
    clock takes the current position as the reference for the rotation set right now. The status page shows
    position and reference.
  - `flashESP.bat`/`flashESP.sh` recognize the board by the CH343P (USB id 1A86:55D3), do not ask for a display
    type and flash the build from `esp32s3`. If no uhr4 replies on the converter, they ask first.
- Time from open WiFis (all builds, "NTP Timezone" tab, on by default): if the clock connects to no stored
  WiFi at boot (out of range or no access, e.g. a wrong password), the clock without an RTC briefly connects
  to up to six open WiFis (from -85 dBm, each name once, 15 s each, 60 s in total at most) and gets the time: NTP, then the `Date:`
  header of an HTTP reply (first without DNS: gateway, 1.1.1.1; then by name), finally follows the redirect to
  the login page once, via HTTPS (TLS without certificate check) and HTTP. Then it disconnects again and, as
  with an RTC, goes into normal clock operation without WPS and access point; the stored WiFi stays and is
  retried hourly with "Reconnect WiFi". The log names every step: reason, network, IP, way and result. Costs
  about 100 KB of flash (TLS).
- Clock face designer, time/date strip: the settings are in three groups time, date and weekday. Each group
  has "show", a "Size" slider and the position (automatic or X/Y); plus time format, seconds and blinking for
  the time, the date format for the date.
  - New: weekday written out in the language of the web interface (German, otherwise English), off by default.
    In automatic mode the switched-on lines are ordered time, weekday, date with equal gaps; in landscape the
    weekday comes before the date.
  - Size for built-in fonts 50–200 %, for designer fonts (VLW) 8–120 px (previously two number fields, VLW
    only). A VLW size is only sent when the slider is released, since each size is a file of its own on the
    clock. The weekday gets its own VLW file with letters (`stripfont_<name>_<size>_wd.vlw`); without it, it
    uses DejaVu.
  - New date format D.MM.YY.
  - The strip stays empty until a current time is available (NTP, RTC, DCF77, open WiFi or Rocrail model
    time) – previously it showed "0:00".
- Seconds subdial: the second hand's pivot can be moved in the clock face designer ("Hub and second hand"
  section, "centre" or X/Y), e.g. into the lower third. The second hand then lies below the hour and minute
  hand; the hand graphic stays, a fittingly short second hand with its own disc is drawn in the hand designer.
  The pivot belongs to the clock face (`facecfg_<name>.txt`, with delete, rename and backup, converted from
  another display type) and also applies in all previews. The starter set contains `face_subdial.bmp` (12, 3, 9,
  subdial instead of the 6), a hand set like set 0 with a short red second hand with a disc and the preset
  "Sekundenfeld" for it.
- The clock faces (starter set) and hand sets (0, 1, 2 and the subdial set) created by the firmware can be
  neither deleted nor renamed nor overwritten – in the web interface, on upload, in the designer and on restore.
  They come first in the "Clock Faces" and "Hand Sets" overviews and show "built-in" instead of "Rename" and
  "Delete". In the designer "Save and apply current …" is locked for them, only "Save as new …" works, and their
  names cannot be chosen. They are not part of the backup; the clock creates them itself when needed. The
  factory reset actions "Delete Clock Faces" and "Delete Hand Sets" leave them in place.
- Starter set: the clock creates missing clock faces, hand sets and presets of the starter set at every start,
  no longer only without any clock face and hand set – so existing clocks get the subdial too. Existing items
  stay; the clock creates the subdial set under the first free number from 3.
- Until a time is available, hour, minute and second hands stand at 12 (previously they ran from 10:10:30).
  When the time arrives, they run to the time.
- Brightness with PWM (backlight): while the clock waits for data at boot (no time yet, WPS, access point), it
  shines at 50 % instead of full.
- Gamma correction (input and curve) only appears with a light sensor - without one it has no effect.
- ESP32-C6: if the maximum brightness is above 128, the brightness page warns of the backlight overheating
  (note from Waveshare), already while typing.
- The strip belongs to the clock face: besides the graphic (`strip_<name>.bmp`) each clock face has its strip
  settings in `stripcfg_<name>.txt` (placement, colours, font, sizes, formats, positions, on/off). "Save
  strip" stores them for the active clock face, "Save" in the designer for the saved one. A clock face without
  its own settings gets the default (white, black text FreeSans Bold, time and date automatic); "Default" in
  the designer sets the same values.
  - Deleting and renaming a clock face takes graphic and settings along. At start the clock deletes strips
    without a clock face.
  - The backup contains the settings. From another display type with a strip the clock converts them:
    positions by the ratio of width or height, sizes by the smaller ratio, VLW sizes to the nearest available
    file. It leaves out the graphics (other aspect ratio). Displays without a strip take over no strip files,
    no fonts either.
  - Presets no longer store strip values, they come with the clock face. At the first start the clock takes
    over the previous settings once for the active clock face and the strip values of existing presets for
    their clock faces.

### Changed
- WiFi connection at start: instead of the turning character (/ - \ -) the clock shows an animated WiFi symbol (dot and three arcs lighting up one after another).
- Hand set "seconds subdial" (generated, usually set 3): the second hand is more delicate (half the needle width,
  disc at the pivot only half the size). The clock creates its file once anew; own hand sets stay untouched.
- Logging is off by default and switches itself off after 24 hours or after 9 log files (the log files are kept on the
  24-hour switch-off). It is also no longer switched on by itself after a firmware update.
- Clock faces and hands are interpolated smoothly instead of in pixel blocks when enlarged (e.g. a backup with 240 clock
  faces on a larger display, or a small uploaded image); hands without a fringe towards the transparency colour.
- Brightness page: the German label of the backlight checkbox is now "Hintergrundbeleuchtung PWM".
- Clock face designer: the "Save" button in the box "Hub and second hand" is gone. Saving at the top stores image,
  hub, pivots and strip together; for the clock faces created by the firmware "Save current clock face" now stores hub,
  pivots and strip (the image stays).
- The pivot of the second hand in the seconds subdial is now part of the clock face file itself (8 bytes behind the
  image data) instead of a file of its own, `facecfg_<name>.txt`. Older firmware keeps reading such files. Existing
  `facecfg_*.txt` are moved into the clock face once at start and deleted, also after restoring an older backup;
  when a backup is converted to another display type the pivot is scaled along. A newly uploaded clock face has the
  centre as pivot again.
- Motion sensor (ESP32-S3): when the clock lies flat, the timing for a stable position starts anew; a single outlier
  after lying a long time no longer turns the display at once. The page `/api/imu` shows acceleration, position and
  rotation for diagnosis.
- ESP32-S3 (Waveshare ESP32-S3-LCD-1.28): the SPI clock to the display rises from 40 to 80 MHz. The start animation
  runs a bit smoother with it (55 instead of 47 frames in 3 s in one measurement); the other chips stay at 40 MHz.
- The ESP32-S2 build is smaller: with "Core Debug Level: None" (Visual Micro) or `DebugLevel=none` it is 56 KB less,
  and the date of the open WiFis is read without `sscanf`. The app now uses about 95 % instead of 98.7 % of the 2 MB
  space. When building with Visual Micro, set the option for the S2 to "None".
- As long as the time is not confirmed after the start (hands at 12), a small hint "warte auf Zeit" / "waiting for
  time" is shown in the lower part of the clock face.
- Uploaded clock faces and strip graphics are averaged when scaled (every target pixel is the mean of the source
  pixels it covers) instead of taking just one source pixel: large images (e.g. 480 x 480) are shrunk without
  aliasing, solid areas stay exactly the same. Applies to all chips, also without PSRAM. Hands scale as before
  (averaged edges would give a fringe towards the transparency colour).
- `flashESP.bat`/`flashESP.sh` download the build of the detected chip from the GitHub release when they start
  (`uhr4-esp32s2.zip`, `-esp32c6.zip`, `-esp32s3.zip`, about 1.3 MB each, with a SHA-256 check before flashing)
  instead of taking it from the ZIP. The flash ZIP now contains only the tools (7 instead of 15 MB). The PC needs
  internet for that; there is no fallback to local files, on an error nothing is flashed. For development
  `clocksetup.ps1 -Local` or `UHR4_LOCAL=1` flashes the folders `esp32s2`/`esp32c6`/`esp32s3`. The packages are
  created by `build_uhr4/publish_builds.ps1`.
- In the file manager the files of the starter set (the firmware's clock faces and hand sets) come first in their
  group and, as on the "Clock face" and "Hands" pages, can be neither deleted nor renamed nor scaled; "Select all"
  leaves them out.
- After a restart (OTA, browser, WPS) the hands stay at 12 for up to 15 s as long as only the system time kept by
  the chip is there (it may be wrong). They then move once to the confirmed time (NTP, RTC, DCF77, Rocrail or
  set by hand), instead of first to the wrong one and then, in the middle of the move, to the right one.
- While the hands move to their position (start, time correction, about 3 s), background work rests: no writing
  of the log to flash, no web server, no NTP replies, no Rocrail. The log then states duration, frame count and the
  longest gap between two frames.
- Deleting a clock face or hand set shows a progress bar at the top ("Deleting - please wait") until the page
  is reloaded.
- Hands move to their position (start, time correction) without a stutter in the middle: jumps over 10° go
  unchanged to the 3-second move, instead of being smoothed step by step first and restarting the move.
- The log now shows address and time source in one line each (I2C scan, time synchronisation).
- The "Presets" page no longer has "Save Presets to File" and "Load Presets from File" (with the routes
  `/exportpresets` and `/importpresets`); the "Backup" page saves presets as well.
- Clicking a preset no longer reloads the "Presets" page: the preset is applied and the message "Preset applied"
  appears at the top (like for clock faces and hand sets).
- The chip (S2, S3, C6) now appears next to the display type in the header of the web interface ("S3
  GC9A01_WITH_BACKLIGHT"), in the backup file name (`uhr4-backup-<host name>-<chip>-<display type>-<date>.tar`) and
  in the header of the status file included in the backup.
- flashESP.bat: after flashing and sending WiFi and time, a serial monitor stays open. It shows the clock's output,
  reconnects by itself after a restart and ends with Q or Esc. The clock only writes if "Enable Logging" is on.
  Without monitor: `clocksetup.ps1 -Flash -NoMonitor`.
- Clock face and hand set page: a click on a clock face or hand set selects it without reloading the page; only the
  "(active)" note moves and the message appears at the top. The preview images carry the file version in their
  address and stay cached in the browser (when a file changes, its address changes). The pages therefore also load
  noticeably faster when opened again. If the call fails, the browser follows the link as before. The strip of the
  clock face preview (shows time/date) is not cached.
- Clock face designer: the hub colour no longer has a colour field of its own. It comes from the toolbox (picker,
  palette, colour field): choose a colour and press "Current colour" in the "Hub and second hand" area; a swatch next
  to it shows the hub colour.
- The clock stores all hand files in the display's new format at start (and after a restore). Hands in the old
  format (narrower and/or shorter) are padded transparent as when loading; the display stays the same, the
  overview shows "new format". Files of any other size stay untouched.
- The clock numbers the hand sets consecutively again at start: 0, 1 and 2 stay, the subdial set becomes 3, the
  others follow in their previous order from 4. Gaps from deleted sets or a restore close. Presets, the active
  hand set and the remembered number of the subdial set follow. If the power fails while renumbering, the clock
  continues at the next start.
- Every hand set now has all three hands: if one is missing, the clock copies it from set 0 – at start, in the
  hand set overview, on activation and after a restore. It used to show the missing hand from set 0 only as a
  substitute and thus followed later changes of set 0; now the copy is a hand of its own in the set.
- ESP32-C6: own partition table `uhr4_c6` with 2.25 MB program (previously 2 MB) and 1.625 MB file system
  (previously 1.875 MB) – the program no longer fit into 2 MB. When building yourself copy `boards.local.txt` and
  `uhr4_c6.csv` into the core folder and choose the scheme "uhr4 C6 (2.25MB APP/1.6MB SPIFFS)"; with "No OTA" the
  build stops with a message. **Take a backup before flashing an existing C6 for the first time:** the file system
  sits elsewhere and is created anew, clock faces, hands, strips and fonts are empty afterwards; settings, WiFi and
  presets stay. Then restore the backup.
- Restoring a backup no longer deletes anything: existing clock faces, hand sets, strips, fonts and presets
  stay, same-named ones from the backup replace them. Backup presets with a new name go into free slots. The
  clock takes over the other settings from the backup as before. Hand sets of the backup with a number the
  firmware occupies (0–2, subdial set – e.g. in an older backup) get the next free number from 3, and the
  presets and the active hand set of the backup follow. If all hands are pixel-identical to the firmware's set
  (the generated sets), they are dropped and the presets point to the firmware's set.
- Every boot starts without a time: the system time used to survive a restart via the reset button or
  software and is now discarded at the start. The clock gets it anew as after a power cut (NTP, RTC, DCF77,
  open WiFi, USB).
- Display selection ("Clock Setup" tab): ILI9341 and ST7789 172 × 320 read "with time and date strip" instead
  of "with time and date below the clock" – the strip can also be above the clock.
- Button (or Boot button) gets a stage for the WiFi: from 10 s a yellow "WiFi Reset" countdown runs for 10 s
  (releasing aborts), from 20 s a red "Factory Reset" countdown for 10 s. Releasing during it deletes all
  stored WiFi networks and restarts; the full factory reset now only comes after 30 s (previously 15 s).
  Every stage is logged.

### Removed
- "Download Additional Clock Faces from GitHub" (clock face page) and "Download Additional Hand Sets from GitHub"
  as well as the "Upload New Hand Set" form (hand set page) are gone. Clock faces and hands come from the
  designers or onto the clock via the file manager and the backup; the upload form of the clock face page stays.

### Fixed
- Hand designer: the hands of the active design are loaded one after another (each tried up to three times) instead of
  in parallel. Before, a broken-off request could leave a hand empty, which was then saved as an empty hand into a new
  design. If a hand cannot be loaded the designer says so and does not save.
- Time from open WiFis on the ESP32-S3: the clock connects to open hotspots at full transmit power (they are usually far
  away), afterwards the lowest step applies again. New diagnostic command `UHR4 OPENWIFI` (USB): gets the time
  from an open WiFi on demand (also from -95 dBm), every step is in the log, then restart.
- ESP32-S3 build: now aborts with a message if PSRAM is not set to "QSPI PSRAM". The chip has 2 MB PSRAM; without it
  only about 30 KB of free heap remained (largest block 8 KB), and pages of the web interface timed out.
- The clock chooses the WiFi transmit power itself (`WIFI_TX_AUTO` in `config.h`). At full power (20 dBm) routers and
  hotspots did not answer the ESP32-S3's login (disconnect reason "2 AUTH_EXPIRE"). The S3 starts at 8.5 dBm and on
  failure goes up via 13 and 17 to 19.5 dBm; S2 and C6 start with the full default power and only go down on failure
  (17, 13, 8.5 dBm). One WPA2-only attempt per step, a last one with WPA3. For a new WiFi the clock checks the
  gateway after connecting (TCP and DNS answer): too high power lets short management frames through but not data.
  If the check passes on no step, the first step on which it connected applies. The step that works is remembered per
  WiFi, as is whether the network needs WPA3 (then it starts with WPA3 right away), and used first at the next start
  (without the check); it is not part of the backup. After a failure the start
  step applies again (access point, open WiFis). The status page shows the transmit power.
- WiFi log: the line for the connection attempt names the length of the password (not the password) and the TX power.
- Time from open WiFis: the TLS connection to a hotspot's login page gets 10 s instead of 4 s, and if it fails, the log
  names the mbedTLS error. Hotspots like "Vodafone Hotspot" redirect to a login page whose reply comes without a
  `Date:` header; the clock then needs the login page's HTTPS reply.
- WiFi: the first connection attempt runs with WPA2 only (without protected management frames, 15 s), only the
  second with WPA3. Routers in WPA2/WPA3 mixed mode did not answer the WPA3 login of the ESP32-S3 (disconnect reason
  "2 AUTH_EXPIRE"). A WPA3-only router therefore connects only on the second attempt.
- WiFi: when connecting, the clock scans all channels and takes the strongest access point with the stored name.
  It used to take the first one found; with several access points of the same name (repeater, mesh) that could be
  the weak one and the connection failed with "2 AUTH_EXPIRE". On success the log names channel, BSSID and signal
  strength.
- WiFi: when a connection fails, the log now names the reason of the disconnect (e.g. "202 AUTH_FAIL" for a wrong
  password, "201 NO_AP_FOUND", "203 ASSOC_FAIL" when the router refuses the device).
- The hostname derived from the MAC address (`clock_XXXXXX`) was `clock_000000` on the ESP32-S3 because the address
  was read too early. The clock now reads it straight from the chip, independent of the WiFi state. A stored custom
  hostname stays unchanged.
- Messages on the display (WPS hint "then Access Point", access point screen with "http://…"): when a text did not fit
  any of the larger fonts, the clock used the tiny fallback font (6 x 8 pixels). Now it shrinks the last larger
  font down to 75 % and only then enlarges the fallback font (up to 3x). This mostly affects round displays, where
  the bottom row is narrow.
- The button and boot button did not react while the clock waited for a WiFi connection (30 s each, at boot
  and on every reconnect), queried open WiFis, was in the WPS countdown or waited for DCF77. They are now
  checked there too. Also, right after a failed boot the clock immediately tried again for 60 s; the first
  reconnect attempt now comes after 5 minutes, then hourly. New USB command `UHR4 PINS` (diagnostics: levels
  of the button and boot button).
- Access point mode after boot (stored WiFi not reachable): 2 minutes after the start a connection attempt
  switched the AP off – if it failed, the clock was reachable neither via WiFi nor via AP until the next
  hourly attempt. Now the clock also checks only hourly in AP mode (with "Reconnect WiFi") and restarts the
  AP right after a failed attempt; if the connection works, it leaves AP mode. During operation the clock
  still never enters AP mode.

## 2026-10-07

### Changed
- The hub (centre over the hands) is set in the clock face designer: radius and colour (colour picker) with
  live preview. The clock shows changes right away, "Save hub" stores them. The "Centre point" form on the
  "Hand Set" page is gone.

## 2026-10-06

### Added
- "Use device time" button: if the clock has no time (no WiFi/NTP, no RTC, no DCF77, e.g. in access point
  mode), the status bar shows it. It sets the clock to the time of the phone or PC, and an existing RTC as
  well. The same button is in the "NTP Timezone" tab.

### Changed
- GitHub repository renamed to `holgiw/ESP32-Station-Clock` (previously `TFT-Clock-GC9A01`; the name no longer
  fit the different displays and boards). GitHub redirects old links, firmware and docs point to the new name.
- Starter set of a new clock:
  - `face_default.bmp` has the numerals 12, 3, 6 and 9.
  - Without clock faces and hand sets the clock also generates `face_numbers.bmp` (1–12), `face_roman.bmp`
    (I–XII), hand sets 1 (bars, black second hand) and 2 (curved) and, if there are no presets, the presets
    "Standard", "1-12" and "I-XII" (without a second hand).
  - The factory reset actions for clock faces, hand sets and presets restore this state.
  - If the generated files do not fit the clock size after a display type change, the clock regenerates them
    at startup.
  - Displays with a strip (ILI9341, ST7789 172 × 320): the three presets have a white strip with black text
    in FreeSans Bold, DejaVu or the designer font "Sans" (if on the clock, otherwise GLCD). A new clock starts
    with the strip of "Standard".
- Restoring a backup of another display type: the clock points this out and only takes over clock faces,
  hands (scaled to its size), presets and general settings. It converts the hub size (also in the presets),
  the hub colour comes along. Display type, rotation, backlight and brightness stay. If the backup's clock
  faces are not 240 × 240, it takes over nothing and says so. If the backup has no strip, a clock with a strip
  gets the standard strip (white, black text, FreeSans Bold). Before, the clock rejected a different clock
  size and restored everything with the same size (e.g. GC9A01 and ILI9341).
- Backup: the display type is always included, even if it was never stored (factory default). Otherwise the
  target clock assumed its own factory default when restoring. Older backups without this entry count as
  GC9A01 (the ESP32-S2's factory default).
- Backup: the file name contains the time with seconds (e.g.
  `uhr4-backup-clock_23F6A4-ST7789-20261006-175812.tar`), so several backups on one day do not overwrite each
  other.
- `flashESP.sh` (Linux) knows the ESP32-C6: recognizes it by the USB port (303a:1001), offers ST7789 and
  ST7789_240 and flashes the build from `esp32c6` with `boot_app0.bin`, without download mode.
- ST7789 172 × 320: start and status messages are shown in landscape (rotated 90°) at double font size.
- Upload:
  - If the file name lacks `face_` (clock face) or `hand_set` (hand), the clock adds it, e.g. `station.bmp` →
    `face_station.bmp`, `set3_hour.bmp` or `3_hour.bmp` → `hand_set3_hour.bmp`. `.BMP` is lowercased.
  - The hints below the upload forms state what the clock accepts: BMP with 16, 24 or 32 bit, at most as large
    as the free space (shown). The clock scales and converts to RGB565.
- Presets store whether the Rocrail model time is on and whether the clock reconnects the WiFi (`rocrail=` and
  `wifiReconnect=true/false` in `/api/setMode`). Server addresses and WiFi credentials stay settings of the
  clock.
- Backup and restore show a progress bar. While the clock is busy, the status bar pauses instead of reporting
  "Connection lost". After restoring, the page waits until the clock answers again after the restart.

### Removed
- "Load Presets from GitHub" button on the presets page, the prompt for an empty list and the route
  `/importpresetsmerge` used only for it.
- Link to the ZIP files (`faces_handsets_240.zip` / `_160.zip`) on the clock faces page.

### Fixed
- Upload: 16-bit BMPs in RGB555 get the right colours. The clock rejects unsupported BMPs (palette, 8/4/1 bit,
  compressed) instead of storing them as a black image; the rejected file does not stay behind.
- Uploading hand sets: a clock face (`face_…`) uploaded there ended up on the clock despite the error message.
  The hand set page only accepts hands now.
- Restore: the warning about a different display type in the backup's file name works (the date with a hyphen
  was not recognized) and knows the ST7789 types.
- ESP32-C6: after a software restart (restore, restart button, flashESP) the display stayed black, only a
  power cycle helped. CS is no longer held LOW permanently, LovyanGFX drives it for each transfer.
- Presets: preview images were often missing. The browser requested all of them at once, and the clock
  (especially without PSRAM) ran out of memory. The page loads them one after another with up to three tries;
  the clock sends each image row by row instead of as a second copy in RAM.
- Clock face designer (displays with a strip): filling stays within the clicked area, clock face or strip.
  Before, e.g. the margin around a round clock face ran on into the strip and coloured it too.
- ESP32-C6 (without PSRAM): a clock face's strip graphic (`strip_….bmp`) was missing because there was not
  enough memory for its copy in RAM; the strip stayed plain. The clock reads it row by row from the file, on
  the display and in the previews.

## 2026-10-05

### Added
- First support for the Waveshare ESP32-C6-LCD-1.47 (ST7789, 172 × 320) and ESP32-C6-LCD-1.3 (ST7789,
  240 × 240) as a separate build:
  - Display types ST7789 (clock 172 × 172 with a time and date strip) and ST7789_240.
  - Pin mapping in `config.h` (`BOARD_WAVESHARE_C6_ST7789`, recognized by the chip; both boards mapped alike),
    no second display.
  - Clock faces from GitHub come from the 240 folder and are scaled down on upload.
  - The linker switch `-mtext-section-literals` only applies to the ESP32-S2 (`board.txt`,
    `platform.local.txt`).
  - Without PSRAM the clock keeps the clock face only compressed (or reads it from the file) and redraws every
    frame from it. Uploads scale row by row without a full frame in RAM; previews (presets, preview page) scale
    down or stream row by row.
  - `flashESP.bat` recognizes the C6 by its USB port and flashes the build from `build_uhr4\esp32c6`.
  - The build needs "USB CDC On Boot: Enabled", otherwise it stops with a message.

## 2026-10-04

### Fixed
- README: note about the English version directly below the title; the link to the no longer existing release
  "Version 3" is gone.

## 2026-10-02

### Changed
- Backup: clock faces, hands and strip graphics are stored as normal BMP images (previously in the clock's
  compressed format) and open in any image program. On restore the clock packs them like on upload; older
  backups can still be restored.
- Retook the screenshots (`screenshots/de`, `screenshots/en`): the DCF77 page from a clock with a receiver,
  again the clock face designer of an ILI9341 clock (`clock_face_designer_ili.png`) and newly its preview with
  the time/date strip (`preview_ili.png`).
- Revised the selection of included clock faces, hands and presets (`graphic/` with `faces_handsets_160.zip`
  and `faces_handsets_240.zip`, `presets.txt`).

### Fixed
- Self-drawn default clock face: on round displays everything outside the circle stays white like with uploaded
  clock faces. It survives backup and restore unchanged pixel for pixel.
- `presets.txt`: the preset "Antik" is taken over again when downloading from GitHub. Name and address were
  separated by a space instead of a tab, so the line was skipped.
