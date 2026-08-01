/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BobOmb: 5 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOBOMB_H
#define BOBOMB_H
#include "types.h"

struct BobOmb {
    u8  pad_000[0x80];
    u8  unk_080;            /* 0x080 */
    u8  pad_081[0x17];
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    u8  pad_0a0[0x10];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x58];
    s32 unk_10c;            /* 0x10c */
    u8  mCylinderClsn;            /* 0x110 */
    u8  pad_111[0x17];
    u8  unk_128;            /* 0x128 */
    u8  pad_129[0x7];
    s32 unk_130;            /* 0x130 */
    u32 unk_134;            /* 0x134 */
    u8  pad_138[0xc];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x63];
    u8  mShadowModel;            /* 0x364 */
    u8  pad_365[0x77];
    s32 unk_3dc;            /* 0x3dc */
    u8  pad_3e0[0x13];
    u8  unk_3f3;            /* 0x3f3 */
    u8  pad_3f4[0x1];
    u8  unk_3f5;            /* 0x3f5 */
    u8  unk_3f6;            /* 0x3f6 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
