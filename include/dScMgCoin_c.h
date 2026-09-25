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
    u8 collected;     /* 0x15 -- func_ov006_020de440 ends the round once every valued
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
    u8 sprite;        /* 0x16 -- into data_ov006_02134b4c (func_ov006_020dbe9c) */
    u8 state;         /* 0x17 */
    u8 bouncesLeft;   /* 0x18 */
    u8 stage;         /* 0x19 */
    u8 pad1a[2];      /* 0x1a */
};
#ifndef SM64DS_PLATFORM_PC
typedef char dScMgCoin_Bouncer_size_must_be_0x1c[sizeof(dScMgCoin_Bouncer) == 0x1c ? 1 : -1];
#endif

/* dScMgCoin_c : dScMgBase_c leaf. Slot 18 is OnYoshiTryEat; the old
   mislabel sat on D0. See dScMgBase_c.h. */
struct dScMgCoin_c : dScMgBase_c {
    virtual ~dScMgCoin_c();
    virtual s32 InitResources();             /* slot 0 */
    virtual s32 Behavior();                  /* slot 6 */
    virtual s32 Render();                    /* slot 9 */
    virtual void OnYoshiTryEat(int arg);     /* slot 18 */

    u8 pad_4660[0x460];                      /* 0x4660 */
    dScMgCoin_Coin mCoins[24];               /* 0x4ac0 */
    u8 pad_4d00[0x8];                        /* 0x4d00 */
    dScMgCoin_Score mScore;                  /* 0x4d08 */
    dScMgCoin_Sparkle mSparkles[0x20];       /* 0x4d14 */
    dScMgCoin_Popup mPopups[24];             /* 0x5014 */
    dScMgCoin_Caption mCaption;              /* 0x5194 */
    dScMgCoin_Bouncer mBouncer;              /* 0x51a8 */
    u8 pad_51c4[0x4];                        /* 0x51c4 */
    s32 unk_51c8;                            /* 0x51c8 phase; 5 is the result */
    s32 mCountdown;                          /* 0x51cc */
    u8 pad_51d0[0x4];                        /* 0x51d0 */
    s32 unk_51d4;                            /* 0x51d4 this player's score */
    u8 pad_51d8[0x2];                        /* 0x51d8 */
    u8 unk_51da;                             /* 0x51da */
    u8 unk_51db;                             /* 0x51db */
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
