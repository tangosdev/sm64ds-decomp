/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class HauntedChair: 5 matched functions, 15 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef HAUNTEDCHAIR_H
#define HAUNTEDCHAIR_H
#include "types.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct HauntedChair {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12HauntedChairD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x124 */
    u8  unk_14c;            /* 0x14c */
    u8  pad_14d[0x2f];
    /* dCcAcPos_c member, named by the class's own destructor calling
       dCcAcPos_c's D1 at +0x17c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12HauntedChairD0Ev.c] */
    dCcAcPos_c mdCcAcPos_c;            /* 0x17c */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x1bc -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN12HauntedChairD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x1bc */
    s32 unk_378;            /* 0x378 */
    u8  pad_37c[0x4];
    s32 unk_380;            /* 0x380 */
    s32 unk_384;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
    s32 unk_38c;            /* 0x38c */
    s32 unk_390;            /* 0x390 */
    s32 unk_394;            /* 0x394 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
