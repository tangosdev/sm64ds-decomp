/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BooCage: 4 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOOCAGE_H
#define BOOCAGE_H
#include "types.h"

struct BooCage {
    u8  pad_000[0x9c];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x6c];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModel;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  mShadowModel;            /* 0x350 */
    u8  pad_351[0x27];
    s32 unk_378;            /* 0x378 */
    s16 unk_37c;            /* 0x37c */
    u8  unk_37e;            /* 0x37e */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Render();
#endif
};

#endif
