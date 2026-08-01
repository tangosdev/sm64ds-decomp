/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class OAM: 7 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef OAM_H
#define OAM_H
#include "types.h"

struct OAM {
    u8  pad_000[0xa8];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x45b4];
    u8  unk_4660;           /* 0x4660 */
    u8  pad_4661[0x23];
    u8  unk_4684;           /* 0x4684 */
    u8  pad_4685[0xb];
    u8  unk_4690;           /* 0x4690 */
    u8  pad_4691[0x23];
    s32 unk_46b4;           /* 0x46b4 */
};

#endif
