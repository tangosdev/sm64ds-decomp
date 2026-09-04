#ifndef DAOBJUKIYUKA_C_H
#define DAOBJUKIYUKA_C_H

#include "types.h"

/* The abstract base of the floating floors: the slabs that sink under your weight
 * and bob back up. `ukiyuka` is a floating floor.
 *
 * A LAYER THE TREE DID NOT HAVE. This header used to be a flat struct under
 * `u8 pad_000[0x320]`, emitted by `tools/rtti_vtables.py --emit-headers` because
 * that pass knew offsets and not sizeof(base). include/dBgActor_c.h has since
 * settled sizeof(dBgActor_c) = 0x320, so the class can be spelled as what it is. The
 * regenerate line is gone with the generated body: that tool deletes only files
 * that still carry it, and this one is hand-written now.
 *
 *   _ZTI14daObjUkiyuka_c  ov002 0x02109104
 *   _ZTS14daObjUkiyuka_c  ov002 0x02109110   "14daObjUkiyuka_c"
 *   vtable                ov002 0x0210912c, 32 slots, same count as the base
 *   kind                  __si_class_type_info, ONE base, subobject offset 0
 *   base                  dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null. Its
 * own overrides are slots 6 (Behavior), 9 (Render), 16 (D1) and 17 (D0).
 *
 * TWO DESCENDANTS: daObjFl_Ukiyuka_c (FloatingFloorLllSmall, which has a second
 * factory FloatingFloorLllBig_Spawn building the same class with different
 * parameters) and daObjKm2_Ukishima_c (daObjKm2_Ukishima_c).
 *
 * FOUR FIELDS, all of them read by this class's own Behavior, ov002 0x020b6494
 * -- now `_ZN14daObjUkiyuka_c8BehaviorEv`:
 *
 *   0x320  the rest height. Behavior compares the actor's own Y at 0x60 against it
 *          and, when the two are equal, restarts the rest timer.
 *   0x324  the bob amplitude, multiplied by sin(phase) and subtracted from Y.
 *   0x328  the bob phase, stepped 0x100 a frame.
 *   0x32a  the rest timer, 0x3c frames. While DecIfAbove0_Short is counting it
 *          down Behavior does nothing but the collider range check.
 *
 * SIZE 0x32c. 0x32a + 2 closes the class, and FloatingFloorBfs_Spawn passes
 * 812 = 0x32c to fBase_c::operator new, which is this class and nothing more:
 * daObjKm2_Ukishima_c adds no field of its own. THE OTHER LEAF IS BIGGER --
 * FloatingFloorLllSmall_Spawn and FloatingFloorLllBig_Spawn both pass 816 = 0x330
 * -- so daObjFl_Ukiyuka_c has 4 bytes this class does not, which is what fixes the
 * boundary at 0x32c rather than anywhere higher.
 *
 * Field NAMES are coined from what the code does; nothing in the ROM names them.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjUkiyuka_c : dBgActor_c {
    /* Field NAMES are placeholders. Offsets, widths and types are observed. */
    s32 mRestY;             /* 0x320 */
    s32 mBobAmplitude;      /* 0x324 */
    s16 mBobPhase;          /* 0x328 */
    u16 mRestTimer;         /* 0x32a */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN14daObjUkiyuka_cD1Ev (which does exist out of line, at ov002
       0x020b63e0, still under its func_ov002_ name). An out-of-line declaration
       here would make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjUkiyuka_c() {}

    /* Slot 6, this class's own override, defined out of line in
       src/_ZN14daObjUkiyuka_c8BehaviorEv.cpp. LAYOUT-NEUTRAL: it re-uses the
       slot dBgActor_c already holds rather than appending one, and adds no
       field, so the 0x32c assert below is untouched.

       Because the destructor above is inline this class has no key function,
       so declaring the first out-of-line virtual makes THIS the key function
       and its translation unit emits _ZTV14daObjUkiyuka_c, the RTTI records
       and four implicit destructor bodies alongside the one function the file
       is for. That is the same shape every migrated D1 file in this family
       already has, and tools/objisolate.py reduces the object to the declared
       function before eligible.py and rombuild.py judge it -- checked on this
       file, not assumed. */
    s32 Behavior();
    s32 Render();                      /* slot 9, ov002 0x020b646c -- mModel.Render(0) */
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

typedef char daObjUkiyuka_c_size_must_be_0x32c[sizeof(daObjUkiyuka_c) == 0x32c ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base sub-object to
   inherit dBgActor_c's fields from and so spells the layout flat. Same arrangement
   as include/dBgActor_c.h. */
struct daObjUkiyuka_c {
    u8  pad_000[0x320];
    s32 mRestY;             /* 0x320 */
    s32 mBobAmplitude;      /* 0x324 */
    s16 mBobPhase;          /* 0x328 */
    u16 mRestTimer;         /* 0x32a */
};

#endif /* __cplusplus */

#endif /* DAOBJUKIYUKA_C_H */
