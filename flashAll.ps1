# Prueft alle Uhren im Netz (checkClocks.ps1) und bringt veraltete auf den Stand der Builds in build_uhr4:
#  - ESP32-S3: per WLAN (OTA, /firmware/update), ohne USB
#  - ESP32-S2 und -C6 haben kein OTA: sie werden nur geflasht, wenn sie am USB haengen (clocksetup.ps1 -Flash -Local,
#    Einstellungen und Dateien bleiben)
# Vor jedem Flash wird gefragt (Enter = Ja). Eine Uhr am USB kann ihre Firmware-Version nicht melden, deshalb wird dort
# immer gefragt. Gebaut wird mit buildAll.bat.
#   .\flashAll.ps1               pruefen, dann fragen und flashen
#   .\flashAll.ps1 -CheckOnly    nur pruefen und anzeigen
#   .\flashAll.ps1 -Yes          ohne Rueckfragen (alle veralteten S3 und alle Uhren am USB)
#   .\flashAll.ps1 -Force        auch aktuelle S3 neu einspielen
#
# Checks all clocks on the network (checkClocks.ps1) and brings outdated ones to the state of the builds in build_uhr4:
#  - ESP32-S3: over WiFi (OTA, /firmware/update), no USB needed
#  - ESP32-S2 and -C6 have no OTA: they are only flashed if connected via USB (clocksetup.ps1 -Flash -Local, settings
#    and files stay)
# Before every flash it asks (Enter = yes). A clock on USB cannot report its firmware version, so it is always asked
# for. Build with buildAll.bat.
#   .\flashAll.ps1               check, then ask and flash
#   .\flashAll.ps1 -CheckOnly    only check and show
#   .\flashAll.ps1 -Yes          without questions (all outdated S3 and all clocks on USB)
#   .\flashAll.ps1 -Force        also re-flash up-to-date S3

param([switch]$CheckOnly, [switch]$Yes, [switch]$Force)

$root = $PSScriptRoot
function Ask([string]$question) {
    if ($Yes) { return $true }
    $a = Read-Host "$question [J/n, Y/n]"
    return ($a -eq '' -or $a -match '^[JjYy]')
}

# ---- 1. Uhren im Netz pruefen / check the clocks on the network
Write-Host '=== Uhren im Netz / clocks on the network ==='
$clocks = @(& "$root\checkClocks.ps1" -PassThru)
if ($clocks.Count -eq 0) {
    Write-Host 'Keine uhr4 im Netz gefunden. / No uhr4 found on the network.'
}
else {
    $clocks | Sort-Object IP | Format-Table IP, Name, Chip, Display, Firmware, Soll, Stand, Zeit -AutoSize | Out-String -Width 200 | Write-Host
}

# ---- 2. ESP32-S3 per OTA / ESP32-S3 via OTA
$bin = Join-Path $root 'build_uhr4\esp32s3\uhr4.ino.bin'
$failed = 0
foreach ($c in ($clocks | Where-Object { $_.Chip -eq 'S3' -and ($_.Stand -eq 'aelter' -or $Force) })) {
    if ($CheckOnly) { Write-Host "S3 $($c.Name) ($($c.IP)): veraltet / outdated"; continue }
    if (-not (Test-Path $bin)) { Write-Host "Build fehlt / missing: $bin"; $failed++; continue }
    if (-not (Ask "S3 $($c.Name) ($($c.IP)) per WLAN aktualisieren / update via WiFi?")) { continue }
    Write-Host "Spiele ein / uploading: $($c.IP) ..."
    $answer = & curl.exe -s -m 240 -F "firmware=@$bin" "http://$($c.IP)/firmware/update"
    if ($answer -ne 'OK') { Write-Host "  Fehler / error: $answer"; $failed++; continue }
    $ok = $false
    for ($i = 0; $i -lt 40 -and -not $ok; $i++) {
        Start-Sleep -Seconds 3
        try {
            $top = (New-Object System.Net.WebClient).DownloadString("http://$($c.IP)/api/topbarStatus")
            if ($top -match [regex]::Escape('"version":"' + $c.Soll + '"')) { $ok = $true }
        } catch {}
    }
    if ($ok) { Write-Host "  $($c.Name) laeuft mit / runs with $($c.Soll)" } else { Write-Host "  $($c.Name) meldet die neue Version nicht / does not report the new version"; $failed++ }
}

# ---- 3. Uhren am USB / clocks on USB
Write-Host ''
Write-Host '=== Uhren am USB / clocks on USB ==='
$boards = @(Get-CimInstance Win32_PnPEntity | Where-Object { $_.Name -match 'COM(\d+)\)' -and $_.DeviceID -match 'VID_303A|VID_1A86&PID_55D3' } | ForEach-Object {
    $port = [int]([regex]::Match($_.Name, 'COM(\d+)\)').Groups[1].Value)
    $chip = if ($_.DeviceID -match 'VID_1A86') { 'S3' } elseif ($_.DeviceID -match 'PID_1001') { 'C6' } else { 'S2' }
    [pscustomobject]@{ Port = $port; Chip = $chip }
})
if ($boards.Count -eq 0) { Write-Host 'Keine Uhr am USB. / No clock on USB.' }
foreach ($b in $boards) {
    if ($CheckOnly) { Write-Host "COM$($b.Port): ESP32-$($b.Chip)"; continue }
    if (-not (Ask "COM$($b.Port) (ESP32-$($b.Chip)) mit dem Build aus build_uhr4 flashen / flash with the build from build_uhr4?")) { continue }
    & powershell -NoProfile -ExecutionPolicy Bypass -File "$root\build_uhr4\clocksetup.ps1" -Flash -Port $b.Port -NoMonitor -Local
    if ($LASTEXITCODE -ne 0) { Write-Host "  Flashen fehlgeschlagen / flashing failed (COM$($b.Port))"; $failed++ }
}

Write-Host ''
if ($failed) { Write-Host "$failed Fehler / error(s)."; exit 1 }
Write-Host 'Fertig. / Done.'
