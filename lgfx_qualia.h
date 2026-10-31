// Adafruit Qualia ESP32-S3 for RGB666 Displays mit dem 2,1" runden Display TL021WVC02 (480x480, Controller der ST7701-Familie,
// Touch CST826). Dieser Teil ersetzt in lgfx_config.h die Waveshare-Teile (Port-Expander CH32V003, Startfolge, Reset).
// Quellen: Pins aus der Variante "adafruit_qualia_s3_rgb666" des ESP32-Arduino-Cores (pins_arduino.h), Startfolge und Zeiten
// aus Adafruit_CircuitPython_Qualia (displays/round21.py), Expander PCA9554A (Adresse 0x3F laut Adafruit-Anleitung).
// NICHT AM GERAET GETESTET.
//
// Adafruit Qualia ESP32-S3 for RGB666 displays with the 2.1" round display TL021WVC02 (480x480, ST7701-family controller,
// CST826 touch). This part replaces the Waveshare parts in lgfx_config.h (CH32V003 port expander, start-up sequence, reset).
// Sources: pins from the variant "adafruit_qualia_s3_rgb666" of the ESP32 Arduino core (pins_arduino.h), start-up sequence
// and timings from Adafruit_CircuitPython_Qualia (displays/round21.py), PCA9554A expander (address 0x3F per the Adafruit guide).
// NOT TESTED ON THE DEVICE.

#pragma once

#include <Wire.h>

    // PCA9554A: Register 0 Eingang, 1 Ausgang, 2 Polaritaet, 3 Richtung (1 = Eingang). Pins: SCK 0, CS 1, Reset 2, Touch-IRQ 3,
    // Beleuchtung 4, Taste UP 5, Taste DOWN 6, MOSI 7. Die 3-Draht-SPI zum Controller laeuft ueber diese Pins, von Hand.

    // PCA9554A: register 0 input, 1 output, 2 polarity, 3 direction (1 = input). Pins: SCK 0, CS 1, reset 2, touch IRQ 3,
    // backlight 4, button UP 5, button DOWN 6, MOSI 7. The 3-wire SPI to the controller runs over these pins, by hand.

constexpr uint8_t QUALIA_PCA_ADDR = 0x3F;
constexpr uint8_t QUALIA_REG_IN = 0, QUALIA_REG_OUT = 1, QUALIA_REG_CFG = 3;
constexpr uint8_t QUALIA_PIN_SCK = 0, QUALIA_PIN_CS = 1, QUALIA_PIN_RESET = 2, QUALIA_PIN_IRQ = 3, QUALIA_PIN_BL = 4,
                  QUALIA_PIN_UP = 5, QUALIA_PIN_DOWN = 6, QUALIA_PIN_MOSI = 7;
constexpr uint8_t QUALIA_DIRECTION = (1 << QUALIA_PIN_IRQ) | (1 << QUALIA_PIN_UP) | (1 << QUALIA_PIN_DOWN); // Eingaenge / inputs

    // Startfolge als Liste: Befehl, Anzahl Daten (Bit 7 = danach 120 ms warten), Daten; Ende mit Anzahl 0xFF. Aus round21.py;
    // dort steht Bit 7 mit einer Wartezeit (0x11: 100 ms, 0x29: 50 ms) - hier einheitlich 120 ms.
    // Start-up sequence as a list: command, number of data bytes (bit 7 = wait 120 ms afterwards), data; the end is count
    // 0xFF. From round21.py; there bit 7 comes with a delay (0x11: 100 ms, 0x29: 50 ms) - here uniformly 120 ms.

