<#
    THE MP2 MILESTONE: TWO COPIES OF THE GAME, ONE MACHINE, PARENT AND CHILD.
    Run mg16, lane MP2.

    Launches two instances of walk_window, wires them to each other over the
    loopback carrier (UDP on 127.0.0.1), and reports whether they connected.
    Instance one is the DS PARENT and is player 1; instance two is a CHILD and
    is player 2. Window titles carry P1 and P2 so they are tellable apart.

    THE DEFAULT IS -Minimized, AND THAT IS DELIBERATE.
    A test window once appeared over the owner's live game. Since then every
    launch this lane makes is minimized and unactivated, and the automated proof
    ladder (port/tools/mp2_proof.py rung 5) exercises ONLY the minimized path.
    Running this VISIBLY is a human's decision, taken with the owner present,
    which is what -Visible is for.

        # what CI and the proof ladder run:
        powershell -NoProfile -ExecutionPolicy Bypass `
            -File port\tools\mp2_two_windows.ps1 -Minimized

        # what a person runs, with the owner present, to actually look at it:
        powershell -NoProfile -ExecutionPolicy Bypass `
            -File port\tools\mp2_two_windows.ps1 -Visible

    IT DIRECT-BOOTS. SM64DS_LEVEL puts both instances straight into the scene;
    neither waits for the main menu, because the menu path is another lane's
    work and the fan-out call site this lane drives is on the level path
    regardless. Level 1 is the default for the reason the proof ladder gives:
    hal/level_boot.cpp seats the VS star-order pointer data_0209f344 from
    VS_STAR_SPAWN_ORDERS indexed by data_020a1040, the VS local-comms record --
    which is precisely the record this lane fills.

    TWO COPIES ON ONE MACHINE NEED THREE SEPARATIONS, and this script does all
    three (hal/instance_tag.h has the long version):
      * its own WORKING DIRECTORY each      -> playlog/, the selftest BMP
      * its own TEMP each                   -> %TEMP%\sm64ds-crashes, which every
                                               boot prunes to four, so a shared
                                               one has P2 deleting P1's dumps
      * SM64DS_INSTANCE=P1 / P2             -> the exe-adjacent files that ARE
                                               separated: startup_error.txt,
                                               savestate.bin, and settings.json's
                                               sibling temp. NOT crash.txt or
                                               exit.txt -- those stay shared and
                                               hal/instance_tag.h says why. The
                                               same value puts [P1]/[P2] in the
                                               window title.
#>

[CmdletBinding()]
param(
    # Default minimized. -Visible is the opt-in, and it is for a human.
    [switch]$Minimized,
    [switch]$Visible,
    [int]$Frames = 0,             # 0 = play until closed; >0 = a scripted run
    [int]$Level  = 1,
    [int]$Port   = 51765,         # kCommsLoopbackPortBase
    [string]$Root,
    [string]$RunDir
)

$ErrorActionPreference = "Stop"

if (-not $Root) {
    $Root = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
}
$exe = Join-Path $Root "build\port\walk_window.exe"
if (-not (Test-Path $exe)) { Write-Host "MP2 TWO-WINDOW: no walk_window.exe at $exe"; exit 1 }

if (-not $RunDir) { $RunDir = Join-Path $Root "runs\mg16\out\MP2\two_windows" }

# -Visible must be asked for. Anything else, including no switch at all, is
# minimized -- the safe default is the one that cannot land on someone's game.
$showMinimized = -not $Visible
if ($Minimized) { $showMinimized = $true }

$dirs = @{}
foreach ($tag in @("P1","P2")) {
    $d = Join-Path $RunDir $tag
    New-Item -ItemType Directory -Force (Join-Path $d "tmp") | Out-Null
    $dirs[$tag] = $d
}

