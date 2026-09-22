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
rem Fail before configure if TWO per-source property calls in CMakeLists.txt
rem name the SAME source for the SAME property. CMake REPLACES a per-source
rem property rather than appending to it, so the second call silently throws
rem the first away: no warning from CMake, no diagnostic from the compiler,
rem just a -D that stopped being on the command line. main's TU consolidation
rem folded one-function files into class TUs and the rows that named them were
rem carried across BY NAME, which is how four factory rows came to name one
rem path each and levels 28 and 39 lost their boot to the stale survivor
rem (398ff4501). Twenty-five more sources had the shape; lane CDEFS cleared
rem them and this line keeps it cleared.
rem NOT THROUGH guardcache, for vtalias_guard's reason: it needs no build and
rem no map, it reads one file, it costs a third of a second, and a remembered
rem verdict is not worth the chance of a stale one. It re-runs its own four
rem examples on every invocation before it looks at CMakeLists.txt and refuses
rem the build if any comes back wrong, so gutting a rule breaks the build
rem instead of silently disarming the check.
python "%~dp0tools\cdefs_guard.py" "%~dp0.."
if errorlevel 1 exit /b 1
rem Fail before configure if a /alternatename joins two vtable names config
rem gives DIFFERENT addresses. An alternatename asserts two names are one
rem object; two addresses say they are two classes, and whichever fill runs
rem last then owns both. That is the twelfth defect, which killed the castle
rem grounds the first time a door was drawn, and the same guard then found
rem five more across three unrelated pairs (ov064, ov030, ov091) that lane
rem ALIAS5 settled from the cartridge and fixed. It cost weeks to find by
rem running the game and costs a third of a second to find here, BEFORE
rem configure, because it needs no build and no map: only port/ source and
rem config/arm9/**/symbols.txt.
rem NOT THROUGH guardcache, for vptr_addend_guard and dtor_store_guard's
rem reason: this is a dispatch question, a remembered verdict is not worth
rem the chance of a stale one, and it is too cheap to be worth caching.
rem It re-drives its own twelve fixtures on every run before it looks at the
rem tree and refuses the build if any comes back wrong, so weakening a rule
rem breaks the build instead of silently disarming the check.
python "%~dp0tools\vtalias_guard.py" "%~dp0.."
if errorlevel 1 exit /b 1
rem Fail before configure if a vtable pointer is stored TWO SLOTS HIGH. mwcc's
rem own vtable symbol denotes the object start and the Itanium address point is
rem two words past it, so a key-function TU writes its vptr as `&_ZTV<X>[2]`;
rem the port's tables are host arrays whose slot 0 IS the first virtual, so that
rem spelling makes ROM slot 0 -- InitResources, the first call on the spawn path
rem -- enter the class's third virtual. Nothing else in this build can see it:
rem the byte gate compares the matched object, where the spelling is right, the
rem linker applies the addend without a word, and linkage.py counts the row.
rem Run link100, lane VPTR measured it; the ruling is in tools/hostgen.py's
rem VPTR_ADDRESS_POINT block and the parked rows are in
rem tools/vptr_addend_baseline.txt.
rem NOT THROUGH guardcache. The guard reads the BUILD's objects, and guardcache
rem keys a remembered verdict on the source trees a guard walks; build/port is
rem not one of those, so a cached green would go stale the moment a source
rem changed and the guard would stop looking. Three seconds, uncached, every
rem build. It reads the PREVIOUS build's objects here, which is the point of
rem the position: a regression refuses before the next six minutes of compiling
rem rather than after. The same guard runs again post-link, below, on the
rem objects this build just made.
python "%~dp0tools\vptr_addend_guard.py" "%~dp0.."
if errorlevel 1 exit /b 1
rem Fail before configure if a class whose INLINE-IN-HEADER destructor got a
rem forwarder in hal/dtor_forwarders_gen.cpp stopped satisfying the ruling
rem that forwarder exists under. MSVC's destructor stores its OWN vftable
rem (??_7Cls@@6B@, D1/D0 folded, so every virtual after the destructor sits a
rem slot early) into word 0 of the object before running the body, where the
rem cartridge's D1 stores _ZTV<Cls>. That store is inert only because nothing
rem reads the object's vptr between it and the base destructor call, which
rem stores a ROM-shaped table again as its own first act. Measured from
rem extracted/ per class, and re-measured here on every build: a body that
rem flips refuses the build rather than shipping a wrong dispatch that no byte
rem gate, no linker and no linkage count can see.
rem NOT THROUGH guardcache, for vptr_addend_guard's reason one line up: this
rem is the copy that convicts the tree in front of you, it costs about three
rem seconds, and a remembered verdict is not worth the chance of a stale one
rem on a dispatch question.
python "%~dp0tools\dtor_store_guard.py" "%~dp0.."
if errorlevel 1 exit /b 1
rem And that the generated file IS what its generator produces from this
rem tree: a hand edit to hal/dtor_forwarders_gen.cpp would otherwise add a
rem forwarder with no derivation behind it and the guard above would happily
rem measure it.
python "%~dp0tools\dtorfwd.py" "%~dp0.." --verify
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
rem CONFIGURE ONLY WHEN THERE IS SOMETHING TO CONFIGURE. This cmake call
rem used to run on every build. On an already-configured tree with nothing
rem changed it was measured at 2m23s here and 4m53s on the integration tree,
rem single-threaded, on one core out of twelve, before a single file
rem compiled -- and it produced a build.ninja identical to the one already
rem there. build.ninja carries its own RERUN_CMAKE edge which reconfigures
rem on demand, and as of the commit before this one that edge lists every
rem configure-time input this build has: all 233 slice lists, the four
rem predicate sources and tools/tierscan.py, on top of CMakeLists.txt, the
rem cache, the compiler files and CMake's own modules. Editing a slice list
rem therefore reconfigures by itself. Read port/CMakeLists.txt's
rem "THE CONFIGURE-TIME INPUTS, REGISTERED" block before touching this: the
rem registration is what makes skipping the call safe, and without it a lane
rem that seats a body would compile the previous configure's source list and
rem see a clean build.
rem Two cases still configure. A build directory with no build.ninja has
rem nothing to re-run, and any argument passed to this script is a -D the
rem cached configure has never seen, so it must reach cmake (that is also
rem the documented way to refresh the git tip baked into port_gittip.c).
set "PORT_NEED_CONFIGURE="
if not exist "%~dp0..\build\port\build.ninja" set "PORT_NEED_CONFIGURE=1"
if not "%~1"=="" set "PORT_NEED_CONFIGURE=1"
if defined PORT_NEED_CONFIGURE (
    cmake -S "%~dp0." -B "%~dp0..\build\port" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="%CMAKEBIN%\Ninja\ninja.exe" %*
    if errorlevel 1 exit /b 1
)
rem Fail AFTER configure and BEFORE the compile if a ROM translation unit this
rem build compiles PLAIN stores to a DS geometry command port. ntr maps real
rem memory across 0x04000000, so that store LANDS -- the word is written, the
rem TU stays byte-matched, the linker is happy and linkage.py counts the row --
rem and the geometry engine is never told. Nothing else in this build can see
rem it; the picture is simply wrong. Run mg15 lane TITLETEX found twelve of
rem these in ov007 by reading literal pools, run link100 lane MODESEL a
rem thirteenth, lane SAMECLASS eleven more. This asks the same question
rem mechanically, in about a second, off the configured build.ninja.
rem HERE rather than post-link because the answer is in build.ninja, which
rem configure has just written: a regression refuses before six minutes of
rem compiling rather than after. NOT THROUGH guardcache, for
rem vptr_addend_guard's reason -- guardcache keys a remembered verdict on the
rem source trees a guard walks and build/port is not one of those, so a cached
rem green would go stale the moment a slice list changed and the guard would
rem stop looking. It re-drives its own seventeen fixtures on every run before
rem it reads the tree, so weakening a rule breaks the build instead of quietly
rem disarming the check, and it stands down BY NAME when a configure has a
rem deliberate pre-fix A/B arm switched on.
rem BRING build.ninja UP TO DATE FIRST, and only build.ninja. The block above
rem deliberately skips the cmake call on an already-configured tree and lets
rem build.ninja's own RERUN_CMAKE edge reconfigure on demand -- which happens
rem INSIDE the ninja run below. A guard placed here without this line reads the
rem PREVIOUS configure's build.ninja, so a source that stopped being routed
rem since the last build compiles plain once and only refuses on the build
rem after. Measured: deleting one row from SAMECLASS_GX_TU and running this
rem script compiled func_ov080_0212677c.c raw with the guard reporting clean.
rem Naming the build.ninja target runs that one edge and nothing else: on a
rem tree with nothing changed it is a no-op, and when a slice list or this
rem file's configure inputs moved it is the same reconfigure the run below
rem would have done anyway, just early enough to be worth checking.
ninja -C "%~dp0..\build\port" build.ninja
if errorlevel 1 exit /b 1
python "%~dp0tools\gxport_guard.py" --build-dir "%~dp0..\build\port"
if errorlevel 1 exit /b 1
ninja -C "%~dp0..\build\port"
if errorlevel 1 exit /b 1
rem Fail after link if a pointer-to-member CELL holds a body that reads its
rem receiver off the caller's stack. MSVC calls a pointer to member with
rem `this + delta` in ECX and pushes NOTHING, through a call and through a
rem tail jump alike, so a seat that writes one of hal/faces_sync_gen.cpp's
rem flat C faces -- or a matched body taken raw -- into such a cell gives the
rem body a receiver nobody wrote. Nine lanes have now each found that at a
rem different class (5ae983797, 27a24ff5a, 651b5e853, f9936e798, 45ce69707,
rem 00732a5ab and this lane's three commits), every time because a seat header
rem reasoned that the dispatcher tail jumps and leaves the caller's own
rem argument in place -- true of a FLAT C dispatcher, false of a __thiscall
rem member, and false again once /O2 inlines the dispatcher into a caller in
rem the same translation unit. Post-link by design: the guard reads the seated
rem words out of the built image's .rdata and walks each body's prologue, so it
rem sees what actually shipped rather than what a comment says. Its ledger of
rem tables, and the three adjudicated __cdecl exceptions with the flat
rem dispatcher named beside each, are in the file's own header. It runs BEFORE
rem alternatename_guard because this line inherits eight defeated aliases from
rem the sync and that guard exits the script.
python "%~dp0tools\pmf_guard.py" --root "%~dp0.."
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
rem And the vtable address-point check again, now on the objects this build
rem just produced rather than the previous build's. The pre-configure copy
rem above refuses a regression early; this one is the copy that actually
rem convicts the build in front of you.
python "%~dp0tools\vptr_addend_guard.py" "%~dp0.."
if errorlevel 1 exit /b 1
