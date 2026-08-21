/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class dBgPi: 5 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DBGPI_H
#define DBGPI_H
#include "types.h"

struct dBgPi {
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

/* Size is the dBgPi's own span AND the stride of the three consecutive results
   dBgCh_SphCrr holds at 0x74/0x9c/0xc4 -- notes/collision-system.md 3.2. */
typedef char dBgPi_size_must_be_0x28[sizeof(struct dBgPi) == 0x28 ? 1 : -1];
typedef struct dBgPi dBgPi;

#endif
