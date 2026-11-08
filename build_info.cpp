#include "uhr4.h"

    // Build-Zeit der Firmware (Kompilierzeit). Sie steht in einer eigenen kleinen Datei: buildAll.bat loescht vor
    // jedem Lauf deren Objektdatei, damit __DATE__ und __TIME__ den Zeitpunkt dieses Builds zeigen, auch wenn nur
    // eine andere .cpp geaendert wurde.

    // Build time of the firmware (compile time). It lives in a small file of its own: buildAll.bat deletes its
    // object file before every run so that __DATE__ and __TIME__ show the time of this build, even if only another
    // .cpp changed.

    const char fwBuildMarker[] __attribute__((used)) = "UHR4_FW_BUILD=" __DATE__ " " __TIME__;
    const char fwBuildDate[] = __DATE__;
    const char fwBuildTime[] = __TIME__;
