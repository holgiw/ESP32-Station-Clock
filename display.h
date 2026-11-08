#pragma once
#include "globals.h"

    // display.cpp - Display: Zifferblatt, Zeiger, Sprites, Helligkeit
    // display.cpp - Display: clock face, hands, sprites, brightness

    // Zifferblaetter des Startpakets (ensureStarterSet(), refreshGeneratedAssets())
    // Clock faces of the starter set (ensureStarterSet(), refreshGeneratedAssets())

    struct StarterFace { const char* path; uint8_t numerals; };
    constexpr uint8_t STARTER_FACE_COUNT = 4; // Eintraege von STARTER_FACES / entries of STARTER_FACES

    // Version der erzeugten Stunden- und Minutenzeiger: bei einer neuen Version legt die Uhr sie einmal neu an
    // (siehe refreshGeneratedAssets())

    // Version of the generated hour and minute hands: with a new version the clock creates them anew once
    // (see refreshGeneratedAssets())

    constexpr uint8_t GENERATED_HAND_VERSION = 4;

    // Version der erzeugten Sekundenzeigers von Satz 3: bei einer neuen Version legt die Uhr ihn einmal neu an (siehe
    // refreshGeneratedAssets()). 2 = filigraner (halbe Nadelbreite, kleinere Scheibe), 3 = Scheibe nochmal halbiert
    // Version of the generated second hand of set 3: with a new version the clock creates it anew once (see
    // refreshGeneratedAssets()). 2 = more delicate (half the needle width, smaller disc), 3 = disc halved again

    constexpr uint8_t SUBDIAL_HAND_VERSION = 3;

    // Dauer einer Zeigerfahrt bei grossen Spruengen (siehe animateHand())
    // Duration of a hand move for large jumps (see animateHand())

    const unsigned long HAND_MOVE_MS = 3000;

    struct HandMove {
        bool active = false;
        float offset = 0.0f;      // angezeigt - Ziel, ungewrappt / shown minus target, unwrapped
        float lastTarget = 0.0f;
        float lastRemain = 1.0f;  // Restanteil (1 - Smoothstep) im letzten Bild / remaining share in the last frame
        unsigned long startMillis = 0;
        unsigned long durationMs = HAND_MOVE_MS;
    };

    // Schriftarten fuer den Streifen mit Uhrzeit/Datum, Auswahl im Zifferblatt-Designer (stripFont). size =
    // Vergroesserung, nur die GLCD-Schrift wird skaliert.

    // Fonts for the time/date strip, chosen in the clock face designer (stripFont). size = magnification, only
    // the GLCD font is scaled.

    struct StripFont {
        const char* name;
        const lgfx::IFont* time;
        const lgfx::IFont* date;
        uint8_t size;
    };

    constexpr uint8_t STRIP_FONT_COUNT = 5; // Eintraege von STRIP_FONTS / entries of STRIP_FONTS
    constexpr uint8_t STRIP_FONT_VLW = 255; // VLW-Schriften aus dem Designer (kantengeglaettet)
                                            // VLW fonts from the designer (anti-aliased)
    constexpr uint8_t STRIP_DATE_FMT_COUNT = 8;
    constexpr long STRIP_SCALE_MIN = 50;  // Groesse der eingebauten Schrift in % / size of the built-in font in %
    constexpr long STRIP_SCALE_MAX = 200;

    void handleSerialOpenWifi() ;
    void* preferPsramMalloc(size_t size) ;
    bool isDisplayConnected(uint8_t displayNum) ;
    uint8_t primaryDisplayRotation() ;
    uint8_t effectiveRotation(uint8_t displayNum) ;
    bool infoStripRect(uint8_t displayNum, int& x, int& y, int& w, int& h, bool& landscape) ;
    bool parseDisplayName(const String& name, uint8_t& type, bool& backlight) ;
    const char* displayChoiceName(uint8_t type, bool backlight) ;
    bool displayTypeSupported(uint8_t type) ;
    const char* supportedDisplayNames() ;
    void adoptUhr3BuildDisplay() ;
    bool hexToText(const String& hex, String& out, size_t maxLen) ;
    void serialReply(const String& reply) ;
    void serialRestart(const String& reply) ;
    void handleSerialWifi(const String& args) ;
    void handleSerialInfo() ;
    void handleSerialDisplay(const String& name) ;
