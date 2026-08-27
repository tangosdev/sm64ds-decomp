# Butterfly, Tornado, StarMarker, ToxBox -- field-name provenance

Four standalone actors whose headers came out of `tools/gen_header.py` with
observed offsets and placeholder names. The offsets and widths were already
evidence; what follows is where each NAME came from. Every citation is a file
in this tree and says what it does with the field.

Naming is byte-neutral: every function listed here still reproduces the
cartridge under 2004/b56, checked per function with `build_pin`'s `verify`
(the tool lives in `tools/build_pin.py`) and then with a whole-ROM
`tools/rombuild.py` run.

## A rule these four share

Tornado and StarMarker remain flat shadow structs that restate `dActor_c`'s
bytes. Butterfly has since been promoted to genuine `Butterfly : dActor_c`
class form; its inherited fields therefore come directly from
`include/dActor_c.h` rather than being repeated locally.

Where `dActor_c` itself still says `unk_`, so does the shadow: `Butterfly`
writes 0x0a4/0x0a8/0x0ac together as an (x, vertical, z) velocity triple,
which is a good lead that `dActor_c::unk_0a4` and `unk_0ac` are the horizontal
halves of a velocity -- but naming them in a derived shadow while the base
they mirror contradicts it would be worse than leaving them alone. That one is
banked for a pass that owns `include/dActor_c.h`.


## Butterfly (`include/Butterfly.h`)

Actor 0x150. The puzzle where three butterflies flutter around and one of them
turns into a 1-Up.

`mState` is a DISPATCH INDEX, and that is the key that opened the rest of the
class. `Butterfly::Behavior` reads it, looks
`data_ov100_02148628[mState]` up as a pointer-to-member-function and calls it.
Every one of the eight state methods between `_ZN9ButterflyD0Ev`
(0x02140dd8) and `_ZN9Butterfly16CleanupResourcesEv` (0x02141988) in
`config/arm9/overlays/ov100/symbols.txt` is therefore one of this class's own
states. The ROM does not retain their descriptive source names, so they are
named `Butterfly::State0` through `State7` from their exact PMF-table indices.

| offset | name | evidence |
| --- | --- | --- |
| 0x080/0x084/0x088 | `mScaleX/Y/Z` | `dActor_c`'s own offsets. `src/game/actors/Butterfly/_ZN9Butterfly8BehaviorEv.cpp` writes all three from `mScale` every frame. |
| 0x08e | `mAngleY` | `dActor_c`'s offset. Behavior copies `mPrevAngleY` here and builds the render matrix from it. |
| 0x092 | `mPrevAngleX` | `dActor_c`'s offset. `Butterfly::State5` eases it toward -0x2000 or 0x2000 to make the butterfly climb or dive. Its sine-table users deliberately load through `u16 *`. |
| 0x094 | `mPrevAngleY` | `dActor_c`'s offset; the heading, eased toward `mWanderAngle`, toward home, or toward the player depending on state. |
| 0x098 | `mHorzSpeed` | `dActor_c`'s offset. |
| 0x0a8 | `mVertSpeed` | `dActor_c`'s offset. |
| 0x3d4/0x3d8/0x3dc | `mHomePosX/Y/Z` | `src/game/actors/Butterfly/_ZN9Butterfly13InitResourcesEv.cpp` copies `mPos` here before anything moves. `State3` and `State2` snap `mPos` back to it; `State5` takes `Vec3_HorzAngle(mPos, mHomePos)` as the heading home. |
| 0x3e0 | `mScale` | InitResources sets 0x1000 and Behavior copies it into all three scale words. `State6` winds it from 0 up to 0x800 in 0x40 steps, or drops it to 0 for a butterfly that is not kind 1; `State7` adds a sine-table wobble driven by `mFlutterPhase`. |
| 0x3e4 | `mState` | The dispatch index above. InitResources sets 0, 1 or 4; 4 is inert (Behavior skips the matrix work and `src/game/actors/Butterfly/_ZN9Butterfly6RenderEv.cpp` draws nothing). |
| 0x3e8 | `mStateTimer` | Seeded to a random 0..99 by InitResources, then zeroed by every state that hands over. States compare it against 0x14, 0x3c, 0x6e, 0x78, 0x9d and 100. |
| 0x3ec | `mWanderAngle` | `State4` rolls a random angle into it at spawn and seeds `mPrevAngleY` from it; after 0x3c frames `State5` steers toward it instead of toward home. |
| 0x3ee | `mFlutterPhase` | `State7` advances it by 0x2710 or 0xfa0 a frame and feeds it to the sine table to pump `mScale`. Zeroed by `State6`. |
| 0x3f0 | `mKind` | `State4` writes `param1 >> 6` here, and spawns three more of actor 0x150 with a kind in bits 0xc0 -- one of them kind 1. Only kind 1 becomes actor 0x114 in `State6`; `State5` tests `(u8)(mKind + 0xff) <= 1`, i.e. kind 1 or 2. Behavior skips all movement while this is 0, which is the spawner's own value. |
| 0x3f1 | `mUseAnimModel` | Behavior and Render both branch on it: 1 draws `mModelAnim` with `mShadowModel1` and advances the animation, 0 draws `mModel` with `mShadowModel2` and does not. Set by InitResources, cleared by `State6` alongside `mScale = 0`. |

