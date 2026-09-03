# Lane HUD-INITRES: HUD::InitResources goes back to src/

Branch `port/hud-initres`, worktree `C:/tmp/hud-initres`, base `5600924cb`
(the cons tip). Scope: one symbol, `_ZN3HUD13InitResourcesEv`, the HUD's
resource loader that status/SHADOWA.md section 4 refused. Nothing under
`src/`, `include/` or cons was touched. Route (a) was taken: the matched TU
is linked, its body verbatim, and the host copy is deleted. Not merged, not
pushed.

## 1. Numbers

Both readings from `python port/tools/linkage.py --queue` in this worktree,
`build/linkage_before.txt` (the base, built here with all guards) and
`build/linkage_after.txt`.

| | before | after | net |
|---|---|---|---|
| linked into walk_window | 9403 (83.0%) | 9404 (83.0%) | +1 |
| SHADOWS, repo-wide | 18 | 18 | |
| MSVC-name shadows | 13 | 12 | -1 |
| documented host-ABI exceptions | 528 | 528 | |
| FACES | 263 | 263 | |

HUD::InitResources was an MSVC-NAME SHADOW (the host copy defined
`?InitResources@HUD@@QAEHXZ` as a real C++ method, so the Itanium-name join
never saw it). `port/tools/objsrc_check.py`: HOSTGEN 214, HOST 0. The map
now reads

    ?InitResources@HUD@@QAEHXZ  _ZN3HUD13InitResourcesEv.cpp.obj
    ?GetHealth@Player@@QAEEXZ   _ZN6Player9GetHealthEv.cpp.obj  (alias, same address as QAEHXZ)
    _port_hud_loadfile / _port_hud_lz16 / ?port_hud_objpltt@GX(S)@@  hud_load.cpp.obj

and `HUD_InitResources.cpp.obj` is gone from it.

## 2. What SHADOWA refused, and where each half went

SHADOWA section 4 named two walls, neither an ABI wall.

**(1) Double decompression.** The port's `LoadFile` (hal/level_boot.cpp) is
SharedFilePtr-backed and hands back decoded bytes; the matched body calls
`LoadFile` and then `DecompressLZ16` on the result, the ROM's contract
(func_0201818c returns the file still compressed). SHADOWA's worry was that
carrying this as ~25 exact-string patches would be a host copy spelled inside
the tool. It is not carried at the call sites. Both walls live BEHIND the
callee NAMES, so hostgen gained one table, `CALLEE_SEAM`, whose entry for
this symbol is three rows:

    LoadFile        -> port_hud_loadfile
    DecompressLZ16  -> port_hud_lz16
    LoadOBJPltt     -> port_hud_objpltt      (GX:: and GXS:: alike)

emitted as one `#define` each at file scope, ahead of the TU's own
declarations. The TU's `void* LoadFile(int handle);` inside its `extern "C"`
block therefore declares the seam with C linkage, `namespace GX { void
LoadOBJPltt(...); }` declares `GX::port_hud_objpltt`, and every one of the 25
calls in the body resolves to the seam with the body untouched. The emitted
file was diffed against src before wiring: the `//cpp` line, hostgen's
header, the three defines and `extern` on the ten globals are the whole
difference. The entry hard-errors, like the exact-string tables, if the TU
stops calling a listed name.

