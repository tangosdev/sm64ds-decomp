# Lane SEAT-OV002-RUNTIME -- four run-time spawns seated

LINKSCOPE.md lane 1: four classes that no level's object table ever places
and that only code ALREADY in the link asks for. A boot census never shows
them, which is why every earlier gate walked past them; a played frame does.
All four are seated, each with its own commit, each byte-matched, each proven
alive under FAULTS_FATAL.

* Worktree `C:\tmp\seat-ov002`, branch `port/seat-ov002-runtime`, rebased
  onto cons tip `3d048eb4e` (port-mount-noseat-cluster). The one rebase
  conflict was the additive CMake source line; both sides kept.
* Commits, in order: `8436aefe3` FIREBALL (254), `6fd029e28`
  BOWSER_SHOCKWAVES (281), `c395e7183` BUBBLE (291), `fd5b384af` YOSHI_EGG
  (9); this status file follows.
* Files: `port/hal/actor_classes_runtime_spawns.cpp` (the four fills, the
  faces, the SM64DS_RS_PROBE tick probe), `port/slice_gate210.txt`, the four
  registry rows in `port/hal/actor_classes.inc`, mount lines in
  `port/ov002_syms.txt` / `port/ov046_syms.txt`,
  `port/unmatched/YoshiEgg_StateDispatch.cpp` (new),
  `port/unmatched/ModelAnim_Renders.cpp` and `port/unmatched/Ov060_Renders.cpp`
  (two Render host copies), one debug-spawn line in `port/hal/level_boot.cpp`,
  one line out of `port/slice_gate10.txt`. Never touched: cons, `src/`,
  `include/`.
* Build TEMP/TMP `C:\tmp\seat-ov002\build\tmp`; the proof logs and the
  battery log live there too.
* linkage (port/tools/linkage.py on walk_window.map): 9302 on the cons tip's
  own build -> 9332 on this branch (82.1% -> 82.4%).
* battery (`python port/tools/battery.py --skip-build`, log `build/tmp/battery.log`): `battery: ALL GREEN`, exit 0; smoke suite, every mounted level and scene selftest, default boot, linkage, ptr_audit and the shipping-config build (PORT_ROM_CLEAN, static CRT, 373s) all ok.

## 1. The rule this lane ran on, and the Klepto lesson

A class is seated when its registry row, its 31-slot fill and its slice
entries are in, every lifecycle TU byte-matches under `tools/match.py`
(strict relocs, the pinned 2004/b56), and the `inferred_stub_guard` is green.
Every func_ov002 body that carries dsd's recovered-name marker was byte-matched
FIRST and ruled REAL_DECOMP in `port/tools/inferred_stub_adjudicated.txt`
before the seat (four bodies: Fireball's slot 18, Bubble's InitResources and
Behavior, YoshiEgg's state-0 main). Any live handler that cannot be matched
gets an inert return with one log line, never an abort; this lane needed none
for a handler, and the one unknown-word default (the egg's state switch) is
exactly that shape.

Proof is not a census. Klepto was "seated" with a quarantined actor sitting
in the census looking alive. So every class here ran its spawning level under
`SM64DS_FAULTS_FATAL=1` for 600 frames (a quarantine is an abort there, rc
nonzero), and the assertion was: rc 0, no `[quarantine]` / `UNHOSTED` line,
the id in both census blocks, and the actor MOVING. `SM64DS_RS_PROBE=1` was
added for the last part: one line per Behavior tick with the position and a
class word, off by default.

## 2. Per class

### FIREBALL (254, ov002) -- `8436aefe3`

Spawned by `Actor::SpawnFireball` (arm9, gate 10) for the fire piranhas and
the ov070/ov063 spitters. Six lifecycle TUs plus three Behavior helpers and
the class's own slot-18 `OnYoshiTryEat` (func_ov002_020f92e4, the marker
body, REAL_DECOMP). Table `_ZTV8Fireball` mounted, `_ZTV12daFPknBall_c`
aliased onto it.

Proof: `SM64DS_LEVEL=7 SM64DS_SPAWN_ACTOR=254`, FAULTS_FATAL, 600 frames rc 0,
census + 254 x1, the fireball flies and poofs. Live edge: `SM64DS_LEVEL=24
SM64DS_SPAWN=-6100,-2000,-3200 SM64DS_SELFTEST_IDLE=1` parks the player 400
units from a placed big fire piranha; it spits, end census + 254 x2, no
quarantine.

### BOWSER_SHOCKWAVES (281, ov060) -- `6fd029e28`

Spawned by Bowser's landing state func_ov060_02115b0c (wave 5). Seven TUs;
Render is the ModelAnim slot-5 collision on TWO ModelAnims and is hosted in
`unmatched/Ov060_Renders.cpp`. The table is a host array (the ov060 mount
excludes vtables). Its two BMA spellings `func_021115e4/f4` are level-window
addresses aliased onto the ov046 arena mount.

Proof: `SM64DS_LEVEL=38 SM64DS_SPAWN_ACTOR=281`, FAULTS_FATAL, 600 frames rc
0, census + 281 x1, alive and rendering all 600 frames. Live edge NOT fired
headless: Bowser never reaches his landing state on level 38 or 40 within
1800 frames without input.

### BUBBLE (291, ov002) -- `c395e7183`

Spawned by TreasureChest's open state func_ov064_0211a39c when the player is
underwater. dsd named none of `daObjAbuku_c`'s methods, so the own slots are
func_ov002 bodies; InitResources (020b3518) and Behavior (020b33dc) carry the
marker, REAL_DECOMP. D0 is a host thunk (the matched body spells its table and
heap as VT0/G0 shadow globals, the PoleLift trap). The unnamed table
`data_ov002_02108964` is mounted with the SpawnInfo.

Proof: `SM64DS_LEVEL=30 SM64DS_SPAWN_ACTOR=291 SM64DS_SPAWN=1500,-700,0`
(the aquarium; the debug spawn lands on the player's boot position and
SM64DS_SPAWN then moves him 1500 units away), FAULTS_FATAL, 600 frames rc 0:
300 Behavior ticks, the timer word 300 -> 1 while the position bobs on the
sine, then the timed pop (sound, particle, MarkForDestruction), end census no
live instance, no quarantine. Spawned ON the player (levels 8, 18, 30 bare) it
ticks twice and pops through the heal branch, which is the chest's whole
point. No vertical rise was observed in the headless run: the Behavior writes
the bob into +0x98 and the chest zeroes the spawn velocity, so what rises in
the game comes from elsewhere; the bytes are the ROM's. Live edge (an
underwater chest opening) needs the JRB chest sequence and was not driven
headless.

