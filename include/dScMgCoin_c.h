#ifndef DSCMGCOIN_C_H
#define DSCMGCOIN_C_H
#include "dScMgBase_c.h"

/* Coincentration. One coin in the 24 that fall into the blocks.
   It falls at vy until it reaches landY, then bounces; once bounces
   catches value it sits in state 2. */
struct dScMgCoin_Coin {
    s32 x;            /* 0x00 */
    s32 y;            /* 0x04 */
    s32 landY;        /* 0x08 */
    s32 vy;           /* 0x0c */
    u8 state;         /* 0x10 */
    u8 pad11;         /* 0x11 */
    u8 visible;       /* 0x12 */
    u8 value;         /* 0x13 */
    u8 pad14;         /* 0x14 */
    u8 collected;     /* 0x15 -- Phase_StatePlay ends the round once every valued
                         coin has it; the result screen prints value only while 0 */
    u8 bounces;       /* 0x16 */
    u8 pad17;         /* 0x17 */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgCoin_Coin_size_must_be_0x18[sizeof(dScMgCoin_Coin) == 0x18 ? 1 : -1];
#endif

/* The running total just under the coins. shown walks up to total. */
struct dScMgCoin_Score {
    u16 total;        /* 0x00 */
    u16 shown;        /* 0x02 */
    u16 tick;         /* 0x04 */
    u8 pad06[5];      /* 0x06 */
    u8 running;       /* 0x0b */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgCoin_Score_size_must_be_0x0c[sizeof(dScMgCoin_Score) == 0x0c ? 1 : -1];
#endif

/* One of the 32 sparkles burst off a coin. */
struct dScMgCoin_Sparkle {
    s32 x;            /* 0x00 */
    s32 y;            /* 0x04 */
    s32 vx;           /* 0x08 */
    s32 vy;           /* 0x0c */
    u16 life;         /* 0x10 */
    u8 pad12[2];      /* 0x12 */
    u8 active;        /* 0x14 */
    u8 visible;       /* 0x15 */
    u8 sprite;        /* 0x16 */
    u8 pad17;         /* 0x17 */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgCoin_Sparkle_size_must_be_0x18[sizeof(dScMgCoin_Sparkle) == 0x18 ? 1 : -1];
#endif

/* The number that pops up over a coin that has just settled. */
/* One of the 40 blocks that hop down the stages (Block_StateHopDown).
   State 1 waits out delay (Block_StateWaiting), 2 has passed its own row,
   4 has landed and scored (Block_StateFallIn). */
struct dScMgCoin_Block {
    s32 x;            /* 0x00 Fix12 */
    s32 y;            /* 0x04 */
    s32 vx;           /* 0x08 */
    s32 vy;           /* 0x0c */
    s16 delay;        /* 0x10 */
    u8 pad12[2];      /* 0x12 */
    u8 row;           /* 0x14 the stage it stops on */
    u8 state;         /* 0x15 */
    u8 unk16;         /* 0x16 */
    u8 unk17;         /* 0x17 */
    u8 unk18;         /* 0x18 */
    u8 hops;          /* 0x19 hops left on this stage */
    u8 stage;         /* 0x1a */
    u8 unk1b;         /* 0x1b */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgCoin_Block_size_must_be_0x1c[sizeof(dScMgCoin_Block) == 0x1c ? 1 : -1];
#endif

struct dScMgCoin_Popup {
    s32 x;            /* 0x00 */
    s32 y;            /* 0x04 */
    u16 timer;        /* 0x08 */
    u16 value;        /* 0x0a */
    u8 live;          /* 0x0c */
    u8 flag;          /* 0x0d */
    u8 pad0e[2];      /* 0x0e */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgCoin_Popup_size_must_be_0x10[sizeof(dScMgCoin_Popup) == 0x10 ? 1 : -1];
#endif

/* The count-in caption. delay, frameTime, state and sprite are the
   animation; x/y are where it is drawn. */
struct dScMgCoin_Caption {
    s32 x;            /* 0x00 */
    s32 y;            /* 0x04 */
    u16 delay;        /* 0x08 */
    u16 frameTime;    /* 0x0a */
    u8 active;        /* 0x0c */
    u8 state;         /* 0x0d */
    u8 visible;       /* 0x0e */
    u8 sprite;        /* 0x0f */
    u8 frame;         /* 0x10 */
    u8 pad11[3];      /* 0x11 */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgCoin_Caption_size_must_be_0x14[sizeof(dScMgCoin_Caption) == 0x14 ? 1 : -1];
#endif

/* The moneybag. One record; the step still takes an index. */
struct dScMgCoin_Bouncer {
    s32 x;            /* 0x00 */
    s32 y;            /* 0x04 */
    s32 vx;           /* 0x08 */
    s32 vy;           /* 0x0c */
    u16 spin;         /* 0x10 */
    u16 countdown;    /* 0x12 */
    u8 active;        /* 0x14 */
    u8 shown;         /* 0x15 */
    u8 sprite;        /* 0x16 -- into sCoinBagSprites (Bag_Draw) */
    u8 state;         /* 0x17 */
    u8 bouncesLeft;   /* 0x18 */
    u8 stage;         /* 0x19 */
    u8 pad1a[2];      /* 0x1a */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgCoin_Bouncer_size_must_be_0x1c[sizeof(dScMgCoin_Bouncer) == 0x1c ? 1 : -1];
#endif

/* dScMgCoin_c : dScMgBase_c leaf. Slot 18 is StartRound; the old
   mislabel sat on D0. See dScMgBase_c.h. */
struct dScMgCoin_c : dScMgBase_c {
    /* Defined inline in src/minigames/d_s_mg_coin.cpp, above the factory: its
       only body is seeding mSeed with 1 through func_0203b9b4. */
    dScMgCoin_c();
    virtual ~dScMgCoin_c();
    virtual s32 InitResources();             /* slot 0 */
    virtual s32 Behavior();                  /* slot 6 */
    virtual s32 Render();                    /* slot 9 */
    virtual void StartRound(int arg);     /* slot 18 */

