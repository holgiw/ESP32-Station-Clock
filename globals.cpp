#include "uhr4.h"

    // Definitionen der globalen Variablen und Objekte; die Erklaerungen stehen bei den Deklarationen in globals.h
    // Definitions of the global variables and objects; the explanations are with the declarations in globals.h

    uint8_t displayType = DISPLAY_TYPE_DEFAULT;
    const DisplayGeometry* displayGeom = &DISPLAY_GEOMETRY[DISPLAY_TYPE_DEFAULT];

    // System / Allgemein
    // System / General

    String currentLanguage = "de";
    char version[20];
    bool loggingEnabled = false;
    bool otaInProgress = false;
    unsigned long otaStartMillis = 0;
    String otaUploadError = "";
    bool timeSetByUser = false;
    bool handsAnimating = false;
    unsigned long handsAnimatingSinceMillis = 0;
    unsigned long handsDeferMs = HANDS_ANIMATION_MAX_DEFER_MS;
    String fwRemoteBuild = "";
    String fwCheckError = "";
    unsigned long fwLastCheckMillis = 0;
    String logLineBuffer = "";
    SemaphoreHandle_t logBufferMutex = nullptr;
    unsigned long lastLogFlushMillis = 0;
    bool initial = true;
    String ipAddress = "";
    UhrLGFX tft;
    bool tftInitialized = false;
    WebServer webserver(80);
    Preferences preferences;
    DNSServer dnsServer;
    WiFiUDP udp;
    WiFiUDP r2rnetDebugUdp;
    uint8_t r2rnetDebugLogCount = 0;
    bool r2rnetDebugListening = false;
    RTC_DS3231 rtc;

    // WLAN
    // WiFi

    String wifiSsid[MAX_WLAN];
    String wifiPass[MAX_WLAN];
    bool wifiActive = true;
    bool wpsPending = false;
    unsigned long wpsStartMillis = 0;
    String wpsPreviousSsid = "";
    volatile bool wpsSuccessEvent = false;
#ifdef WIFI_TX_AUTO
    volatile int8_t wifiTxPowerCurrent = WIFI_TX_FIRST;
