# Smartball minigame -- field-name provenance

Every name in this file was taken from what the matched bodies actually do with
the field. Offsets are from the class's own header; the citation column names
the file that proves the name and says what it does there. Where the tree gives
no evidence the field keeps its `unk_NNN` name -- those rows are listed too, so
the next reader knows the gap was looked at rather than missed.

Naming is byte-neutral: every function listed below still reproduces the
cartridge under 2004/b56, checked per function with `build_pin`'s `verify`
(the tool lives in `tools/build_pin.py`) and then with a whole-ROM
`tools/rombuild.py` run.

The shared base `cMgSmartball_object_c` was deliberately left alone in the
first pass, because renaming it forces matching edits into six sibling headers
that pass did not own. A later pass that owned the whole family finished it --
see "The base" below, which now records a rename that has landed rather than
one that is proposed.


## What the minigame is

`dScMgSmartball_c` owns the whole board. Three of its tables come up over and
over in the classes below, so they are named once here:

| manager offset | what it is | proof |
| --- | --- | --- |
| `mgr+0x4664` | index of the ball currently in play | `func_ov006_021128fc` compares each ball's own `mIndex` against it |
| `mgr+0x4668` / `mgr+0x4688[13]` | count and table of the tracked balls | swept by `cMgSmartball_board_c::SaveSnapshot` and `cMgSmartball_kinoko_c::SaveSnapshot` |
| `mgr+0x4670` / `mgr+0x46bc[]` | count and table of the spawned kinoko | written by `src/func_ov006_02115b0c.c`, read by `src/func_ov006_02112190.c` |

Each object's own `mpManager` is the manager pointer and its own `mIndex` is
its index into those tables. Both are base fields; both are named now (last
section).


## cMgSmartball_ball_c (`include/cMgSmartball_ball_c.h`)

Real ROM name confirmed by `tools/rtti_extract.py` (`build/rtti.json`): own
vtable ov006:0x0213ec98, RTTI ov006:0x0213ebec (`_ZTI19cMgSmartball_ball_c`),
`_ZTS19cMgSmartball_ball_c` at ov006:0x0213edc0. One of eleven direct children
of `cMgSmartball_object_c`. Size 0x12c, from `_Znwj(0x12c)` in
`func_ov006_02115b0c`; the base ends at 0x34, so this class adds 0xf8 bytes --
the densest of the eleven children.

28 of 29 struct members below have names; the sole `unk_` is 0x039. Six more
byte ranges are explicit `pad_` arrays (0x044, 0x0e7, 0x101, 0x111, 0x122,
0x12a) -- evidenced as gaps, not merely unexamined -- see the second table.

**Twelve of the 28 names are borrowed, not invented**: `state3a`, `state3b`,
`hit`, `hitA`, `hitB`, `hitC`, `anyHit`, `specialHit`, `nearby`, `targetIndex`,
`soundTimer`, `soundPlayed`. `func_ov006_02112ad8.c` and
`func_ov006_021128fc.c` -- two out-of-scope helpers `SaveSnapshot` calls with
`this` -- each reinterpret the pointer through their own local Obj-style
struct cast and name these exact offsets. Every one of them is also
independently touched by `RestoreInitial`'s exhaustive zero pass, so the width
and existence of each field is evidenced in-scope; only the spelling is
borrowed. Anything without that corroboration keeps an `unk_`/`pad_` name.

Constructed by `func_ov006_02114548`, a free function per the house recipe. It
calls the base constructor and writes only this vtable and the base's
`unk_028 = 0x8000`; it touches nothing at or past 0x34, so it adds no evidence
to the field list.

