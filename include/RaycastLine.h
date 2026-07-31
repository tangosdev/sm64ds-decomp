/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RaycastLine: 4 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef RAYCASTLINE_H
#define RAYCASTLINE_H
#include "types.h"

struct RaycastLine {
    u8  pad_000[0x10];
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x27];
    u8  unk_038;            /* 0x038 */
    u8  pad_039[0x1b];
    s32 unk_054;            /* 0x054 */
    s32 unk_058;            /* 0x058 */
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    u8  unk_064;            /* 0x064 */
};

#endif
