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

#if BOARD_WAVESHARE_S3_LCD4

    // Waveshare ESP32-S3-Touch-LCD-4 (V2/V3): ST7701 ueber RGB-Bus. Pins, Timings und Startfolge stammen aus dem
    // Demo-Code von Waveshare (ESP32-S3-Touch-LCD-4_democode_V2.zip) und dem Treibertest tools/lcd4_test. Auf dem
    // Geraet noch nicht erprobt. Der Touch (GT911) ist hier noch nicht eingebunden: LovyanGFX braucht dafuer einen
    // eigenen I2C-Port auf denselben Pins wie Wire (RTC, Expander) - das ist noch zu loesen.

    // Waveshare ESP32-S3-Touch-LCD-4 (V2/V3): ST7701 on an RGB bus. Pins, timings and start-up sequence come from
    // Waveshare's demo code (ESP32-S3-Touch-LCD-4_democode_V2.zip) and the driver test tools/lcd4_test. Not yet
    // tried on the device. The touch (GT911) is not integrated here yet: LovyanGFX needs its own I2C port on the
    // same pins as Wire (RTC, expander) for it - still to be solved.

#include <Wire.h>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>

constexpr uint8_t LCD4_TCA9554_ADDR = 0x20;                       // Port-Expander / port expander
constexpr uint8_t LCD4_EXIO_BL_EN = 1, LCD4_EXIO_LCD_RST = 2;     // Beleuchtung an/aus, LCD-Reset
constexpr uint8_t LCD4_EXIO_TP_RST = 0, LCD4_EXIO_SD_CS = 3;      // Touch-Reset (hier nur HIGH), SD-Chip-Select
constexpr int LCD4_PIN_SPI_SCK = 2, LCD4_PIN_SPI_SDA = 1;         // 3-Draht-SPI zum ST7701 (CS = CS_1) / 3-wire SPI to the ST7701

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
    Wire.beginTransmission(LCD4_TCA9554_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

    // Vor tft.init(): LCD-Reset und Beleuchtung ueber den Port-Expander, danach die ST7701-Startfolge. Wire muss
    // laufen (setup()). Die RGB-Signale starten erst mit tft.init().

    // Before tft.init(): LCD reset and backlight via the port expander, then the ST7701 start-up sequence. Wire
    // must be running (setup()). The RGB signals only start with tft.init().

inline void lcd4PanelInit() {
    lcd4ExpanderWrite(0x03, 0xF0);                  // EXIO0..3 als Ausgang / EXIO0..3 as outputs
    uint8_t out = 0xFF & ~(1 << LCD4_EXIO_LCD_RST); // LCD-Reset aktiv / LCD reset active
    lcd4ExpanderWrite(0x01, out);
    delay(20);
    out |= (1 << LCD4_EXIO_LCD_RST) | (1 << LCD4_EXIO_BL_EN) | (1 << LCD4_EXIO_TP_RST) | (1 << LCD4_EXIO_SD_CS);
    lcd4ExpanderWrite(0x01, out);
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

class UhrLGFX : public lgfx::LGFX_Device {
    lgfx::Panel_RGB _panel_rgb;
    lgfx::Bus_RGB _bus_rgb;

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

            // Datenleitungen B0..B4, G0..G5, R0..R4 (RGB565)
            // Data lines B0..B4, G0..G5, R0..R4 (RGB565)

            static const int8_t pins[16] = { 5, 45, 48, 47, 21, 14, 13, 12, 11, 10, 9, 46, 3, 8, 18, 17 };
            for (int i = 0; i < 16; i++) cfg.pin_data[i] = pins[i];
            cfg.pin_henable = 40; // DE
            cfg.pin_vsync = 39;
            cfg.pin_hsync = 38;
            cfg.pin_pclk = 41;
            cfg.freq_write = 16000000;
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

#endif // BOARD_WAVESHARE_S3_LCD4
