<#
.SYNOPSIS
    Unpack the game data out of your own Super Mario 64 DS cartridge dump.

.DESCRIPTION
    Reads a .nds file you supply and writes the game's internal filesystem next
    to this script, in the layout the port expects:

        extracted\dsd\files\<path>     every file, byte for byte as the card has it
        build\assets\files.tsv         file id -> path index, rebuilt from the dump
        build\assets\handles.tsv       game handle -> file id, read out of overlay 0
        build\assets\romdata.bin       code-side data tables, rebuilt from the dump
                                       (the kit's romdata.recipe.tsv says where each
                                       piece lives; the file is hash-checked before
                                       it is written)

    Nothing is downloaded and nothing is installed. Windows PowerShell 5.1 is
    enough; there is no Python, no ndspy and no other dependency.

    The extraction is a plain copy: a file's bytes on disk are exactly the bytes
    the cartridge holds for it. Compressed files are left compressed -- Super
    Mario 64 DS stores those with a four-byte ASCII "LZ77" tag in front of the
    usual type-0x10 header, and the game's own loader looks for that tag, so
    stripping or decoding anything here would break it.

.PARAMETER Rom
    Path to your .nds dump. Optional and taken first when supplied (the launcher
    passes the file it found this way). If you leave it out, the script looks in
    the drop folder next to it ("PLACE YOUR ROM HERE", or the older
    "PLACE EU ROM HERE"), then falls back to a single .nds sitting next to it.

.PARAMETER Destination
    Where to write extracted\ and build\. Defaults to this script's folder.

.EXAMPLE
    .\extract_assets.ps1
    .\extract_assets.ps1 -Rom D:\dumps\sm64ds.nds
#>
[CmdletBinding()]
param(
    [string] $Rom,
    [string] $Destination
)

$ErrorActionPreference = 'Stop'

function Write-Step($text) { Write-Host $text }
function Write-Warn($text) { Write-Host $text -ForegroundColor Yellow }

# Machine-readable progress for a host that is parsing stdout (the launcher).
# One line, '##PROGRESS <pct> <short phase text>'. Percentages are honest against
# the known phase weights below; the file dump dominates, so it owns the widest
# band. A human running the script directly just sees these as extra lines; the
# Write-Step narration is unchanged. Kept on its own line and flushed so the
# launcher's line-reader gets it promptly.
function Write-Prog([int]$pct, [string]$phase) {
    if ($pct -lt 0) { $pct = 0 } elseif ($pct -gt 100) { $pct = 100 }
    Write-Host ("##PROGRESS {0} {1}" -f $pct, $phase)
}

# Machine-readable failure reason, for the same host that parses the progress
# markers. One line, '##REASON <text>', printed immediately before the human
# narration of the same failure.
#
# This exists because the launcher shows the player ONE line when extraction
# fails, and it used to have nothing to show but the exit code. Anything that
# ends this script non-zero must come through here first, so "Extraction failed
# (exit code 1)" is never the whole story again.
#
# Control characters are stripped, not just newlines: several of these messages
# quote text taken straight out of the file the player supplied, and a file that
# is not a cartridge dump quotes raw binary. A stray carriage return would split
# the marker into two lines and the launcher would show half a sentence. Length
# is capped for the same reason, because this ends up on one line of a small
# window.
function Write-Reason([string]$text) {
    $one = (($text -replace '[^ -~]', ' ') -replace '\s+', ' ').Trim()
    if ($one.Length -gt 400) { $one = $one.Substring(0, 397) + '...' }
    Write-Host ("##REASON {0}" -f $one)
}

# What the script is doing right now, in a few words that fit the sentence
# "... failed while $script:Doing", plus the file it is doing it to. The trap
# below uses both to say WHERE an unexpected failure happened, which is the
# difference between "access denied" and "access denied while writing the
# unpacked files".
$script:Doing = 'starting up'
$script:DoingPath = ''

# True if a file is a cloud placeholder whose bytes are not on this PC yet
# (OneDrive "Files On-Demand" and the like). Windows 11 puts the Desktop inside
# OneDrive by default, so a kit unzipped to the Desktop is very often in a synced
# folder, and a ROM dropped into it can be a stub Windows has to download before
# anything can read it. These are the documented attributes: OFFLINE,
# RECALL_ON_OPEN, RECALL_ON_DATA_ACCESS.
#
# Only ever used to EXPLAIN a read that already failed. A placeholder that
# downloads on demand works fine, so this must never block a healthy install.
function Test-CloudPlaceholder([string]$path) {
    try {
        $a = [int][IO.File]::GetAttributes($path)
        return (($a -band 0x1000) -ne 0) -or (($a -band 0x40000) -ne 0) -or (($a -band 0x400000) -ne 0)
    } catch { return $false }
}

