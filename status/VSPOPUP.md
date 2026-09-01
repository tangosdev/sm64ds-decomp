# Lane VSPOPUP -- DELIVERY

Seat the two unregistered actors every live VS match declines: **NUMBER (330,
0x14a)**, the floating score popup, and **WING_FEATHER (345, 0x159)**.

* Worktree `C:\tmp\vspopup`, branch `port/vs-popup` off the cons tip `339db6bb3`
* **Branch tip: `d5f73c7c8`** (one commit; this status file is the second)
* Evidence `C:\tmp\vspopup-out\`, build TEMP `C:\tmp\vspopupbld`
* Binary the proofs below were run against:
  `sha256 21351599782f9769dcdcb4e90322995421e08de36b20ab656601a440341a2a6e`
  (`build/port/walk_window.exe`, the post-battery link)

Both classes were **MATCHED, not scoped** -- every method, both factories and
the one shared helper are byte-matched src with no NONMATCHING banner. Nothing
was guessed and no body was written.

---

## 1. What was seated, and what it turned out to be

Lane VSARENA2 found the symptom in seven of the owner's own live playlogs:
every one carried `[spawn-declined] actor 0x14a not registered`, and maps 1
and 3 carried `0x159` as well. Resolved against the ROM's own SpawnInfo records
in ov002:

| id | SpawnInfo | factory | vtable (RTTI) | object |
|---|---|---|---|---|
| 330 | 0x0210b0c8 | `Number_Spawn` 0x020f0d90 | `_ZTV15InvisibleSecret` 0x0210b0ec (`13daObjNumber_c`) | 336 B: Model +0xd4, TextureSequence +0x124 |
| 345 | 0x02108884 | `WingFeather_Spawn` 0x020b3248 | `_ZTV11WingFeather` 0x021088a8 (`11daFeather_c`) | 904 B: Model +0xd4, MovingCylinderClsn +0x124, WithMeshClsn +0x158, ShadowModel +0x314 |

**Neither id is placed by any level's object table.** Both are spawned at RUN
TIME, and both of their callers have been linked for a long time --
`Actor::SpawnNumber` (arm9 0x02010044, slice_gate10) for the popup and
`func_ov102_021492d4` (slice_gate180, the exclamation block's content table)
for the feather. The classes they asked for were the only thing missing. That
is also why a headless boot census never asked for either and no earlier gate
noticed.

### The config's class names are shifted for NUMBER, and the port already knew

`hal/actor_classes_bob_world.cpp` says in as many words, where it seated
INVISIBLE_SECRET (329), that the six src files spelled `_ZN15InvisibleSecret*`
are NUMBER's methods and 0x0210b0ec is NUMBER's table. 329's real class is the
unnamed `func_ov002_020f0*` family at `data_ov002_0210b030` -- different
record, different factory, different table -- and this seat does not touch it.

---

## 2. The change

| file | what |
|---|---|
| `port/ov002_syms.txt` | `Number_SpawnInfo:0x1c WingFeather_SpawnInfo:0x1c` |
| `port/hal/actor_classes_vspopup.cpp` | NEW: two 31-slot ROM-shaped fills, host storage for both tables, two RTTI aliases, one mangling alias, seven method faces |
| `port/slice_vspopup.txt` | NEW: six src files each + the shared helper |
| `port/hal/actor_classes.inc` | externs + rows `{330 NUMBER}` `{345 WING_FEATHER}` |
| `port/CMakeLists.txt` | slice block, a duplicate guard, the slice on all three targets |

`src/` untouched. No `port/unmatched/` host copy: nothing in either class
needed one.

Things checked against the ROM's own bytes rather than inferred:

* **Record size 0x1c, not the 0x24 next-symbol delta.** Both records read
  `{+0 factory, +4 id, ..., +0x1c 0 offset-to-top, +0x20 typeinfo}` and their
  vtable begins at +0x24, so 0x1c ends the record exactly where the Itanium
  vtable prefix starts. The registry's own +4 cross-check reads **330** and
  **345** back and prints no `WRONG RECORD` line.
* **Slots 18..30 of both tables are the plain shared Actor defaults**, the same
  twelve addresses in both -- neither class overrides any of the interaction
  tail.
* **The five SharedFilePtrs** the two InitResources load were already mounted;
  nothing new was needed for either class's resources.

Two judgement calls, both following existing precedent and both written into
the fill's header:

* **The two D1s are spelled in the fill, not compiled.** Both src D1s are
  MSVC-synthesised member chains over local shadow hierarchies with no bodies
  of their own -- the ONE_UP_LOGO / gate-31 recipe. Number's ROM body has no
  vptr store at all (its whole relocation span is the three destructor calls);
  the feather's does store its own table, which is a no-op over itself since
  nothing derives from it. A CMake guard refuses the slice if either D1, or
  either already-in-slice_gate10 caller, is listed.
* **The slot-5 question, answered before it cost anything.** Both Renders reach
  the Model through a local shadow class and the two are shaped differently:
  Number's has no destructor (MSVC counts the call at 5), the feather's has a
  virtual one (MSVC folds D1/D0 and counts it at 4). Both land on
  `Model::Render`, because `_ZTV5Model` is filled in MSVC numbering with slot 5
  dual-filled. This is the first thing to re-check if either ever renders wrong.

One linker alias was needed: `WingFeather::InitResources` spells its model file
pointer at file scope, so MSVC mangles what the mount emits under its C name
(the IceSheet_ModelFile case, confirmed against the exact LNK2019 text).
Number's four already carried that alias from an earlier gate.

---

## 3. Proof

Every "before" below is the **same binary** with
`SM64DS_SKIP_CLASS=NUMBER,WING_FEATHER`, which leaves the class unregistered so
the spawn gate declines it exactly the way the shipped build does. One
environment variable is the only difference. Every collect and every block hit
runs through the ROM's own handlers -- nothing writes a score or a popup by
hand. All runs quiet, muted, minimised-never-activated.

### 3a. Two-window lobby VS match -- the headline

`re_lob_before` / `re_lob_after`, map 1, two windows against this lane's own
relay on 127.0.0.1:41822 (started and killed by this lane). The env block is
the launcher's own `LobbyLaunch.BuildChildEnv` -- the block VSARENA2 verified is
character-for-character what the owner's matches run. The relay log shows the
real session: `session VSPOP201 create`, `join ... as parent (1/4)`, `join ...
as child (2/4)`. Countdown finished, arena track started, `PLAYER x2`.

```
BEFORE   [spawn-declined] actor 0x14a not registered
         [census] 39 spawned (11 classes), 4 skipped (1 classes)
                  + 191 x2  PLAYER
                  - 330 x4
