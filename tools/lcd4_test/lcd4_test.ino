// Treibertest fuer das Waveshare ESP32-S3-Touch-LCD-4 (V2/V3): 480x480, ST7701 ueber RGB-Bus, GT911-Touch,
// TCA9554-Port-Expander. Kein Teil der Uhr: zeigt nur Testbilder und gibt Messwerte seriell aus (115200 Baud).
// Pins und ST7701-Startfolge stammen aus dem Demo-Code von Waveshare (ESP32-S3-Touch-LCD-4_democode_V2.zip).
//
// Arduino-Einstellungen: Board ESP32S3 Dev Module, PSRAM "OPI PSRAM", Flash 16 MB, USB CDC On Boot "Enabled".
// Bibliothek: LovyanGFX (1.2.x). Beim ersten Start bitte den seriellen Monitor oeffnen und das Bild beschreiben.
//
// Driver test for the Waveshare ESP32-S3-Touch-LCD-4 (V2/V3): 480x480, ST7701 on an RGB bus, GT911 touch,
// TCA9554 port expander. Not part of the clock: only shows test images and prints measurements (115200 baud).
// The pins and the ST7701 start-up sequence come from Waveshare's demo code (ESP32-S3-Touch-LCD-4_democode_V2.zip).
//
// Arduino settings: board ESP32S3 Dev Module, PSRAM "OPI PSRAM", flash 16 MB, USB CDC On Boot "Enabled".
// Library: LovyanGFX (1.2.x). On the first start please open the serial monitor and describe what you see.

#define LGFX_USE_V1
#include <Arduino.h>
#include <Wire.h>
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>

// Bild gedreht? Waveshare nutzt in allen Beispielen Rotation 2 (180 Grad), 0 = Grundstellung des Controllers.
// Image rotated? Waveshare uses rotation 2 (180 degrees) in all examples, 0 = home position of the controller.

#define TEST_ROTATION 2

// Taktflanke der Pixeldaten: Bei Farbrauschen oder versetzten Spalten PCLK_IDLE_HIGH auf 1 aendern.
// Edge of the pixel clock: with colour noise or shifted columns change PCLK_IDLE_HIGH to 1.

#define PCLK_HZ 16000000
#define PCLK_IDLE_HIGH 0

static constexpr int PIN_LCD_CS = 42, PIN_LCD_SCK = 2, PIN_LCD_SDA = 1; // 3-Draht-SPI zum ST7701 / 3-wire SPI to the ST7701
static constexpr int PIN_I2C_SDA = 15, PIN_I2C_SCL = 7, PIN_TOUCH_INT = 16;
static constexpr uint8_t TCA9554_ADDR = 0x20;                           // TCA9554PWR mit A0..A2 = 0 / with A0..A2 = 0
static constexpr uint8_t EXIO_TP_RST = 0, EXIO_BL_EN = 1, EXIO_LCD_RST = 2, EXIO_SD_CS = 3;

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_RGB _panel;
    lgfx::Bus_RGB _bus;
    lgfx::Touch_GT911 _touch;

public:
    LGFX() {
        {
            auto cfg = _panel.config();
            cfg.memory_width = 480;
            cfg.memory_height = 480;
            cfg.panel_width = 480;
            cfg.panel_height = 480;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            _panel.config(cfg);
        }
        {
            auto cfg = _panel.config_detail();
            cfg.use_psram = 2; // Bildspeicher nur im PSRAM / frame buffer in PSRAM only
            _panel.config_detail(cfg);
        }
        {
            auto cfg = _bus.config();
            cfg.panel = &_panel;

            // Datenleitungen in der Reihenfolge B0..B4, G0..G5, R0..R4 (RGB565)
            // Data lines in the order B0..B4, G0..G5, R0..R4 (RGB565)

            static const int8_t pins[16] = { 5, 45, 48, 47, 21, 14, 13, 12, 11, 10, 9, 46, 3, 8, 18, 17 };
            for (int i = 0; i < 16; i++) cfg.pin_data[i] = pins[i];
            cfg.pin_henable = 40; // DE
            cfg.pin_vsync = 39;
            cfg.pin_hsync = 38;
            cfg.pin_pclk = 41;
            cfg.freq_write = PCLK_HZ;
            cfg.hsync_polarity = 1;
            cfg.hsync_front_porch = 10;
            cfg.hsync_pulse_width = 8;
            cfg.hsync_back_porch = 50;
            cfg.vsync_polarity = 1;
            cfg.vsync_front_porch = 10;
            cfg.vsync_pulse_width = 8;
            cfg.vsync_back_porch = 20;
            cfg.pclk_active_neg = 0;
            cfg.pclk_idle_high = PCLK_IDLE_HIGH;
            cfg.de_idle_high = 0;
            _bus.config(cfg);
            _panel.setBus(&_bus);
        }
        {
            auto cfg = _touch.config();
            cfg.x_min = 0;
            cfg.x_max = 479;
            cfg.y_min = 0;
            cfg.y_max = 479;
            cfg.pin_int = PIN_TOUCH_INT;
            cfg.pin_rst = -1; // Reset ueber den Port-Expander / reset via the port expander
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;
            cfg.i2c_port = 1;
            cfg.pin_sda = PIN_I2C_SDA;
            cfg.pin_scl = PIN_I2C_SCL;
            cfg.freq = 400000;
            _touch.config(cfg);
            _panel.setTouch(&_touch);
        }
        setPanel(&_panel);
    }
};