0x3d4..0x3dc, 0x3e8, 0x3ec and 0x3ee were inside `pad_3d4` and `pad_3e8`
before this pass; the writes above are what took them out of the padding.


## Tornado (`include/Tornado.h`)

`Tornado::Behavior` switches on `mState` and calls one of three free
functions, all inside this class's own address range:

- 0 `src/game/actors/Tornado/_ZN7Tornado6State0Ev.cpp` -- dormant at home.
- 1 `src/game/actors/Tornado/_ZN7Tornado6State1Ev.cpp` -- hunting.
- 2 `src/game/actors/Tornado/_ZN7Tornado6State2Ev.cpp` -- winding down.

| offset | name | evidence |
| --- | --- | --- |
| 0x09c | `mVertAccel` | `dActor_c`'s offset; `src/game/actors/Tornado/_ZN7Tornado13InitResourcesEv.cpp` sets -0x1000. |
| 0x0a0 | `mTerminalVelocity` | `dActor_c`'s offset; InitResources sets -0x1e000. |
| 0x33c | `mCaughtActor` | `src/game/actors/Tornado/_ZN7Tornado8BehaviorEv.cpp` resolves `mdCcAc_c.otherOwner` to an actor and stores it here once `func_ov002_020de33c` approves. State 1 re-tests it through `func_ov002_020de328`; state 2 clears it. |
| 0x340/0x344/0x348 | `mHomePosX/Y/Z` | InitResources copies `mPos` here. State 0 snaps `mPos` back to it; states 1 and 2 measure every distance from it rather than from where the tornado is. |
| 0x34c | `mChaseRange` | InitResources builds it from `mParam & 0xff`: `byte * 0x64000`, or 0x5dc000 when the byte is 0xff. State 1 chases only while the player is within it of `mHomePos`. |
| 0x350 | `mStateTimer` | Behavior counts it up every frame and zeroes it when `mState` changed. State 0 spins up over its first 0x3c, state 2 shrinks over its own 0x3c and gives up at 0x168. |
| 0x354 | `mChaseTimer` | Counted up by state 1 only. At 0x384 the chase ends: state 1 drops to state 2, and state 0 uses the same threshold to decide it must teleport home. |
| 0x356 | `mAngleToHome` | State 1 sets it to `Vec3_HorzAngle(mPos, mHomePos)` every frame and steers toward it when not chasing. |
| 0x358 | `mAngleToPlayer` | The same angle to the closest player, steered toward while chasing. |
| 0x35c | `mState` | The switch in Behavior. |
| 0x360 | `mTriggerCount` | An event counter both live states use as a latch, zeroed by Behavior on any state change. State 0 bumps it when a player comes within 0x5dc000 and only spins up while it is non-zero; state 1 bumps it when `mCaughtActor` satisfies `func_ov002_020de328` and stops chasing once it is non-zero. |
| 0x364 | `mParticleHandle0` | `Particle::System::New`'s return for effect 0x11f, fed back in as its own first argument by state 1. |
| 0x368 | `mParticleHandle1` | The same for effect 0x120. |
| 0x36c | `mSoundHandle` | `Sound::PlayLong`'s return for sound 0x85, fed back the same way by states 0 and 1. Was the header's trailing pad; `Tornado_Spawn` allocates 0x370, so it is the last word. |

0x352 and 0x35a keep `unk_` names: the first is only ever zeroed by state 0,
the second only by InitResources, and nothing in the tree reads either.
0x354/0x356/0x358 came out of `pad_352`.


## StarMarker (`include/StarMarker.h`)

The on-screen glint showing where an uncollected star will appear.