AFTER    (no decline line anywhere in the log)
         [census] 43 spawned (12 classes), 0 skipped (0 classes)
                  + 191 x2  PLAYER
                  + 330 x4  NUMBER
```

Four stars taken, four popups. The score really moves through the ROM's own
path: `[vsstar] ... scores=4,0,0,0 sum=4`.

### 3b. Direct VS path -- the deterministic one

`re_col_before` / `re_col_after`, map 1, 900 frames. The arena's markers are
broken through `func_ov002_020e7d84` and the released star is touched through
its own `+0x130/+0x134` words, which `func_ov002_020e930c` reads.

```
BEFORE   [spawn-declined] actor 0x14a not registered
         [census] 37 spawned (11 classes), 3 skipped (1 classes)   - 330 x3
AFTER    (none)
         [census] 40 spawned (12 classes), 0 skipped (0 classes)   + 330 x3
```

**Player position bit-identical in both: `(1135389, 1505363, -6856814)`.** The
popup does not perturb the simulation. (The lobby pair's positions differ by a
few units because a two-window networked run is timing-dependent by nature;
the direct path is the one that can carry this claim, and it does.)

### 3c. The feather, maps 1 and 3

`re_blk1_*` / `re_blk3_*`. The arena's exclamation block is opened through the
ROM's hit path (`func_ov002_020eeca8` -> slot 28 OnHitFromUnderneath); the
block runs state 0 -> 1 -> 2 and its content table spawns 0x159.

```
map 1  BEFORE  [spawn-declined] actor 0x159 not registered   - 345 x1
       AFTER   (none)                                        + 345 x1  WING_FEATHER
map 3  BEFORE  [spawn-declined] actor 0x159 not registered   - 345 x1
       AFTER   (none)                                        + 345 x1  WING_FEATHER
