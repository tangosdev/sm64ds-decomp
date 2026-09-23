#ifndef DAOBJFL_BLOCK_C_H
#define DAOBJFL_BLOCK_C_H

#include "types.h"

/* Lethal Lava Land's block that floats on the lava (registry profile
 * FL_BLOCK): a dBgActor_c that sinks while the player stands on it and rises
 * back to its spawn height once it is left alone. Model at 0xd4, moving mesh
 * collider at 0x124, clsn matrix at 0x2ec, all inherited. Layout evidence:
 * notes/platform-provenance.md.
 *
 * NAME: daObjFl_Block_c is the cartridge's own RTTI spelling. _ZTS at ov022
 * 0x02113e74 is the byte string "15daObjFl_Block_c", and _ZTI at 0x02113e68
 * points at that string and at _ZTI10dBgActor_c, so the ROM states the direct
 * base too. The vtable symbol ov022/symbols.txt spells _ZTV15daObjFl_Block_c
 * (0x02113eac) carries a relocation at its address point minus 4 -- ov022
 * relocs.txt `from:0x02113ea8 to:0x02113e68` -- which is that same _ZTI, so
 * the three records are one triple and this is the class the ROM names.
 */

#include "dBgActor_c.h"

struct daObjFl_Block_c : dBgActor_c {
    /* dBgActor_c ends at 0x31e (its own sizeof rounds 0x31e up to 0x320);
       these two bytes are the base's tail padding, unused here. */
    u8  pad_31e[0x2];
    s32 mMaxPosY;           /* 0x320 -- spawn height; InitResources copies mPosY */
    u8  mHadClsn;           /* 0x324 -- set by the collision callback
                               func_ov022_0211191c when actor 0xbf (the
                               player) touches the block; Behavior clears it */

    /* MEASURED -- DEFINED INLINE, EMPTY, AND DECLARED FIRST. This is the key
       function, so the TU that defines it emits _ZTV15daObjFl_Block_c together
       with _ZTI15daObjFl_Block_c and _ZTS15daObjFl_Block_c as vague linkage,
       and drags the inherited bases' records along. All of them are the
       cartridge's own spellings and carry configured ROM homes, so they
       license as deadstrip-data and the six-function run isolates as one
       object.

       Inline and empty is what puts D1 ahead of D0, the cartridge's order
       (0x021116c4 then 0x02111708): written out of line mwccarm emits the
       synthesized D0 first, which isolation refuses, and it also emits a D2
       this class has no home for. The brace stays on the signature line --
       tools/check_header_offsets.py recognises an inline body only when the
       signature line carries it. */
    virtual ~daObjFl_Block_c() {}    /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();     /* slot  0 */
    virtual s32 CleanupResources();  /* slot  3 */
    virtual s32 Behavior();          /* slot  6 */
    virtual s32 Render();            /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_Block_c_size_must_be_0x328[sizeof(daObjFl_Block_c) == 0x328 ? 1 : -1];
#endif

#endif /* DAOBJFL_BLOCK_C_H */
