#ifndef DACTOR_C_H
#define DACTOR_C_H

#include "types.h"

/* The s16 vector, complete here because mAngle/mPrevAngle below hold one by
   value. Guarded exactly as common.h and dBgW.h guard their copies. */
#ifndef VECTOR3_16_DEFINED
#define VECTOR3_16_DEFINED
struct Vector3_16 {
    s16 x, y, z;
};
#endif

#ifdef __cplusplus
#include "dBase_c.h"
#include "fListNode.h"
#endif

/* The base of every enemy and object class, 0x020100dc..0x020113e4.
 *
 * The chain is fBase_c -> dBase_c -> dActor_c; fBase_c takes 0x00..0x4f, so this
 * class's own fields start at 0x50.
 *
 * DO NOT REORDER THE VIRTUALS. Slot index follows declaration order, and
 * _ZTV8dActor_c (0x0208e3a4) is 31 slots: ten overrides of the eighteen
 * inherited, the destructor at 16/17, and thirteen new ones at 18..30.
 *
 * THE DESTRUCTOR IS DECLARED FIRST ON PURPOSE. CW emits the vtable into the TU
 * that defines the first non-inline virtual declared -- the key function -- and
 * that copy collides with the one the module's gap object supplies from ROM.
 * Putting ~dActor_c first pins that role to the D0/D1/D2 sources. Each source
 * carries the same real destructor definition and objisolate retains only its
 * enrolled variant. Keep it that way.
 *
 * Offsets, widths and vtable slots are pinned by the bytes; field names are not
 * and are safe to improve. Provenance for all of it: notes/actor-core-provenance.md,
 * notes/actor-vtables.md.
 */

#ifdef __cplusplus

struct Player;
struct Vector3;
struct dCc_c;
/* Only ever named through a pointer below; the definition lives in common.h /
   math/Matrix.h, which this header deliberately does not pull in. */
struct Matrix4x3;
struct dExtShadowModel_c;
#include "math/Fix12.h"

/* The actor heap and its deallocator, for the inline operator delete at the end of
   the class. data_020a0eac is the heap every actor is allocated from.

   SPELT EXACTLY AS include/decl_common.h SPELLS IT. The honest second parameter is
   Heap*, but two incompatible extern "C" declarations of one name in a single TU
   are rejected as "illegal function overloading" -- that cost 105 files their
   eligibility when it was tried. Fix it in decl_common.h or not at all. */
extern "C" void _ZN6Memory10DeallocateEPvPN5mHeap6Heap_tE(void *, void *);
extern "C" void *data_020a0eac;