```

Exactly the two maps VSARENA2 named.

### 3d. FAULTS_FATAL, all four maps, both paths, 900 frames

`re_ff_star_0..3` and `re_ff_blk_0..3`, `SM64DS_FAULTS_FATAL=1`:

* star path: rc 0 on every map, `+ 330 x4` on every map, 0 declines
* block path: rc 0 on every map, `+ 345 x1` on maps 1 and 3, 0 declines

A live vtable audit (`SM64DS_VT_AUDIT`) finds 345 on the behaviour list with
its real table and every tail slot a host thunk, and a list trace reads it back
`alive 1 kill 0` -- so InitResources succeeded and the object is fully built.

### 3e. Adventure unchanged

`re_lvl1` diffed against the pre-seat baseline `base_lvl1`: **byte-identical
census**, 82 spawned / 22 classes / 0 skipped, same 22 rows in the same order.
Inert by construction as well as by measurement -- neither id is on any level's
object table, so a registry row can only change what a PLAYED frame builds.
The battery's own bare default boot (no `SM64DS_*` at all) is green too.

### 3f. BMPs -- FOR THE OWNER'S EYES, I am not judging these

`C:\tmp\vspopup-out\bmp\`. Three before/after pairs, identical frame, one env
var apart, each as `_OFF` / `_ON` / `_SIDEBYSIDE` / `_ZOOM`:

| pair | diff box |
|---|---|
| `vs_map1_number_f10` | (234,219)-(275,264) |
| `vs_map1_number_f14` | (234,226)-(275,267) |
| `adventure_lvl1_number_f12` | (234,220)-(278,268) |

Plus `lobby_map1_p1/p2_last_frame` from the live two-window match.

What I can say factually: a compact sprite appears where there was none, and it
moves DOWN between f10 and f14, which is what the class's own Behavior does
(it pops up, decelerates, bounces once, then spawns particle 0xd2 and destroys
itself). **What I am NOT judging and want the owner to look at:** in these
shots the popup is spawned at the player's own feet (the debug spawn puts it
there), so Mario/Yoshi stands in front of it, and the quad reads with a black
surround. That may be the number's normal outline, or it may be the texture's
alpha not keying. In the real game it spawns 200 units above the star you just
took, clear of your body. **Tango: does the number look right?** If the black
box is wrong that is a separate texture-path question, not this seat.

---

## 4. Gates

| gate | result |
|---|---|
| `port/tools/battery.py . --linked-floor 9126` | **ALL GREEN**, rc 0 (`battery.log`) |
| linkage | **9139** (80.7%), floor 9126, **+13** = exactly the thirteen src files this slice adds |
| ptr_audit | 0 unhosted code pointers |
| 50 level selftests, 391 scene selftests, default boot, 20 smokes | all ok |
| shipping config (PORT_ROM_CLEAN, static CRT) | built, linked, selftest rc 0 |
| `port/tools/abicheck.py` | **0 NEW, 0 retired** (92 baselined, 92 live) (`abicheck.log`) |
| abicheck rows from `actor_classes_vspopup.cpp` | 58 rows: 51 OK, 6 DECLINE (the six named traps: slots 13/14/30 x 2 tables), 1 NORETURN, **0 breaks** |
| build guard + inferred_stub_guard | clean, no new guessed vtable body seated |

---

## 5. One open item, named rather than papered over

`SM64DS_SPAWN_ACTOR=345` -- the **harness's** debug spawn, which drops an actor
at the player at BOOT -- faults three frames later, and the faulting frame is
inside the PLAYER's model render (`hal_render_player_world` ->
`ModelAnim::Virtual18` -> `Model::Render` -> `ModelComponents::Render` ->
`func_02045074` -> `func_02044b30` -> `func_0204488c`, reading 0x9). It is not
the feather's own frame.

What I ruled out: the feather is fully initialised (`alive 1 kill 0`, so
InitResources returned 1), the collision registry at boot is byte-identical to
a clean boot, and `data_020a0e68` -- the shared scratch matrix
`func_ov002_020b2c44` writes -- is correctly sized at 48 bytes.

What points away from this seat: **ONE_UP_LOGO (331), merged and shipped, and
the same "InitResources loads its own file" shape, faults on the same harness
path** (rc 127, its own Render in the stack), while COIN / POWER_STAR / CAP --
whose files the level already has resident -- do not. The working reading is a
boot-order or resource-residency limit of the debug spawn.

**It is not on a path the game reaches.** Nothing debug-spawns actors in a real
match, and on the ROM's own spawn paths all eight FAULTS_FATAL runs are clean.
Stated as an open question, not claimed clean. If a reviewer wants it closed
before merge, say so and I will chase it; it would be a harness fix, not a
change to this seat.

---

## 6. Housekeeping

* Local relay on 127.0.0.1:41822 started and killed by this lane. Verified at
  the end: no `walk_window.exe` from `C:\tmp\vspopup\` running, no listener on
  41822. The two live `C:\tmp\vsmercy\...` processes were identified by path and
  **left alone**; so was the owner's own bundle.
* Ports used: relay 41822, game 47300-47309. Never 51765, clear of 51762-51770
  and 58434-58733.
* No live VPS lobby contact. No other lane's tree touched.
* The session was killed mid-flight by the account limit after the battery went
  green. Everything in section 3 was **re-run from scratch afterwards** against
  the post-battery binary, and abicheck was run fresh. The re-runs reproduce the
  earlier numbers exactly, including the BMP diff boxes to the pixel.
