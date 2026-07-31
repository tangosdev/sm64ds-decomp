/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Spindrift: 5 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SPINDRIFT_H
#define SPINDRIFT_H
#include "types.h"

struct Spindrift {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    s32 unk_068;            /* 0x068 */
    s32 unk_06c;            /* 0x06c */
    s32 unk_070;            /* 0x070 */
    u8  pad_074[0x1a];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x1a];
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x50];
    u16 unk_104;            /* 0x104 */
    u8  pad_106[0x1];
    u8  unk_107;            /* 0x107 */
    u8  pad_108[0x8];
    u8  mModelAnim;            /* 0x110 */
    u8  pad_111[0x4f];
    u8  mAnimation;            /* 0x160 */
    u8  pad_161[0x13];
    u8  mShadowModel;            /* 0x174 */
    u8  pad_175[0x27];
    u8  mCylinderClsn;            /* 0x19c */
    u8  pad_19d[0x33];
    u8  mWithMeshClsn;            /* 0x1d0 */
    u8  pad_1d1[0x1c7];
    s16 unk_398;            /* 0x398 */
    u8  unk_39a;            /* 0x39a */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
