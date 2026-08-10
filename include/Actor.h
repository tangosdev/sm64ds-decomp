#ifndef ACTOR_H
#define ACTOR_H

#include "types.h"

#ifdef __cplusplus
#include "ActorDerived.h"
#endif

/* The base of every enemy and object class, 0x020100dc..0x020113e4.
 *
 * The chain is ActorBase -> ActorDerived -> Actor. Actor is NOT a direct child
 * of ActorBase: Actor::Actor calls ActorBase::ActorBase, stores ActorDerived's
 * vptr, then immediately overwrites it with its own. Two consecutive vptr
 * stores is what an inlined intermediate-base constructor looks like. See
 * notes/actor-vtables.md.
 *
 * LAYOUT. ActorBase occupies 0x00..0x4f, so Actor's own fields begin at 0x50.
 * The generated header this replaces duplicated ActorBase's fields inline --
 * uniqueID at 0x04, actorID at 0x0c -- instead of inheriting them, which is why
 * it opened with pad_000[0x4] and a 0x42-byte gap.
 *
 * VTABLE. _ZTV5Actor (0x0208e3a4) has 31 slots. Actor overrides ten of the
 * eighteen it inherits -- 1, 2, 4, 5, 7, 8, 10, 11 and the destructor at 16/17
 * -- and appends thirteen of its own at 18..30. Slots 0, 3, 6, 9, 12, 13, 14
 * and 15 still point at the ActorBase implementations.
 *
 * The destructor is declared FIRST on purpose. CW 1.2 emits the vtable into the
 * TU defining the first non-inline virtual declared in the class (the key
 * function), and that copy collides with the one the module's gap object
 * supplies from ROM data. An override takes its base's slot wherever it is
 * declared, so putting ~Actor first costs nothing and makes it the key
 * function. The thirteen NEW virtuals still take 18..30 from their declaration
 * order below, because new slots append after the inherited table.
 *
 * What makes that safe is NOT that the destructor lives in a C translation
 * unit -- src/_ZN5ActorD1Ev.cpp and _ZN5ActorD2Ev.cpp are C++ and do include
 * this header; only _ZN5ActorD0Ev.c is C. The invariant is that all three
 * define extern "C" free functions under the mangled names and none defines
 * `Actor::~Actor`, so no TU is ever the key function's definition.
 *
 * The rule, stated precisely: the key function -- the first non-inline virtual
 * declared -- must never be defined as a real method in any translation unit.
 * Declaring the destructor first pins that role to TUs which by construction
 * never will. include/ActorBase.h reaches the same end differently: it does
 * declare InitResources (slot 0) in-class, but src/_ZN9ActorBase13InitResourcesEv.cpp
 * deliberately defines it as an extern "C" free function rather than a method.
 * Do not "fix" that file into a real method, and do not remove the declaration
 * from ActorBase.h -- removing it would delete slot 0 and shift all 18 slots.
 *
 * Field NAMES are inferred from behaviour and cannot change codegen, so they are
 * safe to improve. Offsets, widths and vtable slots are pinned by the bytes.
 */

#ifdef __cplusplus

struct Player;
struct Vector3;
struct Vector3_16;
struct CylinderClsn;

