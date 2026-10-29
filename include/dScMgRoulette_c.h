/* dScMgRoulette_c -- Mushroom Roulette. The dealer hands out racers, the
 * player drops them on the board's tiles, and a ball spins round the wheel;
 * the racers on the tile it stops on pay out. Child of dScMgSingle3DBase_c,
 * size 0x5400.
 *
 * RTTI: dScMgRoulette_c : dScMgSingle3DBase_c (tools/rtti_extract.py).
 * Factory dScMgRoulette_c_classInit; historical alias MgMushroomRoulette_Spawn.
 * ROM: D1 0x0210788c below D0 0x02107920, no D2; out of line would emit
 * D2, D0, D1. InitResources is then the first declared non-inline virtual,
 * so it is the key function; this TU licenses _ZTV/_ZTI/_ZTS in
 * compiler_only_output.
 *
 * The destructor is inline and declared first, so D1 comes out before D0
 * and no D2 is emitted. Its body is empty: the compiler tears the members
 * down in reverse declaration order (mBoard's ball and wheel, the racer
 * array through __cxa_vec_cleanup, then mDealer), which is the ROM's order.
 * The factory is `new dScMgRoulette_c`, so the same members are built in
 * declaration order. InitResources stays the key function. dScMgBase_c
 * supplies operator new and delete.
 */
#ifndef DSCMGROULETTE_C_H
#define DSCMGROULETTE_C_H
#include "dScMgSingle3DBase_c.h"
#include "BMD_File.h"
#include "Dealer.h"
#include "dExtSimpleModel_c.h"

/* One racer the dealer hands out, 0x34 bytes; the scene holds five at
 * 0x51a8. The name is coined: the record has no vtable, so no RTTI names it.
 * Its constructor (ov006 0x0210a4ac, in d_s_mg_roulette.cpp) and destructor
 * (0x021079c8, in d_s_mg_roulette_methods.cpp) are both empty, and both are
 * out of line: the scene builds and tears down the array through
 * __cxa_vec_ctor / __cxa_vec_cleanup, which take their addresses.
 * x/y slide toward targetX/targetY by stepX/stepY. state: 0 handled with 3,
 * 1 sliding, 2 under the stylus, 3 stopped, 4 about to hop, 5 hopping,
 * 6 settled, 7 sent to the side pile. */
struct RouletteRacer {
    RouletteRacer();
    ~RouletteRacer();

    s32 x;        /* 0x00 */
    s32 y;        /* 0x04 */
    s32 grabX;    /* 0x08 */
    s32 grabY;    /* 0x0c */
    s32 stepX;    /* 0x10 */
    s32 stepY;    /* 0x14 */
    s32 targetX;  /* 0x18 */
    s32 targetY;  /* 0x1c */
    s32 homeX;    /* 0x20, the deal spot AimAt sends it to; Release returns it there */
    s32 homeY;    /* 0x24 */
    s32 priority; /* 0x28, OAM priority for RenderOamBothScreens; Draw resets it
                     to 1, RouletteStrip_DrawRacers lifts it to 0 */
    s16 tile;     /* 0x2c, Roulette_TileAt's tile; 0x25 is off the board */
    s16 payout;   /* 0x2e, GetPayout's multiplier for the winning tile */
    s16 hop;      /* 0x30 */
    u8 state;     /* 0x32 */
    u8 unk33;     /* 0x33 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char RouletteRacer_size_must_be_0x34[sizeof(RouletteRacer) == 0x34 ? 1 : -1];
#endif

struct RouletteBoard;
typedef void (RouletteBoard::*RouletteBoardState)();

/* The wheel and the ball that orbits it, 0xd4 bytes at the scene's 0x530c.
 * The name is coined (no vtable, so no RTTI). It declares no constructor or
 * destructor of its own: the compiler's implicit ones build and destroy
 * mWheel and mBall, and inline into the scene's. The RouletteBoard_ helpers
 * in d_s_mg_roulette_methods.cpp run its state. */
struct RouletteBoard {
    RouletteBoardState mState; /* 0x00 -- RouletteBoard_StateSpin, then StateSlow */
    BMD_File *mWheelFile;      /* 0x08 -- file 0x21d */
    BMD_File *mBallFile;       /* 0x0c -- file 0x21e */
    dExtSimpleModel_c mWheel;  /* 0x10 -- turned by mBoardAngle */
    dExtSimpleModel_c mBall;   /* 0x60 -- placed each frame from mBallRadius and mOrbitAngle */
    s32 mBallRadius;           /* 0xb0 -- Fix12; 0x4200 when the spin starts */
    s32 mBallRadiusVel;        /* 0xb4 -- added to mBallRadius each frame */
    s32 mSpinSpeed;            /* 0xb8 -- the ball's speed; at 0 it has stopped:
                                  Render draws the cursor and phase 3 pays out */
    s32 mRollSoundHandle;      /* 0xbc -- Sound_PlayIfNotActive's handle */
    s16 mOrbitAngle;           /* 0xc0 -- the ball's angle round the wheel */
    s16 mBoardAngle;           /* 0xc2 -- the wheel's own angle */
    s16 mSlowBias;             /* 0xc4 -- -mBoardAngle when the slowdown starts */
    s16 mAngleStep;            /* 0xc6 -- mOrbitAngle's step this frame */
    s16 mBoardSpin;            /* 0xc8 -- mBoardAngle's step, -0x200 */
    s16 mStopTile;             /* 0xca -- the tile under the ball
                                  (data_ov006_0212ed34); phase 3 scores against it */
    s16 mRollSound;            /* 0xcc -- 0 quiet; 1 on the rim (sound 0x157); 2 off
                                  the rim, in the bowl (0x158). Phase 1 sets 1 once
                                  the dealer is done; striking a stop plays 0x159
                                  and clears it */
    s16 pad_ce;                /* 0xce */
    s32 mBraking;              /* 0xd0 -- phase 2 sets it; the spin then eases to 0 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char RouletteBoard_size_must_be_0xd4[sizeof(RouletteBoard) == 0xd4 ? 1 : -1];
#endif

struct dScMgRoulette_c : dScMgSingle3DBase_c {
    /* Inline and declared first: see the file banner. */
    virtual ~dScMgRoulette_c() {}

