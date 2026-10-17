/* class dScMgCurling_c : dScMgBase_c. Confirmed leaf -- no RTTI record
 * names it as a base (tools/rtti_extract.py). Its D1 (in
 * src/minigames/d_s_mg_curling.cpp) writes only its own vtable then calls
 * dScMgBase_c's D2 directly; no members here need explicit destruction.
 *
 * Fields below 0x4660 (dScMgBase_c's own asserted size) are INHERITED, not
 * dScMgCurling_c's own -- the old flat header's `mRunDuringFade` at 0x0a4 is
 * really dScMgBase_c's own field at that offset, misattributed by
 * tools/deepen_rtti.py's flat-struct generation (it doesn't know about
 * inheritance). SIZE NOT ASSERTED: a leaf, so nothing needs the number
 * (same reasoning include/dScStage_c.h documents). */
#ifndef DSCMGCURLING_C_H
#define DSCMGCURLING_C_H
#include "dScMgBase_c.h"

/* One curling stone, 0x2c bytes, five of them at 0x4660. */
struct dScMgCurling_stone {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 speed;          /* 0x08 */
    s32 lastX;          /* 0x0c */
    s32 lastY;          /* 0x10 */
    s32 grabX;          /* 0x14 */
    s32 grabY;          /* 0x18 */
    s32 slideSnd;       /* 0x1c, slide sound handle */
    u16 timer;          /* 0x20 */
    s16 spin;           /* 0x22 */
    u16 heading;        /* 0x24, spin accumulates here */
    u16 angle;          /* 0x26 */
    u8  state;          /* 0x28, 0 and 3 are skipped by the hit test */
    u8  active;         /* 0x29 */
    u8  dealt;          /* 0x2a, icon shown on the HUD */
    u8  fast;           /* 0x2b, speed >= 0x3800 after a hit */
};

/* One score popup, shown where a stone lands; five of them at 0x473c. */
struct dScMgCurling_popup {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    u16 points;         /* 0x08 */
    u16 countdown;      /* 0x0a */
    u8  live;           /* 0x0c, slot holds a score */
    u8  shown;          /* 0x0d, countdown done -- draw it */
    u8  pad[2];
};

/* One falling decoration ("bit"), 0x24 bytes; 0x32 of them at 0x478c.
 * state and state2 drive the two per-kind handler tables. */
struct dScMgCurling_bit {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 vx;             /* 0x08 */
    s32 vy;             /* 0x0c */
    s32 vyCap;          /* 0x10 */
    u16 wait;           /* 0x14 */
    u16 timer;          /* 0x16 */
    u16 wait2;          /* 0x18 */
    u16 pad1a;
    u8  on;             /* 0x1c */
    u8  kind;           /* 0x1d */
    u8  state;          /* 0x1e */
    u8  state2;         /* 0x1f */
    u8  shown;          /* 0x20 */
    u8  texA;           /* 0x21 */
    u8  texB;           /* 0x22 */
    u8  pad23;
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgCurling_stone_size_must_be_0x2c[sizeof(struct dScMgCurling_stone) == 0x2c ? 1 : -1];
typedef char dScMgCurling_popup_size_must_be_0x10[sizeof(struct dScMgCurling_popup) == 0x10 ? 1 : -1];
typedef char dScMgCurling_bit_size_must_be_0x24[sizeof(struct dScMgCurling_bit) == 0x24 ? 1 : -1];
#endif

struct dScMgCurling_c : dScMgBase_c {
    /* Declared, not defined inline -- a leaf, so nothing needs to inline
       it; real body in src/minigames/d_s_mg_curling.cpp, which is the whole
       class. */
    virtual ~dScMgCurling_c();

    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void StartRound(int arg);               /* slot 18 */

