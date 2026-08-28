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
    # Close both instances as soon as the join is confirmed. For the proof
    # ladder, which needs PLAY mode (no SM64DS_WINDOW_SELFTEST, so the flight
    # recorder path is exercised) but must still terminate. A human never wants
    # this: a play session is theirs to close.
    [switch]$ExitWhenConnected,
    # AUDIO IS OFF UNLESS THIS IS PASSED, and it is not a convenience switch.
    # Run mg16 lane MP3, review fix 1. This script scrubbed every SM64DS_* out
    # of the child environment -- which is right, a proof run must not inherit
    # another lane's knobs -- and then never put SM64DS_VOLUME back. Everywhere
    # else in this tree the mute is INHERITED from the machine-wide setting, so
    # this was the one arm in the whole port that stripped it, and a run out of
    # this script came up at the game's default volume on the owner's desk.
    # Measured in the lane's own 13:42 logs at 50%.
    #
    # So the rebuilt environment sets SM64DS_VOLUME=0 unconditionally, and only
    # an explicit -Audio re-enables it. That is deliberately not tied to
    # -Visible: a visible run is still a silent one unless a human says
    # otherwise, because the failure this is guarding against was noise on his
    # desk, not a window.
    [switch]$Audio,
    # PLAY MODE, FOR A BOUNDED TIME. Run mg16 lane MP3, field-failure lane.
    # Play mode (no SM64DS_WINDOW_SELFTEST) is what the owner actually runs and
    # what the world rungs must measure, but it has no frame budget: the game
    # runs until someone closes it. -PlaySeconds runs a real play session for N
    # seconds and then closes both, so a headless proof can watch the WORLD in
    # the same configuration the owner plays in. -ExitWhenConnected only ever
    # proved the wire, which is how four world defects reached his hands.
    [int]$PlaySeconds = 0,
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

    # THE GAME IS LAUNCHED DIRECTLY. NO SHIM. Run mg16 lane MP3, review fix 1.
    #
    # This used to be `cmd.exe /c "walk_window.exe 2> log"`, and the redirection
    # was the only reason for it. The cost was invisible and total: every window
    # control below is carried in the STARTUPINFO of the process this line
    # names, so WindowStyle and CreateNoWindow applied to CMD.EXE and the game
    # was created by cmd with default show state. The lane's own 13:42 logs
    # caught it -- the game reported SW_SHOWNOACTIVATE, i.e. VISIBLE, on a run
    # that had asked for minimized. A quiet-launch guarantee that lands on a
    # shim is not a guarantee.
    #
    # It also broke teardown: Kill() below killed cmd and ORPHANED the game,
    # which then sat holding its socket and its window for about a minute.
    #
    # The redirection is kept by asking .NET for the stream and copying it to
    # the log file with CopyToAsync -- a pure stream copy with no PowerShell
    # eventing in it. That distinction matters: the FIRST version of this script
    # used Register-ObjectEvent and silently lost the SECOND instance's log
    # entirely (93071 bytes vs 1685, cut off mid-boot), which is the trap the
    # shim was introduced to avoid. A stream copy cannot half-work that way, and
    # port/tools/mp2_proof.py redirects to a file for the same reason.
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName         = $exe
    $psi.WorkingDirectory = $d
    $psi.UseShellExecute  = $false
    $psi.RedirectStandardError = $true
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
    # THE WORLD SEAT, folded in from the coordinator's field-test patch.
    #
    # This script wired the two instances to each other and never told either
    # one to BOOT A TWO-PLAYER WORLD, so both came up single-player and the
    # only thing the session could be shown to do was exchange blocks. The
    # coordinator added this by hand to get a second body on screen at all,
    # which is what surfaced four world defects the ladder could not see.
    #
    # SM64DS_VS_PLAYERS is a proof knob (hal/comms_conductor.cpp's
    # vs_player_count), not the shipping path: with no transport installed the
    # count is 1 and nothing changes. It is set here because a two-window run
    # is by definition a two-player run, and the alternative -- remembering to
    # pass it -- is what a script is for.
    $psi.EnvironmentVariables["SM64DS_VS_PLAYERS"] = "2"
    $psi.EnvironmentVariables["SM64DS_VS_PROBE"]   = "1"
    # PER-INSTANCE SCRIPTED INPUT, for the play-mode world rungs. In play mode
    # nothing drives the characters -- the selftest's automatic walk is not
    # running and SM64DS_PAD_TEST is a selftest-gated knob -- so a headless
    # proof needs a way to press a direction on ONE window and watch the other.
    # MP3_INJECT_P1/P2 name the instance so a rung can drive exactly one side,
    # which is what input-isolation and one-way-link rungs are made of.
    $inj = if ($Tag -eq "P1") { $env:MP3_INJECT_P1 } else { $env:MP3_INJECT_P2 }
    if ($inj) { $psi.EnvironmentVariables["SM64DS_COMMS_INJECT"] = $inj }
    # run mg16 lane MP4: the state-sync knobs, forwarded past the SM64DS_* scrub
    # above. The scrub is right -- a proof run must not inherit another lane's
    # knobs -- so anything a rung needs has to be named here deliberately, which
    # is also a readable list of what the rungs are allowed to vary.
    foreach ($k in @("SM64DS_SYNC", "SM64DS_SYNC_HZ", "SM64DS_SYNC_LERP",
                     "SM64DS_SYNC_SNAP", "SM64DS_SYNC_REPORT",
                     "SM64DS_SYNC_FORCE_V1", "SM64DS_SYNC_DROP",
                     "SM64DS_SYNC_DELAY_MS", "SM64DS_SYNC_NO_EVENTS",
                     "SM64DS_SYNC_PHASE", "SM64DS_SYNC_NO_DR",
                     "SM64DS_FORCE_ANALOG", "SM64DS_PAD_TEST")) {
        $v = (Get-Item -Path ("env:" + $k) -ErrorAction SilentlyContinue).Value
        if ($v) { $psi.EnvironmentVariables[$k] = $v }
    }
    $psi.EnvironmentVariables["SM64DS_NO_DIALOG"]   = "1"
    $psi.EnvironmentVariables["TEMP"]               = (Join-Path $d "tmp")
    $psi.EnvironmentVariables["TMP"]                = (Join-Path $d "tmp")
    # THE THIRD HALF OF THE QUIET RULE, restored after the scrub above removed
    # it. See the -Audio parameter's note: the scrub is correct and the missing
    # restore is what put sound on the owner's desk. Set LAST of the SM64DS_
    # block so nothing above can win, and only lifted by an explicit -Audio.
    if (-not $Audio) { $psi.EnvironmentVariables["SM64DS_VOLUME"] = "0" }
    # THE MINIMIZE, ASKED FOR IN THE ONE WAY THAT SURVIVES THIS LAUNCHER.
    # $psi.WindowStyle above is set and is NOT ENOUGH: .NET does not carry
    # WindowStyle into STARTUPINFO when UseShellExecute is false, which this
    # launcher needs in order to redirect stderr. Measured -- the game reported
    # SW_SHOWNOACTIVATE (visible, unfocused) on a run that asked for Minimized,
    # with and without CreateNoWindow. walk_window's host_show_mode() takes the
    # same request from this env when STARTUPINFO carries no show spelling; the
    # STARTUPINFO path still wins where a launcher can use it, which is what
    # port/tools/mp2_proof.py does.
    if ($showMinimized) { $psi.EnvironmentVariables["SM64DS_MINIMIZED"] = "1" }
    if ($showMinimized) { $psi.EnvironmentVariables["SM64DS_NO_FOCUS"] = "1" }
    if ($Frames -gt 0) {
        $psi.EnvironmentVariables["SM64DS_WINDOW_SELFTEST"] = "$Frames"
        $psi.EnvironmentVariables["SM64DS_FAULTS_FATAL"]    = "1"
    }

    $p = [System.Diagnostics.Process]::Start($psi)
    # Pump stderr into the log with a plain async stream copy. The FileStream
    # and the Task are handed back so the caller can await and dispose them
    # rather than letting a half-written log look like a short session.
    $fs = New-Object System.IO.FileStream($log,
              [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write,
              [System.IO.FileShare]::Read)
    $copy = $p.StandardError.BaseStream.CopyToAsync($fs)
    return [pscustomobject]@{ Proc = $p; Log = $log; Tag = $Tag;
                             Stream = $fs; Copy = $copy;
                             OwnPid = $p.Id; OwnExe = $exe }
}

