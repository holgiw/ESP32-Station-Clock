#pragma once
#include "globals.h"

    // time_sync.cpp - Zeit: RTC, DCF77, NTP-Client und -Server, Zeitzone
    // time_sync.cpp - Time: RTC, DCF77, NTP client and server, timezone

    void IRAM_ATTR isr() ;
    void logTimeSyncDifference(const String& source, const struct timeval& oldTime, unsigned long oldTimeMillis) ; // loggt Alt-/Neu-Zeitdifferenz nach einer Synchronisation (siehe time_sync.cpp)
                                                                                                                  // logs old/new time difference after a sync (see time_sync.cpp)
    void loadTimeFromRTC() ;
    void applyNtpServerDefaultsIfNoneConfigured() ; // Fallback auf NTP_SERVER_1/2, wenn ntpServers[] komplett leer ist (siehe time_sync.cpp)
                                                    // fallback to NTP_SERVER_1/2 when ntpServers[] is completely empty (see time_sync.cpp)
    void applyTimezoneDefaultIfInvalid() ; // Fallback auf TIMEZONE_DEFAULT, wenn `timezone` leer oder ungueltig ist (siehe time_sync.cpp)
                                           // fallback to TIMEZONE_DEFAULT when `timezone` is empty or invalid (see time_sync.cpp)
    void applyTimezoneToSystem() ; // setzt `timezone` sofort als TZ der Systemzeit (siehe time_sync.cpp)
                                   // applies `timezone` right away as the system time's TZ (see time_sync.cpp)
    bool isValidPosixTimezone(const String& tz) ; // Grammatik-/Wertebereichspruefung eines POSIX-TZ-Strings (siehe time_sync.cpp)
                                                  // grammar/range check of a POSIX TZ string (see time_sync.cpp)
    void initializeNtpServers() ;
    struct tm dcf77DecodedToLocalTm() ;
    long rtcDriftSec(struct tm newLocal) ; // Abweichung der RTC-Zeit von newLocal in Sekunden (siehe time_sync.cpp)
                                           // deviation of the RTC time from newLocal in seconds (see time_sync.cpp)
    bool updateDcf77Status() ;
    bool applyDcf77DecodedTime(String source) ;
    void checkDcf77Health() ;
    bool decodeDcf77Telegram(unsigned long decodedAtMillis) ; // false = Telegramm strukturell unmoeglich, Synchronisation verwerfen (siehe processDcf77Bits())
                                                              // false = telegram structurally impossible, drop synchronization (see processDcf77Bits())
    void resetDcf77MarkerStats() ;
    void evaluateDcf77Marker() ;
    void processDcf77Bits() ;
    void checkRtcHealth() ;
    String testNtpServer(const String& server) ;
    time_t ntpQueryEpoch(const String& server) ;
    boolean setupNTP() ; // blockierender Worker - nicht direkt aufrufen, siehe startNtpSyncTask()
                        // blocking worker - do not call directly, see startNtpSyncTask()
    void ntpSyncTaskFunc(void* param) ;
    void startNtpSyncTask(String label) ;
    void pollNtpSyncTask() ;
    void stopNtpSyncTaskIfRunning() ; // hartes Beenden vor Reboot/Werksreset - siehe system_utils.cpp
                                      // hard stop before reboot/factory reset - see system_utils.cpp
    void handleNTPFailure() ;
    void setTimeStruct(const struct tm& timeinfo, String source) ;
    bool setClockTime(const String& arg, const char* source, String& localText) ;
    void handleSerialTime(const String& arg) ;
    uint16_t i2cScan() ;
    bool startNtpServer() ;
    void createNtpResponse(byte* packet, const struct timeval& receivedAt) ;
