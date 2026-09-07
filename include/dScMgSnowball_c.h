/* class dScMgSnowball_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgSnowball_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0214000c; the RTTI
 * pointer one word below it is _ZTI15dScMgSnowball_c, which is what ties
 * that table to this class rather than to a neighbour.
 *
 * SIZE 0xc59c, and the allocation is NOT in this class's own constructor.
 * func_ov006_021295ac takes an already-allocated `char*` -- it is a C2,
 * not a factory. Its one caller, dScMgSnowball_c_classInit, is where
 * `_ZN7fBase_cnwEj(0xc59c)` lives. Deriving a size from the field span
 * would have stopped at 0xbe94 and been 0x708 short.
 *
 * CONSTRUCTOR AND DESTRUCTOR AGREE MEMBER FOR MEMBER, in exact reverse
 * order: mModel at 0xaba4, then mArray1 at 0xacd8 (0x80 elements of 8),
 * mArray2 at 0xb5d8 (0x80 of 8), mArray3 at 0xba14 (0x20 of 0x24). The
 * destructor destroys mArray3, mArray2, mArray1, then mModel. Nothing is
 * left over either way.
 *
 * mModel IS A REAL, TYPED Model, and two independent witnesses fix it:
 * the constructor calls `_ZN5ModelC1Ev(t + 0xaba4)` and the destructor
 * calls `_ZN5ModelD1Ev(c + 0xaba4)`, and `sizeof(Model)` is 0x50, which
 * closes exactly on 0xabf4 -- the very next word the constructor writes
 * (`*(int*)(t + 0xabf4) = 0`). That is the "two witnesses that close on
 * each other with nothing left over" bar. The Model destruction is
 * therefore left COMPILER-GENERATED here rather than written out: mModel
 * is declared before the three arrays, so reverse-declaration order puts
 * its destructor call last, which is exactly where the ROM has it.
 *
 * THE THREE ARRAYS stay raw bytes -- no source in the tree reads inside
 * an element, so no element type is evidenced. mArray1 and mArray2 share
 * an element ctor/dtor pair (func_0203d738 / NullDestructor_0203d47c), so
 * they are the same element type as each other; mArray3's is its own
 * (func_ov006_0212968c / func_ov006_02125800).
 *
 * THE 0xb0d8..0xb5d8 REGION IS FOUR PARALLEL ARRAYS, not padding, and they
 * tile the 0x500 exactly: Snowball's own Render (src/func_ov006_02127d10.c)
 * indexes 0xb0d8 as `*(int*)(c + i*4 + ...)`, 0xb2d8 and 0xb358 as
 * `*(u8*)(c + i + ...)`, and 0xb3d8 as `*(int*)(c + i*4 + ...)`; 0x80
 * elements each gives 0x200 + 0x80 + 0x80 + 0x200 = 0x500, landing on
 * mArray2. src/func_ov006_02125bbc.c writes the first two the same way.
 *
 * 0xbe94..0xc59c IS A FIFTH ARRAY, and it is why this class has NO trailing
 * slack at all: src/func_ov006_02129690.c already carries the full element
 * layout (two s32 then three u8 at +0x1e/0x1f/0x20, stride 0x24) as a local
 * `Elem arr[50]` at exactly this offset, and 50 * 0x24 = 0x708 closes
 * precisely on the allocation literal. At least eight further ov006 files
 * index it. Unlike the other four arrays neither structor touches it, so
 * its element type is trivially constructible -- which is exactly why the
 * structors alone could never have found it.
 *
 * So the field span and the `operator new` literal now AGREE for this
 * class. The size does not rest on the literal alone.
 *
 * mMenuOpen, which the old auto-generated header declared, is dScMgBase_c's
 * own and is already declared there; it is dropped from this file, not
 * lost. src/_ZN15dScMgSnowball_c8OnKickedEv.c reads it through this class
 * and keeps working because it inherits it.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN15dScMgSnowball_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one.
 *
 * SM64DS RTTI names the implementation dScMgSnowball_c. The reconstructed factory
 * dScMgSnowball_c_classInit (historical alias MgSnowballSlalom_Spawn) installs this class's
 * cartridge vtable for the MG_SNOWBALL registry profile.
 */
