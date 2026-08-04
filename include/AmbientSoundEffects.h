/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class AmbientSoundEffects: 4 matched functions, 2 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef AMBIENTSOUNDEFFECTS_H
#define AMBIENTSOUNDEFFECTS_H
#include "types.h"

struct AmbientSoundEffects {
    u8  pad_000[0x8];
    u32 unk_008;            /* 0x008 */
    u8  pad_00c[0xc8];
    u32 unk_0d4;            /* 0x0d4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
