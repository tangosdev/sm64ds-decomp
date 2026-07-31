/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class UpDownLiftBbh: 6 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef UPDOWNLIFTBBH_H
#define UPDOWNLIFTBBH_H
#include "types.h"

struct UpDownLiftBbh {
    u8  pad_000[0xc];
    u8  unk_00c;            /* 0x00c */
    u8  pad_00d[0x53];
    u8  unk_060;            /* 0x060 */
    u8  pad_061[0x2d];
    u8  unk_08e;            /* 0x08e */
    u8  pad_08f[0x3];
    u8  unk_092;            /* 0x092 */
    u8  pad_093[0x3];
    u8  unk_096;            /* 0x096 */
    u8  pad_097[0x3d];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1db];
    u16 unk_300;            /* 0x300 */
    u8  pad_302[0x1e];
    s32 unk_320;            /* 0x320 */
    u8  unk_324;            /* 0x324 */
    u8  pad_325[0x3];
    u8  mVariant;            /* 0x328 */
    u8  pad_329[0x3];
    s32 mState;            /* 0x32c */
    s32 unk_330;            /* 0x330 */
    u8  unk_334;            /* 0x334 */
    u8  pad_335[0x3];
    u8  unk_338;            /* 0x338 */
    u8  pad_339[0x3];
    u8  unk_33c;            /* 0x33c */
    u8  pad_33d[0x3];
    u8  unk_340;            /* 0x340 */
    u8  pad_341[0x5];
    u8  unk_346;            /* 0x346 */
    u8  unk_347;            /* 0x347 */
    u8  unk_348;            /* 0x348 */
    u8  unk_349;            /* 0x349 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
