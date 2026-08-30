#ifndef DAOBJSWDOOR_C_H
#define DAOBJSWDOOR_C_H

#include "types.h"
#include "dBgActor_c.h"

/* The abstract base of the switch-operated shutters: the metal doors that a
 * switch raises. `swdoor` is switch door, the ROM's own contraction.
 *
 * A LAYER THE TREE DID NOT HAVE. The ROM's RTTI names this class and points it at
 * dBgActor_c; the tree named the base's base for both leaves.
 *
 *   _ZTI13daObjSwdoor_c  ov002 0x021099c0
 *   _ZTS13daObjSwdoor_c  ov002 0x021099cc   "13daObjSwdoor_c"
 *   vtable               ov002 0x021099e4, 32 slots, same count as the base
 *   kind                 __si_class_type_info, ONE base, subobject offset 0
 *   base                 dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * ABSTRACT IN THREE SLOTS. 0 (InitResources), 3 (CleanupResources) and 6
 * (Behavior) are all null; this class supplies only 9 (Render), 16 (D1) and 17
 * (D0). Both leaves fill all three nulls, which is why ShutterBob and ShutterHmc
 * each have their own InitResources, CleanupResources and Behavior in the tree.
 *
 * TWO DESCENDANTS: daObjBSwdoor_c (ShutterBob) and daObjCvShutter_c (ShutterHmc).
 * Each one's destructor stores this class's vtable between its own and
 * _ZTV10dBgActor_c.
 *
 * NO FIELDS HERE, AND THAT IS A DELIBERATE FLOOR RATHER THAN A FINDING. Its own
 * Render (`func_ov002_020babf0`) dispatches through the Model at 0xd4 and its own
 * destructor destroys only dBgActor_c's two members; nothing it owns reaches above
 * sizeof(dBgActor_c). Both factories pass 804 = 0x324, four bytes more, and with the
 * Behavior slot null there is no method of this class that could be reading them.
 * The four bytes are declared on the leaves. Same reading as include/daObjMaruta_c.h,
 * and the same caveat: it is the weaker of the two possibilities and the only one
 * the evidence supports.
 */

#ifdef __cplusplus

struct daObjSwdoor_c : dBgActor_c {
    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN13daObjSwdoor_cD1Ev (which does exist out of line, at ov002 0x020bab64,
       still under its func_ov002_ name). An out-of-line declaration here would
       make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjSwdoor_c() {}

    /* Slot 9, ov002 0x020babf0 -- this class's only real slot, so declaring
       it out of line makes THIS the key function: the destructor above is
       inline and nothing else was migrated first. Its translation unit
       picks up _ZTV13daObjSwdoor_c (already delinked data, ov002
       0x021099e4) -- expected to drop this file from
       tools/eligible.py's name list, same mechanism the Guragura/Kuruma
       Render comments describe. */
    s32 Render();
    /* THE NULL SLOTS THE NOTE ABOVE ALREADY NAMES, SPELT SO THE COMPILER AGREES.
       mwccarm lays down a bare 0x00000000 with no relocation for a pure virtual --
       there is no __cxa_pure_virtual in this image for it to point at -- so a zero
       word in a ROM vtable IS the `= 0`, and it is the only thing that produces one.
       Left undeclared, this class silently inherits dBgActor_c's concrete bodies and
       the vtable it emits disagrees with the cartridge at exactly these slots.
       Measured by tools/romdata_check.py, which is the only gate that reads them:
       the ROM build's 106/106 compares .text alone and is blind here.

       DECLARED LAST, AND WITH `virtual` -- unlike the plain overrides above. The
       pure-specifier is only valid on a declaration carrying the keyword, and a
       pure virtual has no body to emit, so it can never become the key function:
       whichever virtual was first and non-inline before is still first now. */
    virtual s32 InitResources() = 0;        /* slot  0 */
    virtual s32 CleanupResources() = 0;     /* slot  3 */
    virtual s32 Behavior() = 0;             /* slot  6 */
};

typedef char daObjSwdoor_c_size_must_be_0x320[sizeof(daObjSwdoor_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJSWDOOR_C_H */
