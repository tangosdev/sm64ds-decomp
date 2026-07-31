/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BookShot: 6 matched functions, 35 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOOKSHOT_H
#define BOOKSHOT_H
#include "types.h"

struct BookShot {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    u8  unk_08e;            /* 0x08e */
    u8  pad_08f[0x9];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0xc];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x50];
    u16 unk_104;            /* 0x104 */
    u8  pad_106[0x1];
    u8  unk_107;            /* 0x107 */
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x7];
    u8  mModelAnim;            /* 0x110 */
    u8  pad_111[0x63];
    u8  mModel;            /* 0x174 */
    u8  pad_175[0x4f];
    u8  mShadowModel;            /* 0x1c4 */
    u8  pad_1c5[0x27];
    u8  unk_1ec;            /* 0x1ec */
    u8  pad_1ed[0x2f];
    u8  mMovingCylinderClsnWithPos;            /* 0x21c */
    u8  pad_21d[0x17];
    u8  unk_234;            /* 0x234 */
    u8  pad_235[0x3];
    u8  unk_238;            /* 0x238 */
    u8  pad_239[0x23];
    u8  mWithMeshClsn;            /* 0x25c */
    u8  pad_25d[0x1bb];
    s32 unk_418;            /* 0x418 */
    s32 unk_41c;            /* 0x41c */
    s32 unk_420;            /* 0x420 */
    s32 mState;            /* 0x424 */
    s32 unk_428;            /* 0x428 */
    s32 unk_42c;            /* 0x42c */
    s32 unk_430;            /* 0x430 */
    s32 unk_434;            /* 0x434 */
    s32 unk_438;            /* 0x438 */
    s32 unk_43c;            /* 0x43c */
    s32 unk_440;            /* 0x440 */
    u8  pad_444[0x8];
    s32 unk_44c;            /* 0x44c */
    s8  unk_450;            /* 0x450 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
#endif
};

#endif
