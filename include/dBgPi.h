/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ClsnResult: 5 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CLSNRESULT_H
#define CLSNRESULT_H
#include "types.h"

struct ClsnResult {
    u8  pad_000[0x4];
    s64 unk_004;            /* 0x004 */
    s32 unk_00c;            /* 0x00c */
    s32 unk_010;            /* 0x010 */
    s32 unk_014;            /* 0x014 */
    u16 unk_018;            /* 0x018 */
    u16 unk_01a;            /* 0x01a */
    s32 unk_01c;            /* 0x01c */
    s32 unk_020;            /* 0x020 */
    s32 unk_024;            /* 0x024 */
#ifdef __cplusplus
    /* methods */
    u32 GetClsnID() const;
#endif
};

#endif