# Finish the stderr pump and close the file, so Read-Log below sees everything
# the instance wrote. Called after both processes are done, and safe to call on
# a handle whose process was killed -- the copy completes when the pipe closes.
function Close-Log($h) {
    try { $h.Copy.Wait(5000) | Out-Null } catch { }
    try { $h.Stream.Flush(); $h.Stream.Dispose() } catch { }
}

Write-Host ("MP2 TWO-WINDOW: starting {0}, level {1}, udp base {2}, frames {3}" -f `
    $(if ($showMinimized) { "MINIMIZED (quiet)" } else { "VISIBLE" }), $Level, $Port, `
    $(if ($Frames -gt 0) { "$Frames" } else { "until closed" }))

# PARENT FIRST. The child needs a door to knock on; the carrier's child arm
# re-knocks every 50 ms until it is accepted, so the gap only has to be small,
# not exact.
$p1 = Start-Instance -Tag "P1" -Role "parent"
Write-Host ("MP2 TWO-WINDOW: P1 pid {0} port {1}" -f $p1.OwnPid, $Port)
Start-Sleep -Milliseconds 600
$p2 = Start-Instance -Tag "P2" -Role "child"
Write-Host ("MP2 TWO-WINDOW: P2 pid {0} port {1}" -f $p2.OwnPid, ($Port + 1))

