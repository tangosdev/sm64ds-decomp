/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ClockPaintingHandShort: 6 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CLOCKPAINTINGHANDSHORT_H
#define CLOCKPAINTINGHANDSHORT_H
#include "types.h"

struct ClockPaintingHandShort {
    u8  pad_000[0xc];
    u8  unk_00c;            /* 0x00c */
    u8  pad_00d[0x83];
    s16 unk_090;            /* 0x090 */
    u8  pad_092[0x3a];
    s8  unk_0cc;            /* 0x0cc */
    u8  pad_0cd[0x7];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mHandIndex;            /* 0x124 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