# Turns a PowerShell error into one plain sentence that names a cause and a next
# step. PowerShell wraps every failure from a .NET call in a
# MethodInvocationException, so unwrap to the real exception first, otherwise
# everything looks like "Exception calling WriteAllBytes".
function Get-PlainReason($errorRecord, [string]$doing, [string]$path) {
    $ex = $errorRecord.Exception
    while ($ex -and $ex.InnerException -and
           ($ex -is [Management.Automation.MethodInvocationException])) {
        $ex = $ex.InnerException
    }
    $where = if ($doing) { " while $doing" } else { "" }
    $code = 0
    try { $code = [int]$ex.HResult -band 0xFFFF } catch { }

    if ($ex -is [IO.PathTooLongException]) {
        return ("A file path was too long$where. Move the kit closer to the top of the " +
                "drive (for example C:\SM64DS) and press Play again.")
    }
    if ($ex -is [UnauthorizedAccessException]) {
        return ("Windows refused permission$where. This usually means the kit is in a " +
                "protected folder (Program Files), or security software is blocking it. " +
                "Move the kit to a normal folder (your Desktop or Documents), or allow it " +
                "in your antivirus, then press Play again.")
    }
    if ($ex -is [OutOfMemoryException]) {
        return "Ran out of memory$where. Close other programs and press Play again."
    }
    if ($ex -is [IO.IOException]) {
        # ERROR_DISK_FULL (112), ERROR_HANDLE_DISK_FULL (39).
        if ($code -eq 112 -or $code -eq 39) {
            return ("The disk is full$where. Free up some space (about 100 MB is plenty) " +
                    "and press Play again.")
        }
        # ERROR_SHARING_VIOLATION (32), ERROR_LOCK_VIOLATION (33).
        if ($code -eq 32 -or $code -eq 33) {
            return ("Another program is holding a file open$where. Close the game, any " +
                    "antivirus scan and any open folder window, then press Play again.")
        }
        # The cloud filter range (362 to 395) is what OneDrive and similar report
        # when a placeholder cannot be downloaded.
        if (($code -ge 362 -and $code -le 395) -or ($path -and (Test-CloudPlaceholder $path))) {
            return ("A file is stored online only and Windows could not download it$where. " +
                    "This happens with OneDrive (or similar) when a file is not kept on " +
                    'this PC. Right-click the kit folder and your .nds file, choose "Always ' +
                    'keep on this device", wait for it to finish, then press Play again.')
        }
    }
    $msg = if ($ex) { $ex.Message } else { "$errorRecord" }
    return "The unpack step failed$where. Windows reported: $msg"
}

# Anything that reaches this trap is a failure nobody wrote a message for: a
# permission, disk or cloud-sync problem inside a .NET call, or a bug in this
# script. Left alone, PowerShell prints a multi-line error record to stderr and
# exits 1, and that exit code was all the player ever saw. Say it in one plain
# line first, then still print the technical detail so the log keeps it.
trap {
    $reason = Get-PlainReason $_ $script:Doing $script:DoingPath
    Write-Reason $reason
    Write-Host ""
    Write-Host $reason -ForegroundColor Red
    Write-Host ""
    Write-Host ("Technical detail: " + $_.Exception.Message)
    exit 1
}

function Stop-Politely($text) {
    Write-Reason $text
    Write-Host ""
    Write-Host $text -ForegroundColor Red
    Write-Host ""
    Write-Host "This kit contains no game data of its own, by design. It needs a dump" -ForegroundColor Red
    Write-Host "of a Super Mario 64 DS cartridge that you own. See README.txt." -ForegroundColor Red
    exit 1
}

# Same as Stop-Politely, for a failure that is NOT about the player's dump: no
# room on the disk, no permission to write, a kit that arrived incomplete. The
# "you need to supply a cartridge dump" trailer would be a wrong answer there,
# and a confidently wrong answer is worse than a bare exit code.
function Stop-Blocked($text) {
    Write-Reason $text
    Write-Host ""
    Write-Host $text -ForegroundColor Red
    Write-Host ""
    exit 1
}

if (-not $Destination) {
    $Destination = if ($PSScriptRoot) { $PSScriptRoot } else { (Get-Location).Path }
}

# ------------------------------------------------------ can we write here at all
# Everything below writes a couple of thousand files into $Destination. Prove we
# can write ONE first. A kit unzipped into Program Files, a folder marked
# read-only, or a folder an antivirus is holding shut all fail here, in a
# sentence, instead of failing a minute later in the middle of the dump with a
# .NET stack trace and an exit code.
$script:Doing = 'checking that the kit folder can be written to'
$script:DoingPath = $Destination
$probe = Join-Path $Destination ".write-probe-$PID"
try {
    [void][IO.Directory]::CreateDirectory($Destination)
    [IO.File]::WriteAllBytes($probe, [byte[]]@(0))
    [IO.File]::Delete($probe)
} catch {
    Stop-Blocked (Get-PlainReason $_ "checking that the kit folder can be written to" $Destination)
}

