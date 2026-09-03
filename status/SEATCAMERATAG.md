# Lane SEAT-CAMERATAG -- CAMERA_TAG (333, ov002) seated, ADDRESS-FIRST

The camera tag: RTTI 10daCamTag_c, a 212-byte plain Actor the camera reads
off the actor list. Seated off its ROM addresses, not the config's names, and
proven alive by direct spawn under FAULTS_FATAL. No shipped level places it.

* Worktree `C:\tmp\seat-cameratag`, branch `port/seat-cameratag`, base cons
  `f1b05e8fe`. Seat commit `057454bcc`, status commit on top. Not merged, not
  pushed. Never touched: cons, `src/`, `include/`.
* Files: `port/hal/actor_classes_bob_world.cpp` (the fill, the host-storage
  table, the SM64DS_CAMTAG_PROBE tick line), `port/hal/actor_classes.inc`
  (three declarations and one registry row), `port/hal/dtor_faces_cpp.cpp`
  and `.h` (the CPPD1(InvisiblePole) face and its prototype; the WHAT IS NOT
  DONE note corrected), `port/ov002_syms.txt` (the SpawnInfo mount),
  `port/slice_gate33.txt` (seven flat TUs), `port/slice_dtorfaces.txt` (the
  class-form D1).
* Build TEMP/TMP `C:\tmp\seat-cameratag\build\tmp`; logs there: `build.log`,
  `proof_l7.log`, `proof_l7_bare.log`, `battery.log`, `census_333.log`.

## 1. The address trail (the trap, and why the config's names are wrong twice)

EVIDENCE LINE (live tree at f1b05e8fe): `port/hal/actor_classes.inc` had no
row for id 333 and no `CameraTag_Spawn` / `CameraTag_SpawnInfo` declaration;
`port/ov002_syms.txt` did not mount `CameraTag_SpawnInfo`; no `_ZTV13InvisiblePole`
or `_ZN13InvisiblePoleD[01]Ev` definition anywhere under port/; the only
mentions were the skip notes in `hal/dtor_faces_cpp.cpp:73-74`,
`status/DTORFACES.md:109-110` and `status/DTORPAIRS.md:63-65`. So 333 was
UNSEATED. The 287 row (`INVISIBLE_POLE`, commit 53f043910) fills
`data_ov002_02108480` (RTTI daBar_c) and is untouched here.

Read straight off the ROM (extracted/arm9_dec.bin, overlay_0002.bin, base
0x020ad660) and config/arm9/overlays/ov002/relocs.txt:

    ACTOR_SPAWN_TABLE[333] (arm9 0x02090864 + 333*4) = 0x02108518
    0x02108518 = CameraTag_SpawnInfo: +0 0x020b07c8 (CameraTag_Spawn), +4 halfword 333
    CameraTag_Spawn pool  reloc from:0x020b07f4 -> 0x0210853c   (the table it installs)
    0x0210853c = config _ZTV13InvisiblePole; typeinfo at -4 = 0x021084fc -> name "10daCamTag_c"
    slots: 0 0x020b07c0  3 0x020b07a4  6 0x020b07b8  9 0x020b07b0  12 0x020b07ac
           16 0x020b0748  17 0x020b076c   (31 words; 1,2,4,5,7,8,10,11,13..15,18..30 arm9 Actor)

So the config's `_ZN13InvisiblePole*` symbols (0x020b0748..0x020b07c0) are
THIS class's slot bodies, and the config's `_ZN9CameraTag*` bodies
(0x020b07f8..0x020b0938) belong to the NEXT table, 0x021085f8, RTTI
"10daChRoom_c" (its Behavior calls ChangeArea; that is the room-change tag,
not the camera tag). Nothing of that second class is touched by this lane.

Byte match at the ROM addresses (`tools/match.py --module ov002`, 2004/b56),
8 of 8: `src/CameraTag_Spawn.c` 0x020b07c8/0x30, `_ZN13InvisiblePoleD1Ev.cpp`
0x020b0748/0x24, `_ZN13InvisiblePoleD0Ev.c` 0x020b076c/0x38,
`..16CleanupResourcesEv.c` 0x020b07a4/0x8, `..16OnPendingDestroyEv.c`
0x020b07ac/0x4, `..6RenderEv.c` 0x020b07b0/0x8, `..8BehaviorEv.c`
0x020b07b8/0x8, `..13InitResourcesEv.c` 0x020b07c0/0x8. None carries the
"recovered from vtable slot identity" marker. So every live handler is a
matched body; no inert-return was needed and none was written.

