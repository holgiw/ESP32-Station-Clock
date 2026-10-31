# Sucht alle uhr4-Uhren im Netz und zeigt eine Tabelle: Adresse, Name, Chip und Display, laufende Firmware (Build-Zeit),
# ob sie dem neuesten Build in build_uhr4 entspricht, ob die Uhrzeit stimmt, freier Speicher.
#   .\checkClocks.ps1                       sucht im Netz des PCs (die ersten drei Zahlen seiner Adresse, z. B. 192.168.0.x)
#   .\checkClocks.ps1 -Subnet 192.168.1     anderes Netz
#   .\checkClocks.ps1 -Ip 192.168.0.169     nur diese Adressen (mehrere mit Komma)
#   .\checkClocks.ps1 -All                  listet zuerst ALLE Geraete im Netz auf (auch keine Uhren): Adresse, MAC, Name,
#                                           Hinweis (Seitentitel, uhr4, ESP-Hersteller), danach die Uhren-Tabelle
#   .\checkClocks.ps1 -PassThru             gibt die Uhren als Objekte zurueck statt als Tabelle (fuer flashAll.ps1)
# Der Soll-Stand ist die Build-Zeit, die in den Dateien build_uhr4\esp32s2|esp32c6|esp32s3|esp32s3lcd4\uhr4.ino.bin steht
# (Kennung UHR4_FW_BUILD=). Aeltere Firmware ohne Kennung zeigt "?". Geraete werden per Ping und ARP gefunden; was
# weder auf Ping antwortet noch im ARP-Cache des PCs steht, fehlt in der Liste.
#
# Finds all uhr4 clocks on the network and shows a table: address, name, chip and display, running firmware (build
# time), whether it matches the newest build in build_uhr4, whether the time is right, free memory.
#   .\checkClocks.ps1                       searches the PC's network (the first three numbers of its address, e.g. 192.168.0.x)
#   .\checkClocks.ps1 -Subnet 192.168.1     another network
#   .\checkClocks.ps1 -Ip 192.168.0.169     only these addresses (several with comma)
#   .\checkClocks.ps1 -All                  first lists ALL devices on the network (also non-clocks): address, MAC, name,
#                                           note (page title, uhr4, ESP vendor), then the clock table
#   .\checkClocks.ps1 -PassThru             returns the clocks as objects instead of a table (for flashAll.ps1)
# The target is the build time stored in the files build_uhr4\esp32s2|esp32c6|esp32s3|esp32s3lcd4\uhr4.ino.bin (marker
# UHR4_FW_BUILD=). Older firmware without the marker shows "?". Devices are found by ping and ARP; anything that
# neither answers a ping nor is in the PC's ARP cache is missing from the list.

param([string]$Subnet, [string[]]$Ip, [switch]$All, [switch]$PassThru)

$ErrorActionPreference = 'SilentlyContinue'

# Mit -File kommt "a,b" als ein Text an - trennen / with -File "a,b" arrives as one string - split it
if ($Ip) { $Ip = @($Ip | ForEach-Object { $_ -split '[,; ]+' } | Where-Object { $_ }) }

# Soll-Build je Chip aus den Binaerdateien lesen, "2026-10-11 10:55:48"
# Read the target build per chip from the binaries, "2026-10-11 10:55:48"
function Get-TargetBuild([string]$chip) {
    $file = Join-Path $PSScriptRoot "build_uhr4\esp32$chip\uhr4.ino.bin"
    if (-not (Test-Path $file)) { return $null }
    $text = [System.Text.Encoding]::GetEncoding(28591).GetString([System.IO.File]::ReadAllBytes($file))
    $at = $text.IndexOf('UHR4_FW_BUILD=')
    if ($at -lt 0) { return $null }
    $end = $text.IndexOf([char]0, $at)
    $stamp = ($text.Substring($at + 14, $end - $at - 14) -replace '\s+', ' ').Trim()
    try { return [datetime]::ParseExact($stamp, 'MMM d yyyy HH:mm:ss', [System.Globalization.CultureInfo]::InvariantCulture).ToString('yyyy-MM-dd HH:mm:ss') }
    catch { return $null }
}
$target = @{ S2 = Get-TargetBuild 's2'; C6 = Get-TargetBuild 'c6'; S3 = Get-TargetBuild 's3'; 'S3-480' = Get-TargetBuild 's3lcd4'; 'S3-QUALIA' = Get-TargetBuild 's3qualia' }

# Netz des PCs bestimmen (erste drei Zahlen der Adresse), falls kein -Subnet angegeben ist
# Determine the PC's network (first three numbers of its address) if no -Subnet is given
$selfIp = $null
if (-not $Subnet -and (-not $Ip -or $All)) {
    $route = Get-NetRoute -DestinationPrefix '0.0.0.0/0' | Sort-Object RouteMetric | Select-Object -First 1
    $addr = Get-NetIPAddress -InterfaceIndex $route.InterfaceIndex -AddressFamily IPv4 | Select-Object -First 1
    if (-not $addr) { Write-Host 'Kein Netz gefunden - -Subnet 192.168.0 angeben. / No network found - give -Subnet 192.168.0.'; exit 1 }
    $selfIp = $addr.IPAddress
    $Subnet = ($addr.IPAddress -split '\.')[0..2] -join '.'
}