# ---------------------------------------------------------------- find the rom
# An explicit -Rom wins (the launcher passes the file it validated). Otherwise
# the named drop folder first -- try both the current bundle folder name and the
# older one -- then next to the script, so a dump placed the old way still works.
if (-not $Rom) {
    # The bundle ships the folder named 'PLACE YOUR ROM HERE'. The earlier kit
    # used 'PLACE EU ROM HERE'; accept both so neither layout breaks. Report the
    # one that actually exists (prefer the current name) in the no-ROM message.
    $dropNames = @('PLACE YOUR ROM HERE', 'PLACE EU ROM HERE')
    $dropDir = $null
    $found = @()
    foreach ($name in $dropNames) {
        $candidate = Join-Path $Destination $name
        if (Test-Path -LiteralPath $candidate -PathType Container) {
            if (-not $dropDir) { $dropDir = $candidate }
            $hits = @(Get-ChildItem -Path $candidate -Filter *.nds -File -ErrorAction SilentlyContinue)
            if ($hits.Count -gt 0) { $found = $hits; break }
        }
    }
    if (-not $dropDir) { $dropDir = Join-Path $Destination $dropNames[0] }
    if ($found.Count -eq 0) {
        $found = @(Get-ChildItem -Path $Destination -Filter *.nds -File -ErrorAction SilentlyContinue)
    }
    if ($found.Count -eq 0) {
        Stop-Politely "No .nds file found in '$dropDir' or next to this script."
    }
    if ($found.Count -gt 1) {
        Write-Host ""
        Write-Host "More than one .nds file is sitting here:" -ForegroundColor Red
        $found | ForEach-Object { Write-Host "    $($_.Name)" }
        Stop-Politely "Leave only the Super Mario 64 DS one, or name it: .\extract_assets.ps1 -Rom <file>"
    }
    $Rom = $found[0].FullName
}
if (-not (Test-Path -LiteralPath $Rom -PathType Leaf)) {
    Stop-Politely ("The .nds file is not there any more: $Rom. Put your Super Mario 64 DS " +
                   "dump back in the folder next to the launcher and press Play again.")
}
$Rom = (Resolve-Path -LiteralPath $Rom).Path

Write-Step "Reading $([IO.Path]::GetFileName($Rom))"
# The launcher has already read the first few hundred bytes of this file to
# check the header, so "it opened once" proves nothing about the rest of it. This
# is the first read of the WHOLE dump, and it is where an online-only OneDrive
# placeholder, a half-copied file or an antivirus block actually shows up.
$script:Doing = 'reading your .nds file'
$script:DoingPath = $Rom
try {
    $romBytes = [IO.File]::ReadAllBytes($Rom)
} catch {
    Stop-Politely (Get-PlainReason $_ "reading your .nds file" $Rom)
}

# ------------------------------------------------------- check it is the game
# Cartridge header: 12-byte internal title, 4-byte game code, then the table
# offsets. A wrong file trips one of these long before it can write nonsense.
if ($romBytes.Length -lt 0x4000) {
    Stop-Politely "That file is only $($romBytes.Length) bytes, too small to be a DS cartridge dump."
}
$title = [Text.Encoding]::ASCII.GetString($romBytes, 0x00, 12).TrimEnd([char]0)
$code  = [Text.Encoding]::ASCII.GetString($romBytes, 0x0C, 4)
if ($title -ne 'S.MARIO64DS' -or $code -notlike 'ASM?') {
    # Whatever this file is, those 16 bytes are not text, so show them as
    # something printable rather than pasting binary into a message box.
    $shownTitle = ($title -replace '[^ -~]', '.')
    $shownCode  = ($code -replace '[^ -~]', '.')
    Stop-Politely ("That is not a Super Mario 64 DS dump (it says title '$shownTitle', " +
                   "game code '$shownCode'). Use a dump of a Super Mario 64 DS cartridge.")
}

$regions = @{ 'E' = 'North America'; 'P' = 'Europe'; 'J' = 'Japan';
              'K' = 'Korea'; 'C' = 'China'; 'U' = 'Australia' }
$regionLetter = $code.Substring(3, 1)
$region = if ($regions.ContainsKey($regionLetter)) { $regions[$regionLetter] } else { "region '$regionLetter'" }

# EU-only gate. The port and this kit's romdata.recipe.tsv are built from the
# European ROM (game code ASMP). A USA/JP/... dump is genuine Super Mario 64 DS,
# passes every header check above, and would otherwise grind all the way to the
# romdata.bin checksum before failing with a confusing "not the revision this
# build was made from" message. Reject it here, up front, by game code, with an
# explicit reason so the player knows exactly which ROM to use.
if ($code -ne 'ASMP') {
    Stop-Politely ("This is the $region ROM (game code $code). This port is built from the " +
                   "European ROM (ASMP), so its data will not come out of a $region cartridge.")
}

$fntOffset = [BitConverter]::ToUInt32($romBytes, 0x40)
$fntSize   = [BitConverter]::ToUInt32($romBytes, 0x44)
$fatOffset = [BitConverter]::ToUInt32($romBytes, 0x48)
$fatSize   = [BitConverter]::ToUInt32($romBytes, 0x4C)
$ovtOffset = [BitConverter]::ToUInt32($romBytes, 0x50)
$ovtSize   = [BitConverter]::ToUInt32($romBytes, 0x54)
$usedSize  = [BitConverter]::ToUInt32($romBytes, 0x80)