    /* The scene's helpers and the per-state handlers reached through the
       .bss PMF tables (sCoinCoinStateTable / 021417c8 / 021417e8 /
       02141810 / 02141840), in ROM order. */
    void Bag_Draw();
    void Bag_StateBounceDown(int i);
    void Bag_StateParked(int index);
    void Bag_StateRise(int index);
    void Bag_Release();
    void Bag_StateIdle();
    void Bag_Update();
    void Bag_Show();
    void Bag_Hide();
    void Caption_Finish();
    void Caption_Trigger();
    void Caption_Draw();
    void Caption_StateLoop(int index);
    void Caption_StateHidden(int index);
    void Caption_StateBlink(int index);
    void Caption_StateCountIn(int index);
    void Caption_StateDelay(int index);
    void Caption_Update();
    void Caption_Show();
    void Caption_Reset();
    void DrawResultPrompt();
    void DrawResultNumbers();
    void Popup_UpdateAll();
    void Popup_Spawn(int index);
    void Sparkle_DrawAll();
    void Sparkle_UpdateAll();
    void Sparkle_Burst(int coinIndex);
    void Popup_DrawAll();
    void DrawScorePlayer();
    void DrawScoreRival();
    void Score_Update();
    void DrawCoinValues();
    void Coin_StateSettled();
    void Coin_StateHop(int index);
    void Coin_StateWaitTouch(int idx);
    void Coin_UpdateAll();
    void LayOutCoins();
    void KickBlockUnderCoin(int id);
    void DrawBlocks();
    void Block_StateFallIn(int i);
    void Block_StateResting();
    void Block_StateDropOnCoin(int index);
    void Block_StateWander(int i);
    void Block_StateHopDown(int i);
    void Block_StateWaiting(int i);
    void Block_ClampToWalls(int idx);
    void Block_UpdateAll();
    void Block_Animate(int index);
    void SpawnBlocks();
    void ResetRound();
    void Phase_StateResult();
    void Phase_StateScoreDelay();
    void Phase_StateSettle();
    void Phase_StatePlay();
    void Phase_StateIntro();
    void Phase_StateIdle();

    dScMgCoin_Block mBlocks[40];             /* 0x4660 */
    dScMgCoin_Coin mCoins[24];               /* 0x4ac0 */
    u8 pad_4d00[0x8];                        /* 0x4d00 */
    dScMgCoin_Score mScore;                  /* 0x4d08 */
    dScMgCoin_Sparkle mSparkles[0x20];       /* 0x4d14 */
    dScMgCoin_Popup mPopups[24];             /* 0x5014 */
    dScMgCoin_Caption mCaption;              /* 0x5194 */
    dScMgCoin_Bouncer mBouncer;              /* 0x51a8 */
    s32 mSeed;                               /* 0x51c4 set to 1 at construction, never read here */
    s32 mPhase;                              /* 0x51c8 5 is the result */
    s32 mCountdown;                          /* 0x51cc */
    u8 pad_51d0[0x4];                        /* 0x51d0 */
    s32 mPlayerScore;                        /* 0x51d4 this player's score */
    u8 pad_51d8[0x2];                        /* 0x51d8 */
    u8 mWins;                                /* 0x51da win count */
    u8 mWonLast;                             /* 0x51db last round won */
    u8 unk_51dc;                             /* 0x51dc */
    u8 pad_51dd;                             /* 0x51dd */
    u8 mCaptionLatch;                        /* 0x51de */
    u8 pad_51df;                             /* 0x51df */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgCoin_c_size_must_be_0x51e0[sizeof(dScMgCoin_c) == 0x51e0 ? 1 : -1];
#endif

#endif
