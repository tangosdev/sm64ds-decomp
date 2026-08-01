/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class KoopaTheQuick: 6 matched functions, 38 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef KOOPATHEQUICK_H
#define KOOPATHEQUICK_H
#include "types.h"

struct KoopaTheQuick {
    u8  pad_000[0x8];
    u8  mParam;            /* 0x008 */
    u8  pad_009[0x53];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x3];
    u8  unk_060;            /* 0x060 */
    u8  pad_061[0x3];
    u8  unk_064;            /* 0x064 */
    u8  pad_065[0x1b];
    u8  mScaleX;            /* 0x080 */
    u8  pad_081[0x3];
    u8  mScaleY;            /* 0x084 */
    u8  pad_085[0x3];
    u8  mScaleZ;            /* 0x088 */
    u8  pad_089[0x3];
    u8  unk_08c;            /* 0x08c */
    u8  pad_08d[0x1];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x6];
    u8  unk_09c;            /* 0x09c */
    u8  pad_09d[0x3];
    u8  unk_0a0;            /* 0x0a0 */
    u8  pad_0a1[0x6f];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  unk_350;            /* 0x350 */
    u8  pad_351[0x13];
    u8  mShadowModel;            /* 0x364 */
    u8  pad_365[0x27];
    s32 unk_38c;            /* 0x38c */
    u8  pad_390[0x4];
    u8  unk_394;            /* 0x394 */
    u8  pad_395[0x3];
    u8  unk_398;            /* 0x398 */
    u8  pad_399[0x3];
    u8  unk_39c;            /* 0x39c */
    u8  pad_39d[0x3];
    u8  unk_3a0;            /* 0x3a0 */
    u8  pad_3a1[0x3];
    u8  unk_3a4;            /* 0x3a4 */
    u8  pad_3a5[0x5];
    u8  unk_3aa;            /* 0x3aa */
    u8  pad_3ab[0x1];
    u8  unk_3ac;            /* 0x3ac */
    u8  unk_3ad;            /* 0x3ad */
    u8  unk_3ae;            /* 0x3ae */
    u8  pad_3af[0x1];
    u8  unk_3b0;            /* 0x3b0 */
    u8  unk_3b1;            /* 0x3b1 */
    u8  pad_3b2[0x2];
    u8  unk_3b4;            /* 0x3b4 */
    u8  unk_3b5;            /* 0x3b5 */
    u8  unk_3b6;            /* 0x3b6 */
    u8  pad_3b7[0x1];
    u8  unk_3b8;            /* 0x3b8 */
    u8  pad_3b9[0x3];
    u8  unk_3bc;            /* 0x3bc */
    u8  pad_3bd[0x3];
    u8  unk_3c0;            /* 0x3c0 */
    u8  pad_3c1[0x3];
    u8  unk_3c4;            /* 0x3c4 */
    u8  pad_3c5[0x3];
    u8  unk_3c8;            /* 0x3c8 */
    u8  pad_3c9[0xf];
    u8  mPathPtr;            /* 0x3d8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