static const uint8_t QUALIA_TL021_INIT[] = {
    0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x10,
    0xC0, 2, 0x3B, 0x00,
    0xC1, 2, 0x0B, 0x02,
    0xC2, 2, 0x00, 0x02,
    0xCC, 1, 0x10,
    0xCD, 1, 0x08,
    0xB0, 16, 0x02, 0x13, 0x1B, 0x0D, 0x10, 0x05, 0x08, 0x07, 0x07, 0x24, 0x04, 0x11, 0x0E, 0x2C, 0x33, 0x1D,
    0xB1, 16, 0x05, 0x13, 0x1B, 0x0D, 0x11, 0x05, 0x08, 0x07, 0x07, 0x24, 0x04, 0x11, 0x0E, 0x2C, 0x33, 0x1D,
    0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x11,
    0xB0, 1, 0x5D,
    0xB1, 1, 0x43,
    0xB2, 1, 0x81,
    0xB3, 1, 0x80,
    0xB5, 1, 0x43,
    0xB7, 1, 0x85,
    0xB8, 1, 0x20,
    0xC1, 1, 0x78,
    0xC2, 1, 0x78,
    0xD0, 1, 0x88,
    0xE0, 3, 0x00, 0x00, 0x02,
    0xE1, 11, 0x03, 0xA0, 0x00, 0x00, 0x04, 0xA0, 0x00, 0x00, 0x00, 0x20, 0x20,
    0xE2, 13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xE3, 4, 0x00, 0x00, 0x11, 0x00,
    0xE4, 2, 0x22, 0x00,
    0xE5, 16, 0x05, 0xEC, 0xA0, 0xA0, 0x07, 0xEE, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xE6, 4, 0x00, 0x00, 0x11, 0x00,
    0xE7, 2, 0x22, 0x00,
    0xE8, 16, 0x06, 0xED, 0xA0, 0xA0, 0x08, 0xEF, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xEB, 7, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00,
    0xED, 16, 0xFF, 0xFF, 0xFF, 0xBA, 0x0A, 0xBF, 0x45, 0xFF, 0xFF, 0x54, 0xFB, 0xA0, 0xAB, 0xFF, 0xFF, 0xFF,
    0xEF, 6, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F,
    0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x13,
    0xEF, 1, 0x08,
    0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x00,
    0x36, 1, 0x00,
    0x3A, 1, 0x60,
    0x11, 128,
    0x29, 128,
    0x00, 0xFF
};

inline uint8_t qualiaOut = (1 << QUALIA_PIN_CS) | (1 << QUALIA_PIN_RESET); // Ausgangsregister (Beleuchtung aus) / output register (backlight off)

