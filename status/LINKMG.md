# Lane LINKMG: the minigame shadows

Branch `port/shadow-replace-mg`, worktree `C:/tmp/c3-link`, base `ad09cd9f5`
(the cons tip). Scope: the 14 minigame host objects the coordinator named,
which carried 46 of the 194 SHADOW rows on that tip. Nothing under
`port/hal/comms_*`, `scene_vs_menu` or `VS_EntryDispatch` was touched.
`src/` was not edited.

## Numbers

Both readings from `python port/tools/linkage.py --queue` in this worktree,
saved as `build/linkage_before.txt` and `build/linkage_after.txt`.

| | before | after | delta |
|---|---|---|---|
| matched TUs in src/ | 11327 | 11327 | |
| linked into walk_window | 9139 (80.7%) | 9142 (80.7%) | +3 |
| SHADOWS, repo-wide | 194 | 148 | -46 |
| in-scope SHADOWS (the 14 objects) | 46 | 0 | -46 |
| documented host-ABI exceptions | 364 | 407 | +43 |
| FACES | 248 | 248 | |
| MSVC-name shadows | 17 | 17 | |

`port/tools/objsrc_check.py`: HOSTGEN 191 (was 189), HOST 0.

Battery: ALL GREEN (`build/battery.log`, run with `--skip-build` on the guarded build above, TEMP redirected as noted): 14/14 smoke binaries ok, 50 level selftests ok, 34 scene selftests ok including every minigame scene 361-389, default title boot ok, ptr_audit 0, shipping config (PORT_ROM_CLEAN, static CRT) built and its selftest rc=0. First attempt failed on smoke_player with `NitroFS table index missing: build/assets/nitrofs.tsv`: the fresh-worktree asset-catalog trap from the decomp-worktree skill, not a code fault; `python tools/asset_catalog.py generate <rom>` into a real build/assets fixed it and the regenerated files.tsv sha256 matches the main checkout's.

## What the 46 actually were

Every one of the 14 files opens with a `PORT_HOST_ABI.` banner (period, not
colon) that states the ruling in prose. `linkage.py` binds a ruling only from a
`PORT_HOST_ABI:` tag in the comment directly above a DEFINITION
(`_reasons_in`, `port/tools/linkage.py:182`). So 43 of the 46 were rulings
that had been made, written down, and never bound to the symbol the queue asks
about. That is exactly the finding `port/arm9_frontier.txt` records for
`func_0203cbc0`, `func_020432e4` and `func_020441cc`, and it takes the same
fix that lane applied: a one-line tag at each definition, comments only, no
code change, no link change. The other 3 were real replacements: two host
bodies whose stated reason the port already had a mechanism for, and one whose
stated reason was not load-bearing on the host at all.

## Per object

REPLACED = the matched src TU is in the binary and the host body is gone.
KEPT = the host body stays and is now tagged at its definition with the reason.

### Replaced (3 symbols, +3 linked TUs, 2 files retired or shrunk)

**MgHud_HighScore.cpp / `func_ov004_020b19f0`: REPLACED.** The minigame HUD's
HIGH SCORE drawer. The host copy was `src/func_ov004_020b19f0.c` with its
`void *` parameter retyped `int`. On a 32-bit cdecl host that retype changes
no code: the src TU is plain C, does not include `decl_common.h`, and both its
parameter and `func_ov004_020b1ea4`'s third argument are the same 4-byte slot
either way. The dropped-argument defect run mg5 lane HSDISP fixed lives
entirely in the CALLER, `func_ov006_020fba48` (s8 at both ends plus an
invented parameter), and that body stays. `src/func_ov004_020b19f0.c` is back
on `port/slice_mg1.txt`; the `SM64DS_MG_SCORE_TRACE` witness moved into the
caller's host body and prints the same two numbers at the same instant.