#ifndef DSCMGSNOWBALL_C_H
#define DSCMGSNOWBALL_C_H
#include "dScMgSingle3DBase_c.h"
#include "Model.h"

extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_02125800(void);
extern "C" void NullDestructor_0203d47c(void);

struct dScMgSnowball_c : dScMgSingle3DBase_c {
    /* The 2-vector value type the collision ring carries the last hit in
       (Fix12 x, y -- the same pair func_0203d388 rotates in place). THE
       EMPTY DESTRUCTOR IS LOAD-BEARING, for the reason types.h gives
       Vector3 one and dScMgFlower_c::Vec2 repeats: a local of this type
       keeps an 8-byte stack home in the frame's aggregate region instead
       of being scalarised into two spill words, and func_ov006_02125f68's
       frame only lays out as the ROM's (sample at sp+0x38, this pair at
       sp+0x40, acc at sp+0x48) with that home -- a plain POD pair, an
       int[2] and two scalar temps all lose it, measured on 2004/b56.
       Nested so the coined name cannot collide with anything global. */
    struct Vec2 {
        Fix12i x;
        Fix12i y;
        ~Vec2() {}
    };

    virtual ~dScMgSnowball_c();

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources();     /* slot 0 -- ov006 0x02129268 */
    virtual s32 CleanupResources();  /* slot 3 -- ov006 0x021291f8 */
    virtual s32 Behavior();          /* slot 6 -- ov006 0x021283a4 */
    virtual s32 Render();            /* slot 9 -- ov006 0x02127d10 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnAttacked2();                        /* slot 23 */
    virtual int  OnKicked();                           /* slot 24 */
    virtual int  OnPushed();                           /* slot 25 */

