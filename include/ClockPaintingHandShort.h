/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ClockPaintingHandShort: 6 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CLOCKPAINTINGHANDSHORT_H
#define CLOCKPAINTINGHANDSHORT_H
#include "types.h"
#include "Model.h"

struct ClockPaintingHandShort {
    u8  pad_000[0xc];
    /* 0x00c..0x090 is ActorBase's, and ActorBase.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u16 unk_00c;                 /* 0x00c */
    u8  aliveState;              /* 0x00e */
    u8  shouldBeKilled;          /* 0x00f */
    u8  unk_010;                 /* 0x010 */
    u8  unk_011;                 /* 0x011 */
    u8  unk_012;                 /* 0x012 */
    u8  unk_013;                 /* 0x013 */
    u8  sceneNode[0x14];               /* 0x014 */
    u8  behavNode[0x10];               /* 0x028 */
    u8  renderNode[0x10];              /* 0x038 */
    u8  pad_048[0x48];
    s16 unk_090;            /* 0x090 */
    u8  pad_092[0x3a];
    s8  unk_0cc;            /* 0x0cc */
    u8  pad_0cd[0x7];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
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