struct dActor_c : dBase_c {
    /* The global actor list (data_0209b468) threads through every actor: the
       constructor appends this node, the destructor unlinks it, and Next() walks
       the next link and reads its owner to get back to the actor. */
    fLiNdBa_c mActorListNode; /* 0x050 -- owner is always `this` */
    /* Each triple below is also one vector: mPos is mPosX/Y/Z, mAngle is
       mAngleX/Y/Z, and so on. Both spellings name the same storage. New code
       should prefer the vector member (`&mPos`, not `(Vector3 *)&mPosX`); the
       scalar names stay so the thousands of existing references need not churn.

       THESE OVERLAYS ARE NOT FREE, AND THEY ARE PAID FOR BELOW. mwcc numbers
       its @NNN temporaries from one per-TU counter, and every unnamed struct
       or union draws two numbers from it: four per overlay, 24 for the six.
       Unpaid, that renumbers the pinned @NNN symbols of every TU including
       this header. A named parameter in a member declaration draws one, so
       24 parameter names further down are left off and the counter ends this
       header exactly where it did before the overlays. Naming or unnaming a
       parameter in this class, or adding an overlay, moves every consumer's
       @NNN: keep the books balanced.

       Vector3 has a constructor, so mwcc warns "unions cannot have nontrivial
       class members" here; the warning is expected. Nothing is constructed
       through these, but the constructor's TU now also emits unused empty
       destructors for the four Vector3 unions. For the port (C++11 and
       later), a nontrivial union member also deletes dActor_c's implicit
       copy constructor and copy assignment; nothing copies an actor. */
    union {
        struct {
            s32 mPosX, mPosY, mPosZ;
        };
        Vector3 mPos; /* 0x05c */
    };
    union {
        struct {
            s32 mPrevPosX, mPrevPosY, mPrevPosZ;
        };
        Vector3 mPrevPos; /* 0x068 -- last frame's position, snapshotted by BeforeBehavior */
    };
    union {
        struct {
            s32 mCamSpacePosX, mCamSpacePosY, mCamSpacePosZ;
        };
        Vector3 mCamSpacePos; /* 0x074 */
    };
    union {
        struct {
            s32 mScaleX, mScaleY, mScaleZ;
        };
        Vector3 mScale; /* 0x080 */
    };
    union {
        struct {
            s16 mAngleX, mAngleY, mAngleZ;
        };
        Vector3_16 mAngle; /* 0x08c */
    };
    union {
        struct {
            s16 mPrevAngleX, mPrevAngleY, mPrevAngleZ;
        };
        Vector3_16 mPrevAngle; /* 0x092 */
    };
    s32 mHorzSpeed;         /* 0x098 */
    s32 mVertAccel;         /* 0x09c -- fix12, negative (gravity) */
    s32 mTerminalVelocity;  /* 0x0a0 -- fix12, negative */
    /* mSpeedX, mVertSpeed and mSpeedZ are the velocity vector:
       UpdatePosWithOnlySpeed adds it to mPos, and UpdatePosWithHorzSpeedAndAng
       derives the x/z words from mHorzSpeed along the heading mPrevAngleY. */
    s32 mSpeedX;            /* 0x0a4 */
    s32 mVertSpeed;         /* 0x0a8 */
    s32 mSpeedZ;            /* 0x0ac */
    /* Two populations of bits share this word. The profile descriptor's +0x08
     * word is copied in verbatim by the constructor; the framework then sets and
     * clears its own bits in the same storage. Across the 353 soundly-parsed
     * actor descriptors no profile authors any runtime bit, and the runtime
     * writes none of the authored ones -- so the split below is a measured
     * partition, not a guess. Census and evidence:
     * notes/profile-lifecycle-crosswalk.md.
     *
     * AUTHORED BY THE PROFILE, with a recovered consumer:
     *   0x00000001 enables the clip test; with 0x08 set, skip behaviour
     *   0x00000002 enables the clip test; with 0x08 set, skip render
     *   0x00010000 defeat the global force-think override while invisible
     *   0x08000000 spawn even if the death table says already killed
     * AUTHORED BY THE PROFILE, consumer not yet recovered:
     *   0x04  0x80  0x200  0x8000  0x800000  0x1000000  0x2000000
     *   0x4000000  0x10000000  0x20000000  0x40000000
     * WRITTEN BY THE FRAMEWORK, never by a profile:
     *   0x000008 off screen        0x000010 beyond mFarDistance (also kills the
     *   0x000020 area not showing    drop shadow)
     *   0x020000 / 0x040000 yoshi-mouth states, written by actor code
     * AfterInitResources seeds 0x38, so an actor starts fully invisible. */
    u32 mFlags;             /* 0x0b0 -- see the bit table above */
    /* The clip volume, all four set together by SetRanges out of the actor's
       entry in the spawn-info table. BeforeBehavior projects
       (mPosX, mPosY + mClipOffsetY, mPosZ) into camera space and hands the
       result to the dClipper; a zero radius means "no clip volume" and skips the
       transform outright. */
    s32 mClipOffsetY;       /* 0x0b4 */
    s32 mClipRadius;        /* 0x0b8 */
    s32 mClipDistance;      /* 0x0bc -- past this the actor is off screen AND far away */
    s32 mFarDistance;       /* 0x0c0 -- past this it is merely far away */
    u8  mClipResult;        /* 0x0c4 -- written by the dClipper */
    /* Pointer alignment leaves 0x0c5..0x0c7 unused. Shared by carried actors
       and attached objects; zero selects the actor's own model matrix. */
    struct Matrix4x3 *mCarryMatrix; /* 0x0c8 */
    s8  mAreaId;            /* 0x0cc -- negative means "not area-bound" */
    u8  pad_0cd[0x1];
    s16 mDeathTableID;      /* 0x0ce -- index into the already-killed bitmap */

