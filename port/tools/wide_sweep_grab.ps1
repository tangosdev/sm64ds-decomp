<#
  Launch walk_window with whatever SM64DS_* environment the caller has already
  set, let it settle, and photograph its CLIENT AREA OFF THE REAL SCREEN.

  WHY CopyFromScreen AND NOT A FRAMEBUFFER DUMP. This tree has already paid for
  that lesson once: the black-bottom present bug wrote a full and correct
  framebuffer while the window on the glass was visibly half black, so a BMP
  written by the game cannot answer a question about what a player sees. Every
  image the widescreen sweep judges comes through here, at the window's own
  client-area screen coordinates, after the window has been raised. Do not
  cover the window while this runs.

  It is the capture half only. It decides nothing and asserts nothing; the
  judging is port/tools/wide_sweep.py, which reads the PNGs this writes.

  The caller owns the environment, which is what makes one script serve every
  state in the sweep: wide_sweep.py sets SM64DS_ASPECT, the state's own
  variables and the control knob, then invokes this once per image.

    powershell -File port/tools/wide_sweep_grab.ps1 -OutPng C:\tmp\x.png -WaitSec 12
#>
param(
  [Parameter(Mandatory=$true)][string]$OutPng,
  [double]$WaitSec = 12.0,
  [double]$SettleSec = 1.2,
  [double]$SettleMaxSec = 240.0,
  [string]$Exe = ""
)
$ErrorActionPreference = "Stop"
if (-not $Exe) {
  $repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
  $Exe  = Join-Path $repo "build\port\walk_window.exe"
}
if (-not (Test-Path $Exe)) { throw "walk_window.exe not found: $Exe" }
New-Item -ItemType Directory -Force -Path (Split-Path $OutPng) | Out-Null

# The windowed slot lock, so this queues behind any other windowed test on the
# machine instead of fighting it for the foreground -- which for a capture is
# not politeness but correctness: another window on top IS what CopyFromScreen
# would photograph.
if (-not $env:SM64DS_TEST_LOCK)      { $env:SM64DS_TEST_LOCK = "1" }
if (-not $env:SM64DS_TEST_LOCK_PATH) { $env:SM64DS_TEST_LOCK_PATH = "C:\tmp\sm64ds-test-slot\windowed_test.lock" }
if (-not $env:SM64DS_TEST_LOCK_TIMEOUT) { $env:SM64DS_TEST_LOCK_TIMEOUT = "5400" }
$env:SM64DS_VOLUME     = "0"
$env:SM64DS_NO_PLAYLOG = "1"
# MINIMIZED and NO_FOCUS are cleared rather than merely unset: this reads the
# glass, so the window has to be restored and on top. A minimised window
# photographs as whatever is behind it, which is a black image that looks like
# a render bug and is not one.
Remove-Item Env:SM64DS_MINIMIZED -ErrorAction SilentlyContinue
Remove-Item Env:SM64DS_NO_FOCUS  -ErrorAction SilentlyContinue

Add-Type -AssemblyName System.Drawing
Add-Type -ReferencedAssemblies ("System.Drawing", "System.Windows.Forms") -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;
public class WsGrab {
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern int  GetWindowText(IntPtr h, System.Text.StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int c);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr after, int x, int y, int cx, int cy, uint f);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  [StructLayout(LayoutKind.Sequential)] public struct RECT  { public int L,T,R,B; }
  [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X,Y; }
  public static IntPtr Found = IntPtr.Zero;
  public static uint TargetPid = 0;
  public static bool Cb(IntPtr h, IntPtr p) {
    uint pid; GetWindowThreadProcessId(h, out pid);
    if (pid == TargetPid && IsWindowVisible(h)) {
      var sb = new System.Text.StringBuilder(256); GetWindowText(h, sb, 256);
      if (sb.ToString().StartsWith("SM64DS")) { Found = h; return false; }
    }
    return true;
  }
  public static IntPtr Find(uint pid) { Found = IntPtr.Zero; TargetPid = pid; EnumWindows(Cb, IntPtr.Zero); return Found; }
  // SWP_NOSIZE|SWP_NOZORDER = 0x0001|0x0004. The corner is PINNED on every
  // raise, not merely requested at launch: CopyFromScreen reads the desktop
  // at the client rectangle, so a window the OS placed off the desktop (a
  // first pass caught one at y=-1117) photographs as nothing at all, and two
  // launches that land at two positions produce two images of two different
  // pieces of glass, which cannot be compared with each other. The game has
  // its own SM64DS_WINDOW_POS and it did NOT hold across every launch here,
  // so this pins it from the outside where nothing can override it.
  public static void Front(IntPtr h) {
    ShowWindow(h, 9); SetWindowPos(h, IntPtr.Zero, 8, 31, 0, 0, 0x0001|0x0004);
    BringWindowToTop(h); SetForegroundWindow(h);
  }
  // Capture once and return BOTH a content hash and (optionally) the file.
  // The hash is what lets the caller wait for the picture to STOP CHANGING
  // rather than guess how long that takes -- see the settle loop below.
  public static string LastGeom = "";
  public static long Shot(IntPtr h, string path) {
    RECT c; GetClientRect(h, out c);
    int w = c.R - c.L, ht = c.B - c.T;
    if (w <= 0 || ht <= 0) return -1;
    POINT o; o.X = 0; o.Y = 0; ClientToScreen(h, ref o);
    long sum = 1469598103934665603L;
    using (var bmp = new Bitmap(w, ht, PixelFormat.Format32bppArgb)) {
      using (var g = Graphics.FromImage(bmp))
        g.CopyFromScreen(o.X, o.Y, 0, 0, new Size(w, ht), CopyPixelOperation.SourceCopy);
      var bd = bmp.LockBits(new Rectangle(0,0,w,ht), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
      int n = Math.Abs(bd.Stride) * ht;
      byte[] buf = new byte[n];
      System.Runtime.InteropServices.Marshal.Copy(bd.Scan0, buf, 0, n);
      bmp.UnlockBits(bd);
      for (int i = 0; i < n; i++) { sum ^= buf[i]; sum *= 1099511628211L; }
      if (!string.IsNullOrEmpty(path)) bmp.Save(path, ImageFormat.Png);
    }
    LastGeom = string.Format("{0}x{1} @ {2},{3}", w, ht, o.X, o.Y);
    return sum;
  }
  public static string Grab(IntPtr h, string path) {
    RECT c; GetClientRect(h, out c);
    int w = c.R - c.L, ht = c.B - c.T;
    if (w <= 0 || ht <= 0) return "ERR no client area";
    POINT o; o.X = 0; o.Y = 0; ClientToScreen(h, ref o);
    using (var bmp = new Bitmap(w, ht, PixelFormat.Format32bppArgb)) {
      using (var g = Graphics.FromImage(bmp))
        g.CopyFromScreen(o.X, o.Y, 0, 0, new Size(w, ht), CopyPixelOperation.SourceCopy);
      bmp.Save(path, ImageFormat.Png);
    }
    return string.Format("{0}x{1} @ {2},{3}", w, ht, o.X, o.Y);
  }
}
'@

