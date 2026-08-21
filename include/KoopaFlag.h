/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class KoopaFlag: 5 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef KOOPAFLAG_H
#define KOOPAFLAG_H
#include "types.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

struct KoopaFlag {
    u8  pad_000[0xd4];
    dCcAc_c mdCcAc_c;         /* 0x0d4 */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x108 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnimation (+0x50 = the
       Animation base), which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x108 */
    u16 mVictoryTimer;            /* 0x16c */
    u8  mHasTouchedFlag;            /* 0x16e */
    /* Trailing remainder, 5 bytes. Both markers were already typed and every
       field the three recovered functions touch ends at 0x16f, but
       KoopaFlag_Spawn allocates 0x174. The reference proposes an unused u8 at
       0x16f and an unused u32 at 0x170; nothing in this tree reads either, so
       they stay padding. */
    u8  pad_16f[0x5];
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

typedef char KoopaFlag_size_must_be_0x174[sizeof(struct KoopaFlag) == 0x174 ? 1 : -1];

#endif
