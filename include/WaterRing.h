/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WaterRing: 5 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WATERRING_H
#define WATERRING_H
#include "types.h"

struct WaterRing {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x74];
    s32 unk_080;            /* 0x080 */
    s32 unk_084;            /* 0x084 */
    s32 unk_088;            /* 0x088 */
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    s16 unk_092;            /* 0x092 */
    s16 unk_094;            /* 0x094 */
    s16 unk_096;            /* 0x096 */
    u8  pad_098[0x68];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    u8  mModel;            /* 0x30c */
    u8  pad_30d[0x4f];
    u8  mTextureTransformer;            /* 0x35c */
    u8  pad_35d[0xb];
    s32 unk_368;            /* 0x368 */
    u8  pad_36c[0x4];
    u8  unk_370;            /* 0x370 */
    u8  pad_371[0xb];
    s32 unk_37c;            /* 0x37c */
    s8  unk_380;            /* 0x380 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
