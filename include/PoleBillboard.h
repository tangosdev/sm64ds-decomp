/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PoleBillboard: 6 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef POLEBILLBOARD_H
#define POLEBILLBOARD_H
#include "types.h"

struct PoleBillboard {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mShadowModel;            /* 0x320 */
    u8  pad_321[0x57];
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
    s32 unk_380;            /* 0x380 */
    s32 unk_384;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
    s32 unk_38c;            /* 0x38c */
    u16 unk_390;            /* 0x390 */
    s16 unk_392;            /* 0x392 */
    s16 unk_394;            /* 0x394 */
    s8  unk_396;            /* 0x396 */
    u8  unk_397;            /* 0x397 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Render();
#endif
};

#endif
