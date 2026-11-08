#pragma once
#include "globals.h"

    // ota_update.cpp - Firmware-Update ueber WLAN (nur ESP32-S3)
    // ota_update.cpp - Firmware update over WiFi (ESP32-S3 only)

    void showOtaStatus(const String& line2) ;
    void showOtaProgress(size_t done, size_t total) ;
    void startArduinoOta() ;
    bool handleOta() ;
    void handleFirmwareUpload() ;
    void setupOtaRoutes() ;
    bool checkFirmwareUpdate() ;
    bool installFirmwareFromGithub(String& error, bool force = false) ;
    int firmwareUpdateState() ;