| offset | name | evidence |
| --- | --- | --- |
| 0x004 | `mUniqueID` | `fBase_c`'s own offset. `src/game/actors/StarMarker/_ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv.cpp` hands it to the star it spawns, at `star+0x434`. |
| 0x18c | `mShadowMtx` | `src/game/actors/StarMarker/_ZN10StarMarker8BehaviorEv.cpp` stores `IDENTITY_MATRIX4X3` over `*(Mtx *)&mShadowMtx` -- a whole Matrix4x3, 0x18c..0x1bb -- and passes `&mShadowMtx` to `dActor_c::DropShadowRadHeight` as the shadow's matrix. |
| 0x1b0/0x1b4/0x1b8 | `mShadowMtxTX/TY/TZ` | The translation row of that same matrix (+0x24/+0x28/+0x2c), set to `mPos >> 3` in the statements right after the identity store. |
| 0x1bc/0x1c0/0x1c4 | `mSpawnPosX/Y/Z` | `src/game/actors/StarMarker/_ZN10StarMarker13InitResourcesEv.cpp` copies `mPos` here. Nothing in the tree reads them; the name records the copy, which is unambiguous. |
| 0x1c8 | `mGroundY` | InitResources raycasts down with a `dBgCh_Gnd` from `mPosY + 0x1e000` and stores the result's own +0x44 here. Behavior turns `mPosY - mGroundY` into the shadow's drop height. |
| 0x1cc | `mSpawnedActorID` | A unique id, not a pointer: `src/game/actors/StarMarker/_ZN10StarMarker16OnPendingDestroyEv.cpp` feeds it to `dActor_c::FindWithID` and, if that actor has no death-table slot of its own, clears `mSpawnedDeathTableID`'s bit. InitResources zeroes it and nothing in the tree sets it non-zero, so the write side is still missing. |
| 0x1d0 | `mHitActor` | Behavior resolves `mdCcAcPos_c.otherOwner` to an actor and stores it here just before calling `func_ov002_020e7d84`. |
| 0x1d6 | `mSpawnedDeathTableID` | The slot `DeathTable_ClearBit` is called on in OnPendingDestroy. InitResources sets -1, the same "no slot" value `dActor_c` uses for its own `mDeathTableID`. |


## ToxBox (`include/ToxBox.h`)

The rolling crush box. `mParam & 3` picks how it moves.

| offset | name | evidence |
| --- | --- | --- |
| 0x320 | `mPlayerActor` | `src/unnamed/ov092/0213/func_ov092_021319b0.cpp` resolves the collision id at 0x50c to an actor, keeps it only if its actorID is 0xbf, and stores it here. `src/unnamed/ov092/0213/func_ov092_021311b0.cpp` uses its position as the epicentre of the landing earthquake and then clears this to 0. |
| 0x528 | `mBaseMtx` | `src/game/actors/ToxBox/_ZN6ToxBox13InitResourcesEv.cpp` copies `mModel`'s own matrix at 0xf0 into it as a whole `Matrix4x3`; `src/unnamed/ov092/0213/func_ov092_02131aec.cpp` copies it back out again. The untumbled base transform. |
| 0x558/0x55c/0x560 | `mRestPosX/Y/Z` | InitResources copies `mPos` here after lifting `mPosY` by 0xfa000; `src/unnamed/ov092/0213/func_ov092_021311b0.cpp` refreshes `mRestPosY` from `mPosY` every time the box lands. |
| 0x568 | `mMoveDir` | `src/unnamed/ov092/0213/func_ov092_021314d0.c` reads it out of `mMoveSeq`; `src/unnamed/ov092/0213/func_ov092_021313b0.cpp` derives it from the horizontal angle to the next path node (2/3/4/5 for the four quadrants, 1 when the node did not move); `src/unnamed/ov092/0213/func_ov092_021311b0.cpp` overrides it with 7 or 8 for the two special floor types, and `src/unnamed/ov092/0213/func_ov092_02131878.c` with 6. |
| 0x56c | `mMoveSeq` | InitResources sets it to `((int **)&data_ov092_02132294)[mMoveKind]`, and reads `*mMoveSeq` as the first direction. `src/unnamed/ov092/0213/func_ov092_021314d0.c` walks it and treats a 0 entry as the end, wrapping to the first. |
| 0x570 | `mMoveSeqIndex` | The index `src/unnamed/ov092/0213/func_ov092_021314d0.c` advances into `mMoveSeq` and resets to 0 on the wrap. |
| 0x574 | `mMoveKind` | `mParam & 3`. 0..2 pick a canned sequence, 3 makes InitResources load a `PathPtr` instead; both `src/unnamed/ov092/0213/func_ov092_021314d0.c` and `src/unnamed/ov092/0213/func_ov092_021316d8.c` branch on `== 3`. |
| 0x575 | `mOrientBits` | Three 2-bit fields packed from the three rotation angles (X >> 0xe, Y >> 0xc, Z >> 0xa). InitResources builds it from the spawn rotation, `src/unnamed/ov092/0213/func_ov092_021314d0.c` rebuilds it after every roll, and `src/unnamed/ov092/0213/func_ov092_021316d8.c` and `src/unnamed/ov092/0213/func_ov092_02131878.c` read it back. |
| 0x578 | `mPathNodeCount` | `PathPtr::NumNodes` for `mPathPtr`, and the wrap bound in `src/unnamed/ov092/0213/func_ov092_021313b0.cpp`. |
| 0x57c | `mPathNodeIndex` | The node the box heads for; `src/unnamed/ov092/0213/func_ov092_021313b0.cpp` advances it and wraps at `mPathNodeCount`. |
| 0x580/0x584/0x588 | `mPathNodeX/Y/Z` | The `Vector3` `PathPtr::GetNode` fills in for `mPathNodeIndex`. `src/unnamed/ov092/0213/func_ov092_021313b0.cpp` keeps the previous one on the stack and takes the horizontal angle between the two to pick `mMoveDir`. Came out of `pad_580`. |