    /* --- vtable. Declared first, see the header comment. Overrides slots
           16 (D1) and 17 (D0); position here does not affect that. --- */
    virtual ~dActor_c();

    /* C1 and C2 each compile the same real constructor definition in separate
       one-function sources. The rich spawn-seeding body still needs its exact
       block structure for frame-slot control. Declaring it lets a real derived constructor emit
       `bl _ZN8dActor_cC2Ev` instead of inlining the implicit base
       construction (notes/ctor-migration.md section 2); it is not the key
       function, so no TU gains a vtable from this. */
    dActor_c();

    /* The 24 unnamed parameters in this class, each left off because a name
       would cost an @NNN number that the vector overlays above spend instead:
         Player & / dActor_c &: slots 19 and 21-28, BumpedUnderneathByPlayer,
           JumpedOnByPlayer's Player, GivePlayerCoins;
         dCc_c & / dCc_c *: FindEgg, FindExplosionActor, MakeVanishLuigiWork,
           JumpedOnByPlayer, UpdatePos, UpdatePosWithOnlySpeed;
         dExtShadowModel_c &: DropShadowScaleXYZ, DropShadowRadHeight;
         dust position: PoofDustAt, DisappearPoofDustAt, TriplePoofDustAt,
           HugeLandingDustAt (LandingDustAt's `pos` was already named).
       Naming one, or unnaming another, moves every consumer's @NNN. */

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
    virtual void OnTurnIntoEgg(Player &);              /* slot 19 */
    virtual int  Virtual50();                          /* slot 20 -- vtable+0x50 */
    /* Slots 21, 24 and 27 return void, NOT int. Nothing in the tree reads them,
       so only an override with early returns can tell the difference -- and one
       of each does, allocating registers differently under `int` even with r0
       untouched. Measured, not assumed; see notes/actor-core-provenance.md 9. */
    virtual void OnGroundPounded(dActor_c &);          /* slot 21 */
    virtual int OnAttacked1(dActor_c &);               /* slot 22 */
    virtual int OnAttacked2(dActor_c &);               /* slot 23 */
    virtual void OnKicked(dActor_c &);                 /* slot 24 */
    virtual int OnPushed(dActor_c &);                  /* slot 25 */
    virtual int OnHitByCannonBlastedChar(dActor_c &);  /* slot 26 */
    virtual void OnHitByMegaChar(Player &);            /* slot 27 */
    virtual int OnHitFromUnderneath(dActor_c &);       /* slot 28 */
    virtual int  OnAimedAtWithEgg();                   /* slot 29 */
    /* Returns a Vector3 by value, and the ROM says so: 0x020100dc writes x/y/z
       through r0 and reads every field off r1, the AAPCS indirect-return shape.
       An indirect RETURN is free; a by-value class PARAMETER is not -- see
       notes/mwccarm-codegen.md 6az. */
    virtual Vector3 OnAimedAtWithEggReturnVec();       /* slot 30 */

    /* --- non-virtual --- */
    /* ClosestPlayer walks the player table measuring from this actor and caches
       both ends in globals; the rest read that cache, which is why each calls
       ClosestPlayer first and then returns a global rather than its result.
       BeforeBehavior clears the cache at the top of every frame. */
    Player *ClosestPlayer();
    Player *ClosestNonVanishPlayer();
    Player *FarthestPlayer();
    s32     DistToCPlayer();

