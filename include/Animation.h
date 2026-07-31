/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Animation: 14 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ANIMATION_H
#define ANIMATION_H
#include "types.h"

/* fwd */
struct flags;
struct Animation {
    u8  unk_000;            /* 0x000 */
    u8  pad_001[0x3];
    u8  mFrameCountAndFlags;            /* 0x004 */
    u8  pad_005[0x3];
    s32 unk_008;            /* 0x008 */
    s32 unk_00c;            /* 0x00c */
    s32 unk_010;            /* 0x010 */
    s32 unk_014;            /* 0x014 */
#ifdef __cplusplus
    /* methods */
    int Finished();
    unsigned int GetFrameCount() const;
    void SetFlags(int flags);
#endif
};

#endif