    /* --- this class's own vtable slots, named from the table ---
       Re-overrides of slots fBase_c already owns, NOT new virtuals: the
       table stays the base's width and no field moves. Declared AFTER the
       destructor so the destructor is still the first virtual declared. */
    virtual s32 InitResources();     /* slot 0 -- src/_ZN15dScMgRoulette_c13InitResourcesEv.cpp */
    virtual s32 CleanupResources();  /* slot 3 -- ov006 0x0210980c */
    virtual s32 Behavior();          /* slot 6 -- ov006 0x02109aac */
    virtual s32 Render();            /* slot 9 -- ov006 0x02109834 */
    virtual void StartRound(int arg);               /* slot 18 */
    virtual int  CanEndState(int mode);              /* slot 19 */

    Dealer mDealer;           /* 0x4f38 */
    RouletteRacer mRacers[5]; /* 0x51a8 */
    u8 mMeter[4][0x18];       /* 0x52ac -- strips for payouts 12, 6, 3, 2 */
    RouletteBoard mBoard;     /* 0x530c */
    s32   unk_53e0;        /* 0x53e0 -- reset to 0x100; not read in this TU */
    s16   mCameraPreset;     /* 0x53e4 -- Render copies row n of the camera
                                tables (data_ov006_0213e34c / _0213e370 at stride
                                0xc, and _0213e2e0 for the angle) into
                                mCameraEye/mCameraTarget/mCameraAngle, then calls
                                Camera_UpdateMatrices. 0 leaves the camera alone */
    s16   mPhase;            /* 0x53e6 -- 1 deals the racers out, 2 runs the
                                countdown and reads the board, 3 pays out per
                                landed tile, 4 announces the result */
    s16   mPhaseTimer;       /* 0x53e8 -- counted down in every phase; each
                                phase reloads it on entry */
    s16   unk_53ea;          /* 0x53ea */
    u8    unk_53ec;          /* 0x53ec */
    u8    unk_53ed;          /* 0x53ed */
    u8    unk_53ee;          /* 0x53ee */
    u8    unk_53ef;          /* 0x53ef */
    u8    unk_53f0;          /* 0x53f0 */
    u8    pad_53f1[0x1];
    s16   mScore;            /* 0x53f2 -- the payout summed over the racers;
                                phase 4 compares it against mTargetScore */
    s16   unk_53f4;          /* 0x53f4 -- cleared when the payout is posted */
    s16   mTargetScore;      /* 0x53f6 -- one per racer that missed the winning
                                tile; the bar mScore has to beat */
    s32   mDealIndex;        /* 0x53f8 -- how many racers have been dealt out;
                                also reused as a loop cursor between phases */
    s32   mRacerCount;       /* 0x53fc -- the bound of every per-racer loop in
                                Behavior and Render */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgRoulette_c_size_must_be_0x5400[sizeof(dScMgRoulette_c) == 0x5400 ? 1 : -1];
#endif

#endif
