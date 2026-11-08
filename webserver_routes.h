#pragma once
#include "globals.h"

    // webserver_routes.cpp - Webinterface: alle HTTP-Routen und HTML-Erzeugung
    // webserver_routes.cpp - Web interface: all HTTP routes and HTML generation

    String selectWithoutReloadScript(const String& message, const String& activeText) ;
    void applyWlanList(String newSsid[MAX_WLAN], String newPass[MAX_WLAN]) ; // schreibt eine komplette WLAN-Liste in Preferences + RAM (siehe webserver_routes.cpp)
    void executePendingAction(String action) ; // fuehrt eine der acht bestaetigten/direkt erlaubten Aktionen aus (siehe webserver_routes.cpp)
                                               // executes one of the eight confirmed/directly-allowed actions (see webserver_routes.cpp)
                                                                             // writes a complete WiFi list to preferences + RAM (see webserver_routes.cpp)
    String generateHtmlHeader(String extraHead = "") ;
    String simpleMessagePage(String heading, String bodyHtml, String extraHead = "") ;
    String generateTopBar() ;
    String getTimeStatus() ;
    String getRtcStatus() ;
    String getDcf77Status() ;
    String dotStatusText(const String& label, const String& state) ;
    bool isSameSubnet(IPAddress ip, IPAddress ownIp, IPAddress mask) ; // byteweiser Subnetzvergleich (siehe webserver_routes.cpp)
                                                                       // byte-wise subnet comparison (see webserver_routes.cpp)
    bool isPrivateNetworkIp(IPAddress ip) ; // im selben Netz wie die Uhr (STA oder AP) oder RFC1918-Rueckfall? (siehe webserver_routes.cpp) - blendet den Status-Tab bei externem Zugriff aus
                                            // in the same network as the clock (STA or AP) or RFC1918 fallback? (see webserver_routes.cpp) - hides the Status tab on external access
                                            // RFC1918 range? (see webserver_routes.cpp) - hides the Status tab on external access
    String escapeHtmlText(const String& text) ;
    bool deleteFileWithSideEffects(const String& path) ; // Datei samt Presets/Streifen-Folgen loeschen (siehe webserver_routes.cpp)
                                                         // delete a file incl. preset/strip consequences (see webserver_routes.cpp)
    String fileManagerReturnTarget(const String& from) ; // /delete- und /rename-Rueckspring-Ziel anhand des "from"-Parameters (siehe webserver_routes.cpp)
                                                         // /delete and /rename return target based on the "from" parameter (see webserver_routes.cpp)
    String escapeJsonText(const String& text) ;
    String dayWindowText() ;
    String generateStorageInfo(size_t used, size_t total, bool forceEnglish = false) ;
    String withBacklightPin(String text) ;
    String infoTip(const String& text) ;
    String checkboxRow(const char* name, bool checked, const String& label, const String& tip, const char* id = "") ;
    String generateFlashMessage() ;
    String generateNavigation() ;
    String currentPreviewSignature() ;
    String generateSettingsTabNav() ;
    String generateLanguageSelector() ;
    String resetReasonToString(esp_reset_reason_t reason) ;
    void generateStatusItems(String& chunk, std::function<void(String&)> flush) ;
    String rtcStatusToString(int status) ;
    String formatDurationMs(unsigned long ms) ;
    bool naturalLess(const String& a, const String& b) ;
    void naturalSortNames(std::vector<String>& names) ;
    void redirectTo(const String& location, const String& body = "") ;
    String beginPage() ;
    void updateNtpServersFromRequest() ;
    void switchPreset(int direction) ;
    int presetPickCount() ; // Presets fuer die Touch-Auswahl / presets for the touch picker
    bool presetPickInfo(int pos, String& name, String& face, String& handSet, bool& active, uint16_t& hubColor, uint8_t& hubSize, bool& showSecond) ;
    void presetPickApply(int pos) ;
    String sanitizeHostname(String input) ;
    void setupWebServer() ;
    String completeUploadName(const String& path, const String& uri) ;
    void handleFileUpload() ;
