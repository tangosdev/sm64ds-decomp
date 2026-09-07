#ifndef DAOBJWANWANSHUTTER_C_H
#define DAOBJWANWANSHUTTER_C_H

#include "types.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dBgActor_c.h"

struct daObjWanwanShutter_c : dBgActor_c {
    u8 mDisabled;                     /* 0x31e -- both Behavior and Render return immediately while it is set */

    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE, do not move out of line.
     * Out of line, mwccarm 2004/b56 emits D0 before D1 (the reverse of the
     * cartridge's 0x02112e0c D1 / 0x02112e50 D0) and a homeless D2, and
     * objisolate rejects the whole translation unit. Defined in the class
     * body it emits D1 then D0 and no D2, which is the ROM's own order.
     *
     * Safe here because the class is a LEAF. Scanning extracted/ for the LE
     * word 0x02114868 (_ZTI20daObjWanwanShutter_c) returns FOUR raw hits, and
     * a reader repeating that scan must not read the extra pair as a
     * descendant: ov014 0x021148ac is this class's own vtable header, while
     * the ov015 hit at 0x021148d8 is ov015's own vtable header pointing at
     * ov015's own typeinfo. ov015 loads at the same base as ov014
     * (0x021111a0), so the two overlays are alternatives and can never be
     * resident together -- a class in one cannot derive from a class in the
     * other. Within ov014 the only pointer at this typeinfo is its own
     * vtable header, so nothing derives from it.
     *
     * The body is empty but the FUNCTION IS NOT: at 0x44 bytes it inlines
     * dBgActor_c's own inline destructor, which destroys the Model at 0xd4
     * and the dBgW_KcMbg at 0x124. Do not write those members as padding.
     */
    virtual ~daObjWanwanShutter_c() {}  /* slots 16 (D1), 17 (D0) */

    /* Diffed slot by slot against _ZTV10dBgActor_c (ov002 0x0210ae38): of the
     * table's 32 slots only the six declared here differ. Every other slot
     * holds the base's own word and is inherited -- slot 31 in particular is
     * dBgActor_c::Kill at 0x020ee55c in BOTH tables, so it is NOT overridden.
     * In ROM vtable order. Do not reorder, and do not drop the `virtual`:
     * an override that stops being declared virtual silently stops
     * overriding, and the vtable still links. */
    virtual s32 InitResources();       /* slot  0 -- 0x02112ffc */
    virtual s32 CleanupResources();    /* slot  3 -- 0x02112f3c */
    virtual s32 Behavior();            /* slot  6 -- 0x02112fc0 */
    virtual s32 Render();              /* slot  9 -- 0x02112f80 */
};

typedef char daObjWanwanShutter_c_size_must_be_0x320[sizeof(daObjWanwanShutter_c) == 0x320 ? 1 : -1];

#endif /* DAOBJWANWANSHUTTER_C_H */
