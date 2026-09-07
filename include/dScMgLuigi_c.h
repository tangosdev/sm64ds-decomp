#ifndef DSCMGLUIGI_C_H
#define DSCMGLUIGI_C_H
#include "dScMgBase_c.h"

/* dScMgLuigi_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   5 (AfterCleanupResources), 6 (Behavior), 9 (Render), 16 (D1), 17 (D0),
   18 (own new slot, not yet named -- stays a raw extern "C" helper, not a
   declared method). Fields below dScMgBase_c's own 0x4660 are INHERITED,
   not this class's own -- accessed via raw offsets on a char* cast of
   `this`, same as every other dScMgBase_c leaf. Own fields observed only
   from slots 0/5/6/9/16/17/18 directly; fields touched only by non-virtual
   helper functions (still raw extern "C" calls, not migrated methods) are
   not represented here.
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

    u8  pad_4660[0x198];
    /* The 120 moving pictures (0x78 slots). Four Fix12 arrays back to back --
       0x47f8 + 4 * 0x1e0 lands exactly on unk_4f78 -- then a u16 phase per slot
       and, from 0x5275, byte arrays of the same count 0x78 apart. All observed
       from the per-slot helpers (func_ov006_020f15ac is the one typed here;
       func_ov006_020f1dbc wraps mPosX/mPosY to the 0x110 x 0xd0 screen). */
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
    u8  unk_52ed[120];       /* 0x52ed -- another per-slot byte (func_ov006_020f1e90) */
    u8  mSpeedLevel[120];    /* 0x5365 -- indexes data_ov006_0212e888/898/8a8 */
    u8  pad_53dd[0x78];      /* 0x53dd -- one more per-slot byte array (func_ov006_020f1318) */
    u8  unk_5455;            /* 0x5455 -- board fully placed */
    u8  unk_5456;            /* 0x5456 -- slots placed so far */
    u8  unk_5457;            /* 0x5457 */
    u8  pad_5458[0x1];
    u8  unk_5459;            /* 0x5459 */
    u8  unk_545a;            /* 0x545a -- speed level the board must not reuse */
};

typedef char dScMgLuigi_c_size_must_be_0x545c[sizeof(dScMgLuigi_c) == 0x545c ? 1 : -1];

#endif