foreach ($span in @(@('file name table', $fntOffset, $fntSize),
                    @('file allocation table', $fatOffset, $fatSize),
                    @('overlay table', $ovtOffset, $ovtSize))) {
    if ($span[1] -eq 0 -or ($span[1] + $span[2]) -gt $romBytes.Length) {
        Stop-Politely ("This dump is truncated: its $($span[0]) runs past the end of the file. " +
                       "Re-dump the cartridge.")
    }
}
if ($usedSize -gt $romBytes.Length) {
    Stop-Politely ("This dump is incomplete: the header says the cartridge holds $usedSize bytes " +
                   "but the file is only $($romBytes.Length). Re-dump the cartridge.")
}
Write-Step "Super Mario 64 DS ($code, $region), $([math]::Round($romBytes.Length / 1MB)) MB"
# Phase weights (honest, against wall-clock on a normal run): the file dump
# dominates (~2..70), then the handle table (70..80), then romdata.bin's
# decompress+assemble+hash (80..99). Header/table parsing is instant, so we open
# at 2 once the dump is confirmed good.
Write-Prog 2 "Reading the file tables"

# ------------------------------------------------------------- the file tables
# FAT: one 8-byte {start, end} pair per file id.
$fatCount = [int]($fatSize / 8)
$fatStart = New-Object 'uint32[]' $fatCount
$fatEnd   = New-Object 'uint32[]' $fatCount
for ($i = 0; $i -lt $fatCount; $i++) {
    $fatStart[$i] = [BitConverter]::ToUInt32($romBytes, $fatOffset + $i * 8)
    $fatEnd[$i]   = [BitConverter]::ToUInt32($romBytes, $fatOffset + $i * 8 + 4)
}

# FNT: a directory table of 8-byte entries, each pointing at a name subtable.
# Walk it from the root so a directory is always named before its children.
# Names with the high bit set are subdirectories and carry a trailing id.
$paths = New-Object 'System.Collections.Generic.SortedDictionary[int,string]'
$queue = New-Object 'System.Collections.Generic.Queue[object]'
$queue.Enqueue(@(0, ''))
$visited = New-Object 'System.Collections.Generic.HashSet[int]'
[void]$visited.Add(0)
while ($queue.Count -gt 0) {
    $item = $queue.Dequeue()
    $dirId = [int]$item[0]
    $base  = [string]$item[1]
    $entry = $fntOffset + $dirId * 8
    if (($entry + 8) -gt $romBytes.Length) {
        Stop-Politely "This dump's file name table is damaged. Re-dump the cartridge."
    }
    $o     = $fntOffset + [BitConverter]::ToUInt32($romBytes, $entry)
    $fileId = [int][BitConverter]::ToUInt16($romBytes, $entry + 4)
    while ($true) {
        if ($o -ge $romBytes.Length) {
            Stop-Politely "This dump's file name table is damaged. Re-dump the cartridge."
        }
        $type = $romBytes[$o]; $o++
        if ($type -eq 0) { break }
        $len = $type -band 0x7F
        # The name, and the 2-byte directory id that follows a directory entry,
        # both have to fit inside the file. Without this a damaged table walks off
        # the end and dies inside GetString, which is an argument-range error with
        # nothing in it for a player.
        $needs = $len + $(if ($type -band 0x80) { 2 } else { 0 })
        if (($o + $needs) -gt $romBytes.Length) {
            Stop-Politely "This dump's file name table is damaged. Re-dump the cartridge."
        }
        $name = [Text.Encoding]::ASCII.GetString($romBytes, $o, $len); $o += $len
        if ($type -band 0x80) {
            $sub = [int]([BitConverter]::ToUInt16($romBytes, $o) -band 0x0FFF); $o += 2
            if ($visited.Add($sub)) {
                $queue.Enqueue(@($sub, $(if ($base) { "$base/$name" } else { $name })))
            }
        } else {
            $paths[$fileId] = $(if ($base) { "$base/$name" } else { $name })
            $fileId++
        }
    }
}
if ($paths.Count -eq 0) {
    Stop-Politely "This dump has no named files in it. Re-dump the cartridge."
}

# --------------------------------------------------------------- write them out
$filesRoot = Join-Path $Destination 'extracted\dsd\files'

# Add up what we are about to write, and check every entry points inside the
# file, BEFORE writing anything. Two failures get caught here that used to be
# discovered the hard way: a dump whose file table points past its own end (an
# argument-range error from Array.Copy, roughly halfway through the dump), and a
# disk with no room for the result (a disk-full IOException, also halfway
# through, having already written whatever fitted).
$script:Doing = 'checking the dump''s file table'
$script:DoingPath = $Rom
$needBytes = [long]0
foreach ($pair in $paths.GetEnumerator()) {
    $id = $pair.Key
    if ($id -lt 0 -or $id -ge $fatCount) {
        Stop-Politely ("This dump is damaged: its file name table names a file (" +
                       "$($pair.Value)) that its file allocation table does not have. " +
                       "Re-dump the cartridge.")
    }
    $length = [long]$fatEnd[$id] - [long]$fatStart[$id]
    if ($length -lt 0 -or ([long]$fatStart[$id] + $length) -gt $romBytes.Length) {
        Stop-Politely ("This dump is damaged: its file table points at data past the end " +
                       "of the file (file $id, '$($pair.Value)'). Re-dump the cartridge.")
    }
    $needBytes += $length
}

