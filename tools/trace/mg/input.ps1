param(
  [Parameter(Mandatory=$true)][string]$Action,   # key | tap | drag | size
  [string]$Button = "",                         # A B X Y L R START SELECT UP DOWN LEFT RIGHT
  [int]$X = 0, [int]$Y = 0,                     # DS bottom-screen coordinates (0..255, 0..191)
  [int]$X2 = 0, [int]$Y2 = 0,
  [int]$Ms = 120
)
# Drive melonDS 1.1: keys (bound as in tools/trace/mg/README.md) and bottom-screen mouse touches.
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class In {
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }
  [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int c);
  [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr h);
  [DllImport("user32.dll")] public static extern bool MoveWindow(IntPtr h, int x, int y, int w, int ht, bool r);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f, uint x, uint y, uint d, IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint f, IntPtr e);
  [DllImport("user32.dll")] public static extern uint MapVirtualKey(uint c, uint t);
}
"@
[In]::SetProcessDPIAware() | Out-Null
$p = Get-Process melonDS -ErrorAction SilentlyContinue | Where-Object { $_.MainWindowHandle -ne 0 } | Select-Object -First 1
if (-not $p) { Write-Error "melonDS not running"; exit 1 }
$h = $p.MainWindowHandle
# Unlock SetForegroundWindow: any synthetic key counts as recent input. F24, not
# Alt: an Alt tap activates the menu bar, and the next key then drives the menu.
[In]::keybd_event(0x87, 0, 0, [IntPtr]::Zero); [In]::keybd_event(0x87, 0, 2, [IntPtr]::Zero)
if ([In]::IsIconic($h)) { [In]::ShowWindow($h, 9) | Out-Null }
[In]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 60

# Keys as bound in README.md: A=X B=Z X=S Y=A L=Q R=W Start=Enter Select=Backspace, D-pad = arrows.
$vk = @{ A=0x58; B=0x5A; X=0x53; Y=0x41; L=0x51; R=0x57; START=0x0D; SELECT=0x08;
         UP=0x26; DOWN=0x28; LEFT=0x25; RIGHT=0x27 }
$ext = @{ UP=1; DOWN=1; LEFT=1; RIGHT=1 }

function Press($b, $ms) {
  $k = [byte]$vk[$b.ToUpper()]
  $s = [byte][In]::MapVirtualKey($k, 0)
  $f = 0; if ($ext[$b.ToUpper()]) { $f = 1 }
  [In]::keybd_event($k, $s, $f, [IntPtr]::Zero)
  Start-Sleep -Milliseconds $ms
  [In]::keybd_event($k, $s, ($f -bor 2), [IntPtr]::Zero)
}

# Bottom-screen mapping, measured from window captures: the DS screens span the
# middle of the client area, two 256x192 screens stacked.
function ToWin($dx, $dy) {
  $r = New-Object In+RECT; [In]::GetWindowRect($h, [ref]$r) | Out-Null
  $w = $r.R - $r.L; $ht = $r.B - $r.T
  # client area below title+menu bar (83 px at this size), screens fill its height
  $top = $r.T + 83; $areaH = ($r.B - 10) - $top
  $scale = $areaH / 384.0
  $left = $r.L + ($w - 256 * $scale) / 2.0
  return @([int]($left + $dx * $scale), [int]($top + 192 * $scale + $dy * $scale))
}

switch ($Action) {
  "size" { $r = New-Object In+RECT; [In]::GetWindowRect($h, [ref]$r) | Out-Null; [In]::MoveWindow($h, $r.L, 0, 900, 1180, $true) | Out-Null }
  "key"  { Press $Button $Ms }
  "tap"  { $c = ToWin $X $Y; [In]::SetCursorPos($c[0], $c[1]) | Out-Null; Start-Sleep -Milliseconds 40
           [In]::mouse_event(2, 0, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds $Ms; [In]::mouse_event(4, 0, 0, 0, [IntPtr]::Zero) }
  "drag" { $a = ToWin $X $Y; $b = ToWin $X2 $Y2; [In]::SetCursorPos($a[0], $a[1]) | Out-Null
           [In]::mouse_event(2, 0, 0, 0, [IntPtr]::Zero)
           for ($i = 1; $i -le 10; $i++) { [In]::SetCursorPos([int]($a[0] + ($b[0]-$a[0])*$i/10), [int]($a[1] + ($b[1]-$a[1])*$i/10)) | Out-Null; Start-Sleep -Milliseconds ([int]($Ms/10)) }
           [In]::mouse_event(4, 0, 0, 0, [IntPtr]::Zero) }
  default { Write-Error "unknown action $Action"; exit 1 }
}
