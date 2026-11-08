#pragma once
#include "globals.h"

    // open_wifi_time.cpp - Uhrzeit aus offenen WLANs beim Start (TLS)
    // open_wifi_time.cpp - Time from open WiFis at boot (TLS)

    time_t httpDateEpoch(const String& target, const String& path, String& note, String& location, bool tls) ;
    time_t openWifiQueryTime(const String& ssid, String& how) ;
    bool fetchTimeFromOpenWifi(bool force = false) ; // force: auch mit gueltiger Zeit/RTC/ausgeschaltet (Diagnose "UHR4 OPENWIFI")
                                                      // force: also with a valid time/RTC/switched off (diagnostic "UHR4 OPENWIFI")
