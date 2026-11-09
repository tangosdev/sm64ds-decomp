@echo off
rem Naming Lab: the traced port (build\port-trace) straight into a level, with the
rem naming-lab mod on, and the lab server (dashboard on http://127.0.0.1:47620/).
rem   naming-lab.cmd          Bob-omb Battlefield (level 6)
rem   naming-lab.cmd 1        any level id (1 = castle grounds)
rem   naming-lab.cmd 6 noserver
rem See port\mods_api\naming_lab\README.md.
setlocal
set "SM64DS_LEVEL=%~1"
if "%SM64DS_LEVEL%"=="" set "SM64DS_LEVEL=6"
rem A level opens with its star-name dialog, and while it is up no actor runs its
rem Behavior. Press A three times shortly after boot to close it (mask@frame, XInput
rem A = 0x1000). Set SM64DS_HOST_PAD yourself to override it.
if not defined SM64DS_HOST_PAD set "SM64DS_HOST_PAD=1000@300,1000@360,1000@420"
cd /d "%~dp0"
start "" "%~dp0build\port-trace\walk_window.exe"
if /i "%~2"=="noserver" goto :eof
start "naming lab server" python "%~dp0port\mods_api\naming_lab\lab_server.py"
echo The dashboard address (it carries this run's access token) is written to
echo   %USERPROFILE%\sm64ds-lab\naming-lab\dashboard-url.txt
echo and printed in the "naming lab server" window.