    void Bit_DrawAll();   /* renders the falling bits */
    void Bit_StateFallSlowDown(int index);
    void Bit_StateFallSpeedUp(int index);
    void Bit_StateFallStart(int index);
    void BitA_StateDriftStop(int index);
    void BitA_StateDriftLeft(int index);
    void BitA_StateDriftRight(int index);
    void BitA_StatePause(int index);
    void Bit_RunKindA(int index);
    void BitB_StateDriftStop(int index);
    void BitB_StateDriftLeft(int index);
    void BitB_StateDriftRight(int index);
    void BitB_StatePause(int idx);
    void Bit_RunKindB(int idx);
    void Bit_PickKind(int idx);
    void Bit_UpdateAll();
    void Bit_InitAll();
    void Popup_DrawAll();   /* score popups */
    void Popup_UpdateAll();
    void ScoreStones();
    void DrawAimMarker();
    void Aim_Swing();   /* stylus handler for the aimed stone */
    void Aim_WaitTouch();
    void DrawStonesLeft();
    void SeparateStones(int idx);   /* stone separation */
    void ResolveStoneCollision(int idx);   /* stone collision */
    void SetStoneSpin(int i);
    void Stone_Drag(int i);     /* stone i while dragged */
    void Stone_Collide(int idx);
    void Stone_Slide(int idx);   /* stone idx while it slides */
    void Stone_WaitRelease(int idx);   /* releases stone idx */
    void BringOutNextStone();   /* brings out the next stone */
    void Scene_Idle();   /* idle state */
    void Scene_End();   /* end state */
    void Scene_Score();   /* scoring state */
    void Scene_Play();   /* playing state */
    void Scene_StartRound();   /* starts a round */
    void PickHouse();   /* picks the house */
    void ResetHouse();   /* marks the house unset */
    void ClearStones();   /* clears stones, popups and aim */

    /* Slot 18 (one of dScMgBase_c's own undeclared new slots 18-35) is
       left unnamed here too, same reasoning as dScMgBase_c.h's own -- its
       target (_ZN14dScMgCurling_c10StartRoundEi, in
       src/minigames/d_s_mg_curling.cpp) had a "recovered name" of
       OnYoshiTryEat_020e3470, which is wrong (same tree-wide mislabel
       documented in notes/dscene-c-siblings-census.md section 3): its
       body sets fields and calls helpers, nothing like a destructor. */

    dScMgCurling_stone mStone[5];   /* 0x4660, stride 0x2c */
    dScMgCurling_popup mPopup[5];     /* 0x473c */
    dScMgCurling_bit mBit[0x32];      /* 0x478c, stride 0x24 */
    s32 mHouseX;             /* 0x4e94 */
    s32 mHouseY;             /* 0x4e98 */
    s32 mHouse;              /* 0x4e9c, 0xff = not picked yet */
    u8  pad_4ea0[0xc];
    s32 mState;              /* 0x4eac */
    s32 mAimX;               /* 0x4eb0 */
    s32 mAimY;               /* 0x4eb4 */
    s32 mAimHomeX;           /* 0x4eb8 */
    s32 mAimHomeY;           /* 0x4ebc */
    s32 mGrabX;              /* 0x4ec0 */
    s32 mGrabY;              /* 0x4ec4 */
    s32 mSwingSpeed;         /* 0x4ec8 */
    s32 unk_4ecc;            /* 0x4ecc */
    s32 unk_4ed0;            /* 0x4ed0 */
    s32 mSwingDelta;         /* 0x4ed4 */
    s32 unk_4ed8;            /* 0x4ed8 */
    u16 unk_4edc;            /* 0x4edc */
    u16 mSwingAng;           /* 0x4ede */
    u16 mStoneDelay;         /* 0x4ee0, before the next stone comes out */
    u16 mStateDelay;         /* 0x4ee2 */
    u8  mStylusHeld;         /* 0x4ee4 */
    u8  mAimShown;           /* 0x4ee5 */
    u8  mThrown;             /* 0x4ee6 */
    u8  mNextStone;          /* 0x4ee7 */
    u8  mRoundOver;          /* 0x4ee8 */
    u8  mSwingTimer;         /* 0x4ee9 */
    u8  mSwingState;         /* 0x4eea */
    u8  pad_4eeb;
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgCurling_c_size_must_be_0x4eec[sizeof(struct dScMgCurling_c) == 0x4eec ? 1 : -1];
#endif

#endif
