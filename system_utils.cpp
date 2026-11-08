#include "uhr4.h"

    // Systemfunktionen: Tasten, Logging, Reset, Neustart, Hilfsfunktionen

    // System functions: buttons, logging, reset, restart, helpers


    // Zeilen der Tastermeldungen: Hoehe der Zeilenmitte als Anteil der Displayhoehe (Titel, Zeile 2, Hinweis) und
    // Schriften von gross nach klein - je Zeile gilt die erste, in der der Text auf dem Display Platz hat. Nur
    // Schriften, die ohnehin in der Firmware stecken (Streifen, Zifferblatt-Generator).

    // Rows of the button messages: height of the row centre as a share of the display height (title, line 2,
    // hint) and fonts from large to small - per row the first one in which the text fits on the display is used.
    // Only fonts that are in the firmware anyway (strip, clock face generator).

    const float BUTTON_ROW_Y[3] = { 0.30f, 0.53f, 0.76f };
    const lgfx::IFont* const TEXT_FONTS_BIG[] = { &fonts::FreeSansBold24pt7b, &fonts::FreeSansBold12pt7b, &fonts::DejaVu18, &fonts::Font0, nullptr };
    const lgfx::IFont* const TEXT_FONTS_TITLE[] = { &fonts::FreeSansBold12pt7b, &fonts::DejaVu18, &fonts::Font0, nullptr };
    const lgfx::IFont* const TEXT_FONTS_HINT[] = { &fonts::DejaVu18, &fonts::Font0, nullptr };

    // Eine Textzeile zentriert auf Hoehe yShare (Anteil der Displayhoehe) zeichnen, in der ersten Schrift aus
    // fontList (Ende = nullptr), in der der Text Platz hat: Displaybreite, beim runden Display die Sehne des
    // Kreises auf dieser Hoehe (mit Rand). clear = Zeile vorher schwarz loeschen (Countdown ohne Flackern).

    // Draw a text row centred at height yShare (share of the display height), in the first font from fontList
    // (end = nullptr) in which the text fits: display width, on a round display the chord of the circle at that
    // height (with a margin). clear = blank the row first (countdown without flicker).

    void drawTextRow(lgfx::LovyanGFX& g, float yShare, const String& text, uint16_t color, const lgfx::IFont* const* fontList, bool clear) {
        int w = statusWidth(), h = CLOCK_HEIGHT;
        int y = (int)(h * yShare);
        float usable = w * 0.92f;
        if (displayGeom->round) {
            float r = h / 2.0f, dy = fabsf(y - r) + h * 0.06f; // halbe Zeilenhoehe mitrechnen / count half a row height
            usable = dy < r ? 2.0f * sqrtf(r * r - dy * dy) * 0.9f : 0;
        }
        g.setTextSize(1);
        int chosen = 0;
        for (int i = 0; fontList[i]; i++) {
            chosen = i;
            g.setFont(fontList[i]);
            if (g.textWidth(text) <= usable) break;
        }

        // Passt nur noch Font0 (6 x 8 Pixel, sehr klein): lieber die vorige Schrift bis auf 75 % verkleinern, und wenn
        // auch das nicht reicht, Font0 in Halbschritten bis 3-fach vergroessern.

        // Only Font0 (6 x 8 pixels, very small) is left: rather shrink the previous font down to 75 %, and if that
        // is not enough either, enlarge Font0 in half steps up to 3x.

        if (chosen > 0 && fontList[chosen] == &fonts::Font0) {
            g.setFont(fontList[chosen - 1]);
            float wide = (float)g.textWidth(text);
            if (wide > 0 && usable / wide >= 0.75f) {
                g.setTextSize(usable / wide);
            }
            else {
                g.setFont(&fonts::Font0);
                float wide0 = (float)g.textWidth(text);
                g.setTextSize(wide0 > 0 ? constrain(floorf(usable / wide0 * 2.0f) / 2.0f, 1.0f, 3.0f) : 1.0f);
            }
        }
        if (clear) {
            int fh = g.fontHeight();
            g.fillRect(0, y - fh / 2 - 2, w, fh + 4, TFT_BLACK);
        }
        g.setTextDatum(lgfx::middle_center);
        g.setTextColor(color, TFT_BLACK);
        g.drawString(text, w / 2, y);

        // Zustand wie der restliche Code ihn erwartet / state as the rest of the code expects it

        g.setTextDatum(lgfx::top_left);
        g.setFont(&fonts::Font0);
        g.setTextSize(1);
    }

    // Meldung der Tasterstufen und des Werksresets: schwarzer Hintergrund, Titel und Zeile 2 zentriert in der
    // Farbe der Stufe (gelb = WLAN, rot = Werksreset), Zeile 3 kleiner als Hinweis in hintColor.

    // Message of the button stages and the factory reset: black background, title and line 2 centred in the
    // colour of the stage (yellow = WiFi, red = factory reset), line 3 smaller as a hint in hintColor.

    void showButtonMessage(uint16_t color, const String& title, const String& line2, const String& hint, uint16_t hintColor) {
        DRAW_ON_BOTH_DISPLAYS(
            tft.fillScreen(TFT_BLACK);
            drawTextRow(tft, BUTTON_ROW_Y[0], title, color, TEXT_FONTS_TITLE, false);
            drawTextRow(tft, BUTTON_ROW_Y[1], line2, color, TEXT_FONTS_BIG, false);
            if (hint.length()) drawTextRow(tft, BUTTON_ROW_Y[2], hint, hintColor, TEXT_FONTS_HINT, false);
        );
    }

    // Nur Zeile 2 neu zeichnen (Countdown) - ohne das ganze Bild zu loeschen, also ohne Flackern
    // Redraw only line 2 (countdown) - without clearing the whole screen, so without flicker

    void updateButtonLine(uint16_t color, const String& line2) {
        DRAW_ON_BOTH_DISPLAYS(
            drawTextRow(tft, BUTTON_ROW_Y[1], line2, color, TEXT_FONTS_BIG, true);
        );
    }


    // Taster (BUTTON1 oder Boot-Taster) in Stufen nach Haltedauer: unter 10 s WLAN anzeigen (gruen); 10-20 s
    // gelber Countdown "WiFi Reset", loslassen bricht ab; 20-30 s roter Countdown "Factory Reset", loslassen
    // loescht alle WLAN-Eintraege und startet neu; ab 30 s vollstaendiger Werksreset.

    // Button (BUTTON1 or boot button) in stages by hold time: below 10 s show the WiFi (green); 10-20 s yellow
    // countdown "WiFi Reset", releasing aborts; 20-30 s red countdown "Factory Reset", releasing deletes all WiFi
    // entries and restarts; from 30 s full factory reset.

    void checkButton() {
        if (pinIsHigh(BUTTON1) || pinIsLow(BOOT_BUTTON) || extraButtonDown()) {

            const unsigned long WIFI_RESET_HOLD = 2 * WAIT_10s;
            const unsigned long FACTORY_RESET_HOLD = 3 * WAIT_10s;
            unsigned long pressStart = millis();
            int shownStage = 0;   // 0 = WLAN-Anzeige, 1 = WLAN-Countdown, 2 = Werksreset-Countdown
                                  // 0 = WiFi info, 1 = WiFi countdown, 2 = factory reset countdown
            int shownSecs = -1;
            DEBUG_PRINTLN(String("[BUTTON] Pressed (") + (pinIsHigh(BUTTON1) ? "button" : (pinIsLow(BOOT_BUTTON) ? "boot button" : "extra button")) + ")");

            clearTFT();

            // Einmalig Anzeige zeichnen
            // Draw display once

            showWlanCredentials(WiFi.SSID());

            // Blockierender Loop während Button gedrückt
            // Blocking loop while button is pressed

            while (pinIsHigh(BUTTON1) || pinIsLow(BOOT_BUTTON) || extraButtonDown()) {
                unsigned long held = millis() - pressStart;

                if (held >= FACTORY_RESET_HOLD) {
                    DEBUG_PRINTLN("[BUTTON] Held for 30 s - factory reset");
                    factoryReset();
                    return;
                }

                if (held >= WIFI_RESET_HOLD) {
                    int secs = (int)((FACTORY_RESET_HOLD - held + 999) / 1000);
                    if (shownStage != 2) {
                        DEBUG_PRINTLN("[BUTTON] Held for 20 s - WiFi reset armed, factory reset countdown");
                        shownStage = 2;
                        shownSecs = secs;
                        showButtonMessage(TFT_RED, "Factory Reset", "in " + String(secs) + " s", "release: WiFi Reset", TFT_YELLOW);
                    }
                    else if (secs != shownSecs) {
                        shownSecs = secs;
                        updateButtonLine(TFT_RED, "in " + String(secs) + " s");
                    }
                }
                else if (held >= WAIT_10s) {
                    int secs = (int)((WIFI_RESET_HOLD - held + 999) / 1000);
                    if (shownStage != 1) {
                        DEBUG_PRINTLN("[BUTTON] Held for 10 s - WiFi reset countdown");
                        shownStage = 1;
                        shownSecs = secs;
                        showButtonMessage(TFT_YELLOW, "WiFi Reset", "in " + String(secs) + " s", "release: cancel", TFT_DARKGREY);
                    }
                    else if (secs != shownSecs) {
                        shownSecs = secs;
                        updateButtonLine(TFT_YELLOW, "in " + String(secs) + " s");
                    }
                }
                delay(10);
            }

            // Zwischen 20 und 30 s losgelassen: alle WLAN-Eintraege loeschen (wie "Gespeicherte Netzwerke
            // zuruecksetzen" in der Weboberflaeche), "done" zeigen und neu starten

            // Released between 20 and 30 s: delete all WiFi entries (like "Reset Saved Networks" in the web
            // interface), show "done" and restart

            if (shownStage == 2) {
                DEBUG_PRINTLN("[BUTTON] Released - deleting all WiFi entries and restarting");
                showButtonMessage(TFT_YELLOW, "WiFi Reset", "...", "", TFT_YELLOW);
                eraseWiFiConfig();
                showButtonMessage(TFT_YELLOW, "WiFi Reset", "done", "restarting", TFT_DARKGREY);
                delay(WAIT_3s);
                espReboot();
                return;
            }
            if (shownStage == 1) {
                DEBUG_PRINTLN("[BUTTON] Released during the WiFi reset countdown - aborted");
                showButtonMessage(TFT_YELLOW, "WiFi Reset", "cancelled", "", TFT_YELLOW);
                delay(WAIT_1s + WAIT_1s / 2);
            }

            // Button wurde vor 10secs losgelassen → WLAN-Credentials für 3 Sekunden anzeigen
            // Button released before 10 secs → show WLAN credentials for 3 seconds

            if (shownStage == 0) {
                delay(WAIT_3s);
            }

            // Malt direkt auf TFT statt Sprite-Backbuffer; firstRun=true laesst
            // die Zeiger sofort zur aktuellen Zeit springen statt einzuschleichen
            // (siehe Glaettung in renderClockFrame()).

            // Draws directly to TFT instead of the sprite backbuffer; firstRun=true
            // makes the hands snap to the current time instead of easing in
            // (see smoothing in renderClockFrame()).

            firstRun = true;
        }
    }

