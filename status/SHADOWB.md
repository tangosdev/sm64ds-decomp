# Lane SHADOW-B: C's classified stand-in retirements

Branch `port/shadow-replace-b`, worktree `C:/tmp/shadow-b`. Rebased onto cons
tip `d63b92a09` (linkage 9238, co-op M2 included) before any commit, so every
delta below is measured against 9238. `src/` is never edited: a retirement
enrolls the matched src TU in its slice gate and removes or patches the host
stand-in, and nothing under `src/` moves.

Each retirement is one commit. Verification per commit: `port/build-port.cmd`
exit 0 (which runs closestplayer / inferred_stub / closure / facegen / mapdiff
/ vtablerows / stategen / alternatename / gxband / tailjump guards), then
`dsstate_guard`, `inferred_stub_guard`, `closestplayer_guard` green, linkage
up on the retired symbol, `git diff --stat d63b92a09 -- src/` empty, and the 20
`smoke_*.exe` all exit 0. Full battery (`port/tools/battery.py`) every few
commits.

Environment: this box needs `TMP`/`TEMP` under `C:\tmp` (the tailjump selftest
writes a synthetic PE that real-time protection refuses under AppData). Ran
everything with `TMP=TEMP=C:/tmp/shadow-b/build/tmp`. The fresh worktree's
`build/assets` junction lacked `nitrofs.tsv` (smoke_player rc=2, the
asset-catalog trap, not a code fault); regenerated with
`python tools/asset_catalog.py generate "<rom>"`, deterministic.

Baseline at d63b92a09: linked 9238 (81.6%), SHADOWS 149, FACES 262,
documented 404.

## 1. WaterRing::Render -- RETIRED PLAIN, and a real bug fix

`src/_ZN9WaterRing6RenderEv.cpp` enrolled in `port/slice_w3c.txt`; the host
copy's `_ZN9WaterRing6RenderEv` body removed from
`port/unmatched/Ov064_WaterRing.cpp` (with its dead `Model.h` include and the
`TextureTransformer::Update` decl it alone used), and the src TU dropped from
the duplicate-definition guard in `port/CMakeLists.txt`.

THE BUG FIX. The host copy called `Model::Render((const Vector3 *)0)` -- a NULL
scale. The matched TU calls `((Sub*)(c+0x30c))->g5(c+0x80)` -- self+0x80. The
ROM agrees with the matched TU, proven by disassembling the body out of
`extracted/overlays/overlay_0064.bin` (ov064 .text base 0x02115ee0, so
0x02119fc8 is file offset 0x40e8):

    0x02119fdc  add r0, r4, #0x30c      ; the plain Model at self+0x30c
    0x02119fe0  ldr r2, [r0]            ; its vptr
    0x02119fe4  add r1, r4, #0x80       ; the argument = self+0x80
    0x02119fe8  ldr r2, [r2, #0x14]     ; vtable slot 5 (0x14/4)
    0x02119fec  blx r2                  ; Render(self+0x80)

So the ROM passes this+0x80 to slot 5; the host copy's NULL was wrong.

SLOT 5 DUAL-FILL confirmed at `port/hal/cxxname_bridge.cpp:508-515`:
`_ZTV5Model[4] = mv_render` (Model.h-compiled TUs land Render at 4) AND
`_ZTV5Model[5] = mv_render` (local-shadow TUs count in ROM/Itanium numbering
with two dtor slots, landing Render at 5). `mv_render` is
`__fastcall(self, edx, s)` calling `Model::Render((Vector3*)s)`, so the shadow's
one-argument slot-5 call reaches Render with s = this+0x80. The host banner's
old claim that slot 5 was Virtual18's MSVC slot predates this dual fill.

Verification: build exit 0; guards + dsstate + inferred_stub + closestplayer
green; linkage 9238 -> 9239 (+1), SHADOWS 149 -> 148, `_ZN9WaterRing6RenderEv`
out of the SHADOW queue and present in `walk_window.map`; `git diff --stat
d63b92a09 -- src/` empty; all 20 smokes pass.

Commit `95f574fa4`.

## 2. SkiLift::InitResources -- RETIRED PLAIN

`src/_ZN7SkiLift13InitResourcesEv.cpp` enrolled in `port/slice_gate191.txt`;
host copy `port/unmatched/MotherPenguin_InitResources.cpp` removed from the
three CMake target lists and `git rm`'d. (The class is really MOTHER_PENGUIN
(257) under a dsd-era `_ZN7SkiLift*` mislabel; the retirement changes nothing
about that.)

