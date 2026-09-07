#ifndef DSCMGLUIGI_C_H
#define DSCMGLUIGI_C_H
#include "dScMgBase_c.h"

/* dScMgLuigi_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   5 (AfterCleanupResources), 6 (Behavior), 9 (Render), 16 (D1), 17 (D0),
   18 (OnYoshiTryEat). Fields below dScMgBase_c's own 0x4660 are INHERITED,
   not this class's own -- accessed via raw offsets on a char* cast of
   `this`, same as every other dScMgBase_c leaf. The 48 non-virtual members
   declared below are methods too; the fields they touch are named here
   wherever the matched body proves a type, and the ones still reached
   through a raw offset on a char* cast of `this` are not represented.
 *
 * SM64DS RTTI names the implementation dScMgLuigi_c. The reconstructed factory
 * dScMgLuigi_c_classInit (historical alias MgWanted_Spawn) installs this class's
 * cartridge vtable for the MG_LUIGI registry profile.
 */
struct dScMgLuigi_c : dScMgBase_c {
    virtual ~dScMgLuigi_c();
    virtual s32 InitResources();          /* slot 0 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot 5 */
    virtual s32 Behavior();               /* slot 6 */
    virtual s32 Render();                 /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    /* Non-virtual members, in ROM address order.  MEMBER-NESS is evidenced two
       ways and neither is a guess: the ones marked PMF below are named by a
       zero-adjustment pointer-to-member record in ov006 .data (the run at
       0x0213cd8c..0x0213ce4c), and the rest are only ever reached with this
       object in r0, from another member of the same run.  The NAMES are all
       coined -- the
       cartridge carries the address and nothing else, so each one describes the
       matched body and no more.  See symbols/actor_renames.tsv for the
       per-symbol evidence, one row each. */
    void BuildIrisTable(int i);       /* 0x020efdf0 */
    void IrisStop(int idx);           /* 0x020eff20  PMF */
    void IrisGrow(int idx);           /* 0x020effb8  PMF */
    void IrisHold(int idx);           /* 0x020f002c  PMF */
    void UpdateIris();                /* 0x020f0044 */
    void StartIris();                 /* 0x020f00a4 */
    void DrawReward();                /* 0x020f01d8 */
    void UpdateReward();              /* 0x020f0274 */
    void StartReward(int i);          /* 0x020f049c */
    void DrawCurtains();              /* 0x020f04ec */
    void CurtainStateSlow(int i);     /* 0x020f05d8  PMF */
    void CurtainStateFast(int i);     /* 0x020f06fc  PMF */
    void CurtainStateHold(int i);     /* 0x020f088c  PMF */
    void CurtainStateBounce(int i);   /* 0x020f0a6c  PMF */
    void CurtainUpdate(int i);        /* 0x020f0ba0  PMF */
    void CurtainStart(int i);         /* 0x020f0bf0  PMF */
    int  CurtainsDone();              /* 0x020f0d58 */
    void ResetCurtains();             /* 0x020f0dd8 */
    void DrawTimer();                 /* 0x020f0e28 */
    void TickTimer();                 /* 0x020f0eac */
    void DrawPictures();              /* 0x020f0f7c */
    void DrawPenalties();             /* 0x020f100c */
    void UpdatePenalties();           /* 0x020f10ec */
    void AddPenalty(int idx);         /* 0x020f120c */
    void DrawWantedIcon();            /* 0x020f12c8 */
    void TickPictureFlash(int idx);   /* 0x020f1318  PMF */
    void MovePictureBounce(int i);    /* 0x020f13cc  PMF */
    void MovePictureSway(int idx);    /* 0x020f15ac  PMF */
    void MovePictureDriftRandom(int idx);   /* 0x020f17fc  PMF */
    void MovePictureDriftByRow(int idx);    /* 0x020f192c  PMF */
    void MovePictureDriftByColumn(int idx); /* 0x020f1a70  PMF */
    void MovePictureDriftByLevel(int idx);  /* 0x020f1b98  PMF */
    void MovePictureDriftFixed(int idx);    /* 0x020f1cb4  PMF */
    void WrapPicture(int i);          /* 0x020f1dbc */
    void StopPicture(int idx);        /* 0x020f1e40  PMF */
    void RestartPicture(int idx);     /* 0x020f1e58  PMF */
    void UpdatePictures();            /* 0x020f1e90 */
    void BeginCatch(int found);       /* 0x020f1ef8 */
    void CheckTouch();                /* 0x020f1fcc */
    void PlaceBoardLayout();          /* 0x020f2224 */
    void PlaceNextPicture();          /* 0x020f2790 */
    void ChooseTarget();              /* 0x020f2cb8 */
    void ChooseBoard();               /* 0x020f2e20 */
    void ResetBoard();                /* 0x020f2ec0 */
    void StateCatch();                /* 0x020f300c  PMF, Behavior state 3 */
    void StatePlay();                 /* 0x020f319c  PMF, Behavior state 2 */
    void StatePlace();                /* 0x020f31dc  PMF, Behavior state 1 */
    void StateSetup();                /* 0x020f3260  PMF, Behavior state 0 */

