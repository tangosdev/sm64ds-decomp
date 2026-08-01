/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MrI_Projectile: 5 matched functions, 20 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MRI_PROJECTILE_H
#define MRI_PROJECTILE_H
#include "types.h"

struct MrI_Projectile {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x6];
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  unk_0a4;            /* 0x0a4 */
    u8  pad_0a5[0xb];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    u8  mShadowModel;            /* 0x0d4 */
    u8  pad_0d5[0x27];
    u8  mMovingCylinderClsnWithPos;            /* 0x0fc */
    u8  pad_0fd[0x3f];
    u8  mWithMeshClsn;            /* 0x13c */
    u8  pad_13d[0x1bb];
    u8  unk_2f8;            /* 0x2f8 */
    u8  pad_2f9[0x2f];
    s32 unk_328;            /* 0x328 */
    s32 unk_32c;            /* 0x32c */
    s16 unk_330;            /* 0x330 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
