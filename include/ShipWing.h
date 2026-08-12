/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RotatingPlatformRr: 5 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ROTATINGPLATFORMRR_H
#define ROTATINGPLATFORMRR_H
#include "types.h"
#include "CommonModel.h"

struct RotatingPlatformRr {
    u8  pad_000[0x8];
    u32 unk_008;            /* 0x008 */
    u8  pad_00c[0x80];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    u8  pad_092[0x42];
    /* CommonModel member, named by the class's own destructor calling
       CommonModel's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN18RotatingPlatformRrD0Ev.c] */
    CommonModel mCommonModel;            /* 0x0d4 */
    s16 unk_110;            /* 0x110 */
    s16 unk_112;            /* 0x112 */
    s16 unk_114;            /* 0x114 */
    u16 unk_116;            /* 0x116 */
    u8  unk_118;            /* 0x118 */
    u8  unk_119;            /* 0x119 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
