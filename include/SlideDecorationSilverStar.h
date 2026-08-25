/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SlideDecorationSilverStar: 5 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SLIDEDECORATIONSILVERSTAR_H
#define SLIDEDECORATIONSILVERSTAR_H
#include "types.h"
#include "Model.h"

struct SlideDecorationSilverStar {
    u8  pad_000[0xc];
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0xc6];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    s8  mVariant;            /* 0x124 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char SlideDecorationSilverStar_size_must_be_0x128[
    sizeof(struct SlideDecorationSilverStar) == 0x128 ? 1 : -1];

#endif