$p = Start-Process -FilePath $Exe -WorkingDirectory (Split-Path $Exe) -PassThru
$hwnd = [IntPtr]::Zero
$deadline = (Get-Date).AddSeconds($WaitSec)
while ((Get-Date) -lt $deadline) {
  if ($p.HasExited) { break }
  if ($hwnd -eq [IntPtr]::Zero) { $hwnd = [WsGrab]::Find([uint32]$p.Id) }
  Start-Sleep -Milliseconds 200
}
if ($hwnd -eq [IntPtr]::Zero) {
  if (-not $p.HasExited) { $p.Kill() }
  throw "no SM64DS window appeared within $WaitSec s (process exited=$($p.HasExited))"
}
# WAIT FOR THE PICTURE TO STOP CHANGING, then photograph it.
#
# The sweep isolates the HUD by subtracting one capture from another, which
# is only meaningful between two captures of the SAME frame. That needs the
# game frozen -- SM64DS_MENU_AT / SM64DS_SCENE_MENU stop the tick at a named
# frame -- and it needs the capture to happen AFTER the freeze has been
# reached, not merely after a hopeful number of seconds.
#
# A FIXED DWELL IS NOT GOOD ENOUGH, measured. With a visible window the game
# presents every frame to the glass and runs far slower than the minimised
# runs the frame-cost numbers came from, so a dwell tuned on one aspect
# misses the freeze on another. The VS time-up state (frame 2900) and the
# 21:9 course HUD both came back with two launches differing in 17 to 94
# percent of the frame: not noise, just two different live frames.
#
# So this polls instead: grab, wait, grab again, and accept only when two
# consecutive captures a second apart are IDENTICAL. That is the same
# condition the checker later re-verifies across two separate launches, so
# the capture cannot hand it a frame it will refuse. If the picture never
# settles the script says so and still writes the last frame, because a
# state that genuinely cannot be frozen is a result worth seeing.
[WsGrab]::Front($hwnd)
Start-Sleep -Milliseconds ([int]($SettleSec * 1000))
$settleDeadline = (Get-Date).AddSeconds($SettleMaxSec)
# THREE IN A ROW, NOT TWO, and the third one is not belt-and-braces.
# A LIVE scene can hold still for a moment -- an idle player, a camera
# between moves, a menu mid-fade -- so two consecutive identical frames
# can happen by coincidence well before the freeze frame is reached. Accept
# on two and the run settles at an arbitrary live frame; two LAUNCHES then
# settle at two different arbitrary frames and the checker correctly throws
# the pair out as DYNAMIC. Measured exactly that way: states that were
# reliably still under a fixed dwell started reporting dynamic once a
# two-frame settle was introduced. A genuinely frozen picture repeats
# forever, so demanding three costs it two extra seconds and costs a
# coincidence almost everything.
$prev = [long]0; $have = $false; $settled = $false; $polls = 0; $same = 0
while ((Get-Date) -lt $settleDeadline) {
  if ($p.HasExited) { break }
  [WsGrab]::Front($hwnd)
  $h1 = [WsGrab]::Shot($hwnd, $null)
  $polls++
  if ($have -and $h1 -eq $prev) { $same++ } else { $same = 0 }
  if ($same -ge 2) { $settled = $true; break }
  $prev = $h1; $have = $true
  Start-Sleep -Milliseconds 1000
}
[WsGrab]::Front($hwnd)
Start-Sleep -Milliseconds 200
[void][WsGrab]::Shot($hwnd, $OutPng)
$geom = [WsGrab]::LastGeom
$tag = if ($settled) { "settled" } else { "NEVER-SETTLED" }
Write-Output "GRABBED $OutPng client=$geom $tag after $polls polls"
if (-not $p.HasExited) { $p.Kill() | Out-Null }
