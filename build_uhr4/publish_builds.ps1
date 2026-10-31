# Erzeugt die Pakete, die flashESP.bat / flashESP.sh beim Flashen vom GitHub-Release laden:
# je Chip uhr4-esp32s2.zip, uhr4-esp32c6.zip, uhr4-esp32s3.zip, uhr4-esp32s3lcd4.zip, uhr4-esp32s3qualia.zip (Bootloader, Partitionstabelle,
# App, bei C6 und S3 boot_app0.bin - flach im Zip) und dazu uhr4-<chip>.sha256 ("<SHA-256>  <Zip>").
# Quelle sind die Ordner esp32s2, esp32c6, esp32s3, esp32s3lcd4 und esp32s3qualia neben diesem Skript.
#   .\publish_builds.ps1                      Pakete in einen Ordner schreiben (Standard: Temp), nichts hochladen
#   .\publish_builds.ps1 -OutDir C:\test      Zielordner angeben
#   .\publish_builds.ps1 -Upload              zusaetzlich in das Release hochladen (gh, ersetzt vorhandene Dateien)
# Zusaetzlich entsteht uhr4_flash.zip mit den Werkzeugen und den Builds aller Chips (Ordner esp32*, Marke bundled.txt:
# Flashen ohne Internet; ohne publish_*.ps1) aus dem letzten Commit, dazu uhr4_flash.sha256 und je Chip uhr4-<Tag>-esp32<chip>-merged.bin (ein Abbild fuer Adresse 0x0, zum Flashen von Hand).
# Zum Testen ohne Release: $env:UHR4_BUILD_URL = 'file:///C:/test' vor flashESP.bat bzw. clocksetup.ps1.
#
# Creates the packages that flashESP.bat / flashESP.sh download from the GitHub release when flashing:
# per chip uhr4-esp32s2.zip, uhr4-esp32c6.zip, uhr4-esp32s3.zip, uhr4-esp32s3lcd4.zip, uhr4-esp32s3qualia.zip (bootloader, partition table, app,
# boot_app0.bin for C6 and S3 - flat in the zip) and uhr4-<chip>.sha256 ("<SHA-256>  <zip>").
# The source is the folders esp32s2, esp32c6, esp32s3, esp32s3lcd4 and esp32s3qualia next to this script.
#   .\publish_builds.ps1                      write the packages to a folder (default: temp), upload nothing
#   .\publish_builds.ps1 -OutDir C:\test      set the target folder
#   .\publish_builds.ps1 -Upload              also upload to the release (gh, replaces existing files)
# uhr4_flash.zip is created as well, with the tools and the builds of all chips (folders esp32*, marker bundled.txt:
# flashing without internet; no publish_*.ps1) from the last commit, plus uhr4_flash.sha256 and per chip uhr4-<tag>-esp32<chip>-merged.bin (one image for address 0x0, for flashing by hand).
# To test without a release: $env:UHR4_BUILD_URL = 'file:///C:/test' before flashESP.bat or clocksetup.ps1.

param([string]$OutDir, [switch]$Upload, [string]$Tag = 'v4')

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
if (-not $OutDir) { $OutDir = Join-Path ([System.IO.Path]::GetTempPath()) 'uhr4-packages' }
New-Item -ItemType Directory -Path $OutDir -Force | Out-Null

$files = @()

# esptool.exe: build_uhr4 oder Arduino-Core. Es kommt ins Werkzeug-Zip und erzeugt die merged.bin.
# esptool.exe: build_uhr4 or the Arduino core. It goes into the tools zip and creates the merged.bin.
$esp = Join-Path $PSScriptRoot 'esptool.exe'
if (-not (Test-Path $esp)) {
    $core = Get-ChildItem "$env:LOCALAPPDATA\Arduino15\packages\esp32\tools\esptool_py\*\esptool.exe" -ErrorAction SilentlyContinue | Sort-Object FullName | Select-Object -Last 1
    if ($core) { $esp = $core.FullName }
}
if (-not (Test-Path $esp)) { throw 'esptool.exe not found (build_uhr4 or Arduino ESP32 core)' }
# Das Qualia-Paket ist dabei, aber NICHT AM GERAET GETESTET (Release-Notizen und flashESP weisen darauf hin)
# The Qualia package is included, but NOT TESTED ON THE DEVICE (release notes and flashESP point this out)
$chips = @('s2', 'c6', 's3', 's3lcd4', 's3qualia')

