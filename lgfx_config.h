#pragma once
    // LovyanGFX-Konfiguration: Bus, Panel und Init-Sequenz. Pins kommen aus
    // config.h, alles liegt im Projekt, an der Bibliothek muss nichts
    // angepasst werden.

    // LovyanGFX configuration: bus, panel and init sequence. Pins come from
    // config.h, everything lives in the project, nothing has to be changed
    // inside the library.

#define LGFX_USE_V1
#include <LovyanGFX.hpp>


    // GC9A01 mit eigener, erprobter Init-Sequenz (gegenueber der LovyanGFX-
    // Vorgabe: 0x74 mit 0x85 statt 0x68, Invertierung an). Das GC9D01 hat
    // einen eigenen Treiber (Panel_UhrGC9D01 weiter unten).

    // GC9A01 with its own proven init sequence (compared to the LovyanGFX
    // default: 0x74 with 0x85 instead of 0x68, inversion on). The GC9D01 has
    // its own driver (Panel_UhrGC9D01 further below).

class Panel_UhrGC9A01 : public lgfx::Panel_GC9A01 {
protected:
    const uint8_t* getInitCommands(uint8_t listno) const override {
        static constexpr uint8_t list0[] = {
            0xEF, 0,
            0xEB, 1, 0x14,
            0xFE, 0,
            0xEF, 0,
            0xEB, 1, 0x14,
            0x84, 1, 0x40,
            0x85, 1, 0xFF,
            0x86, 1, 0xFF,
            0x87, 1, 0xFF,
            0x88, 1, 0x0A,
            0x89, 1, 0x21,
            0x8A, 1, 0x00,
            0x8B, 1, 0x80,
            0x8C, 1, 0x01,
            0x8D, 1, 0x01,
            0x8E, 1, 0xFF,
            0x8F, 1, 0xFF,
            0xB6, 2, 0x00, 0x20,
            0x36, 1, 0x48,
            0x3A, 1, 0x05,
            0x90, 4, 0x08, 0x08, 0x08, 0x08,
            0xBD, 1, 0x06,
            0xBC, 1, 0x00,
            0xFF, 3, 0x60, 0x01, 0x04,
            0xC3, 1, 0x13,
            0xC4, 1, 0x13,
            0xC9, 1, 0x22,
            0xBE, 1, 0x11,
            0xE1, 2, 0x10, 0x0E,
            0xDF, 3, 0x21, 0x0C, 0x02,
            0xF0, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
            0xF1, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
            0xF2, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
            0xF3, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
            0xED, 2, 0x1B, 0x0B,
            0xAE, 1, 0x77,
            0xCD, 1, 0x63,
            0x70, 9, 0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03,
            0xE8, 1, 0x34,
            0x62, 12, 0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70,
            0x63, 12, 0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70,
            0x64, 7, 0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07,
            0x66, 10, 0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00,
            0x67, 10, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98,
            0x74, 7, 0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00,
            0x98, 2, 0x3E, 0x07,
            0x35, 0,
            0x21, 0,
            0x11, 0 + CMD_INIT_DELAY, 120,
            0x29, 0 + CMD_INIT_DELAY, 20,
            0xFF, 0xFF, // Ende / end
        };
        return (listno == 0) ? list0 : nullptr;
    }
};


    // GC9D01 (160x160): eigener Chip mit eigener Startsequenz (dieselbe wie in uhr3) - mit der GC9A01-Sequenz
    // blieb es schwarz. Endzustand: MADCTL 0x00 (RGB, keine Hardware-Rotation - uhr4 dreht per Software),
    // keine Invertierung, Farbtiefe 0x05.

    // GC9D01 (160x160): a chip of its own with its own init sequence (the same as in uhr3) - with the GC9A01
    // sequence it stayed black. Final state: MADCTL 0x00 (RGB, no hardware rotation - uhr4 rotates in
    // software), no inversion, color depth 0x05.

class Panel_UhrGC9D01 : public lgfx::Panel_GC9A01 {
public:
    Panel_UhrGC9D01() {
        _cfg.panel_width = _cfg.memory_width = 160;
        _cfg.panel_height = _cfg.memory_height = 160;
    }

protected:
    const uint8_t* getInitCommands(uint8_t listno) const override {
        static constexpr uint8_t list0[] = {
            0xFE, 0,
            0xEF, 0,
            0x80, 1, 0xFF,
            0x81, 1, 0xFF,
            0x82, 1, 0xFF,
            0x83, 1, 0xFF,
            0x84, 1, 0xFF,
            0x85, 1, 0xFF,
            0x86, 1, 0xFF,
            0x87, 1, 0xFF,
            0x88, 1, 0xFF,
            0x89, 1, 0xFF,
            0x8A, 1, 0xFF,
            0x8B, 1, 0xFF,
            0x8C, 1, 0xFF,
            0x8D, 1, 0xFF,
            0x8E, 1, 0xFF,
            0x8F, 1, 0xFF,
            0x3A, 1, 0x05,
            0xEC, 1, 0x01,
            0x74, 7, 0x02, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x98, 1, 0x3E,
            0x99, 1, 0x3E,
            0xB5, 2, 0x0D, 0x0D,
            0x60, 4, 0x38, 0x0F, 0x79, 0x67,
            0x61, 4, 0x38, 0x11, 0x79, 0x67,
            0x64, 6, 0x38, 0x17, 0x71, 0x5F, 0x79, 0x67,
            0x65, 6, 0x38, 0x13, 0x71, 0x5B, 0x79, 0x67,
            0x6A, 2, 0x00, 0x00,
            0x6C, 7, 0x22, 0x02, 0x22, 0x02, 0x22, 0x22, 0x50,
            0x6E, 32, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0x0F, 0x0F,
                      0x0D, 0x0D, 0x0B, 0x0B, 0x09, 0x09, 0x00, 0x00,
                      0x00, 0x00, 0x0A, 0x0A, 0x0C, 0x0C, 0x0E, 0x0E,
                      0x10, 0x10, 0x00, 0x00, 0x02, 0x02, 0x04, 0x04,
            0xBF, 1, 0x01,
            0xF9, 1, 0x40,
            0x9B, 1, 0x3B,
            0x93, 3, 0x33, 0x7F, 0x00,
            0x7E, 1, 0x30,
            0x70, 6, 0x0D, 0x02, 0x08, 0x0D, 0x02, 0x08,
            0x71, 3, 0x0D, 0x02, 0x08,
            0x91, 2, 0x0E, 0x09,
            0xC3, 1, 0x19,
            0xC4, 1, 0x19,
            0xC9, 1, 0x3C,
            0xF0, 6, 0x53, 0x15, 0x0A, 0x04, 0x00, 0x3E,
            0xF2, 6, 0x53, 0x15, 0x0A, 0x04, 0x00, 0x3A,
            0xF1, 6, 0x56, 0xA8, 0x7F, 0x33, 0x34, 0x5F,
            0xF3, 6, 0x52, 0xA4, 0x7F, 0x33, 0x34, 0xDF,
            0x36, 1, 0x00,
            0x11, 0 + CMD_INIT_DELAY, 200,
            0x29, 0 + CMD_INIT_DELAY, 20,
            0xFF, 0xFF, // Ende / end
        };
        return (listno == 0) ? list0 : nullptr;
    }

    // Keine Hardware-Rotation (MADCTL immer 0, mit rgb_order = RGB 0x00)
    // No hardware rotation (MADCTL always 0, with rgb_order = RGB 0x00)

    uint8_t getMadCtl(uint8_t) const override { return 0; }

    // Farbtiefe 0x05 (wie uhr3) statt LovyanGFX-Standard 0x55
    // Color depth 0x05 (as in uhr3) instead of the LovyanGFX default 0x55

    uint8_t getColMod(uint8_t bpp) const override { return (bpp > 16) ? RGB888_3BYTE : 0x05; }
};


    // Ein Geraet fuer BEIDE Displays: CS wird manuell umgeschaltet (setCS1()/setCS2() in display.h), daher
    // pin_cs = -1. Fuenf Panel-Treiber, gewaehlt nach dem Displaytyp per selectPanel() VOR tft.init() (setup()).

    // One device for BOTH displays: CS is switched manually (setCS1()/setCS2() in display.h), hence
    // pin_cs = -1. Five panel drivers, chosen by the display type via selectPanel() BEFORE tft.init() (setup()).

#if BOARD_RGB_PANEL

    // Waveshare ESP32-S3-Touch-LCD-4 (V2/V3): ST7701 ueber RGB-Bus. Pins, Timings und Startfolge stammen aus dem
    // Demo-Code von Waveshare (ESP32-S3-Touch-LCD-4_democode_V2.zip) und dem Treibertest tools/lcd4_test. Auf dem
    // Geraet: Bild und Touch funktionieren im Treibertest. Der Touch (GT911) ist hier noch nicht eingebunden:
    // LovyanGFX meldete nichts, im Test liest Wire ihn direkt (Adresse 0x14, Register 0x814E/0x8150).

    // Waveshare ESP32-S3-Touch-LCD-4 (V2/V3): ST7701 on an RGB bus. Pins, timings and start-up sequence come from
    // Waveshare's demo code (ESP32-S3-Touch-LCD-4_democode_V2.zip) and the driver test tools/lcd4_test. Not yet
    // device: image and touch work in the driver test. The touch (GT911) is not integrated here yet: LovyanGFX
    // reported nothing, in the test Wire reads it directly (address 0x14, registers 0x814E/0x8150).

