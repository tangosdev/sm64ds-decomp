/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WingFeather: 5 matched functions, 20 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WINGFEATHER_H
#define WINGFEATHER_H
#include "types.h"
#include "Model.h"
#include "ShadowModel.h"

struct WingFeather {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x6];
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11WingFeatherD0Ev.c] */
    Model mModel;            /* 0x0d4 */
    u8  mdCcAc_c;            /* 0x124 */
    u8  pad_125[0x23];
    u32 unk_148;            /* 0x148 */
    u8  pad_14c[0xc];
    u8  mWithMeshClsn;            /* 0x158 */
    u8  pad_159[0x1a7];
    u8  unk_300;            /* 0x300 */
    u8  pad_301[0x13];
    /* ShadowModel member. The cartridge's own ~WingFeather calls _ZN11ShadowModelD1Ev
       at +0x314 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel;            /* 0x314 */
    u8  pad_33c[0x3c];
    s32 unk_378;            /* 0x378 */
    u16 unk_37c;            /* 0x37c */
    u8  pad_37e[0x2];
    u32 unk_380;            /* 0x380 */
    u8  unk_384;            /* 0x384 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

typedef char WingFeather_size_must_be_0x388[sizeof(struct WingFeather) == 0x388 ? 1 : -1];

#endif
