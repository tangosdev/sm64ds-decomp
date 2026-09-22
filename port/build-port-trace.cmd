@echo off
rem TRACING BUILD ONLY (lane TRACEPORT). Configures a SEPARATE build directory
rem build/port-trace with -DPORT_FN_TRACE=ON so the shipped build/port tree is
rem never poisoned by the cache variable, and builds walk_window alone. The guard
rem wave is deliberately NOT run here: the guards belong to the plain build, and
rem tailjump_guard in particular cannot read a /Gh build (every frame opens with
rem an E8 to _penter). Extra -D arguments are passed straight through, which is
rem how the pre-fix arm is selected: build-port-trace.cmd -DTITLEQUAD_GX_PLAIN=ON
setlocal
set "PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer;%PATH%"
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
if errorlevel 1 exit /b 1
set "CMAKEBIN=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake"
set "PATH=%CMAKEBIN%\CMake\bin;%CMAKEBIN%\Ninja;%PATH%"
cmake -S "%~dp0." -B "%~dp0..\build\port-trace" -G Ninja -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_MAKE_PROGRAM="%CMAKEBIN%\Ninja\ninja.exe" -DPORT_FN_TRACE=ON %*
if errorlevel 1 exit /b 1
ninja -j10 -C "%~dp0..\build\port-trace" walk_window
if errorlevel 1 exit /b 1
echo TRACE_BUILD_OK
