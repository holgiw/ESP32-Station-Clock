#pragma once
#include "globals.h"

    // presets_manager.cpp - Presets: Laden, Speichern und Wechseln vordefinierter Anzeigekonfigurationen
    // presets_manager.cpp - Presets: load, save and switch predefined display configurations

    void migrateStripSettings() ;
    void removeOrphanedStrips() ;
    String stripRotationParam(const String& url) ;
    void loadPresets() ;
    void savePresets() ;
    String presetUrlEncode(const String& value) ;
    String buildPresetUrl(const String& face, const String& handSet, bool stationMode, bool showSecondHand, bool smoothMinute, bool smoothSecond, uint8_t hubSize, uint32_t hubColor) ;
    String setPresetUrlParam(const String& url, const String& key, const String& value) ;
    int rewriteHandSetRefs(const std::function<String(const String&)>& fn) ;
    void finishHandSetRenumber(String plan) ;
    void recoverHandSetRenumber() ;
    void renumberHandSets() ;
    bool stripVlwSizes(const char* name, int& minSize, int& maxSize) ;
    void addStarterPresets() ;
    void ensureStarterSet() ;
    void refreshGeneratedAssets() ;
    String presetUrlDecode(const String& value) ;
    bool createPresetFromPreferences(const String& customName = "") ;
    void parsePresetForPreview(const String& url, String& faceOut, String& handSetOut, uint16_t& hubColorOut, uint8_t& hubSizeOut, bool& showSecondOut) ;
    void removeOrphanedPresets(const String& deletedFace, const String& deletedHandSet) ;
    void resetAllPresets() ;
