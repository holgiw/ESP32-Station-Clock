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
// Adresse des Port-Expanders: V2 hat 0x20 (A0..A2 = 0), die V3 meldet sich auf 0x24 - setup() nimmt die erste gefundene
// Address of the port expander: V2 has 0x20 (A0..A2 = 0), the V3 answers at 0x24 - setup() takes the first one found

static uint8_t TCA9554_ADDR = 0x20;

// Die V3 hat statt des TCA9554 einen Waveshare-Expander (CH32V003, Adresse 0x24) mit eigener Registerbelegung
// (aus der ESPHome-Komponente waveshare_io_ch32v003): Richtung (1 = Ausgang), Ausgang, Eingang, PWM-Helligkeit.
// The V3 has a Waveshare expander (CH32V003, address 0x24) instead of the TCA9554, with its own register map
// (from the ESPHome component waveshare_io_ch32v003): direction (1 = output), output, input, PWM brightness.

static constexpr uint8_t EXP_REG_DIR = 0x02, EXP_REG_OUT = 0x03, EXP_REG_IN = 0x04, EXP_REG_PWM = 0x05;

// Touch direkt per Wire lesen (GT911, Adresse 0x5D oder 0x14) statt ueber LovyanGFX - zur Fehlersuche
// Read the touch directly via Wire (GT911, address 0x5D or 0x14) instead of via LovyanGFX - for troubleshooting

#define TOUCH_VIA_WIRE 1
static uint8_t gt911Addr = 0;

static bool gt911Read(uint16_t reg, uint8_t* buf, size_t len) {
    Wire.beginTransmission(gt911Addr);
    Wire.write(reg >> 8);
    Wire.write(reg & 0xFF);
    if (Wire.endTransmission(true) != 0) return false;
    if (Wire.requestFrom((int)gt911Addr, (int)len) != (int)len) return false;
    for (size_t i = 0; i < len; i++) buf[i] = Wire.read();
    return true;
}

static void gt911Write(uint16_t reg, uint8_t value) {
    Wire.beginTransmission(gt911Addr);
    Wire.write(reg >> 8);
    Wire.write(reg & 0xFF);
    Wire.write(value);
    Wire.endTransmission(true);
}
static constexpr uint8_t EXIO_TP_RST = 0, EXIO_BL_EN = 1, EXIO_LCD_RST = 2, EXIO_SD_CS = 3;

// EXIO5 = Beeper (high = an): als Ausgang auf low setzen, sonst schwebt der Pin hoch und der Beeper piept dauernd
// EXIO5 = buzzer (high = on): set it to an output at low, otherwise the pin floats high and the buzzer sounds all the time

static constexpr uint8_t EXIO_BUZZER = 5;

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
            if (!TOUCH_VIA_WIRE) _panel.setTouch(&_touch);
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

// Fehlersuche: antwortet der GT911 (0x5D oder 0x14)? / troubleshooting: does the GT911 answer (0x5D or 0x14)?

static int gt911Probe() {
    for (uint8_t a : { (uint8_t)0x5D, (uint8_t)0x14 }) {
        Wire.beginTransmission(a);
        if (Wire.endTransmission() == 0) return a;
    }
    return 0;
}

static void expanderDump(const char* what) {
    Serial.printf("Expander %s:", what);
    for (uint8_t reg = EXP_REG_DIR; reg <= EXP_REG_IN; reg++) {
        Wire.beginTransmission(TCA9554_ADDR);
        Wire.write(reg);
        if (Wire.endTransmission(true) == 0 && Wire.requestFrom((int)TCA9554_ADDR, 1) == 1) Serial.printf(" r%d=0x%02X", reg, Wire.read());
        else Serial.printf(" r%d=--", reg);
    }
    Serial.printf("  GT911: 0x%02X  INT=%d\n", gt911Probe(), digitalRead(PIN_TOUCH_INT));
}

