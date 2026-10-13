# Veroeffentlicht die ESP32-S3-Firmware fuer die Update-Funktion der Uhr ("Firmware von GitHub", ota_update.h):
# laedt je Board uhr4-s3.bin / uhr4-s3.txt (ESP32-S3-LCD-1.28, Ordner esp32s3) und uhr4-s3lcd4.bin / uhr4-s3lcd4.txt
# (ESP32-S3-Touch-LCD-4, Ordner esp32s3lcd4) - Zeile 1 der .txt: Build-Zeit, Zeile 2: SHA-256 - in das neueste
# GitHub-Release. Die Build-Zeit steht als Kennung "UHR4_FW_BUILD=..." in der .bin.
# Aufruf: .\publish_s3_update.ps1 [-DryRun]
#
# Publishes the ESP32-S3 firmware for the clock's update function ("Firmware from GitHub", ota_update.h):
# per board uploads uhr4-s3.bin / uhr4-s3.txt (ESP32-S3-LCD-1.28, folder esp32s3) and uhr4-s3lcd4.bin /
# uhr4-s3lcd4.txt (ESP32-S3-Touch-LCD-4, folder esp32s3lcd4) - line 1 of the .txt: build time, line 2: SHA-256 - to
# the latest GitHub release. The build time is in the .bin as marker "UHR4_FW_BUILD=...".
# Usage: .\publish_s3_update.ps1 [-DryRun]

param([switch]$DryRun)

$ErrorActionPreference = 'Stop'
$boards = @(
    @{ Dir = 'esp32s3';     Asset = 'uhr4-s3' },
    @{ Dir = 'esp32s3lcd4'; Asset = 'uhr4-s3lcd4' },
    @{ Dir = 'esp32s3qualia'; Asset = 'uhr4-s3qualia' } # NICHT AM GERAET GETESTET / NOT TESTED ON THE DEVICE
)

$work = Join-Path ([System.IO.Path]::GetTempPath()) ('uhr4-s3-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $work | Out-Null
try {
    $uploads = @()
    foreach ($b in $boards) {
        $bin = Join-Path $PSScriptRoot "$($b.Dir)\uhr4.ino.bin"
        if (-not (Test-Path $bin)) { throw "Not found: $bin" }

        $bytes = [System.IO.File]::ReadAllBytes($bin)
        $text = [System.Text.Encoding]::GetEncoding(28591).GetString($bytes)
        $at = $text.IndexOf('UHR4_FW_BUILD=')
        if ($at -lt 0) { throw "Marker UHR4_FW_BUILD= not found in $bin - the .bin was built without the update function" }
        $end = $text.IndexOf([char]0, $at)
        $stamp = ($text.Substring($at + 14, $end - $at - 14) -replace '\s+', ' ').Trim()
        $built = [datetime]::ParseExact($stamp, 'MMM d yyyy HH:mm:ss', [System.Globalization.CultureInfo]::InvariantCulture)
        $iso = $built.ToString('yyyy-MM-dd HH:mm:ss')
        $sha = (Get-FileHash -Algorithm SHA256 $bin).Hash.ToLower()

        Copy-Item $bin (Join-Path $work "$($b.Asset).bin")
        [System.IO.File]::WriteAllText((Join-Path $work "$($b.Asset).txt"), "$iso`n$sha`n")
        Write-Host "$($b.Asset): build $iso  SHA-256 $sha  ($([math]::Round($bytes.Length / 1KB)) KB)"
        $uploads += (Join-Path $work "$($b.Asset).bin"), (Join-Path $work "$($b.Asset).txt")
    }
    if ($DryRun) { Write-Host 'Dry run - nothing uploaded'; return }
    $tag = (gh release view --json tagName -q .tagName).Trim()
    Write-Host "Uploading to release $tag ..."
    gh release upload $tag @uploads --clobber
    if ($LASTEXITCODE -ne 0) { throw 'gh release upload failed' }
    Write-Host 'Done'
}
finally {
    Remove-Item -Recurse -Force $work
}
