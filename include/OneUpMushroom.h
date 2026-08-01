/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class OneUpMushroom: 6 matched functions, 24 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ONEUPMUSHROOM_H
#define ONEUPMUSHROOM_H
#include "types.h"

struct OneUpMushroom {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x34];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x1c];
    s32 mEatingPlayer;            /* 0x0d0 */
    u8  pad_0d4[0x3c];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x17];
    u8  unk_128;            /* 0x128 */
    u8  pad_129[0x3];
    u8  unk_12c;            /* 0x12c */
    u8  pad_12d[0x17];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModel;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  mShadowModel;            /* 0x350 */
    u8  pad_351[0x27];
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
    s32 unk_380;            /* 0x380 */
    s32 mMushroomType;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
    u8  pad_38c[0x2];
    u8  unk_38e;            /* 0x38e */
    u8  unk_38f;            /* 0x38f */
    s32 unk_390;            /* 0x390 */
    s32 unk_394;            /* 0x394 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    unsigned int Render();
#endif
};

#endif
