@echo off
rem Sucht alle uhr4-Uhren im Netz und zeigt eine Tabelle (Adresse, Name, Chip, Display, Firmware, Stand, Zeit, Speicher).
rem Doppelklick: sucht im Netz des PCs. Parameter werden an checkClocks.ps1 weitergegeben, zum Beispiel
rem   checkClocks.bat -Subnet 192.168.1        anderes Netz
rem   checkClocks.bat -Ip 192.168.0.169        nur diese Adresse
rem Finds all uhr4 clocks on the network and shows a table (address, name, chip, display, firmware, state, time, memory).
rem Double-click: searches the PC's network. Parameters are passed on to checkClocks.ps1, for example
rem   checkClocks.bat -Subnet 192.168.1        another network
rem   checkClocks.bat -Ip 192.168.0.169        only this address
setlocal
cd /d "%~dp0"
if not exist "%~dp0checkClocks.ps1" (
    echo checkClocks.ps1 fehlt / is missing.
    pause
    exit /b 1
)
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0checkClocks.ps1" %*
if "%~1"=="" pause
