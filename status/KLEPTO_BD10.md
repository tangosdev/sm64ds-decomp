# Lane KLEPTO_BD10 -- DELIVERY

func_ov062_0211bd10, Klepto's e17c MAIN state handler (the per-frame cap-steal
logic, ov062, size 0x508), is decompiled to a byte-matching translation unit
and seated in the port in place of the inert-return stub.

* Worktree `C:\tmp\klepto-decode`, branch `port/klepto-bd10` off the cons tip
  `2d4f3104e`
* Branch tip: see the report; this file is the last commit
* Under `src/` exactly one file is new: `src/func_ov062_0211bd10.cpp`. No
  header, no `config/` file and no other matched source was touched

---

## 1. What was wrong

`port/unmatched/Klepto_StateDispatch.cpp` seated nine matched handlers over
Klepto's ten state-template records and an inert-return stub over the tenth,
0x0211bd10. e17c is the state a default (cap-stealing) Klepto enters from
InitResources and never leaves on its own, so its MAIN half is the one that
runs every frame: on the port the bird hovered at its spawn point, never flew
its path, never watched the player and never dove. The behaviour gap (no cap
steal) was logged once and left.

## 2. The decompilation

`src/func_ov062_0211bd10.cpp`, `//cpp`, the same extern "C" free-function
shape the sibling handlers use (func_ov062_0211c2f4.cpp is the closest
relative: same PathPtr shadow, same helpers, same offset spelling). What the
body does, in order:

1. While the post-steal cooldown (+0x444) runs: turn toward the home point
   (+0x484) or, on a wall, 90 degrees off the current heading; ease pitch and
   roll; when within 500 units of home clear the cooldown, else keep it at
   30; drive the velocity from the heading (func_ov062_0211b2fc) and return.
2. Otherwise compute the pitch to the home point (nothing held) or to the
   current path node (+0x430, something held) and a third of the yaw error
   (AngleDiff / 3); ease pitch (ApproachAngle on SSL, ApproachLinear
   elsewhere) and roll; yaw by 0x200 on SSL or when carrying a carried item,
   0x400 otherwise.
3. Nothing held and the player still has his cap: find the closest player;
   off SSL refuse if he is more than 40 units above the bird or +0x706 is set;
   if he is within 1000 units (SSL) or 700 units (elsewhere) horizontally and
   neither +0x709 nor +0x6fb is set, enter the dive state (e18c). Drive the
   velocity and return.
4. Something held: the flap/glide animation state machine on +0x43c (SetAnim
   on the two BCA files at data_ov062_0211e104 / e114, the +0x440 count-down
   on SSL and the two 0x18/0x19 levels).
5. Off the carried-item levels, with something held, re-pick the path node
   (func_ov062_0211b3ac) and cancel the +0x100 timer when it changes.
6. Drive the velocity; when both timers are 0 and the bird faces its current
   path node to within 0x2000, enter the path-node state (e15c).

Every offset it reads is one the nine matched siblings already read under the
same meaning; no new field was invented.

### Match evidence

    python tools/match.py --c src/func_ov062_0211bd10.cpp --func func_ov062_0211bd10
        --addr 0x0211bd10 --size 0x508 --bin extracted/dsd/arm9_overlays/ov062.bin
        --base 0x02115ee0 --module ov062 --strict-relocs --cpp-check
    === mwccarm 2004/b56 ===
    MATCHING VERSIONS: 2004/b56

Strict relocs ran (no "reloc-destination check unavailable" line); 36 reloc
slots, every destination the one config/arm9/overlays/ov062/relocs.txt
records. Then the pre-push hook's own gate, which links the object and
compares the linked bytes to the ROM:

    python tools/prepush_linkcheck.py --files src/func_ov062_0211bd10.cpp
    [OK  ] func_ov062_0211bd10                          VERIFIED
    prepush-linkcheck: 1 checked - 1 verified, 0 warning(s), 0 blocking

Two source forms were tried on the way. The first draft declared `PathPtr
path;` at function scope, which made mwcc emit the constructor call at entry
(the ROM constructs it inside the final `if`), and copied the player position
through `*(Vector3 *)`, which loads through immediate offsets where the ROM
takes the address first: 4 bytes long. Moving the PathPtr into the block and
copying through a `Vector3 *` matched. The SetAnim call was first spelled as
a local `BlendModelAnim::SetAnim(BCA_File &, int, int, int, unsigned short)`
member (the b880 / bc54 / b930 spelling): it byte-matches but the mangled
name it emits is `..iiit`, not the ROM's `..ii5Fix12IiEt`, so linkcheck
reported BLIND-2 as it does for those three siblings. A `Fix12<int>` template
parameter reproduces the ROM name but changes the frame (0x30, not 0x28), so
the call is spelled by its ROM name under extern "C", the InitResources.cpp
convention, and the link check verifies every slot.

No NONMATCHING banner, no asm, no pragma.

## 3. The port change

| file | what |
|---|---|
| `port/slice_klepto.txt` | `src/func_ov062_0211bd10.cpp` joins the ten handlers; `src/func_ov062_0211b2fc.c` (the velocity-from-heading helper bd10 and ba84 both call) is listed explicitly. Banner updated. |
| `port/unmatched/Klepto_StateDispatch.cpp` | `klepto_bd10_unmatched` deleted; the seat row for 0x0211bd10 points at `func_ov062_0211bd10`. Banner rewritten: all ten handlers are matched src. |
| `port/hal/actor_classes_koopa_chuckya.cpp` | `SM64DS_KLEPTO_PROBE=1`: one line per Behavior tick with the position, the seated state descriptor (e14c..e18c), the held-actor id, the two timers, the carried-item word and the save's lost-cap flag. Off by default, prints nothing. |

