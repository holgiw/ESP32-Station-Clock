#pragma once
#include "globals.h"

    // rocrail_client.cpp - Rocrail-Modellzeit: Discovery, TCP-Client, Auswertung der Uhrzeit
    // rocrail_client.cpp - Rocrail model time: discovery, TCP client, clock parsing

    void pollRocrailClient() ;
    void connectRocrailClient() ;
    void triggerRocrailConnectNow() ;
    void setRocrailEnabled(bool on) ;
    void startRocrailConnectTask() ;
    void loadRocrailServerList() ;
    void rocrailConnectTaskFunc(void* param) ;
    void pollRocrailConnectTask() ;
    void processRocrailBuffer() ;
    void processRocrailClockPayload(const String& payload) ;
    void advanceRocrailTime() ;
    int rocrailXmlAttrInt(const String& tag, const char* attr, int fallback) ;
    String rocrailXmlAttrString(const String& tag, const char* attr) ;
    bool shouldLogThrottled(uint8_t& counter, bool& isLastLogged, uint8_t limit = ROCRAIL_LOG_THROTTLE_LIMIT) ; // Drosselung wiederkehrender Log-Zeilen (siehe rocrail_client.cpp)
                                                                                                                 // throttling for recurring log lines (see rocrail_client.cpp)

    // R2RNet-Multicast-Diagnose (siehe rocrail_client.cpp) - keine echte
    // Discovery, nur Logging eingehender Pakete zur Formatanalyse.

    // R2RNet multicast diagnostics (see rocrail_client.cpp) - not real
    // discovery, just logging incoming packets for format analysis.

    bool startR2rnetDebugListener() ;
    void pollR2rnetDebugListener() ;