    /* 0x4f38 -- the generated course, 0x10 lanes of 0x2e0 rows. The stride
       is the lane's: func_ov006_02126ee4 writes every tile as
       `this + 0x4f38 + lane * 0x5c0 + row * 2`, so the lane index carries
       0x5c0 = 0x2e0 * sizeof(u16) and the row index the halfword. 0x10 lanes
       times 0x5c0 is 0x5c00, which closes exactly on mPosX. */
    u16   mTileMap[0x10][0x2e0]; /* 0x4f38 */
    s32   mPosX;            /* 0xab38 -- Fix12 world position of the ball */
    s32   mPosY;            /* 0xab3c */
    s32   mPrevPosX;        /* 0xab40 -- Behavior's copy of mPos at tick start */
    s32   mPrevPosY;        /* 0xab44 */
    s32   mDrawPosX;        /* 0xab48 -- where Render puts the ball sprite */
    s32   mDrawPosY;        /* 0xab4c */
    s32   mSoundPosX;       /* 0xab50 -- position of the last roll sound; a new
                                one fires every 0x30000 of travel */
    s32   mSoundPosY;       /* 0xab54 */
    s32   unk_ab58;         /* 0xab58 -- downhill/uphill debt; while it is 0 the
                                climb feeds mBallSize instead */
    s32   unk_ab5c;         /* 0xab5c -- func_ov006_02126ee4 clears it, then
                                writes the generated corridor's centre lane in
                                Fix12 on the row before the goal */
    s32   mVelX;            /* 0xab60 -- Fix12, capped at 0x8000 */
    s32   mVelY;            /* 0xab64 */
    s32   mScrollX;         /* 0xab68 -- subtracted from every world X to draw */
    s32   mScrollY;         /* 0xab6c -- mPosY - 0x190000, clamped; drives all
                                four BG offsets */
    s32   mTouchX;          /* 0xab70 -- last stylus sample */
    s32   mTouchY;          /* 0xab74 */
    u16   mRollAngle;       /* 0xab78 -- += speed * 0x2710 / mBallSize */
    u16   unk_ab7a;         /* 0xab7a */
    u16   mHeadingAngle;    /* 0xab7c -- atan2 of the velocity */
    u16   mPrevRollAngle;   /* 0xab7e -- Behavior copies 0xab78..0xab7c here */
    u16   unk_ab80;         /* 0xab80 */
    u16   mPrevHeadingAngle;/* 0xab82 */
    s32   mSpinAxis[3];     /* 0xab84 -- Vector3, seeded (0, 0x1000, 0) */
    s32   mSpinQuat[4];     /* 0xab90 -- Quaternion_FromVector3 of mSpinAxis */
    s32   mBallSize;        /* 0xaba0 -- grows with the climb, capped 0x37000,
                                shrinks by 0x1000 a tick while melting */
    Model mModel;           /* 0xaba4 -- 0x50, see file banner */
    s32   unk_abf4;         /* 0xabf4 -- the constructor's own `= 0` write */
    u8    mProbeHit[0x20];  /* 0xabf8 -- one flag per 32-step angle probe: set when that probe hit solid or breakable */
    u8    mProbePush[0x20]; /* 0xac18 -- same probe hit solid; drives the push-out */
    u8    mProbeWater[0x20];/* 0xac38 -- same probe is inside water */
    u8    mArray1Active[0x80]; /* 0xac58 -- 1 = this mArray1 slot is live */
    s32   mArray1[0x80][2]; /* 0xacd8 -- 0x80 * 8, elem dtor NullDestructor_0203d47c;
                               each element is a Fix12 {x,y} the ROM's own
                               Render indexes as i*8 + 0 / i*8 + 4 */
    s32   mArray1Kind[0x80]; /* 0xb0d8 -- 1 picks the 8-frame animated sprite
                                table, anything else the static one */
    u8    mArray1Hit[0x80]; /* 0xb2d8 -- src/func_ov006_02125bbc.c sets it on
                               contact; Render then offsets the frame by 8 */
    u8    mArray2Active[0x80]; /* 0xb358 -- 1 = this mArray2 slot is live */
    s32   mArray2Kind[0x80]; /* 0xb3d8 -- Render switches 0..2 against 3 to
                                pick the sprite */
    s32   mArray2[0x80][2]; /* 0xb5d8 -- 0x80 * 8, elem dtor NullDestructor_0203d47c;
                               same Fix12 {x,y} element shape as mArray1 */
    s32   mAnimCounter;     /* 0xb9d8 -- Render bumps it, wraps at 0x20; the
                                obstacle sprite frame is (n / 4) & 7 */
    s32   mTimeLeft;        /* 0xb9dc -- frames; Render draws it as
                                seconds.centiseconds, 0 ends the run */
    s32   mScore;           /* 0xb9e0 -- +1 a tick while rolling; handed to the
                                HUD counter func_ov004_020adb1c on the crash */
    u8    mHitBreakable;    /* 0xb9e4 -- a breakable was hit this tick */
    u8    mAllWater;        /* 0xb9e5 -- every probe was in water */
    u8    mSoundPending;    /* 0xb9e6 */
    u8    pad_b9e7[0x1];    /* 0xb9e7 */
    s32   unk_b9e8;         /* 0xb9e8 */
    u8    pad_b9ec[0x8];    /* 0xb9ec */
    s32   mState;           /* 0xb9f4 -- 0 count-in, 1 rolling, 2/3 crash,
                                4 melt, 5 over */
    u8    mScreensSwapped;  /* 0xb9f8 -- set once mPosY passes 0xe8000; swaps
                                which physical screen shows the upper half */
    u8    pad_b9f9[0x3];    /* 0xb9f9 */
    s32   mCountdownTimer;  /* 0xb9fc -- 0xf1 down to 0x3c; Render draws the
                                3-2-1 banner from it */
    s32   mStartY;          /* 0xba00 -- mPosY starts here (<< 12) */
    s32   mGoalY;           /* 0xba04 -- the finish; also the progress bar's
                                other end */
    s32   mScrollLimit;     /* 0xba08 -- mScrollY stops at (n << 16) - 0x1d0000 */
    s32   mCrashTimer;      /* 0xba0c -- counts up through states 2 and 4 */
    s32   mEndDelayTimer;   /* 0xba10 -- 0xb4 frames of state 3 */
    u8    mArray3[0x480];   /* 0xba14 -- 0x20 * 0x24, elem dtor func_ov006_02125800 */
    u8    mArray4[0x708];   /* 0xbe94 -- 50 * 0x24, trivial elements, see file banner */
};

typedef char dScMgSnowball_c_size_must_be_0xc59c[sizeof(dScMgSnowball_c) == 0xc59c ? 1 : -1];

#endif
