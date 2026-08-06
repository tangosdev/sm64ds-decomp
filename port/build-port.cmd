@echo off
rem Build the PC port's gate smoke runners: 32-bit MSVC via CMake + Ninja.
rem
rem The toolchain is LOCATED, not hardcoded. This script used to name
rem "...\2022\BuildTools\..." directly; when the machine moved to VS 18 the
rem path stopped existing and the failure surfaced as a confusing error from
rem cmake rather than "your compiler moved". vswhere is the supported way to
rem ask where the toolset actually is.
setlocal

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo error: vswhere.exe not found at "%VSWHERE%"
    echo        install Visual Studio 2019+ or the VS Build Tools.
    exit /b 1
)

rem -products * so Build Tools installs count, not just the IDE SKUs.
set "VSINSTALL="
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
if not defined VSINSTALL (
    echo error: no Visual Studio install carries the MSVC x86/x64 toolset.
    echo        install the "Desktop development with C++" workload.
    exit /b 1
)

set "VCVARS=%VSINSTALL%\VC\Auxiliary\Build\vcvars32.bat"
if not exist "%VCVARS%" (
    echo error: vcvars32.bat missing under "%VSINSTALL%"
    exit /b 1
)
rem vcvars32.bat itself shells out to a BARE `vswhere` to resolve the Windows
rem SDK, so the Installer directory has to be on PATH before the call -- drop
rem this and vcvars prints "'vswhere.exe' is not recognized" and then leaves
rem the environment half-built, which surfaces much later as a missing cl.
for %%d in ("%VSWHERE%") do set "PATH=%%~dpd;%PATH%"
call "%VCVARS%" >nul
if errorlevel 1 exit /b 1

rem CMake and Ninja: prefer the copies bundled with the "C++ CMake tools for
rem Windows" component, then anything vcvars or the user put on PATH.
set "VSCMAKE=%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake"
set "CMAKE_EXE="
set "NINJA_EXE="
if exist "%VSCMAKE%\CMake\bin\cmake.exe" set "CMAKE_EXE=%VSCMAKE%\CMake\bin\cmake.exe"
if exist "%VSCMAKE%\Ninja\ninja.exe" set "NINJA_EXE=%VSCMAKE%\Ninja\ninja.exe"
if not defined CMAKE_EXE call :locate cmake CMAKE_EXE
if not defined NINJA_EXE call :locate ninja NINJA_EXE

rem Last resort: `pip install cmake ninja` is what the error below tells you
rem to do, and it drops both into Python's Scripts directory -- which is NOT
rem on PATH in a default Windows Python install, so `where` misses them.
rem Take our own advice seriously enough to find the result.
if not defined CMAKE_EXE goto :pyprobe
if not defined NINJA_EXE goto :pyprobe
goto :haveboth
:pyprobe
set "PYSCRIPTS="
for /f "usebackq delims=" %%i in (`python -c "import sysconfig;print(sysconfig.get_path('scripts'))" 2^>nul`) do set "PYSCRIPTS=%%i"
if defined PYSCRIPTS (
    if not defined CMAKE_EXE if exist "%PYSCRIPTS%\cmake.exe" set "CMAKE_EXE=%PYSCRIPTS%\cmake.exe"
    if not defined NINJA_EXE if exist "%PYSCRIPTS%\ninja.exe" set "NINJA_EXE=%PYSCRIPTS%\ninja.exe"
)
:haveboth

if not defined CMAKE_EXE (
    echo error: cmake.exe not found.
    echo        add the VS component "C++ CMake tools for Windows",
    echo        or: pip install cmake ninja
    exit /b 1
)
if not defined NINJA_EXE (
    echo error: ninja.exe not found.
    echo        add the VS component "C++ CMake tools for Windows",
    echo        or: pip install cmake ninja
    exit /b 1
)

echo toolchain: %VSINSTALL%
echo cmake:     %CMAKE_EXE%
echo ninja:     %NINJA_EXE%

"%CMAKE_EXE%" -S "%~dp0." -B "%~dp0..\build\port" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="%NINJA_EXE%" %*
if errorlevel 1 exit /b 1
"%NINJA_EXE%" -C "%~dp0..\build\port"
exit /b %errorlevel%

rem :locate <exe-name> <var-to-set> -- first PATH hit, or leaves the var unset.
:locate
for /f "usebackq delims=" %%i in (`where %1 2^>nul`) do (
    set "%2=%%i"
    goto :eof
)
goto :eof