WHY PLAIN WORKS. The host copy existed for the TextureSequence::Prepare
calling-convention seam. Prepare never uses `this`: it tail-calls
func_02046d50(arg, t) and that body reads only r0=arg and r1=t. The matched src
declares Prepare as a 2-arg function `(bmd, btp)` and calls
`Prepare(model, animFile)` -- the ROM's real register layout (r0=model,
r1=animFile). The host bridge (`hal/player_bridges.cpp:1666`) is 3-arg
`(self, bmd, btp)`. By cdecl argument aliasing, the matched src's 2-arg call
binds to the bridge as self=model, bmd=animFile, btp=dead, so
func_02046d50 is reached with arg=model, t=animFile -- the exact corrected order
the host copy produced by hand (it passed self=model, bmd=animFile explicitly,
with a valid-but-dead third value). The third value is never read past
func_02046d50's prologue, so a garbage btp is harmless; the two are
behaviourally identical. cdecl is caller-cleans, so the 2-vs-3 arity is safe.

Verification: build exit 0; guards + dsstate + inferred_stub + closestplayer
green; linkage 9239 -> 9240 (+1), MSVC-NAME SHADOWS 18 -> 17,
`_ZN7SkiLift13InitResourcesEv` out of the queue and in `walk_window.map`;
`git diff --stat d63b92a09 -- src/` empty; all 20 smokes pass. Level selftests
(the CCM/MotherPenguin fault check) validated in the battery run recorded below.

Commit `6855aa6ce`.

## 3. OneUpLogo::InitResources -- RETIRED PLAIN

The same TextureSequence::Prepare seam as item 2, one gate earlier (gate 190,
the first caller this seam was solved for). `src/_ZN9OneUpLogo13InitResourcesEv.cpp`
enrolled in `port/slice_gate190.txt`; host copy
`port/unmatched/OneUpLogo_InitResources.cpp` removed from the three CMake
target lists and `git rm`'d. The matched src declares Prepare 2-arg and calls
`Prepare(model.file, anim.file)`; against the 3-arg bridge, cdecl aliasing lands
self=model, bmd=animFile, btp=dead -- the same order the host copy passed by
hand as `(model.file, anim.file, anim.file)`, with func_02046d50 never reading
the third value. Behaviourally identical, so it retires plain.

Verification: build exit 0; guards + dsstate + inferred_stub + closestplayer
green; linkage 9240 -> 9241 (+1), MSVC-NAME SHADOWS 17 -> 16,
`_ZN9OneUpLogo13InitResourcesEv` out of the queue and in `walk_window.map`;
`git diff --stat d63b92a09 -- src/` empty.

## Battery after items 1-3 -- ALL GREEN

`python port/tools/battery.py --skip-build` (`build/tmp/battery_1_3.log`):
20/20 smoke binaries ok, every level selftest ok (including CCM/MotherPenguin
and the ov002 OneUpLogo path that exercise the retired Prepare callers), every
scene selftest 361-390 ok, default boot reaches TITLE 300 frames clean, linkage
9241 (81.6%), ptr_audit 0 unhosted code pointers, shipping config
(PORT_ROM_CLEAN, static CRT) built and its selftest rc=0. Pre-existing skips
unrelated to this lane: level 27 (TtcMovingCube has no matched body), level 45
(Goomboss func_ov074_02121380 has no matched body).

## 4. FlyGuy::InitResources -- RETIRED via a HOSTGEN linkage patch

The matched src declares its eight `data_ov070_*` externs ABOVE its own
`extern "C"` block, so MSVC gives them C++ linkage and asks the linker for
`?data_ov070_02123530@@3USharedFilePtr@@A` and siblings while the port's mount
emits those names with C linkage -- eight LNK2019. The host mirror
`unmatched/FlyGuy_InitResources.cpp` fixed this by hand, moving the eight lines
inside `extern "C"`.

Retired via hostgen instead. New exact-string mechanism `EXTERN_C_DATA` in
`port/tools/hostgen.py` (wired into `emit()` next to the other patch tables)
moves the `extern "C" {` opener above the data declarations for
`_ZN6FlyGuy13InitResourcesEv`, so they take C linkage. The symbol is added to
`OV70GEN_SYMS` in `port/CMakeLists.txt` (the ov070 hostgen set, already linked
into smoke_player / walk_window / walk_window_hires), the host mirror is removed
from those three target lists and `git rm`'d. The emitted file's body is
byte-identical to the src and to the retired host mirror (diffed); only the
extern "C" opener moved.

Verification: `python port/tools/hostgen.py` emits the file with all eight
externs inside the block, body unchanged; build exit 0, the generated object
compiled into all three targets; guards + dsstate + inferred_stub +
closestplayer green; linkage 9241 -> 9242 (+1), MSVC-NAME SHADOWS 16 -> 15,
`_ZN6FlyGuy13InitResourcesEv` out of the queue and in `walk_window.map`;
`git diff --stat d63b92a09 -- src/` empty; all 20 smokes pass.