/* The actor heap and its deallocator, for the inline operator delete at the end of
   the class. `data_020a0eac` is the heap every actor is allocated from -- the same
   one ActorBase::operator new (src/_ZN9ActorBasenwEj.cpp) passes to
   Memory::Allocate.

   SPELT EXACTLY AS include/decl_common.h SPELLS IT, deliberately. The mangled name
   says the second parameter is a `Heap*`, and decl_common.h says `void*`; declaring
   the honest type here instead makes two incompatible `extern "C"` declarations of
   one name visible in the same translation unit, which mwcc rejects as "illegal
   function overloading". That cost 105 files their eligibility when it was tried.
   Correcting the parameter type is worth doing -- in decl_common.h, once, for every
   caller at the same time. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

struct Actor : ActorDerived {
    s32 unk_050;            /* 0x050 */
    s32 unk_054;            /* 0x054 */
    u8  unk_058;            /* 0x058 */
    u8  pad_059[0x3];
    s32 mPosX;              /* 0x05c */
    s32 mPosY;              /* 0x060 */
    s32 mPosZ;              /* 0x064 */
    s32 unk_068;            /* 0x068 -- previous position, copied from 0x5c..0x64 */
    s32 unk_06c;            /* 0x06c */
    s32 unk_070;            /* 0x070 */
    s32 mCamSpacePosX;      /* 0x074 */
    s32 mCamSpacePosY;      /* 0x078 */
    s32 mCamSpacePosZ;      /* 0x07c */
    /* 0x080..0x08b and the 0x098..0x0ab block below were bare padding and u8
       placeholders here, while Player.h -- describing the same bytes -- named
       them and typed them s32. Player is right, and the evidence is outside
       Player: BooCage::InitResources and MadPiano::InitResources write -0x4000
       and -0x2000 to 0x09c and -0x46000 / -0x3c000 to 0x0a0, which are fix12
       gravity and terminal velocity, not bytes. Player::St_Walk_Main passes
       0x098 as a 32-bit argument.

       These were deliberately left wrong until now: nothing compiled against
       Actor's copies, so no gate could prove a change either way. Now that
       Player inherits them, 62 files using mHorzSpeed and 49 using mVertSpeed
       resolve through this header, and a wrong width fails immediately. */
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    s32 mHorzSpeed;         /* 0x098 */
    s32 mVertAccel;         /* 0x09c -- fix12, negative (gravity) */
    s32 mTerminalVelocity;  /* 0x0a0 -- fix12, negative */
    u8  pad_0a4[0x4];       /* likely the same physics block; unproven */
    s32 mVertSpeed;         /* 0x0a8 */
    u8  pad_0ac[0x4];       /* likely the same physics block; unproven */
    u32 mFlags;             /* 0x0b0 -- bit 0x10000 suppresses behaviour */
    s32 unk_0b4;            /* 0x0b4 */
    s32 unk_0b8;            /* 0x0b8 -- clip radius; 0 skips the camera transform */
    s32 unk_0bc;            /* 0x0bc */
    s32 unk_0c0;            /* 0x0c0 */
    u8  unk_0c4;            /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;            /* 0x0cc -- negative means "not area-bound" */
    u8  pad_0cd[0x1];
    s16 unk_0ce;            /* 0x0ce */

    /* --- vtable. Declared first, see the header comment. Overrides slots
           16 (D1) and 17 (D0); position here does not affect that. --- */
    virtual ~Actor();

    /* --- overrides of inherited slots. Each takes its base's index. --- */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess);    /* slot  2 */
    virtual int  BeforeCleanupResources();             /* slot  4 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                     /* slot  7 */
    virtual void AfterBehavior(u32 vfSuccess);         /* slot  8 */
    virtual int  BeforeRender();                       /* slot 10 */
    virtual void AfterRender(u32 vfSuccess);           /* slot 11 */

    /* --- new slots, 18..30, in declaration order. Do not reorder. --- */
    virtual int  OnYoshiTryEat();                      /* slot 18 */
    virtual int  OnTurnIntoEgg(Player &player);        /* slot 19 */
    virtual int  Virtual50();                          /* slot 20 -- vtable+0x50 */
    virtual int  OnGroundPounded(Actor &other);        /* slot 21 */
    virtual int  OnAttacked1(Actor &other);            /* slot 22 */
    virtual int  OnAttacked2(Actor &other);            /* slot 23 */
    virtual int  OnKicked(Actor &other);               /* slot 24 */
    virtual int  OnPushed(Actor &other);               /* slot 25 */
    virtual int  OnHitByCannonBlastedChar(Actor &other); /* slot 26 */
    virtual int  OnHitByMegaChar(Player &player);      /* slot 27 */
    virtual int  OnHitFromUnderneath(Actor &other);    /* slot 28 */
    virtual int  OnAimedAtWithEgg();                   /* slot 29 */

    /* slot 30. Returns a Vector3 BY VALUE, and the ROM says so plainly: the
       definition at 0x020100dc writes x/y/z through r0 and reads every field
       off r1, which is the AAPCS indirect-return shape -- r0 is the caller's
       return slot, and `this` has been pushed along to r1. A method returning
       `int` would have had `this` in r0 and nothing in r1.

       It was declared `virtual int` until the definition was migrated, which is
       when it became falsifiable: nothing in the tree calls this slot, so no
       caller could ever have contradicted the wrong type.

       Returning a class by value is NOT the by-value-parameter trap of
       notes/mwccarm-codegen.md 6az. That one is about mwccarm homing r0-r3 for
       a by-value class ARGUMENT, costing +0x14. An indirect return costs
       nothing here and byte-matches -- measured, not assumed. */
    virtual Vector3 OnAimedAtWithEggReturnVec();       /* slot 30 */

    /* --- non-virtual --- */
    /* The player-proximity group. ClosestPlayer does the work: it walks the
       player table at 0x0209f394 measuring against THIS actor's position
       (`Vec3_Dist(&mPosX, ...)`), and caches both ends in globals -- nearest in
       0x0209b458, farthest in 0x0209b450, their distances in 0x0208e380 and
       0x0208e37c. The other three are readers of that cache, which is why each
       calls ClosestPlayer first and then returns a global. Members, all four:
       the distance is measured from `this`. */
    Player *ClosestPlayer();
    Player *ClosestNonVanishPlayer();
    Player *FarthestPlayer();
    s32     DistToCPlayer();

    /* The readers of that same cache, 0x0201045c..0x020109e4. Every one opens
       by calling ClosestPlayer() -- which is what refills 0x0209b458 and
       0x0209b450 -- and only then reads the global. That is why each looks
       like it throws a return value away: the call is made for its side
       effect, not its result.

       All five are members. Four take `this`'s position from 0x5c and one
       reads mAngleY, so the dereference is visible in the bytes. The
       Vector3-taking overload is the exception worth naming: its body never
       touches a field, and it is a member because the ROM puts `pos` in r1 and
       maxDist in r2, leaving r0 written-but-unread -- the same argument as the
       dust workers above, with the same premise about the mangled name.

       The angles return s16: HorzAngleToCPlayerOrAng's null path is an
       `ldrsh [r4,#0x8e]`, a SIGNED halfword load of mAngleY, and the other two
       tail straight into Vec3_HorzAngle. maxDist arrives as a plain integer and
       is shifted into 20.12 at the comparison (`cmp r0, r4, lsl #12`), so it is
       a whole-unit distance, not a fix12 one. */
    s16  HorzAngleToCPlayer();
    s16  HorzAngleToFPlayer();
    s16  HorzAngleToCPlayerOrAng();
    bool IsPlayerInRange(s32 maxDist);
    bool IsPlayerInRange(const Vector3 &pos, s32 maxDist);

    /* Static: the mangled name carries both parameters and the ROM reads them
       from r0 and r1, leaving no register for a `this`. */
    static Actor *FindWithActorID(u32 actorID, Actor *after);

    /* Integrates horizontal speed along the facing angle and applies gravity,
       all through `this` -- mHorzSpeed at 0x98, mVertAccel 0x9c,
       mTerminalVelocity 0xa0, mVertSpeed 0xa8. */
    void UpdatePosWithHorzSpeedAndAng();
    int  BumpedUnderneathByPlayer(Player &player);
    int  GetSubtraction(short a, short b);

    /* The cylinder-collision group, 0x0200f7a8..0x02010c5c. Everything here
       reads a CylinderClsn the caller already has; see include/CylinderClsn.h
       for the layout (0x18 flags, 0x20 hitFlags, 0x24 otherOwner).

       All five are members. UpdatePos, MakeVanishLuigiWork and
       JumpedOnByPlayer dereference `this` outright. The two finders do not
       touch a field, and are members on the same argument as the dust workers:
       the clsn arrives in r1 and r0 is written by every caller and read by
       none -- name plus ABI, not ABI alone.

       FindEgg and FindExplosionActor are the same function twice over. Both
       ask "did the thing that hit me have an owner, and was the hit of my
       kind", then resolve the owner to an Actor. Only the hitFlags bit
       differs: 0x2000 egg, 0x4000 explosion. Kept as two, because the ROM has
       two.

       UpdatePos takes a POINTER where the rest take references -- the mangled
       name says `P12CylinderClsn`, not `R`. It is a two-line forwarder and
       passes the pointer straight through to UpdatePosWithOnlySpeed. */
    Actor *FindEgg(CylinderClsn &clsn);               /* hitFlags 0x2000 */
    Actor *FindExplosionActor(CylinderClsn &clsn);    /* hitFlags 0x4000 */
    void   MakeVanishLuigiWork(CylinderClsn &clsn);
    int    JumpedOnByPlayer(CylinderClsn &clsn, Player &player);
    void   UpdatePos(CylinderClsn *clsn);
    void   UpdatePosWithOnlySpeed(CylinderClsn *clsn);
    /* The dust group, 0x0200fac4..0x0200fe70. Two shapes, paired:
       an `...At` worker taking a position, and a no-argument wrapper that
       copies the actor's own 0x5c..0x64 into a stack Vector3 and calls it.

       Five workers + five wrappers = ten, and all ten are members. The workers
       need that said out loud, because not one of them reads a field: each
       opens `mov r4, r1` and every later load is off r4, so the position
       arrives in r1 and r0 is written by every caller and read by none of
       them. A static `...At(const Vector3&)` would have taken the position in
       r0 instead. Disassembled at 0x0200fac4, 0x0200fb84, 0x0200fd04,
       0x0200fd74 and 0x0200fdfc -- all five.

       State that premise precisely, because the bytes alone do not carry it:
       what the register evidence settles is static-vs-non-static GIVEN the
       mangled name's parameter list, which is the one distinction mangling
       cannot express. A free function with an unused leading `Actor*` would be
       byte-identical; it is excluded by the attested symbol name, not by the
       disassembly. The names are the premise the whole tree rests on, so the
       conclusion holds -- but it is name plus ABI, not ABI alone.

       NAMING, and it is not a typo below: the two landing-dust wrappers call
       the OTHER name's worker. HugeLandingDust (0x0200fb4c) calls 0x0200fac4 =
       LandingDustAt, and LandingDust (0x0200fc0c) calls 0x0200fb84 =
       HugeLandingDustAt -- verified by disassembling both `bl` targets, so it
       is a property of the ROM and not of a stale comment. One side of each
       pair carries the wrong adjective. Which side is unknowable from here:
       these are imported names, nothing in the bytes ranks 0xb1/+0x5a000
       against 0xb2/+0x28000 as the "huge" one, and a symbol rename would have
       to find callers that never spell the name. Recorded, deliberately not
       renamed. */
    void PoofDust();
    void PoofDustAt(const Vector3 &pos);            /* particles 0x122, 0x123 */
    void SmallPoofDust();
    void DisappearPoofDustAt(const Vector3 &pos);   /* particles 0x127, 0x128 */
    void TriplePoofDust();
    void TriplePoofDustAt(const Vector3 &pos);      /* particles 0x124..0x126 */
    void LandingDust(bool doRaycast);
    void HugeLandingDust(bool doRaycast);
    void HugeLandingDustAt(Vector3 &pos, bool doRaycast);  /* particle 0xb2 */
    void LandingDustAt(Vector3 &pos, bool doRaycast);      /* particle 0xb1 */

    /* The death table, which records actors already killed so they do not
       respawn. GetBitInDeathTable is a member and takes nothing: its caller
       BeforeInitResources loads `this` into r0 explicitly (`mov r0,r4`)
       immediately before `bl 0x0200f9f4`, which a static taking no arguments
       would have had no reason to do. */
    int  GetBitInDeathTable();
    void KillAndTrackInDeathTable();
    void TrackInDeathTable();
    void SpawnSoundObj(u32 soundObjParam);
    s32  GetWaterHeightWDW();

    /* Static: searches the live-actor list rather than acting on an instance. */
    static Actor *FindWithID(u32 id);

    /* Star tracking, 0x0200ff14..0x02010043 -- three functions in one
       uninterrupted run, which is why they migrate as a set.

       A "star marker" is the on-screen glint that shows where an uncollected
       star will appear. STAR_MARKERS (0x0209f40c) is a twelve-entry table of
       the actors currently holding one; SetStarMarker(slot, actor, type)
       writes it. All three functions here are members.

       TrackStar claims the first free slot and returns its index, or -1.
       `this` is visible in the bytes: r0 is stashed in r5 at entry and handed
       to SetStarMarker as its second argument (`mov r1, r5`), so the actor
       being registered IS the receiver. The loop counter is s8 -- the
       increment is `add`/`lsl #0x18`/`asr #0x18`, a sign-extending byte
       round-trip -- and it is bounded at 0xc, which is what fixes the table's
       length at twelve.

       The markerType == 2 path is the "only the star you are playing for"
       rule: when the requested star is not the selected one (0x0209f220, an
       unsigned byte -- `ldrb`) and the current sublevel (0x0209f2f8, a SIGNED
       byte -- `ldrsb`) maps to level <= 0xe, no marker is placed at all.
       Above 0xe every star is marked. If the star is already collected the
       type is bumped 2 -> 3, which is how a collected star's marker differs.

       The return type is NOT observable at the definition, and the eleven
       callers that spell the mangled name disagree -- four say `int`, seven
       say `unsigned char`. s32 is kept because that is what reproduces and
       what the widest caller declares; every caller in fact stores only the
       low byte, into an s8 field of its own, which is the same width
       UntrackStar then takes by reference.

       UntrackStar is the counterpart: it clears the slot and writes -1 back
       through the reference. Its body never touches a field -- r0 is written
       by both of its callers and read by neither, while the s8& arrives in r1
       -- so it is a member on the same argument as the dust workers, and with
       the same premise: name plus ABI, not ABI alone. The `ldrsb` on the
       reference is what makes the referent signed rather than u8.

       UntrackAndSpawnStar untracks, then spawns actor 0xb2 (the star itself)
       at the given position, inheriting mAreaId -- read with `ldrsb`, which
       agrees with the s8 declared at 0x0cc. It is a member outright: r0 is
       kept in r6 across the call precisely so that field can be loaded.

       THE SYMBOL WAS RENAMED HERE, `...RK7Vector3j` -> `...RK7Vector3h`.
       howToSpawnStar is read `ldrb` -- 8-bit -- but the imported name spelled
       it `j`. Nothing upstream could have caught that: AAPCS widens an
       integer argument either way, so no caller can observe an over-wide
       parameter, and the file byte-matched throughout as a `.c` because it
       spelled the wrong name by hand. Migrating the definition is the first
       point at which the compiler mangles the name for us and the claim
       becomes falsifiable -- and it was false. Declared u8 the bytes
       reproduce exactly; written as a cast on a u32 parameter they do not
       (`ldr` + `and #0xff`, one instruction long). See the definition. */
    s32    TrackStar(u32 starIdx, u32 markerType);
    void   UntrackStar(s8 &starID);
    Actor *UntrackAndSpawnStar(s8 &trackStarID, u32 starID,
                               const Vector3 &spawnPos, u8 howToSpawnStar);

    /* The spawn-and-reward group, 0x02010044..0x02010929. All three are
       members and all three dereference `this`: SpawnNumber and GivePlayerCoins
       for a field (mAreaId at 0xcc, mCamSpacePos at 0x74), ClosestWithActorID
       for the position it measures from.

       SpawnNumber puts up actor 0x14a, the floating score/count popup, and
       hands it to a helper that makes it follow `owner`. packLowNibble is a
       `bool` in the mangled name and arrives in r3, where `cmp r3, #0` cannot
       tell bool from int -- the name is the authority, and the bytes do not
       contradict it. What it selects IS visible: `and #0xf` then `orr #0x10`,
       so the value is squeezed into a nibble and tagged. delay is `ldrh` --
       u16 -- and is written to the popup's own 0x14c; callers pass 0 or 0x15.

       GivePlayerCoins takes Player BY REFERENCE and then tests it against
       null, which the ROM does plainly (`movs r5, r1` into the argument, then
       a conditional return). That is not a contradiction to resolve away: the
       mangled name says R6Player, and the null test is real, so the source has
       to say both. count is `h` (u8) and coinKind is `j` compared `cmp #3` with
       an UNSIGNED `hs` branch, which is what makes it u32 rather than int.

       ClosestWithActorID walks FindWithActorID and keeps the nearest, skipping
       `this` itself; the sentinel is `mvn r5, #0x80000000` = 0x7fffffff. */
    Actor *SpawnNumber(const Vector3 &pos, u32 value, bool packLowNibble,
                       u16 delay, Actor *owner);
    void   GivePlayerCoins(Player &player, u8 count, u32 coinKind);
    Actor *ClosestWithActorID(u32 actorID);

    /* Static: builds an actor from an ID rather than acting on an instance --
       r0 carries actorID, not `this`, at 0x02010e2c's only in-tree call site.
       Declared here so callers mangle it; still defined as an extern "C" free
       function under its mangled name.

       THE LAST TWO PARAMETERS ARE s8 AND s16, AND THE SYMBOL NOW SAYS SO --
       renamed `..._16ii` -> `..._16as`. The definition at 0x02010e2c reads
       them `ldrsb r2, [sp, #0x10]` and `ldrsh r3, [sp, #0x14]`. Declared this
       way the body reproduces all 0x4c bytes; declared (s32, s32) mwcc emits
       `ldr` for both, the function comes out 0x5c, and it misses. Both were
       built.

       162 files spelled the old name and not one of them could have caught it:
       arguments five and six occupy full stack words however they are
       declared, so no call site's bytes depend on the difference. The
       narrowing belongs to the callee and is visible only there. */
    static Actor *Spawn(u32 actorID, u32 spawnParam, const Vector3 &pos,
                        const Vector3_16 *rot, s8 areaID, s16 deathTableID);

    /* Methods whose mangled names carry a by-value class parameter (5Fix12IiE,
       and the Vector3 forms) are deliberately NOT declared here as definable
       methods -- see notes/mwccarm-codegen.md 6az. CW homes class-typed by-value
       parameters to the stack, costing +0x14, so those keep extern "C"
       definitions with scalar args. A true-signature declaration for callers is
       fine and is tracked separately. */

    /* THE COUNTERPART OF ActorBase::operator new, AND WHAT LETS A REAL `~Class()`
       REPRODUCE THE ROM'S DELETING DESTRUCTOR.

       The compiler generates D0 -- the deleting destructor, vtable slot 17 -- as
       "run the destructor body, then call operator delete on the class". Without
       this declaration it emits a call to the global `_ZdlPv`, which exists nowhere
       in this image, and D0 comes out three instructions short of the ROM's.

       Inline, and that is not a style choice, it is what the ROM shows.
       ActorBase::operator new is a real function at 0x02043444, but NO operator
       delete symbol exists anywhere in the image, and every deleting destructor
       ends with the same two instructions -- load the actor heap, call
       Memory::Deallocate -- rather than a call to a shared helper. Only an inline
       member produces that.

       DECLARED HERE AND NOT ON ActorBase, and the difference is load-bearing: mwcc
       inlines it only when it is found in the class itself or its IMMEDIATE base.
       On ActorBase, every Actor-derived D0 emits an out-of-line call instead and
       misses. Enemy carries its own copy for the same reason -- it is a flattened
       struct that does not derive from Actor in these headers, so this one is not
       even in scope for the classes under it.

       No layout effect: an inline non-virtual member adds no field and no vtable
       slot, and Actor's 0xd0 assertion still holds. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }
};

#else

/* Flat layout for the C translation units, which can express neither the base
   class nor the virtual functions. 0x00..0x4f is ActorBase. */
