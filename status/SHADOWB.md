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
