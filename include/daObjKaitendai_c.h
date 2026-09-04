#ifndef DAOBJKAITENDAI_C_H
#define DAOBJKAITENDAI_C_H

#include "types.h"
#include "dBgActor_c.h"

/* The abstract base of the turntable platforms: the discs that spin in place and
 * carry Mario round with them. `kaitendai` is a revolving stand.
 *
 * A LAYER THE TREE DID NOT HAVE. Five classes read as direct dBgActor_c subclasses
 * because the tree named the base's base. The ROM's RTTI names this one and points
 * it at dBgActor_c.
 *
 *   _ZTI16daObjKaitendai_c  ov002 0x021091ac
 *   _ZTS16daObjKaitendai_c  ov002 0x021091b8   "16daObjKaitendai_c"
 *   vtable                  ov002 0x021091d4, 32 slots, same count as the base
 *   kind                    __si_class_type_info, ONE base, subobject offset 0
 *   base                    dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null. Its
 * own overrides are slots 6 (Behavior), 9 (Render), 16 (D1) and 17 (D0); every
 * other slot holds the base's word and is inherited.
 *
 * FIVE DESCENDANTS, more than any other class in this series:
 * daObjBk_Ukisima_c (was RotatingPlatformWf), daObjFl_Koma_D_c (RotatingPlatformLll),
 * daObjWc_Obj07_c (was RotatingPlatformWdw), daObjRc_Kaitendai_c (was RotatingPlatformRr)
 * and daObjKm3_Kaitendai_c (include/daObjKm3_Kaitendai_c.h). Each one's destructor
 * stores this class's vtable between its own and _ZTV10dBgActor_c, which is the
 * same fact the RTTI records, arrived at from the other direction.
 *
 * NO FIELDS, and that is measured rather than assumed. Its own Behavior
 * (ov002 0x020b6718, now `_ZN16daObjKaitendai_c8BehaviorEv`) reads this+0x94,
 * which is dBgActor_c's mPrevAngleY, and
 * otherwise only calls dBgActor_c::UpdateModelPosAndRotY and the guarded
 * UpdateClsnPosAndRot. Its own Render (`func_ov002_020b66f0`) dispatches through
 * the Model at 0xd4. Its own destructor destroys only dBgActor_c's two members. And
 * all five factories -- daObjBk_Ukisima_c_classInit, daObjFl_Koma_D_c_classInit,
 * daObjWc_Obj07_c_Spawn, RotatingPlatformRr_Spawn and `func_ov047_021113bc`,
 * which is daObjKm3_Kaitendai_c's real factory -- pass 800 = 0x320 to
 * fBase_c::operator new, which is sizeof(dBgActor_c) exactly. There is no room for
 * a field anywhere in this class or in any of the five leaves.
 *
 * NOT daObjKm3_Kurumajiku_c_classInit (historical alias RickshawBs_Spawn).
 * That function allocates 816 and builds
 * daObjKm3_Kurumajiku_c; the ov047 "Bs" names were crossed -- daObjKm3_Kaitendai_c
 * itself was misnamed RickshawBs until the rename in include/daObjKm3_Kaitendai_c.h,
 * settled by vtable evidence rather than by either factory's own name.
 */

#ifdef __cplusplus

struct daObjKaitendai_c : dBgActor_c {
    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN16daObjKaitendai_cD1Ev (which does exist out of line, at ov002
       0x020b6664, still under its func_ov002_ name). An out-of-line declaration
       here would make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjKaitendai_c() {}

    /* Slot 6, this class's own override, defined out of line in
       src/_ZN16daObjKaitendai_c8BehaviorEv.cpp. LAYOUT-NEUTRAL: it re-uses the
       slot dBgActor_c already holds rather than appending one, and adds no
       field, so the 0x320 assert below is untouched.

       Because the destructor above is inline this class has no key function,
       so declaring the first out-of-line virtual makes THIS the key function
       and its translation unit emits _ZTV16daObjKaitendai_c, the RTTI records
       and the implicit destructor bodies alongside the one function the file
       is for. That is the same shape every migrated D1 file in this family
       already has, and tools/objisolate.py reduces the object to the declared
       function before eligible.py and rombuild.py judge it -- checked on this
       file, not assumed. */
    s32 Behavior();
    s32 Render();                      /* slot 9, ov002 0x020b66f0 -- mModel.Render(0) */
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
};

typedef char daObjKaitendai_c_size_must_be_0x320[sizeof(daObjKaitendai_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKAITENDAI_C_H */
