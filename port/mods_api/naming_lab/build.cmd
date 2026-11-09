@echo off
rem Builds naming_lab.dll (32-bit, VS 2022 Build Tools) and installs the mod into
rem build\port-trace\mods\naming-lab (the tracing build the lab runs on) and, if
rem it exists, build\port\mods\naming-lab (a plain build: actors and fields only).
setlocal
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 1
set "CMAKEBIN=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake"
set "PATH=%CMAKEBIN%\CMake\bin;%CMAKEBIN%\Ninja;%PATH%"
set "ROOT=%~dp0..\..\.."
cmake -S "%~dp0." -B "%ROOT%\build\naming_lab" -G Ninja -DCMAKE_BUILD_TYPE=Release >nul || exit /b 1
ninja -C "%ROOT%\build\naming_lab" || exit /b 1
for %%T in (port-trace port) do (
  if exist "%ROOT%\build\%%T\walk_window.exe" (
    if not exist "%ROOT%\build\%%T\mods\naming-lab\plugin" mkdir "%ROOT%\build\%%T\mods\naming-lab\plugin"
    copy /y "%~dp0mod.json" "%ROOT%\build\%%T\mods\naming-lab\" >nul
    copy /y "%ROOT%\build\naming_lab\naming_lab.dll" "%ROOT%\build\%%T\mods\naming-lab\plugin\" >nul || exit /b 1
    echo installed build\%%T\mods\naming-lab
  )
)