| offset | name | evidence |
| --- | --- | --- |
| 0x034 | `mZoneCooldown` | `src/_ZN19cMgSmartball_ball_c12SaveSnapshotEv.cpp` decrements it while >0 and refuses to re-latch `mUpperWallSolid` until it reads <= 0. A cooldown, not a lifetime: `RestoreInitial` zeroes it. |
| 0x038 | `mUpperWallSolid` | Set to 1 by `SaveSnapshot` when the ball is inside x [0xe8000,0xf0000) z [0x78000,0xa8000). Its only reader is `src/func_ov006_021126b4.c`, the out-of-bounds predicate, where a non-zero value makes it report z >= 0x78000 in the x band [0xd8000,0xe0000) as blocked. |
| 0x03a | `state3a` | Borrowed from `func_ov006_02112ad8.c`'s own Obj-style cast of this class; gates most of that function's per-frame collision work. Width and existence corroborated by `RestoreInitial`. |
| 0x03b | `state3b` | Ditto, same helper. Set once `mZoneDwell` (below) passes 0x1e, and by the same `func_ov006_02111d4c.c` that raises `mIsFrozen`. |
| 0x03c | `mZoneDwell` | `SaveSnapshot` counts it up while the ball is inside x [0x8000,0xd8000) z [0x74000,0x7c000), sets `state3b` past 0x1e, saturates at 0x12c, and resets it to 0 on the frame the ball leaves. A dwell counter, not a countdown. |
| 0x040 | `mExpireTimer` | Armed by `src/func_ov006_02111dcc.c`, which refuses to re-arm it while it is positive. `SaveSnapshot` ages it and, on expiry, clears the base's `mIsActive`; after that both `SaveSnapshot` and `Update` return immediately. |
| 0x04c | `hit[0x20]` | Borrowed from `func_ov006_02112ad8.c`/`func_ov006_021128fc.c`'s local Obj-style cast; width and existence corroborated in-scope by `RestoreInitial`'s exhaustive zero pass. Meaning of the 0x20-byte slot beyond the name is not evidenced. |
| 0x06c | `hitA[0x20]` | Same corroboration as `hit`. |
| 0x08c | `hitB[0x20]` | Same corroboration as `hit`. |
| 0x0ac | `hitC[0x20]` | Same corroboration as `hit`. |
| 0x0cc | `anyHit` | Same borrowed-naming/`RestoreInitial` corroboration as the `hit*` group. |
| 0x0cd | `specialHit` | Same corroboration; additionally, `SaveSnapshot` bails out of its per-frame work early while this is set. |
| 0x0ce | `nearby[0x19]` | Same borrowed-naming/`RestoreInitial` corroboration as the `hit*` group; ends at 0xe6, immediately before the `pad_0e7` gap below. |
| 0x0f8 | `targetIndex` | Set to -1 by `RestoreInitial`; begins immediately after the `pad_0e7` gap below. Borrowed from the same two helpers' Obj cast. |
| 0x0fc | `mCollisionCooldown` | Three frames of deafness after a ball-vs-ball hit. `src/func_ov006_02115830.c`, the collision resolver, returns without touching either velocity while EITHER participant's counter is positive, and sets BOTH to 3 on the way out of either path; `SaveSnapshot` ages it and `RestoreInitial` zeroes it. |
| 0x100 | `mIsWaiting` | 1 = still queued. `RestoreInitial` sets it; `SaveSnapshot` clears it once this ball's index equals `mgr+0x4664` and `mInPlay` is up. While set, `SaveSnapshot` runs `src/func_ov006_021128fc.c` (line up behind the predecessor in `mgr+0x4688`); once clear it runs the physics path `src/func_ov006_02112ad8.c`. |
| 0x104 | `mQueueGap` | `src/func_ov006_021128fc.c` writes it twice, both times as `(this ball's index - mgr[0x4664]) * 0xf`. Nothing reads it back -- the name records the arithmetic, which is unambiguous, not a use. |
| 0x108 | `mFrozenPos0` | Written only by `src/func_ov006_02111d4c.c`, which copies `mCurrent0` here. |
| 0x10c | `mFrozenPos1` | Same function, the z half. |
| 0x110 | `mIsFrozen` | Raised by the same `src/func_ov006_02111d4c.c` (which also sets `state3b`). While set, `SaveSnapshot` forces `mCurrent` back to `mFrozenPos`, zeroes the base's velocity pair and returns before any physics. Only `RestoreInitial` clears it. |
| 0x114 | `mLastPos0` | `SaveSnapshot` refreshes the pair from `mCurrent` whenever the ball is in bounds and has travelled 0x8000 or more from it. |
| 0x118 | `mLastPos1` | Same, the z half. |
| 0x11c | `mStuckFrames` | Incremented by `SaveSnapshot` when the ball is out of bounds or has not moved 0x8000; reset to 0 on a refresh. `src/func_ov006_02111df4.c` reports the ball as needing a rescue once it reaches 0x78 -- 120 frames, two seconds. |
| 0x120 | `mPipeUsed` | One-shot: this ball has already been taken by the pipe. `cMgSmartball_dokan_c::SaveSnapshot` sweeps the manager's ball table, skips any ball with it set, and -- for a ball in the pipe's mouth (within 0x8000 in x, -0x40000..-0x38000 in z) -- warps it, scores it and sets this. `RestoreInitial` zeroes it, so it is once per ball per round. |
| 0x121 | `mInPlay` | `src/_ZN19cMgSmartball_ball_c6UpdateEv.cpp` draws nothing while it is 0; `src/func_ov006_02111df4.c` reports a ball with it clear as finished; `src/func_ov006_021128fc.c` tests it on both this ball and its predecessor; `src/func_ov006_02111e7c.c` raises it. `RestoreInitial` clears it only for slot indices >= 9 on the second board. |
| 0x124 | `soundTimer` | Countdown, decremented while >0 by `SaveSnapshot`, zeroed by `RestoreInitial`. Borrowed from the same two helpers' Obj cast. |
| 0x128 | `soundPlayed` | Zeroed by `RestoreInitial`. Borrowed from the same two helpers' Obj cast. |
| 0x129 | `mExitGateOpen` | `SaveSnapshot` latches it once the ball has crossed the plane through the corner (0xd8000, -0x80000) by more than the base's `mRadius`. Until then `src/func_ov006_021126b4.c` reports z < -0x80000 as out of bounds, and `src/func_ov006_021122e0.c` gates its bottom-right exit test on it. |

