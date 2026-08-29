/* class dScMgSmartball_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgSmartball_c : dScMgBase_c, single edge, offset 0 (build/rtti.json).
 * Its own vtable is ov006:0x0213eefc; the RTTI pointer one word below it is
 * _ZTI16dScMgSmartball_c. The LAST direct child of dScMgBase_c to become a
 * real class -- with this, every RTTI descendant of dScMgBase_c is one.
 *
 * A DIRECT child, with no intermediate: its own D1 stores this class's
 * vtable, destroys the four arrays, and calls _ZN11dScMgBase_cD2Ev
 * straight away -- no second vptr store, which is what an inlined
 * intermediate base's destructor would have left behind.
 *
 * SIZE 0x629c, from MgBingoBallSlotsShot_Spawn's own
 * `_ZN7fBase_cnwEj(0x629c)`, and the last array closes EXACTLY on it:
 * 0x599c + 0x40 * 0x24 = 0x629c. The English _Spawn name is already
 * attributed and is kept, the same convention as MgPicturePoker_Spawn for
 * dScMgCard_c.
 *
 * FACTORY AND DESTRUCTOR AGREE MEMBER FOR MEMBER, in exact reverse order:
 * built 0x47c8, 0x4854, 0x48d4, 0x599c; destroyed 0x599c, 0x48d4, 0x4854,
 * 0x47c8. Nothing is left over either way. Three of the four share an
 * element ctor/dtor pair (func_0203d738 / NullDestructor_0203d47c) so they
 * are the same element type as each other; mArray4's is its own
 * (func_ov006_02119900 / func_ov006_0210d894).
 *
 * THE PADS HERE ARE NOT UNREAD SPACE, AND THIS FILE WILL NOT PRETEND THEY
 * ARE. A scan of this class's own seven recovered methods -- checked for
 * split-literal forms as well as whole constants -- finds 45 distinct
 * offsets between 0x4660 and 0x629c, thirteen of them inside pad_4954
 * alone. They are real matched access; they are simply not modelled yet,
 * because each one's WIDTH has to come from its access site and that is a
 * separate reading pass from making the class real. Naming them changes no
 * byte and needs no migration -- every one of those seven files reaches
 * them by raw offset today and will keep working untouched.
 *
 * So: the four arrays below are evidenced by two witnesses each and the
 * size is evidenced by a third. Everything else is honest padding-for-now,
 * not a claim that nothing reads it.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN16dScMgSmartball_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, the immediate base, already provides one. */
#ifndef DSCMGSMARTBALL_C_H
#define DSCMGSMARTBALL_C_H
#include "dScMgBase_c.h"

extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void NullDestructor_0203d47c(void);
extern "C" void func_ov006_0210d894(void);

struct dScMgSmartball_c : dScMgBase_c {
    virtual ~dScMgSmartball_c();

    /* This class's own overrides, read off the ROM's vtable: the slots where the
       table differs from dScMgBase_c's. Spelled WITHOUT the `virtual` keyword, the
       way include/daObjMarioCap_c.h and include/daObjRc_Dorifu_c.h spell theirs --
       an override of a virtual an ancestor already declares is implicitly virtual
       either way, so each reuses an existing slot and adds no field, and the
       0x629c assert below still holds. The destructor above is declared first and
       out of line, so it stays this class's KEY FUNCTION and none of these
       translation units emits _ZTV16dScMgSmartball_c.

       AfterCleanupResources takes dScMgBase_c's signature exactly, u32 and all.
       The pre-migration file spelled the parameter `int mode` and still matched,
       because the ROM only ever compares it against 2 -- but the declaration it is
       overriding is `void AfterCleanupResources(u32)`, so that is what it is. */
    void AfterCleanupResources(u32 vfSuccess);
                           /* slot  5 -- src/_ZN16dScMgSmartball_c21AfterCleanupResourcesEj.cpp */
    s32  Behavior();       /* slot  6 -- src/_ZN16dScMgSmartball_c8BehaviorEv.cpp */
    s32  Render();         /* slot  9 -- src/_ZN16dScMgSmartball_c6RenderEv.cpp */

    u8 pad_4660[0x168];   /* 0x4660 -- dScMgBase_c's data ends here; real matched
                              access inside, see file banner */
    u8 mArray1[0x28];     /* 0x47c8 -- 5 * 8,      elem dtor NullDestructor_0203d47c */
    u8 pad_47f0[0x64];    /* 0x47f0 -- real matched access, see file banner */
    u8 mArray2[0x80];     /* 0x4854 -- 0x10 * 8,   elem dtor NullDestructor_0203d47c */
    u8 mArray3[0x80];     /* 0x48d4 -- 0x10 * 8,   elem dtor NullDestructor_0203d47c */
    u8 pad_4954[0x1048];  /* 0x4954 -- real matched access (13 offsets), see banner */
    u8 mArray4[0x900];    /* 0x599c -- 0x40 * 0x24, elem dtor func_ov006_0210d894 */
};

typedef char dScMgSmartball_c_size_must_be_0x629c[sizeof(dScMgSmartball_c) == 0x629c ? 1 : -1];

#endif
