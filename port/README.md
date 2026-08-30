# port/ - the native PC build

This tree hosts the PC port of the decompiled game: the same `src/` sources
compiled for the host, a platform seam instead of DS hardware, and its own
build that never touches the byte-matching pipeline.

## Rules

- **`src/` stays the byte-verified source of truth.** The port compiles files
  FROM `src/`; it never edits them for host convenience. A file that needs a
  host-side change gets that change behind `SM64DS_PLATFORM_*` guards or a
  seam header, reviewed like any other src change.
- **Logically-correct-but-unmatched implementations live in
  `port/unmatched/`, never in `src/`.** The NDS hybrid build keeps selecting
  delinked ROM bytes for those functions; the PC build selects these.
- **32-bit host first.** The recovered ABI assumes 4-byte pointers. x64 comes
  after struct recovery makes layout host-independent.

## How the game boots

A launch with no `SM64DS_*` environment beyond what the launcher passes
(`SM64DS_ASSET_ROOT`, `SM64DS_VOLUME`) boots the ROM's own opening:

    title -> menu -> file select -> a slot is picked -> opening cutscene -> adventure

That order is the ROM's, and it is worth reading carefully, because the
cutscene is **last**, not first: `Stage::LoadClsnAndObjects` is what calls
`StartIntroCutscene`, and that runs during the level boot `StartFile` asks for
after the file has been picked. There is no cutscene before the title.

Two toggles, both of which the launcher exposes:

| Variable | Effect |
|---|---|
| `SM64DS_SKIP_MENU=1` | boot straight to the file select. The player still picks A, B or C. |
| `SM64DS_SKIP_INTRO=1` | no opening cutscene. The title still comes up. |

They compose. Both set is "file select, then straight into the game"; only
`SKIP_MENU` still plays the cutscene, because the cutscene is downstream of the
file pick rather than upstream of the title.

**An absent variable means off.** The launcher removes the name when the box is
unticked rather than writing `0`, so the game must read absence as off, and it
does. A present `0` is honoured too.

There is **no save medium yet**, so the file select offers three fresh files on
every boot. All three are selectable and all three start a new adventure.
Persistence is deliberately out of scope.

### Developer opt-outs

| Variable | Effect |
|---|---|
| `SM64DS_LEVEL=<n>` | boot that level directly, as before. The whole battery and every level proof uses this. |
| `SM64DS_SCENE=<id>` | boot that scene directly, as before. Read **before** the default is consulted, so a named scene never sees the default. |
| `SM64DS_VS_MAP=<0..3>` | boot a VS match. A destination too: the debug menu's VS row clears `SCENE` and `LEVEL` and sets only this. |
| `SM64DS_BOOT_CLASSIC=1` | the pre-ruling boot, straight to castle grounds as Yoshi. `SM64DS_TITLE_ENTRY=0` is the same thing under the name the tree already used. |

The whole decision lives in one place, `port_boot_default_scene` in
`hal/title_entry.cpp`, and that file's banner carries the derivation and the
frame-by-frame trace of the chain.

## Gate ledger