Left `unk_`/`pad_`, deliberately:

| offset | why |
| --- | --- |
| 0x039 | Cleared next to `mUpperWallSolid` and by `RestoreInitial`. Nothing in the tree reads it. |
| `pad_044[0x8]` (0x044-0x04b) | `hitX`/`hitZ` in `func_ov006_02112ad8.c`'s borrowed naming, but none of this class's own four functions touches them, so per the `wing_c` precedent (see below) they stay an explicit pad -- unmodelled, not unread. |
| `pad_0e7[0x11]` (0xe7-0xf7) | A genuine gap: `RestoreInitial`'s exhaustive zero pass skips straight over it (`nearby[]` ends at 0xe6, `targetIndex` starts at 0xf8) and `func_ov006_02112ad8.c`'s Obj cast also treats it as padding. |
| `pad_101`, `pad_111`, `pad_122`, `pad_12a` | Pure alignment gaps between adjacent int fields (house style: explicit pads over implicit compiler-inserted ones). |

`mCollisionCooldown` and `mPipeUsed` were in this table until a later pass
found their readers. Both live OUTSIDE the class, reaching the ball through
the manager's table at `mgr+0x4688` rather than through `this`, and both spell
the field as a raw offset off that table entry -- so neither a read of the
class's own four functions nor a grep for the field name turns them up. That
is the search this family's remaining `unk_` fields should get before anyone
concludes they have no reader.


## cMgSmartball_slot_c (`include/cMgSmartball_slot_c.h`)