# Free space. Skipped, never fatal, if the drive cannot be measured (a network
# path, a substituted drive): a check we cannot make must not stop a good install.
$script:Doing = 'checking free disk space'
$script:DoingPath = $Destination
try {
    $root = [IO.Path]::GetPathRoot([IO.Path]::GetFullPath($Destination))
    $free = (New-Object IO.DriveInfo $root).AvailableFreeSpace
    $wantBytes = $needBytes + 16MB      # the dump, plus room for romdata.bin and slack
    if ($free -lt $wantBytes) {
        Stop-Blocked ("There is not enough free space on $root to unpack the game data. " +
                      "About $([math]::Round($wantBytes / 1MB)) MB is needed and " +
                      "$([math]::Round($free / 1MB)) MB is free. Free up some space and " +
                      "press Play again.")
    }
} catch [Management.Automation.MethodInvocationException] {
    Write-Warn "    (could not measure free disk space here; carrying on)"
} catch [ArgumentException] {
    Write-Warn "    (could not measure free disk space here; carrying on)"
}

Write-Step "Writing $($paths.Count) files to $filesRoot"
$script:Doing = 'writing the unpacked files'
$madeDirs = New-Object 'System.Collections.Generic.HashSet[string]'
$written = 0
$bytes = [long]0
$total = $paths.Count
foreach ($pair in $paths.GetEnumerator()) {
    $id = $pair.Key
    $relative = $pair.Value.Replace('/', '\')
    $target = Join-Path $filesRoot $relative
    $script:DoingPath = $target
    $dir = [IO.Path]::GetDirectoryName($target)
    if ($madeDirs.Add($dir)) { [void][IO.Directory]::CreateDirectory($dir) }
    $length = [int]($fatEnd[$id] - $fatStart[$id])
    $buffer = New-Object 'byte[]' $length
    if ($length -gt 0) { [Array]::Copy($romBytes, [int]$fatStart[$id], $buffer, 0, $length) }
    [IO.File]::WriteAllBytes($target, $buffer)
    $written++
    $bytes += $length
    if (($written % 500) -eq 0) {
        Write-Step "    $written of $total"
        # File dump spans 2..70. There are a few thousand files, so every 500
        # keeps the bar moving well under 5s apart.
        Write-Prog (2 + [int](68 * $written / $total)) "Unpacking file $written of $total"
    }
}
Write-Step "    $written files, $([math]::Round($bytes / 1MB, 1)) MB"
Write-Prog 70 "Unpacked $written files"

# ------------------------------------------------------------ overlay 0 for the
# handle table. The game asks for assets by handle, not by file id; overlay 0
# carries the handle -> path-string table that translates them. Overlays are
# stored back-to-front LZ compressed ("BLZ"), so it has to be decoded first.
function Expand-Blz([byte[]] $data) {
    if ($data.Length -lt 8) { return $data }
    $footer = [BitConverter]::ToUInt32($data, $data.Length - 8)
    $extraSize = [BitConverter]::ToUInt32($data, $data.Length - 4)
    if ($extraSize -eq 0) { return $data }        # flagged, but stored plain
    $headerLen = [int]($footer -shr 24)
    $compressedLen = [int]($footer -band 0xFFFFFF)
    if ($headerLen -lt 8 -or $headerLen -gt $data.Length -or $compressedLen -gt $data.Length) {
        throw "overlay 0 has an unreadable compression header"
    }
    if ($compressedLen -ge $data.Length) { $compressedLen = $data.Length }

    # Anything before the compressed run is already plain and stays put.
    $passthrough = $data.Length - $compressedLen
    $compLen = $compressedLen - $headerLen
    $out = New-Object 'byte[]' ($data.Length + $extraSize - $passthrough)
    $outLen = $out.Length

    # Both streams run backwards: index -1-n from the end of each.
    $read = 0; $done = 0; $flags = 0; $mask = 1
    while ($done -lt $outLen) {
        if ($mask -eq 1) {
            if ($read -ge $compressedLen) { throw "overlay 0 ended mid-stream" }
            $flags = $data[$passthrough + $compLen - 1 - $read]; $read++
            $mask = 0x80
        } else {
            $mask = $mask -shr 1
        }
        if ($flags -band $mask) {
            $b1 = $data[$passthrough + $compLen - 1 - $read]; $read++
            $b2 = $data[$passthrough + $compLen - 1 - $read]; $read++
            $length = ($b1 -shr 4) + 3
            $disp = ((($b1 -band 0x0F) -shl 8) -bor $b2) + 3
            if ($disp -gt $done) {
                if ($done -lt 2) { throw "overlay 0 back-references data that is not there yet" }
                $disp = 2
            }
            $from = $done - $disp
            for ($i = 0; $i -lt $length; $i++) {
                $out[$outLen - 1 - $done] = $out[$outLen - 1 - $from]
                $from++; $done++
            }
        } else {
            $out[$outLen - 1 - $done] = $data[$passthrough + $compLen - 1 - $read]
            $read++; $done++
        }
    }
    $result = New-Object 'byte[]' ($passthrough + $outLen)
    if ($passthrough -gt 0) { [Array]::Copy($data, 0, $result, 0, $passthrough) }
    [Array]::Copy($out, 0, $result, $passthrough, $outLen)
    return $result
}

# Overlay table entries are 32 bytes: id, ram address, size, bss, two static
# initialiser bounds, file id, then (compressed size | flags << 24).
if ($ovtSize -lt 32) { Stop-Politely "This dump has no ARM9 overlays. Re-dump the cartridge." }
$ov0Ram    = [BitConverter]::ToUInt32($romBytes, $ovtOffset + 4)
$ov0FileId = [int][BitConverter]::ToUInt32($romBytes, $ovtOffset + 24)
$ov0Flags  = [int]([BitConverter]::ToUInt32($romBytes, $ovtOffset + 28) -shr 24)
$script:Doing = 'reading overlay 0 out of the dump'
if ($ov0FileId -lt 0 -or $ov0FileId -ge $fatCount) {
    Stop-Politely ("This dump is damaged: its overlay table points at a file its file " +
                   "allocation table does not have. Re-dump the cartridge.")
}
$ov0Length = [int]($fatEnd[$ov0FileId] - $fatStart[$ov0FileId])
$ov0 = New-Object 'byte[]' $ov0Length
[Array]::Copy($romBytes, [int]$fatStart[$ov0FileId], $ov0, 0, $ov0Length)
# The BLZ decode below runs silently and is not instant, so announce it before
# starting -- this is the step that looked hung in testing.
Write-Prog 71 "Decompressing overlay 0"
if ($ov0Flags -band 1) {
    try { $ov0 = Expand-Blz $ov0 }
    catch { Stop-Politely "Could not read overlay 0 of this dump ($_). Re-dump the cartridge." }
}

# The table's address and length are properties of the game, not of the dump:
# the matched initialiser func_ov000_020aa420 walks exactly this many entries.
$handleTableAddress = 0x020BD4B8
$handleCount = 0x80A
$tableOffset = $handleTableAddress - $ov0Ram
if ($tableOffset -lt 0 -or ($tableOffset + $handleCount * 4) -gt $ov0.Length) {
    Stop-Politely ("This dump is Super Mario 64 DS but not a revision this build knows: " +
                   "its overlay 0 has no asset-handle table where one is expected.")
}

$idByPath = @{}
foreach ($pair in $paths.GetEnumerator()) { $idByPath[$pair.Value] = $pair.Key }

$kinds = @{ '.bca' = 'animation'; '.bmd' = 'model'; '.btp' = 'texture-sequence';
            '.kcl' = 'collision'; '.narc' = 'archive'; '.sdat' = 'sound-archive';
            '.bin' = 'data' }
function Get-Kind($path) {
    $suffix = [IO.Path]::GetExtension($path).ToLowerInvariant()
    if ($kinds.ContainsKey($suffix)) { $kinds[$suffix] } else { 'file' }
}

Write-Step "Reading the asset-handle table out of overlay 0"
Write-Prog 72 "Reading the asset-handle table"
$handleRows = New-Object Text.StringBuilder
[void]$handleRows.Append("handle`thex_handle`tfile_id`thex_file_id`tpath`tkind`tsize`n")
for ($h = 0; $h -lt $handleCount; $h++) {
    # Handle table spans 72..80.
    if (($h % 400) -eq 0 -and $h -gt 0) {
        Write-Prog (72 + [int](8 * $h / $handleCount)) "Cataloguing asset handle $h of $handleCount"
    }
    $pointer = [BitConverter]::ToUInt32($ov0, $tableOffset + $h * 4)
    $stringOffset = [int]($pointer - $ov0Ram)
    if ($stringOffset -lt 0 -or $stringOffset -ge $ov0.Length) {
        Stop-Politely ("This dump is Super Mario 64 DS but not a revision this build knows: " +
                       "asset handle $h points outside overlay 0.")
    }
    $end = $stringOffset
    while ($end -lt $ov0.Length -and $ov0[$end] -ne 0) { $end++ }
    $assetPath = [Text.Encoding]::ASCII.GetString($ov0, $stringOffset, $end - $stringOffset)
    if (-not $idByPath.ContainsKey($assetPath)) {
        Stop-Politely ("This dump is Super Mario 64 DS but not a revision this build knows: " +
                       "asset handle $h names '$assetPath', which its filesystem does not have.")
    }
    $id = $idByPath[$assetPath]
    $size = [int]($fatEnd[$id] - $fatStart[$id])
    [void]$handleRows.Append(("{0}`t0x{0:x4}`t{1}`t0x{1:x4}`t{2}`t{3}`t{4}`n" -f
                              $h, $id, $assetPath, (Get-Kind $assetPath), $size))
}

$fileRows = New-Object Text.StringBuilder
[void]$fileRows.Append("file_id`thex_id`tpath`tkind`tsize`n")
foreach ($pair in $paths.GetEnumerator()) {
    $id = $pair.Key
    $size = [int]($fatEnd[$id] - $fatStart[$id])
    [void]$fileRows.Append(("{0}`t0x{0:x4}`t{1}`t{2}`t{3}`n" -f
                            $id, $pair.Value, (Get-Kind $pair.Value), $size))
}

$assetsDir = Join-Path $Destination 'build\assets'
$script:Doing = 'writing the asset catalogue'
$script:DoingPath = $assetsDir
[void][IO.Directory]::CreateDirectory($assetsDir)
$utf8 = New-Object Text.UTF8Encoding $false
[IO.File]::WriteAllText((Join-Path $assetsDir 'files.tsv'), $fileRows.ToString(), $utf8)
[IO.File]::WriteAllText((Join-Path $assetsDir 'handles.tsv'), $handleRows.ToString(), $utf8)
Write-Step "    $($paths.Count) files and $handleCount handles catalogued"
Write-Prog 80 "Rebuilding romdata.bin"

# ------------------------------------------------------------ romdata.bin
# ROM-CLEAN: the game's executable ships with every code-side data table
# zeroed and fills them at boot from build\assets\romdata.bin. The kit ships
# romdata.recipe.tsv -- offsets and hashes only, no game bytes -- and this
# stage rebuilds romdata.bin from the dump: decompress the arm9 program and
# the overlays the recipe names, copy each range, hash-check the result.
# Ranges that reach past a decompressed image are runtime-zero (bss) and stay
# zero, exactly as the recipe's hashes expect.
$recipePath = Join-Path $Destination 'romdata.recipe.tsv'
if (-not (Test-Path -LiteralPath $recipePath)) {
    $recipePath = Join-Path $Destination 'build\assets\romdata.recipe.tsv'
}
if (-not (Test-Path -LiteralPath $recipePath)) {
    # This used to be a warning that still exited 0. It never could be: without
    # romdata.bin the game refuses to start, so a "successful" run that skipped it
    # just moved the failure somewhere with less to say about it.
    Stop-Blocked ("This kit is incomplete: romdata.recipe.tsv is missing, so the game data " +
                  "cannot be rebuilt. Download the kit again and unpack the whole zip into " +
                  "one folder (keep the files together).")
} else {
    $script:Doing = 'reading romdata.recipe.tsv from the kit'
    $script:DoingPath = $recipePath
    try {
        $recipeLines = [IO.File]::ReadAllLines($recipePath)
    } catch {
        Stop-Blocked (Get-PlainReason $_ "reading romdata.recipe.tsv from the kit" $recipePath)
    }
    if ($recipeLines.Count -lt 2 -or $recipeLines[0] -notmatch '^# romdata-recipe v1 ([0-9a-f]{64}) (\d+)$') {
        Stop-Blocked ("This kit's romdata.recipe.tsv is damaged, so the game data cannot be " +
                      "rebuilt. Download the kit again.")
    }
    $wantSha = $Matches[1]
    $blobTotal = [int]$Matches[2]

    # Which images the recipe needs, and where the overlays sit in the dump.
    # Only the header was ever checked, so a truncated or re-saved recipe reached
    # the assembler as blank and half-width lines and died on a null field. Check
    # the shape of every row here, where the answer is "your copy of the kit is
    # damaged" rather than an argument-null error from inside a loop.
    $needed = New-Object 'System.Collections.Generic.HashSet[string]'
    foreach ($line in $recipeLines) {
        if ($line.StartsWith('#') -or $line.Trim().Length -eq 0) { continue }
        $f = $line.Split("`t")
        if ($f.Count -lt 4) {
            Stop-Blocked ("This kit's romdata.recipe.tsv is damaged (a line is incomplete), " +
                          "so the game data cannot be rebuilt. Download the kit again.")
        }
        [void]$needed.Add($f[2])
    }
    $ovByName = @{}
    for ($i = 0; $i -lt [int]($ovtSize / 32); $i++) {
        $e = $ovtOffset + $i * 32
        $ovId = [BitConverter]::ToUInt32($romBytes, $e)
        $ovByName[('ov{0:d3}' -f $ovId)] = @(
            [int][BitConverter]::ToUInt32($romBytes, $e + 24),          # file id
            [int]([BitConverter]::ToUInt32($romBytes, $e + 28) -shr 24) # flags
        )
    }

    Write-Step "Rebuilding romdata.bin ($($needed.Count) images to decompress, the slow part, please wait)"
    $script:Doing = 'rebuilding the game data from the dump'
    $script:DoingPath = $Rom
    $images = @{}
    # Image decompression spans 80..97. Announce each image before decompressing
    # it (the decompress itself is silent and can be a second or two), so the bar
    # never sits still through this phase.
    $imgDone = 0
    $imgTotal = $needed.Count
    foreach ($src in $needed) {
        Write-Prog (80 + [int](17 * $imgDone / $imgTotal)) "Decompressing $src ($($imgDone + 1) of $imgTotal)"
        if ($src -eq 'arm9') {
            # Program header: rom offset at 0x20, ram address 0x28, size 0x2C.
            # The payload is stored with the same back-to-front compression as
            # the overlays.
            $a9Off  = [int][BitConverter]::ToUInt32($romBytes, 0x20)
            $a9Size = [int][BitConverter]::ToUInt32($romBytes, 0x2C)
            $a9 = New-Object 'byte[]' $a9Size
            [Array]::Copy($romBytes, $a9Off, $a9, 0, $a9Size)
            try { $images[$src] = Expand-Blz $a9 }
            catch { Stop-Politely "Could not read this dump's program data ($_). Re-dump the cartridge." }
        } else {
            if (-not $ovByName.ContainsKey($src)) {
                Stop-Politely ("This dump is Super Mario 64 DS but not a revision this build knows: " +
                               "it has no overlay '$src'.")
            }
            $fileId = $ovByName[$src][0]
            if ($fileId -lt 0 -or $fileId -ge $fatCount) {
                Stop-Politely ("This dump is damaged: its overlay table points at a file its " +
                               "file allocation table does not have (overlay '$src'). " +
                               "Re-dump the cartridge.")
            }
            $length = [int]($fatEnd[$fileId] - $fatStart[$fileId])
            if ($length -lt 0 -or ([long]$fatStart[$fileId] + $length) -gt $romBytes.Length) {
                Stop-Politely ("This dump is damaged: overlay '$src' runs past the end of the " +
                               "file. Re-dump the cartridge.")
            }
            $raw = New-Object 'byte[]' $length
            [Array]::Copy($romBytes, [int]$fatStart[$fileId], $raw, 0, $length)
            if ($ovByName[$src][1] -band 1) {
                try { $raw = Expand-Blz $raw }
                catch { Stop-Politely "Could not read overlay '$src' of this dump ($_). Re-dump the cartridge." }
            }
            $images[$src] = $raw
        }
        Write-Step "    $src ($([math]::Round($images[$src].Length / 1KB)) KB)"
        $imgDone++
    }

    # Assemble. The buffer starts zeroed, so a range past its image's end
    # simply keeps its zeros.
    Write-Prog 98 "Assembling and checking romdata.bin"
    $script:Doing = 'assembling romdata.bin'
    $script:DoingPath = ''
    $blob = New-Object 'byte[]' $blobTotal
    foreach ($line in $recipeLines) {
        if ($line.StartsWith('#') -or $line.Trim().Length -eq 0) { continue }
        $f = $line.Split("`t")
        $off = [int]$f[0]; $size = [int]$f[1]; $img = $images[$f[2]]; $srcOff = [int]$f[3]
        if ($off -lt 0 -or $size -lt 0 -or $srcOff -lt 0 -or ($off + $size) -gt $blobTotal) {
            Stop-Blocked ("This kit's romdata.recipe.tsv is damaged (a line is out of range), " +
                          "so the game data cannot be rebuilt. Download the kit again.")
        }
        $have = [Math]::Min($size, [Math]::Max(0, $img.Length - $srcOff))
        if ($have -gt 0) { [Array]::Copy($img, $srcOff, $blob, $off, $have) }
    }

    $sha = [BitConverter]::ToString(
        [Security.Cryptography.SHA256]::Create().ComputeHash($blob)).Replace('-', '').ToLowerInvariant()
    if ($sha -ne $wantSha) {
        # The header said ASMP, so this is a European cartridge dump, and it still
        # did not rebuild. What is left is a dump that is not a byte-for-byte copy
        # of the retail cartridge: a patched or translated ROM, a trimmed one, or
        # a bad dump. Say that, because "checksum mismatch" sends people looking
        # for the wrong problem.
        Stop-Politely ("This is a European dump (game code $code), but its contents do not " +
                       "match the cartridge this build was made from. That usually means the " +
                       "file has been modified (a patch, a translation or a trimmed dump) or " +
                       "the dump is faulty. Use an untouched dump of your own retail " +
                       "cartridge, then press Play again.")
    }
    $script:Doing = 'writing the rebuilt game data'
    $script:DoingPath = (Join-Path $assetsDir 'romdata.bin')
    [IO.File]::WriteAllBytes((Join-Path $assetsDir 'romdata.bin'), $blob)
    Write-Step "    romdata.bin verified ($blobTotal bytes, checksum OK)"

    # The game verifies romdata.bin at boot against romdata.manifest, which the
    # kit ships next to this script; put it where the game looks.
    $script:Doing = 'copying romdata.manifest into place'
    $manifestSrc = Join-Path $Destination 'romdata.manifest'
    $script:DoingPath = $manifestSrc
    if (Test-Path -LiteralPath $manifestSrc) {
        Copy-Item -LiteralPath $manifestSrc (Join-Path $assetsDir 'romdata.manifest') -Force
    } elseif (-not (Test-Path -LiteralPath (Join-Path $assetsDir 'romdata.manifest'))) {
        # Also a warning that exited 0 once, and also guaranteed to strand the
        # player at a game that refuses to start.
        Stop-Blocked ("This kit is incomplete: romdata.manifest is missing, so the game " +
                      "would refuse to start. Download the kit again and unpack the whole " +
                      "zip into one folder (keep the files together).")
    }
}

Write-Prog 100 "Done"
Write-Host ""
Write-Host "Done. The game data is ready in $Destination" -ForegroundColor Green
exit 0
