#pragma once
#include "globals.h"

    // system_utils.cpp - Systemfunktionen: Tasten, Touch, Logging, Reset, Neustart, Hilfsfunktionen
    // system_utils.cpp - System functions: buttons, touch, logging, reset, restart, helpers

#if BOARD_WAVESHARE_S3_LCD4
    String lcd4TouchDebug() ;
#endif
    void drawTextRow(lgfx::LovyanGFX& g, float yShare, const String& text, uint16_t color, const lgfx::IFont* const* fontList, bool clear) ;
    void showButtonMessage(uint16_t color, const String& title, const String& line2, const String& hint, uint16_t hintColor) ;
    void updateButtonLine(uint16_t color, const String& line2) ;
    void checkButton() ;
    void checkTouch() ;
    String factoryResetActionLabel(const String& action) ; // kurze Beschriftung fuer Display + Web-Eingabeseite (siehe system_utils.cpp)
                                                            // short label for display + web entry page (see system_utils.cpp)
    void requestConfirmationCode(const String& action) ; // erzeugt/zeigt einen neuen Bestaetigungscode (siehe system_utils.cpp)
    bool rejectIfConfirmationPending() ; // true + Hinweisseite, wenn schon ein anderer Code aussteht - vor pendingWifi*-Zuweisungen aufrufen (siehe system_utils.cpp)
                                        // true + hint page if a different code is already pending - call before pendingWifi* assignments (see system_utils.cpp)
                                                          // generates/shows a new confirmation code (see system_utils.cpp)
    bool checkFactoryResetCodePending() ; // siehe system_utils.cpp - true, waehrend ein Bestaetigungscode angezeigt wird
                                         // see system_utils.cpp - true while a confirmation code is being displayed
    void checkWeeklyRestart() ;
    void eraseAllNVS() ;
    void factoryReset() ;
    void espReboot() ;
    String getCurrentLogFileName() ;
    void deleteAllLogFiles() ;
    void checkHeapWarning(const String& context) ;
    void logToFile(const String& message) ;
    String logFileHeader() ;
    void flushLogBuffer() ;
    void checkLogFlush() ;
    void checkLoggingExpiry() ;
    String trim(const String& str) ;
    bool getSmoothSecondPref(bool stationModeFallback) ; // liest PK_SMOOTH_SECOND mit Migrations-Fallback auf stationMode (siehe system_utils.cpp)
                                                          // reads PK_SMOOTH_SECOND with a migration fallback to stationMode (see system_utils.cpp)