11 of 11 named. It is a three-reel slot machine, and the three out-of-scope
helpers are what say so.

| offset | name | evidence |
| --- | --- | --- |
| 0x034 | `mReelOffset[3]` (`.x`,`.y`) | `src/_ZN19cMgSmartball_slot_c6UpdateEv.cpp` adds `[i].x` to `mCurrent0` and `[i].y` to `mCurrent1` to place each sprite. `src/func_ov006_0210fb58.c` adds the speed into `.y` and wraps it at 0x30000 -- three symbols of 0x10000 -- which is what makes `.y` a reel position. |
| 0x04c | `mReelVel[3]` (`.x`,`.y`) | `src/func_ov006_0210fb58.c` ramps `.y` up toward 0x3000 while `mIsSpeedingUp`, down to 0x200/0x800 while `mIsSlowingDown`, and adds it into `mReelOffset[i].y` every frame. `.x` is only ever zeroed. |
| 0x064 | `mReelStopCount[3]` | `src/func_ov006_0210fa6c.c` rolls each entry to a random 3..5 when the stop begins; `src/func_ov006_0210fb58.c` spends one per symbol boundary the reel crosses and, at zero, snaps the reel to the boundary and zeroes its speed. |
| 0x070 | `mReelStopping[3]` | `src/func_ov006_0210fb58.c` sets it when that reel's speed bottoms out at 0x800; `src/func_ov006_0210fa6c.c` clears all three. It is the precondition for a reel actually being allowed to stop. |
| 0x073 | `mIsSpinning` | Raised by `src/func_ov006_0210fb04.c`, cleared by `src/func_ov006_0210fb58.c` once every reel has settled. `src/_ZN19cMgSmartball_slot_c12SaveSnapshotEv.cpp` will not start a spin while it is set. |
| 0x074 | `mIsSpeedingUp` | Raised with `mIsSpinning` by `src/func_ov006_0210fb04.c`; `src/func_ov006_0210fb58.c` clears it once all three reels have reached 0x3000. |
| 0x075 | `mIsSlowingDown` | Raised by `src/func_ov006_0210fa6c.c`; while set, `src/func_ov006_0210fb58.c` brakes the reels and counts `mReelStopCount` down. |
| 0x078 | `mSpinsQueued` | `SaveSnapshot` spends one and starts a spin when `mIsSpinning` is clear and `mSpinCooldown` has drained. `src/func_ov006_0210fb04.c` hands the credit straight back -- `++*(int*)(self + 0x78)` -- if a spin was already running, which is what makes this a count of owed spins rather than a timer. |
| 0x07c | `mSpinCooldown` | Set to 0x3c by `src/func_ov006_0210fb58.c` when a spin finishes, aged by `SaveSnapshot`, and blocks the next spin while positive. |
| 0x080 | `mSpinTimer` | Armed with 0xb4 by `src/func_ov006_0210fb04.c` at the start of a spin; `SaveSnapshot` ages it and calls `src/func_ov006_0210fa6c.c` on it reaching exactly 0. |
| 0x084 | `mSoundHandle` | `src/func_ov006_0210fb58.c` does `*(int*)(c+0x84) = Sound_PlayIfNotActive(*(int*)(c+0x84), 2, 0x1a6, 0)` -- the handle fed back in as its own first argument -- and clears it when the loop should stop. |


## cMgSmartball_board_c (`include/cMgSmartball_board_c.h`)

7 of 7 named. A 3x3 board of flip-over cells plus the eight lines through it.

