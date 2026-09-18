#ifndef DAOBJFL_UKIKI_C_H
#define DAOBJFL_UKIKI_C_H

#include "types.h"
#include "dBgActor_c.h"

/* Lethal Lava Land bobbing plank (registry profile FL_UKI_KI, actor 82): a
 * dBgActor_c whose Behavior drives mPosY off the shared sine table and drags
 * the mesh collider along with it.
 *
 * NAME: daObjFl_UkiKi_c is the cartridge's own RTTI spelling -- _ZTS at ov022
 * 0x021141b8 is the byte string "15daObjFl_UkiKi_c", and _ZTI at 0x021141ac
 * reads [__si_class_type_info+8, that string, _ZTI10dBgActor_c], so the ROM
 * also states the direct base.
 *
 * Vtable _ZTV15daObjFl_UkiKi_c at ov022 0x021141f0: only the slots declared
 * below differ from _ZTV10dBgActor_c, every other slot holds the base's own
 * word and is inherited. The destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and the
 * Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c,
 * and everything this header used to restate below 0x31e is inherited.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up, and agrees with the 0x328 the
 * factory at ov022 0x02112350 allocates. It guards this declaration; it is not
 * independent evidence about the ROM.
 */

#ifdef __cplusplus

struct daObjFl_UkiKi_c : dBgActor_c {
    /* dBgActor_c's fields end at 0x31e; mOriginalPosY needs 4-byte alignment. */
    u8  pad_31e[0x2];
    /* Where InitResources found the plank. Behavior rebuilds mPosY from this
       rest height plus the sine term, so the bob never accumulates drift. */
    s32 mOriginalPosY;  /* 0x320 */
    /* Seeded from mAngleX, += 0x400 per Behavior; (u16)>>4 indexes the shared
       sin/cos table at arm9 0x02082214. */
    s16 mPhaseAngle;    /* 0x324 */

    /* MEASURED -- DEFINED INLINE, EMPTY, AND DECLARED FIRST. This is the key
       function, so the TU that defines it emits _ZTV15daObjFl_UkiKi_c together
       with _ZTI15daObjFl_UkiKi_c and _ZTS15daObjFl_UkiKi_c as vague linkage.
       All three are the cartridge's own spellings and carry configured ROM
       homes in ov022, so they license as deadstrip-data and the whole
       six-function run isolates as one object.

       Inline and empty is what puts D1 ahead of D0, the cartridge's order:
       written out of line mwccarm emits the synthesized D0 first, which
       isolation refuses, and it also emits a D2 this class has no home for.
       The brace stays on the signature line -- tools/check_header_offsets.py
       recognises an inline body only when the signature line carries it. */
    virtual ~daObjFl_UkiKi_c() {}    /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();     /* slot  0 */
    virtual s32 CleanupResources();  /* slot  3 */
    virtual s32 Behavior();          /* slot  6 */
    virtual s32 Render();            /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_UkiKi_c_size_must_be_0x328[sizeof(daObjFl_UkiKi_c) == 0x328 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJFL_UKIKI_C_H */