struct Actor {
    void **vtable;          /* 0x000 */
    u32 uniqueID;           /* 0x004 */
    u32 param1;             /* 0x008 */
    u16 actorID;            /* 0x00c */
    u8  aliveState;         /* 0x00e */
    u8  shouldBeKilled;     /* 0x00f */
    u8  pad_010[0x4];       /* 0x010 */
    u8  sceneNode[0x14];    /* 0x014 */
    u8  behavNode[0x10];    /* 0x028 */
    u8  renderNode[0x10];   /* 0x038 */
    void *unk_048;          /* 0x048 */
    void *heap;             /* 0x04c */
    s32 unk_050;            /* 0x050 */
    s32 unk_054;
    u8  unk_058;
    u8  pad_059[0x3];
    s32 mPosX;              /* 0x05c */
    s32 mPosY;
    s32 mPosZ;
    s32 unk_068;
    s32 unk_06c;
    s32 unk_070;
    s32 mCamSpacePosX;      /* 0x074 */
    s32 mCamSpacePosY;
    s32 mCamSpacePosZ;
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;
    s32 mScaleZ;
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;        /* 0x092 */
    s16 mPrevAngleY;        /* 0x094 */
    s16 mPrevAngleZ;        /* 0x096 */
    s32 mHorzSpeed;         /* 0x098 */
    s32 mVertAccel;         /* 0x09c */
    s32 mTerminalVelocity;  /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;         /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;             /* 0x0b0 */
    s32 unk_0b4;
    s32 unk_0b8;
    s32 unk_0bc;
    s32 unk_0c0;
    u8  unk_0c4;
    u8  pad_0c5[0x7];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 unk_0ce;            /* 0x0ce */
};

#endif /* __cplusplus */

/* Outside the split, so the C and C++ spellings cannot drift apart.
 *
 * Read what this does and does not claim. 0xd0 is the size this header's own
 * field list computes -- it is NOT independent ROM evidence that an Actor is
 * 0xd0 bytes. What it buys is real all the same: the two spellings are held to
 * each other, a field retyped without shrinking the pad after it stops
 * compiling, and include/Player.h becomes checkable -- a derived struct's fields
 * start at its base's size, and tools/check_header_offsets.py will not guess
 * that number. */
typedef char Actor_size_must_be_0xd0[sizeof(struct Actor) == 0xd0 ? 1 : -1];

#endif
