/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Swoop: 5 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SWOOP_H
#define SWOOP_H
#include "types.h"

struct Swoop {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x2];
    s16 unk_092;            /* 0x092 */
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0xa];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x54];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x7];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim1;            /* 0x300 */
    u8  pad_301[0x5b];
    s32 unk_35c;            /* 0x35c */
    u8  pad_360[0x4];
    u8  mModelAnim2;            /* 0x364 */
    u8  pad_365[0x5b];
    s32 unk_3c0;            /* 0x3c0 */
    u8  pad_3c4[0x4];
    u8  mShadowModel;            /* 0x3c8 */
    u8  pad_3c9[0x5b];
    s32 unk_424;            /* 0x424 */
    s32 unk_428;            /* 0x428 */
    s32 unk_42c;            /* 0x42c */
    u8  pad_430[0xc];
    u8  unk_43c;            /* 0x43c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
