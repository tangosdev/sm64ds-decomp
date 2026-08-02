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
    virtual int  OnAimedAtWithEggReturnVec();          /* slot 30 */

    /* --- non-virtual --- */
    int  BumpedUnderneathByPlayer(Player &player);
    int  GetSubtraction(short a, short b);
    void HugeLandingDustAt(Vector3 &pos, bool b);
    void LandingDustAt(Vector3 &pos, bool b);
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

#endif
