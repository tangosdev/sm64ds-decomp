/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class TextureSequence: 7 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TEXTURESEQUENCE_H
#define TEXTURESEQUENCE_H
#include "types.h"

/* fwd */
struct ModelComponents;
struct model_;
struct TextureSequence {
    u8  pad_000[0x2];
    u16 unk_002;            /* 0x002 */
    s32 unk_004;            /* 0x004 */
    u32 unk_008;            /* 0x008 */
    s32 unk_00c;            /* 0x00c */
    s32 unk_010;            /* 0x010 */
    s32 unk_014;            /* 0x014 */
    s32 unk_018;            /* 0x018 */
    u16 unk_01c;            /* 0x01c */
    u8  pad_01e[0x2];
    s32 unk_020;            /* 0x020 */
#ifdef __cplusplus
    /* methods */
    void Update(ModelComponents & model_);
#endif
};

#endif
