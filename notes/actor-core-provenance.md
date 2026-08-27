# Actor core: provenance

Everything here was moved out of `include/dActor_c.h`, `include/fBase_c.h` and the
`dActor_c`/`fBase_c` constructor sources so that those files read like game source
rather than a lab notebook. Nothing was deleted: the matching constraints those
essays carried are still stated, condensed, at their site in the header, and the
long form is below.

See also `notes/actor-vtables.md`, `notes/mwccarm-codegen.md`, `notes/actor-naming.md`.

---

## 1. `fBase_c` -- the range in arm9

`fBase_c`'s code is one contiguous run, `0x02043444..0x02043f4c`, 25 functions.

That range is a correction. The banner used to say `0x02043494..0x02043e04` and
both ends were wrong; `src_tu/actors/ActorBase.cpp` reconciled it against the
cartridge while rebuilding the translation unit.

* `0x02043444` is the real start: `_ZN7fBase_cnwEj`, this class's own
  `operator new` (size `0x50`). The old `0x02043494` began at `OnHeapCreated`
  and excluded it.
* `0x02043f4c` is the real end -- the byte after `_ZN7fBase_cC2Ev`, and the
  address of `func_02043f4c`, the next unrelated function.
* `0x02043e04` was not a function boundary at all. It falls `0x18` bytes INSIDE
  the constructor (`_ZN7fBase_cC2Ev`, `0x02043dec`, size `0x160`), so the old
  end cut that function in half.

The run is bracketed by unlabelled neighbours on both sides -- `func_020433b8`
ends at `0x02043444`, `func_02043f4c` begins at `0x02043f4c` -- and contains
exactly two unnamed functions, `func_02043810` and `func_02043880`, both enclosed
by named members and both reading this layout directly. 24 of the 25 are
byte-verified together as one TU; the constructor is the exception, and the only
source the tree has for it is a NONMATCHING hand-written asm transcription that
`config/arm9/delinks.txt` does not enrol.

Separately, the nested class `fBase_c::SceneNode` has two functions of its own
`0x8000` bytes away at `0x0203b4ac..0x0203b4dc`, sitting next to the intrusive-list
primitives this class calls. Different translation unit; not part of the run above.

## 2. `fBase_c` layout, read out of the ROM

`fBase_c::fBase_c` stores its vptr with `str r1, [r4]`, so the vptr is at `0x0`.
The same constructor does `add r5, r4, #0x14` and passes r5 to
`SceneNode::SceneNode`, then writes an owner back-pointer with `str r4, [r5, #0x10]`
-- which pins `sceneNode` at `0x14` and makes it `0x14` bytes, not the `0x10`
that `include/ActorBase__SceneNode.h` describes. It then initialises two identical
`0x10`-byte nodes at `r5+0x14` (`0x28`) and `r5+0x24` (`0x38`), the two the
destructor tears down in reverse order.

## 3. `fBase_c` vtable order

Read directly out of `_ZTV7fBase_c` (`0x02099edc`, 18 slots). Every slot resolves
to a named function, so no inference was needed here -- unlike `include/Fader.h`.
Two consequences, both easy to get wrong:

* The destructor is at slots 16/17, NOT 0/1. Slot index follows declaration
  order, so `~fBase_c` must be declared AFTER `OnHeapCreated`. Copying the Fader
  header shape, where the destructor comes first, shifts sixteen slots and
  silently changes every virtual call in the tree.
* `AfterCleanupResources` dispatches through `vtable+0x40`. That is slot 16, the
  D1 destructor -- not `OnPendingDestroy`, which is slot 12 (`vtable+0x30`, the
  slot `MarkForDestruction` uses). It destructs and then deallocates; calling
  that `OnPendingDestroy` would describe a leak.

## 4. `dActor_c` -- the chain, and why two vptr stores

The chain is `fBase_c -> dBase_c -> dActor_c`. `dActor_c` is NOT a direct child of
`fBase_c`: `dActor_c::dActor_c` calls `fBase_c::fBase_c`, stores `dBase_c`'s vptr,
then immediately overwrites it with its own. Two consecutive vptr stores is what
an inlined intermediate-base constructor looks like. See `notes/actor-vtables.md`.