## 2. The seat

* Registry row `{333, "CAMERA_TAG", CameraTag_SpawnInfo, CameraTag_Spawn,
  hal_fill_camera_tag_vtable}`; SpawnInfo mounted `CameraTag_SpawnInfo:0x1c`.
* Vtable: host storage `void *_ZTV13InvisiblePole[31]` (DSSTATE bracketed),
  defined under the config's own name so the D0 body's store and the factory's
  install hit one array with no alternatename. `bw_fill_shared` then slots
  0/3/6/9/12 -> the five flat bodies, 16 -> `hal_cppd1_InvisiblePole` (the
  dtor_faces_cpp face over the class-form D1), 17 -> `_ZN13InvisiblePoleD0Ev`.
* The DTORFACES / DTORPAIRS wall ("stores _ZTV13InvisiblePole, which is
  CameraTag's table, no host table") is exactly what this lane removes; the
  notes in dtor_faces_cpp.cpp say so now.

## 3. The edge: no level places 333

The runtime census of all 50 mounted levels (cons runner at f1b05e8fe,
`build/tmp/census_333.log`) names 333 nowhere. A static walk of all 52
LVL_Overlay object tables (data_02092208 / data_020758c8, every star group,
Standard and Simple loaders, raw index 11 -> 333 via data_ov002_0210cbf4)
finds 0 records; the same walk reproduces INVISIBLE_POLE (287) on level 7 x2
and level 8 x4, the counts the level-7/8 notes already carry. No src body
spawns 333 by id (every 0x14d in src/ is a sound id). So the class's live
edge is the debug spawn, which is the same Actor::Spawn path a level record
takes.

## 4. Proof

Level 7 (Whomp's Fortress, the pole's level, a full cast), 600 frames:

    SM64DS_LEVEL=7 SM64DS_SPAWN_ACTOR=333:0 SM64DS_FAULTS_FATAL=1
    SM64DS_WINDOW_SELFTEST=600 SM64DS_CAMTAG_PROBE=1 walk_window.exe   rc 0
    [dbgspawn] actor 333 (CAMERA_TAG) param 0x0 at (3100, 1950, 4400) yaw 4000 area 0 -> 3000FAEC
    [census] 191 spawned (50 classes), 0 skipped (0 classes)   + 333 x1 CAMERA_TAG
    [camtagprobe] tick 1 3000FAEC id 333 pos (3100,1950,4400) param 0x0 area 0
    [camtagprobe] tick 598 3000FAEC id 333 pos (3100,1950,4400) param 0x0 area 0
    598 Behavior ticks, 0 fault / quarantine / UNHOSTED lines, no REGRESSION.

Bare level 7 (no spawn override), same flags: rc 0, 190 spawned / 0 skipped,
0 fault lines -- unchanged from before the seat.

## 5. Gates

* `port\build-port.cmd`: all pre-configure guards green (closestplayer_guard
  9651 TUs; inferred_stub_guard "no new guessed vtable body seated", 2 on
  baseline unchanged), full configure + ninja, alternatename_guard,
  gxband_guard, tailjump_guard all OK. Exit 0.
* linkage (`port/tools/linkage.py` on walk_window.map): before 9479 (83.7%)
  on the cons map at f1b05e8fe; after 9487 (83.7%). Delta +8: the eight
  matched TUs above, each present in the map.
* battery (2026-09-03, `python port/tools/battery.py --skip-build` on this
  build, log `build/tmp/battery.log`): ALL GREEN. 50 mounted levels + 34
  hosted scenes under SM64DS_FAULTS_FATAL=1, linkage 9487 (83.7%), ptr_audit
  0 unhosted code pointers, shipcfg build + selftest ok. Only the pre-existing
  decomp-owned skips: level 27 (TTC_MOVING_BEAM) and level 45 (GOOMBOSS).

## 6. Gaps

* No shipped level places 333, so there is no level-table proof to give; the
  proof is the direct spawn through the same spawn path. If a level's table
  is ever found to carry raw index 11, it is already served.
* The camera-side reader of the tag (whatever walks the list for id 333) was
  not exercised: a tag alone has nothing to change, its Behavior returns 1
  by the ROM's own byte-matched body. This lane seats the class; it does not
  claim the camera consumes it.
* The config's `_ZN9CameraTag*` / `_ZTV9CameraTag` (daChRoom_c) stay
  unseated and misnamed on the decomp side; a naming correction there is
  decomp work, not port work.