The seams are `port/hal/hud_load.cpp`. `port_hud_loadfile` is
`func_0201817c` (hal/fs.cpp, the raw loader: bytes on the game heap, still
compressed, a fresh allocation per call so the body's `Deallocate` is right),
which is exactly what the host copy substituted by hand at every site.
`port_hud_lz16` calls the port's own `DecompressLZ16`; the two
`port_hud_objpltt` call the ROM-named `_ZN2GX11LoadOBJPlttEPKvjj` /
`_ZN3GXS11LoadOBJPlttEPKvjj`. So the pair decodes exactly once, into the same
VRAM addresses, from the same bytes, as before.

**(2) The missing handles.** The seam keeps the host copy's null path
verbatim: a null from the raw loader prints `[hud] file 0x%x is not in this
extraction; skipped` (up to eight times) and the two consumers skip a null.
`Deallocate` is not routed because src/Deallocate.c already tolerates a null.
Measured on this extraction the path never fires: the EU dump carries
ARCHIVE/cee.narc and data/2D_cad/EUR/ENG/d_2d_cmn_icon_E_ncg.bin, and the
before and after run logs carry zero `[hud]` lines and zero `load-at ... not
found` lines on levels 1 and 6. SHADOWA's "ten handles absent from the
catalog" reading came from build/assets/handles.tsv, which lists plain FAT
handles only; handles at or past 0x8000 are archive-interior ids that
hal/fs.cpp resolves through port_archive_map, not the TSV. The guard stays
because the refusal was written against a US dump that lacked the archive
(hal/sub_screen.cpp records the same history for the in-game BG set), and a
tree built from one would otherwise fault in DecompressLZ16 on the first
level boot.

**The third item, `u8 GetHealth()`.** The TU's shadow declaration returns
u8; the matched definition returns int; MSVC puts the return type in a
member's decorated name, so the TU asks for `?GetHealth@Player@@QAEEXZ`.
hud_load.cpp carries the `/alternatename` onto `?GetHealth@Player@@QAEHXZ`
that SHADOWA prescribed. Both sides are __thiscall with no arguments
(aliascheck rule R and rule P both hold from the decoration), the value is
eax either way and the body stores it to a u8. alternatename_guard is green
post-link (the LHS is not defined anywhere).

**--extern-data.** Already able to read `void* data_0209f394[];` since
SHADOWA; the emitted file declares all ten globals, and the seven the port
owns elsewhere (the Player latches, the ov002 mount, the save block in
hal/level_boot.cpp) keep their storage.

## 3. Proof

`port/tools/hud_initres_proof.py` (committed) runs walk_window.exe on a level
under `SM64DS_FAULTS_FATAL=1 SM64DS_WINDOW_SELFTEST=300`, the tree's quiet
spawner (mp2_proof.env_base: NO_FOCUS, VOLUME=0, MINIMIZED, every inherited
SM64DS_ knob dropped), with `SM64DS_DUAL_SCREEN=1` so the presented stacked
image exists and `SM64DS_STACK_BMP` at frame 220 (both engines, host
overlays included), and copies that plus the 512x384 selftest framebuffer
into build/hud_initres_proof/<tag>/. `compare` diffs two captures pixel for
pixel, whole and per screen. Before-captures were taken on the base build in
this same build directory, after-captures on the changed build; the dsstate
base is 0x00EF9000 in all four runs, so notes/port-selftest-bmp-gate.md's
precondition for a byte compare holds.

    level 6 (Bob-omb Battlefield, coin counter on screen)
      before rc 0  pos=(-14379421, 52, 15814209)
      after  rc 0  pos=(-14379421, 52, 15814209)
      stack.bmp                44a990449a529ffc0a61b06035ea1196 == 44a990449a529ffc0a61b06035ea1196
      walk_window_selftest.bmp e63d5491e83d86f55f692b386ee889d1 == e63d5491e83d86f55f692b386ee889d1
      whole / upper screen / lower screen: 0 differing pixels each
    level 1 (castle grounds)
      before rc 0  pos=(-4915200, 2929633, 11141348)
      after  rc 0  pos=(-4915200, 2929633, 11141348)
      stack.bmp                6c840da306e0a39837e647a244ce56af == 6c840da306e0a39837e647a244ce56af
      walk_window_selftest.bmp 0016db4872abdb87363d53d7a29e08ef == 0016db4872abdb87363d53d7a29e08ef
      whole / upper screen / lower screen: 0 differing pixels each

The before and after run logs are identical line for line once the timing
lines are dropped.

That the seam is what runs, and not the port's decoding LoadFile: a level-6
run under `SM64DS_FS_TRACE=1` (build/hud_initres_proof/fstrace/run.log) shows
the body's six ENG handles going through hal/fs.cpp's raw reader
(`fs: load-at`), in the body's order, each resolved and none reported
missing:

    fs: load-at handle=0x269 id=187        0x6400000 / 0x6600000 tiles
    fs: load-at handle=0xa00e id=40974     vram_a + 0x2000
    fs: load-at handle=0x229 id=123        vram_b + 0x2000
    fs: load-at handle=0xa00a id=40970     GX + GXS palette 0..0x120
    fs: load-at handle=0x980f id=38927     GX palette 0x120..0x200
    fs: load-at handle=0x22a id=124        GXS palette 0x120..0x200

Battery: ALL GREEN (`build/battery.log`, `--skip-build` on the guarded build
above, TMP/TEMP under build/tmp): 20/20 smoke binaries ok, 0 level selftests ok
under SM64DS_FAULTS_FATAL=1, 0 scene selftests ok (the VS rows among them), default
title boot ok, linkage 9404 (83.0%), ptr_audit 0, shipping config (PORT_ROM_CLEAN,
static CRT) built and its selftest rc=0.

## 4. Gaps

- The VS branch of the body (`data_0209f2d8 == 1`, handles 0x8000..0x8003
  and the 0xX003/0xX007 language pairs) is exercised by the battery's VS
  scene rows and not by the two level captures above; its loads go through
  the same three seams.
- The null path is code that this extraction never runs. It is kept for the
  reason in section 2 and retires with the extraction note in hud_load.cpp.
- Environment: `port/build-port.cmd` needs TMP/TEMP under C:/tmp on this box
  (tailjump_guard's selftest, the finding LINKMG.md records); every build and
  run here used `build/tmp`.

## Commands

    git worktree add C:/tmp/hud-initres -b port/hud-initres 5600924cb
    # junctions from the main checkout: extracted, tools/mwccarm, tools/bin,
    # build/objcache, build/assets
    export TMP=C:/tmp/hud-initres/build/tmp TEMP=C:/tmp/hud-initres/build/tmp
    cmd //c "port\build-port.cmd"                         # base, all guards
    python port/tools/linkage.py --queue > build/linkage_before.txt
    python port/tools/hud_initres_proof.py capture before --level 6
    python port/tools/hud_initres_proof.py capture before_l1 --level 1
    # hostgen CALLEE_SEAM; hal/hud_load.cpp; CMake GATE27_GEN; slice note;
    # git rm port/unmatched/HUD_InitResources.cpp
    cmd //c "port\build-port.cmd"                         # after, all guards
    python port/tools/hud_initres_proof.py capture after --level 6
    python port/tools/hud_initres_proof.py capture after_l1 --level 1
    python port/tools/hud_initres_proof.py compare before after
    python port/tools/hud_initres_proof.py compare before_l1 after_l1
    python port/tools/linkage.py --queue > build/linkage_after.txt
    python port/tools/objsrc_check.py
    python port/tools/battery.py --skip-build > build/battery.log
