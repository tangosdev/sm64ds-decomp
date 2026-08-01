/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class LavaBubble: 4 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef LAVABUBBLE_H
#define LAVABUBBLE_H
#include "types.h"

struct LavaBubble {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x34];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x4c];
    s16 unk_100;            /* 0x100 */
    u8  pad_102[0xe];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x17];
    u8  unk_128;            /* 0x128 */
    u8  pad_129[0x7];
    s32 unk_130;            /* 0x130 */
    u32 unk_134;            /* 0x134 */
    u8  pad_138[0xc];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  unk_300;            /* 0x300 */
    u8  pad_301[0x3];
    s32 unk_304;            /* 0x304 */
    s32 unk_308;            /* 0x308 */
    s32 unk_30c;            /* 0x30c */
    u8  unk_310;            /* 0x310 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
