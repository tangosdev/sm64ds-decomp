/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Rabbit: 5 matched functions, 27 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef RABBIT_H
#define RABBIT_H
#include "types.h"

struct Rabbit {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x74];
    s32 mScale;            /* 0x080 */
    s32 unk_084;            /* 0x084 */
    s32 unk_088;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  unk_0cc;            /* 0x0cc */
    u8  pad_0cd[0x3];
    s32 unk_0d0;            /* 0x0d0 */
    u8  pad_0d4[0x3c];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x7];
    u8  unk_308;            /* 0x308 */
    u8  pad_309[0x3];
    u8  unk_30c;            /* 0x30c */
    u8  pad_30d[0x4f];
    s32 unk_35c;            /* 0x35c */
    u8  pad_360[0x8];
    u8  mShadowModel1;            /* 0x368 */
    u8  pad_369[0x57];
    u8  mShadowModel2;            /* 0x3c0 */
    u8  pad_3c1[0x5f];
    s32 mColorVariant;            /* 0x420 */
    u8  pad_424[0x4];
    u8  unk_428;            /* 0x428 */
    u8  unk_429;            /* 0x429 */
    u8  pad_42a[0xe];
    s32 unk_438;            /* 0x438 */
    s32 mRabbitId;            /* 0x43c */
    s32 mCharacterId;            /* 0x440 */
    s32 unk_444;            /* 0x444 */
    s32 unk_448;            /* 0x448 */
    u8  pad_44c[0x10];
    s32 unk_45c;            /* 0x45c */
    u8  pad_460[0x8];
    s32 unk_468;            /* 0x468 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