# Nichts veroeffentlichen, was den Windows-Benutzernamen oder Projektpfade des Build-PCs enthaelt (buildAll.bat setzt dafuer
# -ffile-prefix-map). Pruefung der App-Images aller Chips.
# Publish nothing that contains the Windows user name or project paths of the build PC (buildAll.bat sets -ffile-prefix-map
# for that). Checks the app images of all chips.
$projectRoot = Split-Path $PSScriptRoot -Parent
$private = @($env:USERNAME, $projectRoot, ($projectRoot -replace '\\', '/'), (Split-Path (Split-Path $projectRoot -Parent) -Parent)) | Where-Object { $_ -and $_.Length -gt 4 }
foreach ($chip in $chips) {
    $text = [System.Text.Encoding]::GetEncoding(28591).GetString([System.IO.File]::ReadAllBytes((Join-Path $PSScriptRoot "esp32$chip\uhr4.ino.bin")))
    foreach ($p in $private) {
        if ($text.IndexOf($p, [StringComparison]::OrdinalIgnoreCase) -ge 0) { throw "esp32$chip\uhr4.ino.bin contains '$p' - rebuild with buildAll.bat (-ffile-prefix-map)" }
    }
}
foreach ($chip in $chips) {
    $src = Join-Path $PSScriptRoot "esp32$chip"
    $parts = @('uhr4.ino.bootloader.bin', 'uhr4.ino.partitions.bin', 'uhr4.ino.bin')
    if ($chip -ne 's2') { $parts += 'boot_app0.bin' }
    $stage = Join-Path ([System.IO.Path]::GetTempPath()) ('uhr4-stage-' + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $stage | Out-Null
    try {
        foreach ($p in $parts) {
            $f = Join-Path $src $p
            if (-not (Test-Path $f)) { throw "Missing: $f" }
            Copy-Item $f $stage
        }
        $name = "uhr4-esp32$chip"
        $zip = Join-Path $OutDir "$name.zip"
        if (Test-Path $zip) { Remove-Item $zip -Force }
        [System.IO.Compression.ZipFile]::CreateFromDirectory($stage, $zip)
        $hash = (Get-FileHash -Algorithm SHA256 $zip).Hash.ToLower()
        $shaFile = Join-Path $OutDir "$name.sha256"
        [System.IO.File]::WriteAllText($shaFile, "$hash  $name.zip`n")
        Write-Host ("{0}  {1}  {2:N0} KB" -f $name, $hash, ((Get-Item $zip).Length / 1KB))
        $files += $zip, $shaFile

        # Ein Abbild fuer Adresse 0x0 (ESP32-S2: Bootloader bei 0x1000, davor 0xFF) - loescht beim Flashen das Dateisystem
        # One image for address 0x0 (ESP32-S2: bootloader at 0x1000, 0xFF before it) - erases the file system when flashed
        $merged = Join-Path $OutDir "uhr4-$Tag-esp32$chip-merged.bin"
        if (Test-Path $merged) { Remove-Item $merged -Force }
        $chipName = if ($chip -eq 's2') { 'esp32s2' } elseif ($chip -eq 'c6') { 'esp32c6' } else { 'esp32s3' }
        $bootAddr = if ($chip -eq 's2') { '0x1000' } else { '0x0' }
        $mergeArgs = @('--chip', $chipName, 'merge_bin', '-o', $merged, '--flash_mode', 'keep', '--flash_freq', 'keep', '--flash_size', 'keep', $bootAddr, (Join-Path $stage 'uhr4.ino.bootloader.bin'), '0x8000', (Join-Path $stage 'uhr4.ino.partitions.bin'))
        if ($chip -ne 's2') { $mergeArgs += '0xe000', (Join-Path $stage 'boot_app0.bin') }
        $mergeArgs += '0x10000', (Join-Path $stage 'uhr4.ino.bin')
        & $esp @mergeArgs | Out-Null
        if ($LASTEXITCODE -ne 0 -or -not (Test-Path $merged)) { throw "merge_bin failed for $chip" }
        Write-Host ("{0}  {1:N0} KB" -f (Split-Path $merged -Leaf), ((Get-Item $merged).Length / 1KB))
        $files += $merged
    }
    finally {
        Remove-Item -Recurse -Force $stage
    }
}

# Werkzeug-Zip aus dem letzten Commit (Skripte, Builds aller Chips, readme.txt; esptool.exe kommt unten dazu)
# Tools zip from the last commit (scripts, builds of all chips, readme.txt; esptool.exe is added below)
$toolsZip = Join-Path $OutDir 'uhr4_flash.zip'
if (Test-Path $toolsZip) { Remove-Item $toolsZip -Force }
Push-Location (Split-Path $PSScriptRoot -Parent)
git archive --format=zip -o $toolsZip HEAD -- build_uhr4 LICENSE THIRD_PARTY_LICENSES.md ':(exclude)build_uhr4/publish_*.ps1' ':(exclude)build_uhr4/cleanup_history.ps1'
$gitOk = ($LASTEXITCODE -eq 0)
Pop-Location
if (-not $gitOk) { throw 'git archive failed' }

# esptool.exe liegt nicht im Repository (Fremdsoftware, GPL-2.0-or-later): ins Werkzeug-Zip legen, mit Hinweis auf
# Lizenz und Quelle.
# esptool.exe is not in the repository (third-party software, GPL-2.0-or-later): put it into the tools zip, with a
# note on license and source.
# Version der mitgelieferten esptool.exe fuer Quelltext-Link und Hinweis / version of the shipped esptool.exe for the source link and the note
$espVersion = ((& $esp version 2>&1 | Out-String).Trim() -split '\s+' | Select-Object -Last 1)
if ($espVersion -notmatch '^\d+(\.\d+)+$') { throw "esptool version not recognized: $espVersion" }
$notice = @"
esptool.exe gehoert nicht zu uhr4. Es ist esptool $espVersion von Espressif (https://github.com/espressif/esptool), lizenziert unter
GPL-2.0-or-later; der Lizenztext liegt als esptool-LICENSE.txt bei. Es wird hier unveraendert nur zum Flashen mitgeliefert.
Quelltext dieser Version: https://github.com/espressif/esptool/tree/v$espVersion . Auf Anfrage (siehe Kontakt in README.md) wird der
Quelltext auch zugesandt (drei Jahre ab Weitergabe). Die Datei enthaelt ausserdem Python und weitere Bibliotheken (zum Beispiel
pyserial, cryptography) unter ihren eigenen Lizenzen. Die Lizenz von uhr4 (GPL-3.0) liegt als LICENSE bei, weitere Lizenzen
stehen in THIRD_PARTY_LICENSES.md.

esptool.exe is not part of uhr4. It is esptool $espVersion by Espressif (https://github.com/espressif/esptool), licensed under
GPL-2.0-or-later; the license text is included as esptool-LICENSE.txt. It is shipped here unchanged, only for flashing.
Source code of this version: https://github.com/espressif/esptool/tree/v$espVersion . On request (see contact in README.md) the
source code is also sent (three years from distribution). The file also contains Python and further libraries (for example
pyserial, cryptography) under their own licenses. The license of uhr4 (GPL-3.0) is included as LICENSE, further licenses are in
THIRD_PARTY_LICENSES.md.
"@
$zipArchive = [System.IO.Compression.ZipFile]::Open($toolsZip, 'Update')
try {
    foreach ($n in 'build_uhr4/esptool.exe', 'build_uhr4/esptool-NOTICE.txt') { $old = $zipArchive.GetEntry($n); if ($old) { $old.Delete() } }
    [void][System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zipArchive, $esp, 'build_uhr4/esptool.exe')
    $entry = $zipArchive.CreateEntry('build_uhr4/esptool-NOTICE.txt')
    $w = New-Object System.IO.StreamWriter($entry.Open(), (New-Object System.Text.UTF8Encoding($false)))
    $w.Write($notice); $w.Dispose()
}
finally { $zipArchive.Dispose() }
Write-Host ("uhr4_flash.zip  {0:N0} KB" -f ((Get-Item $toolsZip).Length / 1KB))
$files += $toolsZip
$toolsSha = Join-Path $OutDir 'uhr4_flash.sha256'
[System.IO.File]::WriteAllText($toolsSha, "$((Get-FileHash -Algorithm SHA256 $toolsZip).Hash.ToLower())  uhr4_flash.zip`n")
$files += $toolsSha
Write-Host "Packages in $OutDir"

if ($Upload) {
    Write-Host "Uploading to release $Tag ..."
    gh release upload $Tag @files --clobber
    if ($LASTEXITCODE -ne 0) { throw 'gh release upload failed' }
    Write-Host 'Done'
}