| offset | name | evidence |
| --- | --- | --- |
| 0x034 | `mCellsTail[6]` | The tail of the nine per-cell claimed flags that begin at the base's `unk_031`; `src/func_ov006_0210ef48.c` models the same nine bytes as `u8 cells[9]` at 0x31 in its own shadow struct and sets an entry to 1 when a ball claims that cell. Never indexed through this name -- see the header for why the array is reached as `(&unk_031)[i]`. |
| 0x03c | `mCellFlipAngle[9]` | `SaveSnapshot` advances a claimed cell's entry by 0x400 a frame and clamps at 0x8000, a half turn; `Update` feeds it to the sine table `data_02082214` to shear the sprite and picks a flipped-face sprite past 0x4000. |
| 0x060 | `mCellHighlight[9]` | `Update` sets it for each of a scoring line's three cells -- `{i,i+3,i+6}`, `{3k..3k+2}`, `{0,4,8}`, `{2,4,6}` -- and blinks those cells off `mFanfareTimer`. |
| 0x06c | `mLineTimer[8]` | `src/func_ov006_0210ef48.c` arms the completed line's entry with 0x21d; `SaveSnapshot` ages each one and `Update` draws that line's banner while it is positive. |
| 0x08c | `mLineScored[8]` | Set alongside the timer by `src/func_ov006_0210ef48.c` and never cleared until `RestoreInitial`. `Update` keeps drawing the banner for such a line after its timer expires while the manager's 0x4660 flag is up -- so it is the persistent "this line was completed", not a second timer. |
| 0x094 | `mFanfareTimer` | Armed with 0xb5 by `src/func_ov006_0210ef48.c` on every line completion. `SaveSnapshot` spends it and replays a jingle once a second (`% 60`); `Update` reads bits 0x3c of it to blink. |
| 0x098 | `mLinesScored` | `src/func_ov006_0210ef48.c` increments it once per line it completes; `SaveSnapshot` switches on it to pick one of four jingles. A count, not a timer -- nothing ever decrements it. |


## cMgSmartball_kinoko_c (`include/cMgSmartball_kinoko_c.h`)

3 of 3 named.

| offset | name | evidence |
| --- | --- | --- |
| 0x034 | `mVariant` | Written exactly twice in the tree, both by the spawner `src/func_ov006_02115b0c.c`: objects built from the first tilemap layer get 0, those from the second get 1. The constructor `src/func_ov006_02111b40.c` leaves 2, which is the value `Update`'s switch has no case for. Never assigned at runtime, so it is a kind, not a state. |
| 0x038 | `mWasHit` | Raised by `src/func_ov006_02112190.c`, the ball-vs-kinoko proximity test, on the object it hit (reached through `mgr+0x46bc`). `SaveSnapshot` consumes it: fires the effect and sound, clears the flag and re-arms `mHitTimer`. |
| 0x03c | `mHitTimer` | `SaveSnapshot` ages it and, in its last four frames, sweeps `mgr+0x4688` for a live ball within 0x11000 -- re-arming to 4 while one is still resting on the mushroom. For a variant-1 mushroom it also drives the base's `mRadius` down by 0x1000 a frame while it runs and back to 0x7000 once it drains, which is the squash `Update` turns into a render scale. |


## cMgSmartball_wing_c (`include/cMgSmartball_wing_c.h`)

2 of 5 named. All five are zeroed by `RestoreInitial`, but only two are ever
read back, and neither reader is a method of this class.

| offset | name | evidence |
| --- | --- | --- |
| 0x040 | `mTriggerCount` | Incremented by `src/func_ov006_0210d8bc.c`, which `cMgSmartball_pakkun_c::SaveSnapshot` calls on this object (reached as `mgr+0x4780`) each time the piranha takes a ball. Three readers agree it is a count and not a flag: this class's `SaveSnapshot` eases the angle only while it is > 0, its `Update` draws the two extra base sprites only while it is <= 1, and `src/func_ov006_0210dbb0.c` -- the wing's collision routine -- runs its body test only while it is <= 1. `func_ov006_0210d8bc` itself branches on the value reaching exactly 1, playing sound 0x1a1 the first time and 0x1a2 after. |
| 0x044 | `mAngleSettled` | Raised by `src/func_ov006_021156f8.c`, the manager's "is the table at rest" check, once `src/func_ov006_0210d898.c` reports this wing done -- which is either `mTriggerCount == 0` or the angle at 0x32 having reached its 0x3000 limit. Its one reader is this class's `SaveSnapshot`, which stops easing the angle while it is set. |