# Kandidaten: offene Port-80-Adressen im Netz
# Candidates: addresses with an open port 80 on the network
if (-not $Ip) {
    Write-Host "Suche in / searching $Subnet.1 - $Subnet.254 ..."
    $attempts = 1..254 | ForEach-Object {
        $c = New-Object System.Net.Sockets.TcpClient
        [pscustomobject]@{ Ip = "$Subnet.$_"; Client = $c; Task = $c.ConnectAsync("$Subnet.$_", 80) }
    }
    Start-Sleep -Milliseconds 1500
    $Ip = @($attempts | Where-Object { $_.Task.IsCompleted -and -not $_.Task.IsFaulted -and $_.Client.Connected } | ForEach-Object { $_.Ip })
    $attempts | ForEach-Object { $_.Client.Close() }
}

function Get-Page([string]$url) {
    try {
        $wc = New-Object System.Net.WebClient
        $wc.Encoding = [System.Text.Encoding]::UTF8
        return $wc.DownloadString($url)
    } catch { return $null }
}

$webTitle = @{}
$clockInfo = @{}
$rows = foreach ($ipAddr in $Ip) {
    $page = Get-Page "http://$ipAddr/"
    if (-not $page) { continue }
    if ($page -notmatch 'UHR&middot;4') {
        $t = [regex]::Match($page, '<title>([^<]*)</title>', 'IgnoreCase').Groups[1].Value.Trim()
        $webTitle[$ipAddr] = $(if ($t) { $t } else { 'Webseite' })
        continue
    }
    $name = [regex]::Match($page, "class='host-link' href='[^']*'>([^<]*)").Groups[1].Value
    $chipDisp = [regex]::Match($page, "title='Chip / [^']*'>([^<]*)").Groups[1].Value
    if (-not $chipDisp) {

        # aeltere Firmware: nur der Displaytyp steht in der Kopfzeile, der Chip fehlt
        # older firmware: only the display type is in the header, the chip is missing

        $oldDisp = [regex]::Match($page, "class='ip-hint' title='[^']*'>([A-Za-z0-9_]+)</span>").Groups[1].Value
        if ($oldDisp) { $chipDisp = '? ' + $oldDisp }
    }
    $parts = $chipDisp -split ' ', 2
    $chip = $parts[0]; $display = if ($parts.Count -gt 1) { $parts[1] } else { '' }

    # ESP32-S3 mit ST7701_480 = Waveshare ESP32-S3-Touch-LCD-4 (eigener Build, esp32s3lcd4)
    # ESP32-S3 with ST7701_480 = Waveshare ESP32-S3-Touch-LCD-4 (own build, esp32s3lcd4)

    if ($chip -eq 'S3' -and $display -eq 'ST7701_480') { $chip = 'S3-480' }
    $top = Get-Page "http://$ipAddr/api/topbarStatus"
    $version = [regex]::Match([string]$top, '"version":"([^"]*)"').Groups[1].Value
    $timeOk = [regex]::Match([string]$top, '"time":"([^"]*)"').Groups[1].Value
    $dt = [regex]::Match([string]$top, '"datetime":"([^"]*)"').Groups[1].Value
    $status = Get-Page "http://$ipAddr/status"
    $statusText = [regex]::Replace([string]$status, '<[^>]*>', "`n")
    $heap = [regex]::Match($statusText, 'Free Heap: (\d+ KB)').Groups[1].Value
    $psram = [regex]::Match($statusText, 'PSRAM Free: (\d+ kB)').Groups[1].Value
    $soll = $target[$chip]
    $state = if (-not $soll -or -not $version) { '?' } elseif ($version -eq $soll) { 'aktuell' } elseif ($version -lt $soll) { 'aelter' } else { 'neuer' }
    $clockInfo[$ipAddr] = "uhr4 $chip $display ($state)"
    [pscustomobject]@{
        IP = $ipAddr; Name = $name; Chip = $chip; Display = $display; Firmware = $version; Soll = $soll
        Stand = $state; Zeit = $(if ($timeOk -eq 'ok') { 'OK' } else { 'FEHLT' }); Uhrzeit = $dt
        Heap = $heap; PSRAM = $(if ($psram) { $psram } else { '-' })
    }
}