    /* maxDist is a whole-unit distance, not a fix12 one: it is shifted into
       20.12 at the comparison (`cmp r0, r4, lsl #12`). */
    s16  HorzAngleToCPlayer();
    s16  HorzAngleToFPlayer();
    s16  HorzAngleToCPlayerOrAng();
    /* Both took a by-value Fix12<int> in their old mangled names; measured
       2026-10-23, the honest parameter is the scalar Fix12i and both reproduce
       as real methods (0x02010e08/0x24 and 0x0200f70c/0x54).
       notes/mwccarm-codegen.md 6az. Parameters are unnamed on purpose: a
       named class-scope parameter consumes an @NNN number in every consumer.
       SetRanges(clipOffsetY, clipRadius, clipDistance, farDistance);
       IsTooFarAwayFromPlayer(threshold). */
    void SetRanges(Fix12i, Fix12i, Fix12i, Fix12i);
    int IsTooFarAwayFromPlayer(Fix12i);
    /* Intensity is a 0x1000-scaled Fix12i, not a by-value Fix12<int>. */
    void Earthquake(const Vector3 &, Fix12i);

    bool IsPlayerInRange(s32 maxDist);
    bool IsPlayerInRange(const Vector3 &pos, s32 maxDist);

    static dActor_c *FindWithActorID(u32 actorID, dActor_c *after);
    static dActor_c *Next(const dActor_c *after);

    /* Integrates mHorzSpeed along mAngleY and applies gravity. */
    void UpdatePosWithHorzSpeedAndAng();
    int BumpedUnderneathByPlayer(Player &);
    int  GetSubtraction(short a, short b);

    /* Rebuilds the scratch matrix at 0x020a0e68 from the carrying player's hand
       transform, writes this actor's own position out of it, and returns it. */
    Matrix4x3 *UpdateCarry(Player &player, const Vector3 &vec);

    /* The cylinder-collision group. Everything here reads a dCc_c the caller
       already has; see include/dCc_c.h for the layout.

       FindEgg and FindExplosionActor are the same function twice over -- "did
       the thing that hit me have an owner, and was the hit of my kind" -- and
       differ only in the hitFlags bit. Kept as two, because the ROM has two.

       UpdatePos takes a POINTER where the rest take references; the mangled name
       says P5dCc_c, not R. */
    dActor_c *FindEgg(dCc_c &);            /* hitFlags 0x2000 */
    dActor_c *FindExplosionActor(dCc_c &); /* hitFlags 0x4000 */
    void MakeVanishLuigiWork(dCc_c &);
    int JumpedOnByPlayer(dCc_c &, Player &);
    void UpdatePos(dCc_c *);
    void UpdatePosWithOnlySpeed(dCc_c *);

    /* The dust group: an `...At` worker taking a position, and a no-argument
       wrapper that copies this actor's own position into a stack Vector3 and
       calls it.

       NOT A TYPO: each landing-dust wrapper calls the OTHER name's worker.
       HugeLandingDust (0x0200fb4c) calls LandingDustAt and LandingDust
       (0x0200fc0c) calls HugeLandingDustAt -- verified against both bl targets.
       One side of each pair carries the wrong adjective; these are imported
       names and nothing in the bytes says which side. Deliberately not renamed. */
    void PoofDust();
    void PoofDustAt(const Vector3 &); /* particles 0x122, 0x123 */
    void SmallPoofDust();
    void DisappearPoofDustAt(const Vector3 &); /* particles 0x127, 0x128 */
    void TriplePoofDust();
    void TriplePoofDustAt(const Vector3 &); /* particles 0x124..0x126 */
    void LandingDust(bool doRaycast);
    void HugeLandingDust(bool doRaycast);
    void HugeLandingDustAt(Vector3 &, bool doRaycast);     /* particle 0xb2 */
    void LandingDustAt(Vector3 &pos, bool doRaycast);      /* particle 0xb1 */

