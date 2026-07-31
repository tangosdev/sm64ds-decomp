/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class CccArena: 6 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CCCARENA_H
#define CCCARENA_H
#include "types.h"

struct CccArena {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    u8  pad_092[0x42];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x3f];
    s32 unk_114;            /* 0x114 */
    s32 unk_118;            /* 0x118 */
    s32 unk_11c;            /* 0x11c */
    u8  pad_120[0x4];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  unk_320;            /* 0x320 */
    u8  pad_321[0xf];
    u16 unk_330;            /* 0x330 */
    u8  pad_332[0x2];
    s32 unk_334;            /* 0x334 */
    s32 unk_338;            /* 0x338 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
