# Walk melonDS's Qt menus through UI Automation. Needs no keyboard focus.
# usage: uia_menu.ps1 -ProcessId <pid> -Path "File|Load state|1" [-List]
# Each step expands (or invokes) the first VISIBLE, enabled menu item with that
# exact name, searching the popup opened by the previous step first.
param([int]$ProcessId, [string]$Path, [switch]$List)
Add-Type -AssemblyName UIAutomationClient
Add-Type -AssemblyName UIAutomationTypes
$A = [System.Windows.Automation.AutomationElement]
$TS = [System.Windows.Automation.TreeScope]
$desk = $A::RootElement
$pidCond = New-Object System.Windows.Automation.PropertyCondition($A::ProcessIdProperty, $ProcessId)
function Tops() { $desk.FindAll($TS::Children, $pidCond) }
function Items() {
  $out = @()
  foreach ($t in (Tops)) {
    foreach ($e in $t.FindAll($TS::Descendants, [System.Windows.Automation.Condition]::TrueCondition)) {
      if ($e.Current.ControlType.ProgrammaticName -eq 'ControlType.MenuItem') { $out += ,@($t, $e) }
    }
  }
  return $out
}
function FindItem($name, $prevTop) {
  $hit = $null
  foreach ($pair in (Items)) {
    $t = $pair[0]; $e = $pair[1]
    if ($e.Current.Name -ne $name -or -not $e.Current.IsEnabled -or $e.Current.IsOffscreen) { continue }
    if ($prevTop -ne $null -and $t -eq $prevTop) { continue }  # prefer a popup other than the last one
    $hit = @($t, $e)
  }
  if ($hit -eq $null -and $prevTop -ne $null) {
    foreach ($pair in (Items)) {
      $e = $pair[1]
      if ($e.Current.Name -eq $name -and $e.Current.IsEnabled -and -not $e.Current.IsOffscreen) { $hit = $pair }
    }
  }
  return $hit
}
function Dump() { foreach ($pair in (Items)) { $e = $pair[1]; "  " + $e.Current.Name + " | enabled=" + $e.Current.IsEnabled + " offscreen=" + $e.Current.IsOffscreen } }
function Open($e) {
  try { $e.GetCurrentPattern([System.Windows.Automation.ExpandCollapsePattern]::Pattern).Expand(); return "expand" } catch {}
  try { $e.GetCurrentPattern([System.Windows.Automation.InvokePattern]::Pattern).Invoke(); return "invoke" } catch {}
  return "none"
}
$prev = $null
foreach ($n in ($Path -split '\|')) {
  $pair = $null
  for ($i = 0; $i -lt 10 -and $pair -eq $null; $i++) {
    $pair = FindItem $n $prev
    if ($pair -eq $null) { Start-Sleep -Milliseconds 200 }
  }
  if ($pair -eq $null) { "missing: $n"; Dump; exit 1 }
  "$n -> " + (Open $pair[1])
  $prev = $pair[0]
  Start-Sleep -Milliseconds 300
}
if ($List) { Dump }
