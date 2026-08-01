/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FortressWall: 6 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FORTRESSWALL_H
#define FORTRESSWALL_H
#include "types.h"

struct FortressWall {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x3];
    u8  unk_060;            /* 0x060 */
    u8  pad_061[0x3];
    u8  unk_064;            /* 0x064 */
    u8  pad_065[0x29];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x3c];
    u8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    u8  unk_31e;            /* 0x31e */
    u8  unk_31f;            /* 0x31f */
    u8  pad_320[0x1];
    u8  unk_321;            /* 0x321 */
    u8  unk_322;            /* 0x322 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
