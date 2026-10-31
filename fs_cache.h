// Zwischenspeicher fuer langsame LittleFS-Abfragen. Auf der 9,9-MB-Partition kostet usedBytes() und totalBytes() je
// ca. 82 ms, ein Durchlauf durch das Wurzelverzeichnis ca. 330 ms (33 Dateien) und exists() ca. 9 ms - waehrend einer
// Webseite steht die Hauptschleife und damit das Zeichnen (der Sekundenzeiger bleibt stehen). Nach dem Einbinden
// ersetzt ein Makro LittleFS durch cachedFs: Alle Schreibzugriffe (open zum Schreiben, remove, rename, mkdir, format,
// begin) laufen darueber und verwerfen die Zwischenspeicher, daher sind die Werte nie aelter als der letzte Schreibzugriff.
//
// Cache for slow LittleFS queries. On the 9.9 MB partition usedBytes() and totalBytes() cost about 82 ms each, a walk
// through the root directory about 330 ms (33 files) and exists() about 9 ms - while a web page is built the main
// loop and thus the drawing stand still (the second hand stops). After including it a macro replaces LittleFS by
// cachedFs: all write accesses (open for writing, remove, rename, mkdir, format, begin) go through it and discard
// the caches, so the values are never older than the last write access.

#pragma once

#include <LittleFS.h>
#include <map>
#include <vector>

struct FsEntry {
    String name;
    size_t size;
    bool dir;
    time_t modified;
};

// Wird bei jedem Schreibzugriff aufgerufen (gesetzt in loop(), sobald setup() fertig ist) - auf dem 480x480-Board
// zeigt das Display dann eine ruhige Flaeche, weil Flash-Schreibzugriffe das Bild stoeren.
// Called on every write access (set in loop() once setup() is done) - on the 480x480 board the display then shows a
// calm area, because flash writes disturb the image.
inline void (*fsWriteHook)(const char* path) = nullptr;

class CachedFs {
public:
    bool begin(bool formatOnFail = false, const char* basePath = "/littlefs", uint8_t maxOpenFiles = 10, const char* partitionLabel = "spiffs") {
        invalidate();
        return ::LittleFS.begin(formatOnFail, basePath, maxOpenFiles, partitionLabel);
    }
    void end() { invalidate(); ::LittleFS.end(); }
    bool format() { invalidate(); return ::LittleFS.format(); }

    // Zum Schreiben/Anhaengen/Erzeugen geoeffnet = Aenderung; "r" und das Wurzelverzeichnis aendern nichts
    // Opened for writing/appending/creating = change; "r" and the root directory change nothing
    File open(const char* path, const char* mode = FILE_READ, const bool create = false) {
        if (create || (mode && mode[0] != 'r') || (mode && strchr(mode, '+'))) touch(path);
        return ::LittleFS.open(path, mode, create);
    }
    File open(const String& path, const char* mode = FILE_READ, const bool create = false) { return open(path.c_str(), mode, create); }

    bool exists(const char* path) {
        if (isLog(path)) return ::LittleFS.exists(path);
        expire();
        auto it = existsCache.find(path);
        if (it != existsCache.end()) return it->second;
        bool e = ::LittleFS.exists(path);
        if (existsCache.size() >= 96) existsCache.clear();
        existsCache[path] = e;
        return e;
    }
    bool exists(const String& path) { return exists(path.c_str()); }

    bool remove(const char* path) { touch(path); return ::LittleFS.remove(path); }
    bool remove(const String& path) { return remove(path.c_str()); }
    bool rename(const char* from, const char* to) { touch(from); touch(to); return ::LittleFS.rename(from, to); }
    bool rename(const String& from, const String& to) { return rename(from.c_str(), to.c_str()); }
    bool mkdir(const char* path) { touch(path); return ::LittleFS.mkdir(path); }
    bool mkdir(const String& path) { return mkdir(path.c_str()); }

    // totalBytes aendert sich nie; usedBytes hoechstens 5 s alt (nur Anzeige - Entscheidungen ueber freien Platz mit usedBytesFresh())
    // totalBytes never changes; usedBytes at most 5 s old (display only - decisions about free space use usedBytesFresh())
    size_t totalBytes() {
        if (!totalKnown) { total = ::LittleFS.totalBytes(); totalKnown = true; }
        return total;
    }
    size_t usedBytes() {
        if (!usedValid || millis() - usedAt > 5000) {
            used = ::LittleFS.usedBytes();
            usedAt = millis();
            usedValid = true;
        }
        return used;
    }
    size_t usedBytesFresh() { usedValid = false; return usedBytes(); }

    // Wurzelverzeichnis als Liste (Name, Groesse, Verzeichnis, Aenderungszeit), hoechstens 15 s alt und nach jedem Schreibzugriff
    // neu; fresh = true erzwingt eine neue Liste. Nur lesen, waehrend der Schleife nicht erneut aufrufen.
    // Root directory as a list (name, size, directory, modification time), at most 15 s old and renewed after every write
    // access; fresh = true forces a new list. Read only, do not call again inside the loop.
    const std::vector<FsEntry>& rootEntries(bool fresh = false) {
        if (fresh || !rootValid || millis() - rootAt > 15000) {
            rootList.clear();
            File root = ::LittleFS.open("/");
            if (root) {
                for (File f = root.openNextFile(); f; f = root.openNextFile()) {
                    rootList.push_back({ String(f.name()), f.size(), f.isDirectory(), f.getLastWrite() });
                }
                root.close();
            }
            rootValid = true;
            rootAt = millis();
        }
        return rootList;
    }

    void invalidate() {
        existsCache.clear();
        rootValid = false;
        usedValid = false;
    }

private:
    static bool isLog(const char* path) {
        size_t n = strlen(path);
        return n > 4 && strcmp(path + n - 4, ".log") == 0;
    }

    // Schreibzugriff auf path: Logdateien wachsen laufend und aendern Verzeichnis und exists() nur beim Anlegen/Loeschen
    // (Verzeichnisliste hoechstens 15 s alt, exists() fuer Logdateien nie gespeichert) - nur der Belegungswert wird verworfen.
    // Write access to path: log files grow continuously and change the directory and exists() only when created/deleted
    // (directory list at most 15 s old, exists() never cached for log files) - only the usage value is discarded.
    void touch(const char* path) {
        if (isLog(path)) { usedValid = false; return; }
        invalidate();
        if (fsWriteHook) fsWriteHook(path);
    }

    void expire() {
        if (millis() - existsAt > 60000) { existsCache.clear(); existsAt = millis(); }
    }

    std::map<String, bool> existsCache;
    std::vector<FsEntry> rootList;
    size_t total = 0, used = 0;
    unsigned long usedAt = 0, rootAt = 0, existsAt = 0;
    bool totalKnown = false, usedValid = false, rootValid = false;
};

inline CachedFs cachedFs;

#define LittleFS cachedFs