inline bool qualiaPcaWrite(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(QUALIA_PCA_ADDR);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

inline void qualiaSetPin(uint8_t pin, bool high) {
    if (high) qualiaOut |= (1 << pin); else qualiaOut &= ~(1 << pin);
    qualiaPcaWrite(QUALIA_REG_OUT, qualiaOut);
}

    // 9-Bit-SPI von Hand: erstes Bit 0 = Befehl, 1 = Daten; je Bit zwei I2C-Schreibzugriffe (Daten bei SCK low, dann SCK high)
    // 9-bit SPI by hand: first bit 0 = command, 1 = data; two I2C writes per bit (data with SCK low, then SCK high)

inline void qualiaSpiBit(bool bit) {
    qualiaOut &= ~((1 << QUALIA_PIN_SCK) | (1 << QUALIA_PIN_MOSI));
    if (bit) qualiaOut |= (1 << QUALIA_PIN_MOSI);
    qualiaPcaWrite(QUALIA_REG_OUT, qualiaOut);
    qualiaOut |= (1 << QUALIA_PIN_SCK);
    qualiaPcaWrite(QUALIA_REG_OUT, qualiaOut);
}

inline void qualiaSpiByte(bool isData, uint8_t b) {
    qualiaSpiBit(isData);
    for (uint8_t m = 0x80; m; m >>= 1) qualiaSpiBit(b & m);
}

    // Wie auf dem Waveshare-Board: Reset ueber den Expander, dann die Startfolge (CS je Befehl). Wire muss laufen. Die RGB-Signale
    // starten erst mit tft.init(); die Beleuchtung geht danach an.

    // As on the Waveshare board: reset via the expander, then the start-up sequence (CS per command). Wire has to run. The RGB
    // signals only start with tft.init(); the backlight goes on afterwards.

inline void lcd4PanelInit() {
    Wire.begin(SDA_PIN, SCL_PIN, 400000);
    qualiaOut = (1 << QUALIA_PIN_CS) | (1 << QUALIA_PIN_RESET);
    qualiaPcaWrite(QUALIA_REG_OUT, qualiaOut);
    qualiaPcaWrite(QUALIA_REG_CFG, QUALIA_DIRECTION);
    delay(10);
    qualiaSetPin(QUALIA_PIN_RESET, false);
    delay(20);
    qualiaSetPin(QUALIA_PIN_RESET, true);
    delay(120);

    const uint8_t* p = QUALIA_TL021_INIT;
    while (p[1] != 0xFF) {
        uint8_t cmd = p[0], n = p[1] & 0x7F;
        bool waitAfter = p[1] & 0x80;
        qualiaSetPin(QUALIA_PIN_CS, false);
        qualiaSpiByte(false, cmd);
        for (uint8_t i = 0; i < n; i++) qualiaSpiByte(true, p[2 + i]);
        qualiaOut &= ~(1 << QUALIA_PIN_SCK);
        qualiaOut |= (1 << QUALIA_PIN_CS);
        qualiaPcaWrite(QUALIA_REG_OUT, qualiaOut);
        p += 2 + n;
        if (waitAfter) delay(120);
    }
    qualiaSetPin(QUALIA_PIN_BL, true); // Beleuchtung an (nur an/aus) / backlight on (on/off only)
}

    // Im Betrieb: Ausgangsregister gelegentlich erneuern (Beleuchtung an) - ein Schreibzugriff je Aufruf
    // During operation: renew the output register now and then (backlight on) - one write per call

inline void lcd4ExpanderRefresh() {
    qualiaPcaWrite(QUALIA_REG_OUT, qualiaOut);
}

    // Die Beleuchtung hat nur an/aus (PWM nur mit Loetjumper am Board): die Helligkeit regelt das Abdunkeln der Pixel
    // The backlight is only on/off (PWM only with a solder jumper on the board): brightness is done by darkening the pixels

inline void lcd4SetBacklight(uint8_t) {}
inline void lcd4BacklightTick() {} // ohne PWM nichts zu tun / nothing to do without PWM
inline bool lcd4ExpanderIdle() { return true; }

    // Tasten UP/DOWN am Expander (low = gedrueckt), hoechstens alle 20 ms gelesen
    // Buttons UP/DOWN on the expander (low = pressed), read at most every 20 ms

inline bool extraButtonDown() {
    static unsigned long lastRead = 0;
    static bool down = false;
    if (millis() - lastRead >= 20) {
        lastRead = millis();
        Wire.beginTransmission(QUALIA_PCA_ADDR);
        Wire.write(QUALIA_REG_IN);
        if (Wire.endTransmission(true) == 0 && Wire.requestFrom((int)QUALIA_PCA_ADDR, 1) == 1) {
            uint8_t in = Wire.read();
            down = !(in & (1 << QUALIA_PIN_UP)) || !(in & (1 << QUALIA_PIN_DOWN));
        }
    }
    return down;
}

    // Touch CST826 (I2C 0x15): Register 0x02 = Anzahl Beruehrungen. Liefert true einmal je neuer Beruehrung (erst nach dem
    // Loslassen wieder). Der Chip schlaeft bei Ruhe und wacht bei Beruehrung auf - ein Zugriff im Schlaf kann fehlschlagen.

    // Touch CST826 (I2C 0x15): register 0x02 = number of touches. Returns true once per new touch (again only after the
    // release). The chip sleeps when idle and wakes on touch - an access during sleep may fail.

inline bool lcd4TouchPressed() {
    static uint8_t addr = 0;
    static unsigned long lastProbe = 0;
    static bool wasDown = false;
    if (!addr) {
        if (millis() - lastProbe < 2000) return false;
        lastProbe = millis();
        Wire.beginTransmission(0x15);
        if (Wire.endTransmission() == 0) addr = 0x15;
        return false;
    }
    bool down = false;
    Wire.beginTransmission(addr);
    Wire.write(0x02);
    if (Wire.endTransmission(true) == 0 && Wire.requestFrom((int)addr, 1) == 1) down = (Wire.read() & 0x0F) > 0;
    bool fresh = down && !wasDown;
    wasDown = down;
    return fresh;
}
