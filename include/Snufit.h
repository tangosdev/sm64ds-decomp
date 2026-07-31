/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Snufit: 5 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SNUFIT_H
#define SNUFIT_H
#include "types.h"

struct Snufit {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0xa];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x54];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x1];
    u8  unk_10a;            /* 0x10a */
    u8  pad_10b[0x5];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x5b];
    s32 unk_35c;            /* 0x35c */
    u8  pad_360[0x4];
    u8  mShadowModel;            /* 0x364 */
    u8  pad_365[0x67];
    s32 unk_3cc;            /* 0x3cc */
    s32 unk_3d0;            /* 0x3d0 */
    s32 unk_3d4;            /* 0x3d4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