function Read-One([string]$path) {
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

function Read-Log([string]$dir) {
    # THE FLIGHT RECORDER MOVES STDERR, AND THIS SCRIPT'S VERDICT USED TO MISS IT.
    #
    # walk_window.cpp only keeps stderr on the handle its launcher gave it when
    # SM64DS_WINDOW_SELFTEST is SET. With the knob unset -- which is exactly the
    # -Frames 0 PLAY mode a human runs -- it freopen()s stderr into
    # playlog/play_<timestamp>.log a few lines into main. So run.log is created,
    # stays EMPTY for the whole session, and every [comms:level] line this
    # script greps for goes somewhere else.
    #
    # That is not hypothetical: the first visible two-window run reported NOT
    # CONNECTED while both playlogs showed a clean join at round 0 and 6211
    # completed rounds on each side. The session was fine; the verdict was
    # blind. The ladder never caught it because every headless rung sets
    # SM64DS_WINDOW_SELFTEST, which is the one configuration where run.log
    # works.
    #
    # So: read BOTH, and prefer whichever actually has content.
    $t = Read-One (Join-Path $dir "run.log")
    $pl = Join-Path $dir "playlog"
    if (Test-Path $pl) {
        $newest = Get-ChildItem -Path $pl -Filter "*.log" -ErrorAction SilentlyContinue |
                  Sort-Object LastWriteTime | Select-Object -Last 1
        if ($newest) { $t = $t + "`n" + (Read-One $newest.FullName) }
    }
    return $t
}

# The verdict is read off the GAME'S OWN report line in both logs, not guessed
# from the fact that two processes exist.
$deadline  = (Get-Date).AddSeconds(180)
$connected = $false
$c1 = ""; $c2 = ""
while ((Get-Date) -lt $deadline) {
    $t1 = Read-Log $dirs["P1"]
    $t2 = Read-Log $dirs["P2"]
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

# ===========================================================================
# NEVER KILL A PROCESS THIS SCRIPT DID NOT START.
#
# Run mg16 lane MP3, after this harness twice ended a game instance the owner
# was LIVE-DRIVING in another session on the same machine. Another
# walk_window.exe on this desktop is HIS GAME until proven otherwise, and a
# proof harness has no business ending one.
#
# Every kill below already targeted a .NET Process object this script started
# rather than an image name -- there is no taskkill and no Get-Process here,
# and there never was. That was not enough to make the rule visible, so it is
# enforced rather than assumed: Stop-Mine refuses anything whose PID is not the
# one we recorded at spawn AND whose image is not the exact exe we launched.
# A handle we hold cannot have its PID recycled underneath us, so the pair is
# a complete identity check.
function Stop-Mine($h) {
    if ($h.Proc.HasExited) { return }
    $ok = $false
    try {
        $ok = ($h.Proc.Id -eq $h.OwnPid) -and
              ($h.Proc.MainModule.FileName -eq $h.OwnExe)
    } catch { $ok = ($h.Proc.Id -eq $h.OwnPid) }
    if (-not $ok) {
        Write-Host ("  REFUSING to kill pid {0}: it is not the process this " +
                    "script started ({1}). Another session's game is not " +
                    "ours to close." -f $h.Proc.Id, $h.OwnPid)
        return
    }
    $h.Proc.Kill()
}

# THESE KILLS NOW REACH THE GAME. Run mg16 lane MP3, review fix 1: while this
# script launched through a cmd.exe shim, $h.Proc WAS THE SHIM, so Kill() ended
# cmd and left walk_window running -- orphaned, still holding its UDP port and
# its window, for about a minute until its own frame budget ran out. Launching
# the exe directly (see Start-Instance) makes the handle the game's, so the
# teardown below does what it has always said it does.
if ($connected -and $ExitWhenConnected) {
    Write-Host "MP2 TWO-WINDOW: join confirmed, closing both (-ExitWhenConnected)"
    foreach ($h in @($p1,$p2)) { Stop-Mine $h }
} elseif ($PlaySeconds -gt 0) {
    Write-Host ("MP2 TWO-WINDOW: play session, closing both after {0}s" -f $PlaySeconds)
    Start-Sleep -Seconds $PlaySeconds
    foreach ($h in @($p1,$p2)) { Stop-Mine $h }
} elseif ($Frames -gt 0) {
    foreach ($h in @($p1,$p2)) { $h.Proc.WaitForExit(900000) | Out-Null }
} elseif (-not $connected) {
    # Only tear down what we started, and only when the point of the run (the
    # join) did not happen. A play session is the human's to close.
    foreach ($h in @($p1,$p2)) { Stop-Mine $h }
}

foreach ($h in @($p1,$p2)) { Close-Log $h }

foreach ($h in @($p1,$p2)) {
    Write-Host ("  {0} exit={1} log={2}" -f $h.Tag,
        $(if ($h.Proc.HasExited) { $h.Proc.ExitCode } else { "running" }), $h.Log)
}

if (-not $connected) { Write-Host "MP2 TWO-WINDOW: NOT CONNECTED"; exit 1 }
exit 0
