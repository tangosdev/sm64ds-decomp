/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class EnemySwitchTag: 5 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ENEMYSWITCHTAG_H
#define ENEMYSWITCHTAG_H
#include "types.h"
#include "dCcAc_c.h"

struct EnemySwitchTag {
    u8  pad_000[0xd4];
    /* dCcAc_c member. The cartridge's own ~EnemySwitchTag calls _ZN7dCcAc_cD1Ev at
       +0x0d4 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    u16 unk_108;            /* 0x108 */
    u16 unk_10a;            /* 0x10a */
    u8  unk_10c;            /* 0x10c */
    u8  mEventID;            /* 0x10d */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
#endif
};

typedef char EnemySwitchTag_size_must_be_0x110[sizeof(struct EnemySwitchTag) == 0x110 ? 1 : -1];

#endif