#if ARDUINO_USB_CDC_ON_BOOT && !ARDUINO_USB_MODE
    void usbCdcLineCodingEvent(void* arg, esp_event_base_t base, int32_t id, void* data) ;
#endif
    void handleSerialCommands() ;
    float wrapAngle(float a) ;
    float animateHand(HandMove& m, float shown, float target, unsigned long now) ;
    void loadDisplayType() ;
    void resetPanels() ;
    void setDisplayType(uint8_t type, bool backlight) ;
    uint8_t hardwareRotation(uint8_t displayNum) ;
    void recordRenderFrame(uint32_t durationMicros, bool partial) ;
    void pushBackgroundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t ox = 0, int32_t oy = 0) ;
    void clockOrigin(uint8_t displayNum, int& ox, int& oy) ;
    bool createSprite16(LGFX_Sprite& sprite, int32_t w, int32_t h, bool preferPsram = true) ;
    void setupTextStyle(lgfx::LovyanGFX& gfx) ;
    String tftText(const String& text) ;
    void setCSIdle() ;
    void applyDisplayRotation(uint8_t displayNum, uint8_t newRotation) ;
    void setCS1(bool state) ;
    void setCS2(bool state) ;
    bool statusLandscape() ;
    int statusWidth() ;
    lgfx::LovyanGFX& beginStatusDraw(uint8_t displayNum) ;
    void endStatusDraw(uint8_t displayNum) ;
    void setAutoBrightness(bool enabled) ;
    bool applyBrightnessPresetValue(const String& key, const String& value) ;
    void putBrightnessDefaults(bool backlight) ;
    void applyBacklightPin() ;
    void setBacklightMode(bool enabled) ;
    uint16_t setPixelBrightness(uint16_t pixel) ;
    bool isRleFace(const uint8_t* header4) ;
    size_t rleMaxEncodedSize(size_t pixelCount) ;
    size_t rleEncode565(const uint16_t* pixels, size_t count, uint8_t* out) ;
    void rleDecode565(const uint8_t* in, size_t inSize, uint16_t* out, size_t outCount) ;
    void drawDefaultFace(uint16_t* dest) ;
    bool writeGeneratedFace(const char* path, uint8_t numeralMode) ;
    bool ensureDefaultFace() ;
    void loadFaceOrDefault(const String& path, uint16_t* dest) ;
    bool writeRleImage(const String& path, int w, int h, const std::function<void(int, uint16_t*)>& rowFn) ;
    float subdialSecondHalfWidth(float d) ;
    float curvedHandHalfWidth(const char* part, float L, float d) ;
    void generatedHandRow(int set, const char* part, int y, uint16_t* row) ;
    void drawDefaultHand(const char* part, uint16_t* dest) ;
    bool writeGeneratedHandSet(int style, const String& id) ;
    bool handSetExists(const String& id) ;
    String subdialHandSet() ;
    bool copyLittleFile(const String& from, const String& to) ;
    void completeHandSet(const String& id) ;
    void completeHandSets() ;
    int upgradeHandFiles() ;
    bool isProtectedHandSetId(const String& id) ;
    bool isProtectedFile(const String& path) ;
    String protectedFacesJs() ;
    String protectedSetsJs() ;
    bool ensureDefaultHands() ;
    String handSetFileId(const String& setId) ;
    void loadHandOrDefault(const String& setId, const char* part, uint16_t* dest) ;
    bool loadFaceBmpInto(const String& path, uint16_t* dest, int32_t expectedW, int32_t expectedH) ;
    void loadClockFace(uint8_t rotation = primaryDisplayRotation()) ; // ohne Argument = Rotation des ersten angeschlossenen Displays (Standardverhalten fuer alle Aufrufer ausserhalb von renderClockFrame())
                                                                      // no argument = rotation of the first connected display (default behaviour for every caller outside renderClockFrame())
    void freeClockFaceBuffer() ;
    void resetFacesToDefault() ;
    void resetHandsToDefault() ;
    void pushHandRowCentered(LGFX_Sprite* sprite, int row, uint16_t* rowPixels, int srcWidth, const uint8_t* transparentColor) ;
    void loadHandSprites() ;
    void placeHand(const uint16_t* src, int w, int h, uint16_t* dest) ;
    bool loadHandPixels(const String& path, uint16_t* dest) ;
    void handTargetSize(const char* path, int& outW, int& outH) ;
    bool readImageSize(const char* path, int32_t& w, int32_t& h) ;
    bool isValidHandSize(int32_t w, int32_t h) ;
    String fileVersion(const String& path) ;
    String handFormatLabel(const String& path) ;
    float shortestAngleDiff(float from, float to) ;
    bool lowMemoryFace() ;
    void prepareClockFaceRle() ;
    void drawFaceRows(LGFX_Sprite& dest) ;
    int prepareClockFaceCache() ;
    int faceOrientationFor(uint8_t rotation) ;
    bool blitFaceIntoSprite(LGFX_Sprite& dest, uint8_t rotation, int32_t x0 = 0, int32_t y0 = 0, int32_t x1 = -1, int32_t y1 = -1) ;
    bool buildHandComposite(HandComposite& comp, uint8_t rotation, float hourAngle, float minuteAngle, bool hands) ;
    bool drawCompositeInto(uint8_t displayNum, uint8_t rotation, float hourAngle, float minuteAngle, bool hands) ;
    bool renderClockFrame(uint8_t displayNum, uint8_t rotation, float& lastHourAngleRef, float& lastMinuteAngleRef, float& lastSecondAngleRef, bool& firstRunRef) ; // false = Frame unveraendert, nichts gesendet
                                                                                                                                                                  // false = frame unchanged, nothing sent
    String faceSidePath(const String& facePath, const char* prefix) ;
    String stripConfigPath(const String& facePath) ;
    String faceConfigPath(const String& facePath) ;
    bool faceTrailerOffset(File& f, size_t& off) ;
    bool readFaceTrailer(const String& facePath, FacePivots& out) ;
    void readFacePivots(const String& facePath, FacePivots& out) ;
    bool saveFacePivots(const String& facePath, const FacePivots& p) ;
    void readFaceSecPivot(const String& facePath, int16_t& x, int16_t& y) ;
    void migrateFaceCfgFiles() ;
    HandPlan makeHandPlanFrom(const FacePivots& p, bool drawSecond) ;
    HandPlan makeHandPlan(bool drawSecond) ;
    void pivotToDisplay(uint8_t rotation, float px, float py, float& x, float& y) ;
    bool saveFaceSettings(const String& facePath, int16_t x, int16_t y) ;
    void ensureFaceSettings() ;
    bool secondPivotAt(uint8_t rotation, float& x, float& y) ;
    void resetStripSettings() ;
    String stripSettingsQuery() ;
    void applyStripQuery(const String& text) ;
    bool loadStripSettingsForFace(const String& facePath) ;
    bool saveStripSettingsForFace(const String& facePath) ;
    void ensureStripSettings() ;
    bool loadLegacyStripPrefs() ;
    uint8_t nearestStripVlwSize(uint8_t target, bool weekday) ;
    void scaleStripConfigFile(const String& path, int fromW, int fromH) ;
    uint8_t* loadFileToPsram(const char* path) ;
    String stripVlwPath(uint8_t size, bool weekday) ;
    bool ensureStripVlw() ;
    void setStripBefore(bool before) ;
    bool applyStripValue(const String& key, const String& value) ;
    void drawStripTime(lgfx::LovyanGFX& g, const String& text, bool colon, int cx, int y) ;
    String stripPathForFace(const String& facePath) ;
    bool ensureStripImage() ;
    bool drawStripImageStreamed(LGFX_Sprite& s, bool landscape, bool push) ;
    void renderInfoStrip(int x, int y, int w, int h, bool landscape, const String* lines, uint8_t count, bool colon, const String& suffix, uint16_t bg, uint16_t fg, bool push, bool useImage, uint16_t* overlayBuf = nullptr) ;
    uint32_t infoOverlayKeyNow() ;
    void applyInfoOverlay(uint16_t* buf) ;
    void infoOverlayRefresh(uint32_t key) ;
    void stripDateText(const struct tm& t, String& full, String& a, String& b) ;
    String stripWeekdayText(const struct tm& t) ;
    void stripContent(bool landscape, String* lines, uint8_t& count, bool& colon, String& suffix, const struct tm* fixedTime = nullptr) ;
    uint16_t stripColor565(uint32_t rgb) ;
    bool renderStripPreview(int w, int h, bool textOnly, const struct tm* fixedTime = nullptr, uint16_t textBg = TRANSPARENT_COLOR) ;
    bool stripShowsSeconds() ;
    bool renderFaceStripPreview(const String& faceFile, bool& before) ;
    void drawStripThumb(LGFX_Sprite& canvas, int y0, int w, int h) ;
    void drawFaceOverlayThumb(LGFX_Sprite& canvas, const String& faceFile, int size) ;
    bool generateFaceStripBmp(const String& faceFile, int outW, uint8_t** outBytes, size_t& outSize) ;
    void drawInfoStrips() ;
    void updateClock() ;
    void updateBrightness() ;
    uint16_t getAdjustedAdcValue(int rawValue) ;
    float easeInOutSine(float t) ;
    uint32_t crc32Update(uint32_t crc, const uint8_t* buf, size_t len) ;
    void streamHandPng(const uint16_t* src, int srcW, int w, int h) ;
    uint8_t* encodeBmpToBytes(const uint16_t* data, int width, int height, size_t* outSize) ;
    void clearTFT() ;
    float rotatedAngle(float angle, int orientation) ;
    bool checkBmpFormat(const String& filename, int expectedWidth = CLOCK_WIDTH, int expectedHeight = CLOCK_HEIGHT) ;
    String getBmpInfo(const String& filename) ;
    bool scaleAndSaveBmpLowMem(const char* sourcePath, const char* targetPath, int outW, int outH) ;
    bool scaleAndSaveBmp(const char* sourcePath, const char* targetPath, int outW, int outH) ;
    void migrateFaceBmpsToRLE() ;
    void migrateHandBmpsToRLE() ;
    bool peekFirstPixelIsWhite(const String& path) ;
    void remaskExistingFaceCorners() ;
    void sendScaledBmpPreview(const String& sourcePath, int outW, int outH) ;
    bool readRleSize(const String& path, int32_t& w, int32_t& h) ;
    size_t bmpFileSize565(int32_t w, int32_t h) ;
    void buildBmpHeader565(uint8_t* hdr, int32_t w, int32_t h) ;
    bool writeRleAsBmp(const String& path, const std::function<void(const uint8_t*, size_t)>& out) ;
    bool streamRleFaceAsStandardBmp(const String& path, const char* contentType = "image/bmp") ;
    bool drawFaceThumbStreamed(const String& faceFile, LGFX_Sprite& canvas, int oy, int size) ;
    bool sendPresetPreviewBmp(const String& faceFile, const String& handSetName, uint16_t hubColorRgb565, uint8_t hubSize, bool showSecond, const String& presetUrl = "") ;
    void setLedOff() ;
    void setLedOn() ;
    void validateSelectedBackground() ;
    void updateHandWidths(int newHourWidth, int newMinuteWidth, int newSecondWidth) ;
    void parseBackgroundFilename(const String& filename, int& hourWidth, int& minuteWidth, int& secondWidth) ;

    float handPivotX(int spriteWidth);
    bool timeTrusted();

    extern const StarterFace STARTER_FACES[STARTER_FACE_COUNT];
    extern const StripFont STRIP_FONTS[STRIP_FONT_COUNT];