    /* The death table: actors already killed, so they do not respawn. All three
       key off mDeathTableID. */
    int  GetBitInDeathTable();
    void KillAndTrackInDeathTable();
    void TrackInDeathTable();
    void UntrackInDeathTable();
    /* Returns the spawned daSoundObj_c: r0 is Spawn's result at the return,
       and daStar_c and Player keep or test it. */
    dActor_c *SpawnSoundObj(u32 soundObjParam);
    s32  GetWaterHeightWDW();
    bool DetectRaycastClsn(Vector3 &start, Vector3 &end, bool storeHitPos);

    static dActor_c *FindWithID(u32 id);

    /* Star tracking. A star marker is the on-screen glint showing where an
       uncollected star will appear; STAR_MARKERS (0x0209f40c) holds twelve.
       TrackStar claims the first free slot and returns its index, or -1.
       howToSpawnStar is u8 and the symbol must say so -- declared u32 the body
       picks up an extra `and #0xff` and misses. */
    s32    TrackStar(u32 starIdx, u32 markerType);
    void   UntrackStar(s8 &starID);
    dActor_c *UntrackAndSpawnStar(s8 &trackStarID, u32 starID,
                               const Vector3 &spawnPos, u8 howToSpawnStar);

    /* SpawnNumber puts up actor 0x14a, the floating score popup, and makes it
       follow `owner`. packLowNibble squeezes value into a nibble and tags it
       0x10. GivePlayerCoins takes Player by reference and then null-tests it --
       the mangled name says R6Player and the null test is real, so the source
       has to say both. */
    dActor_c *SpawnNumber(const Vector3 &pos, u32 value, bool packLowNibble,
                       u16 delay, dActor_c *owner);
    void GivePlayerCoins(Player &, u8 count, u32 coinKind);
    dActor_c *ClosestWithActorID(u32 actorID);

    /* areaID and deathTableID are s8/s16 and the symbol must say so: the body
       reads them `ldrsb`/`ldrsh`, and declared (s32, s32) it comes out 0x5c
       instead of 0x4c and misses. */
    static dActor_c *Spawn(u32 actorID, u32 spawnParam, const Vector3 &pos,
                        const Vector3_16 *rot, s8 areaID, s16 deathTableID);

    /* MEASURED 2026-10-26: the five Fix12 declarations below -- SpawnFireball,
       DropShadowScaleXYZ, ReflectAngle, SpawnCoins, DropShadowRadHeight -- are
       real methods now, bytes unchanged (IsPlayerInRange was already scalar).
       They used to be "declared for callers only" on the rule that a mangled
       name carrying a by-value class parameter can never be DEFINED as a
       method, because CW homes such a parameter to the stack (+0x14). The
       by-value class was never in the ROM -- it was our own spelling of a
       20.12 scalar -- so with Fix12i the rule does not apply. The parameter
       NAMES here are load-bearing: a class-scope named parameter consumes an
       @NNN number in every consumer, so they must not be dropped.
       notes/mwccarm-codegen.md 6az. */
    /* Returns the spawned ball; see the definition. */
    dActor_c *SpawnFireball(const Vector3 &pos, const Vector3_16 *rot, Fix12i horzSpeed,
                            Fix12i targetSpeed, u32 param1);
    bool IsPlayerInRange(Fix12i posX, Fix12i posY, Fix12i posZ, s32 maxDist);
    void DropShadowScaleXYZ(dExtShadowModel_c &, Matrix4x3 &matrix, Fix12i scaleX, Fix12i scaleY,
                            Fix12i scaleZ, u8 opacity);
    s16 ReflectAngle(Fix12i normalX, Fix12i normalZ, s16 angToReflect);
    /* count is signed; see the branch evidence at the definition. */
    void SpawnCoins(const Vector3 &pos, s32 count, Fix12i spread, s16 angle);
    void DropShadowRadHeight(dExtShadowModel_c &, Matrix4x3 &matrix, Fix12i radius, Fix12i depth,
                             u8 opacity);