### YOSHI_EGG (9, ov002) -- `fd5b384af`

Spawned by the Player's egg lay (func_ov002_020d6368 / 020d5ab4, gate 10).
33 TUs byte-match (6 lifecycle + the 27 helpers between 0x020ec610 and
0x020eddc4, eight of them the state bodies); 25 enrolled here, the rest were
already on gate 10. Two host copies, both established shapes:

* Render: the ModelAnim slot-5 collision, `unmatched/ModelAnim_Renders.cpp`.
* The state setter (020ed63c) and main dispatch (020ed684): mwcc
  pointer-to-member over the bss table `data_ov002_02110a5c`, which
  `__sinit_ov002_02107118` fills with DS code addresses. Host copies in
  `unmatched/YoshiEgg_StateDispatch.cpp` read the {code, delta} pair plain
  and route through stategen's eight-case switch (state 0 held: 020ed5b0 /
  020ed0d4; 1 thrown: 020ecfc8 / 020ecf94; 2 landed: 020ecad4 / 020ec9c4;
  3 blue-coin rest: 020ec978 / 020ec938, read off the raw overlay image).
  020ed63c left slice_gate10.txt: its MSVC build was a call through a raw DS
  word that nothing had dispatched before this seat.

Marker body: func_ov002_020ed0d4 (dsd: daWarpkun_c::Kill, a misname),
REAL_DECOMP. `func_02123804` (the Whomp hit-by-egg call from the collision
helper) aliased onto ov079's linked func_ov079_02123804. Mount grows by the
SpawnInfo, the table, the model table 021000a0 and the tumble table 021000a8.
`port_debug_spawn_at` writes the laying player into a debug-spawned egg's
+0x38c, the write the real spawner makes before InitResources.

Proof: `SM64DS_LEVEL=7 SM64DS_SPAWN_ACTOR=9`, FAULTS_FATAL, 600 frames rc 0,
no quarantine, census + 9 x1 at both ends, 598 Behavior ticks in state 0, the
egg tracking the player's mouth from (3100,1950,4400) to (2150,250,4040) as he
walks. The `[dbgspawn] REGRESSION ... area -1` stderr line is the egg's own
InitResources writing all-areas (-1) to +0xcc, not a spawn-path fault.

## 3. Gaps, plainly

* Live edges not driven headless: the egg lay (needs Yoshi and an input
  script), the underwater chest, Bowser's landing. Fireball's live edge was.
* The egg's throw states (1..3) ran zero frames in the proof: state 0 holds
  the egg until the player throws. They are matched bytes behind a switch, not
  a stub, but they are unexercised.
* Slot 30 (`OnAimedAtWithEggReturnVec`, SRET) is a trap in every fill in the
  port, including this file's shared half. The egg's aim helper
  (func_ov002_020ecd18) calls slot 30 on its TARGET when one is in range, so
  an egg aimed at an enemy in play will hit that enemy's trap30 (decline,
  quarantine of the target; abort under FAULTS_FATAL). Not this lane's fills
  to change; the base body `_ZN5Actor25OnAimedAtWithEggReturnVecEv`
  (0x020100dc) exists in src and the fix is a lane over the shared fills.
* Bubble: no vertical rise seen headless (see above).