static LGFX lcd;

// ---- TCA9554: Ausgangs-Register 1, Richtung 3 (0 = Ausgang) / output register 1, direction register 3 (0 = output)

static uint8_t expanderOut = 0xFF;

static bool expanderWrite(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(TCA9554_ADDR);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

static void expanderBit(uint8_t exio, bool high) {
    if (high) expanderOut |= (1 << exio); else expanderOut &= ~(1 << exio);
    expanderWrite(0x01, expanderOut);
}

// ---- ST7701: 9-Bit-SPI von Hand (erstes Bit: 0 = Befehl, 1 = Daten)
// ---- ST7701: 9-bit SPI by hand (first bit: 0 = command, 1 = data)

static void spiBit(bool bit) {
    digitalWrite(PIN_LCD_SDA, bit);
    digitalWrite(PIN_LCD_SCK, HIGH);
    digitalWrite(PIN_LCD_SCK, LOW);
}

static void spiByte(bool isData, uint8_t b) {
    spiBit(isData);
    for (uint8_t m = 0x80; m; m >>= 1) spiBit(b & m);
}

// Eintrag: Befehl, Anzahl Daten (Bit 7 = danach 120 ms warten), Daten. Ende: Anzahl 0xFF.
// Entry: command, number of data bytes (bit 7 = wait 120 ms afterwards), data. End: count 0xFF.

static const uint8_t ST7701_INIT[] = {
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
    0x21, 0,          // IPS: Farben invertieren / IPS: invert colours
    0x3A, 1, 0x60,    // Pixelformat wie im Waveshare-Demo / pixel format as in the Waveshare demo
    0x11, 0x80,       // Sleep Out, danach warten / Sleep Out, then wait
    0x29, 0x80,       // Display On, danach warten / Display On, then wait
    0x00, 0xFF
};

static void st7701Init() {
    pinMode(PIN_LCD_CS, OUTPUT);
    pinMode(PIN_LCD_SCK, OUTPUT);
    pinMode(PIN_LCD_SDA, OUTPUT);
    digitalWrite(PIN_LCD_CS, HIGH);
    digitalWrite(PIN_LCD_SCK, LOW);
    digitalWrite(PIN_LCD_SDA, LOW);

    digitalWrite(PIN_LCD_CS, LOW); // Software-Reset / software reset
    spiByte(false, 0x01);
    digitalWrite(PIN_LCD_CS, HIGH);
    delay(120);

    const uint8_t* p = ST7701_INIT;
    digitalWrite(PIN_LCD_CS, LOW);
    while (p[1] != 0xFF) {
        uint8_t cmd = p[0], n = p[1] & 0x7F;
        bool waitAfter = p[1] & 0x80;
        spiByte(false, cmd);
        for (uint8_t i = 0; i < n; i++) spiByte(true, p[2 + i]);
        p += 2 + n;
        if (waitAfter) {
            digitalWrite(PIN_LCD_CS, HIGH);
            delay(120);
            digitalWrite(PIN_LCD_CS, LOW);
        }
    }
    digitalWrite(PIN_LCD_CS, HIGH);
}

// ---- Testbild und Messung / test image and measurement

static void drawTestImage() {
    const uint16_t bars[8] = { TFT_WHITE, TFT_YELLOW, TFT_CYAN, TFT_GREEN, TFT_MAGENTA, TFT_RED, TFT_BLUE, TFT_BLACK };
    int w = lcd.width(), h = lcd.height();
    for (int i = 0; i < 8; i++) lcd.fillRect(i * w / 8, 0, w / 8 + 1, h / 3, bars[i]);
    for (int x = 0; x < w; x++) lcd.drawFastVLine(x, h / 3, h / 6, lcd.color565(x * 255 / w, x * 255 / w, x * 255 / w)); // Graustufen / grey ramp
    lcd.fillRect(0, h / 2, w, h / 2, TFT_BLACK);
    lcd.drawRect(0, 0, w, h, TFT_WHITE);                     // Rand: zeigt, ob das ganze Bild sichtbar ist / border: shows whether the whole image is visible
    lcd.drawRect(1, 1, w - 2, h - 2, TFT_RED);
    lcd.drawCircle(w / 2, h / 2, h / 2 - 2, TFT_WHITE);     // Kreis: Seitenverhaeltnis / circle: aspect ratio
    lcd.fillTriangle(w / 2, h / 2 + 10, w / 2 - 40, h / 2 + 90, w / 2 + 40, h / 2 + 90, TFT_ORANGE); // Spitze zeigt nach oben / tip points up
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    lcd.setTextDatum(lgfx::middle_center);
    lcd.setFont(&fonts::Font4);
    lcd.drawString("uhr4 480x480 test", w / 2, h / 2 + 130);
    lcd.setFont(&fonts::Font2);
    lcd.drawString("ST7701 / RGB / LovyanGFX", w / 2, h / 2 + 160);
}

static void measureFill() {
    uint32_t t0 = millis();
    const int rounds = 20;
    for (int i = 0; i < rounds; i++) lcd.fillScreen(i & 1 ? TFT_BLUE : TFT_RED);
    uint32_t dt = millis() - t0;
    Serial.printf("fillScreen: %.1f ms pro Bild / per frame (%d Bilder / frames)\n", dt / (float)rounds, rounds);
}

void setup() {
    Serial.begin(115200);
    delay(1500);
    Serial.println("lcd4_test: start");
    Serial.printf("PSRAM: %u KB, frei / free: %u KB\n", (unsigned)(ESP.getPsramSize() / 1024), (unsigned)(ESP.getFreePsram() / 1024));

    // Port-Expander: LCD_RST, Touch-Reset, Beleuchtung ein
    // Port expander: LCD reset, touch reset, backlight on

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 400000);
    Serial.println("I2C-Suche / I2C scan (SDA 15, SCL 7):");
    for (uint8_t a = 1; a < 127; a++) {
        Wire.beginTransmission(a);
        if (Wire.endTransmission() == 0) Serial.printf("  Adresse / address 0x%02X\n", a);
    }
    if (expanderWrite(0x03, 0xF0)) {            // EXIO0..3 als Ausgang / EXIO0..3 as outputs
        expanderOut = 0xFF;
        expanderBit(EXIO_LCD_RST, false);
        expanderBit(EXIO_TP_RST, false);
        delay(20);
        expanderBit(EXIO_LCD_RST, true);
        expanderBit(EXIO_TP_RST, true);
        expanderBit(EXIO_BL_EN, true);
        expanderBit(EXIO_SD_CS, true);
        delay(120);
        Serial.println("TCA9554: ok (Reset und Beleuchtung gesetzt / reset and backlight set)");
    }
    else {
        Serial.println("TCA9554 antwortet nicht / does not answer - Beleuchtung und Reset unklar / backlight and reset unclear");
    }
    Wire.end();

    st7701Init();
    if (!lcd.init()) Serial.println("lcd.init() fehlgeschlagen / failed");
    lcd.setRotation(TEST_ROTATION);
    lcd.setBrightness(255);
    drawTestImage();
    Serial.printf("Display %d x %d\n", (int)lcd.width(), (int)lcd.height());
    measureFill();
    drawTestImage();
    Serial.println("Beruehren Sie das Display: Koordinaten erscheinen hier und als Kreuz / touch the display: coordinates appear here and as a cross");
}

void loop() {
    static int lastX = -1, lastY = -1;
    lgfx::touch_point_t tp;
    if (lcd.getTouch(&tp)) {
        if (tp.x != lastX || tp.y != lastY) {
            Serial.printf("Touch: x=%d y=%d\n", (int)tp.x, (int)tp.y);
            lcd.drawFastHLine(tp.x - 12, tp.y, 25, TFT_GREEN);
            lcd.drawFastVLine(tp.x, tp.y - 12, 25, TFT_GREEN);
            lastX = tp.x;
            lastY = tp.y;
        }
    }
    else {
        lastX = lastY = -1;
    }
    delay(10);
}