    /* INLINE, AND ON THIS CLASS RATHER THAN A BASE -- both are load-bearing.
       CW builds D0 (slot 17) as "run the destructor, then call operator delete",
       and inlines it only when it is found in the class itself or its IMMEDIATE
       base. Without this every dActor_c-derived D0 calls the global _ZdlPv,
       which exists nowhere in this image, and comes out short. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvPN5mHeap6Heap_tE(ptr, data_020a0eac); }
};

#else

/* Flat layout for the C translation units, which can express neither the base
   class nor the virtual functions. 0x00..0x4f is fBase_c. Held to the C++
   spelling above field for field by the size assertion below. */
struct dActor_c {
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
    void *lifecycleState;   /* 0x048 */
    void *heap;             /* 0x04c */
    void *mListPrev;        /* 0x050 */
    void *mListNext;        /* 0x054 */
    struct dActor_c *mListOwner; /* 0x058 */
    /* The same vector overlays as the C++ layout above. */
    union {
        struct {
            s32 mPosX, mPosY, mPosZ;
        };
        Vector3 mPos; /* 0x05c */
    };
    union {
        struct {
            s32 mPrevPosX, mPrevPosY, mPrevPosZ;
        };
        Vector3 mPrevPos; /* 0x068 */
    };
    union {
        struct {
            s32 mCamSpacePosX, mCamSpacePosY, mCamSpacePosZ;
        };
        Vector3 mCamSpacePos; /* 0x074 */
    };
    union {
        struct {
            s32 mScaleX, mScaleY, mScaleZ;
        };
        Vector3 mScale; /* 0x080 */
    };
    union {
        struct {
            s16 mAngleX, mAngleY, mAngleZ;
        };
        struct Vector3_16 mAngle; /* 0x08c */
    };
    union {
        struct {
            s16 mPrevAngleX, mPrevAngleY, mPrevAngleZ;
        };
        struct Vector3_16 mPrevAngle; /* 0x092 */
    };
    s32 mHorzSpeed;         /* 0x098 */
    s32 mVertAccel;         /* 0x09c */
    s32 mTerminalVelocity;  /* 0x0a0 */
    /* mSpeedX, mVertSpeed and mSpeedZ are the velocity vector:
       UpdatePosWithOnlySpeed adds it to mPos, and UpdatePosWithHorzSpeedAndAng
       derives the x/z words from mHorzSpeed along the heading mPrevAngleY. */
    s32 mSpeedX;            /* 0x0a4 */
    s32 mVertSpeed;         /* 0x0a8 */
    s32 mSpeedZ;            /* 0x0ac */
    u32 mFlags;             /* 0x0b0 */
    s32 mClipOffsetY;       /* 0x0b4 */
    s32 mClipRadius;        /* 0x0b8 */
    s32 mClipDistance;      /* 0x0bc */
    s32 mFarDistance;       /* 0x0c0 */
    u8  mClipResult;        /* 0x0c4 */
    /* Pointer alignment leaves 0x0c5..0x0c7 unused. Shared by carried actors
       and attached objects; zero selects the actor's own model matrix. */
    struct Matrix4x3 *mCarryMatrix; /* 0x0c8 */
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 mDeathTableID;      /* 0x0ce */
};

#endif /* __cplusplus */

/* Outside the split, so the C and C++ spellings cannot drift apart. 0xd0 is what
   this header's own field list computes -- it is not independent ROM evidence --
   but it holds the two spellings to each other, catches a field retyped without
   shrinking the pad after it, and gives tools/check_header_offsets.py the base
   size it refuses to guess when checking include/Player.h. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dActor_c_size_must_be_0xd0[sizeof(struct dActor_c) == 0xd0 ? 1 : -1];
#endif

#endif
