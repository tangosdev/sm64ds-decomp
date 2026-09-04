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
# Raise, let the compositor actually paint it, then grab. The settle is not
# superstition: a window raised and photographed in the same tick is caught
# mid-present and the bottom band comes back as whatever was underneath.
[WsGrab]::Front($hwnd)
Start-Sleep -Milliseconds ([int]($SettleSec * 1000))
[WsGrab]::Front($hwnd)
Start-Sleep -Milliseconds 250
$geom = [WsGrab]::Grab($hwnd, $OutPng)
Write-Output "GRABBED $OutPng client=$geom"
if (-not $p.HasExited) { $p.Kill() | Out-Null }
