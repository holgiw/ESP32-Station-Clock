#pragma once
#include "globals.h"

    // wifi_manager.cpp - WLAN: Verbindungsaufbau, Access-Point, Scan, Reconnect
    // wifi_manager.cpp - WiFi: connection setup, access point, scan, reconnect

    void startWPS() ;
    void applyWifiTxPower() ;
    bool wifiLinkWorks() ;
    void onWifiStartEvent(arduino_event_id_t event, arduino_event_info_t info) ;
    String wifiDisconnectReasonText(uint8_t reason) ;
    void onWifiDisconnectEvent(arduino_event_id_t event, arduino_event_info_t info) ;
    bool checkWiFiReconnect() ;
    void wipeWifiDriverStorage() ;
    bool wifiStoreCrypt(bool encrypt, const uint8_t* iv, uint8_t* tag, const uint8_t* in, uint8_t* out, size_t len) ;
    String wifiPassEncrypt(const String& plain) ;
    String wifiPassDecrypt(const String& stored) ;
    String loadWifiPass(int i) ;
    void storeWifiPass(int i, const String& pass) ;
    bool storeWifiPassVerified(int i, const String& pass) ;
    void migrateWifiPasswords() ;
    bool readWpsCredentials(String& ssid, String& pass) ;
    int saveWpsCredentials(const String& ssid, const String& pass) ;
    void onWpsEvent(WiFiEvent_t event) ;
    void restorePreviousWpsConnection() ;
    void startAP() ;
    String buildLabel() ; // "Build " + Version fuer die WLAN-Anzeigen / "Build " + version for the WiFi screens
    void showStartScreen() ; // Build und WLAN gleich nach dem Panel-Start (siehe wifi_manager.cpp) / build and WiFi right after the panel start (see wifi_manager.cpp)
    void drawWifiSymbol(int cx, int cy, int lit) ; // WLAN-Symbol einmal zeichnen (siehe wifi_manager.cpp) / draw the WiFi symbol once (see wifi_manager.cpp)
    int connectWiFi(int number, bool verboseMode, bool wpa2Only = false) ; // wpa2Only: Verbindung ohne WPA3 (siehe wifi_manager.cpp)
                                                                            // wpa2Only: connect without WPA3 (see wifi_manager.cpp)
    int connectWiFiWithRetries(int number, const String& label, bool verboseMode = true) ; // ruft connectWiFi() bis zu WIFI_CONNECT_ATTEMPTS mal auf (siehe wifi_manager.cpp)
                                                                                            // calls connectWiFi() up to WIFI_CONNECT_ATTEMPTS times (see wifi_manager.cpp)
    void animateCursor(int x, int y, int delayMs) ;
    void showWlanCredentials(String wlan) ;
    void showApInfo() ; // Access-Point-Zugangsdaten auf dem Display / access point credentials on the display
    void eraseWiFiConfig() ;
    void startWiFiScan() ;
    void collectStrongestNetworks(int totalFound) ;
    void checkWiFiScan() ;
    void scanAndCacheNetworks() ;
    bool putStringVerified(const char* key, const String& value) ;