#if BOARD_RGB_PANEL

#if BOARD_WAVESHARE_S3_LCD4

    // Touch (GT911 am I2C 15/7, nach dem Reset auf 0x14 oder 0x5D) direkt per Wire lesen - LovyanGFX' Touch_GT911 meldete
    // auf dem Board nichts. Register 0x814E = Status (Bit 7 = Daten bereit, Bits 0-3 = Anzahl Punkte), 0x8150 = erster
    // Punkt. Der Status muss nach dem Lesen auf 0 gesetzt werden.

    // Read the touch (GT911 on I2C 15/7, at 0x14 or 0x5D after the reset) directly via Wire - LovyanGFX's Touch_GT911
    // reported nothing on the board. Register 0x814E = status (bit 7 = data ready, bits 0-3 = number of points), 0x8150 =
    // first point. The status has to be set to 0 after reading.

    uint8_t gt911Addr = 0;

    bool gt911Read(uint16_t reg, uint8_t* buf, size_t len) {
        Wire.beginTransmission(gt911Addr);
        Wire.write((uint8_t)(reg >> 8));
        Wire.write((uint8_t)(reg & 0xFF));
        if (Wire.endTransmission(true) != 0) return false;
        if (Wire.requestFrom((int)gt911Addr, (int)len) != (int)len) return false;
        for (size_t i = 0; i < len; i++) buf[i] = Wire.read();
        return true;
    }

    void gt911Write(uint16_t reg, uint8_t value) {
        Wire.beginTransmission(gt911Addr);
        Wire.write((uint8_t)(reg >> 8));
        Wire.write((uint8_t)(reg & 0xFF));
        Wire.write(value);
        Wire.endTransmission(true);
    }


    // Waehrend die Beruehrung abgefragt wird, sucht lcd4TouchPressed() den Touch-Chip alle 2 s, bis er antwortet. Waveshare: GT911,
    // Status in 0x814E (Bit 7 = Daten bereit, Bits 0-3 = Anzahl Punkte), danach quittieren.
    // While touches are polled, lcd4TouchPressed() searches the touch chip every 2 s until it answers. Waveshare: GT911, status
    // in 0x814E (bit 7 = data ready, bits 0-3 = number of points), acknowledged afterwards.

    bool lcd4TouchPressed() {
        static unsigned long lastProbe = 0;
        if (!gt911Addr) {
            if (millis() - lastProbe < 2000) return false;
            lastProbe = millis();
            for (uint8_t a : { (uint8_t)0x14, (uint8_t)0x5D }) {
                Wire.beginTransmission(a);
                if (Wire.endTransmission() == 0) { gt911Addr = a; break; }
            }
            if (gt911Addr) DEBUG_PRINTLN("[TOUCH] GT911 found at 0x" + String(gt911Addr, HEX));
            return false;
        }

        uint8_t status = 0;
        if (!gt911Read(0x814E, &status, 1) || !(status & 0x80)) return false;
        gt911Write(0x814E, 0);
        return (status & 0x0F) != 0;
    }

    void lcd4TouchDone() { gt911Write(0x814E, 0); }

    // Die letzten 8 Beruehrungen (Aufsetzen): Position des Touch-Chips (x, y) und auf dem Zifferblatt (sx, sy), fuer
    // /api/touchlog.

    // The last 8 touches (touch-down): position of the touch chip (x, y) and on the clock face (sx, sy), for
    // /api/touchlog.

    inline int lcd4TouchLog[8][4] = {};
    inline uint32_t lcd4TouchLogCount = 0;

    String lcd4TouchDebug() {
        String r = "{\"count\":" + String(lcd4TouchLogCount) + ",\"rotation\":" + String(tftRotation1) + ",\"touches\":[";
        for (uint32_t k = 0; k < 8 && k < lcd4TouchLogCount; k++) {
            const int* e = lcd4TouchLog[(lcd4TouchLogCount - 1 - k) % 8]; // neueste zuerst / newest first
            r += String(k ? "," : "") + "[" + String(e[0]) + "," + String(e[1]) + "," + String(e[2]) + "," + String(e[3]) + "]";
        }
        r += "],\"max\":";
        uint8_t cfg[4] = {};
        if (gt911Addr && gt911Read(0x8048, cfg, 4)) {
            r += "[" + String(cfg[0] | (cfg[1] << 8)) + "," + String(cfg[2] | (cfg[3] << 8)) + "]"; // X-/Y-Aufloesung des Chips / X/Y resolution of the chip
        }
        else {
            r += "null";
        }
        return r + "}";
    }

    // Fuer die Touch-Zonen: 0 = nichts Neues, 1 = Beruehrung an der Stelle (x, y), 2 = losgelassen. Koordinaten wie im
    // GT911: Register 0x8150 = x (2 Byte), y (2 Byte), Groesse.

    // For the touch zones: 0 = nothing new, 1 = touch at (x, y), 2 = released. Coordinates as in the GT911: register
    // 0x8150 = x (2 bytes), y (2 bytes), size.

    int lcd4TouchRead(int& x, int& y) {
        if (!gt911Addr) {
            lcd4TouchPressed(); // sucht den Touch-Chip / searches the touch chip
            return 0;
        }
        uint8_t status = 0;
        if (!gt911Read(0x814E, &status, 1) || !(status & 0x80)) return 0;
        const uint8_t points = status & 0x0F;
        uint8_t p[8] = {};
        const bool ok = points > 0 && gt911Read(0x8150, p, 8);
        gt911Write(0x814E, 0);
        if (points == 0) return 2;
        if (!ok) return 0;
        x = p[0] | (p[1] << 8); // 0x8150 = X niedrig, X hoch, Y niedrig, Y hoch / X low, X high, Y low, Y high
        y = p[2] | (p[3] << 8);
        return 1;
    }

    // Position des Touch-Chips (liegt fest am Panel) in die Lage des Zifferblatts umrechnen (480 x 480, Ursprung links
    // oben): Das Bild ist um tftRotation1 x 90 Grad im Uhrzeigersinn gedreht (am Geraet bei 180 Grad geprueft), die
    // Beruehrung muss genauso gedreht werden.

    // Convert a position of the touch chip (fixed to the panel) to the orientation of the clock face (480 x 480, origin
    // top left): the image is rotated clockwise by tftRotation1 x 90 degrees (checked on the device at 180 degrees), the
    // touch has to be rotated the same way.

    void touchToScreen(int x, int y, int& sx, int& sy) {
        switch (tftRotation1) {
            case 1:  sx = y;       sy = 479 - x; break;
            case 2:  sx = 479 - x; sy = 479 - y; break;
            case 3:  sx = 479 - y; sy = x;       break;
            default: sx = x;       sy = y;       break;
        }
    }

#else

    void lcd4TouchDone() {} // Platzhalter fuer Boards ohne Touch / placeholder for boards without touch

#endif

    // Beruehrung des Displays: wie ein kurzer Tastendruck die WLAN-Anzeige (Name, IP-Adresse) fuer 5 Sekunden zeigen,
    // dann zurueck zur Uhr. Abgefragt wird alle 25 ms; der Touch-Chip je Board in lcd4TouchPressed() (system_utils.cpp).

    // Touching the display: like a short button press show the WiFi info (name, IP address) for 5 seconds, then back to
    // the clock. Polled every 25 ms; the touch chip per board in lcd4TouchPressed() (system_utils.cpp).

