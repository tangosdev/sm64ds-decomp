/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Boo: 5 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOO_H
#define BOO_H
#include "types.h"

struct Boo {
    u8  pad_000[0x80];
    u8  unk_080;            /* 0x080 */
    u8  pad_081[0x2f];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x58];
    s32 unk_10c;            /* 0x10c */
    u8  pad_110[0x74];
    u8  mMovingCylinderClsnWithPos;            /* 0x184 */
    u8  pad_185[0x17];
    u8  unk_19c;            /* 0x19c */
    u8  pad_19d[0x3];
    u8  unk_1a0;            /* 0x1a0 */
    u8  pad_1a1[0x23];
    u8  mWithMeshClsn;            /* 0x1c4 */
    u8  pad_1c5[0x1bb];
    u8  mModelAnim;            /* 0x380 */
    u8  pad_381[0x63];
    u8  mModel;            /* 0x3e4 */
    u8  pad_3e5[0x4f];
    u8  mShadowModel1;            /* 0x434 */
    u8  pad_435[0x27];
    u8  mShadowModel2;            /* 0x45c */
    u8  pad_45d[0xb3];
    u8  unk_510;            /* 0x510 */
    u8  pad_511[0x87];
    u8  unk_598;            /* 0x598 */
    u8  pad_599[0x2f];
    u8  unk_5c8;            /* 0x5c8 */
    u8  pad_5c9[0x3];
    u8  unk_5cc;            /* 0x5cc */
    u8  pad_5cd[0x2];
    u8  unk_5cf;            /* 0x5cf */
    u8  pad_5d0[0x4];
    u8  unk_5d4;            /* 0x5d4 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
