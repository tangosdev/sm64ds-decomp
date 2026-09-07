#ifndef DSCMGPACHINKO_C_H
#define DSCMGPACHINKO_C_H
#include "dScMgBase_c.h"

/* dScMgPachinko_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
   (no RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper). rtti_vtables.py --own also
   reports slots 36/38, but both are spurious: the true vtable ends at 36
   words (address-gap math against the neighbouring g_profile_MG_TAMAIRE
   table, and relocs.txt cross-references confirm it belongs to a wholly
   different actor's spawn descriptor, not this class) -- excluded, along
   with func_ov006_021019e0 and dScMgPachinko2_c_classInit, from this migration.
   Fields below dScMgBase_c's own 0x4660 are INHERITED, not this class's
   own -- accessed via raw offsets on a char* cast of `this` (0xa4, 0xc0,
   0xc3, 0xc4 all touched here, all already dScMgBase_c's own, same as
   dScMgPachinko2_c's).
 *
 * SM64DS RTTI names the implementation dScMgPachinko_c. The reconstructed factory
 * dScMgPachinko_c_classInit (historical alias MgBobOmbSquad_Spawn) installs this class's
 * cartridge vtable for the MG_PACHINKO registry profile.
 */
/* One pachinko ball on the board, 0x38 bytes, thirty of them at 0x4660. */
struct dScMgPachinko_ball {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 unk08;          /* 0x08 */
    s32 unk0c;          /* 0x0c */
    s32 unk10;          /* 0x10 */
    s32 unk14;          /* 0x14 */
    s32 unk18;          /* 0x18 */
    s32 unk1c;          /* 0x1c */
    s32 unk20;          /* 0x20 */
    u16 unk24;          /* 0x24 */
    u16 angle;          /* 0x26 */
    u16 unk28;          /* 0x28 */
    u8  unk2a[0x2];     /* 0x2a */
    u8  active;         /* 0x2c */
    u8  unk2d;          /* 0x2d */
    u8  unk2e;          /* 0x2e */
    u8  unk2f;          /* 0x2f */
    u8  unk30;          /* 0x30 */
    u8  unk31[0x2];     /* 0x31 */
    u8  unk33;          /* 0x33 */
    u8  unk34[0x2];     /* 0x34 */
    u8  unk36;          /* 0x36 */
    u8  unk37;          /* 0x37 */
};

typedef char dScMgPachinko_ball_size_must_be_0x38[sizeof(struct dScMgPachinko_ball) == 0x38 ? 1 : -1];

/* One ball being aimed with the stylus, 0x38 bytes, at 0x4ed8. */
struct dScMgPachinko_shot {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 velX;           /* 0x08 */
    s32 velY;           /* 0x0c */
    s32 offX;           /* 0x10, grab offset from the stylus */
    s32 offY;           /* 0x14 */
    u8  unk18[0x8];     /* 0x18 */
    s32 speed;          /* 0x20 */
    s32 unk24;          /* 0x24 */
    s32 sound;          /* 0x28, sound handle */
    s32 dist;           /* 0x2c, last frame's distance from the launcher */
    u16 angle;          /* 0x30 */
    u16 timer;          /* 0x32 */
    u8  unk34;          /* 0x34 */
    u8  state;          /* 0x35 */
    u8  unk36;          /* 0x36 */
    u8  unk37;          /* 0x37 */
};

typedef char dScMgPachinko_shot_size_must_be_0x38[sizeof(struct dScMgPachinko_shot) == 0x38 ? 1 : -1];

struct dScMgPachinko_c : dScMgBase_c {
    virtual ~dScMgPachinko_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    dScMgPachinko_ball mBall[30];    /* 0x4660, stride 0x38, ends 0x4cf0 */
    u8  pad_4cf0[0x1e8];
    dScMgPachinko_shot mShot[24];    /* 0x4ed8, stride 0x38; 24 puts the
                                        siblings' own 0x5418 field exactly
                                        one record past the end */
    u8  pad_5418[0x7f8];
    s32 unk_5c10;            /* 0x5c10 */
    u8  pad_5c14[0x4];
    u16 unk_5c18;            /* 0x5c18 */
    u8  pad_5c1a[0x2];
    u16 unk_5c1c;            /* 0x5c1c */
    u8  pad_5c1e[0x6];
    u16 unk_5c24;            /* 0x5c24 */
    u16 unk_5c26;            /* 0x5c26 */
    u16 unk_5c28;            /* 0x5c28 */
    u16 unk_5c2a;            /* 0x5c2a */
    /* trailing extent the ROM's `new dScMgPachinko_c` literal proves; see tools/opnew_sizes.py */
    u8  pad_5c2c[0x5];
    u8  unk_5c31;            /* 0x5c31 */
    u8  pad_5c32[0x6];
};

typedef char dScMgPachinko_c_size_must_be_0x5c38[sizeof(struct dScMgPachinko_c) == 0x5c38 ? 1 : -1];

#endif
