/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BlueCoinSwitch: 5 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BLUECOINSWITCH_H
#define BLUECOINSWITCH_H
#include "types.h"

struct BlueCoinSwitch {
    u8  pad_000[0xd4];
    u8  mMovingCylinderClsn;            /* 0x0d4 */
    u8  pad_0d5[0x17];
    u8  unk_0ec;            /* 0x0ec */
    u8  pad_0ed[0xb];
    s32 unk_0f8;            /* 0x0f8 */
    u8  pad_0fc[0xc];
    u16 unk_108;            /* 0x108 */
    u16 unk_10a;            /* 0x10a */
    u8  unk_10c;            /* 0x10c */
    u8  mEventID;            /* 0x10d */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
#endif
};

#endif
