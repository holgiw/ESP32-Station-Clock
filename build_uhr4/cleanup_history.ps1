# SPDX-License-Identifier: GPL-3.0-only (siehe / see LICENSE)
# Entfernt die Firmware (build_uhr4\esp32*) aus allen aelteren Commits, damit das Repository mit jedem neuen Build nicht um ~11 MB waechst.
# Nur der aktuelle Commit (HEAD) behaelt die vier .bin je Chip. Aufruf NACH dem Commit des neuen Builds:
#   .\cleanup_history.ps1          schreibt die Historie lokal um (Sicherung als Bundle in %TEMP%), pusht nicht
#   .\cleanup_history.ps1 -Push    schreibt um und pusht danach per Force-Push (master und Tags)
# Achtung: Die Commit-IDs aller aelteren Commits und Tags aendern sich. Alte Builds bleiben in den Release-Paketen; Klone, die vorher
# angelegt wurden, muessen neu geholt werden (git fetch, git reset --hard origin/master).
#
# Removes the firmware (build_uhr4\esp32*) from all older commits so the repository does not grow by ~11 MB with every new build.
# Only the current commit (HEAD) keeps the four .bin files per chip. Run AFTER committing the new build:
#   .\cleanup_history.ps1          rewrites the history locally (backup as a bundle in %TEMP%), does not push
#   .\cleanup_history.ps1 -Push    rewrites and then force-pushes (master and tags)
# Note: the commit IDs of all older commits and tags change. Old builds stay in the release packages; clones made earlier have to be
# fetched again (git fetch, git reset --hard origin/master).
param(
    [switch]$Push,
    [string[]]$Folder = @('build_uhr4/esp32s2', 'build_uhr4/esp32c6', 'build_uhr4/esp32s3', 'build_uhr4/esp32s3lcd4', 'build_uhr4/esp32s3qualia'),
    [string]$Branch = 'master'
)
# Continue statt Stop: Windows PowerShell 5.1 macht aus Textausgaben von git auf stderr sonst Fehler; Rueckgabewerte werden einzeln geprueft
# Continue instead of Stop: Windows PowerShell 5.1 otherwise turns git's stderr text into errors; exit codes are checked one by one
$ErrorActionPreference = 'Continue'
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root

if (git status --porcelain --untracked-files=no) { throw 'tracked files have uncommitted changes - commit or stash first' }
if ((git rev-parse --abbrev-ref HEAD).Trim() -ne $Branch) { throw "not on branch $Branch" }

$tip = (git rev-parse HEAD).Trim()
# Aeltere Commits, in deren Stand die Firmware liegt (auch Commits ohne Aenderung daran, die sie nur mittragen)
# Older commits whose state contains the firmware (also commits that did not change it but just carry it along)
$old = @(git rev-list --branches --tags | Where-Object { $_ -ne $tip -and (git ls-tree -r --name-only $_ -- $Folder) })
if ($old.Count -eq 0) { Write-Host 'Nothing to clean: only the newest commit contains the firmware.'; exit 0 }
Write-Host "$($old.Count) older commit(s) contain the firmware and will be cleaned."

$backup = Join-Path $env:TEMP ("repo-backup-{0}.bundle" -f (Get-Date -Format 'yyyyMMdd-HHmmss'))
git bundle create $backup --all 2>&1 | Out-Null
if ($LASTEXITCODE -ne 0 -or -not (Test-Path $backup)) { throw 'backup bundle could not be created' }
Write-Host "Backup: $backup"

# Filter als Datei, damit keine Anfuehrungszeichen durch die Shell wandern / filter as a file so no quotes travel through the shell
$quoted = ($Folder | ForEach-Object { "`"$_`"" }) -join ' '
$filter = Join-Path $env:TEMP 'uhr4-index-filter.sh'
[IO.File]::WriteAllText($filter, "#!/bin/sh`nif [ `"`$GIT_COMMIT`" != `"`$KEEP_COMMIT`" ]; then git rm -r -q --cached --ignore-unmatch -- $quoted; fi`n", (New-Object Text.UTF8Encoding($false)))
$env:KEEP_COMMIT = $tip
$env:FILTER_BRANCH_SQUELCH_WARNING = '1'
$shPath = $filter -replace '\\', '/'
git filter-branch -f --index-filter "sh '$shPath'" --tag-name-filter cat -- --branches --tags 2>&1 | ForEach-Object { "$_" } | Select-Object -Last 3
if ($LASTEXITCODE -ne 0) { throw 'git filter-branch failed (backup: ' + $backup + ')' }

# Sicherungsreferenzen und alte Objekte entfernen / remove backup refs and old objects
git for-each-ref --format='%(refname)' refs/original | ForEach-Object { git update-ref -d $_ }
git reflog expire --expire=now --all
git gc -q --prune=now

$bins = @(git ls-tree -r --name-only HEAD -- $Folder)
Write-Host "HEAD keeps $($bins.Count) file(s) in $($Folder -join ', ')"
if ($bins.Count -eq 0) { throw 'firmware missing in HEAD - restore from the backup: ' + $backup }
Write-Host ('.git size: {0:N1} MB' -f ((Get-ChildItem .git -Recurse -File | Measure-Object Length -Sum).Sum / 1MB))

if ($Push) {
    git push --force origin $Branch
    if ($LASTEXITCODE -ne 0) { throw 'push failed' }
    git push --force origin --tags
    Write-Host 'Pushed (force).'
}
else { Write-Host 'Not pushed. Push with: git push --force origin master; git push --force origin --tags (or run again with -Push).' }
