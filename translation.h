#pragma once
#include "globals.h"

    // translation.cpp - Uebersetzung der Oberflaeche
    // translation.cpp - Translation of the interface

    void loadLanguage();
    void saveLanguage(String lang);
    String translate(const String& key);

    extern const std::set<String> availableLanguages;