Left `unk_`, deliberately:

| offset | why |
| --- | --- |
| 0x034 | `src/func_ov006_0210d8bc.c` sets it to 0x14 on the call that takes `mTriggerCount` to 1 and to 0 on every later call; `RestoreInitial` zeroes it. Nothing reads it back. The shape suggests a timer; a suggestion is not evidence. |
| 0x038 | Zeroed by the same call and by `RestoreInitial`. No reader. |
| 0x03c | Set to 1 by the same call, zeroed by `RestoreInitial`. No reader. |


## cMgSmartball_propeller_c (`include/cMgSmartball_propeller_c.h`)

2 of 2 named. The pair is a classic ease-toward-target, and the ease is the
whole evidence: one field is stepped toward the other.

| offset | name | evidence |
| --- | --- | --- |
| 0x034 | `mSpinSpeedTarget` | The value `mSpinSpeed` is stepped toward. `RestoreInitial` is its only writer in the tree and sets it to 0x40. "Target" is what the ease relationship in `SaveSnapshot` proves; nothing in the tree assigns it a role beyond that. |
| 0x036 | `mSpinSpeed` | `src/_ZN24cMgSmartball_propeller_c12SaveSnapshotEv.cpp` steps it by +-8 toward `mSpinSpeedTarget`, clamping on overshoot in either direction, and then ADDS it onto the base's angle at 0x32 every frame -- which is what makes it an angular speed and not an angle. `RestoreInitial` zeroes it. |

Note for anyone editing the bodies: `SaveSnapshot` deliberately does NOT spell
these two by name. Named member access there compiles to 0x98 bytes against a
required 0xac, because mwcc CSEs the repeated load; the raw-cast shape is
load-bearing. `Update` and `RestoreInitial` are unaffected -- see the header.


## cMgSmartball_pushswitch_c (`include/cMgSmartball_pushswitch_c.h`)

1 of 1 named.

| offset | name | evidence |
| --- | --- | --- |
| 0x034 | `mReleaseDelay` | How long the switch stays down once nothing is holding it. `src/func_ov006_0210e1fc.c` -- the ball-vs-switch proximity test -- presses the switch: it sets the base's `unk_031`, arms this field with 0xa, starts the slot machine (`func_ov006_0210fb04` on `mgr+0x4778`) and plays sound 0x1a4. `SaveSnapshot` then ages it while positive and, only while `unk_031` is set, calls `src/func_ov006_0210e120.c`, which returns 1 exactly when NO active ball is within 0x12000 of the switch. Clear and drained to zero -> `unk_031` is cleared and the switch pops back out; a ball still near -> the field is reloaded to 0xa. So the ten frames are counted from the last frame a ball was on it, not from the press. `RestoreInitial` zeroes it. |


## cMgSmartball_ana_c (`include/cMgSmartball_ana_c.h`)

1 of 1 named. `ana` is the hole a ball drops into.

| offset | name | evidence |
| --- | --- | --- |
| 0x034 | `mRespawnTimer` | Armed with 0x3c -- one second -- by `src/func_ov006_021115cc.c`, which `src/func_ov006_02112ad8.c` (the ball physics) calls on this hole the frame a ball falls in; the same call raises the base's `unk_031` and plays the effect. `SaveSnapshot` ages it and, on the frame it reaches zero, calls `src/func_ov006_02114ec0.c` on the manager -- which walks the ball table and puts the first active ball not yet in play INTO play -- and sets the base's `unk_032`. So the field is the delay between losing a ball down this hole and being served the next one. `Update` treats it as exactly that: while `unk_031` is set it picks one of three warning sprites at the 0x28 and 0x14 thresholds, i.e. the countdown is shown to the player. `RestoreInitial` zeroes it. |


