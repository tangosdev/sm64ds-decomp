# Lane DBGSPAWN -- DELIVERY

Close chip task_495fb9f1: `SM64DS_SPAWN_ACTOR=345` (rc 139) and `=331`
(rc 127) fault at boot-spawn; regression required both to survive 60 frames.

* Worktree `C:\tmp\dbgspawn`, branch `port/dbgspawn-wings` off cons tip
  `952b3c135`
* Three feature commits: `417a0b393` (wing dispatch), `27f6eee3a`
  (debug-spawn param), `855c1ec16` (feather-collect trigger); this status
  file is the fourth
* Evidence `C:\tmp\dbgspawn-out\` (run dirs, `branch.patch`, exe hashes,
  `battery.log`, `abicheck.log`), build TEMP `C:\tmp\dbgspawnbld-tmp`
* Base exe `5af8009de...` (clean tip), fixed exe `5833b44a6...` (commits
  1-2), final exe `18c32d3da...` (with the trigger); sha256 files beside them

## The chip's framing was wrong, and the record should say so

The chip (status/VSPOPUP.md section 5 and the reviewer's REVIEW.md section 6)
classified both faults as one harness limitation for "actors whose
InitResources loads its own file". That classification is a confound: 331 and
345 do both load their own files, but the two faults have two unrelated
causes. Neither cause is the file load, and one of them is not a harness bug
at all.

## 1. 345 WING_FEATHER: a GAME bug, first exercised by the harness

The feather spawns at the player's feet and is collected within two frames:
`WingFeather::Behavior` reads its collider's collected-by word `unk_148`
(src/_ZN11WingFeather8BehaviorEv.cpp:84-96 -- `otherOwner` at +0x24 of the
MovingCylinderClsn at +0x124, include/CylinderClsn.h:61) and calls
`Player::InitWingFeathers(1)` -> `mHasWings = 1`. One frame later the render
latch `unk_700` opens and `hal_render_player_world`'s wing block dispatches
the wing ModelAnim (Player+0x174) through vtable slot 5.

That slot holds `hal/cxxname_bridge.cpp:530`'s `ma2_virtual18`, a
`__fastcall` face with a DEAD edx and TWO stack arguments
`(self, dummy, unsigned mat, const void *scale)`. The call site wore the
head's three-parameter render shape: `composed` rode the dead edx, the
callee read `mat` from the stack slot holding the scale pointer, read
`scale` from a slot nobody wrote (the fault's accessed 0x9), and popped 8
bytes where the caller pushed 4.

The gate opens in NO existing test -- the wing block's own comment records
`unk_700` staying 0 the whole run -- so the debug spawn was the first
exercise this path ever got. It is NOT debug-only: a live feather collect in
a real VS match (maps 1 and 3, exclamation block) walks the same path, and
those collects became REACHABLE the moment vs-popup seated class 345. This
fix is what un-held the 0.3.1 cut. `level_boot.cpp`'s level-31 note
(frame-zero fault inside the player's own render, "a player-model gate") is
in all likelihood the same smashed call -- that level enters winged -- but
level 31 is not a hosted level, so it stays a note.

### Debug-path proof (one binary per column, one variable per pair)

| run | build | result |
|---|---|---|
| `base_345` | clean tip | rc 139, FAULT accessing 0x9, frame 3 |
| `base_345probe` | clean tip | `[wings] gate OPENED` immediately followed by the FAULT |
| `fix_345` | fixed | rc 0, 900 frames, gate OPENED, wings drawn |
| `fix_345_60` | fixed | rc 0 (the chip's own regression check) |

`fix_345`'s final position is `(1126628, 1505315, -6833950)` -- bit-identical
to VSPOPUP.md 3c's established map-1 block-path triple, so the wing render
does not perturb the simulation. Wings visibly draw: the before/after
frame-30 BMP diff is a compact 41x45-pixel box at the player's back
(`wings_sidebyside_zoom.png`; the owner's eyes are on whether they look
RIGHT, asked separately).

### Real-path proof: the block's own feather, in a live two-window match

`lob_feather/`, map 1, parent+child through this lane's own relay
(127.0.0.1:41888, session `DBGSP11`, started and killed by PID here),
`SM64DS_FAULTS_FATAL=1` on BOTH windows:

* f200 `SM64DS_POUND_PROBE=22:200:28` drives the exclamation block's own
  OnHitFromUnderneath through `func_ov002_020eeca8`; block state 0 -> 1,
  RETURNED 1
* the block's content table spawns the feather (census `+ 345 x1`)
* f280 `SM64DS_FEATHER_TRIGGER=280` (commit `855c1ec16`, the star trigger's
  sibling -- writes the player's uid into feather+0x148, the one word
  Behavior reads; spawns nothing itself) -- the ROM's own Behavior runs the
  real InitWingFeathers
* `[wings] gate OPENED`, 620 winged frames, rc 0 on both windows, final
  position the same established triple

`lob_feather_fly/` adds `SM64DS_SELFTEST_JUMPSPAM=1`: the winged player
cycles Jump/Land/Walk/OnWall/SlopeJump/ButtSlide for the rest of the match,
rc 0 both windows. Stated honestly: headless jump spam never produced the
triple-jump FLIGHT, so `St_Fly` itself was not entered. Both fly states ARE
hosted (`port/hal/player_states.inc` rows `0x020e027c` St_Fly_Init /
`0x020dfdf0` St_Fly_Main -- matched bodies, not stubs), and the crash
surface this lane fixed is the winged RENDER, which those 620 frames
exercise every frame. A FORCE_STATE fly probe is buildable on the gate's
word if wanted.

## 2. 331 ONE_UP_LOGO: a harness input bug, param 0

`OneUpLogo::InitResources` takes `param - 1` as the TextureSequence start
frame. The debug spawn's default param is 0, so the sequence starts at frame
0xffff of an 8-frame BTP and the first Render walks off the file into
`Crash()` (rc 127). The ROM never spawns 331 with param 0: the slide's
proximity spawn hard-codes 8 (`src/func_ov002_020b76ec.c:45`) and
`Player::IncMegaKillCount` passes the running kill count. Param 0 is outside
the class's domain; the class owes it nothing.

| run | build | result |
|---|---|---|
| `base_331` | clean tip | rc 127, `Crash()` at frame 1 |
| `base_331p8` | clean tip | **rc 0**, 900 frames -- the param alone explains the whole crash |
| `fix_331` | fixed | rc 0, substitution line printed, param 0x8 spawned |
| `fix_331_60` | fixed | rc 0 (the chip's regression check) |

The fix: `port_debug_spawn_env` carries a measured id->param table (one row,
331 -> 8) applied only when no `:param` is spelled, and it says so on
stderr. An explicit `:param` -- including `:0` -- is honoured verbatim, so
the out-of-domain probe stays reachable on purpose.

## 3. The sibling question, closed by a re-runnable audit

The generalised form of bug 1 is "a raw vtable dispatch cast to the wrong
face arity". Enumeration, re-runnable as spelled:

```
grep -rn "__fastcall \*)" port/
```

over the whole port tree returns eight lines: five call sites, one comment
pair describing the convention (scene_mg_trampoline2.cpp:428 +
slice_tte.txt:395) and one comment (MgBase_ShadowSlot19.cpp:84). The five
sites, each checked against the face actually installed in that slot:

| site | slot | face installed there | face arity (stack args) | cast arity | verdict |
|---|---|---|---|---|---|
| player_bridges.cpp:288 (head) | Model 4 | ma2_render / mv_render `(self, edx, s)` | 1 | 1 | OK |
| player_bridges.cpp:813 (wing) | ModelAnim 5 | ma2_virtual18 `(self, edx, m, s)` | 2 | was 1, NOW 2 | THE BUG, fixed |
| player_bridges.cpp:852 (head) | Model 4 | ma2_render / mv_render | 1 | 1 | OK |
| tests/walk_window.cpp:7015 (ground probe) | 6 | collider slot-6 face | 1 | 1 | OK, exercised every boot |
| unmatched/MgPanel_StateDispatch.cpp:593 | 35 | mb_v35 `(self, edx)` | 0 | 0 | OK |

Exactly one mismatched site existed in the whole port, and it is the one
fixed. NUMBER (330) specifically: its Render reaches Model slot 5 through
compiler-generated virtual calls on the dual-filled table
(cxxname_bridge.cpp:504-511), no raw cast anywhere in its paths, and its
runs are green and byte-identical to base (below).

## 4. Inertness

`fix_330` vs `base_330` and `fix_331p8` vs `base_331p8`: censuses, spawn
lines and final positions byte-identical (only `[lvl-perf]` wall-clock lines
differ). Structurally: the wing call changes only code inside
`if (gate != 0)`, which no plain boot reaches; the param table only reads on
a bare env id matching a row; the feather trigger is inert without its env
var.

## 5. Gates (final tip `855c1ec16`, exe `18c32d3da...`)

| gate | result |
|---|---|
| `port/tools/battery.py . --linked-floor 9139` | ALL GREEN, rc 0; linkage 9139 (80.7%), +0 (no src change); skips level 27 TTC_MOVING_BEAM + level 45 GOOMBOSS, both pre-existing (`battery.log`) |
| `port/tools/abicheck.py` | 92 baselined, 92 live, 0 NEW, 0 retired -- EXTENSION RATCHET PASSED, rc 0 (`abicheck.log`) |
| build guards (closestplayer, inferred_stub, closure, facegen, mapdiff, vtablerows, stategen, alternatename, gxband, tailjump) | all green on every build in this record |

An earlier battery ran ALL GREEN rc 0 at the same floor on commits 1-2
alone; its log was overwritten by the final run's (same filename -- a
process nit for the next lane: name logs per run). The delivered line above
is the FINAL tip's own run.

## 6. Housekeeping

* Own paths only: worktree `C:\tmp\dbgspawn`, evidence `C:\tmp\dbgspawn-out`,
  TEMP `C:\tmp\dbgspawnbld-tmp` + per-run tmp dirs. The cons checkout was
  never built in, never relinked, never touched.
* Own ports only: relay 41888 (started and killed by PID, listener verified
  gone), games 47500-47509. Never 51765, clear of 41822/41833/47300s/47400s
  and 58434-58733.
* Process sweep before each relay/battery: every live python identified by
  command line (desktop extensions, unrelated servers); the one live
  `walk_window.exe` is the owner's own Desktop v0.1.1 bundle, left alone.
* The VSPOPUP.md section-5 open item and REVIEW.md section-6 chip wording
  ("InitResources loads its own file") are superseded by this record; the
  next debugger should key on "wing gate first opened" and "param outside
  the ROM's domain" instead.
