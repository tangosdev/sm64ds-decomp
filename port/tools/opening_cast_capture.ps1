<#
  Capture walk_window's CLIENT AREA off the REAL SCREEN during the new-file
  opening, for port/tools/opening_cast_proof.py.

  WHY CopyFromScreen. A framebuffer dump cannot see a present-path defect --
  the black-bottom bug wrote a full, correct framebuffer while the window on
  screen was visibly half black. CopyFromScreen reads the composited desktop
  at the window's own client-area screen coordinates, so anything that goes
  wrong between the framebuffer and the glass still shows up here. The window
  is raised before every grab; do not cover it while this runs.

  The touch probe drives the file-select menu into a NEW file, which is the
  only path that plays the opening with the cutscene cast.

    powershell -File port/tools/opening_cast_capture.ps1 -Tag after
    python port/tools/opening_cast_proof.py <OutRoot>/after
#>
param(
  [string]$Tag = "after",
  [int]$DurationSec = 80,
  [double]$IntervalSec = 1.0,
  [string]$Exe = "",
  [string]$OutRoot = "$env:TEMP\opening_cast_shots"
)
$ErrorActionPreference = "Stop"
if (-not $Exe) {
  $repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
  $Exe  = Join-Path $repo "build\port\walk_window.exe"
}
if (-not (Test-Path $Exe)) { throw "walk_window.exe not found: $Exe" }

$outdir = Join-Path $OutRoot $Tag
New-Item -ItemType Directory -Force -Path $outdir | Out-Null
Get-ChildItem -Path $outdir -Filter *.png -ErrorAction SilentlyContinue | Remove-Item -Force

# The windowed slot lock: this launches walk_window, so it queues like any
# other windowed test instead of fighting whatever else is on the machine.
if (-not $env:SM64DS_TEST_LOCK) { $env:SM64DS_TEST_LOCK = "1" }
if (-not $env:SM64DS_TEST_LOCK_PATH) {
  $env:SM64DS_TEST_LOCK_PATH = "C:\tmp\sm64ds-test-slot\windowed_test.lock"
}
if (-not $env:SM64DS_TEST_LOCK_TIMEOUT) { $env:SM64DS_TEST_LOCK_TIMEOUT = "5400" }
$env:SM64DS_SKIP_MENU     = "1"
$env:SM64DS_SCENE_WINDOW  = "1"
$env:SM64DS_SCENE_FRAMES  = "100000"
$env:SM64DS_TOUCH_PROBE   = "560-563:44:68,620-623:44:68,680-683:44:68,760-763:44:68,860-863:44:68"
$env:SM64DS_VOLUME        = "0"
$env:SM64DS_NO_PLAYLOG    = "1"
# NO_FOCUS is deliberately cleared: CopyFromScreen reads the glass, so the
# window has to actually be on top.
Remove-Item Env:SM64DS_NO_FOCUS -ErrorAction SilentlyContinue

Add-Type -AssemblyName System.Drawing
Add-Type -ReferencedAssemblies ("System.Drawing", "System.Windows.Forms") -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;
public class OcScrCap {
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern int  GetWindowText(IntPtr h, System.Text.StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int c);
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
  public static void Front(IntPtr h) { ShowWindow(h, 5); BringWindowToTop(h); SetForegroundWindow(h); }
  public static string Grab(IntPtr h, string path) {
    RECT c; GetClientRect(h, out c);
    int w = c.R - c.L, ht = c.B - c.T;
    if (w <= 0 || ht <= 0) return "no client area";
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
Write-Host "launched pid $($p.Id): $Exe"
$hwnd = [IntPtr]::Zero
$deadline = (Get-Date).AddSeconds($DurationSec)
$i = 0; $geom = ""
while ((Get-Date) -lt $deadline) {
  if ($p.HasExited) { Write-Host "process exited early (rc=$($p.ExitCode))"; break }
  if ($hwnd -eq [IntPtr]::Zero) {
    $hwnd = [OcScrCap]::Find([uint32]$p.Id)
    if ($hwnd -ne [IntPtr]::Zero) { [OcScrCap]::Front($hwnd); Start-Sleep -Milliseconds 400 }
  }
  if ($hwnd -ne [IntPtr]::Zero) {
    [OcScrCap]::Front($hwnd)
    $secs = [int]((Get-Date) - $p.StartTime).TotalSeconds
    try { $geom = [OcScrCap]::Grab($hwnd, (Join-Path $outdir ("scr_{0:000}_t{1:000}s.png" -f $i, $secs))) }
    catch { Write-Host "grab error: $_" }
    $i++
  }
  Start-Sleep -Milliseconds ([int]($IntervalSec * 1000))
}
Write-Host "captured $i frames to $outdir  client=$geom"
if (-not $p.HasExited) { $p.Kill(); Write-Host "killed pid $($p.Id)" }