    u8  pad_4660[0x198];
    /* The 120 moving pictures (0x78 slots). Four Fix12 arrays back to back --
       0x47f8 + 4 * 0x1e0 lands exactly on unk_4f78 -- then a u16 phase per slot
       and, from 0x5275, byte arrays of the same count 0x78 apart. All observed
       from the per-slot helpers (dScMgLuigi_c::MovePictureSway is the one typed here;
       dScMgLuigi_c::WrapPicture wraps mPosX/mPosY to the 0x110 x 0xd0 screen). */
    Fix12i mPosX[120];       /* 0x47f8 -- 20.12 screen x, wraps at 0x110 */
    Fix12i mPosY[120];       /* 0x49d8 -- 20.12 screen y, wraps at 0xd0 */
    Fix12i mVelX[120];       /* 0x4bb8 -- per-frame x step */
    Fix12i mVelY[120];       /* 0x4d98 -- per-frame y step */
    s32 unk_4f78;            /* 0x4f78 */
    u16 mMovePhase[120];     /* 0x4f7c -- 0 slowing, 1 speeding up rightward,
                                2 speeding up leftward */
    u8  pad_506c[0xf0];
    u16 unk_515c[4];         /* 0x515c -- four random 4.12 phases, seeded once
                                the whole board has been placed */
    u8  pad_5164[0xe];
    s16 unk_5172;            /* 0x5172 */
    u16 unk_5174;            /* 0x5174 -- board id; indexes data_ov006_0213ce84,
                                data_ov006_0213ce98 and data_ov006_0213cee0 */
    u8  pad_5176[0x2];
    u8  mGrid[13][9];        /* 0x5178 -- 13 x 9 occupancy grid for placement */
    u8  unk_51ed[8];         /* 0x51ed -- eight random 0/1 bytes, reseeded per board */
    u8  unk_51f5[6];         /* 0x51f5 -- six more of the same */
    u8  pad_51fb[0x2];
    u8  unk_51fd[120];       /* 0x51fd -- per-slot byte, cleared on placement */
    u8  mStarted[120];       /* 0x5275 -- 0 until the slot's first tick */
    u8  unk_52ed[120];       /* 0x52ed -- another per-slot byte (dScMgLuigi_c::UpdatePictures) */
    u8  mSpeedLevel[120];    /* 0x5365 -- indexes data_ov006_0212e888/898/8a8 */
    u8  pad_53dd[0x78];      /* 0x53dd -- one more per-slot byte array (dScMgLuigi_c::TickPictureFlash) */
    u8  unk_5455;            /* 0x5455 -- board fully placed */
    u8  unk_5456;            /* 0x5456 -- slots placed so far */
    u8  unk_5457;            /* 0x5457 */
    u8  pad_5458[0x1];
    u8  unk_5459;            /* 0x5459 */
    u8  unk_545a;            /* 0x545a -- speed level the board must not reuse */
};

typedef char dScMgLuigi_c_size_must_be_0x545c[sizeof(dScMgLuigi_c) == 0x545c ? 1 : -1];

#endif
