/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MadPiano: 6 matched functions, 20 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MADPIANO_H
#define MADPIANO_H
#include "types.h"

struct MadPiano {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    s32 unk_068;            /* 0x068 */
    s32 unk_06c;            /* 0x06c */
    s32 unk_070;            /* 0x070 */
    u8  pad_074[0x1a];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0xc];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mModelAnim;            /* 0x320 */
    u8  pad_321[0x63];
    u8  mShadowModel1;            /* 0x384 */
    u8  pad_385[0x27];
    u8  mShadowModel2;            /* 0x3ac */
    u8  pad_3ad[0x27];
    u8  mShadowModel3;            /* 0x3d4 */
    u8  pad_3d5[0x137];
    u8  mWithMeshClsn;            /* 0x50c */
    u8  pad_50d[0x1bf];
    s32 unk_6cc;            /* 0x6cc */
    u8  pad_6d0[0x4];
    s32 unk_6d4;            /* 0x6d4 */
    s32 unk_6d8;            /* 0x6d8 */
    s32 unk_6dc;            /* 0x6dc */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