`fBase_c` occupies `0x00..0x4f`, so `dActor_c`'s own fields begin at `0x50`. The
generated header this replaces duplicated `fBase_c`'s fields inline -- `uniqueID`
at `0x04`, `actorID` at `0x0c` -- instead of inheriting them, which is why it
opened with `pad_000[0x4]` and a `0x42`-byte gap.

## 5. `dActor_c` vtable, and the key-function rule

`_ZTV8dActor_c` (`0x0208e3a4`) has 31 slots. `dActor_c` overrides ten of the
eighteen it inherits -- 1, 2, 4, 5, 7, 8, 10, 11 and the destructor at 16/17 --
and appends thirteen of its own at 18..30. Slots 0, 3, 6, 9, 12, 13, 14 and 15
still point at the `fBase_c` implementations.

The destructor is declared FIRST on purpose. CW 1.2 emits the vtable into the TU
defining the first non-inline virtual declared in the class (the key function),
and that copy collides with the one the module's gap object supplies from ROM
data. An override takes its base's slot wherever it is declared, so putting
`~dActor_c` first costs nothing and makes it the key function. The thirteen NEW
virtuals still take 18..30 from their declaration order, because new slots append
after the inherited table.

What makes that safe is NOT that the destructor lives in a C translation unit --
`src/game/actors/dActor_c/_ZN8dActor_cD1Ev.cpp` and `_ZN8dActor_cD2Ev.cpp` are C++ and do include the
header; only `_ZN8dActor_cD0Ev.cpp` is C. The invariant is that all three define
`extern "C"` free functions under the mangled names and none defines
`dActor_c::~dActor_c`, so no TU is ever the key function's definition.

Stated precisely: the key function -- the first non-inline virtual declared --
must never be defined as a real method in any translation unit. Declaring the
destructor first pins that role to TUs which by construction never will.
`include/fBase_c.h` reaches the same end differently: it does declare
`InitResources` (slot 0) in-class, but `src/game/actors/fBase_c/_ZN7fBase_c13InitResourcesEv.cpp`
deliberately defines it as an `extern "C"` free function rather than a method. Do
not "fix" that file into a real method, and do not remove the declaration from
`fBase_c.h` -- removing it would delete slot 0 and shift all 18 slots.

## 6. `operator delete`, inline, on both classes

The compiler generates D0 -- the deleting destructor, vtable slot 17 -- as "run
the destructor body, then call `operator delete` on the class". Without an
in-class declaration it emits a call to the global `_ZdlPv`, which exists nowhere
in this image, and D0 comes out three instructions short of the ROM's.

Inline, and that is not a style choice, it is what the ROM shows.
`fBase_c::operator new` is a real function at `0x02043444`, but NO `operator
delete` symbol exists anywhere in the image, and every deleting destructor ends
with the same two instructions -- load the actor heap, call `Memory::Deallocate`
-- rather than a call to a shared helper. Only an inline member produces that.

Measured, not assumed. The ROM's two D0s under `fBase_c` are both exactly their
D1 body plus those instructions:

    fBase_c::~fBase_c [D0]  0x02043d78  0x44 = D1's 0x30 + 0x14
    dBase_c::~dBase_c [D0]  0x02013ea4  0x38 = D1's 0x24 + 0x14

Compiled without the declaration, `src_tu/actors/ActorDerived.cpp`'s D0 came out the
wrong SIZE (`999 word(s) differ`); with it, 5/5 MATCH.

Why it is declared on `dActor_c` as well as on `fBase_c`: mwcc inlines
`operator delete` only when it is found in the class itself or its IMMEDIATE
base. A declaration on `fBase_c` does NOT reach `dActor_c` (two levels down), nor
`HUD`, `Minimap` or `dScene_c`, whose immediate base is `dBase_c`. The only
classes `fBase_c`'s copy changes are `fBase_c` itself and `dBase_c` -- the two
whose D0 the ROM shows inlining it. `dEnemyBase_c` carries its own copy for the
same reason: it is a flattened struct that does not derive from `dActor_c` in
these headers. The two `src/` files that declare a local `struct dActor_c : fBase_c`
(`EndKuppaScript.cpp`, `func_ov002_020b7e1c.cpp`) use their own local shadow
`fBase_c`, not this one, so they are out of scope too.