#include <Wire.h>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>

#if BOARD_WAVESHARE_S3_LCD4

    // Port-Expander der V3: Waveshare-Chip CH32V003 (nicht TCA9554) mit eigenen Registern: Richtung (1 = Ausgang),
    // Ausgang, PWM-Helligkeit der Beleuchtung (1..247). EXIO6 ist der Beeper (high = an).

    // Port expander of the V3: Waveshare chip CH32V003 (not TCA9554) with its own registers: direction (1 = output),
    // output, PWM brightness of the backlight (1..247). EXIO6 is the buzzer (high = on).

constexpr uint8_t LCD4_EXPANDER_ADDR = 0x24;
constexpr uint8_t LCD4_EXP_REG_DIR = 0x02, LCD4_EXP_REG_OUT = 0x03, LCD4_EXP_REG_PWM = 0x05;

    // Beleuchtung per PWM-Register: INVERTIERT, ein kleiner Wert ist hell, 247 fast dunkel (Waveshare gibt 1..247 als
    // sicheren Bereich an). 10 = hell. ACHTUNG: Schon ein Lesezugriff auf ein Expander-Register (Register-Nummer ohne
    // Daten schreiben) ueberschreibt es mit dem zuletzt gesendeten Wert - Register nie zur Kontrolle auslesen.

    // Backlight via the PWM register: INVERTED, a small value is bright, 247 almost dark (Waveshare gives 1..247 as the
    // safe range). 10 = bright. CAUTION: even a read access to an expander register (writing the register number without
    // data) overwrites it with the last value sent - never read registers back to check them.

constexpr uint8_t LCD4_BACKLIGHT_PWM = 10;
constexpr uint8_t LCD4_EXIO_BL_EN = 1, LCD4_EXIO_LCD_RST = 2;     // Beleuchtung an/aus, LCD-Reset
constexpr uint8_t LCD4_EXIO_TP_RST = 0, LCD4_EXIO_SD_CS = 3;      // Touch-Reset (hier nur HIGH), SD-Chip-Select
constexpr uint8_t LCD4_EXIO_BUZZER = 6;                           // Beeper (high = an) / buzzer (high = on)
constexpr int LCD4_PIN_SPI_SCK = 2, LCD4_PIN_SPI_SDA = 1;         // 3-Draht-SPI zum ST7701 (CS = CS_1) / 3-wire SPI to the ST7701

#endif

// Zeitaufteilung der Bilder fuer /api/lcdstats ([0] Teilbild, [1] volles Bild: Anzahl, Vorbereitung, Zwischenbild, Zeiger, Senden in us)
// Frame time split for /api/lcdstats ([0] partial frame, [1] full frame: count, preparation, composite, hands, send in us)
inline volatile uint32_t lcd4Prof[2][5] = {};
inline volatile uint32_t lcd4ProfPixels = 0;
inline volatile uint32_t lcd4Why[16] = {}; // warum ein Frame voll statt Teilbild war (renderClockFrame()) / why a frame was full instead of partial (renderClockFrame())

    // Pixeltakt des RGB-Busses: 12 MHz = 42 Bilder/s. Das Nachfuellen der Bounce-Puffer aus dem PSRAM schafft bei den
    // 16 MHz des Waveshare-Demos (32 MB/s) nur knapp - bei jeder weiteren Speicherlast fehlten Zeilen und das Bild rollte.

    // Pixel clock of the RGB bus: 12 MHz = 42 frames/s. Refilling the bounce buffers from PSRAM only just manages the
    // 16 MHz of the Waveshare demo (32 MB/s) - with any further memory load lines were missing and the picture rolled.

constexpr uint32_t LCD4_PCLK_HZ = 12000000;

    // Zeilen je Bounce-Puffer (zwei Puffer, je 480 x Zeilen x 2 Byte internes RAM): so lange darf die CPU den
    // Speicherzugriff blockieren, ohne dass das Bild zuckt.

    // Lines per bounce buffer (two buffers, each 480 x lines x 2 bytes of internal RAM): the CPU may block memory
    // access this long without the image twitching.

constexpr int LCD4_BOUNCE_LINES = 16;

    // Pause nach jedem Schreibblock bei Firmware-Update und Sicherung einspielen. Seit das Display dabei auf eine feste
    // Farbe schaltet (lcd4StaticOn) nicht mehr noetig, daher 0; der Wert bleibt zum Einstellen.

    // Pause after every write block when updating the firmware or restoring a backup. Not needed any more since the display
    // switches to a fixed colour then (lcd4StaticOn), hence 0; the value stays for tuning.

constexpr uint32_t LCD4_FLASH_PAUSE_MS = 0;

#if BOARD_WAVESHARE_S3_LCD4

    // ST7701-Startfolge als Liste: Befehl, Anzahl Daten (Bit 7 = danach 120 ms warten), Daten; Ende mit Anzahl 0xFF.
    // ST7701 start-up sequence as a list: command, number of data bytes (bit 7 = wait 120 ms afterwards), data; the
    // end is count 0xFF.

static const uint8_t LCD4_ST7701_INIT[] = {
    0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x10,
    0xC0, 2, 0x3B, 0x00,
    0xC1, 2, 0x0D, 0x02,
    0xC2, 2, 0x31, 0x05,
    0xCD, 1, 0x08,
    0xB0, 16, 0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08, 0x07, 0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18,
    0xB1, 16, 0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08, 0x08, 0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18,
    0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x11,
    0xB0, 1, 0x60,
    0xB1, 1, 0x32,
    0xB2, 1, 0x07,
    0xB3, 1, 0x80,
    0xB5, 1, 0x49,
    0xB7, 1, 0x85,
    0xB8, 1, 0x21,
    0xC1, 1, 0x78,
    0xC2, 1, 0x78,
    0xE0, 3, 0x00, 0x1B, 0x02,
    0xE1, 11, 0x08, 0xA0, 0x00, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x44, 0x44,
    0xE2, 12, 0x11, 0x11, 0x44, 0x44, 0xED, 0xA0, 0x00, 0x00, 0xEC, 0xA0, 0x00, 0x00,
    0xE3, 4, 0x00, 0x00, 0x11, 0x11,
    0xE4, 2, 0x44, 0x44,
    0xE5, 16, 0x0A, 0xE9, 0xD8, 0xA0, 0x0C, 0xEB, 0xD8, 0xA0, 0x0E, 0xED, 0xD8, 0xA0, 0x10, 0xEF, 0xD8, 0xA0,
    0xE6, 4, 0x00, 0x00, 0x11, 0x11,
    0xE7, 2, 0x44, 0x44,
    0xE8, 16, 0x09, 0xE8, 0xD8, 0xA0, 0x0B, 0xEA, 0xD8, 0xA0, 0x0D, 0xEC, 0xD8, 0xA0, 0x0F, 0xEE, 0xD8, 0xA0,
    0xEB, 7, 0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40,
    0xEC, 2, 0x3C, 0x00,
    0xED, 16, 0xAB, 0x89, 0x76, 0x54, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x45, 0x67, 0x98, 0xBA,
    0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x13,
    0xE5, 1, 0xE4,
    0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x00,
    0x21, 0,
    0x3A, 1, 0x60,
    0x11, 0x80,
    0x29, 0x80,
    0x00, 0xFF
};

inline void lcd4SpiBit(bool bit) {
    digitalWrite(LCD4_PIN_SPI_SDA, bit);
    digitalWrite(LCD4_PIN_SPI_SCK, HIGH);
    digitalWrite(LCD4_PIN_SPI_SCK, LOW);
}

    // 9-Bit-SPI von Hand: erstes Bit 0 = Befehl, 1 = Daten
    // 9-bit SPI by hand: first bit 0 = command, 1 = data

inline void lcd4SpiByte(bool isData, uint8_t b) {
    lcd4SpiBit(isData);
    for (uint8_t m = 0x80; m; m >>= 1) lcd4SpiBit(b & m);
}