Commit `2de1180ae`.

## 5. Player St_Jump_Main, St_NoControl_Init, St_WallJump_Main -- RETIRED via hal_call_state_fn HOSTGEN patches

Each of these three Player state functions reads a per-character/per-kind row
the ROM's sinit copied out of a code-pointer table and dispatches it as an mwcc
member pointer: `obj = this + (word1 >> 1)`, virtual bit `word1 & 1`, and the
code word (`word0`) is a DS code address or a DS vtable BYTE OFFSET. Called raw,
the non-virtual branch jumps to a ROM code address that on the host lands inside
the mounted ov002 DATA image -- that is the walljump crash St_WallJump_Main took
(row0 = 0x020e200c for Mario). The port owns `hal_call_state_fn(self, ds_addr)`
(`hal/player_bridges.cpp:1286`), which maps the DS code word to the hosted body,
applies the ROM null guard, and reports an unhosted address as a loud no-op.

New hostgen mechanism `CALL_STATE_FN` in `port/tools/hostgen.py` (decl-prepend
like DS_DIV/MG_PMF_CALL, wired into `emit()`) swaps each function's dispatch for
the seam call, exactly reproducing the retired `Player_St_*.cpp` host copies:
St_Jump_Main routes only the non-virtual branch (its table's ptr words are
filled and the host vtables are runtime-filled), while St_NoControl_Init and
St_WallJump_Main route BOTH branches (their ptr words are all zero, so the
virtual branch is dead and word0 would be a DS vtable byte offset). The three
symbols are enrolled in `GATE10_SYMS` (whose GATE10_GEN feeds the same three
targets as slice_gate10), the three host copies are removed from
`slice_gate10.txt` and `git rm`'d. Each hostgen output's dispatch site was
diffed against its host copy: identical.

Verification: `python port/tools/hostgen.py` on each emits the exact host-copy
dispatch (Jump 1 seam call, WallJump/NoControl 2 each, decl prepended); build
exit 0, all three generated objects compiled into the three targets; guards +
dsstate + inferred_stub + closestplayer green; linkage 9242 -> 9245 (+3),
MSVC-NAME SHADOWS 15 -> 12, all three out of the queue and in `walk_window.map`;
`git diff --stat d63b92a09 -- src/` empty; all 20 smokes pass.

Commit `bf5977ba5`.

## Battery after item 5 -- ALL GREEN

`python port/tools/battery.py --skip-build` (`build/tmp/battery_5.log`): 20/20
smokes ok, every level selftest ok, every scene selftest 361-390 ok, default
boot reaches TITLE 300 frames clean, linkage 9245 (81.6%), ptr_audit 0, shipping
config built and selftest rc=0. Same two pre-existing skips (level 27
TtcMovingCube, level 45 Goomboss). This run exercises the retired walljump path.

## 6. func_ov002_020ef670 -- RETIRED via DS_DIV

PathLift's path-follow tick. `dv = cstd::fdiv(len2, speed) / 0x1000` is zero
whenever a path segment is shorter than one frame of travel, and the two
angle-step divisions `ad2 / dv` and `ad1 / dv` then divide by zero. The ROM's
AEABI idiv answers x/0 = 0; x86 idiv faults c0000094 (measured on the VS castle
grounds, level 51, actor 0x1f, reproduced with SM64DS_LEVEL=51). The host copy
`unmatched/VS_PathDivGuard.cpp` routed both sites through a local
`arm_sdiv(a,b) = b ? a/b : 0`.

Retired through hostgen's DS_DIV table: two exact-string entries route
`ad2 / dv` and `ad1 / dv` to `ds_idiv(...)` (`hal/cstd_div.c`, same `b ? a/b : 0`
semantics); `/ 0x1000` is a constant divisor and is left raw, as the mechanism
requires. The slice_w6d CMake loop already skipped the plain src line; that skip
now generates the hostgen'd `.cpp` into `SLICE_W6D_SOURCES` (same three targets
as the retired host copy's `SLICE_VS_SOURCES`), and `VS_PathDivGuard.cpp` is
removed from `SLICE_VS_SOURCES` and `git rm`'d. The hostgen output's two
division sites were confirmed to route through ds_idiv, matching the host copy.

Verification: build exit 0; guards + dsstate + inferred_stub + closestplayer
green; linkage 9245 -> 9246 (+1), SHADOWS 148 -> 147, `func_ov002_020ef670` out
of the queue and in `walk_window.map`; `git diff --stat d63b92a09 -- src/`
empty; all 20 smokes pass.
