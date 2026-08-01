/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Door: 5 matched functions, 15 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DOOR_H
#define DOOR_H
#include "types.h"

struct Door {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    u8  pad_092[0x12];
    s32 unk_0a4;            /* 0x0a4 */
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x24];
    u8  mCommonModel;            /* 0x0d4 */
    u8  pad_0d5[0xb];
    u8  unk_0e0;            /* 0x0e0 */
    u8  pad_0e1[0x23];
    s32 unk_104;            /* 0x104 */
    s32 unk_108;            /* 0x108 */
    s32 unk_10c;            /* 0x10c */
    u8  unk_110;            /* 0x110 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
