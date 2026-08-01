/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SlideDecorationSilverStar: 5 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SLIDEDECORATIONSILVERSTAR_H
#define SLIDEDECORATIONSILVERSTAR_H
#include "types.h"

struct SlideDecorationSilverStar {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0xc6];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    s8  mVariant;            /* 0x124 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