## cMgSmartball_spring_c (`include/cMgSmartball_spring_c.h`)

3 of 5 named.

| offset | name | evidence |
| --- | --- | --- |
| 0x038 | `mDriveBase` | Captured at the trigger frame from the same table as `mDriveNow`, less however far `mCurrent1` already sat above the 0xa0000 rest height. Every active frame `src/_ZN21cMgSmartball_spring_c12SaveSnapshotEv.cpp` recomputes `mCurrent1 = mDriveNow - mDriveBase + 0xa0000`, so this is the zero point the live reading is measured from. |
| 0x040 | `mDriveNow` | `data_020a0deb[level*4] << 12`, re-read every active frame and clamped to `[mDriveBase, mDriveBase + 0x38000]` before it sets the height. |
| 0x044 | `mSoundHandle` | `mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 2, 0x16c, 0)` -- the handle fed back in as its own first argument. The only field this class's `RestoreInitial` zeroes. |

Left `unk_`, deliberately: 0x034 and 0x03c, and the outside-the-class search
was redone for both after it turned up readers for two ball fields that had
been written off. The spring is the object at `mgr+0x4684`, and every file that
reaches that slot was read: `src/_ZN16dScMgSmartball_c8BehaviorEv.cpp` and
`src/_ZN16dScMgSmartball_c6RenderEv.cpp` only call its virtuals,
`src/func_ov006_02115b0c.c` and
`src/_ZN16dScMgSmartball_c21AfterCleanupResourcesEj.cpp` only build and free it,
and `src/func_ov006_0211248c.c` only reads its position. None of them touches
0x34 or 0x3c. `SaveSnapshot` sets `unk_03c` from
`data_020a0dea[level*4] << 12` in the trigger branch and copies it to
`unk_034`; nothing in the tree ever reads either back. Their width and their
one write are evidenced, their meaning is not.


## The base, cMgSmartball_object_c -- renamed

6 of 9 named. The rename landed in one commit across the base header, all
eleven child headers and the fifteen `.cpp` bodies that reach these fields by
name; the whole ROM still reproduces (11,059 / 11,059, module fidelity
106/106).

Nothing outside `src/` spells these fields: the family has no `src_tu/` or
`port/` presence, and every `.c` file in it (the eleven constructors and the
free helpers) reaches the object by raw offset off a `char *`, so a rename
cannot reach them. That was checked by grep before the rename, not assumed.

Before using any of these names again, note that `cMgSmartball_object_c` is a
ROOT -- it has no base whose members a derived name could shadow. The check
that mattered here was the other direction: none of the eleven children
already declared any of the six identifiers (grep over
`include/cMgSmartball_*.h` returned nothing), so no child field was silently
rebound.

| offset | name | evidence |
| --- | --- | --- |
| 0x004 | `mpManager` | Every child dereferences it as the `dScMgSmartball_c` that owns the object: `mgr+0x4664`, `mgr+0x4668`, `mgr+0x4670`, `mgr+0x4688`, `mgr+0x46bc`, `mgr+0x595c`, `mgr+0x595d`. Kept declared `s32`: the manager has no header in this tree and each of the fifteen readers writes its own cast, so widening the declaration to a pointer would state more than the evidence does. |
| 0x020 | `mVel0` | `cMgSmartball_ball_c::SaveSnapshot` does `mCurrent0 += mVel0; mCurrent1 += mVel1;` -- and passes `&mVel0` to `Vec2_Len` as a 2-vector, i.e. the pair is contiguous and is a velocity, not two unrelated scalars. |
| 0x024 | `mVel1` | Same statement; also the value `cMgSmartball_spring_c::SaveSnapshot` eases `mCurrent1` back to rest with. |
| 0x028 | `mRadius` | Each child's constructor writes its own constant (0x8000 ball, 0x14000 pakkun, 0x18000 board, 0x20000 dokan/propeller/spring, 0x7000 kinoko) and both readers use it as a distance: `cMgSmartball_ball_c::SaveSnapshot` compares a signed plane distance against `-mRadius`, and `cMgSmartball_kinoko_c` eases it to 0 and back to 0x7000 as the mushroom squashes, with `Update` turning it into a render scale. |
| 0x02c | `mIndex` | `src/func_ov006_021128fc.c` compares it against `mgr+0x4664` and indexes `mgr+0x4688` with it minus one. |
| 0x030 | `mIsActive` | The constructor sets it to 1. `cMgSmartball_ball_c::SaveSnapshot` and `::Update` return immediately while it is 0, `src/func_ov006_02111dcc.c` refuses to arm an expiry on an object with it clear, and both `cMgSmartball_board_c::SaveSnapshot` and `cMgSmartball_kinoko_c::SaveSnapshot` skip a tracked ball whose byte at 0x30 is 0. |

