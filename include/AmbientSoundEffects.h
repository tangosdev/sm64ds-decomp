/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class AmbientSoundEffects: 4 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef AMBIENTSOUNDEFFECTS_H
#define AMBIENTSOUNDEFFECTS_H
#include "types.h"

struct AmbientSoundEffects {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x31];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x8];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x30];
    s8  mAreaId;            /* 0x0cc */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