## 4. How it was proven

### 4.1 Level 16 selftest, bare, 900 frames

build/port rebuilt off this branch through port/build-port.cmd with every guard
(rc 0), then:

    SM64DS_LEVEL=16 SM64DS_WINDOW_SELFTEST=900 SM64DS_FAULTS_FATAL=1
    SM64DS_NO_FOCUS=1 SM64DS_VOLUME=0 SM64DS_KLEPTO_PROBE=1 walk_window.exe
    rc 0
    [census] 89 spawned (34 classes), 0 skipped (0 classes)
             + 239 x1   KLEPTO

898 `[klepto]` ticks for the level's own bird (3002709C), 367 distinct
positions, 658 ticks in e17c (the bd10 main) and 240 in e15c (the path-node
state bd10 hands off to): the bird flies its path and is never quarantined.
First and last ticks:

    [klepto] 3002709C pos (1749,1292,770) state e17c held 00000056 t100 600 t444 0 item 1 lostcap 0
    [klepto] 3002709C pos (-3009,1292,870) state e15c held 00000056 t100 0 t444 0 item 1 lostcap 0

(held 00000056 / item 1 is the level's own Klepto, which InitResources spawns
carrying its star item; the bare-cap default is the param-0 bird section 4.2
injects.)

### 4.2 The scripted steal, level 16, 900 frames

A bare param-0 Klepto (the cap-stealing default; the level's own bird carries
its star item and never steals) is dropped ON the grounded player at frame
120 by `SM64DS_KLEPTO_SPAWN_AT=120` (port/hal/input_probe.cpp, the same
port_debug_spawn_at path SM64DS_KEY_SPAWN_AT uses, class 239). The boot-time
`SM64DS_SPAWN_ACTOR=239` was tried first and cannot land the steal: it fires
at the level-16 entrance 1737 units above the sand, the player falls to y=0
in 40 ticks, and the bird's 50-tick dive (func_ov062_0211ba84, ~22 units a
tick) runs out ~600 units short every time (run C, kept in the lane's
build/tmp, 8 dives, held 0 throughout).

    SM64DS_LEVEL=16 SM64DS_WINDOW_SELFTEST=900 SM64DS_FAULTS_FATAL=1
    SM64DS_NO_FOCUS=1 SM64DS_VOLUME=0 SM64DS_KLEPTO_PROBE=1
    SM64DS_KLEPTO_SPAWN_AT=120 walk_window.exe
    rc 0
      [klepto] f120 spawning KLEPTO param 0 at the player (830,0,5909) floor 0
    [dbgspawn] actor 239 (KLEPTO) param 0x0 at (830, 0, 5909) yaw 80b0 area 0 -> 30026248
    [census] 93 spawned (34 classes), 0 skipped (0 classes)
             + 239 x2   KLEPTO
             + 269 x5   CAP
    selftest: 900 frames, pos=(2648865, 31, 24111969)

The dropped bird (30026248) runs bd10 as its e17c main, sees the player
inside the 1000-unit SSL window, enters the e18c dive, and on the dive's third
tick the +0x144 cylinder is over him and func_ov062_0211b51c steals. The
probe line the tick before and the tick of the steal (walk_window frames f217
and f218):

    [klepto] 30026248 pos (825,37,6055) state e18c held 00000000 t100 48 t444 0 item 0 lostcap 0
    [klepto]   player (837,0,5958) floor 0 f6fb 0 hit134 00000000 hit168 00000000
    [f217] pos=(837.6,0.0,5957.7) spd=73728 st=020d9b68 ...
    [head] f217 30039f88 30025e78 00000000 00000000
    [klepto] 30026248 pos (827,38,6025) state e17c held 0000005b t100 600 t444 30 item 0 lostcap 16777216

held 0000005b is the CAP actor (class 0x10d) b51c spawned and handed the
bird; lostcap 16777216 is 0x1000000, exactly the `0x1000000 << currentCharacter`
bit SaveData::PlayerLoseCap sets for Mario (src/_ZN8SaveData13PlayerLoseCapEv.c),
read back through HasPlayerLostCap; the player is knocked (spd 0 -> 73728,
state 020dd104 -> 020d9b68) and the [head] line is his cap coming off. The cap
census went 2 -> 5 over the run because the cycle then repeats the way the ROM
writes it: the bird drops the cap seven ticks later (held -> 0, the Behavior
host copy's +0x403 arm), the player standing under it picks it up at once
(lostcap -> 0), bd10 sees a capped player again and dives again (caps 5c and
5d); the third cap it keeps and climbs away with (e15c at y=1043 by tick 665,
held 0000005d to the end). The dropped bird's state/held/lostcap changes:

    tick  98  e17c held 0000005b lostcap 0x1000000   (first steal)
    tick 105  e17c held 00000000 lostcap 0x1000000   (cap dropped)
    tick 106  e17c held 00000000 lostcap 0           (player re-collects)
    tick 156  e18c                                   (bd10 dives again)
    tick 191  e17c held 0000005c lostcap 0x1000000   (second steal)
    tick 249  e18c
    tick 284  e17c held 0000005d lostcap 0x1000000   (third steal, kept)
    tick 665  e15c held 0000005d                     (path node, cap in tow)

The level's own bird (3002709C) is untouched by any of this: e17c/e15c on its
path at y=1292 for the whole run, held 00000056 (its star item) throughout.

hit134/hit168 print 0 on every line because the probe runs after
Klepto::Behavior, which Clears both cylinders as its last act; the steal is
witnessed by held/lostcap/[head], not by those two words.

BATTERY_PLACEHOLDER

## 5. What is NOT proven

GAPS_PLACEHOLDER
