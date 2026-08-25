@echo off
rem Build every PC-port smoke runner with 32-bit MSVC. Discover Visual Studio
rem through its supported installer API so Community and Build Tools installs,
rem including newer releases, work without editing this file.
setlocal

if not exist "%~dp0..\extracted\arm9_dec.bin" (
    echo ERROR: extracted\arm9_dec.bin is missing. Run the repository ROM setup first. 1>&2
    exit /b 1
)
if not exist "%~dp0..\build\assets\files.tsv" (
    echo ERROR: the local asset catalog is missing. 1>&2
    echo Run: python tools\asset_catalog.py generate sm64.nds 1>&2
    exit /b 1
)
if not exist "%~dp0..\build\assets\handles.tsv" (
    echo ERROR: the local asset handle catalog is missing. 1>&2
    echo Run: python tools\asset_catalog.py generate sm64.nds 1>&2
    exit /b 1
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo ERROR: Visual Studio Installer's vswhere.exe was not found. 1>&2
    exit /b 1
)

set "VSINSTALL="
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
if not defined VSINSTALL (
    echo ERROR: no Visual Studio installation with the x86 C++ tools was found. 1>&2
    exit /b 1
)

call "%VSINSTALL%\VC\Auxiliary\Build\vcvars32.bat" >nul
if errorlevel 1 exit /b 1

set "PYTHON_EXE="
for /f "delims=" %%I in ('py -3 -c "import sys; print(sys.executable)" 2^>nul') do if not defined PYTHON_EXE set "PYTHON_EXE=%%I"
if not defined PYTHON_EXE for /d %%I in ("%LocalAppData%\Programs\Python\Python*") do if exist "%%~fI\python.exe" set "PYTHON_EXE=%%~fI\python.exe"
if not defined PYTHON_EXE for /f "delims=" %%I in ('where python.exe 2^>nul') do if not defined PYTHON_EXE set "PYTHON_EXE=%%I"
if not defined PYTHON_EXE (
    echo ERROR: Python 3 was not found on PATH or through py.exe. 1>&2
    exit /b 1
)
"%PYTHON_EXE%" -c "import sys; raise SystemExit(0 if sys.version_info.major == 3 else 1)" >nul 2>&1
if errorlevel 1 (
    echo ERROR: the discovered python.exe is not a working Python 3 installation. 1>&2
    exit /b 1
)
for %%I in ("%PYTHON_EXE%") do set "PYTHON_DIR=%%~dpI"
set "PATH=%PYTHON_DIR%;%PYTHON_DIR%Scripts;%PATH%"

set "CMAKE_EXE="
for /f "delims=" %%I in ('where cmake.exe 2^>nul') do if not defined CMAKE_EXE set "CMAKE_EXE=%%I"
if not defined CMAKE_EXE for /d %%I in ("%LocalAppData%\Programs\Python\Python*") do if exist "%%~fI\Scripts\cmake.exe" set "CMAKE_EXE=%%~fI\Scripts\cmake.exe"
if not defined CMAKE_EXE if exist "%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" set "CMAKE_EXE=%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if not defined CMAKE_EXE (
    echo ERROR: cmake.exe was not found on PATH or in Visual Studio. 1>&2
    exit /b 1
)
for %%I in ("%CMAKE_EXE%") do set "CTEST_EXE=%%~dpIctest.exe"
if not exist "%CTEST_EXE%" (
    echo ERROR: ctest.exe was not found next to cmake.exe. 1>&2
    exit /b 1
)

set "NINJA_EXE="
for /f "delims=" %%I in ('where ninja.exe 2^>nul') do if not defined NINJA_EXE set "NINJA_EXE=%%I"
if not defined NINJA_EXE for /d %%I in ("%LocalAppData%\Programs\Python\Python*") do if exist "%%~fI\Scripts\ninja.exe" set "NINJA_EXE=%%~fI\Scripts\ninja.exe"
if not defined NINJA_EXE if exist "%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe" set "NINJA_EXE=%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
if not defined NINJA_EXE (
    echo ERROR: ninja.exe was not found on PATH or in Visual Studio. 1>&2
    exit /b 1
)

"%CMAKE_EXE%" -S "%~dp0." -B "%~dp0..\build\port" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="%NINJA_EXE%" %* -DBUILD_TESTING=ON
if errorlevel 1 exit /b 1
"%CMAKE_EXE%" --build "%~dp0..\build\port"
if errorlevel 1 exit /b 1
pushd "%~dp0..\build\port"
if errorlevel 1 exit /b 1
"%CTEST_EXE%" --output-on-failure
set "CTEST_RESULT=%ERRORLEVEL%"
popd
exit /b %CTEST_RESULT%