Left `unk_`, deliberately:

| offset | why |
| --- | --- |
| 0x031 | A 0/1 flag to eight readers, an array index to `cMgSmartball_pushswitch_c::Update`, and the FIRST CELL of `cMgSmartball_board_c`'s nine-byte cell array. No one name covers those three. |
| 0x032 | The low byte of a 16-bit angle to wing (signed) and propeller (unsigned), an independent flag to ana, a board cell to board. Same problem, and the two angle readers disagree with each other on signedness. |
| 0x033 | The high byte of that angle; a board cell. |

0x031/0x032/0x033 stay three `u8`s -- see `include/cMgSmartball_object_c.h`,
which records why three children read the same three bytes three incompatible
ways. The three fields with names in the first pass (`mCurrent0/1`,
`mSnapshot0/1`, `mInitial0/1`) are unchanged.


## What is still `unk_`, and why, in one place

Nine `unk_` fields across the family, plus two unmodelled pad ranges. Every
one of the nine has had the outside-the-class search applied -- the search that
found readers for `cMgSmartball_ball_c`'s 0x0fc and 0x120 after both had been
written off. The two pad ranges are a different case and are listed with them
so the table is the whole picture.

| class | offsets | why |
| --- | --- | --- |
| `cMgSmartball_object_c` | 0x031, 0x032, 0x033 | Not missing evidence -- too much of it. Three children read the same three bytes three incompatible ways (a flag, a byte of a 16-bit angle whose two readers disagree on signedness, a cell of a nine-byte board array). No one name is right for all of them. |
| `cMgSmartball_ball_c` | 0x039 | Cleared beside `mUpperWallSolid` and by `RestoreInitial`. No reader anywhere in the smartball function range. |
| `cMgSmartball_wing_c` | 0x034, 0x038, 0x03c | All three written by `src/func_ov006_0210d8bc.c` on the call that bumps `mTriggerCount`, all three zeroed by `RestoreInitial`, none read back. |
| `cMgSmartball_spring_c` | 0x034, 0x03c | Latched together once in the trigger branch and never read; every file reaching the spring's slot at `mgr+0x4684` was checked. |
| `cMgSmartball_propeller_c` | 0x038..0x077 | A pad, and unmodelled rather than unread: `src/func_ov006_02110e28.c` read-modify-writes eight `(x, y)` pairs there through its own shadow struct. Naming them is that helper's job, not this header's. |
| `cMgSmartball_wing_c` | 0x045..0x087 | The same shape, with `src/func_ov006_0210d93c.c` as the helper. |

`cMgSmartball_slot_c`, `cMgSmartball_board_c`, `cMgSmartball_kinoko_c`,
`cMgSmartball_pakkun_c`, `cMgSmartball_pushswitch_c` and `cMgSmartball_ana_c`
have no `unk_` fields left. `cMgSmartball_dokan_c` never had any -- it adds no
fields at all.
