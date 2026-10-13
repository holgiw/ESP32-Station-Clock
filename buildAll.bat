@echo off
rem Baut die Firmware mit arduino-cli (der Arduino IDE) fuer ESP32-S3, ESP32-S2, ESP32-C6 und das Waveshare-480x480-Board nacheinander.
rem   buildAll.bat              alle vier Ziele (Reihenfolge s3, s2, c6, s3lcd4); das Qualia nur, wenn es genannt wird
rem   buildAll.bat s3 c6        nur die genannten Chips
rem   buildAll.bat s3lcd4       nur das Waveshare ESP32-S3-Touch-LCD-4 (480x480, PSRAM octal)
rem   buildAll.bat s3qualia      nur das Adafruit Qualia ESP32-S3 mit dem 2,1" runden Display (480x480, PSRAM octal; nie automatisch, ungetestet)
rem Nach jedem erfolgreichen Build werden die fertigen Dateien nach build_uhr4\esp32s3, esp32s2, esp32c6, esp32s3lcd4 bzw. esp32s3qualia kopiert.
rem Die Zwischenstaende liegen in %TEMP%\uhr4-build\[chip] (spaetere Laeufe sind schneller), das Protokoll in
rem %TEMP%\uhr4-build\[chip].log. Die Chips werden nacheinander gebaut: parallele Laeufe teilen sich Zwischenordner im
rem Sketch-Cache und stoeren einander. S2 und S3 brauchen -mtext-section-literals (sonst Linkerfehler "l32r: literal
rem target out of range"). Der C6 braucht die Partitionstabelle uhr4_c6: die Datei boards.local.txt und uhr4_c6.csv aus
rem diesem Ordner werden dafuer bei Bedarf in den Arduino-Core kopiert. Beim S3 muss PSRAM an sein (PSRAM=enabled). Der S2 hat
rem DebugLevel=none ("Core Debug Level: None" in Visual Micro): spart 56 KB, die S2-App ist fast voll (2 MB).
rem
rem Builds the firmware with arduino-cli (from the Arduino IDE) for ESP32-S3, ESP32-S2, ESP32-C6 and the Waveshare 480x480 board one after another.
rem   buildAll.bat              all four targets (order s3, s2, c6, s3lcd4); the Qualia only when it is named
rem   buildAll.bat s3 c6        only the named chips
rem   buildAll.bat s3lcd4       only the Waveshare ESP32-S3-Touch-LCD-4 (480x480, PSRAM octal)
rem   buildAll.bat s3qualia      only the Adafruit Qualia ESP32-S3 with the 2.1" round display (480x480, PSRAM octal; never automatic, untested)
rem After every successful build the finished files are copied to build_uhr4\esp32s3, esp32s2, esp32c6, esp32s3lcd4 or esp32s3qualia.
rem The intermediate state is in %TEMP%\uhr4-build\[chip] (later runs are faster), the log in
rem %TEMP%\uhr4-build\[chip].log. The chips are built one after another: parallel runs share intermediate folders in
rem the sketch cache and disturb each other. S2 and S3 need -mtext-section-literals (otherwise linker error "l32r:
rem literal target out of range"). The C6 needs the partition table uhr4_c6: boards.local.txt and uhr4_c6.csv from this
rem folder are copied into the Arduino core if needed. On the S3 PSRAM must be on (PSRAM=enabled). The S2 has DebugLevel=none
rem ("Core Debug Level: None" in Visual Micro): saves 56 KB, the S2 app is almost full (2 MB).
setlocal EnableDelayedExpansion
cd /d "%~dp0"

set "CLI=%LOCALAPPDATA%\Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe"
set "CFG=%USERPROFILE%\.arduinoIDE\arduino-cli.yaml"
set "OUT=%TEMP%\uhr4-build"

if not exist "%CLI%" (
    echo arduino-cli.exe nicht gefunden / not found: %CLI%
    echo Arduino IDE 2 installieren oder den Pfad in dieser Datei anpassen. / Install Arduino IDE 2 or adjust the path in this file.
    exit /b 1
)
if not exist "uhr4.ino" (
    echo uhr4.ino nicht gefunden - buildAll.bat gehoert in den Projektordner. / uhr4.ino not found - buildAll.bat belongs in the project folder.
    exit /b 1
)
if not exist "%OUT%" mkdir "%OUT%"

set "WANT="
rem "copy" (frueherer Schalter) wird ignoriert / "copy" (former switch) is ignored
for %%A in (%*) do (
    if /i not "%%A"=="copy" set "WANT=!WANT! %%A"
)
if "!WANT!"=="" set "WANT=s3 s2 c6 s3lcd4"

rem ESP32-C6: Partitionstabelle uhr4_c6 in den Core kopieren, falls sie dort fehlt
rem ESP32-C6: copy the partition table uhr4_c6 into the core if it is missing there
echo !WANT! | findstr /i /c:"c6" >nul
if not errorlevel 1 (
    for /d %%D in ("%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\*") do (
        if not exist "%%D\tools\partitions\uhr4_c6.csv" (
            echo Partitionstabelle uhr4_c6 wird in den Core kopiert / copying partition table uhr4_c6 into the core: %%D
            copy /y "%~dp0uhr4_c6.csv" "%%D\tools\partitions\" >nul
        )
        if not exist "%%D\boards.local.txt" copy /y "%~dp0boards.local.txt" "%%D\" >nul
    )
)

set "RESULT="
set "FAILED=0"
for %%C in (!WANT!) do (
    call :build %%C
    if errorlevel 1 set "FAILED=1"
)

echo.
echo ===== Ergebnis / Result =====
echo !RESULT!
exit /b !FAILED!


:build
set "CHIP=%~1"
set "FQBN="
set "PROP="
set "DIR="
if /i "%CHIP%"=="s3" (
    set "DIR=esp32s3"
    set "FQBN=esp32:esp32:esp32s3:UploadSpeed=921600,USBMode=hwcdc,CDCOnBoot=default,MSCOnBoot=default,DFUOnBoot=default,UploadMode=cdc,CPUFreq=240,FlashMode=qio,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,DebugLevel=none,PSRAM=enabled,LoopCore=1,EventsCore=1,JTAGAdapter=default,ZigbeeMode=default"
    set "PROP=--build-property compiler.cpp.extra_flags=-mtext-section-literals"
)
if /i "%CHIP%"=="s2" (
    set "DIR=esp32s2"
    set "FQBN=esp32:esp32:lolin_s2_pico:CDCOnBoot=default,MSCOnBoot=default,DFUOnBoot=default,PartitionScheme=no_ota,DebugLevel=none"
    set "PROP=--build-property compiler.cpp.extra_flags=-mtext-section-literals"
)
if /i "%CHIP%"=="c6" (
    set "DIR=esp32c6"
    set "FQBN=esp32:esp32:esp32c6:UploadSpeed=921600,CDCOnBoot=cdc,CPUFreq=160,FlashFreq=80,FlashMode=qio,FlashSize=4M,PartitionScheme=uhr4_c6,DebugLevel=none,JTAGAdapter=default,ZigbeeMode=default"
)
if /i "%CHIP%"=="s3lcd4" (
    set "DIR=esp32s3lcd4"
    set "FQBN=esp32:esp32:esp32s3:UploadSpeed=921600,USBMode=hwcdc,CDCOnBoot=cdc,MSCOnBoot=default,DFUOnBoot=default,UploadMode=cdc,CPUFreq=240,FlashMode=qio,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,DebugLevel=none,PSRAM=opi,LoopCore=1,EventsCore=1,JTAGAdapter=default,ZigbeeMode=default"
    set "PROP=--build-property "compiler.cpp.extra_flags=-mtext-section-literals -DUHR4_BOARD_S3_LCD4""
)
if /i "%CHIP%"=="s3qualia" (
    set "DIR=esp32s3qualia"
    set "FQBN=esp32:esp32:esp32s3:UploadSpeed=921600,USBMode=hwcdc,CDCOnBoot=cdc,MSCOnBoot=default,DFUOnBoot=default,UploadMode=cdc,CPUFreq=240,FlashMode=qio,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,DebugLevel=none,PSRAM=opi,LoopCore=1,EventsCore=1,JTAGAdapter=default,ZigbeeMode=default"
    set "PROP=--build-property "compiler.cpp.extra_flags=-mtext-section-literals -DUHR4_BOARD_QUALIA""
)
if not defined FQBN (
    echo Unbekannter Chip / unknown chip: %CHIP% ^(s3, s2, c6, s3lcd4, s3qualia^)
    set "RESULT=!RESULT! %CHIP%=unbekannt/unknown"
    exit /b 1
)

echo.
echo [%TIME:~0,8%] Baue / building %CHIP% ... ^(Protokoll / log: %OUT%\%CHIP%.log^)
"%CLI%" compile --config-file "%CFG%" --fqbn "%FQBN%" %PROP% --build-path "%OUT%\%CHIP%" . > "%OUT%\%CHIP%.log" 2>&1
if errorlevel 1 (
    echo [%TIME:~0,8%] FEHLER / ERROR %CHIP%:
    findstr /i /c:"error" "%OUT%\%CHIP%.log"
    set "RESULT=!RESULT! %CHIP%=FEHLER/ERROR"
    exit /b 1
)
for %%F in ("%OUT%\%CHIP%\uhr4.ino.bin") do echo [%TIME:~0,8%] %CHIP% fertig / done: %%~zF Bytes, %%~tF
set "RESULT=!RESULT! %CHIP%=OK"

if not exist "build_uhr4\%DIR%" mkdir "build_uhr4\%DIR%"
for %%F in (uhr4.ino.bin uhr4.ino.bootloader.bin uhr4.ino.partitions.bin uhr4.ino.merged.bin) do copy /y "%OUT%\%CHIP%\%%F" "build_uhr4\%DIR%\" >nul
echo %CHIP%: nach build_uhr4\%DIR% kopiert / copied to build_uhr4\%DIR%
exit /b 0
