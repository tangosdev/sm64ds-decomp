@echo off
rem Build the PC port's gate-1 smoke runner: 32-bit MSVC via VS Build Tools,
rem same toolchain-location pattern as the recomp's build scripts.
setlocal
set "PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer;%PATH%"
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
if errorlevel 1 exit /b 1
set "CMAKEBIN=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake"
set "PATH=%CMAKEBIN%\CMake\bin;%CMAKEBIN%\Ninja;%PATH%"
rem Fail before configure if a slice gate activates a receiver dropping raw reader.
python "%~dp0tools\closestplayer_guard.py"
if errorlevel 1 exit /b 1
rem Fail before configure if a NEW guessed vtable body got seated past the baseline.
python "%~dp0tools\inferred_stub_guard.py"
if errorlevel 1 exit /b 1
cmake -S "%~dp0." -B "%~dp0..\build\port" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="%CMAKEBIN%\Ninja\ninja.exe" %*
if errorlevel 1 exit /b 1
ninja -C "%~dp0..\build\port"