**MgBase_DeclConflict.cpp / `func_ov004_020aeed8`, `func_ov004_020af094`:
REPLACED, file deleted.** dScMgBase_c slots 30 and 29. Both src TUs include
`decl_common.h`, which declares them `(void*)`, and define them `(char*)` and
`(Obj*)`: C2733 under MSVC. The file's banner said, correctly, that this was a
declaration conflict and "NOT AN ABI WORKAROUND AND NOT TAGGED AS ONE", and it
ruled out widening the header. It did not use the port's existing per-TU
answer: `tools/hostgen.py`'s `HEADER_SHADOW`, which wraps the include in
`#define name name__hdrshadow` / `#undef name` so the header's declaration of
the TU's OWN name is hidden and the definition keeps its src spelling. That
route is already taken by `func_ov002_020cfbdc`, `func_ov102_0214b248`,
`func_ov071_02121ba4` and the Whomp/TtcRotatingCube cases. Two entries added;
a CMake loop after `SCENE_MG_SOURCES` generates the two objects into
`build/port/host-src/src/`; the host file is `git rm`'d rather than left
inert. hostgen also rewrites the 18 raw `*(u32*)0x400xxxx` derefs into
`NTR_MMIO` proxies. For DISPCNT (0x4000000 / 0x4001000) and POWCNT1
(0x4000304) `ntr/io.cpp` models no write side effect, so the proxy lands on the
same mapped register page the raw write did; behaviour is identical by
construction, and the emitted files were diffed against src before wiring
(only the `//cpp` line, the generated header and the 18 MMIO lines differ).

### Kept, now ruled at the definition (43 in-scope symbols plus 23 siblings)

All of these are the mwcc POINTER-TO-MEMBER WALL. An mwcc member pointer is an
8-byte `{code, adjustment}` pair called through a five-instruction sequence
(disassembled in `unmatched/MgBase_StateDispatch.cpp` section 1). MSVC's is 4
bytes with an incompatible call shape, so the src TUs cannot compile and
behave; each host copy is the src body with the one PMF call replaced by the
address switch. The tag text names the class and the mechanism. The siblings
in parentheses are bodies of the same shape whose symbol MSVC folded into the
switch that calls them, so no map row exists and they were never SHADOWS; they
are tagged for consistency, exactly as the arm9 lane did for `func_02042ffc`.

- MgBase_StateDispatch (7): 020add88 020adf2c 020b31b4 020b321c 020b3278 020b8714 020b8778
- MgPachinko_StateDispatch (5): 020fad34 020fb60c 020fc7d0 020fda7c 020fe248
- MgLuigi_StateDispatch (4, +2): 020f0d58 020f1e90 020f300c 020f3414 (+ 020f0044 020f0ba0)
- MgBomroom_StateDispatch (4, +3): 020d65c8 020d836c 020d8f98 020d91b0 (+ 020d7c00 020d6278 020d5fec)
- MgTeresa_StateDispatch (4): 0211d5a8 0211dd0c 0211f6fc 021203ac
- MgPachinko2_StateDispatch (3): 020fff84 0210246c 02102e8c
- MgCoin_StateDispatch (3, +4): 020dd2cc 020de26c 020de69c (+ 020dc298 020dc754 020ddd6c 020de440)
- MgPanel_StateDispatch (3, +8): 021050bc 02106ca4 02107358 (+ 02106bc0 02106eb8 02106f44 02106fdc 0210709c 02104ac4 02104c60 021057f0)
- Mg3DEsp_StateDispatch (3, +4): 020e8d08 020e9374 020e9e00 (+ 020e9b70 020e8a44 020e8830 020e82fc)
- MgCurling_StateDispatch (2, +3): 020e3078 020e3528 (+ 020e12d0 020e0d84 020e1214)
- MgMemory2_StateDispatch (2, +2): 020f5c40 020f7458 (+ 020f7234 020f71c8)

Three carry a reason of their own:

- MgBase_StateSetter / `func_ov004_020b87e0`: KEPT. The framework's state
  setter; builds and indexes the 8-byte pair table and dispatches through it.
  PMF wall, host-copied as an address switch.
- MgBase_StateSetter / `func_ov004_020b7460`: KEPT. ARM r0 ride-through. src is
  `void f(void) { func_ov004_020b743c(); }`, a tail-jump veneer that drops the
  object pointer its two callers pass; on ARM it rides in r0, on cdecl it is
  lost. The host copy takes and forwards it.
- MgHud_HighScore / `func_ov006_020fba48`: KEPT. ARM r0 ride-through. src
  declares the 32-bit score `s8` at both ends and invents a parameter the ROM
  does not take; MSVC writes one byte of the outgoing slot and passes four
  (dumpbin evidence in the file banner). This is the body that painted 999999.

## Not done, and why

- No `src/` edits. The DeclConflict header fix proper (five files in the
  byte-gated tree) stays routed as before; `HEADER_SHADOW` makes it unnecessary
  for the port.
- Lane records that cite `MgBase_DeclConflict.cpp` as a finding of their own
  sweep are left as written (slice_bnt, slice_pch, slice_ccn, slice_sos,
  slice_wig, slice_flw, slice_lkt, mg_fanout_costs.txt,
  stage_lifecycle_map.txt, and the banners of MgBase_Slot1/Slot7/LangTable,
  MgFlower_InitScore, MgMemory2_SubObjCtor, MgPachinko_Award,
  MgPachinko_OamRender). They describe what that lane found on its day. The
  live wiring (CMakeLists.txt, slice_mg1.txt, hostgen.py) is updated.
- The other three ROM callers of the drawer (`func_ov006_020e34ec` correct,
  `func_ov006_020e67f0` and `func_ov006_021004c0` call it with no argument)
  are unchanged; HSDISP's banner already records them and they belong to other
  classes' seats.
- The remaining Mg* queue rows (MgBase_ShadowSlot19, MgBase_Slot1/5/7,
  MgBase_LangTable, MgPachinko_OamRender/Award/Faces, MgLuigi_Faces,
  MgTeresa_InitResources) were not in the coordinator's list and were not
  touched.

## Environment finding

`port/build-port.cmd` failed twice before configure on this machine:
`tailjump_guard.py --selftest` writes a fabricated PE to `%TEMP%` and gets
`PermissionError` reading it back (real-time protection refusing the synthetic
exe under `C:\Users\...\AppData\Local\Temp`). Deterministic, three of three
by hand. With `TMP` and `TEMP` pointed under `C:\tmp` the selftest passes
45/45 and the build runs. Every build and the battery here ran with
`TMP=TEMP=C:/tmp/c3-link/build/tmp`. Not a tree defect; recorded so the next
lane on this box does not read it as one.

## Commands

    git worktree add C:/tmp/c3-link -b port/shadow-replace-mg ad09cd9f5
    # junctions (PowerShell New-Item -ItemType Junction) from the main
    # checkout: extracted, tools/mwccarm, tools/bin, build/objcache, build/assets
    export TMP=C:/tmp/c3-link/build/tmp TEMP=C:/tmp/c3-link/build/tmp
    cmd //c "port\build-port.cmd"                        # baseline, all guards
    python port/tools/linkage.py --queue > build/linkage_before.txt
    python <scratch>/add_tags.py . <scratch>/plan.json   # 68 tags, 12 files
    # MgHud_HighScore.cpp split; hostgen HEADER_SHADOW x2; CMake hostgen loop;
    # slice_mg1.txt line + notes; git rm port/unmatched/MgBase_DeclConflict.cpp
    cmd //c "port\build-port.cmd"                        # incremental, all guards
    python port/tools/linkage.py --queue > build/linkage_after.txt
    python port/tools/objsrc_check.py
    python port/tools/battery.py --skip-build > build/battery.log