inline void lcd4ExpanderWrite(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(LCD4_EXPANDER_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

    // Ganz am Anfang von setup(): Beeper ausschalten. Der Expander startet mit allen Ausgaengen high, der Beeper
    // (EXIO6) piept sonst, bis resetPanels() laeuft. Startet auch Wire (I2C 15/7).

    // Very first thing in setup(): switch the buzzer off. The expander starts with all outputs high, so the buzzer
    // (EXIO6) would sound until resetPanels() runs. Also starts Wire (I2C 15/7).

inline bool lcd4ExpanderSafeWrite(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(LCD4_EXPANDER_ADDR);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

    // Beeper aus, mehrfach: erst das Ausgangsregister (Beeper low), dann die Richtung (Pins werden Ausgang) - so gibt der Pin nie
    // kurz "an" aus. Nach dem Einschalten braucht der Expander evtl. einen Moment, einzelne Zugriffe gehen auch verloren, daher
    // bis zu 20 Versuche und danach zwei Wiederholungen.

    // Buzzer off, several times: first the output register (buzzer low), then the direction (pins become outputs) - so the pin
    // never briefly outputs "on". After power-up the expander may need a moment and single accesses get lost, hence up to 20
    // attempts and two repetitions afterwards.

inline void lcd4BuzzerOff() {
    Wire.begin(SDA_PIN, SCL_PIN, 400000);
    const uint8_t outAll = (1 << LCD4_EXIO_BL_EN) | (1 << LCD4_EXIO_LCD_RST) | (1 << LCD4_EXIO_TP_RST) | (1 << LCD4_EXIO_SD_CS);
    for (int tries = 0; tries < 20; tries++) {
        bool ok = lcd4ExpanderSafeWrite(LCD4_EXP_REG_OUT, outAll);
        ok = lcd4ExpanderSafeWrite(LCD4_EXP_REG_DIR, 0xFF) && ok;
        if (ok) break;
        delay(10);
    }
    for (int rep = 0; rep < 2; rep++) {
        delay(5);
        lcd4ExpanderSafeWrite(LCD4_EXP_REG_OUT, outAll);
        lcd4ExpanderSafeWrite(LCD4_EXP_REG_DIR, 0xFF);
    }
}

    // Beeper kurz einschalten (beim Start): Ausgangsregister mit gesetztem Beeper-Bit, nach ms wieder aus - mit Abstand zwischen
    // den Zugriffen, der Expander verwechselt sonst Register (siehe lcd4ExpanderRefresh()). Blockiert ms Millisekunden.

    // Switch the buzzer on briefly (at start): output register with the buzzer bit set, off again after ms - with spacing between
    // the accesses, otherwise the expander mixes up registers (see lcd4ExpanderRefresh()). Blocks for ms milliseconds.

inline volatile uint8_t lcd4BeepWrites = 0, lcd4BeepAcked = 0; // Zugriffe und davon bestaetigte (fuer /api/lcdstats) / accesses and how many were acknowledged (for /api/lcdstats)

inline void lcd4Beep(uint16_t ms) {
    const uint8_t outAll = (1 << LCD4_EXIO_BL_EN) | (1 << LCD4_EXIO_LCD_RST) | (1 << LCD4_EXIO_TP_RST) | (1 << LCD4_EXIO_SD_CS);
    const uint8_t on = outAll | (1 << LCD4_EXIO_BUZZER);

    // Dauer in Schritten zu 50 ms: Beeper-Bit je Schritt erneut setzen, falls ein Zugriff verloren geht oder der Expander das Register zuruecksetzt
    // Duration in steps of 50 ms: set the buzzer bit again each step in case an access is lost or the expander resets the register

    delay(10);
    for (uint16_t t = 0; t < ms; t += 50) {
        lcd4BeepWrites++;
        if (lcd4ExpanderSafeWrite(LCD4_EXP_REG_OUT, on)) lcd4BeepAcked++;
        delay(50);
    }
    for (int rep = 0; rep < 3; rep++) {
        lcd4ExpanderSafeWrite(LCD4_EXP_REG_OUT, outAll);
        delay(5);
    }
}

    // Versuch fuer die Fehlersuche (/api/beeptest): beliebigen Expander-Ausgang 4..7 ms lang auf high, wahlweise als Ton (rund 2 kHz
    // Rechteck, falls der Beeper passiv ist) statt als Dauerpegel.

    // Experiment for troubleshooting (/api/beeptest): any expander output 4..7 high for ms, optionally as a tone (about 2 kHz
    // square wave, in case the buzzer is passive) instead of a steady level.

inline void lcd4BeepPin(uint8_t pin, uint16_t ms, bool tone) {
    const uint8_t outAll = (1 << LCD4_EXIO_BL_EN) | (1 << LCD4_EXIO_LCD_RST) | (1 << LCD4_EXIO_TP_RST) | (1 << LCD4_EXIO_SD_CS);
    const uint8_t on = outAll | (1 << pin);
    if (!tone) {
        for (uint16_t t = 0; t < ms; t += 50) {
            lcd4ExpanderSafeWrite(LCD4_EXP_REG_OUT, on);
            delay(50);
        }
    }
    else {
        const uint32_t end = millis() + ms;
        bool level = false;
        while ((int32_t)(end - millis()) > 0) {
            level = !level;
            lcd4ExpanderSafeWrite(LCD4_EXP_REG_OUT, level ? on : outAll);
            delayMicroseconds(250);
        }
    }
    for (int rep = 0; rep < 3; rep++) {
        lcd4ExpanderSafeWrite(LCD4_EXP_REG_OUT, outAll);
        delay(5);
    }
}

    // Im Betrieb erneuern: je Aufruf EIN Register, reihum Ausgaenge (Beeper aus), Richtung und Helligkeit. Zwei Zugriffe dicht
    // hintereinander verwechselt der Expander - der Wert landet dann im falschen Register (Beeper an, Display dunkel) -, daher
    // ruft checkTouch() das nur alle 300 ms auf. Faengt eine Stoerung oder einen kurzen Spannungseinbruch ab.
    // Nur schreiben, nie lesen (ein Lesezugriff veraendert die Register, siehe oben).

    // Renew during operation: ONE register per call, in turn outputs (buzzer off), direction and brightness. The expander mixes
    // up two accesses close together - the value then lands in the wrong register (buzzer on, display dark) - so checkTouch()
    // calls this only every 300 ms. Catches a disturbance or a short voltage dip.
    // (a read changes the registers, see above).


inline volatile uint8_t lcd4BacklightPwm = LCD4_BACKLIGHT_PWM; // aktueller PWM-Wert (Register 0x05) / current PWM value (register 0x05)

    // Helligkeit 0-255 (255 = hell) in den umgekehrten Registerwert 247..LCD4_BACKLIGHT_PWM umrechnen; geschrieben
    // wird der Wert vom naechsten lcd4ExpanderRefresh() (I2C-Schreibzugriffe brauchen Abstand).

    // Convert brightness 0-255 (255 = bright) to the inverted register value 247..LCD4_BACKLIGHT_PWM; the value is
    // written by the next lcd4ExpanderRefresh() (I2C writes need spacing).

inline void lcd4SetBacklight(uint8_t level) {
    lcd4BacklightPwm = (uint8_t)(247 - ((247 - LCD4_BACKLIGHT_PWM) * (int)level + 127) / 255);
}

    // Zuletzt an den Expander geschriebener PWM-Wert und Zeitpunkt des letzten Zugriffs. Aendert sich die Helligkeit,
    // schreibt lcd4BacklightTick() den neuen Wert gleich (statt erst beim naechsten Refresh, der das PWM-Register nur
    // alle 600 ms erreicht) - mit mindestens LCD4_EXPANDER_SPACING_MS Abstand zu jedem anderen Zugriff, denn zwei
    // Zugriffe dicht hintereinander verwechselt der Expander. Das Hoch- und Runterdimmen laeuft so in feinen Schritten.

    // PWM value last written to the expander and time of the last access. If the brightness changes,
    // lcd4BacklightTick() writes the new value right away (instead of at the next refresh, which reaches the PWM
    // register only every 600 ms) - with at least LCD4_EXPANDER_SPACING_MS to any other access, because the expander
    // mixes up two accesses close together. Dimming up and down thus runs in fine steps.

inline volatile uint8_t lcd4PwmWritten = LCD4_BACKLIGHT_PWM;
inline volatile unsigned long lcd4LastExpanderMs = 0;
constexpr unsigned long LCD4_EXPANDER_SPACING_MS = 50; // wie die Schritte beim Beeper / like the steps of the buzzer

inline bool lcd4ExpanderIdle() {
    return millis() - lcd4LastExpanderMs >= LCD4_EXPANDER_SPACING_MS;
}

inline void lcd4ExpanderRefresh() {
    static uint8_t next = 0;
    const uint8_t outAll = (1 << LCD4_EXIO_BL_EN) | (1 << LCD4_EXIO_LCD_RST) | (1 << LCD4_EXIO_TP_RST) | (1 << LCD4_EXIO_SD_CS);
    lcd4LastExpanderMs = millis();
    if (next == 1) lcd4ExpanderSafeWrite(LCD4_EXP_REG_OUT, outAll);
    else if (next == 3) lcd4ExpanderSafeWrite(LCD4_EXP_REG_DIR, 0xFF);
    else {
        const uint8_t pwm = lcd4BacklightPwm;
        lcd4ExpanderSafeWrite(LCD4_EXP_REG_PWM, pwm);
        lcd4PwmWritten = pwm;
    }
    next = (next + 1) % 4;
}

    // Geaenderten PWM-Wert gleich schreiben, wenn der letzte Zugriff lang genug her ist (aus checkTouch(), jeder Durchlauf)
    // Write a changed PWM value right away if the last access is long enough ago (from checkTouch(), every pass)

inline void lcd4BacklightTick() {
    const uint8_t pwm = lcd4BacklightPwm;
    if (pwm == lcd4PwmWritten || !lcd4ExpanderIdle()) return;
    lcd4LastExpanderMs = millis();
    lcd4ExpanderSafeWrite(LCD4_EXP_REG_PWM, pwm);
    lcd4PwmWritten = pwm;
}

    // Vor tft.init(): LCD-Reset und Beleuchtung ueber den Port-Expander, danach die ST7701-Startfolge. Wire muss
    // laufen (setup()). Die RGB-Signale starten erst mit tft.init().

    // Before tft.init(): LCD reset and backlight via the port expander, then the ST7701 start-up sequence. Wire
    // must be running (setup()). The RGB signals only start with tft.init().

inline void lcd4PanelInit() {
    lcd4ExpanderWrite(LCD4_EXP_REG_DIR, 0xFF);      // alle Pins als Ausgang (sonst schwebt der Beeper-Pin hoch) / all pins as outputs (otherwise the buzzer pin floats high)
    uint8_t out = (1 << LCD4_EXIO_BL_EN) | (1 << LCD4_EXIO_TP_RST) | (1 << LCD4_EXIO_SD_CS); // LCD-Reset aktiv, Beeper aus / LCD reset active, buzzer off
    lcd4ExpanderWrite(LCD4_EXP_REG_OUT, out);
    delay(20);
    out |= (1 << LCD4_EXIO_LCD_RST);
    lcd4ExpanderWrite(LCD4_EXP_REG_OUT, out);
    lcd4ExpanderWrite(LCD4_EXP_REG_PWM, lcd4BacklightPwm);
    lcd4PwmWritten = lcd4BacklightPwm;
    lcd4LastExpanderMs = millis();
    delay(120);

    pinMode(CS_1, OUTPUT);
    pinMode(LCD4_PIN_SPI_SCK, OUTPUT);
    pinMode(LCD4_PIN_SPI_SDA, OUTPUT);
    digitalWrite(CS_1, HIGH);
    digitalWrite(LCD4_PIN_SPI_SCK, LOW);
    digitalWrite(LCD4_PIN_SPI_SDA, LOW);

    digitalWrite(CS_1, LOW); // Software-Reset / software reset
    lcd4SpiByte(false, 0x01);
    digitalWrite(CS_1, HIGH);
    delay(120);

    const uint8_t* p = LCD4_ST7701_INIT;
    digitalWrite(CS_1, LOW);
    while (p[1] != 0xFF) {
        uint8_t cmd = p[0], n = p[1] & 0x7F;
        bool waitAfter = p[1] & 0x80;
        lcd4SpiByte(false, cmd);
        for (uint8_t i = 0; i < n; i++) lcd4SpiByte(true, p[2 + i]);
        p += 2 + n;
        if (waitAfter) {
            digitalWrite(CS_1, HIGH);
            delay(120);
            digitalWrite(CS_1, LOW);
        }
    }
    digitalWrite(CS_1, HIGH);
}

#else

#include "lgfx_qualia.h" // Adafruit Qualia: Startfolge, Expander, Touch

#endif

    // RGB-Bus ueber den ESP-IDF-Treiber (esp_lcd) im Bounce-Betrieb ohne eigenen Bildspeicher des Treibers: Das Bild liegt
    // in einem eigenen Speicher im PSRAM (LovyanGFX zeichnet dorthin), der DMA liest aus zwei kleinen Puffern im internen RAM,
    // die lcd4BounceEmpty() daraus nachfuellt. Flash-Zugriffe (WLAN, Dateien, Einstellungen) sperren den Cache und damit
    // auch die Unterbrechungen des Displays; dabei verpasst der Treiber Pufferwechsel und das Bild bleibt dauerhaft
    // verschoben. Ein Neustart des Treibers (lcd4Reinit()) behebt das, ohne den eigenen Bildspeicher zu beruehren; ihn
    // loest lcd4VsyncEvent() aus, sobald ein Bild ungewoehnlich lang gedauert hat. Pins, Takt und Zeiten kommen aus der
    // Bus_RGB-Konfiguration.

    // RGB bus via the ESP-IDF driver (esp_lcd) in bounce mode without a frame buffer of the driver's own: the image lives in
    // a memory of its own in PSRAM (LovyanGFX draws there), the DMA reads from two small buffers in internal RAM which
    // lcd4BounceEmpty() refills from it. Flash accesses (WiFi, files, settings) lock the cache and with it the display's
    // interrupts; the driver then misses buffer switches and the picture stays shifted permanently. Restarting the driver
    // (lcd4Reinit()) fixes that without touching the own frame buffer; lcd4VsyncEvent() triggers it as soon as a frame took
    // unusually long. Pins, clock and timings come from the Bus_RGB configuration.

#include <esp_timer.h>
#include <atomic>

inline esp_lcd_panel_handle_t lcd4PanelHandle = nullptr;
inline uint8_t* lcd4Fb = nullptr;                       // angezeigter Bildspeicher im PSRAM (liest die Bounce-Unterbrechung) / displayed frame buffer in PSRAM (read by the bounce interrupt)

    // Doppelpuffer: Das Panel liest immer einen ganzen Bildspeicher (lcd4Fb), LovyanGFX schreibt in den anderen (lcd4DrawFb). Zu Beginn
    // eines Bildes tauscht die Unterbrechung beide, wenn die Uhr ein fertiges Bild gemeldet hat (display()) und nicht gerade schreibt.
    // Dann sieht das Panel nie ein halb altes, halb neues Bild (geteilte Zeiger). Danach holt sich der Hauptlauf vor dem naechsten
    // Schreiben die geaenderten Rechtecke aus dem angezeigten Speicher, damit beide wieder gleich sind (lcd4ResyncDraw()).

    // Double buffer: the panel always reads a whole frame buffer (lcd4Fb), LovyanGFX writes into the other one (lcd4DrawFb). At the
    // start of a frame the interrupt swaps both if the clock has reported a finished frame (display()) and is not writing right now.
    // The panel then never sees a half old, half new picture (split hands). Afterwards the main loop fetches the changed rectangles
    // from the displayed buffer before the next write, so that both are equal again (lcd4ResyncDraw()).

inline uint8_t* lcd4FbBuf[2] = { nullptr, nullptr };
inline uint8_t* lcd4DrawFb = nullptr;                    // Bildspeicher, in den geschrieben wird / frame buffer being written into
inline uint8_t* lcd4FrameFb = nullptr;                   // Bildspeicher des gerade ausgegebenen Bildes / frame buffer of the frame being output
inline std::atomic<bool> lcd4Open{ false };              // seit dem letzten display() wurde geschrieben / written since the last display()
inline std::atomic<bool> lcd4PresentPending{ false };    // fertiges Bild wartet auf den Tausch / finished frame waits for the swap
inline std::atomic<bool> lcd4Swapped{ false };           // getauscht, der Hauptlauf muss noch angleichen / swapped, the main loop still has to equalise
inline volatile int64_t lcd4OpenSinceUs = 0;
inline volatile uint32_t lcd4Swaps = 0;
struct Lcd4Rect { int16_t l = INT16_MAX, t = INT16_MAX, r = -1, b = -1; };
inline Lcd4Rect lcd4Dirty, lcd4SwapDirty;                // geaendert seit dem letzten Tausch / changed since the last swap; Rechteck des letzten Tauschs / rectangle of the last swap
inline portMUX_TYPE lcd4Mux = portMUX_INITIALIZER_UNLOCKED;
inline esp_lcd_rgb_panel_config_t lcd4PanelCfg = {};    // einmal aufgebaut, fuer jeden Neustart wiederverwendet / built once, reused for every restart
inline int lcd4BounceLines = LCD4_BOUNCE_LINES;
inline uint32_t lcd4FramePeriodUs = 0;
inline volatile int64_t lcd4VsyncLastUs = 0;
inline volatile int64_t lcd4BounceLastUs = 0;
inline int64_t lcd4LineNs = 0;                         // Dauer einer Zeile / duration of one line
inline int64_t lcd4BlankUs = 0;                        // Austastung am Bildende (Vsync-Pulse + Back-/Front-Porch) / blanking at the frame end (vsync pulse + back/front porch)
inline int lcd4LastEndPx = 0;                          // Ende (Pixel) des zuletzt gefuellten Bounce-Puffers / end (pixels) of the bounce buffer filled last
inline volatile int64_t lcd4IgnoreUntilUs = 0;         // nach dem Treiberstart: Auffaelligkeiten nicht werten / after the driver start: do not count irregularities
inline volatile uint32_t lcd4PosErrors = 0;            // Bounce-Puffer nicht lueckenlos aufeinander folgend / bounce buffers not following each other without a gap
inline volatile uint32_t lcd4Anom[4] = {};             // letzte Auffaelligkeit: Art (1 Luecke, 2 Position, 3 Bildlaenge), Wert, Position, Zeit ms / last irregularity: kind (1 gap, 2 position, 3 frame length), value, position, time ms
inline int64_t lcd4BounceGapUs = 0;                    // Abstand der Bounce-Unterbrechungen, ab dem eine verpasst wurde / interval of the bounce interrupts above which one was missed
inline volatile bool lcd4StallSeen = false;
inline volatile bool lcd4StaticOn = false;             // Update/Sicherung laeuft: feste Farbe statt Bild / update or backup running: fixed colour instead of the image
inline volatile int64_t lcd4TextStartUs = 0;           // Beginn der festen Farbe: der Text steht nur kurz / start of the fixed colour: the text stays only briefly
inline volatile uint32_t lcd4LastPauseMs = 0;          // Zeitpunkt des letzten Schreibblocks / time of the last write block
inline volatile uint32_t lcd4StallCount = 0, lcd4ReinitCount = 0, lcd4MaxGapUs = 0, lcd4VsyncStalls = 0; // Zaehler fuer /api/lcdstats / counters for /api/lcdstats

    // Textkachel fuer die feste Farbe waehrend Update/Sicherung: 480 x 32 Punkte, 1 Bit je Punkt, im internen RAM (rund 2 KB),
    // ueber den ganzen Bildschirm wiederholt. 32 Zeilen = zwei Bounce-Puffer: Spielt der DMA bei gesperrter Unterbrechung einen
    // alten Puffer noch einmal ab, zeigt er dasselbe wie der richtige - der Text kann nicht springen oder flimmern.

    // Text tile for the fixed colour during update/backup: 480 x 32 dots, 1 bit per dot, in internal RAM (about 2 KB), repeated
    // over the whole screen. 32 lines = two bounce buffers: if the DMA replays an old buffer while the interrupt is locked it
    // shows the same as the right one - the text cannot jump or flicker.

inline lgfx::LGFX_Sprite lcd4Banner;
inline const uint8_t* lcd4BannerBuf = nullptr;
inline volatile bool lcd4BannerRot180 = false;         // Kachel um 180 Grad gedreht lesen / read the tile rotated by 180 degrees
constexpr int LCD4_TILE_H = 32;

    // Text und Fortschritt (aus line2, zum Beispiel "45 %") in die Kachel: oben eine Textzeile, unten ein 4 Punkte hoher Balken
    // Text and progress (from line2, for example "45 %") into the tile: a text line on top, a bar 4 dots high at the bottom

// Textstreifen leeren: die feste Farbe bleibt ohne jeden Text / clear the text banner: the fixed colour stays without any text
inline void lcd4StaticTextClear() {
    if (lcd4BannerBuf) lcd4Banner.fillSprite(0);
}

inline void lcd4StaticText(const String& line1, const String& line2, bool rot180 = false) {
    if (!lcd4BannerBuf) {
        lcd4Banner.setPsram(false);
        lcd4Banner.setColorDepth(lgfx::palette_1bit);
        if (!lcd4Banner.createSprite(480, LCD4_TILE_H)) return;
        lcd4Banner.setPaletteColor(0, 0, 0, 0);
        lcd4Banner.setPaletteColor(1, 255, 255, 255);
        lcd4BannerBuf = (const uint8_t*)lcd4Banner.getBuffer();
    }
    lcd4BannerRot180 = rot180;
    lcd4Banner.fillSprite(0);
    lcd4Banner.setFont(&fonts::FreeSansBold9pt7b);
    lcd4Banner.setTextColor(1, 0);
    lcd4Banner.setTextDatum(lgfx::top_center);
    lcd4Banner.drawString(line2.length() ? line1 + " " + line2 : line1, 240, 1);
    int pct = line2.toInt();
    if (pct > 0) lcd4Banner.fillRect(0, LCD4_TILE_H - 4, 480 * min(pct, 100) / 100, 4, 1);
}

    // Bounce-Puffer aus dem eigenen Bildspeicher fuellen (laeuft in der Display-Unterbrechung). Die Pause ums Bildende (Austastung,
    // rund 2,7 ms) ist normal und zaehlt nicht. Kommt die Unterbrechung
    // spaeter als 2 Pufferzeiten nach der vorigen, wurde ein Pufferwechsel verpasst: Der Treiber zaehlt dann falsch und das
    // Bild bleibt verschoben - Neustart vormerken (lcd4Reinit()).

    // Fill the bounce buffer from the own frame buffer (runs in the display interrupt). The pause around the frame end
    // (blanking, about 2.7 ms) is normal and does not count. If the interrupt arrives later than
    // 2 buffer times after the previous one, a buffer switch was missed: the driver then counts wrongly and the picture
    // stays shifted - note a restart (lcd4Reinit()).

inline bool lcd4BounceEmpty(esp_lcd_panel_handle_t, void* buf, int posPx, int lenBytes, void*) {
    if (lcd4StaticOn) {

        // Feste Farbe (dunkelblau, RGB565 byteweise vertauscht: Bytes 0x00 0x08): kein Zugriff auf das PSRAM, bleibt auch
        // bei gesperrter Unterbrechung eine ruhige Flaeche
        // Fixed colour (dark blue, RGB565 byte-swapped: bytes 0x00 0x08): no PSRAM access, stays a calm area even when
        // the interrupt is locked

        uint16_t* d = (uint16_t*)buf;
        int y0 = posPx / 480, lines = lenBytes / 960;
        bool rot = lcd4BannerRot180;
        for (int l = 0; l < lines; l++, d += 480) {
            if (lcd4BannerBuf && esp_timer_get_time() - lcd4TextStartUs < 1000000) { // Text nur die erste Sekunde / text only the first second
                int ty = (y0 + l) & (LCD4_TILE_H - 1);
                if (rot) ty = LCD4_TILE_H - 1 - ty;
                const uint8_t* row = lcd4BannerBuf + ty * 60;
                for (int x = 0; x < 480; x++) {
                    int xs = rot ? 479 - x : x;
                    d[x] = (row[xs >> 3] & (0x80 >> (xs & 7))) ? 0xFFFF : 0x0800;
                }
            }
            else {
                for (int x = 0; x < 480; x++) d[x] = 0x0800;
            }
        }
        return false;
    }
    int64_t now = esp_timer_get_time();

    // Die Puffer muessen lueckenlos aufeinander folgen und gleichmaessig kommen. Am Bildende (letzter Puffer, Position 480 x 480)
    // darf die Pause um die Austastung laenger sein, sonst nicht - so gibt es keinen blinden Bereich mehr. Ein verpasster Pufferwechsel
    // zeigt sich als Luecke oder als Sprung der Position und verschiebt das Bild dauerhaft - Neustart vormerken (lcd4Reinit()).

    // The buffers must follow each other without a gap and evenly. At the frame end (last buffer, position 480 x 480) the pause around
    // the blanking may be longer, otherwise not - so there is no blind area any more. A missed buffer switch shows as a gap or as a
    // jump of the position and shifts the picture permanently - note a restart (lcd4Reinit()).

    if (now < lcd4IgnoreUntilUs) {
        lcd4BounceLastUs = 0;
    }
    else if (lcd4BounceLastUs) {
        const bool boundary = lcd4LastEndPx >= 480 * 480 || lcd4LastEndPx == 2 * lcd4BounceLines * 480; // Bildende, und der zweite Puffer eines Bildes (die Austastung liegt zwischen seiner Fuellung und der des dritten) / frame end, and the second buffer of a frame (the blanking lies between its filling and that of the third)
        const uint32_t gap = (uint32_t)(now - lcd4BounceLastUs);
        const int64_t limit = lcd4BounceGapUs + (boundary ? lcd4BlankUs : 0);
        if (!boundary && gap > lcd4MaxGapUs) lcd4MaxGapUs = gap;
        if (gap > limit) {
            lcd4StallSeen = true; lcd4StallCount++;
            lcd4Anom[0] = 1; lcd4Anom[1] = gap; lcd4Anom[2] = (uint32_t)posPx; lcd4Anom[3] = (uint32_t)(now / 1000);
        }
        if (posPx != (lcd4LastEndPx >= 480 * 480 ? 0 : lcd4LastEndPx)) {
            lcd4StallSeen = true; lcd4PosErrors++;
            lcd4Anom[0] = 2; lcd4Anom[1] = (uint32_t)lcd4LastEndPx; lcd4Anom[2] = (uint32_t)posPx; lcd4Anom[3] = (uint32_t)(now / 1000);
        }
    }
    lcd4BounceLastUs = now;
    lcd4LastEndPx = posPx + lenBytes / 2;
    if (posPx == 0) {

        // Bildanfang: ein fertiges Bild jetzt anzeigen (Zeiger der beiden Bildspeicher tauschen), sofern die Uhr nicht mitten im Schreiben
        // ist; haengt das Schreiben laenger als 200 ms, gilt es als beendet. Das ganze Bild kommt dann aus einem Speicher.
        // Start of a frame: show a finished frame now (swap the pointers of the two frame buffers) if the clock is not in the middle of
        // writing; if the writing hangs for more than 200 ms it counts as finished. The whole frame then comes from one buffer.

        if (lcd4Open.load() && now - lcd4OpenSinceUs > 200000) lcd4Open.store(false);
        if (lcd4PresentPending.load() && !lcd4Open.load() && !lcd4Swapped.load()) {
            uint8_t* shown = lcd4DrawFb;
            lcd4DrawFb = lcd4Fb;
            lcd4Fb = shown;
            portENTER_CRITICAL_ISR(&lcd4Mux);
            lcd4SwapDirty = lcd4Dirty;
            lcd4Dirty = Lcd4Rect();
            portEXIT_CRITICAL_ISR(&lcd4Mux);
            lcd4PresentPending.store(false);
            lcd4Swapped.store(true);
            lcd4Swaps++;
        }
        lcd4FrameFb = lcd4Fb;
    }
    memcpy(buf, (lcd4FrameFb ? lcd4FrameFb : lcd4Fb) + (size_t)posPx * 2, lenBytes);
    return false;
}

    // Bildende: dauert ein Bild deutlich laenger oder kuerzer als vorgesehen, war die Unterbrechung gesperrt - Neustart vormerken
    // End of frame: if a frame takes clearly longer or shorter than planned the interrupt was locked - note a restart

inline bool lcd4VsyncEvent(esp_lcd_panel_handle_t, const esp_lcd_rgb_panel_event_data_t*, void*) {
    int64_t now = esp_timer_get_time();
    if (now < lcd4IgnoreUntilUs) lcd4VsyncLastUs = 0;
    if (lcd4VsyncLastUs) {
        int64_t dt = now - lcd4VsyncLastUs;
        if (dt > (int64_t)lcd4FramePeriodUs * 5 / 4 || dt < (int64_t)lcd4FramePeriodUs * 3 / 4) {
            lcd4StallSeen = true; lcd4VsyncStalls++;
            lcd4Anom[0] = 3; lcd4Anom[1] = (uint32_t)dt; lcd4Anom[2] = 0; lcd4Anom[3] = (uint32_t)(now / 1000);
        }
    }
    lcd4VsyncLastUs = now;
    return false;
}

    // Treiber anlegen und starten. Der Bounce-Betrieb ohne Treiber-Bildspeicher braucht Puffer: reicht das freie RAM nicht
    // fuer zwei am Stueck, mit weniger Zeilen versuchen.

    // Create and start the driver. Bounce mode without a driver frame buffer needs buffers: if the free RAM does not hold two
    // in one piece, try fewer lines.

inline bool lcd4CreatePanel() {
    esp_lcd_rgb_panel_config_t p = lcd4PanelCfg;
    const int tries[] = { lcd4BounceLines, 16, 8 };
    lcd4PanelHandle = nullptr;
    for (int lines : tries) {
        p.bounce_buffer_size_px = 480 * lines;
        esp_err_t err = esp_lcd_new_rgb_panel(&p, &lcd4PanelHandle);
        if (err == ESP_OK) { lcd4BounceLines = lines; lcd4BounceGapUs = (int64_t)lines * lcd4LineNs * 205 / 100 / 1000; break; }
        Serial.printf("[LCD] RGB panel with %d bounce lines failed: %s (free heap %u, largest block %u)\n", lines, esp_err_to_name(err),
                      (unsigned)ESP.getFreeHeap(), (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA));
        lcd4PanelHandle = nullptr;
    }
    if (!lcd4PanelHandle) return false;

    esp_lcd_rgb_panel_event_callbacks_t cb = {};
    cb.on_bounce_empty = lcd4BounceEmpty;
    cb.on_vsync = lcd4VsyncEvent;
    lcd4VsyncLastUs = 0;
    lcd4BounceLastUs = 0;
    lcd4LastEndPx = 0;
    lcd4IgnoreUntilUs = esp_timer_get_time() + 500000; // der Treiberstart selbst ist unruhig: eine halbe Sekunde nicht werten / the driver start itself is uneven: do not count for half a second
    lcd4StallSeen = false;
    if (esp_lcd_rgb_panel_register_event_callbacks(lcd4PanelHandle, &cb, nullptr) != ESP_OK) return false;
    if (esp_lcd_panel_reset(lcd4PanelHandle) != ESP_OK) return false;
    return esp_lcd_panel_init(lcd4PanelHandle) == ESP_OK;
}

    // Treiber neu starten (Bildspeicher bleibt): behebt ein dauerhaft verschobenes Bild nach Flash-Schreibzugriffen
    // Restart the driver (the frame buffer stays): fixes a permanently shifted picture after flash writes

inline void lcd4Reinit() {
    if (!lcd4PanelHandle) return;
    lcd4ReinitCount++;
    esp_lcd_panel_del(lcd4PanelHandle);
    lcd4CreatePanel();
}

inline void lcd4Resync() { lcd4Reinit(); }

    // Nach jedem Schreibblock aufgerufen (Firmware-Update, Sicherung): schaltet das Display beim ersten Aufruf auf eine
    // feste Farbe. checkTouch() schaltet nach 1,5 s ohne Schreibblock zurueck.

    // Called after every write block (firmware update, backup): switches the display to a fixed colour on the first call.
    // checkTouch() switches back after 1.5 s without a write block.

inline void lcd4StaticBegin() {
    lcd4LastPauseMs = millis();
    if (!lcd4StaticOn) {

        // Ab jetzt feste Farbe, Treiber sauber neu starten
        // Fixed colour from now on, restart the driver cleanly

        lcd4TextStartUs = esp_timer_get_time();
        lcd4StaticOn = true;
        lcd4Reinit();
    }
}

inline void lcd4FlashPause() {
    lcd4StaticBegin();
    if (LCD4_FLASH_PAUSE_MS) delay(LCD4_FLASH_PAUSE_MS);
}

// Schreibzugriffe auf den Flash (Speichern von Zifferblatt/Zeiger, Hochladen): ruhige blaue Flaeche ohne Text, bis 1,5 s nach
// dem letzten Aufruf - statt eines flackernden Bildes. Jeder Aufruf verlaengert die Zeit.
// Writes to the flash (saving a clock face/hand, uploading): a calm blue area without text until 1.5 s after the last call -
// instead of a flickering image. Every call extends the time.
inline void lcd4BusyBegin() {
    lcd4StaticTextClear();
    lcd4FlashPause();
}

class Bus_EspLcdRGB : public lgfx::Bus_RGB {
public:
    bool init(void) override {
        const auto& c = config();
        esp_lcd_rgb_panel_config_t& p = lcd4PanelCfg;
        p = {};
        p.clk_src = LCD_CLK_SRC_DEFAULT;
        p.timings.pclk_hz = c.freq_write;
        p.timings.h_res = 480;
        p.timings.v_res = 480;
        p.timings.hsync_pulse_width = c.hsync_pulse_width;
        p.timings.hsync_back_porch = c.hsync_back_porch;
        p.timings.hsync_front_porch = c.hsync_front_porch;
        p.timings.vsync_pulse_width = c.vsync_pulse_width;
        p.timings.vsync_back_porch = c.vsync_back_porch;
        p.timings.vsync_front_porch = c.vsync_front_porch;
        p.timings.flags.hsync_idle_low = !c.hsync_polarity;
        p.timings.flags.vsync_idle_low = !c.vsync_polarity;
        p.timings.flags.de_idle_high = c.de_idle_high;
        p.timings.flags.pclk_active_neg = 0;
        p.timings.flags.pclk_idle_high = c.pclk_idle_high;
        p.data_width = 16;
        p.bits_per_pixel = 16;
        p.num_fbs = 0;
        p.dma_burst_size = 64;
        p.hsync_gpio_num = c.pin_hsync;
        p.vsync_gpio_num = c.pin_vsync;
        p.de_gpio_num = c.pin_henable;
        p.pclk_gpio_num = c.pin_pclk;
        p.disp_gpio_num = -1;
        p.flags.no_fb = 1;

        // LovyanGFX legt die Farbe byteweise vertauscht ab (hohes Byte zuerst): Datenleitung j traegt Farbbit (j + 8) % 16
        // LovyanGFX stores the colour byte-swapped (high byte first): data line j carries colour bit (j + 8) % 16

        for (int j = 0; j < 16; j++) p.data_gpio_nums[j] = c.pin_data[(j + 8) & 15];

        uint64_t totalPx = (uint64_t)(c.hsync_pulse_width + c.hsync_back_porch + 480 + c.hsync_front_porch) *
                           (c.vsync_pulse_width + c.vsync_back_porch + 480 + c.vsync_front_porch);
        lcd4FramePeriodUs = (uint32_t)(totalPx * 1000000ULL / c.freq_write);
        lcd4LineNs = (int64_t)(c.hsync_pulse_width + c.hsync_back_porch + 480 + c.hsync_front_porch) * 1000000000LL / c.freq_write;
        lcd4BlankUs = (int64_t)(c.vsync_pulse_width + c.vsync_back_porch + c.vsync_front_porch) * lcd4LineNs / 1000;

        for (int i = 0; i < 2; i++) {
            if (!lcd4FbBuf[i]) lcd4FbBuf[i] = (uint8_t*)heap_caps_aligned_calloc(64, 1, 480 * 480 * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        }
        if (!lcd4FbBuf[0] || !lcd4FbBuf[1]) return false;
        lcd4Fb = lcd4FbBuf[0];
        lcd4DrawFb = lcd4FbBuf[1];
        lcd4FrameFb = lcd4Fb;
        lcd4Open.store(false); lcd4PresentPending.store(false); lcd4Swapped.store(false);
        lcd4Dirty = Lcd4Rect(); lcd4SwapDirty = Lcd4Rect();
        return lcd4CreatePanel();
    }

    uint8_t* getDMABuffer(uint32_t) override { return lcd4DrawFb; }

    void release(void) override {
        if (lcd4PanelHandle) { esp_lcd_panel_del(lcd4PanelHandle); lcd4PanelHandle = nullptr; }
        for (int i = 0; i < 2; i++) { if (lcd4FbBuf[i]) { heap_caps_free(lcd4FbBuf[i]); lcd4FbBuf[i] = nullptr; } }
        lcd4Fb = nullptr; lcd4DrawFb = nullptr; lcd4FrameFb = nullptr;
    }
};

    // Nach einem Tausch (lcd4Swapped) vor dem naechsten Schreiben: die Zeilenzeiger auf den neuen Schreibspeicher setzen und die
    // geaenderten Zeilen aus dem angezeigten Speicher dorthin kopieren. Nur im Hauptlauf.
    // After a swap (lcd4Swapped) before the next write: point the line pointers at the new write buffer and copy the changed lines from
    // the displayed buffer there. Main loop only.

struct Panel_UhrRGB : public lgfx::Panel_RGB {
    void lcd4SetLines(uint8_t* fb) {
        if (!_lines_buffer) return;
        const size_t stride = (size_t)(((_cfg.panel_width + 3) & ~3) * _write_bits >> 3);
        for (int y = 0; y < (int)_cfg.panel_height; ++y) _lines_buffer[y] = fb + (size_t)y * stride;
    }
    void lcd4WriteBegin() {
        lcd4OpenSinceUs = esp_timer_get_time();
        lcd4Open.store(true);
        if (lcd4Swapped.load()) {
            Lcd4Rect d;
            portENTER_CRITICAL(&lcd4Mux);
            d = lcd4SwapDirty;
            portEXIT_CRITICAL(&lcd4Mux);
            lcd4SetLines(lcd4DrawFb);
            if (d.r >= d.l && d.b >= d.t) {
                const int l = max((int)d.l, 0), r = min((int)d.r, 479);
                for (int y = max((int)d.t, 0); y <= min((int)d.b, 479); y++) {
                    memcpy(lcd4DrawFb + (size_t)y * 960 + (size_t)l * 2, lcd4Fb + (size_t)y * 960 + (size_t)l * 2, (size_t)(r - l + 1) * 2);
                }
            }
            lcd4Swapped.store(false);
        }
    }
    void display(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h) override {
        Lcd4Rect m;
        if (_internal_rotation != 0) { m.l = 0; m.t = 0; m.r = 479; m.b = 479; }
        else {
            int_fast16_t l = _range_mod.left, rr = _range_mod.right, t = _range_mod.top, b = _range_mod.bottom;
            if (0 < w && 0 < h) { l = std::min<int_fast16_t>(l, (int_fast16_t)x); rr = std::max<int_fast16_t>(rr, (int_fast16_t)(x + w - 1)); t = std::min<int_fast16_t>(t, (int_fast16_t)y); b = std::max<int_fast16_t>(b, (int_fast16_t)(y + h - 1)); }
            if (rr >= l && b >= t) { m.l = (int16_t)l; m.r = (int16_t)rr; m.t = (int16_t)t; m.b = (int16_t)b; }
        }
        if (m.r >= m.l && m.b >= m.t) {
            portENTER_CRITICAL(&lcd4Mux);
            lcd4Dirty.l = min(lcd4Dirty.l, m.l); lcd4Dirty.t = min(lcd4Dirty.t, m.t);
            lcd4Dirty.r = max(lcd4Dirty.r, m.r); lcd4Dirty.b = max(lcd4Dirty.b, m.b);
            portEXIT_CRITICAL(&lcd4Mux);
        }
        lgfx::Panel_RGB::display(x, y, w, h);
        lcd4PresentPending.store(true);
        lcd4Open.store(false);
    }
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override { lcd4WriteBegin(); lgfx::Panel_RGB::drawPixelPreclipped(x, y, rawcolor); }
    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override { lcd4WriteBegin(); lgfx::Panel_RGB::writeFillRectPreclipped(x, y, w, h, rawcolor); }
    void writeBlock(uint32_t rawcolor, uint32_t length) override { lcd4WriteBegin(); lgfx::Panel_RGB::writeBlock(rawcolor, length); }
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, lgfx::pixelcopy_t* param, bool use_dma) override { lcd4WriteBegin(); lgfx::Panel_RGB::writeImage(x, y, w, h, param, use_dma); }
    void writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, lgfx::pixelcopy_t* param) override { lcd4WriteBegin(); lgfx::Panel_RGB::writeImageARGB(x, y, w, h, param); }
    void writePixels(lgfx::pixelcopy_t* param, uint32_t len, bool use_dma) override { lcd4WriteBegin(); lgfx::Panel_RGB::writePixels(param, len, use_dma); }
    void copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y) override { lcd4WriteBegin(); lgfx::Panel_RGB::copyRect(dst_x, dst_y, w, h, src_x, src_y); }
};

class UhrLGFX : public lgfx::LGFX_Device {
    Panel_UhrRGB _panel_rgb;
    Bus_EspLcdRGB _bus_rgb;

public:
    UhrLGFX() {
        {
            auto cfg = _panel_rgb.config();
            cfg.memory_width = 480;
            cfg.memory_height = 480;
            cfg.panel_width = 480;
            cfg.panel_height = 480;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            _panel_rgb.config(cfg);
        }
        {
            auto cfg = _panel_rgb.config_detail();
            cfg.use_psram = 2; // Bildspeicher nur im PSRAM / frame buffer in PSRAM only
            _panel_rgb.config_detail(cfg);
        }
        {
            auto cfg = _bus_rgb.config();
            cfg.panel = &_panel_rgb;

#if BOARD_WAVESHARE_S3_LCD4
            // Datenleitungen B0..B4, G0..G5, R0..R4 (RGB565)
            // Data lines B0..B4, G0..G5, R0..R4 (RGB565)

            static const int8_t pins[16] = { 5, 45, 48, 47, 21, 14, 13, 12, 11, 10, 9, 46, 3, 8, 18, 17 };
            for (int i = 0; i < 16; i++) cfg.pin_data[i] = pins[i];
            cfg.pin_henable = 40; // DE
            cfg.pin_vsync = 39;
            cfg.pin_hsync = 38;
            cfg.pin_pclk = 41;
            cfg.freq_write = LCD4_PCLK_HZ;
            cfg.hsync_polarity = 1;
            cfg.hsync_front_porch = 10;
            cfg.hsync_pulse_width = 8;
            cfg.hsync_back_porch = 50;
            cfg.vsync_polarity = 1;
            cfg.vsync_front_porch = 10;
            cfg.vsync_pulse_width = 8;
            cfg.vsync_back_porch = 20;
            cfg.pclk_active_neg = 0;
            cfg.pclk_idle_high = 0;
            cfg.de_idle_high = 0;
#else

            // Adafruit Qualia (Variante adafruit_qualia_s3_rgb666): B1..B5, G0..G5, R1..R5 in der Reihenfolge B0..B4, G0..G5, R0..R4
            // Adafruit Qualia (variant adafruit_qualia_s3_rgb666): B1..B5, G0..G5, R1..R5 in the order B0..B4, G0..G5, R0..R4

            static const int8_t pins[16] = { 40, 39, 38, 0, 45, 48, 47, 21, 14, 13, 12, 11, 10, 9, 46, 3 };
            for (int i = 0; i < 16; i++) cfg.pin_data[i] = pins[i];
            cfg.pin_henable = 2; // DE
            cfg.pin_vsync = 42;
            cfg.pin_hsync = 41;
            cfg.pin_pclk = 1;
            cfg.freq_write = LCD4_PCLK_HZ;
            cfg.hsync_polarity = 1;
            cfg.hsync_front_porch = 40;
            cfg.hsync_pulse_width = 20;
            cfg.hsync_back_porch = 40;
            cfg.vsync_polarity = 1;
            cfg.vsync_front_porch = 40;
            cfg.vsync_pulse_width = 10;
            cfg.vsync_back_porch = 40;
            cfg.pclk_active_neg = 0;
            cfg.pclk_idle_high = 0;
            cfg.de_idle_high = 0;
#endif
            _bus_rgb.config(cfg);
            _panel_rgb.setBus(&_bus_rgb);
        }
        setPanel(&_panel_rgb);
    }

    // Ein fest verbautes Panel - nichts zu waehlen
    // One built-in panel - nothing to choose

    void selectPanel(uint8_t) {}
};

#else

    // Nur auf dem ESP32-S3-Touch-LCD-4 sinnvoll (Pause nach Flash-Schreibbloecken, Display neu synchronisieren)
    // Only useful on the ESP32-S3-Touch-LCD-4 (pause after flash write blocks, re-synchronise the display)

inline void lcd4FlashPause() {}
inline void lcd4Resync() {}
inline void lcd4StaticText(const String&, const String&, bool = false) {}
inline void lcd4StaticBegin() {}
inline void lcd4BusyBegin() {}
inline void lcd4ExpanderRefresh() {}
inline void lcd4BacklightTick() {}
inline bool lcd4ExpanderIdle() { return true; }
inline void lcd4SetBacklight(uint8_t) {}

class UhrLGFX : public lgfx::LGFX_Device {
    Panel_UhrGC9A01 _panel_gc9a01;
    Panel_UhrGC9D01 _panel_gc9d01;
    lgfx::Panel_ILI9341 _panel_ili9341;
    lgfx::Panel_ST7789 _panel_st7789;
    lgfx::Panel_ST7789 _panel_st7789_240;
    lgfx::Bus_SPI _bus_instance;

    // memWidth/memHeight/offsetX: sichtbarer Ausschnitt im 240x320-Speicher des ST7789 (172x320: ab Spalte
    // 34; 240x240: die Hoehe 320 braucht LovyanGFX fuer den Versatz bei 180/270 Grad)

    // memWidth/memHeight/offsetX: visible area in the ST7789's 240x320 memory (172x320: from column 34;
    // 240x240: LovyanGFX needs the height 320 for the offset at 180/270 degrees)

    void configPanel(lgfx::Panel_LCD& panel, uint16_t width, uint16_t height, bool invert, bool rgbOrder,
                     uint16_t memWidth = 0, uint16_t memHeight = 0, int16_t offsetX = 0) {
        panel.setBus(&_bus_instance);
        auto cfg = panel.config();
        cfg.pin_cs = LGFX_CS_PIN;     // -1 = manuell (CS_1/CS_2), ESP32-C6/-S3: LovyanGFX, siehe config.h
                                      // -1 = manual (CS_1/CS_2), ESP32-C6/-S3: LovyanGFX, see config.h

        // Kein Reset durch LovyanGFX (nur 8 ms Puls, 64 ms Wartezeit). Den
        // Reset macht setup() selbst mit laengeren Zeiten (resetPanels() in
        // display.h).

        // No reset by LovyanGFX (only an 8 ms pulse, 64 ms wait). setup()
        // does the reset itself with longer timing (resetPanels() in
        // display.h).

        cfg.pin_rst = -1;
        cfg.pin_busy = -1;
        cfg.panel_width = width;
        cfg.panel_height = height;
        cfg.memory_width = memWidth ? memWidth : width;
        cfg.memory_height = memHeight ? memHeight : height;
        cfg.offset_x = offsetX;
        cfg.offset_y = 0;
        cfg.offset_rotation = 0;
        cfg.readable = false;
        cfg.invert = invert;
        cfg.rgb_order = rgbOrder;
        cfg.dlen_16bit = false;
        cfg.bus_shared = false;
        panel.config(cfg);
    }

public:
    UhrLGFX() {
        {
            auto cfg = _bus_instance.config();
#if BOARD_WAVESHARE_C6_ST7789
            cfg.spi_host = SPI2_HOST;     // der ESP32-C6 hat nur diesen frei nutzbaren SPI
                                          // the only freely usable SPI on the ESP32-C6
#elif BOARD_WAVESHARE_S3_GC9A01
            cfg.spi_host = SPI2_HOST;     // FSPI, wie in den Beispielen von Waveshare
                                          // FSPI, as in the Waveshare examples
#else
            cfg.spi_host = SPI3_HOST;     // HSPI wie bisher (USE_HSPI_PORT)
                                          // HSPI as before (USE_HSPI_PORT)
#endif
            cfg.spi_mode = 0;
            cfg.freq_write = TFT_SPI_FREQUENCY;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = true;

            // DMA AUS: LovyanGFX 1.2.x hat keinen DMA-Zweig fuer den ESP32-S2 und verwirft DMA-Uebertragungen
            // still - auch normale Uebertragungen von 65..1023 Byte (z.B. jede Zeile eines Teilbilds), die
            // intern auf DMA umgeleitet werden. Ohne DMA-Kanal sendet LovyanGFX immer per CPU.

            // DMA OFF: LovyanGFX 1.2.x has no DMA branch for the ESP32-S2 and silently drops DMA transfers -
            // also normal transfers of 65..1023 bytes (e.g. every row of a partial frame), which are rerouted
            // to DMA internally. Without a DMA channel LovyanGFX always sends via the CPU.

            cfg.dma_channel = 0;
            cfg.pin_sclk = TFT_SCLK;
            cfg.pin_mosi = TFT_MOSI;
            cfg.pin_miso = -1;
            cfg.pin_dc = TFT_DC;
            _bus_instance.config(cfg);
        }

        // GC9A01: Invertierung an (INVON), BGR-Farbreihenfolge
        // GC9A01: inversion on (INVON), BGR color order

        configPanel(_panel_gc9a01, 240, 240, true, false);

        // GC9D01: keine Invertierung, RGB (Endzustand der Startsequenz)
        // GC9D01: no inversion, RGB (final state of the init sequence)

        configPanel(_panel_gc9d01, 160, 160, false, true);

        // ILI9341 (240x320): LovyanGFX-Standardtreiber, keine Invertierung, BGR wie bei den meisten Modulen
        // ILI9341 (240x320): LovyanGFX default driver, no inversion, BGR as on most modules

        configPanel(_panel_ili9341, 240, 320, false, false);

        // ST7789 172x320 (Waveshare ESP32-C6-LCD-1.47): invertiert, Bild ab Spalte 34 des 240er-Speichers
        // ST7789 172x320 (Waveshare ESP32-C6-LCD-1.47): inverted, image from column 34 of the 240 memory

        configPanel(_panel_st7789, 172, 320, true, false, 240, 320, 34);

        // ST7789 240x240 (Waveshare ESP32-C6-LCD-1.3): invertiert, wie die Beispiele von Waveshare
        // ST7789 240x240 (Waveshare ESP32-C6-LCD-1.3): inverted, as in the Waveshare examples

        configPanel(_panel_st7789_240, 240, 240, true, false, 240, 320);
        setPanel(&_panel_gc9a01);
    }

    // Panel-Treiber zum Displaytyp waehlen - nur VOR tft.init()
    // Choose the panel driver for the display type - only BEFORE tft.init()

    void selectPanel(uint8_t type) {
        if (type == DISPLAY_TYPE_GC9D01) setPanel(&_panel_gc9d01);
        else if (type == DISPLAY_TYPE_ILI9341) setPanel(&_panel_ili9341);
        else if (type == DISPLAY_TYPE_ST7789) setPanel(&_panel_st7789);
        else if (type == DISPLAY_TYPE_ST7789_240) setPanel(&_panel_st7789_240);
        else setPanel(&_panel_gc9a01);
    }
};

#endif // BOARD_RGB_PANEL


    // Beleuchtung per PWM setzen (0-255): ESP32-S3-Touch-LCD-4 ueber den Port-Expander, sonst ledcWrite() am
    // Beleuchtungs-Pin.

    // Set the backlight via PWM (0-255): ESP32-S3-Touch-LCD-4 via the port expander, otherwise ledcWrite() on the
    // backlight pin.

inline void backlightWrite(uint8_t level) {
#if BOARD_RGB_PANEL
    lcd4SetBacklight(level);
#else
    ledcWrite(TFT_Backlight, level);
#endif
}


    // Weitere Taster ausser BUTTON1/BOOT_BUTTON (Qualia: UP/DOWN am Port-Expander); sonst nie gedrueckt
    // Further buttons besides BUTTON1/BOOT_BUTTON (Qualia: UP/DOWN on the port expander); otherwise never pressed

#if !BOARD_QUALIA
inline bool extraButtonDown() { return false; }
#endif