function Start-Instance {
    # The window TITLE is not a separate knob: SM64DS_INSTANCE puts the tag in
    # front of the title bar (walk_window.cpp, host_window_open) as well as in
    # front of the exe-adjacent filenames, so P1/P2 is one env, not two.
    param([string]$Tag, [string]$Role)

    $d   = $dirs[$Tag]
    $log = Join-Path $d "run.log"
    Remove-Item $log -ErrorAction SilentlyContinue

    # STDERR GOES STRAIGHT TO A FILE, through cmd's own redirection.
    #
    # The first version of this script captured stderr with
    # Register-ObjectEvent + a StringBuilder. It worked for the first instance
    # and SILENTLY LOST THE SECOND: P1's log came out 93071 bytes with 426
    # report lines, P2's came out 1685 bytes with none, cut off mid-boot. The
    # join had actually happened; the evidence had not been recorded. A file
    # redirection cannot half-work like that, and it is what the Python driver
    # (port/tools/mp2_proof.py) does for the same reason.
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName         = "cmd.exe"
    $psi.Arguments        = '/c ""' + $exe + '" 2> "' + $log + '""'
    $psi.WorkingDirectory = $d
    $psi.UseShellExecute  = $false
    # THE SHOW MODE, and why "Minimized" here is not the activating one.
    # .NET's ProcessWindowStyle has no SW_SHOWMINNOACTIVE spelling -- Minimized
    # is SW_SHOWMINIMIZED, which DOES activate, and port/tools/mp2_proof.py asks
    # for 7 (SW_SHOWMINNOACTIVE) directly through STARTUPINFO because Python can.
    # What makes the two agree is the TARGET: walk_window.cpp's host_show_mode()
    # normalises SW_SHOWMINIMIZED, SW_MINIMIZE and SW_SHOWMINNOACTIVE all to
    # SW_SHOWMINNOACTIVE, and SM64DS_NO_FOCUS (set below on this path) composes
    # with it as WS_EX_NOACTIVATE + SW_SHOWNOACTIVATE. So the effective mode is
    # SW_SHOWMINNOACTIVE either way. That is a RELIANCE ON THE TARGET rather
    # than on this line, which is why it is written down: if host_show_mode ever
    # stops normalising, this script starts activating windows and only the
    # comment will say so.
    $psi.WindowStyle      = if ($showMinimized) { "Minimized" } else { "Normal" }
    $psi.CreateNoWindow   = $showMinimized

    # A clean slate: never inherit another lane's SM64DS_* into a session that
    # is about to open a socket.
    foreach ($k in @($psi.EnvironmentVariables.Keys | Where-Object { $_ -like "SM64DS_*" })) {
        $psi.EnvironmentVariables.Remove($k) | Out-Null
    }
    $psi.EnvironmentVariables["SM64DS_ASSET_ROOT"]  = $Root
    $psi.EnvironmentVariables["SM64DS_LEVEL"]       = "$Level"
    $psi.EnvironmentVariables["SM64DS_INSTANCE"]    = $Tag
    $psi.EnvironmentVariables["SM64DS_COMMS_ROLE"]  = $Role
    $psi.EnvironmentVariables["SM64DS_COMMS_PORT"]  = "$Port"
    $psi.EnvironmentVariables["SM64DS_COMMS_FANOUT"]= "1"
    $psi.EnvironmentVariables["SM64DS_COMMS_REPORT"]= "1"
    $psi.EnvironmentVariables["SM64DS_NO_DIALOG"]   = "1"
    $psi.EnvironmentVariables["TEMP"]               = (Join-Path $d "tmp")
    $psi.EnvironmentVariables["TMP"]                = (Join-Path $d "tmp")
    if ($showMinimized) { $psi.EnvironmentVariables["SM64DS_NO_FOCUS"] = "1" }
    if ($Frames -gt 0) {
        $psi.EnvironmentVariables["SM64DS_WINDOW_SELFTEST"] = "$Frames"
        $psi.EnvironmentVariables["SM64DS_FAULTS_FATAL"]    = "1"
    }

    $p = [System.Diagnostics.Process]::Start($psi)
    return [pscustomobject]@{ Proc = $p; Log = $log; Tag = $Tag }
}

Write-Host ("MP2 TWO-WINDOW: starting {0}, level {1}, udp base {2}, frames {3}" -f `
    $(if ($showMinimized) { "MINIMIZED (quiet)" } else { "VISIBLE" }), $Level, $Port, `
    $(if ($Frames -gt 0) { "$Frames" } else { "until closed" }))

# PARENT FIRST. The child needs a door to knock on; the carrier's child arm
# re-knocks every 50 ms until it is accepted, so the gap only has to be small,
# not exact.
$p1 = Start-Instance -Tag "P1" -Role "parent"
Start-Sleep -Milliseconds 600
$p2 = Start-Instance -Tag "P2" -Role "child"

function Read-Log([string]$path) {
    if (-not (Test-Path $path)) { return "" }
    # Shared read: the game still has the file open for writing.
    try {
        $fs = [System.IO.File]::Open($path, 'Open', 'Read', 'ReadWrite')
        $sr = New-Object System.IO.StreamReader($fs)
        $t  = $sr.ReadToEnd()
        $sr.Close(); $fs.Close()
        return $t
    } catch { return "" }
}

# The verdict is read off the GAME'S OWN report line in both logs, not guessed
# from the fact that two processes exist.
$deadline  = (Get-Date).AddSeconds(180)
$connected = $false
$c1 = ""; $c2 = ""
while ((Get-Date) -lt $deadline) {
    $t1 = Read-Log $p1.Log
    $t2 = Read-Log $p2.Log
    $c1 = ($t1 -split "`n" | Where-Object { $_ -match '^\[comms:level\] transport=loopback.*connected=yes.*players=2.*role=1' } | Select-Object -Last 1)
    $c2 = ($t2 -split "`n" | Where-Object { $_ -match '^\[comms:level\] transport=loopback.*connected=yes.*players=2.*role=2' } | Select-Object -Last 1)
    if ($c1 -and $c2) {
        $connected = $true
        Write-Host "MP2 TWO-WINDOW: CONNECTED"
        Write-Host ("  P1 " + $c1.Trim())
        Write-Host ("  P2 " + $c2.Trim())
        break
    }
    if ($p1.Proc.HasExited -and $p2.Proc.HasExited) { break }
    Start-Sleep -Milliseconds 500
}

if ($Frames -gt 0) {
    foreach ($h in @($p1,$p2)) { $h.Proc.WaitForExit(900000) | Out-Null }
} elseif (-not $connected) {
    # Only tear down what we started, and only when the point of the run (the
    # join) did not happen. A play session is the human's to close.
    foreach ($h in @($p1,$p2)) { if (-not $h.Proc.HasExited) { $h.Proc.Kill() } }
}

foreach ($h in @($p1,$p2)) {
    Write-Host ("  {0} exit={1} log={2}" -f $h.Tag,
        $(if ($h.Proc.HasExited) { $h.Proc.ExitCode } else { "running" }), $h.Log)
}

if (-not $connected) { Write-Host "MP2 TWO-WINDOW: NOT CONNECTED"; exit 1 }
exit 0