# ---- -All: alle Geraete im Netz / all devices on the network
if ($All) {
    Write-Host "Suche alle Geraete in / searching all devices in $Subnet.0/24 ..."

    # Ping an alle Adressen, dann der ARP-Cache: er enthaelt auch Geraete, die nicht auf Ping antworten
    # Ping every address, then the ARP cache: it also holds devices that do not answer a ping
    $pings = 1..254 | ForEach-Object {
        $p = New-Object System.Net.NetworkInformation.Ping
        [pscustomobject]@{ Ip = "$Subnet.$_"; Task = $p.SendPingAsync("$Subnet.$_", 1500) }
    }
    Start-Sleep -Milliseconds 2200
    $found = @{}
    $pings | Where-Object { $_.Task.IsCompleted -and -not $_.Task.IsFaulted -and $_.Task.Result.Status -eq 'Success' } | ForEach-Object { $found[$_.Ip] = '' }
    foreach ($ipAddr in $Ip) { $found[$ipAddr] = '' }
    foreach ($line in (arp -a)) {
        $m = [regex]::Match($line, '^\s*(\d+\.\d+\.\d+\.\d+)\s+([0-9a-fA-F]{2}(?:-[0-9a-fA-F]{2}){5})\s+(dynamisch|dynamic)', 'IgnoreCase')
        if ($m.Success -and $m.Groups[1].Value.StartsWith("$Subnet.") -and -not $m.Groups[1].Value.EndsWith('.255')) {
            $found[$m.Groups[1].Value] = $m.Groups[2].Value.ToUpper().Replace('-', ':')
        }
    }
    if ($selfIp) { $found[$selfIp] = '' }

    # Namen per DNS-Rueckfrage (kurz warten)
    # Names by reverse DNS lookup (wait briefly)
    $dns = @{}
    foreach ($ipAddr in $found.Keys) { $dns[$ipAddr] = [System.Net.Dns]::GetHostEntryAsync($ipAddr) }
    Start-Sleep -Milliseconds 1800

    # Espressif-Herstellerkennungen (Auswahl, nicht vollstaendig)
    # Espressif vendor prefixes (a selection, not complete)
    $espOui = @('24:0A:C4','24:6F:28','30:AE:A4','3C:71:BF','3C:0F:02','40:F5:20','4C:11:AE','54:5A:A6','60:01:94','7C:9E:BD',
        '80:65:99','84:CC:A8','84:F3:EB','8C:AA:B5','94:B9:7E','A4:CF:12','AC:67:B2','B4:E6:2D','BC:DD:C2','C4:4F:33','CC:50:E3',
        'D8:A0:1D','DC:4F:22','E8:68:E7','EC:FA:BC','34:85:18','48:27:E2','F0:F5:BD','7C:DF:A1','34:B7:DA','68:67:25','70:04:1D',
        '08:3A:F2','0C:B8:15','10:52:1C','18:8B:0E','1C:9D:C2','2C:3A:E8','2C:F4:32','30:C6:F7','38:18:2B','44:17:93','48:3F:DA',
        '50:02:91','58:BF:25','5C:CF:7F','64:E8:33','68:B6:B3','78:21:84','78:E3:6D','90:38:0C','98:CD:AC','A0:20:A6','A8:03:2A',
        'B8:D6:1A','BC:FF:4D','C8:2B:96','D4:D4:DA','E0:E2:E6','E8:9F:6D','F4:CF:A2','FC:F5:C4','D0:EF:76','EC:62:60','F0:08:D1')

    $devices = foreach ($ipAddr in ($found.Keys | Sort-Object { [version]$_ })) {
        $mac = $found[$ipAddr]
        $name = ''
        if ($dns[$ipAddr] -and $dns[$ipAddr].IsCompleted -and -not $dns[$ipAddr].IsFaulted) { $name = [string]$dns[$ipAddr].Result.HostName }
        $note = ''
        if ($clockInfo[$ipAddr]) { $note = $clockInfo[$ipAddr] }
        elseif ($webTitle[$ipAddr]) { $note = 'Web: ' + $webTitle[$ipAddr] }
        if (-not $note -and $mac -and $espOui -contains $mac.Substring(0, 8)) { $note = 'Espressif (ESP)' }
        if ($ipAddr -eq $selfIp) { $note = 'dieser PC / this PC' }
        [pscustomobject]@{ IP = $ipAddr; MAC = $mac; Name = $name; Hinweis = $note }
    }
    Write-Host ''
    Write-Host ('Alle Geraete / all devices: {0}' -f @($devices).Count)
    $devices | Format-Table -AutoSize | Out-String -Width 220 | Write-Host
}

if ($PassThru) { return @($rows) }
if (-not $rows) { Write-Host 'Keine uhr4 gefunden. / No uhr4 found.'; exit 1 }
$rows | Sort-Object IP | Format-Table -AutoSize
Write-Host ('Soll / target:  S2 {0}   C6 {1}   S3 {2}   S3-480 {3}   S3-QUALIA {4}' -f $target.S2, $target.C6, $target.S3, $target['S3-480'], $target['S3-QUALIA'])
