/* class dScMgCurling2_c : dScMgBase_c. Same shape as dScMgCurling_c --
 * confirmed leaf, D1 writes only its own vtable then calls dScMgBase_c's
 * D2 directly. See include/dScMgCurling_c.h for the shared reasoning
 * (inherited-field misattribution below 0x4660, no size assertion). */
#ifndef DSCMGCURLING2_C_H
#define DSCMGCURLING2_C_H
#include "dScMgBase_c.h"

/* One curling stone, 0x30 bytes, eleven of them at 0x4660 (dScMgCurling_c's
 * stone is the same record at 0x2c; this one carries four more bytes). */
struct dScMgCurling2_stone {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 speed;          /* 0x08 */
    u8  unk0c[0x1a];    /* 0x0c */
    u16 angle;          /* 0x26 */
    u8  state;          /* 0x28 */
    u8  active;         /* 0x29 */
    u8  unk2a;          /* 0x2a */
    u8  fast;           /* 0x2b, speed >= 0x3800 after a hit */
    u8  unk2c[0x4];     /* 0x2c */
};

typedef char dScMgCurling2_stone_size_must_be_0x30[sizeof(struct dScMgCurling2_stone) == 0x30 ? 1 : -1];

struct dScMgCurling2_c : dScMgBase_c {
    virtual ~dScMgCurling2_c();

    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    /* Slot 18 left unnamed -- same reasoning as dScMgCurling_c.h. */

    /* NON-VIRTUAL MEMBERS REACHED THROUGH THE OV006 POINTER-TO-MEMBER TABLES.
     * These seventeen are members, not free functions: ov006 .data carries an
     * 8-byte {code pointer, zero adjustment} record for sixteen of them, and
     * __sinit_ov006_02130758 copies those records into the BSS arrays that
     * dScMgCurling2_c's own code dispatches through.  A zero-adjustment
     * pointer-to-member record IS the proof of member-ness; the ROM does not
     * carry the original identifiers, so the NAMES BELOW ARE COINED.
     *
     * Each name is derived from the byte the handler drives and from its index
     * in its own table, both read out of the cartridge:
     *
     *   data_ov006_02141988 (+0x1d, the mode byte)
     *     [0] PickStepMode   [1] StepXOnly   [2] StepXAndY
     *   data_ov006_021419f8 (+0x1e, X increment, 8 a frame, +/-0x300)
     *     [0] StepXPick  [1] StepXPushPos  [2] StepXPushNeg  [3] StepXSettle
     *   data_ov006_021419b8 (+0x1e, X increment, 0x20 a frame, +/-0x400)
     *     [0] StepXPickFast [1] StepXPushPosFast
     *     [2] StepXPushNegFast [3] StepXSettleFast
     *   data_ov006_021419a0 (+0x1f, Y increment)
     *     [0] StepYRestart   [1] StepYRampUp   [2] StepYRampDown
     *   data_ov006_02141978 (the stylus drag)
     *     [0] DragBegin      [1] DragUpdate
     *
     * The index each handler writes back agrees with that layout in every case
     * -- the two pushers hand to 3, the settler hands to 0, the Y chain runs
     * 0 -> 1 -> 2 -> 0 -- and data_ov006_0212e4f4/_0212e4f8/_0212e4fc, the ROM
     * tables the two pickers and PickStepMode read from, hold exactly {1, 2}.
     * What the 0x48c0 records MEAN in the minigame is still unproven and no
     * name here claims it; see src/actors/dScMgCurling2_c.cpp's own banner.
     *
     * Members whose C name a still-shard caller spells stay func_ov006_*: the
     * twelve listed in that banner cannot be renamed without editing segment B
     * or include/decl_common.h. */
    void PickStepMode(int entry);
    void StepXOnly(int entry);
    void StepXAndY(int entry);

    void StepXPick(int entry);
    void StepXPushPos(int entry);
    void StepXPushNeg(int entry);
    void StepXSettle(int entry);

    void StepXPickFast(int entry);
    void StepXPushPosFast(int entry);
    void StepXPushNegFast(int entry);
    void StepXSettleFast(int entry);

    void StepYRestart(int entry);
    void StepYRampUp(int entry);
    void StepYRampDown(int entry);

    void DragBegin();
    void DragUpdate();

    void SpawnValue(int stone, int other);

    dScMgCurling2_stone mStone[11]; /* 0x4660, stride 0x30 */
    u8  pad_4870[0xd10];
    s32 unk_5580;            /* 0x5580 */
    s32 unk_5584;            /* 0x5584, drag x (fx32) */
    s32 unk_5588;            /* 0x5588, drag y */
    u8  pad_558c[0x4];
    s32 unk_5590;            /* 0x5590, drag y at the last direction change */
    s32 unk_5594;            /* 0x5594, drag x offset from the stylus */
    s32 unk_5598;            /* 0x5598, drag y offset */
    s32 unk_559c;            /* 0x559c, throw power */
    u8  pad_55a0[0x8];
    s32 unk_55a8;            /* 0x55a8, last drag dy */
    s32 unk_55ac;            /* 0x55ac */
    u8  pad_55b0[0x2];
    u16 unk_55b2;            /* 0x55b2, throw angle */
    u16 unk_55b4;            /* 0x55b4 */
    u16 unk_55b6;            /* 0x55b6 */
    u8  unk_55b8;            /* 0x55b8 */
    u8  unk_55b9;            /* 0x55b9 */
    u8  pad_55ba[0x1];
    u8  unk_55bb;            /* 0x55bb */
    u8  pad_55bc[0x2];
    u8  unk_55be;            /* 0x55be, drag phase */
    u8  pad_55bf[0x4];
    u8  unk_55c3;            /* 0x55c3 */
};

typedef char dScMgCurling2_c_size_must_be_0x55c4[sizeof(dScMgCurling2_c) == 0x55c4 ? 1 : -1];

#endif