No layout effect: an inline non-virtual member adds no field and no vtable slot,
and the `0x50` / `0xd0` size assertions still hold.

### 6a. Why the deallocator is declared with a `void *` second parameter

`_ZN6Memory10DeallocateEPvP4Heap` is spelt in `dActor_c.h` and `fBase_c.h`
EXACTLY as `include/decl_common.h` spells it, deliberately. The mangled name says
the second parameter is a `Heap*`, and `decl_common.h` says `void*`; declaring the
honest type in the actor headers instead makes two incompatible `extern "C"`
declarations of one name visible in the same translation unit, which mwcc rejects
as "illegal function overloading". That cost 105 files their eligibility when it
was tried. Correcting the parameter type is worth doing -- in `decl_common.h`,
once, for every caller at the same time.

### 6b. Why `operator new` is not declared in-class

CW 1.2 rejects an in-class declaration of `operator new` ("illegal 'operator'
declaration"), and it is neither virtual nor layout-affecting, so
`src/game/actors/fBase_c/_ZN7fBase_cnwEj.cpp` defines it under its mangled name instead.

## 7. `dActor_c` field widths -- the `0x080..0x0ab` block

`0x080..0x08b` and the `0x098..0x0ab` block were bare padding and `u8`
placeholders in `dActor_c.h`, while `Player.h` -- describing the same bytes --
named them and typed them `s32`. Player is right, and the evidence is outside
Player: `BooCage::InitResources` and `MadPiano::InitResources` write `-0x4000`
and `-0x2000` to `0x09c` and `-0x46000` / `-0x3c000` to `0x0a0`, which are fix12
gravity and terminal velocity, not bytes. `Player::St_Walk_Main` passes `0x098`
as a 32-bit argument.

These were deliberately left wrong for a long time: nothing compiled against
`dActor_c`'s copies, so no gate could prove a change either way. Once `Player`
inherits them, 62 files using `mHorzSpeed` and 49 using `mVertSpeed` resolve
through this header, and a wrong width fails immediately.

`0x0a4` and `0x0ac` were padding, annotated "likely the same physics block;
unproven". They are real, and `dEnemyBase_c` is the evidence: its generated header
declared both as `s32` and its sources read them, so once `dEnemyBase_c : dActor_c`
they have to exist. Still unnamed -- what they mean is not evidenced, only that
they are `dActor_c`'s and four bytes wide.

The `#else` C branch carried `pad_0a4`/`pad_0ac` long after the C++ branch above
promoted them to real `s32` fields -- a drift no gate could catch, because until
`include/Door.h`'s own C branch nested this struct, NO C translation unit in the
tree included the header at all and the whole `#else` was dead. The two spellings
now agree field for field.

## 8. `dActor_c` field names recovered in this pass

Named from matched bodies only. See `include/dActor_c.h` for the short form.

| offset | name | evidence |
| --- | --- | --- |
| `0x050` | `mListPrev` | `dActor_c::dActor_c` passes `this + 0x50` to `func_0203b244`, the append primitive, with the global actor list `data_0209b468`; that primitive's node layout is `{prev@0, next@4}`. `~dActor_c` passes the same address to `func_0203b27c` (unlink). |
| `0x054` | `mListNext` | `dActor_c::Next(const dActor_c *)` reads `self->0x54` to get the next NODE, then dereferences `node + 8` to recover that node's actor. Forward link, therefore, is `+4`. |
| `0x058` | `mListOwner` | the constructor stores `this` there; `Next` reads `node + 8` and returns it as a `dActor_c *`. Same owner-back-pointer shape as `fBase_c::sceneNode.owner` at `0x24`. |
| `0x068`/`0x06c`/`0x070` | `mPrevPosX/Y/Z` | `dActor_c::BeforeBehavior` ends by copying `mPosX/Y/Z` into them, on every path that lets the actor think. |
| `0x0b4` | `mClipOffsetY` | `BeforeBehavior` builds the point it hands to the camera transform as `(mPosX, mPosY + 0xb4, mPosZ) >> 3` -- a vertical offset from the actor's origin to the centre of its clip volume. It is the one of the four `SetRanges` arguments stored WITHOUT the `>> 3`, i.e. it is in position units, not clip units. `Platform.cpp` adds it to a `Vector3`'s `y` for the same purpose. |
| `0x0b8` | `mClipRadius` | passed as `Clipper::Func_020150E8`'s radius argument; zero skips the camera transform entirely ("no clip volume"). `Platform.cpp` reads it back `<< 3`. |
| `0x0bc` | `mClipDistance` | `BeforeBehavior` sets flags `0x18` (off screen + far away) when the clipper's answer exceeds it; `data_0209f274` doubles the threshold. |
| `0x0c0` | `mFarDistance` | the shorter of the two thresholds: exceeding it sets only flag `0x10`, "far away". |
| `0x0c4` | `mClipResult` | the `u8 *` out-parameter `BeforeBehavior` hands to `Clipper::Func_020150E8`. |
| `0x0ce` | `mDeathTableID` | `GetBitInDeathTable`, `TrackInDeathTable` and `UntrackInDeathTable` pass it straight to `DeathTable_GetBit`/`SetBit`/`ClearBit`; `dActor_c::Spawn`'s last parameter is `s16 deathTableID` and the constructor seeds it from `data_0208e378`. |

`0x0a4` and `0x0ac` are deliberately still `unk_`: `dEnemyBase_c` and `Door` prove
they exist and are four bytes, and nothing in the tree shows what they mean.
So are `0x050`'s neighbours `0x058`'s width aside, `0x0c4`'s exact bit meanings,
and `fBase_c`'s `0x10..0x13` and `0x48`.

## 9. Return types corrected from `void` to `int` -- what made each falsifiable

Nothing in the tree reads any of these slots' return values, so no caller could
ever have contradicted the wrong type. Each became falsifiable only when an
override with enough shape to expose the difference was migrated. In every case
mwcc allocates registers differently for `int` vs `void` even when r0 is neither
read nor set -- measured by building both, not assumed.

* slot 20 `Virtual50` -- still `int`; untested.
* slot 21 `OnGroundPounded` -- `Stump::OnGroundPounded` (ov091 `0x02133648`)
  compares two class fields and returns early on either. See `include/Stump.h`.
  `BigBrickBlock`'s and `QuestionBlock`'s overrides happened not to trigger the
  difference under `int` and were re-verified under `void`, so the correction is
  free there.
* slot 24 `OnKicked` -- `BigBrickBlock::OnKicked` (ov002 `0x020b36dc`) has two
  locals and two early returns. See `include/BigBrickBlock.h`.
* slot 27 `OnHitByMegaChar` -- `Stump::OnHitByMegaChar` (ov091 `0x021335d4`):
  four early-return field checks, 6-word register mismatch under `int`, exact
  match under `void`. See `include/Stump.h`. The two already-landed overrides,
  `dScMgSlot1_c`'s and `daObjMaruta_c`'s, have no locals or early returns, so the
  correction is a re-verified no-op for them.
* slot 30 `OnAimedAtWithEggReturnVec` returns a `Vector3` BY VALUE, and the ROM
  says so plainly: the definition at `0x020100dc` writes x/y/z through r0 and
  reads every field off r1, which is the AAPCS indirect-return shape -- r0 is the
  caller's return slot, and `this` has been pushed along to r1. A method returning
  `int` would have had `this` in r0 and nothing in r1. Returning a class by value
  is NOT the by-value-PARAMETER trap of `notes/mwccarm-codegen.md` 6az; an
  indirect return costs nothing here and byte-matches.

## 10. Static vs member, and the premise the argument rests on

Several `dActor_c` methods never touch a field, and are members anyway. The
argument, stated once because it recurs:

What the register evidence settles is static-vs-non-static GIVEN the mangled
name's parameter list, which is the one distinction mangling cannot express. A
free function with an unused leading `dActor_c *` would be byte-identical; it is
excluded by the attested symbol name, not by the disassembly. The names are the
premise the whole tree rests on, so the conclusion holds -- but it is name plus
ABI, not ABI alone.

Where it is applied:

* The five dust `...At` workers (`0x0200fac4`, `0x0200fb84`, `0x0200fd04`,
  `0x0200fd74`, `0x0200fdfc`). Each opens `mov r4, r1` and every later load is
  off r4, so the position arrives in r1 and r0 is written by every caller and
  read by none. A static `...At(const Vector3 &)` would have taken the position
  in r0.
* `FindEgg` and `FindExplosionActor` -- the clsn arrives in r1, r0 written by
  every caller and read by none.
* `IsPlayerInRange(const Vector3 &, s32)` -- `pos` in r1, `maxDist` in r2, r0
  written-but-unread.
* `UntrackStar` -- r0 written by both callers and read by neither, the `s8 &` in r1.

`GetBitInDeathTable` is a member for a stronger reason: its caller
`BeforeInitResources` loads `this` into r0 explicitly (`mov r0, r4`) immediately
before `bl 0x0200f9f4`, which a static taking no arguments would have had no
reason to do.

`FindWithActorID` and `Spawn` are static: the ROM reads their parameters from r0
onwards, leaving no register for a `this`.

## 11. The player-proximity cache

`ClosestPlayer` does the work: it walks the player table at `0x0209f394`
measuring against THIS actor's position (`Vec3_Dist(&mPosX, ...)`), and caches
both ends in globals -- nearest in `0x0209b458`, farthest in `0x0209b450`, their
distances in `0x0208e380` and `0x0208e37c`.

`ClosestNonVanishPlayer`, `FarthestPlayer`, `DistToCPlayer` and the five readers
at `0x0201045c..0x020109e4` are readers of that cache, which is why each calls
`ClosestPlayer` first and then returns a global. That is why each looks like it
throws a return value away: the call is made for its side effect, not its result.
`dActor_c::BeforeBehavior` clears `0x0209b458` at the top of every frame, which is
what makes it a per-frame cache.

The angle readers return `s16`: `HorzAngleToCPlayerOrAng`'s null path is an
`ldrsh [r4, #0x8e]`, a SIGNED halfword load of `mAngleY`, and the other two tail
straight into `Vec3_HorzAngle`. `maxDist` arrives as a plain integer and is
shifted into 20.12 at the comparison (`cmp r0, r4, lsl #12`), so it is a
whole-unit distance, not a fix12 one.

## 12. The dust group, and a naming defect kept on purpose

Two shapes, paired: an `...At` worker taking a position, and a no-argument
wrapper that copies the actor's own `0x5c..0x64` into a stack `Vector3` and calls
it. Five workers + five wrappers = ten.

The two landing-dust wrappers call the OTHER name's worker, and it is not a typo
in the header. `HugeLandingDust` (`0x0200fb4c`) calls `0x0200fac4` =
`LandingDustAt`, and `LandingDust` (`0x0200fc0c`) calls `0x0200fb84` =
`HugeLandingDustAt` -- verified by disassembling both `bl` targets, so it is a
property of the ROM and not of a stale comment. One side of each pair carries the
wrong adjective. Which side is unknowable from here: these are imported names,
nothing in the bytes ranks `0xb1`/`+0x5a000` against `0xb2`/`+0x28000` as the
"huge" one, and a symbol rename would have to find callers that never spell the
name. Recorded, deliberately not renamed.

## 13. The cylinder-collision group

`0x0200f7a8..0x02010c5c`. Everything here reads a `dCc_c` the caller already has;
see `include/dCc_c.h` for the layout (`0x18` flags, `0x20` hitFlags, `0x24`
otherOwner).

`FindEgg` and `FindExplosionActor` are the same function twice over. Both ask
"did the thing that hit me have an owner, and was the hit of my kind", then
resolve the owner to a `dActor_c`. Only the hitFlags bit differs: `0x2000` egg,
`0x4000` explosion. Kept as two, because the ROM has two.

`UpdatePos` takes a POINTER where the rest take references -- the mangled name
says `P5dCc_c`, not `R`. It is a two-line forwarder and passes the pointer
straight through to `UpdatePosWithOnlySpeed`.

## 14. Star tracking

`0x0200ff14..0x02010043` -- three functions in one uninterrupted run, which is why
they migrate as a set.

A "star marker" is the on-screen glint that shows where an uncollected star will
appear. `STAR_MARKERS` (`0x0209f40c`) is a twelve-entry table of the actors
currently holding one; `SetStarMarker(slot, actor, type)` writes it.

`TrackStar` claims the first free slot and returns its index, or -1. `this` is
visible in the bytes: r0 is stashed in r5 at entry and handed to `SetStarMarker`
as its second argument (`mov r1, r5`), so the actor being registered IS the
receiver. The loop counter is `s8` -- the increment is `add`/`lsl #0x18`/`asr #0x18`,
a sign-extending byte round-trip -- and it is bounded at `0xc`, which is what
fixes the table's length at twelve.

The `markerType == 2` path is the "only the star you are playing for" rule: when
the requested star is not the selected one (`0x0209f220`, an unsigned byte --
`ldrb`) and the current sublevel (`0x0209f2f8`, a SIGNED byte -- `ldrsb`) maps to
level `<= 0xe`, no marker is placed at all. Above `0xe` every star is marked. If
the star is already collected the type is bumped 2 -> 3.

`TrackStar`'s return type is NOT observable at the definition, and the eleven
callers that spell the mangled name disagree -- four say `int`, seven say
`unsigned char`. `s32` is kept because that is what reproduces and what the widest
caller declares; every caller in fact stores only the low byte, into an `s8` field
of its own, which is the same width `UntrackStar` then takes by reference.

`UntrackAndSpawnStar` untracks, then spawns actor `0xb2` (the star itself) at the
given position, inheriting `mAreaId` -- read with `ldrsb`, which agrees with the
`s8` declared at `0x0cc`.

The symbol was renamed here, `...RK7Vector3j` -> `...RK7Vector3h`.
`howToSpawnStar` is read `ldrb` -- 8-bit -- but the imported name spelled it `j`.
Nothing upstream could have caught that: AAPCS widens an integer argument either
way, so no caller can observe an over-wide parameter, and the file byte-matched
throughout as a `.c` because it spelled the wrong name by hand. Migrating the
definition is the first point at which the compiler mangles the name for us and
the claim becomes falsifiable -- and it was false. Declared `u8` the bytes
reproduce exactly; written as a cast on a `u32` parameter they do not
(`ldr` + `and #0xff`, one instruction long).

## 15. The spawn-and-reward group

`0x02010044..0x02010929`. All three are members and all three dereference `this`:
`SpawnNumber` and `GivePlayerCoins` for a field (`mAreaId` at `0xcc`,
`mCamSpacePos` at `0x74`), `ClosestWithActorID` for the position it measures from.

`SpawnNumber` puts up actor `0x14a`, the floating score/count popup, and hands it
to a helper that makes it follow `owner`. `packLowNibble` is a `bool` in the
mangled name and arrives in r3, where `cmp r3, #0` cannot tell bool from int --
the name is the authority, and the bytes do not contradict it. What it selects IS
visible: `and #0xf` then `orr #0x10`, so the value is squeezed into a nibble and
tagged. `delay` is `ldrh` -- u16 -- and is written to the popup's own `0x14c`;
callers pass 0 or `0x15`.

`GivePlayerCoins` takes `Player` BY REFERENCE and then tests it against null,
which the ROM does plainly (`movs r5, r1` into the argument, then a conditional
return). That is not a contradiction to resolve away: the mangled name says
`R6Player`, and the null test is real, so the source has to say both. `count` is
`h` (u8) and `coinKind` is `j` compared `cmp #3` with an UNSIGNED `hs` branch,
which is what makes it `u32` rather than `int`.

`ClosestWithActorID` walks `FindWithActorID` and keeps the nearest, skipping
`this` itself; the sentinel is `mvn r5, #0x80000000` = `0x7fffffff`.

## 16. `dActor_c::Spawn`'s last two parameters are `s8` and `s16`

The symbol was renamed `..._16ii` -> `..._16as`. The definition at `0x02010e2c`
reads them `ldrsb r2, [sp, #0x10]` and `ldrsh r3, [sp, #0x14]`. Declared this way
the body reproduces all `0x4c` bytes; declared `(s32, s32)` mwcc emits `ldr` for
both, the function comes out `0x5c`, and it misses. Both were built.

162 files spelled the old name and not one of them could have caught it:
arguments five and six occupy full stack words however they are declared, so no
call site's bytes depend on the difference. The narrowing belongs to the callee
and is visible only there.

## 17. Why the by-value-`Fix12` methods are not declared as definable methods

Methods whose mangled names carry a by-value class parameter (`5Fix12IiE`, and the
`Vector3` forms) are deliberately not declared in `dActor_c` as methods that any
TU could define -- see `notes/mwccarm-codegen.md` 6az. CW homes class-typed
by-value parameters to the stack, costing `+0x14`, so those keep `extern "C"`
definitions with scalar args. A true-signature declaration for callers is fine and
is tracked separately.

## 18. The size assertions

`typedef char dActor_c_size_must_be_0xd0[...]` and its `fBase_c` counterpart sit
OUTSIDE the `__cplusplus` split on purpose, so the C and C++ spellings cannot
drift apart.

Read what they do and do not claim. `0xd0` is the size the header's own field list
computes -- it is NOT independent ROM evidence that a `dActor_c` is `0xd0` bytes.
What it buys is real all the same: the two spellings are held to each other, a
field retyped without shrinking the pad after it stops compiling, and
`include/Player.h` becomes checkable -- a derived struct's fields start at its
base's size, and `tools/check_header_offsets.py` will not guess that number.

## 19. `dActor_c::BeforeBehavior` -- flag bits, and a dead lever

Flag bits, as that function uses them:

    0x00002  do not render while off screen
    0x00008  off screen        )  0x38 = these three, "fully invisible"
    0x00010  far away          )
    0x00020  wrong area        )
    0x00003  has a clip volume -- 0x10003 with the suppress bit
    0x10000  suppress behaviour; combined with off-screen it returns 0