#endif
    volatile int lastTxPowerResult = -1;
    volatile uint8_t lastWifiDisconnectReason = 0;
    volatile bool wpsFailedEvent = false;
    bool wpsStartRequested = false;
    unsigned long wpsStartRequestedAtMillis = 0;

    // MAC Adresse
    // MAC address

    uint8_t mac[6];
    char hostname[32];
    char apPassword[64] = "";
    bool pingHostname = false;
    bool softAPIP = false;
    long softAPIPstart = 0;
    String factoryResetCode = "";
    unsigned long factoryResetCodeStartMillis = 0;
    bool factoryResetCodeShown = false;
    String factoryResetPendingAction = "";
    uint8_t factoryResetCodeAttempts = 0;
    int pendingWifiChangeIndex = -1;
    String pendingWifiSsid[MAX_WLAN];
    String pendingWifiPass[MAX_WLAN];
    WifiNetwork availableNetworks[MAX_WLAN];
    int foundNetworkCount = 0;
    bool isScanning = false;

    // Zeit, NTP, DCF77, RTC
    // Time, NTP, DCF77, RTC

    // NTP-Server-Port
    // NTP server port

    // NTP-Paketgröße
    // NTP packet size

    byte ntpPacket[NTP_PACKET_SIZE];
    volatile uint16_t dcf77Count = 0;
    volatile bool dcfTimeFound = false;
    volatile bool dcfLedTogglePending = false;
    unsigned long dcfLedOffAtMillis = 0;
    bool dcfSyncLedEnabled = true;
    bool boardLedEnabled = true;
    bool touchEnabled = true;
    bool cornerHintsActive = false;
    time_t lastDcfSyncTime = 0;
    unsigned long lastDcf77PulseChangeMillis = 0;
    uint8_t dcf77PlausiblePulseStreak = 0;
    bool dcf77Confirmed = false;
    volatile unsigned long dcf77EdgeMillis[DCF77_EDGE_BUFFER_SIZE];
    volatile uint8_t dcf77EdgeHead = 0;
    volatile uint32_t dcf77EdgeDropped = 0;
    uint8_t dcf77EdgeTail = 0;
    int8_t dcf77Bits[DCF77_GRID_SLOTS];
    uint8_t dcf77BitIndex = 0;
    bool dcf77Synced = false;
    uint8_t dcf77Phase = 0;
    uint8_t dcf77MarkerMiss[60] = { 0 };
    uint8_t dcf77MarkerHit[60] = { 0 };
    int8_t dcf77MarkerPos = -1;
    int8_t dcf77LastSecond = -1;
    uint8_t dcf77StructFails = 0;
    uint32_t dcf77PulsesSeen = 0;
    uint32_t dcf77PulsesMissed = 0;
    uint32_t dcf77PhaseBreaks = 0;
    uint16_t dcf77DiagWidth[DCF77_DIAG_SLOTS] = { 0 };
    uint16_t dcf77DiagGap[DCF77_DIAG_SLOTS] = { 0 };
    uint8_t dcf77DiagIdx = 0;
    uint8_t dcf77DiagCount = 0;
    Dcf77Decoded dcf77LastDecoded;
    time_t dcf77PrevEpoch = 0;
    unsigned long dcf77PrevAtMillis = 0;
    bool ntpServerRunning = false;
    uint32_t ntpRequestsReceived = 0;
    uint32_t ntpRepliesSent = 0;
    unsigned long lastNTPUpdate = 0;
    unsigned long lastDCFUpdate = 0;
    unsigned long lastRTCUpdate = 0;
    bool serialTimeSet = false;
    volatile unsigned long lastNtpSuccessMillis = 0;
    TaskHandle_t ntpSyncTaskHandle = NULL;
    volatile bool ntpSyncTaskRunning = false;
    volatile bool ntpSyncTaskDone = false;
    volatile bool ntpSyncTaskResult = false;
    unsigned long ntpSyncCheckBeforeMillis = 0;
    String ntpSyncCheckLabel = "";
    struct tm timeinfo;
    struct tm rocrailTimeinfo;
    bool rocrailEnabled = false;
    bool rocrailConnected = false;
    String rocrailServerHost = "";
    uint16_t rocrailServerPort = ROCRAIL_DEFAULT_PORT;
    char rocrailServerHostSnapshot[64];
    uint16_t rocrailServerPortSnapshot = ROCRAIL_DEFAULT_PORT;
    char rocrailServerList[MAX_WLAN][64];
    uint16_t rocrailServerPortList[MAX_WLAN];
    char rocrailServerNameList[MAX_WLAN][40];
    int rocrailActiveServerIndex = -1;
    uint8_t rocrailDivider = 1;
    bool rocrailFrozen = false;
    uint8_t rocrailBrightness = 255;
    bool rocrailBrightnessKnown = false;
    WiFiClient rocrailClient;
    String rocrailRxBuffer;
    uint8_t rocrailLogChunkCount = 0;
    uint8_t rocrailClockLogCount = 0;
    uint8_t rocrailConnectFailLogCount = 0;
    unsigned long rocrailLastClockMillis = 0;
    float rocrailDisplaySeconds = 0.0f;
    unsigned long rocrailLastAdvanceMillis = 0;
    float rocrailDriftSeconds = 0.0f;
    float rocrailSecFrac = 0.0f;
    unsigned long rocrailLastConnectAttemptMillis = 0;
    TaskHandle_t rocrailConnectTaskHandle = NULL;
    volatile bool rocrailConnectTaskRunning = false;
    volatile bool rocrailConnectTaskDone = false;
    volatile bool rocrailConnectTaskResult = false;
    String timezone = TIMEZONE_DEFAULT;
    char timezoneSnapshot[64];
    char ntpServers[MAX_WLAN][64];
    char ntpServersSnapshot[MAX_WLAN][64];
    int rtcOk = RTC_NOT_AVAILABLE;
    String i2cAddr = "";

    // Zifferblatt / Display
    // Clock face / Display

    String tftType = "UNKNOWN";
    LGFX_Sprite backgroundSprite(&tft);
    LGFX_Sprite hourHandSprite(&tft);
    LGFX_Sprite minuteHandSprite(&tft);
    LGFX_Sprite secondHandSprite(&tft);
    LGFX_Sprite statusSprite1(&tft);
    LGFX_Sprite statusSprite2(&tft);
    bool statusSprite1Created = false;
    bool statusSprite2Created = false;
    String selectedBackground = "/face_default.bmp";
    bool stationMode;
    bool smoothMinute;
    bool smoothSecond;
    bool showSecondHand;
    int hourHandWidth = HAND_WIDTH;
    int minuteHandWidth = HAND_WIDTH;
    int secondHandWidth = HAND_WIDTH;

    // nabe
    // hub

    uint16_t hubColor = 0;
    uint8_t hubSize = 0;
    bool firstRun = true;
    bool firstRun2 = true;
    uint8_t tftRotation1 = TFT_ROTATION1_DEFAULT;
    bool autoRotation = false;
    uint8_t tftRotation2 = TFT_ROTATION2_DEFAULT;
    uint16_t rowBuffer[CLOCK_MAX];
    bool gc9d01SwRotation = false;
    uint16_t* clockFaceBuffer = nullptr;
    uint8_t* clockFaceRle = nullptr;
    size_t clockFaceRleSize = 0;
    String clockFaceRlePath = "";
    bool clockFaceRleReady = false;
    HandComposite handComposite[2];
    ClockFrameKey lastClockFrame[2];
    bool lastRenderPartial = false;
    bool partialUpdateEnabled = true;
    uint32_t compositeBuildCount = 0;
    RenderStats renderStats;
    int32_t lastRenderRect[4] = { 0, 0, 0, 0 };
    bool clockFrameDirty[2] = { true, true };
    bool infoStripDirty[2] = { true, true };
    uint32_t stripBgRgb = 0x000000;
    uint32_t stripFgRgb = 0xFFFFFF;
    uint8_t stripFont = 0;
    bool stripBlink = true;
    bool stripBefore = false;
    int16_t stripTimeX = -1;
    int16_t stripTimeY = -1;
    int16_t stripDateX = -1;
    int16_t stripDateY = -1;
    int16_t stripAutoTimeY = 0;
    int16_t stripAutoDateY = 0;
    LGFX_Sprite infoStripSprite(&tft);
    bool infoStripSpriteCreated = false;
    uint16_t* stripImage = nullptr;
    String stripImageFor = "?";
    uint8_t stripTimeFmt = 0;
    uint8_t stripSeconds = 0;
    uint8_t stripDateFmt = 0;
    uint8_t stripWeekdayFmt = 0;
    uint8_t* stripVlwTime = nullptr;
    uint8_t* stripVlwDate = nullptr;
    bool stripVlwStale = true;
    uint32_t stripVlwGeneration = 0;
    String stripVlwName = "";
    uint8_t stripVlwTimeSize = 44;
    uint8_t stripVlwDateSize = 22;
    int16_t secPivotX = -1;
    int16_t secPivotY = -1;
    int16_t hmPivotX = -1;
    int16_t hmPivotY = -1;
    uint8_t secOrder = 0;
    String faceSettingsFor = "?";
    String stripSettingsFor = "?";
    uint8_t stripTimeScale = 100;
    uint8_t stripDateScale = 100;
    bool stripShowTime = true;
    bool stripShowDate = true;
    bool stripShowWeekday = false;
    uint8_t stripWeekdayScale = 100;
    uint8_t stripVlwWeekdaySize = 22;
    int16_t stripWeekdayX = -1;
    int16_t stripWeekdayY = -1;
    int16_t stripAutoWeekdayY = 0;
    uint8_t* stripVlwWeekday = nullptr;
    int8_t stripWeekdayLine = -1;
    uint32_t infoOverlayRevision = 0;
    uint32_t infoOverlayAppliedKey = 0;
    uint32_t infoOverlayRevApplied = 0;
    int16_t infoOverlayBox[4] = { 0, 0, -1, -1 };
    int16_t infoOverlayBoxNew[4] = { 0, 0, -1, -1 };
    int16_t infoOverlayPending[4] = { 0, 0, -1, -1 };
    int16_t stripPreviewBox[4] = { 0, 0, -1, -1 };
    uint32_t clockAssetGeneration = 1;
    uint16_t* clockFaceBrightBuffer = nullptr;
    bool displayNeedsBlank[2] = { true, true };
    bool adcInverted = false;
    bool useAdc = false;
    bool photoresistorFound = false;
    uint8_t currentBrightness = 255;
    uint8_t brightnessTestValue = 0;
    unsigned long brightnessTestUntil = 0;
    int16_t brightnessOverride = -1;
    uint8_t lastAppliedBrightness = 255;
    uint8_t lastHandBrightness = 255;
    uint8_t targetBrightness = 255;
    int lowThreshold = 40;
    int highThreshold = 60;
    uint8_t minBrightness = 75;
    uint8_t maxBrightness = 255;
    bool startupFaceDrawn = false;

    // Zeitabhängige Helligkeit
    // Time-dependent brightness

    uint8_t brightStartHour = 8;
    uint8_t brightEndHour = 22;
    float gammaBrightness = 2.2f;
    bool useBacklight = false;
    bool backlightAttached = false;
    int adcHistory[ADC_SMOOTHING];
    int adcIndex = 0;
    int currentAdcAvg = 0;
    int currentLightPercent = 0;
    unsigned long lastAdcSampleMillis = 0;
    int pendingBrightnessState = 0;
    unsigned long brightnessThresholdSinceMillis = 0;

    // Presets
    // Presets

    Preset presets[MAX_PRESETS];

    // Datei-Upload / Wartung
    // File upload / maintenance

    File uploadFile;
    String uploadFilePath = "";
    bool uploadSuccess = false;
    int lastResetWeek = -1;
    int currentWeek = -1;

    // Uebersetzungen fuer verschiedene Sprachen
    // Translations for various languages
