#pragma once
#include "globals.h"

    // backup.cpp - Komplettsicherung und -wiederherstellung (TAR)
    // backup.cpp - Full backup and restore (TAR)

    void backupWipe(String& s) ;

#define BACKUP_SETTINGS_NAME   "settings.txt"

    // Wiederherstellen: Zustand ueber die Upload-Stuecke hinweg
    // Restore: state across the upload chunks

    struct BackupRestoreState {
        enum Phase { HEADER, DATA, PAD, END, FAILED } phase = HEADER;
        uint8_t header[512];
        size_t headerFill = 0;
        size_t remaining = 0;
        size_t padding = 0;
        bool firstHeader = true;   // der erste Kopf muss settings.txt sein
                                   // the first header must be settings.txt
        bool toSettings = false;
        String settings;
        File out;
        String bmpTarget;          // Bild, das gerade nach BACKUP_TMP_PATH geschrieben wird
                                   // image currently being written to BACKUP_TMP_PATH
        std::vector<String> restoredFiles;
        std::vector<String> tempHandIds; // Nummern von Zeigersaetzen der Sicherung, die hier die Firmware belegt
                                         // numbers of hand sets in the backup that the firmware occupies here
        String error;

        // Formularfelder (stehen im Formular VOR der Datei, damit sie beim
        // Upload-Start schon vorliegen), Ergebnis der Pruefung von settings.txt

        // Form fields (placed BEFORE the file in the form so they are already
        // available at upload start), result of checking settings.txt

        bool restoreWifi = false;
        bool settingsChecked = false; // ab hier wurde an der Uhr etwas geaendert
                                      // from here on something was changed on the clock
        bool applyWifi = false;
        bool keepBrightness = false;  // anderer Backlight-Modus / other backlight mode
        bool otherType = false;       // Sicherung mit anderem Displaytyp / backup of another display type
        int backupType = 0;           // Displaytyp der Sicherung / display type of the backup
        String backupDisplay;         // Displaytyp der Sicherung wie im Dateinamen / backup's display type as in the file name
        String wifiPlain;             // entschluesselte WLAN-Zeilen / decrypted WiFi lines

        ~BackupRestoreState() {
            backupWipe(wifiPlain);
        }
    };

    String backupHex(const uint8_t* data, size_t len) ;
    int backupUnhex(const String& hex, uint8_t* out, size_t maxLen) ;
    void collectLogFiles(std::vector<String>& names, std::vector<size_t>& sizes) ;
    void appendUtf8(String& out, uint32_t cp) ;
    String htmlToText(const String& html) ;
    String buildStatusText() ;
    void streamBackup(bool includeWifi) ;
    void handleBackupRestoreUpload() ;

    extern BackupRestoreState* backupRestore;