`mAreaId` is signed and negative means "not area-bound", which is why the
area test is `>= 0` before calling `IsAreaShowing` rather than after.

Dead lever, recorded so nobody re-derives it: the old source wrote every
read-modify-write of the flags as `*(u32 *)((int)(self + 0xb0)) |= ...`, routing
the address through `int` -- the shape you reach for when CW has to re-read a
field instead of keeping it in a register across the update. Both spellings were
built and compared under 2004/b56, and the plain `mFlags |= ...` is byte-identical
across all `0x240` bytes. The casts bought nothing here.

Load-bearing and still in the file: inside `extern "C" { ... }` a variable
declaration WITHOUT `extern` is a DEFINITION in C++ -- C's tentative-definition
rule does not apply -- so the object would define those globals alongside the
ROM's gap object. The same shape in `dActor_c::HorzAngleToFPlayer` produced
`Multiply-defined: "data_0209b450" ... Previously defined in _dsd_gap@main_40.o`
and aborted the link. It did not abort in `BeforeBehavior`, which is luck about
how mwcc emits each type, not correctness. Function declarations need no `extern`;
only the objects do.

## 20. `decl_*.h` C linkage

`include/decl_Actor.h` and `include/decl_ActorBase.h` (and their siblings) declare
ROM symbols by their exact final names, so a C++ translation unit including them
must not mangle them -- a bare `void Foo(int);` seen from C++ emits `_Z3Fooi`,
which exists nowhere. The file still byte-matches, because `match.py` compares
relocated words as wildcards, so nothing catches it until the ROM link -- and
`eligible.py` refuses to enrol a file with unresolvable references, so the link
never sees it either. Hence the `extern "C"` guard those generated headers carry.

Verified safe: of the 1,644 function names declared across the `decl_*.h` headers,
1,572 are themselves the ROM symbol and 0 exist ONLY in a mangled form, so no
declaration relies on C++ mangling. The remaining 72 resolve to neither spelling
and are unresolvable with or without the guard.
