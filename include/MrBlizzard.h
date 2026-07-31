/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MrBlizzard: 5 matched functions, 29 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MRBLIZZARD_H
#define MRBLIZZARD_H
#include "types.h"

struct MrBlizzard {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x3b];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x1];
    u8  unk_10a;            /* 0x10a */
    u8  pad_10b[0x5];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    u8  mModelAnim;            /* 0x30c */
    u8  pad_30d[0x63];
    u8  mShadowModel;            /* 0x370 */
    u8  pad_371[0x8b];
    s32 unk_3fc;            /* 0x3fc */
    s32 unk_400;            /* 0x400 */
    u8  pad_404[0x10];
    s16 unk_414;            /* 0x414 */
    u8  pad_416[0x2];
    s32 mPathId;            /* 0x418 */
    s32 mType;            /* 0x41c */
    s32 unk_420;            /* 0x420 */
    s32 unk_424;            /* 0x424 */
    u8  pad_428[0x24];
    s32 unk_44c;            /* 0x44c */
    s32 unk_450;            /* 0x450 */
    s32 unk_454;            /* 0x454 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
