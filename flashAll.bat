@echo off
rem Prueft alle Uhren im Netz und bringt veraltete auf den Stand der Builds in build_uhr4 (gebaut mit buildAll.bat):
rem   ESP32-S3 per WLAN (OTA), ESP32-S2 und -C6 per USB (sie haben kein OTA). Vor jedem Flash wird gefragt (Enter = Ja).
rem   flashAll.bat               pruefen, dann fragen und flashen
rem   flashAll.bat -CheckOnly    nur pruefen
rem   flashAll.bat -Yes          ohne Rueckfragen
rem   flashAll.bat -Force        auch aktuelle S3 neu einspielen
rem Checks all clocks on the network and brings outdated ones to the state of the builds in build_uhr4 (built with
rem buildAll.bat): ESP32-S3 over WiFi (OTA), ESP32-S2 and -C6 via USB (they have no OTA). It asks before every flash
rem (Enter = yes).
rem   flashAll.bat               check, then ask and flash
rem   flashAll.bat -CheckOnly    only check
rem   flashAll.bat -Yes          without questions
rem   flashAll.bat -Force        also re-flash up-to-date S3
setlocal
cd /d "%~dp0"
if not exist "%~dp0flashAll.ps1" (
    echo flashAll.ps1 fehlt / is missing.
    pause
    exit /b 1
)
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0flashAll.ps1" %*
set "RESULT=%errorlevel%"
if "%~1"=="" pause
exit /b %RESULT%
