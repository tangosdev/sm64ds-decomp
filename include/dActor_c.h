#ifndef DACTOR_C_H
#define DACTOR_C_H

#include "types.h"

#ifdef __cplusplus
#include "dBase_c.h"
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
 * Putting ~dActor_c first pins that role to the D0/D1/D2 files, which by
 * construction define extern "C" free functions and never a real
 * `dActor_c::~dActor_c`. Keep it that way.
 *
 * Offsets, widths and vtable slots are pinned by the bytes; field names are not
 * and are safe to improve. Provenance for all of it: notes/actor-core-provenance.md,
 * notes/actor-vtables.md.
 */

#ifdef __cplusplus

struct Player;
struct Vector3;
struct Vector3_16;
struct dCc_c;
/* Only ever named through a pointer below; the definition lives in common.h /
   math/Matrix.h, which this header deliberately does not pull in. */
struct Matrix4x3;
struct ShadowModel;
#include "math/Fix12.h"

/* The actor heap and its deallocator, for the inline operator delete at the end of
   the class. data_020a0eac is the heap every actor is allocated from.

   SPELT EXACTLY AS include/decl_common.h SPELLS IT. The honest second parameter is
   Heap*, but two incompatible extern "C" declarations of one name in a single TU
   are rejected as "illegal function overloading" -- that cost 105 files their
   eligibility when it was tried. Fix it in decl_common.h or not at all. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

struct dActor_c : dBase_c {
    /* The global actor list (data_0209b468) threads through every actor: the
       constructor appends this node, the destructor unlinks it, and Next() walks
       mListNext and then reads that node's mListOwner to get back to the actor. */
    void *mListPrev;        /* 0x050 */
    void *mListNext;        /* 0x054 */
    dActor_c *mListOwner;   /* 0x058 -- always `this` */
    s32 mPosX;              /* 0x05c */
    s32 mPosY;              /* 0x060 */
    s32 mPosZ;              /* 0x064 */
    s32 mPrevPosX;          /* 0x068 -- last frame's position, snapshotted by BeforeBehavior */
    s32 mPrevPosY;          /* 0x06c */
    s32 mPrevPosZ;          /* 0x070 */
    s32 mCamSpacePosX;      /* 0x074 */
    s32 mCamSpacePosY;      /* 0x078 */
    s32 mCamSpacePosZ;      /* 0x07c */
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;        /* 0x092 */
    s16 mPrevAngleY;        /* 0x094 */
    s16 mPrevAngleZ;        /* 0x096 */
    s32 mHorzSpeed;         /* 0x098 */
    s32 mVertAccel;         /* 0x09c -- fix12, negative (gravity) */
    s32 mTerminalVelocity;  /* 0x0a0 -- fix12, negative */
    s32 unk_0a4;            /* 0x0a4 */
    s32 mVertSpeed;         /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u32 mFlags;             /* 0x0b0 -- bit 0x10000 suppresses behaviour */
    /* The clip volume, all four set together by SetRanges out of the actor's
       entry in the spawn-info table. BeforeBehavior projects
       (mPosX, mPosY + mClipOffsetY, mPosZ) into camera space and hands the
       result to the Clipper; a zero radius means "no clip volume" and skips the
       transform outright. */
    s32 mClipOffsetY;       /* 0x0b4 */
    s32 mClipRadius;        /* 0x0b8 */
    s32 mClipDistance;      /* 0x0bc -- past this the actor is off screen AND far away */
    s32 mFarDistance;       /* 0x0c0 -- past this it is merely far away */
    u8  mClipResult;        /* 0x0c4 -- written by the Clipper */
    u8  pad_0c5[0x7];
    s8  mAreaId;            /* 0x0cc -- negative means "not area-bound" */
    u8  pad_0cd[0x1];
    s16 mDeathTableID;      /* 0x0ce -- index into the already-killed bitmap */

    /* --- vtable. Declared first, see the header comment. Overrides slots
           16 (D1) and 17 (D0); position here does not affect that. --- */
    virtual ~dActor_c();

    /* DECLARED, never defined as a method -- src/_ZN8dActor_cC1Ev.cpp and
       src/_ZN8dActor_cC2Ev.cpp provide the symbol as hand-spelt extern "C"
       bodies (the rich spawn-seeding logic needs exact frame-slot control).
       Declaring it is what lets a real derived constructor emit
       `bl _ZN8dActor_cC2Ev` instead of inlining the implicit base
       construction (notes/ctor-migration.md section 2); it is not the key
       function, so no TU gains a vtable from this. */
    dActor_c();

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
    /* Slots 21, 24 and 27 return void, NOT int. Nothing in the tree reads them,
       so only an override with early returns can tell the difference -- and one
       of each does, allocating registers differently under `int` even with r0
       untouched. Measured, not assumed; see notes/actor-core-provenance.md 9. */
    virtual void OnGroundPounded(dActor_c &other);        /* slot 21 */
    virtual int  OnAttacked1(dActor_c &other);            /* slot 22 */
    virtual int  OnAttacked2(dActor_c &other);            /* slot 23 */
    virtual void OnKicked(dActor_c &other);               /* slot 24 */
    virtual int  OnPushed(dActor_c &other);               /* slot 25 */
    virtual int  OnHitByCannonBlastedChar(dActor_c &other); /* slot 26 */
    virtual void OnHitByMegaChar(Player &player);      /* slot 27 */
    virtual int  OnHitFromUnderneath(dActor_c &other);    /* slot 28 */
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
    bool IsPlayerInRange(s32 maxDist);
    bool IsPlayerInRange(const Vector3 &pos, s32 maxDist);

    static dActor_c *FindWithActorID(u32 actorID, dActor_c *after);

    /* Integrates mHorzSpeed along mAngleY and applies gravity. */
    void UpdatePosWithHorzSpeedAndAng();
    int  BumpedUnderneathByPlayer(Player &player);
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
    dActor_c *FindEgg(dCc_c &clsn);               /* hitFlags 0x2000 */
    dActor_c *FindExplosionActor(dCc_c &clsn);    /* hitFlags 0x4000 */
    void   MakeVanishLuigiWork(dCc_c &clsn);
    int    JumpedOnByPlayer(dCc_c &clsn, Player &player);
    void   UpdatePos(dCc_c *clsn);
    void   UpdatePosWithOnlySpeed(dCc_c *clsn);

    /* The dust group: an `...At` worker taking a position, and a no-argument
       wrapper that copies this actor's own position into a stack Vector3 and
       calls it.

       NOT A TYPO: each landing-dust wrapper calls the OTHER name's worker.
       HugeLandingDust (0x0200fb4c) calls LandingDustAt and LandingDust
       (0x0200fc0c) calls HugeLandingDustAt -- verified against both bl targets.
       One side of each pair carries the wrong adjective; these are imported
       names and nothing in the bytes says which side. Deliberately not renamed. */
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

    /* The death table: actors already killed, so they do not respawn. All three
       key off mDeathTableID. */
    int  GetBitInDeathTable();
    void KillAndTrackInDeathTable();
    void TrackInDeathTable();
    void UntrackInDeathTable();
    void SpawnSoundObj(u32 soundObjParam);
    s32  GetWaterHeightWDW();

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
    void   GivePlayerCoins(Player &player, u8 count, u32 coinKind);
    dActor_c *ClosestWithActorID(u32 actorID);

    /* areaID and deathTableID are s8/s16 and the symbol must say so: the body
       reads them `ldrsb`/`ldrsh`, and declared (s32, s32) it comes out 0x5c
       instead of 0x4c and misses. */
    static dActor_c *Spawn(u32 actorID, u32 spawnParam, const Vector3 &pos,
                        const Vector3_16 *rot, s8 areaID, s16 deathTableID);

    /* Declared for callers only. Methods whose mangled names carry a by-value
       class parameter must never be DEFINED as methods -- CW homes those to the
       stack, costing +0x14. See notes/mwccarm-codegen.md 6az. */
    void SpawnFireball(const Vector3 &pos, const Vector3_16 *rot,
                        Fix12<int> horzSpeed, Fix12<int> unk35c, u32 param1);
    bool IsPlayerInRange(Fix12<int> posX, Fix12<int> posY, Fix12<int> posZ,
                          s32 maxDist);
    void DropShadowScaleXYZ(ShadowModel &shadow, Matrix4x3 &matrix,
                             Fix12<int> scaleX, Fix12<int> scaleY,
                             Fix12<int> scaleZ, u32 opacity);
    s16  ReflectAngle(Fix12<int> normalX, Fix12<int> normalZ, s16 angToReflect);
    void SpawnCoins(const Vector3 &pos, u32 count, Fix12<int> spread, s16 angle);
    void DropShadowRadHeight(ShadowModel &shadow, Matrix4x3 &matrix,
                              Fix12<int> radius, Fix12<int> depth, u32 opacity);

    /* INLINE, AND ON THIS CLASS RATHER THAN A BASE -- both are load-bearing.
       CW builds D0 (slot 17) as "run the destructor, then call operator delete",
       and inlines it only when it is found in the class itself or its IMMEDIATE
       base. Without this every dActor_c-derived D0 calls the global _ZdlPv,
       which exists nowhere in this image, and comes out short. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }
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
    void *unk_048;          /* 0x048 */
    void *heap;             /* 0x04c */
    void *mListPrev;        /* 0x050 */
    void *mListNext;        /* 0x054 */
    struct dActor_c *mListOwner; /* 0x058 */
    s32 mPosX;              /* 0x05c */
    s32 mPosY;
    s32 mPosZ;
    s32 mPrevPosX;          /* 0x068 */
    s32 mPrevPosY;
    s32 mPrevPosZ;
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
    s32 unk_0a4;            /* 0x0a4 */
    s32 mVertSpeed;         /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u32 mFlags;             /* 0x0b0 */
    s32 mClipOffsetY;       /* 0x0b4 */
    s32 mClipRadius;        /* 0x0b8 */
    s32 mClipDistance;      /* 0x0bc */
    s32 mFarDistance;       /* 0x0c0 */
    u8  mClipResult;        /* 0x0c4 */
    u8  pad_0c5[0x7];
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
typedef char dActor_c_size_must_be_0xd0[sizeof(struct dActor_c) == 0xd0 ? 1 : -1];

#endif