#if BOARD_WAVESHARE_S3_LCD4

    // Wischen: Das Zifferblatt folgt dem Finger (Verschiebung des ausgegebenen Bildes, lcd4ShiftX/Y) und gleitet bei einem
    // Wisch ganz heraus; dann wird das Preset gewechselt und das neue gleitet von der anderen Seite herein. Ohne Wisch gleitet
    // das Bild zurueck. Phasen: 1 heraus, 2 warten auf das neu gezeichnete Bild, 3 herein, 4 zurueck.

    // Swipe: the clock face follows the finger (shift of the output picture, lcd4ShiftX/Y) and slides out completely on a
    // swipe; then the preset is changed and the new one slides in from the other side. Without a swipe the picture slides
    // back. Phases: 1 out, 2 waiting for the redrawn picture, 3 in, 4 back.

    inline int slideCur = 0, slideFrom = 0, slideTo = 0;
    inline uint8_t slidePhase = 0;
    inline unsigned long slideT0 = 0, slideDur = 0;
    inline uint32_t slideSwaps0 = 0;

    // Verschiebung dx in Zifferblatt-Richtung (links/rechts wie der Nutzer sie sieht) in Panel-Koordinaten umrechnen, passend zu
    // touchToScreen()

    // Convert the shift dx in clock face direction (left/right as the user sees it) to panel coordinates, matching
    // touchToScreen()

    void setSlide(int dx) {
        dx = constrain(dx, -480, 480);
        slideCur = dx;
        int px = 0, py = 0;
        switch (tftRotation1) {
            case 1:  py = dx;  break;
            case 2:  px = -dx; break;
            case 3:  py = -dx; break;
            default: px = dx;  break;
        }
        lcd4ShiftX = (int16_t)px;
        lcd4ShiftY = (int16_t)py;
    }

    void startSlide(uint8_t phase, int from, int to, unsigned long dur) {
        slidePhase = phase;
        slideFrom = from;
        slideTo = to;
        slideT0 = millis();
        slideDur = dur;
    }

    bool slideStep() {
        if (slidePhase == 0) return false;
        if (slidePhase == 2) {
            const unsigned long waited = millis() - slideT0;
            if ((lcd4Swaps != slideSwaps0 && waited >= 80) || waited > 700) startSlide(3, slideFrom, 0, 180);
            return true;
        }
        float t = (float)(millis() - slideT0) / (float)slideDur;
        if (t > 1.0f) t = 1.0f;
        const float ease = (slidePhase == 3) ? 1.0f - (1.0f - t) * (1.0f - t) : t * t; // herein: bremsen, heraus: beschleunigen / in: decelerate, out: accelerate
        setSlide(slideFrom + (int)((slideTo - slideFrom) * ease));
        if (t < 1.0f) return true;
        if (slidePhase == 1) {
            switchPreset(slideTo < 0 ? 1 : -1);
            firstRun = true;
            slideSwaps0 = lcd4Swaps;
            slideFrom = -slideTo; // das neue Zifferblatt kommt von der Gegenseite / the new face comes from the opposite side
            slidePhase = 2;
            slideT0 = millis();
            return true;
        }
        slidePhase = 0;
        return false;
    }


    // Blockierendes Gleiten des Bildes von 'from' nach 'to' (Bildpunkte, Richtung des Zifferblatts) in dur ms - fuer die Auswahlen
    // Blocking slide of the picture from 'from' to 'to' (pixels, clock face direction) in dur ms - for the pickers

    void slideAnimate(int from, int to, unsigned long dur) {
        if (from == to) { setSlide(to); return; }
        const unsigned long t0 = millis();
        for (;;) {
            float t = (float)(millis() - t0) / (float)dur;
            if (t > 1.0f) t = 1.0f;
            setSlide(from + (int)((to - from) * (1.0f - (1.0f - t) * (1.0f - t))));
            if (t >= 1.0f) break;
            delay(4);
        }
    }

    // Set-Auswahl: alle gespeicherten Presets (Zifferblatt mit Zeigern) als Miniaturen (3 x 3 je Seite, Wischen blaettert),
    // Antippen waehlt, Antippen neben einer Miniatur oder 30 s ohne Beruehrung schliesst. Das aktive Preset hat einen Rahmen in der Akzentfarbe
    // der Weboberflaeche. Die Miniaturen (Zeiger auf 10:10:30) werden einzeln geladen und im PSRAM gehalten.

    // Set picker: all saved presets (clock face with hands) as thumbnails (3 x 3 per page, swiping turns pages), a tap
    // selects, a tap beside a thumbnail or 30 s without touch closes. The active preset has a frame in the accent colour. The thumbnails
    // (hands at 10:10:30) are loaded one at a time and kept in PSRAM.

    const int FM_T = 136, FM_G = 14, FM_M = 22, FM_MAX = 36;
    const uint16_t FM_ACCENT = 0xF524; // Akzentfarbe der Weboberflaeche (#f5a623) als RGB565 / accent colour of the web interface (#f5a623) as RGB565

    void faceMenuCellPos(int i, int& x, int& y) {
        x = FM_M + (i % 3) * (FM_T + FM_G);
        y = FM_M + (i / 3) * (FM_T + FM_G);
    }

    // Angaben zur eingeblendeten Zeit eines Zifferblatts (aus ihrer stripcfg-Datei, ohne Datei: aus)
    // Information on the time shown on a clock face (from its stripcfg file, without a file: off)

    struct FmInfo {
        bool time = false, date = false, wday = false; // ab Werk aus (Zifferblatt ohne Streifen) / off by default (clock face without strip)
        uint16_t fg = 0xFFFF;
    };

    uint16_t faceMenuRgb565(uint32_t rgb) {
        return (uint16_t)(((rgb >> 8) & 0xF800) | ((rgb >> 5) & 0x07E0) | ((rgb >> 3) & 0x001F));
    }

    FmInfo faceMenuInfo(const String& face) {
        FmInfo fi;
        const String path = stripConfigPath("/" + face);
        if (!path.length() || !LittleFS.exists(path)) return fi;
        File f = LittleFS.open(path, "r");
        if (!f) return fi;
        const String text = f.size() < 2048 ? f.readString() : String("");
        f.close();
        auto value = [&](const char* key) -> String {
            const int p = text.indexOf(String(key) + "=");
            if (p < 0) return "";
            const int s = p + strlen(key) + 1;
            const int e = text.indexOf('&', s);
            return e < 0 ? text.substring(s) : text.substring(s, e);
        };
        if (value("stripShowT").length()) fi.time = value("stripShowT") != "0";
        if (value("stripShowD").length()) fi.date = value("stripShowD") != "0";
        if (value("stripShowW").length()) fi.wday = value("stripShowW") != "0";
        if (value("stripFg").length()) fi.fg = faceMenuRgb565(strtoul(value("stripFg").c_str(), nullptr, 16));
        return fi;
    }

    void faceMenuDrawCell(lgfx::LovyanGFX& g, int i, const uint16_t* thumb, bool current, const String& label, const FmInfo& fi) {
        int x, y;
        faceMenuCellPos(i, x, y);
        g.fillRect(x - 5, y - 5, FM_T + 10, FM_T + 10, TFT_BLACK);
        if (current) {
            g.drawRect(x - 4, y - 4, FM_T + 8, FM_T + 8, FM_ACCENT);
            g.drawRect(x - 3, y - 3, FM_T + 6, FM_T + 6, FM_ACCENT);
        }
        if (thumb) {
            g.setSwapBytes(true);
            g.pushImage(x, y, FM_T, FM_T, thumb);
            g.setSwapBytes(false);
        }
        else {
            g.fillRect(x, y, FM_T, FM_T, 0x2104); // noch nicht geladen / not loaded yet
        }

        // Eingeblendete Zeit, Wochentag und Datum direkt auf dem Zifferblatt, wie die Uhr sie im unteren Teil zeigt (Schriftfarbe der
        // Einstellung, Beispielwerte); ab Werk aus
        // Time, weekday and date directly on the clock face, as the clock shows them in the lower part (text colour of the
        // setting, sample values); off by default

        if (thumb && (fi.time || fi.date || fi.wday)) {
            g.setFont(&fonts::Font0);
            g.setTextSize(1);
            g.setTextColor(fi.fg);
            g.setTextDatum(lgfx::top_center);
            int ty = y + FM_T * 60 / 100;
            if (fi.time) { g.drawString("10:10", x + FM_T / 2, ty); ty += 12; }
            if (fi.wday) { g.drawString("Sa", x + FM_T / 2, ty); ty += 12; }
            if (fi.date) g.drawString("08.11.", x + FM_T / 2, ty);
        }
        String text = tftText(label);
        if (text.length() > 22) text = text.substring(0, 21) + ".";
        g.fillRect(x, y + FM_T - 11, FM_T, 11, TFT_BLACK);
        g.setFont(&fonts::Font0);
        g.setTextColor(TFT_WHITE, TFT_BLACK);
        g.setTextDatum(lgfx::top_left);
        g.drawString(text, x + 2, y + FM_T - 9);
    }

    void faceMenuDrawDots(lgfx::LovyanGFX& g, int page, int pages) {
        g.fillRect(0, 462, 480, 18, TFT_BLACK);
        for (int p = 0; p < pages && pages > 1; p++) {
            g.fillCircle(240 + (int)((p - (pages - 1) * 0.5f) * 18), 471, 4, p == page ? TFT_WHITE : TFT_DARKGREY);
        }
    }

    // Miniatur eines Presets: Zifferblatt (Mittelwert aus 3 x 3 Pixeln), darauf die Zeiger auf 10:10:30 (Rueckwaertsabbildung der
    // Zeigerbitmaps, 0xFFFF = durchsichtig)

    // Thumbnail of a preset: clock face (average of 3 x 3 pixels) with the hands at 10:10:30 on top (reverse mapping of the hand
    // bitmaps, 0xFFFF = transparent)

    bool faceMenuMakeThumb(const String& face, const String& handSet, bool showSecond, uint16_t hubColor, uint8_t hubSize, uint16_t* t, uint16_t* tmp, uint16_t* hand) {
        const String path = face.length() ? "/" + face : String("/face_default.bmp");
        if (!loadFaceBmpInto(path, tmp, CLOCK_WIDTH, CLOCK_HEIGHT)) {
            if (!ensureDefaultFace() || !loadFaceBmpInto("/face_default.bmp", tmp, CLOCK_WIDTH, CLOCK_HEIGHT)) return false;
        }
        for (int ty = 0; ty < FM_T; ty++) {
            const int y0 = ty * CLOCK_HEIGHT / FM_T;
            for (int tx = 0; tx < FM_T; tx++) {
                const int x0 = tx * CLOCK_WIDTH / FM_T;
                int r = 0, g = 0, b = 0;
                for (int dy = 0; dy < 3; dy++) {
                    for (int dx = 0; dx < 3; dx++) {
                        const uint16_t p = tmp[(y0 + dy) * CLOCK_WIDTH + x0 + dx];
                        r += (p >> 11) & 0x1F;
                        g += (p >> 5) & 0x3F;
                        b += p & 0x1F;
                    }
                }
                t[ty * FM_T + tx] = (uint16_t)(((r / 9) << 11) | ((g / 9) << 5) | (b / 9));
            }
        }
        static const char* parts[3] = { "hour", "minute", "second" };
        static const float angles[3] = { 305.0f, 60.0f, 180.0f };
        const float scale = (float)CLOCK_WIDTH / FM_T; // Quellpixel je Miniaturpixel / source pixels per thumbnail pixel

        // Drehpunkte des Zifferblatts (Sekundenfeld: Sekundenzeiger eigener Drehpunkt, liegt unter den anderen), wie renderClockFrame()
        // Pivots of the clock face (seconds subdial: second hand has its own pivot and lies below the others), like renderClockFrame()

        FacePivots facePiv;
        readFacePivots(path, facePiv);
        const HandPlan plan = makeHandPlanFrom(facePiv, showSecond);
        const int order[3] = { plan.secUnder ? 2 : 0, plan.secUnder ? 0 : 1, plan.secUnder ? 1 : 2 };
        for (int n = 0; n < 3; n++) {
            const int h = order[n];
            loadHandOrDefault(handSet, parts[h], hand);
            const bool isSec = h == 2;
            if (isSec && !showSecond) continue; // Set ohne Sekundenzeiger / set without second hand
            const bool moved = isSec ? (plan.secOwn || plan.hmCustom) : plan.hmCustom;
            const float cX = moved ? (isSec ? plan.secX : plan.hmX) + 0.5f : CLOCK_WIDTH / 2.0f;
            const float cY = moved ? (isSec ? plan.secY : plan.hmY) + 0.5f : CLOCK_HEIGHT / 2.0f;
            const float a = angles[h] * 0.0174532925f;
            const float sn = sinf(a), cs = cosf(a);
            const float pivX = (HAND_WIDTH - 1) / 2.0f;
            for (int ty = 0; ty < FM_T; ty++) {
                for (int tx = 0; tx < FM_T; tx++) {
                    const float ox = (tx + 0.5f) * scale - cX, oy = (ty + 0.5f) * scale - cY;
                    const int hx = (int)floorf(pivX + ox * cs + oy * sn + 0.5f);        // quer zum Zeiger / across the hand
                    const int hy = (int)floorf(HAND_PIVOT_Y - ox * sn + oy * cs + 0.5f); // entlang des Zeigers / along the hand
                    if (hx < 0 || hx >= HAND_WIDTH || hy < 0 || hy >= HAND_HEIGHT) continue;
                    const uint16_t p = hand[hy * HAND_WIDTH + hx];
                    if (p != 0xFFFF && p != TRANSPARENT_COLOR) t[ty * FM_T + tx] = p;
                }
            }
        }

        // Nabe (Mittelpunkt) in der Farbe und Groesse des Presets, am Drehpunkt von Stunde/Minute
        // Hub in the colour and size of the preset, at the pivot of hour/minute

        {
            const float hx = (plan.hmCustom ? plan.hmX + 0.5f : CLOCK_WIDTH / 2.0f) / scale;
            const float hy = (plan.hmCustom ? plan.hmY + 0.5f : CLOCK_HEIGHT / 2.0f) / scale;
            const float r = max(1.0f, hubSize / scale);
            for (int ty = 0; ty < FM_T; ty++) {
                for (int tx = 0; tx < FM_T; tx++) {
                    const float dx = tx + 0.5f - hx, dy = ty + 0.5f - hy;
                    if (dx * dx + dy * dy <= r * r) t[ty * FM_T + tx] = hubColor;
                }
            }
        }
        return true;
    }

    void faceMenu() {
        const int count = min(presetPickCount(), FM_MAX);
        if (count == 0) {
            DEBUG_PRINTLN("[TOUCH] No presets saved");
            return;
        }
        const int pages = (count + 8) / 9;

        uint16_t* tmp = (uint16_t*)ps_malloc((size_t)CLOCK_WIDTH * CLOCK_HEIGHT * 2);
        uint16_t* hand = (uint16_t*)ps_malloc((size_t)HAND_WIDTH * HAND_HEIGHT * 2);
        uint16_t* thumbs = (uint16_t*)ps_malloc((size_t)count * FM_T * FM_T * 2);
        if (!tmp || !hand || !thumbs) {
            DEBUG_PRINTLN("[PRESETS] No PSRAM for the thumbnails");
            if (tmp) free(tmp);
            if (hand) free(hand);
            if (thumbs) free(thumbs);
            return;
        }
        std::vector<uint8_t> state(count, 0); // 0 offen, 1 geladen, 2 Fehler / 0 pending, 1 loaded, 2 failed
        std::vector<String> names(count), faces(count), handSets(count);
        std::vector<uint8_t> active(count, 0);
        std::vector<FmInfo> infos(count);
        std::vector<uint16_t> hubColors(count, 0xF800);
        std::vector<uint8_t> hubSizes(count, 6);
        std::vector<uint8_t> showSecs(count, 1);
        int page = 0;
        for (int i = 0; i < count; i++) {
            bool a = false;
            bool sec = true;
            presetPickInfo(i, names[i], faces[i], handSets[i], a, hubColors[i], hubSizes[i], sec);
            showSecs[i] = sec;
            infos[i] = faceMenuInfo(faces[i]);
            active[i] = a;
            if (a) page = i / 9;
        }

        auto drawPage = [&]() {
            DRAW_ON_BOTH_DISPLAYS(
                tft.fillScreen(TFT_BLACK);
                for (int k = 0; k < 9 && page * 9 + k < count; k++) {
                    const int i = page * 9 + k;
                    faceMenuDrawCell(tft, k, state[i] == 1 ? thumbs + (size_t)i * FM_T * FM_T : nullptr, active[i], names[i], infos[i]);
                }
                faceMenuDrawDots(tft, page, pages);
            );
        };
        drawPage();

        bool touching = false, choose = false;
        int sx0 = 0, sy0 = 0, lx = 0, ly = 0, picked = -1;
        unsigned long idle = millis(), last = 0, openedAt = millis();
        while (!choose && millis() - idle < 30000) {
            if (backgroundWorkAllowed()) webserver.handleClient();

            // Eine fehlende Miniatur der Seite laden und gleich zeichnen / load one missing thumbnail of the page and draw it right away

            for (int k = 0; k < 9 && page * 9 + k < count; k++) {
                const int i = page * 9 + k;
                if (state[i] != 0) continue;
                uint16_t* t = thumbs + (size_t)i * FM_T * FM_T;
                if (faceMenuMakeThumb(faces[i], handSets[i], showSecs[i], hubColors[i], hubSizes[i], t, tmp, hand)) {
                    state[i] = 1;
                    DRAW_ON_BOTH_DISPLAYS(faceMenuDrawCell(tft, k, t, active[i], names[i], infos[i]););
                }
                else {
                    state[i] = 2;
                }
                break;
            }

            int x = 0, y = 0;
            const int st = lcd4TouchRead(x, y);
            if (st == 1) {
                idle = millis();
                last = millis();
                touchToScreen(x, y, lx, ly);
                if (!touching) { touching = true; sx0 = lx; sy0 = ly; }
                const int mx = lx - sx0, my = ly - sy0;
                if (pages > 1 && abs(mx) > 30 && abs(mx) > 2 * abs(my)) setSlide(mx > 0 ? mx - 30 : mx + 30);
                else if (slideCur != 0) setSlide(0);
            }
            else if (touching && (st == 2 || millis() - last >= 200)) {
                touching = false;
                const int dx = lx - sx0, dy = ly - sy0;
                if (millis() - openedAt < 400) {
                    // gleich nach dem Oeffnen ignorieren / ignore right after opening
                }
                else if (abs(dx) >= 100 && abs(dx) > 2 * abs(dy)) {
                    const int np = constrain(page + (dx < 0 ? 1 : -1), 0, pages - 1);
                    if (np != page) {
                        slideAnimate(slideCur, dx < 0 ? -480 : 480, 140);
                        page = np;
                        drawPage();
                        slideAnimate(dx < 0 ? 480 : -480, 0, 180);
                    }
                    else slideAnimate(slideCur, 0, 150);
                }
                else if (abs(dx) < 50 && abs(dy) < 50) {
                    int hit = -1;
                    for (int k = 0; k < 9 && page * 9 + k < count; k++) {
                        int cx, cy;
                        faceMenuCellPos(k, cx, cy);
                        if (sx0 >= cx - 6 && sx0 < cx + FM_T + 6 && sy0 >= cy - 6 && sy0 < cy + FM_T + 6) hit = page * 9 + k;
                    }
                    picked = hit;
                    choose = true;
                }
            }
            if (!touching && slideCur != 0) slideAnimate(slideCur, 0, 150); // nicht ausgeloester Wisch: zurueck / swipe without effect: back
            delay(5);
        }
        setSlide(0);
        free(tmp);
        free(hand);
        free(thumbs);
        lcd4TouchDone();
        if (picked >= 0 && !active[picked]) {
            DEBUG_PRINTLN("[TOUCH] Set picker: " + names[picked]);
            presetPickApply(picked);
        }
        firstRun = true;
    }

    // WLAN-Auswahl: Liste der gespeicherten WLANs (eine Zeile je Platz, das aktive gruen). Antippen einer Zeile merkt sie als
    // aktives WLAN (PK_LAST_WLAN) und startet neu - die Uhr verbindet sich dann mit diesem Netz, wie "Verbinden" in der Weboberflaeche.
    // Antippen daneben oder 30 s ohne Beruehrung schliesst.

    // WiFi picker: list of the saved WiFi networks (one row per slot, the active one green). Tapping a row stores it as the
    // active WiFi (PK_LAST_WLAN) and restarts - the clock then connects to that network, like "Connect" in the web interface.
    // Tapping beside it or 30 s without touch closes.

    void wifiMenu() {
        int slots[MAX_WLAN], n = 0;
        for (int i = 0; i < MAX_WLAN; i++) {
            if (preferences.getString(pkSsid(i).c_str(), "") != "") slots[n++] = i;
        }
        const int active = preferences.getInt(PK_LAST_WLAN, -1);
        const int ROW_H = 48, ROW_BOX = 40, TOP = 44, PER_PAGE = 9; // grosse Zeilen mit Abstand zum sicheren Tippen / large rows with gaps for safe tapping
        const int pages = (n + PER_PAGE - 1) / PER_PAGE;
        int page = 0;
        if (n == 0) {
            DEBUG_PRINTLN("[TOUCH] No saved WiFi networks");
            return;
        }
        for (int k = 0; k < n; k++) if (slots[k] == active) page = k / PER_PAGE;
        // DejaVu hat keine Umlaute: ae, oe, ue, ss statt ä, ö, ü, ß (UTF-8-Folgen ersetzen)
        // DejaVu has no umlauts: ae, oe, ue, ss instead of ä, ö, ü, ß (replace the UTF-8 sequences)

        auto plain = [](String s) {
            s.replace("\xC3\xA4", "ae"); s.replace("\xC3\xB6", "oe"); s.replace("\xC3\xBC", "ue");
            s.replace("\xC3\x84", "Ae"); s.replace("\xC3\x96", "Oe"); s.replace("\xC3\x9C", "Ue");
            s.replace("\xC3\x9F", "ss");
            return s;
        };
        auto drawPage = [&]() {
            DRAW_ON_BOTH_DISPLAYS(
                tft.fillScreen(TFT_BLACK);
                tft.setFont(&fonts::DejaVu18);
                tft.setTextSize(1);
                tft.setTextDatum(lgfx::top_left);
                tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
                tft.drawString(pages > 1 ? "WiFi  " + String(page + 1) + "/" + String(pages) : String("WiFi"), 20, 12);
                for (int r = 0; r < PER_PAGE && page * PER_PAGE + r < n; r++) {
                    const int k = page * PER_PAGE + r;
                    String ssid = plain(preferences.getString(pkSsid(slots[k]).c_str(), ""));
                    if (ssid.length() > 28) ssid = ssid.substring(0, 26) + "..";
                    const bool isActive = slots[k] == active;
                    const uint16_t bg = isActive ? 0x0320 : 0x2104;
                    tft.fillRect(10, TOP + r * ROW_H, 460, ROW_BOX, bg);
                    tft.setTextColor(TFT_WHITE, bg);
                    tft.drawString(String(slots[k] + 1) + "  " + ssid, 20, TOP + r * ROW_H + 9);
                }
                tft.setTextSize(1);
            );
        };
        drawPage();

        bool touching = false, choose = false;
        int sx0 = 0, sy0 = 0, lx = 0, ly = 0;
        unsigned long idle = millis(), last = 0, openedAt = millis();
        while (!choose && millis() - idle < 30000) {
            if (backgroundWorkAllowed()) webserver.handleClient();
            int x = 0, y = 0;
            const int st = lcd4TouchRead(x, y);
            if (st == 1) {
                idle = millis();
                last = millis();
                touchToScreen(x, y, lx, ly);
                if (!touching) { touching = true; sx0 = lx; sy0 = ly; }
            }
            else if (touching && (st == 2 || millis() - last >= 200)) {
                touching = false;
                if (millis() - openedAt < 400) continue;
                const int dx = lx - sx0, dy = ly - sy0;
                if (abs(dx) >= 100 && abs(dx) > 2 * abs(dy)) {
                    const int np = constrain(page + (dx < 0 ? 1 : -1), 0, pages - 1);
                    if (np != page) { page = np; drawPage(); }
                }
                else if (abs(dx) < 50 && abs(dy) < 50) {
                    const int rel = sy0 - TOP;
                    const int r = rel / ROW_H;
                    const int k = page * PER_PAGE + r;
                    if (rel >= 0 && rel % ROW_H < ROW_BOX && r < PER_PAGE && k < n && sx0 >= 10 && sx0 < 470) {
                        lcd4TouchDone();
                        const int slot = slots[k];
                        if (slot != active || WiFi.status() != WL_CONNECTED) {
                            DEBUG_PRINTLN("[TOUCH] WiFi picker: slot " + String(slot + 1) + " - restarting");
                            preferences.putInt(PK_LAST_WLAN, slot);
                            delay(WAIT_1s);
                            espReboot();
                        }
                    }
                    choose = true;
                }
            }
            delay(5);
        }
        lcd4TouchDone();
        firstRun = true;
    }

    // Hilfetext (Tippen rechts oben): erklaert die Touch-Bedienung, darunter WLAN-Name und IP-Adresse. Jedes weitere Antippen oder
    // 30 s ohne Beruehrung schliesst.

    // Help text (tap top right): explains the touch controls, below it the WiFi name and IP address. Any further tap or 30 s
    // without touch closes it.

    void touchHelp() {
        const bool de = currentLanguage == "de";
        const char* const lines[] = {
            de ? "Touch-Bedienung" : "Touch controls",
            de ? "Oben links: Anzeige drehen" : "Top left: rotate display",
            de ? "Oben rechts: WLAN-Daten zeigen" : "Top right: show WiFi info",
            de ? "Unten links: WLAN waehlen" : "Bottom left: choose WiFi",
            de ? "Mitte: Set waehlen" : "Centre: choose set",
            de ? "Wischen: naechstes / voriges Set" : "Swipe: next / previous set",
            de ? "Unten rechts: diese Hilfe" : "Bottom right: this help"
        };
        const String wifi = WiFi.status() == WL_CONNECTED ? tftText(WiFi.SSID()) : String("-");
        const String ip = WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : String("-");
        DRAW_ON_BOTH_DISPLAYS(
            tft.fillScreen(TFT_BLACK);
            tft.setFont(&fonts::DejaVu18);
            tft.setTextSize(1);
            tft.setTextDatum(lgfx::top_left);
            for (int i = 0; i < 7; i++) {
                tft.setTextColor(i == 0 ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
                tft.drawString(tftText(lines[i]), 24, 50 + i * 40);
            }
            tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
            tft.drawString(wifi.substring(0, 30), 24, 360);
            tft.drawString(ip, 24, 390);
            tft.setTextSize(1);
        );

        bool touching = false;
        unsigned long idle = millis(), last = 0, openedAt = millis();
        while (millis() - idle < 30000) {
            if (backgroundWorkAllowed()) webserver.handleClient();
            int x = 0, y = 0;
            const int st = lcd4TouchRead(x, y);
            if (st == 1) {
                last = millis();
                touching = true;
            }
            else if (touching && (st == 2 || millis() - last >= 200)) {
                touching = false;
                if (millis() - openedAt >= 400) break;
            }
            delay(5);
        }
        lcd4TouchDone();
        firstRun = true;
    }

    // WLAN-Daten (Tippen rechts oben): 30 s lang zeigen oder bis zum naechsten Antippen
    // WiFi credentials (tap top right): show for 30 s or until the next tap

    void wifiInfoTouch() {
        clearTFT();
        showWlanCredentials(WiFi.SSID());
        bool touching = false;
        unsigned long idle = millis(), last = 0, openedAt = millis();
        while (millis() - idle < 30000) {
            if (backgroundWorkAllowed()) webserver.handleClient();
            int x = 0, y = 0;
            const int st = lcd4TouchRead(x, y);
            if (st == 1) {
                last = millis();
                touching = true;
            }
            else if (touching && (st == 2 || millis() - last >= 200)) {
                touching = false;
                if (millis() - openedAt >= 400) break;
            }
            delay(5);
        }
        lcd4TouchDone();
        firstRun = true;
    }

    // Kurze Hinweise in den vier Ecken, 30 s nach dem Start und nach dem Drehen per Touch. Gezeichnet werden sie nicht hier, sondern
    // von der Uhr selbst in jedes volle Bild (drawCornerHints() in display.cpp) - so bleibt der Bildspeicher in Ordnung. Hier laeuft nur
    // die Zeit; beim Ein- und Ausschalten wird ein volles Bild erzwungen.

    // Short hints in the four corners, for 30 s after the start and after rotating by touch. They are not drawn here but by the
    // clock itself into every full frame (drawCornerHints() in display.cpp) - so the frame buffer stays consistent. Only the time runs
    // here; a full frame is forced when they switch on and off.

    inline unsigned long cornerHintsStart = 0; // 0 = beim naechsten Aufruf starten / 0 = start at the next call
    inline bool cornerHintsFinished = false;

    // Hinweise 30 s lang neu anzeigen (nach dem Drehen per Touch)
    // Show the hints for 30 s again (after rotating by touch)

    void cornerHintsRestart() {
        cornerHintsStart = 0;
        cornerHintsFinished = false;
    }

    void cornerHintsTick() {
        const bool want = touchEnabled && !cornerHintsFinished && tftInitialized && !lcd4StaticOn;
        static unsigned long validSince = 0; // Zeit gueltig seit / time valid since
        if (!validSince && timeinfo.tm_year >= 100) validSince = millis();
        const bool animationDone = validSince != 0 && millis() - validSince > 2000 && !handsAnimating; // erst nach der Startfahrt der Zeiger / only after the start move of the hands
        if (want && !cornerHintsStart && animationDone) cornerHintsStart = millis();
        const bool on = want && cornerHintsStart != 0 && millis() - cornerHintsStart < 30000;
        if (want && cornerHintsStart != 0 && !on) cornerHintsFinished = true;
        if (on != cornerHintsActive) {
            cornerHintsActive = on;
            clockFrameDirty[0] = true; // volles Bild mit bzw. ohne Hinweise / full frame with or without the hints
        }
    }
#endif

    void checkTouch() {
        static unsigned long lastPoll = 0, lastReinit = 0, lastExpander = 0;

        // Beeper aus und Beleuchtung wieder setzen, ein Register alle 300 ms (siehe lcd4ExpanderRefresh())
        // Buzzer off and set the backlight again, one register every 300 ms (see lcd4ExpanderRefresh())

        // Eine geaenderte Helligkeit gleich schreiben (lcd4BacklightTick(), glattes Dimmen); das Auffrischen der anderen
        // Register folgt nur, wenn der letzte Zugriff lang genug her ist.
        // Write a changed brightness right away (lcd4BacklightTick(), smooth dimming); refreshing the other registers
        // follows only if the last access is long enough ago.

        if (tftInitialized && !lcd4StaticOn) {
            lcd4BacklightTick();
            if (millis() - lastExpander > 300 && lcd4ExpanderIdle()) {
                lastExpander = millis();
                lcd4ExpanderRefresh();
            }
        }

        // Hat ein Flash-Schreibzugriff die Display-Unterbrechung gesperrt (siehe lcd4VsyncEvent()), den Treiber neu starten
        // If a flash write has locked the display interrupt (see lcd4VsyncEvent()), restart the driver

        if (lcd4StaticOn && millis() - lcd4LastPauseMs > 1500) {
            lcd4StaticOn = false;
            lcd4Reinit();
            firstRun = true; // Uhr komplett neu zeichnen / redraw the clock completely
        }
        if (!lcd4StaticOn && lcd4StallSeen && millis() - lastReinit > 2000) {
            lastReinit = millis();
            lcd4Reinit();
        }
#if BOARD_WAVESHARE_S3_LCD4
        if (!touchEnabled) {
            if (slideCur != 0) setSlide(0);
            slidePhase = 0;
            if (cornerHintsActive) { cornerHintsActive = false; clockFrameDirty[0] = true; } // Hinweise sofort entfernen / remove the hints right away
            return; // Touch-Bedienung aus: alles ignorieren / touch control off: ignore everything
        }
        if (slideStep()) return; // Wisch-Animation laeuft / swipe animation running
        cornerHintsTick();
#endif
        if (millis() - lastPoll < 25) return;
        lastPoll = millis();

#if BOARD_WAVESHARE_S3_LCD4

        // Touch-Zonen (Ecken, je ZONE_PX gross): rechts oben die WLAN-Anzeige (Status), links oben die naechste Rotation
        // (0-90-180-270-0); Wischen waagerecht wechselt das Preset. Ausgeloest wird beim Loslassen
        // (oder 200 ms ohne neue Position), danach sind RELEASE_MS lang keine neuen Beruehrungen erlaubt.

        // Touch zones (corners, ZONE_PX each): top right the WiFi info (status), top left the next rotation
        // (0-90-180-270-0); a horizontal swipe changes the preset. Triggered on release (or 200 ms without a
        // new position), after that new touches are ignored for RELEASE_MS.

        static bool touching = false, logged = false;
        static int startSx = 0, startSy = 0, lastSx = 0, lastSy = 0, startZone = 0;
        static unsigned long touchLast = 0, touchStart = 0, blockedUntil = 0;
        const int ZONE_PX = 160, SWIPE_PX = 100, MOVE_PX = 50;
        const unsigned long DEBOUNCE_MS = 50, RELEASE_MS = 400;

        // Ausgewertet wird beim Loslassen: Wisch waagerecht ab SWIPE_PX (links = naechstes, rechts = vorheriges
        // Preset), Tippen bleibt unter MOVE_PX.
        // Evaluated on release: horizontal swipe from SWIPE_PX (left = next, right = previous preset), a tap
        // stays below MOVE_PX.

        int x = 0, y = 0;
        const int state = lcd4TouchRead(x, y);
        if (state != 1) {
            if (!touching || !(state == 2 || millis() - touchLast >= 200)) return;
            touching = false;
            logged = false;
            blockedUntil = millis() + RELEASE_MS;
            const int dx = lastSx - startSx, dy = lastSy - startSy;
            if (abs(dx) >= SWIPE_PX && abs(dx) > 2 * abs(dy)) {
                DEBUG_PRINTLN("[TOUCH] Swipe " + String(dx < 0 ? "left" : "right") + " -> preset");
                startSlide(1, slideCur, dx < 0 ? -480 : 480, 160); // wechselt das Preset, wenn das Bild draussen ist / changes the preset when the picture is out
                return;
            }
            if (slideCur != 0) startSlide(4, slideCur, 0, 150);
            if (startZone == 0 || abs(dx) >= MOVE_PX || abs(dy) >= MOVE_PX || touchLast - touchStart < DEBOUNCE_MS) return;
            if (startZone == 3) {
                DEBUG_PRINTLN("[TOUCH] Tap in the middle - set picker");
                faceMenu();
                blockedUntil = millis() + RELEASE_MS;
                return;
            }
            if (startZone == 1) {
                DEBUG_PRINTLN("[TOUCH] Tap top right - WiFi credentials");
                wifiInfoTouch();
                blockedUntil = millis() + RELEASE_MS;
                return;
            }
            if (startZone == 5) {
                DEBUG_PRINTLN("[TOUCH] Tap bottom right - help text");
                touchHelp();
                blockedUntil = millis() + RELEASE_MS;
                return;
            }
            if (startZone == 4) {
                DEBUG_PRINTLN("[TOUCH] Tap bottom left - WiFi picker");
                wifiMenu();
                blockedUntil = millis() + RELEASE_MS;
                return;
            }
            if (startZone == 2) {
                DEBUG_PRINTLN("[TOUCH] Tap top left - next rotation");
                applyDisplayRotation(1, (tftRotation1 + 1) % 4);
                freeClockFaceBuffer();
                loadClockFace();
                loadHandSprites();
                cornerHintsRestart(); // Hinweise in den Ecken wieder 30 s zeigen / show the corner hints again for 30 s
                return;
            }
        }
        else {
            touchLast = millis();
            if ((long)(blockedUntil - millis()) > 0) return;
            int sx = 0, sy = 0;
            touchToScreen(x, y, sx, sy);
            if (!touching) {
                touching = true;
                touchStart = millis();
                startSx = sx;
                startSy = sy;
                startZone = 0; // 1 Status, 2 Rotation, 3 Set-Auswahl, 4 WLAN-Auswahl, 5 Hilfe / 1 status, 2 rotation, 3 set picker, 4 WiFi picker, 5 help
                if (sy < ZONE_PX && sx >= 480 - ZONE_PX) startZone = 1;
                else if (sy < ZONE_PX && sx < ZONE_PX) startZone = 2;
                else if (sy >= 480 - ZONE_PX && sx < ZONE_PX) startZone = 4;
                else if (sy >= 480 - ZONE_PX && sx >= 480 - ZONE_PX) startZone = 5;
                else startZone = 3;
            }
            lastSx = sx;
            lastSy = sy;
            const int mx = sx - startSx, my = sy - startSy;
            if (abs(mx) > 30 && abs(mx) > 2 * abs(my)) setSlide(mx > 0 ? mx - 30 : mx + 30);
            else if (slideCur != 0) setSlide(0);
            if (!logged) {
                logged = true;
                lcd4TouchLog[lcd4TouchLogCount % 8][0] = x;
                lcd4TouchLog[lcd4TouchLogCount % 8][1] = y;
                lcd4TouchLog[lcd4TouchLogCount % 8][2] = sx;
                lcd4TouchLog[lcd4TouchLogCount % 8][3] = sy;
                lcd4TouchLogCount++;
                DEBUG_PRINTLN("[TOUCH] Touch at x=" + String(x) + " y=" + String(y) + " -> screen " + String(sx) + "," + String(sy) + ", zone " + String(startZone));
            }
            return;
        }

#else

        if (!lcd4TouchPressed()) return;

#endif

        DEBUG_PRINTLN("[TOUCH] Touch - showing the WiFi info for 5 s");
        clearTFT();
        showWlanCredentials(WiFi.SSID());
        unsigned long until = millis() + WAIT_5s;
        while ((long)(until - millis()) > 0) {
            if (backgroundWorkAllowed()) webserver.handleClient();
            delay(10);
        }
        lcd4TouchDone();
        firstRun = true; // Zeiger springen sofort auf die aktuelle Zeit / the hands snap to the current time
    }

#else

    void checkTouch() {}

#endif


    // Prueft, ob der woechentliche geplante Neustart faellig ist, und loest ihn
    // bei Bedarf aus (praeventiver Neustart gegen langsame Speicherfragmentierung)

    // Checks if the weekly scheduled restart is due and triggers it
    // if needed (preventive restart against slow memory fragmentation)

    void checkWeeklyRestart() {

        // Eigene lokale Zeitstruktur statt globaler 'timeinfo' (wird von
        // Zifferblatt/DCF77/NTP genutzt). Timeout 0 statt 5000ms, sonst
        // blockierte getLocalTime() ohne gueltige Zeit 5s pro Durchlauf.

        // Own local time struct instead of the global 'timeinfo' (used by the
        // clock face/DCF77/NTP). Timeout 0 instead of 5000ms, otherwise
        // getLocalTime() blocked for 5s per pass without a valid time.

        struct tm restartTime;
        if (!getLocalTime(&restartTime, 0)) return;

        if (restartTime.tm_wday == 0 &&
            restartTime.tm_hour == 3 && restartTime.tm_min == 5 && restartTime.tm_sec == 5) {

            lastResetWeek = preferences.getInt(PK_LAST_RESET_WEEK, -1);

            char weekStr[3];
            strftime(weekStr, sizeof(weekStr), "%V", &restartTime); // ISO-Woche (01–53)
                                                                    // ISO week (01-53)
            currentWeek = atoi(weekStr);

            if (lastResetWeek == -1) {
                lastResetWeek = currentWeek;
                preferences.putInt(PK_LAST_RESET_WEEK, lastResetWeek);
            }


            if (currentWeek != lastResetWeek) {
                DEBUG_PRINTF("Woechentlicher Reboot in Woche %d\n", currentWeek);
                preferences.putInt(PK_LAST_RESET_WEEK, currentWeek);
                delay(WAIT_1s);
                DEBUG_PRINTLN("reboot now..");
                delay(WAIT_1s);

                // espReboot() statt eigenem preferences.end()+ESP.restart(): erledigt
                // Log-Eintrag, "Rebooting.."-Anzeige und preferences.end() zentral.

                // espReboot() instead of a direct preferences.end()+ESP.restart(): handles
                // the log entry, "Rebooting.." screen and preferences.end() centrally.

                espReboot();
            }
        }
    }


    void eraseAllNVS() {
        esp_err_t result = nvs_flash_erase();
        if (result == ESP_OK) {
            DEBUG_PRINTLN("Complete NVS storage erased (incl. WiFi, Preferences)");
            nvs_flash_init();  // Wichtig: Danach wieder initialisieren!
                               // Important: must re-initialize afterward!
        }
        else {
            DEBUG_PRINTF("NVS erase failed: %s\n", esp_err_to_name(result));
        }
    }


    // Kurze englische Beschriftung einer per Code zu bestaetigenden Aktion - gemeinsam fuer Display und
    // Web-Eingabeseite (/factoryReset/enterCode), damit beide denselben Text zeigen.

    // Short English label for an action confirmed by code - shared by the display and the web entry page
    // (/factoryReset/enterCode), so both show the same text.

    String factoryResetActionLabel(const String& action) {
        if (action == "all") return "Factory Reset";
        if (action == "wifi") return "Reset WiFi";
        if (action == "faces") return "Delete Faces";
        if (action == "hands") return "Delete Hands";
        if (action == "presets") return "Delete Presets";
        if (action == "wlanDeleteActive") return "Delete Active WiFi";
        if (action == "wlanOverwriteActive") return "Change Active WiFi";
        if (action == "wlanSwitchActive") return "Switch Active WiFi";
        return "Confirm Action";
    }


    // Erzeugt einen 3-stelligen Bestaetigungscode fuer `action`, merkt sich die Aktion und setzt Anzeige und
    // Fehlversuche zurueck (gezeichnet von checkFactoryResetCodePending()). Der Aufrufer setzt vorher die
    // Nutzlast (pendingWifiChangeIndex bzw. pendingWifiSsid[]/pendingWifiPass[]).

    // Generates a 3-digit confirmation code for `action`, remembers the action and resets display and
    // attempts (drawn by checkFactoryResetCodePending()). The caller sets the payload beforehand
    // (pendingWifiChangeIndex or pendingWifiSsid[]/pendingWifiPass[]).

    void requestConfirmationCode(const String& action) {

        // Noch gueltiger Code anhaengig? Dann nicht ueberschreiben und das Zeitfenster nicht verlaengern -
        // sonst koennten wiederholte Anfragen von aussen die Anzeige dauerhaft blockieren.

        // A still-valid code pending? Then do not overwrite it or extend its window - otherwise repeated
        // requests from outside could block the display permanently.

        if (!factoryResetCode.isEmpty() && millis() - factoryResetCodeStartMillis < FACTORY_RESET_CODE_TIMEOUT_MS) {
            return;
        }

        int code = random(100, 1000); // 3-stellig: 100-999
                                      // 3-digit: 100-999
        factoryResetCode = String(code);
        factoryResetCodeStartMillis = millis();
        factoryResetCodeShown = false; // loop() zeichnet ihn beim naechsten Durchlauf
                                       // loop() draws it on the next pass
        factoryResetPendingAction = action;
        factoryResetCodeAttempts = 0; // frischer Code, frisches Kontingent an Versuchen
                                      // fresh code, fresh attempt budget
    }


    // Liefert true (mit Hinweisseite), wenn schon ein anderer gueltiger Code aussteht. MUSS vor dem Setzen
    // der Nutzlast aufgerufen werden, sonst wuerde die Nutzlast der wartenden Anfrage ueberschrieben und mit
    // deren Code die FALSCHE Aktion ausgefuehrt. Bei true sofort zurueckkehren.

    // Returns true (with a hint page) if a different valid code is already pending. MUST be called before
    // setting the payload, otherwise the pending request's payload would be overwritten and its code would
    // run the WRONG action. Return immediately on true.

    bool rejectIfConfirmationPending() {
        if (!factoryResetCode.isEmpty() && millis() - factoryResetCodeStartMillis < FACTORY_RESET_CODE_TIMEOUT_MS) {
            DEBUG_PRINTLN("[SECURITY] Request from " + webserver.client().remoteIP().toString() + " ignored - a different confirmation (action: " + factoryResetPendingAction + ") is still pending");
            webserver.send(200, "text/html", simpleMessagePage(translate("Factory&nbsp;Reset"), "<p>" + translate("Another confirmation is already pending. Please complete it or wait a moment and try again") + ".</p>"));
            return true;
        }
        return false;
    }


    // Zeigt den 3-stelligen Bestaetigungscode fuer Werksreset-Aktionen und Aenderungen am aktiven WLAN -
    // Beweis physischen Zugriffs, da nur am Geraet ablesbar. Nicht blockierend: zeichnet einmal und verwirft
    // den Code nach FACTORY_RESET_CODE_TIMEOUT_MS, danach zeichnet updateClock() wieder normal.

    // Shows the 3-digit confirmation code for factory reset actions and changes to the active WiFi - proof of
    // physical access, since it can only be read on the device. Non-blocking: draws once and discards the
    // code after FACTORY_RESET_CODE_TIMEOUT_MS, then updateClock() draws normally again.

    bool checkFactoryResetCodePending() {
        if (factoryResetCode.isEmpty()) return false;

        if (millis() - factoryResetCodeStartMillis > FACTORY_RESET_CODE_TIMEOUT_MS) {
            DEBUG_PRINTLN("[SECURITY] Confirmation code expired unused (action: " + factoryResetPendingAction + ")");
            factoryResetCode = "";
            factoryResetCodeShown = false;
            factoryResetPendingAction = "";
            factoryResetCodeAttempts = 0;
            pendingWifiChangeIndex = -1;

            // Auch pendingWifiSsid[]/pendingWifiPass[] verwerfen, damit ein Klartext-Passwort eines
            // verfallenen Antrags nicht unnoetig im RAM bleibt.

            // Also discard pendingWifiSsid[]/pendingWifiPass[], so a plain-text password of an expired
            // request does not linger in RAM.

            for (int i = 0; i < MAX_WLAN; i++) {
                pendingWifiSsid[i] = "";
                pendingWifiPass[i] = "";
            }
            return false;
        }

        if (!factoryResetCodeShown) {
            factoryResetCodeShown = true;

            // Nur der nackte Code, gross und mittig; welche Aktion bestaetigt wird, zeigt die
            // Web-Eingabeseite. Die "_" davor und danach sind rein optisch - verglichen werden nur die 3
            // Ziffern.

            // Only the bare code, large and centered; the web entry page shows which action is being
            // confirmed. The "_" before and after are purely visual - only the 3 digits are compared.

            String displayCode = "_" + factoryResetCode + "_";

            // Textgroesse: groesstes ganzzahliges Vielfaches der 6x8-Schrift, bei dem der Code (5 Zeichen) in
            // 75 % der Meldungsbreite (statusWidth()) passt - skaliert so auch auf dem 160x160-Display passend.

            // Text size: the largest integer multiple of the 6x8 font at which the code (5 characters) fits
            // into 75 % of the message width (statusWidth()) - so it also scales correctly on the 160x160 display.

            showButtonMessage(TFT_YELLOW, "Code", displayCode, "", TFT_DARKGREY);
        }
        return true;
    }


    void factoryReset() {

        // Zuerst hier: eine evtl. laufende NTP-Sync-Task (siehe time_sync.cpp)
        // wuerde sonst waehrend LittleFS.format()/eraseAllNVS()/dem folgenden
        // preferences.end()/begin() weiter auf genau diese Subsysteme zugreifen.

        // First, here: an NTP sync task still running (see time_sync.cpp) would
        // otherwise keep accessing exactly these subsystems during
        // LittleFS.format()/eraseAllNVS()/the preferences.end()/begin() below.

        stopNtpSyncTaskIfRunning();

        showButtonMessage(TFT_RED, "Factory Reset", "...", "", TFT_RED);
        preferences.begin("clock", false);
        preferences.putInt(PK_FIRST_START, 0);
        preferences.end();
        delay(100);
        DEBUG_PRINTLN(">>> Factory reset started..");
        LittleFS.begin(false, "/littlefs", 10, LITTLEFS_PARTITION);
        LittleFS.format();
        LittleFS.end();
        eraseWiFiConfig();
        eraseAllNVS();
        showButtonMessage(TFT_RED, "Factory Reset", "done", "restarting", TFT_DARKGREY);
        delay(WAIT_3s);
        DEBUG_PRINTLN(">>> Restarting..");
        espReboot();
    }


    void espReboot() {

        // Zuerst eine laufende NTP-Sync-Task beenden - sie wuerde sonst nach preferences.end() ueber
        // logToFile() weiter auf Preferences zugreifen. Gilt fuer alle Neustarts (woechentlich, Web-Buttons),
        // die alle hier durchlaufen.

        // First stop a running NTP sync task - it would otherwise keep accessing Preferences via logToFile()
        // after preferences.end(). Applies to all restarts (weekly, web buttons), which all go through here.

        stopNtpSyncTaskIfRunning();

        // Generischer Log-Eintrag fuer jeden Software-Reboot, zentral hier statt
        // an jeder Aufrufstelle. Wird VOR den Display-Aktionen geloggt, damit er
        // sicher im Logfile landet, bevor der ESP neu startet.

        // Generic log entry for every software-triggered reboot, centralized here
        // instead of at each call site. Logged BEFORE the display actions, so it
        // reliably ends up in the log file before the ESP restarts.

        DEBUG_PRINTLN("[SYSTEM] Software-triggered reboot - restarting now..");

        // Gepufferte Log-Zeilen JETZT schreiben, nicht erst beim naechsten checkLogFlush() - sonst gingen sie
        // beim Neustart verloren.

        // Write buffered log lines NOW, not at the next checkLogFlush() - otherwise they would be lost on
        // restart.

        flushLogBuffer();

        // Erst HIER geschlossen (nach dem Log-Eintrag): logToFile() liest
        // PK_LOG_FILE_NUMBER selbst aus preferences - waere der Handle schon zu,
        // laende der Eintrag in der falschen Datei.

        // Closed only HERE (after the log entry): logToFile() itself reads
        // PK_LOG_FILE_NUMBER from preferences - if the handle were already
        // closed, the entry would land in the wrong file.

        preferences.end();

        // Kurze Verzoegerung, gibt dem Flash-Subsystem Luft nach dem Log-Schreiben.
        // Short delay, gives the flash subsystem breathing room after the log write.

        delay(100);

        showButtonMessage(TFT_GREEN, "Restart", "...", "", TFT_DARKGREY);
        delay(WAIT_3s);
        DRAW_ON_BOTH_DISPLAYS(
            tft.fillScreen(TFT_BLACK);
        );
        delay(100);
        ESP.restart();
    }


    // Log-Funktionen. Aktuelle Logdatei wie in logToFile() bestimmen, aber ohne Seiteneffekte - fuer Log-Tab
    // und /api/currentLog, damit beide dieselbe Datei sehen.

    // Log functions. Determine the current log file like logToFile(), but without side effects - for the Log
    // tab and /api/currentLog, so both see the same file.

    String getCurrentLogFileName() {
        uint16_t logfileNumber = preferences.getInt(PK_LOG_FILE_NUMBER, 1);
        return "/log_" + String(logfileNumber) + ".log";
    }


    void deleteAllLogFiles() {

        File root = LittleFS.open("/");
        File file = root.openNextFile();

        while (file) {
            String fileName = file.name();
            file.close();

            if (fileName.endsWith(".log")) {
                if (LittleFS.remove("/" + fileName)) {
                  //  DEBUG_PRINTLN("[LOG] Successfully deleted: " + fileName);
                }
            }
            file = root.openNextFile();
        }

        // Reset auf 1 statt 0: alle anderen Stellen (getCurrentLogFileName(),
        // logToFile()) nutzen 1 als Fallback/Rollover-Wert; 0 waere inkonsistent.

        // Reset to 1 instead of 0: every other spot (getCurrentLogFileName(),
        // logToFile()) uses 1 as the fallback/rollover value; 0 would be inconsistent.

        preferences.putInt(PK_LOG_FILE_NUMBER, 1);
    }


    // Prueft den freien Heap und schreibt bei Unterschreiten von HEAP_WARNING_THRESHOLD
    // eine Log-Zeile mit Kontext + Minimum seit Boot - an speicherhungrigen Stellen
    // aufgerufen, damit sich knapper Heap einer Codestelle zuordnen laesst.

    // Checks free heap and logs a line with context + minimum since boot
    // when it drops below HEAP_WARNING_THRESHOLD; call this at memory-heavy
    // spots to trace low heap back to a specific place in the code.

    void checkHeapWarning(const String& context) {
        size_t freeHeap = ESP.getFreeHeap();
        if (freeHeap < HEAP_WARNING_THRESHOLD) {
            DEBUG_PRINTLN("[HEAP WARNING] " + context + ": only " + String(freeHeap) +
                " bytes free (minimum since boot: " + String(ESP.getMinFreeHeap()) + " bytes)");
        }
    }


    // Schreibt eine Lognachricht in den RAM-Puffer, wenn Logging aktiviert ist -
    // der tatsaechliche Flash-Zugriff passiert erst in flushLogBuffer(), siehe dort.

    // Writes a log message into the RAM buffer if logging is enabled - the
    // actual flash access happens only in flushLogBuffer(), see there.

    void logToFile(const String& message) {
        if (!loggingEnabled) {
            return;
        }

        String trimmedMessage = message;
        trimmedMessage.trim(); // Entfernt auch \n, \r
                               // Also removes \n, \r
        if (trimmedMessage.isEmpty()) {
            return;
        }

        char timestamp[32];

        // Zeitzone hier NICHT erneut setzen: configTzTime() wuerde bei JEDEM
        // Log-Eintrag (jeder DEBUG_PRINTLN, siehe config.h) den SNTP-Client
        // neustarten und die Zeitsynchronisation staendig unterbrechen.

        // Do NOT set the timezone again here: configTzTime() would restart the
        // SNTP client on EVERY log entry (every DEBUG_PRINTLN, see config.h),
        // constantly disrupting time sync.

        unsigned long currentMillis = millis();
        unsigned long millisInSecond = currentMillis % 1000;

        // Eigene lokale Zeitstruktur wie in checkWeeklyRestart() (siehe dort). Timeout 0 und keine
        // WLAN-Bedingung - die Zeit ist auch ohne WLAN gueltig, wenn sie von RTC oder DCF77 stammt.

        // Own local time struct, same reason as in checkWeeklyRestart() (see there). Timeout 0 and no WiFi
        // condition - the time is valid without WiFi too when it comes from RTC or DCF77.

        struct tm logTime;
        if (getLocalTime(&logTime, 0)) {
            strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S", &logTime);
            snprintf(timestamp + strlen(timestamp), sizeof(timestamp) - strlen(timestamp), ".%03lu] ", millisInSecond);
        }
        else {
            snprintf(timestamp, sizeof(timestamp), "[%lu ms] ", currentMillis);
        }

        // logLineBuffer wird sowohl vom Haupt-Loop als auch von der NTP-/
        // Rocrail-Sync-Task beschrieben (siehe globals.h) - ohne den Mutex
        // waere das ein Data Race auf den internen String-Speicher.

        // logLineBuffer is written from both the main loop and the NTP/
        // Rocrail sync task (see globals.h) - without the mutex this would
        // be a data race on the String's internal storage.

        if (logBufferMutex != nullptr && xSemaphoreTake(logBufferMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            logLineBuffer += String(timestamp) + trimmedMessage + "\n";
            xSemaphoreGive(logBufferMutex);
        }
    }


    // Kopf einer Logdatei: Name der Uhr, Hostname, ESP32-Chip (Modell, Revision, Arduino-Core), Display und
    // Kompilierdatum/-zeit. Steht am Anfang jeder neuen Logdatei, damit sie auch einzeln zuordenbar ist.

    // Header of a log file: clock name, hostname, ESP32 chip (model, revision, Arduino core), display and compile
    // date/time. Sits at the start of every new log file, so each file can be attributed on its own.

    String logFileHeader() {
        return String("# uhr4 | Host: ") + hostname + " | ESP32: " + ESP.getChipModel() + " rev " + String(ESP.getChipRevision()) +
               ", " CHIP_SHORT_NAME ", Arduino-Core " ESP_ARDUINO_VERSION_STR " | Display: " + displayChoiceName(displayType, useBacklight) +
               " | Build: " + (fwBuildMarker + 14) + "\n";
    }


    // Schreibt den gesammelten Log-Puffer auf einen Rutsch (statt einer Datei-Runde je Zeile) - weniger
    // Flash-Zugriffe und Aussetzer. Aufruf regelmaessig per checkLogFlush() sowie vor
    // espReboot()/factoryReset().

    // Writes the collected log buffer in one go (instead of a file round per line) - fewer flash accesses and
    // stalls. Called regularly via checkLogFlush() and before espReboot()/factoryReset().

    void flushLogBuffer() {
        if (logBufferMutex == nullptr) return;

        String textToWrite;
        if (xSemaphoreTake(logBufferMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            textToWrite = logLineBuffer;
            logLineBuffer = "";
            xSemaphoreGive(logBufferMutex);
        }
        lastLogFlushMillis = millis();

        if (textToWrite.isEmpty()) return;

        if (!LittleFS.begin(false, "/littlefs", 10, LITTLEFS_PARTITION)) {
            if (loggingEnabled) Serial.println("[LOG] LittleFS is not mounted. Log will not be written");
            return;
        }

        size_t freeSpace = LittleFS.totalBytes() - LittleFS.usedBytesFresh();
        if (freeSpace < 15 * 1024) { // Weniger als 15 KB frei
                                     // Less than 15 KB free
            deleteAllLogFiles(); // Alle Logdateien löschen, um Platz zu schaffen
                                 // Delete all log files to free up space
            freeSpace = LittleFS.totalBytes() - LittleFS.usedBytesFresh();
            if (freeSpace < 10 * 1024) {
                if (loggingEnabled) Serial.println("[LOG] Not enough free space on LittleFS. Log will not be written");
                return;
            }
        }

        uint16_t logfileNumber = preferences.getInt(PK_LOG_FILE_NUMBER, 1);
        if (logfileNumber > 9) {
            logfileNumber = 1;
            preferences.putInt(PK_LOG_FILE_NUMBER, logfileNumber);
            preferences.putBool(PK_LOGGING_ENABLED, false);
            deleteAllLogFiles();
            loggingEnabled = preferences.getBool(PK_LOGGING_ENABLED, false);
            return; // Logging jetzt deaktiviert, kein Logfile schreiben
                    // Logging now disabled, don't write a log file
        }

        String logFileName = "/log_" + String(logfileNumber) + ".log";

        if (LittleFS.exists(logFileName)) {
            File currentLogFile = LittleFS.open(logFileName, FILE_READ);
            if (currentLogFile) {
                size_t fileSize = currentLogFile.size();
                currentLogFile.close();

                if (fileSize > 10 * 1024) {
                    logfileNumber++;
                    preferences.putInt(PK_LOG_FILE_NUMBER, logfileNumber);
                    logFileName = "/log_" + String(logfileNumber) + ".log";
                }
            }
        }

        // Neue Logdatei: Kopfzeile mit Uhr, Hostname, Chip, Display und Build voranstellen.
        // New log file: put a header line with clock, hostname, chip, display and build first.

        bool newLogFile = !LittleFS.exists(logFileName);

        File logFile = LittleFS.open(logFileName, FILE_APPEND);
        if (!logFile) {
            if (loggingEnabled) Serial.println("[LOG] Error opening log file: " + logFileName);
            return;
        }
        if (newLogFile) logFile.print(logFileHeader());
        logFile.print(textToWrite);
        logFile.close();
    }


    // Aus loop() bei jedem Tick aufgerufen (no-op, solange nichts zu tun
    // ist): leert den Log-Puffer, sobald er seit dem letzten Leeren
    // LOG_FLUSH_INTERVAL_MS alt ist oder LOG_FLUSH_MAX_BUFFER_BYTES erreicht.

    // Called from loop() on every tick (no-op as long as there's nothing to
    // do): flushes the log buffer once it's LOG_FLUSH_INTERVAL_MS old since
    // the last flush, or has reached LOG_FLUSH_MAX_BUFFER_BYTES.

    void checkLogFlush() {
        if (logBufferMutex == nullptr) return;
        if (!backgroundWorkAllowed()) return; // Flash-Zugriff stoert die Zeigerfahrt / flash access disturbs the hand move

        // Laenge nur unter dem Mutex lesen - logLineBuffer wird auch von der NTP-/Rocrail-Task beschrieben.
        // Only read the length under the mutex - logLineBuffer is also written by the NTP/Rocrail task.

        size_t bufferedBytes = 0;
        if (xSemaphoreTake(logBufferMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            bufferedBytes = logLineBuffer.length();
            xSemaphoreGive(logBufferMutex);
        }
        if (bufferedBytes == 0) return;

        if (bufferedBytes >= LOG_FLUSH_MAX_BUFFER_BYTES ||
            millis() - lastLogFlushMillis >= LOG_FLUSH_INTERVAL_MS) {
            flushLogBuffer();
        }
    }


    // Schaltet das Logging nach 24 Stunden selbst ab (und nach 9 Logdateien, siehe flushLogBuffer()), damit es nicht
    // dauernd in den Flash schreibt. Die Startzeit steht in den Preferences (PK_LOGGING_START) und ueberlebt Neustarts;
    // ohne gueltige Uhrzeit beginnt die Frist erst, wenn die Zeit da ist. Die Logdateien bleiben erhalten.

    // Switches logging off by itself after 24 hours (and after 9 log files, see flushLogBuffer()) so it does not write
    // to flash all the time. The start time is in the preferences (PK_LOGGING_START) and survives restarts; without a
    // valid time the period only starts once the time is there. The log files are kept.

    void checkLoggingExpiry() {
        static unsigned long lastCheck = 0;
        if (!loggingEnabled || millis() - lastCheck < 60000) return;
        lastCheck = millis();
        if (!timeTrusted()) return;

        int64_t now = time(nullptr);
        int64_t start = preferences.getUInt(PK_LOGGING_START, 0);
        if (start == 0 || start > now) {
            preferences.putUInt(PK_LOGGING_START, (uint32_t)now);
            return;
        }
        if (now - start >= 24 * 3600) {
            DEBUG_PRINTLN("[LOG] Logging switched off after 24 hours");
            flushLogBuffer();
            preferences.putBool(PK_LOGGING_ENABLED, false);
            preferences.putUInt(PK_LOGGING_START, 0);
            loggingEnabled = false;
        }
    }


    // eigenes trim function
    // Custom trim function

    String trim(const String& str) {
        int start = 0;
        int end = str.length() - 1;

        while (start <= end && isspace(str[start])) {
            start++;
        }

        while (end >= start && isspace(str[end])) {
            end--;
        }

        return str.substring(start, end + 1);
    }


    // Liest PK_SMOOTH_SECOND mit Migrations-Fallback auf stationMode, statt
    // die Formel an mehreren Stellen zu wiederholen (siehe presets_manager.cpp,
    // main.cpp, webserver_routes.cpp).

    // Reads PK_SMOOTH_SECOND with a migration fallback to stationMode,
    // instead of repeating the formula at several places (see
    // presets_manager.cpp, main.cpp, webserver_routes.cpp).

    bool getSmoothSecondPref(bool stationModeFallback) {
        return preferences.getBool(PK_SMOOTH_SECOND, stationModeFallback);
    }
