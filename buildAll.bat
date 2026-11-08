@echo off
rem Baut die Firmware mit arduino-cli (der Arduino IDE) fuer ESP32-S3, ESP32-S2, ESP32-C6 und das Waveshare-480x480-Board nacheinander.
rem   buildAll.bat              alle vier Ziele (Reihenfolge s3, s2, c6, s3lcd4)
rem   buildAll.bat s3 c6        nur die genannten Chips
rem   buildAll.bat -parallel      alle Chips gleichzeitig (eigene Prozesse, eigener Cache je Chip); gemessen 10,5 statt 59 Minuten (16 Kerne, 15 GB RAM)
rem Am Ende stehen die Bauzeit je Chip und die Gesamtzeit.
rem   buildAll.bat s3lcd4       nur das Waveshare ESP32-S3-Touch-LCD-4 (480x480, PSRAM octal)
rem Nach jedem erfolgreichen Build werden die fertigen Dateien nach build_uhr4\esp32s3, esp32s2, esp32c6 bzw. esp32s3lcd4 kopiert.
rem Die Zwischenstaende liegen in %TEMP%\uhr4-build\[chip] (spaetere Laeufe sind schneller), das Protokoll in
rem %TEMP%\uhr4-build\[chip].log. Die Chips werden nacheinander gebaut; mit -parallel laufen sie gleichzeitig,
rem jeder mit eigenem Cache (--build-cache-path), damit sie einander nicht stoeren. S2 und S3 brauchen -mtext-section-literals (sonst Linkerfehler "l32r: literal
rem target out of range"). Der C6 braucht die Partitionstabelle uhr4_c6: die Datei boards.local.txt und uhr4_c6.csv aus
rem diesem Ordner werden dafuer bei Bedarf in den Arduino-Core kopiert. Beim S3 muss PSRAM an sein (PSRAM=enabled). Der S2 hat
rem DebugLevel=none ("Core Debug Level: None" in Visual Micro): spart 56 KB, die S2-App ist fast voll (2 MB).
rem
rem Builds the firmware with arduino-cli (from the Arduino IDE) for ESP32-S3, ESP32-S2, ESP32-C6 and the Waveshare 480x480 board one after another.
rem   buildAll.bat              all four targets (order s3, s2, c6, s3lcd4)
rem   buildAll.bat s3 c6        only the named chips
rem   buildAll.bat -parallel      all chips at the same time (own processes, own cache per chip); measured 10.5 instead of 59 minutes (16 cores, 15 GB RAM)
rem At the end the build time per chip and the total time are shown.
rem   buildAll.bat s3lcd4       only the Waveshare ESP32-S3-Touch-LCD-4 (480x480, PSRAM octal)
rem After every successful build the finished files are copied to build_uhr4\esp32s3, esp32s2, esp32c6, or esp32s3lcd4.
rem The intermediate state is in %TEMP%\uhr4-build\[chip] (later runs are faster), the log in
rem %TEMP%\uhr4-build\[chip].log. The chips are built one after another; with -parallel they run at the same time,
rem each with its own cache (--build-cache-path) so they do not disturb each other. S2 and S3 need -mtext-section-literals (otherwise linker error "l32r:
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
set "PARALLEL="
rem "copy" (frueherer Schalter) wird ignoriert / "copy" (former switch) is ignored
for %%A in (%*) do (
    if /i "%%A"=="-parallel" (set "PARALLEL=1") else if /i "%%A"=="/parallel" (set "PARALLEL=1") else if /i "%%A"=="parallel" (set "PARALLEL=1") else if /i not "%%A"=="copy" set "WANT=!WANT! %%A"
)
if "!WANT!"=="" set "WANT=s3 s2 c6 s3lcd4"
call :now T0

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
set "PAR="
if defined PARALLEL if not defined UHR4_CHILD set "PAR=1"
if defined PAR (
    echo Paralleler Lauf / parallel run: !WANT!
    for %%C in (!WANT!) do (
        if exist "%OUT%\%%C.exit" del "%OUT%\%%C.exit"
        if exist "%OUT%\%%C.res" del "%OUT%\%%C.res"
    )
    for %%C in (!WANT!) do (
        start "uhr4 %%C" /b cmd /c "set NOPAUSE=1& set UHR4_CHILD=%%C& call "%~f0" %%C > "%OUT%\%%C.out" 2>&1"
    )
    call :waitchildren
    for %%C in (!WANT!) do (
        type "%OUT%\%%C.out"
        set "CODE=1"
        set "RES="
        if exist "%OUT%\%%C.exit" set /p CODE=<"%OUT%\%%C.exit"
        if exist "%OUT%\%%C.res" set /p RES=<"%OUT%\%%C.res"
        if not "!CODE!"=="0" set "FAILED=1"
        set "RESULT=!RESULT! !RES!"
    )
) else (
    for %%C in (!WANT!) do (
        call :build %%C
        if errorlevel 1 set "FAILED=1"
    )
)

call :now T1
set /a ELAPSED=T1-T0
set /a EM=ELAPSED/60, ES=ELAPSED%%60
if !ES! lss 10 set "ES=0!ES!"
echo.
if defined UHR4_CHILD (
    > "%OUT%\!UHR4_CHILD!.res" echo !RESULT!
    echo [%TIME:~0,8%] !UHR4_CHILD!: Gesamtzeit / total time !EM!:!ES! min
    > "%OUT%\!UHR4_CHILD!.exit" echo !FAILED!
    exit /b !FAILED!
)
echo ===== Ergebnis / Result =====
echo !RESULT!
echo Gesamtzeit / total time: !EM!:!ES! min
if not defined NOPAUSE (
    echo.
    set "ENTER="
    set /p "ENTER=Eingabetaste druecken zum Beenden / Press Enter to close . . . "
)
exit /b !FAILED!


:build
set "CHIP=%~1"
set "FQBN="
set "PROP="
set "DIR="
set "CACHE="
if defined UHR4_CHILD set "CACHE=--build-cache-path "%OUT%\cache-%CHIP%""
call :now B0
rem -ffile-prefix-map entfernt den Benutzerordner aus den Pfaden im Binary (__FILE__), damit die Builds keinen Benutzernamen enthalten
rem -ffile-prefix-map removes the user folder from the paths in the binary (__FILE__) so the builds contain no user name
if /i "%CHIP%"=="s3" (
    set "DIR=esp32s3"
    set "FQBN=esp32:esp32:esp32s3:UploadSpeed=921600,USBMode=hwcdc,CDCOnBoot=default,MSCOnBoot=default,DFUOnBoot=default,UploadMode=cdc,CPUFreq=240,FlashMode=qio,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,DebugLevel=none,PSRAM=enabled,LoopCore=1,EventsCore=1,JTAGAdapter=default,ZigbeeMode=default"
    set "PROP=--build-property "compiler.cpp.extra_flags=-mtext-section-literals -ffile-prefix-map=%USERPROFILE%=""
)
if /i "%CHIP%"=="s2" (
    set "DIR=esp32s2"
    set "FQBN=esp32:esp32:lolin_s2_pico:CDCOnBoot=default,MSCOnBoot=default,DFUOnBoot=default,PartitionScheme=no_ota,DebugLevel=none"
    set "PROP=--build-property "compiler.cpp.extra_flags=-mtext-section-literals -ffile-prefix-map=%USERPROFILE%=""
)
if /i "%CHIP%"=="c6" (
    set "DIR=esp32c6"
    set "FQBN=esp32:esp32:esp32c6:UploadSpeed=921600,CDCOnBoot=cdc,CPUFreq=160,FlashFreq=80,FlashMode=qio,FlashSize=4M,PartitionScheme=uhr4_c6,DebugLevel=none,JTAGAdapter=default,ZigbeeMode=default"
    set "PROP=--build-property "compiler.cpp.extra_flags=-ffile-prefix-map=%USERPROFILE%=""
)
if /i "%CHIP%"=="s3lcd4" (
    set "DIR=esp32s3lcd4"
    set "FQBN=esp32:esp32:esp32s3:UploadSpeed=921600,USBMode=hwcdc,CDCOnBoot=cdc,MSCOnBoot=default,DFUOnBoot=default,UploadMode=cdc,CPUFreq=240,FlashMode=qio,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,DebugLevel=none,PSRAM=opi,LoopCore=1,EventsCore=1,JTAGAdapter=default,ZigbeeMode=default"
    set "PROP=--build-property "compiler.cpp.extra_flags=-mtext-section-literals -DUHR4_BOARD_S3_LCD4 -ffile-prefix-map=%USERPROFILE%=""
)
if not defined FQBN (
    echo Unbekannter Chip / unknown chip: %CHIP% ^(s3, s2, c6, s3lcd4^)
    set "RESULT=!RESULT! %CHIP%=unbekannt/unknown"
    exit /b 1
)

rem Build-Zeit neu setzen: build_info.cpp (enthaelt __DATE__ und __TIME__) muss in jedem Lauf neu uebersetzt werden, auch wenn nur eine andere .cpp geaendert wurde
rem Re-stamp the build time: build_info.cpp (contains __DATE__ and __TIME__) has to be compiled in every run, even if only another .cpp changed
if exist "%OUT%\%CHIP%\sketch\build_info.cpp.o" del "%OUT%\%CHIP%\sketch\build_info.cpp.o"
echo.
echo [%TIME:~0,8%] Baue / building %CHIP% ... ^(Protokoll / log: %OUT%\%CHIP%.log^)
"%CLI%" compile --config-file "%CFG%" --fqbn "%FQBN%" %PROP% --build-property compiler.c.extra_flags=-ffile-prefix-map=%USERPROFILE%= %CACHE% --build-path "%OUT%\%CHIP%" . > "%OUT%\%CHIP%.log" 2>&1
if errorlevel 1 (
    echo [%TIME:~0,8%] FEHLER / ERROR %CHIP%:
    findstr /i /c:"error" "%OUT%\%CHIP%.log"
    set "RESULT=!RESULT! %CHIP%=FEHLER/ERROR"
    exit /b 1
)
for %%F in ("%OUT%\%CHIP%\uhr4.ino.bin") do echo [%TIME:~0,8%] %CHIP% fertig / done: %%~zF Bytes, %%~tF
call :now B1
set /a BT=B1-B0, BM=BT/60, BS=BT%%60
if !BS! lss 10 set "BS=0!BS!"
echo [%TIME:~0,8%] %CHIP% Bauzeit / build time !BM!:!BS! min
set "RESULT=!RESULT! %CHIP%=OK^(!BM!:!BS!^)"

if not exist "build_uhr4\%DIR%" mkdir "build_uhr4\%DIR%"
for %%F in (uhr4.ino.bin uhr4.ino.bootloader.bin uhr4.ino.partitions.bin uhr4.ino.merged.bin) do copy /y "%OUT%\%CHIP%\%%F" "build_uhr4\%DIR%\" >nul
echo %CHIP%: nach build_uhr4\%DIR% kopiert / copied to build_uhr4\%DIR%
exit /b 0

:now
for /f %%T in ('powershell -NoProfile -Command "[DateTimeOffset]::UtcNow.ToUnixTimeSeconds()"') do set "%~1=%%T"
exit /b 0


:waitchildren
set "PENDING="
for %%C in (!WANT!) do if not exist "%OUT%\%%C.exit" set "PENDING=1"
if defined PENDING (
    rem ping statt timeout: timeout bricht bei umgeleiteter Eingabe sofort ab / ping instead of timeout: timeout aborts at once with redirected input
    ping -n 6 127.0.0.1 >nul
    goto :waitchildren
)
exit /b 0
