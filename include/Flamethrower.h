/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Flamethrower: 3 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FLAMETHROWER_H
#define FLAMETHROWER_H
#include "types.h"

struct Flamethrower {
    u8  pad_000[0x5c];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x2f];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
#endif
};

#endif
