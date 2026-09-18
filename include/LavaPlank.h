#ifndef LAVAPLANK_H
#define LAVAPLANK_H

#include "types.h"
#include "dBgActor_c.h"

/* Lethal Lava Land bobbing plank (registry profile FL_UKI_KI, actor 82): a
 * dBgActor_c whose Behavior drives mPosY off the shared sine table and drags
 * the mesh collider along with it.
 *
 * NAME: the cartridge's own RTTI spells this class daObjFl_UkiKi_c -- _ZTS at
 * ov022 0x021141b8 is the byte string "15daObjFl_UkiKi_c", and _ZTI at
 * 0x021141ac reads [__si_class_type_info+8, that string, _ZTI10dBgActor_c], so
 * the ROM also states the direct base. `LavaPlank` is this project's coined
 * name, still carried by every symbol here; renaming it to the ROM's spelling
 * is deliberately kept separate from this file's promotion.
 *
 * Vtable _ZTV9LavaPlank at ov022 0x021141f0: only the slots declared below
 * differ from _ZTV10dBgActor_c, every other slot holds the base's own word and
 * is inherited.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up, and agrees with the 0x328 the
 * factory at ov022 0x02112350 allocates. It guards this declaration; it is not
 * independent evidence about the ROM.
 */

#ifdef __cplusplus

struct LavaPlank : dBgActor_c {
    /* dBgActor_c's fields end at 0x31e; mOriginalPosY needs 4-byte alignment. */
    u8  pad_31e[0x2];
    /* Where InitResources found the plank. Behavior rebuilds mPosY from this
       rest height plus the sine term, so the bob never accumulates drift. */
    s32 mOriginalPosY;  /* 0x320 */
    /* Seeded from mAngleX, += 0x400 per Behavior; (u16)>>4 indexes the shared
       sin/cos table at arm9 0x02082214. */
    s16 mPhaseAngle;    /* 0x324 */

    /* MEASURED -- DECLARED, NOT DEFINED, AND DECLARED FIRST. This is the key
       function, so whichever TU defines it emits _ZTV9LavaPlank together with
       _ZTI9LavaPlank and _ZTS9LavaPlank. The cartridge spells those two RTTI
       records daObjFl_UkiKi_c, so under this coined class name they have no
       configured ROM home and cannot be licensed as compiler-only data; a
       multi-function source that defines D1/D0 is therefore refused at
       isolation. D1 and D0 accordingly stay one-function sources for now --
       objisolate reduces a single-function object without a policy. Declared
       FIRST so a TU that does not define it emits no vague-linkage data at
       all. */
    virtual ~LavaPlank();            /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();     /* slot  0 */
    virtual s32 CleanupResources();  /* slot  3 */
    virtual s32 Behavior();          /* slot  6 */
    virtual s32 Render();            /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char LavaPlank_size_must_be_0x328[sizeof(LavaPlank) == 0x328 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* LAVAPLANK_H */
