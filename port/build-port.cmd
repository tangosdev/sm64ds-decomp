@echo off
rem Build the PC port's gate-1 smoke runner: 32-bit MSVC via VS Build Tools,
rem same toolchain-location pattern as the recomp's build scripts.
setlocal
set "PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer;%PATH%"
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul
if errorlevel 1 exit /b 1
set "CMAKEBIN=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake"
set "PATH=%CMAKEBIN%\CMake\bin;%CMAKEBIN%\Ninja;%PATH%"
rem THE GUARD WAVE. Every check below still runs, in this order, with its own
rem output and its own exit code; tools/guardcache.py just runs them ONCE PER
rem CHANGE instead of once per build. This line runs the whole set in parallel
rem and remembers each GREEN verdict against a key made of the guard's own
rem code, every other file in port/tools, the python and MSVC identity, and a
rem stat signature of every tree the guard reads. If nothing a guard reads has
rem moved, nothing re-runs. A RED verdict is never remembered: a refusing guard
rem re-runs on every build until the tree is fixed. Each guard's own line below
rem then reports what the wave found, at the same point in the script, so a
rem failure reads exactly as it always did. Measured on this tree the block was
rem about fourteen minutes of every build, on every lane, every time.
rem Lose the cache entirely (delete build/port/guardcache) and the only cost is
rem time: --replay runs the guard for real when the wave has no answer for it.
python "%~dp0tools\guardcache.py" --prime
if errorlevel 1 exit /b 1
rem Fail before configure if a slice gate activates a receiver dropping raw reader.
python "%~dp0tools\guardcache.py" --replay closestplayer_guard.py
if errorlevel 1 exit /b 1
rem Fail before configure if a NEW guessed vtable body got seated past the baseline.
python "%~dp0tools\guardcache.py" --replay inferred_stub_guard.py
if errorlevel 1 exit /b 1
rem Fail before configure if the closure prober's selftest breaks: the probe
rem sizes slice walls and predicts collisions, and a broken prober lies
rem quietly. There is no port CI; this block is where loudness lives.
python "%~dp0tools\guardcache.py" --replay closure.py --selftest
if errorlevel 1 exit /b 1
rem Fail before configure if facegen's selftest breaks: generated faces get
rem wired by slices, and a generator that stops refusing the judgment rows
rem is a silent hazard, not a convenience.
python "%~dp0tools\guardcache.py" --replay facegen.py --selftest
if errorlevel 1 exit /b 1
rem Fail before configure if mapdiff's selftest breaks: reviews and delta-0
rem claims read their decomposition off it, and a differ that miscounts or
rem stops refusing a truncated map turns a review into an eyeball again.
python "%~dp0tools\guardcache.py" --replay mapdiff.py --selftest
if errorlevel 1 exit /b 1
rem Fail before configure if vtablerows' selftest breaks: the minigame
rem fan-out lanes read their override/marker/nosrc census off it, and a
rem reader that miscounts a marker row skips a ROM adjudication silently.
python "%~dp0tools\guardcache.py" --replay vtablerows.py --selftest
if errorlevel 1 exit /b 1
rem And its reconstruction against mg_fanout_costs section 3: the selftest
rem runs on fixtures and cannot see the real-tree wiring (paths, the base
rem table constant, the symbol tables); the 29/29 reconstruction is the net
rem for exactly that half. Needs extracted/overlays, which every port tree
rem needs anyway (the binaries abort without the NitroFS emissions).
rem Quiet on the green path (35 lines per build otherwise); on failure the
rem rerun prints the DIVERGE lines, so the refusal stays loud.
python "%~dp0tools\guardcache.py" --replay vtablerows.py --reconstruct >nul
if errorlevel 1 (
    python "%~dp0tools\vtablerows.py" --reconstruct
    exit /b 1
)
rem Fail before configure if stategen's selftest breaks: it generates the
rem address switch a pointer-to-member state machine is dispatched through,
rem and a generator that drops a slot emits a switch that is short by one
rem state and looks complete. Every parse shape and every refusal is pinned
rem on fixtures here.
python "%~dp0tools\guardcache.py" --replay stategen.py --selftest
if errorlevel 1 exit /b 1
rem And its reconstruction of the two hand artifacts: the selftest runs on
rem fixtures and cannot see the real-tree wiring (the extracted/config paths,
rem the mount lists, the precedent file locations). Reproducing the 25-case
rem curling switch and all 197 player rows is the net for that half. Quiet on
rem the green path; on failure the rerun prints the DIVERGE lines.
python "%~dp0tools\guardcache.py" --replay stategen.py --reconstruct >nul
if errorlevel 1 (
    python "%~dp0tools\stategen.py" --reconstruct
    exit /b 1
)
rem Fail before configure if the alternatename guard's scoping fixture breaks.
rem The guard decides what counts as a linker input, and it used to read lane
rem prose as one: a quoted directive in a .txt was a build input, so deleting a
rem real alias left the quote of it failing the build. The fixture pins that
rem scope. The guard's own map check still runs post-link, below.
python "%~dp0tools\guardcache.py" --replay alternatename_guard.py --selftest
if errorlevel 1 exit /b 1
rem Fail before configure if the band guard's fixture battery breaks. Each arm
rem has a break only that arm catches, including the two the tree has actually
rem shipped (a GX band member split back out of its grouped section, and a
rem hosted global sized by its first caller). Its map check runs post-link,
rem below.
python "%~dp0tools\guardcache.py" --replay gxband_guard.py --selftest
if errorlevel 1 exit /b 1
rem Fail before configure if the tail-jump guard's fixture battery breaks. Both
rem directions are pinned there -- a forwarder that must jump failing when it
rem calls, and a classified seam that must call failing when it jumps -- plus
rem the displacement retirement that lets a seated row leave the set without a
rem hand edit. Its map check runs post-link, below.
python "%~dp0tools\guardcache.py" --replay tailjump_guard.py --selftest
if errorlevel 1 exit /b 1
cmake -S "%~dp0." -B "%~dp0..\build\port" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="%CMAKEBIN%\Ninja\ninja.exe" %*
if errorlevel 1 exit /b 1
ninja -C "%~dp0..\build\port"
if errorlevel 1 exit /b 1
rem Fail after link if any /alternatename LHS is also a DEFINED symbol in the
rem map -- a defined LHS defeats the alias silently (the wave-5 R1/R2 class;
rem EyerokD0 and the data_ov075 aliases flip the same way if their overlays
rem land). Post-link by design: the guard needs walk_window.map.
python "%~dp0tools\alternatename_guard.py" --map "%~dp0..\build\port\walk_window.map"
if errorlevel 1 exit /b 1
rem Fail after link if a hosted DS BAND did not come out of the linker in ROM
rem order. The DS reaches a band's members as INTERIOR ADDRESSES of its head --
rem the SetBankFor* family writes the GX bank block out to +0x18, the interrupt
rem handlers write the DTCM's OSi_IrqCheckFlag at DTCM_END - 8 -- so a split or
rem short host object puts every one of those writes on whatever the linker put
rem next. The expected offsets come from config/arm9/symbols.txt at run time.
rem Post-link by design, and over EVERY map rather than walk_window's alone:
rem /MAP is on CMAKE_EXE_LINKER_FLAGS so each target writes one, and the
rem runtime check in hal/cxx_aliases.cpp only ever reached the binaries that
rem link hal/sub_screen.cpp, on the one bring-up path that calls it.
python "%~dp0tools\gxband_guard.py" --build-dir "%~dp0..\build\port"
if errorlevel 1 exit /b 1
rem Fail after link if a frame that carries an ARM argument through on the host
rem stopped being a TAIL JUMP. Roughly fifty rows in the ov007 slice are
rem correct only because MSVC compiles a one-call forwarder as a jmp, which
rem reuses the caller's own cdecl frame so an argument the forwarder never
rem names is still where its target reads it. Nothing in the tree asks for
rem that. /Od, /Ob0 or one added statement in any forwarder turns it into a
rem real prologue and every affected row breaks in the same build, as a scatter
rem of unrelated-looking faults with no single change to point at. The scan
rem needs no disassembler -- an E8 or E9 rel32 in a frame's own map span,
rem resolved against the callee's address -- and it runs over EVERY map for
rem gxband_guard's reason: /MAP is on CMAKE_EXE_LINKER_FLAGS, three targets
rem host the ov007 slice, and nothing else in this build asks the question.
python "%~dp0tools\tailjump_guard.py" --build-dir "%~dp0..\build\port"
if errorlevel 1 exit /b 1