static void expanderBit(uint8_t exio, bool high) {
    if (high) expanderOut |= (1 << exio); else expanderOut &= ~(1 << exio);
    expanderWrite(EXP_REG_OUT, expanderOut);
    char what[24];
    snprintf(what, sizeof(what), "EXIO%d=%d", (int)exio, (int)high);
    expanderDump(what);
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
    // Seriell nie blockieren: ohne geoeffnetes Terminal (Kaltstart ueber USB) wartet jede Ausgabe sonst auf das Leeren des
    // Puffers, das Setup dauert ~10 s und der Beeper bleibt so lange an.
    // Never block on serial output: without an open terminal (cold start via USB) every print would wait for the buffer to
    // drain, the setup takes ~10 s and the buzzer stays on that long.

    Serial.begin(115200);
    Serial.setTxTimeoutMs(0);

    // Beeper (EXIO5) sofort ausschalten: der Expander startet mit allen Ausgaengen high
    // Switch the buzzer (EXIO5) off immediately: the expander starts with all outputs high

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 400000);
    for (int tries = 0; tries < 20; tries++) {
        Wire.beginTransmission(0x24);
        Wire.write(EXP_REG_DIR);
        Wire.write(0xFF);
        bool ok = Wire.endTransmission() == 0;
        Wire.beginTransmission(0x24);
        Wire.write(EXP_REG_OUT);
        Wire.write(0x0F);
        if (Wire.endTransmission() == 0 && ok) break;
        delay(10);
    }
    delay(1500);
    Serial.println("lcd4_test: start");
    Serial.printf("PSRAM: %u KB, frei / free: %u KB\n", (unsigned)(ESP.getPsramSize() / 1024), (unsigned)(ESP.getFreePsram() / 1024));

    // Port-Expander: LCD_RST, Touch-Reset, Beleuchtung ein
    // Port expander: LCD reset, touch reset, backlight on

    Serial.println("I2C-Suche / I2C scan (SDA 15, SCL 7):");
    for (uint8_t a = 1; a < 127; a++) {
        Wire.beginTransmission(a);
        if (Wire.endTransmission() == 0) Serial.printf("  Adresse / address 0x%02X\n", a);
    }
    for (uint8_t a = 0x20; a <= 0x27; a++) {
        Wire.beginTransmission(a);
        if (Wire.endTransmission() == 0) { TCA9554_ADDR = a; break; }
    }
    Serial.printf("Port-Expander / port expander: 0x%02X\n", TCA9554_ADDR);
    pinMode(PIN_TOUCH_INT, INPUT);
    expanderDump("vor dem Schreiben / before writing");
    bool dirOk = false;
    for (int tries = 0; tries < 10 && !dirOk; tries++) {   // nach dem Einschalten braucht der Expander evtl. etwas / after power-up the expander may need a moment
        dirOk = expanderWrite(EXP_REG_DIR, 0xFF);          // alle Pins als Ausgang / all pins as outputs
        if (!dirOk) { Serial.printf("Expander: Schreiben %d fehlgeschlagen / write %d failed\n", tries + 1, tries + 1); delay(50); }
    }
    if (dirOk) {
        expanderDump("Richtung gesetzt / direction set");
        expanderOut = 0x0F;                                // nur EXIO0..3 high, Beeper (EXIO5) und die uebrigen low / only EXIO0..3 high, buzzer (EXIO5) and the rest low
        expanderWrite(EXP_REG_OUT, expanderOut);
        expanderBit(EXIO_LCD_RST, false);
        expanderBit(EXIO_TP_RST, false);
        delay(20);
        expanderBit(EXIO_LCD_RST, true);
        expanderBit(EXIO_TP_RST, true);
        expanderBit(EXIO_BL_EN, true);
        expanderWrite(EXP_REG_PWM, 247); // Helligkeit der Beleuchtung, 1..247 / backlight brightness, 1..247
        expanderBit(EXIO_SD_CS, true);
        delay(120);
        Serial.println("TCA9554: ok (Reset und Beleuchtung gesetzt / reset and backlight set)");
    }
    else {
        Serial.println("TCA9554 antwortet nicht / does not answer - Beleuchtung und Reset unklar / backlight and reset unclear");
    }
    if (TOUCH_VIA_WIRE) {
        pinMode(PIN_TOUCH_INT, INPUT);
        for (uint8_t a : { (uint8_t)0x5D, (uint8_t)0x14 }) {
            gt911Addr = a;
            uint8_t id[4] = { 0 };
            if (gt911Read(0x8140, id, 4)) {
                Serial.printf("GT911 bei 0x%02X, Produkt-ID / product ID: %c%c%c%c\n", a, id[0], id[1], id[2], id[3]);
                break;
            }
            gt911Addr = 0;
        }
        if (!gt911Addr) Serial.println("GT911 antwortet nicht / does not answer");
    }
    else Wire.end();

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
    bool touched = false;
    if (TOUCH_VIA_WIRE) {
        static uint32_t lastStatus = 0;
        uint8_t st = 0;
        if (gt911Addr && gt911Read(0x814E, &st, 1)) {
            if (st & 0x80) {                                   // Daten bereit / data ready
                uint8_t n = st & 0x0F;
                if (n >= 1) {
                    uint8_t p[8];
                    if (gt911Read(0x8150, p, 8)) {
                        int rx = p[0] | (p[1] << 8), ry = p[2] | (p[3] << 8);
                        Serial.printf("GT911 roh / raw: x=%d y=%d (Punkte / points %d)\n", rx, ry, (int)n);
                        tp.x = (TEST_ROTATION == 2) ? 479 - rx : rx;
                        tp.y = (TEST_ROTATION == 2) ? 479 - ry : ry;
                        touched = true;
                    }
                }
                gt911Write(0x814E, 0);                         // Status quittieren / acknowledge the status
            }
            else if (millis() - lastStatus > 3000) {
                lastStatus = millis();
                Serial.printf("GT911 Status 0x%02X, INT-Pin %d\n", st, digitalRead(PIN_TOUCH_INT));
            }
        }
    }
    else touched = lcd.getTouch(&tp);
    if (touched) {
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