Each gate is a slice manifest + a smoke binary that proves one seam with
game data. `build-port.cmd` builds all of them into `build\port\`.

| Gate | Smoke | What runs on host |
|---|---|---|
| 1 | `smoke` | types, fx math, matrices, Timer, Fader hierarchy |
| 2 | `smoke_heap` | ExpandingHeapAllocator, 5,000-op torture |
| 3a | `smoke_roots` | SetupRootHeap + the Memory:: layer (game global heap) |
| 3b | `smoke_fs` | SharedFilePtr over the catalog card seam, LZ77 cross-checked |
| 4a | `smoke_gx` | the interrupt-driven display-list pump, byte-equal vs harness |
| 4b | `smoke_model` | the whole Model pipeline: load, rebase, VRAM upload, materials, render (Mario, textured) |
| 4c | `smoke_anim` | Animation/UpdateBones recursion (the Mad Piano, posed) |
| 4d | `smoke_soak` | every catalog model: 448/448 loadable render, zero faults |
| 5 | `smoke_frames` | the fiber frame loop: game-shaped frames, the piano attack in motion |
| 5b | `smoke_soak_anim` | every compatible model+BCA pair: 472/473 animate, zero faults |
| 6 | `smoke_oam` | the 2D sprite engine: OAM emit, affine params, upload to mapped OAM |
| 6b | `smoke_oam` | OBJ scan-out: the game's sprite placements become pixels |
| 7 | `smoke_modelanim` | ModelAnim: the game owns frame progression (speed, loop wrap) |
| 8 | `smoke_clsn` | collision: the octree walk answers ground queries over level KCL |
| 9 | `smoke_actor` | the actor framework: spawn, init, behave, render through vtable dispatch |

From gate 10 on the slices stack rather than standing alone, so they are
carried by two binaries instead of one smoke each: `smoke_player` runs the
whole stack headless, and `walk_window` is the same stack in an interactive
window. Gate 24 is the exception and keeps its own smoke.

| Gate | Binary | What runs on host |
|---|---|---|
| 10 | `smoke_player` | Player spawns and stands: ten body/head models from real archives, InitResources end to end, the real `ChangeState` into `St_Walk` |
| 12 | `walk_window` | the interactive window: WASD walks, ESC opens the debug menu, camera follows |
| 13 | `walk_window` | jump, camera-relative controls, and the real Camera actor's 19-state machine driving the projection |
| 14 | `walk_window` | the real level boot: `Stage::LoadClsnAndObjects` over the castle grounds' own ov009, mounted whole |
| 15 | `walk_window` | the game's own per-frame actor spine and real `WithMeshClsn` tracking (nothing calls `Player::Behavior` directly any more) |
| 16 | `walk_window` | real objects on the castle grounds: the five processing lists plus the actor classes themselves |
| 17 | `walk_window` | the level overlay's own actors, the first time any ov009 *code* compiles |
| 18 | `walk_window` | ov085: the castle grounds' rabbits and the Lakitu with the camera |
| 19 | `walk_window` | ov098: the cannon behind the grate on the west moat wall |
| 20 | `walk_window` | the last two ov002 classes the level names: EXIT x4, WATERFALL_MIST x7 |
| 21 | `walk_window` | ov100's butterfly and fish |
| 22 | `walk_window` | ov100's doors, loaded by `LoadDoorObjects` rather than the standard loader |
| 23 | `walk_window` | ov102's question block on the castle roof, the last id on the skip list |
| 24 | `smoke_modelanim` | BlendModelAnim: the cross-fading ModelAnim, on the gate-7 stack |
| 25 | `walk_window` | the bottom screen: the OAM lifecycle, `Stage::LoadGraphics2D`, `Stage::CheckCameraInput` |
| 26 | `walk_window` | the boot spine: the real Stage actor replacing the harness-staged scene root |
| 27 | `walk_window` | the HUD actor (id 334): hearts, coins, stars, timer, camera buttons |
| 28 | `walk_window` | the Minimap actor (id 335), plus the BG3-sub tilemap and extended palette |
| 30 | `walk_window` | the level is a parameter: `SM64DS_LEVEL` picks it, and Bob-omb Battlefield (level 6, ov014) boots and walks |
| 31 | `walk_window` | the level HANDOFF: LoadLevel/ExitLevel tear the level down through the game's own destroy path and bring the next one up |
| 33 | `walk_window` | Bob-omb Battlefield's mechanisms, terrain and pickups: its 60 coins and 8 red coins, the four warps, the six cannon lids, the arrow signs, the five secrets and the brick blocks |
| 35 | `walk_window` | the course loop: damage, death and respawn through the ROM's own states, coins into the counter and the health, the star's bookkeeping and the course-clear handoff, and the course's own sound group, bank and music |

Gate 30 mounts a level and gate 31 changes between them; the seam they meet at
is `port_level_mount_register` in `hal/level_change.cpp`, which gate 30's
`port_level_mounts_install` fills in.

There is no gate 11: it was folded into the gate-10 walking campaign before
either landed. Gates 25 through 28 were **renumbered at merge** because three
parallel streams each picked 24 the same night; the animation stream kept 24
for BlendModelAnim. Gate 29 belongs to the unmerged `port-particles` branch,
and gates 32 and 34 to branches that had not merged when 35 landed.

Gate 30's slice is empty of `src/`, which is the point rather than a gap: the
boot was already generic matched code walking the level's own tables, so a
second level cost no matched code at all. See the header of
`slice_gate30.txt`, and the evidence chain at the top of `hal/level_boot.cpp`
for how each level's identity is read out of the ROM.

Gate 35 is driven by `SM64DS_COURSE_PROBE=<what>[,<frame>]` on `walk_window`:
`coin`, `hurt`, `hurt2`, `drown`, `death`, `star`. `SM64DS_SND_PROBE=<N>`
counts sounding voices every N frames, and `SM64DS_COURSE_MUSIC=<seq>`
overrides the sublevel's own music row (the castle grounds' row really is
"no layer-1 track"; Bob-omb Battlefield's is sequence 58).

Supporting machinery: `tools/hostgen.py` (MMIO transform into the build
tree; src/ is never edited), `tools/romdata.py` (ROM constants from the
dsd-extracted image; Nintendo bytes stay out of git), `tools/ntr_manifest.py`
(fs manifest for the ntr backend), the `ntr/` platform layer (see its
README), and `hal/` (seams, bridges, and the storage for DS BSS globals).

Grow a gate by adding files to its `slice_gate*.txt` and assertions to its
smoke; `tools/host_frontier.py` reports how much of `src/` syntax-compiles
for the host so the next subsystem is a measurement, not a guess.

## Why the emulator is not here

The hybrid (tangOS-SM64DS) stays the verification harness: it proves
functions byte-for-byte against real hardware behaviour. The port is the
destination. Benchmarks on 2026-08-02 showed natives inside the hybrid are
performance-neutral (~2% of the emulated clock), which is the measured
argument for building the port rather than growing the hybrid further.

## Frontier baseline (2026-08-02)

First full sweep of `tools/host_frontier.py` (cl /Zs, 32-bit, all of src/):

    compiles for host: 11,005 / 11,222  (98.1%)

Refined the same day: the C2054 x98 bucket decoded as the asm-hatch TUs
(`asm void`, mwccarm syntax) -- DS hardware operations that SHOULD not
compile on a host. CP15 cache management is a host no-op, block copy/fill is
memcpy, context switch is the threads seam. The tool now classifies them as
HAL-owned rather than as syntax defects, which makes the honest frontier:

    compiles for host: 11,005 / 11,125  (98.9%)   + 97 asm TUs -> HAL backlog

120 real failures remain: generic syntax x44, C2732 extern-C linkage
contradictions x19, undeclared identifiers x13, unknown types x11, the rest
single digits. The takeaway stands, stronger: the port's work is platform
seams and link closure, not mass source repair. Re-run after each seam
lands; the number should only go up.
