#ifndef DAOBJMC_METALNET_C_H
#define DAOBJMC_METALNET_C_H

#include "types.h"
#include "dBgActor_c.h"

/* The MC_METALNET profile's chain-link net: a static mesh the player can climb,
 * with no state of its own. It decides ONCE, in InitResources, whether it is in
 * the level at all -- past the 150-star mark it returns 0 and is destroyed
 * before it is ever drawn -- and after that it only keeps its collider where
 * its model is.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov009 0x02113aa4  "18daObjMc_Metalnet_c"
 *   _ZTI  ov009 0x02113a98  __si_class_type_info (vptr 0x0209a764); +8 ->
 *                           _ZTI10dBgActor_c (ov002 0x021089ec), so the DIRECT
 *                           base is dBgActor_c and nothing else.
 *   _ZTV  ov009 0x02113ae0  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x02111f40). The vptr store is
 *                           addend-0 against this symbol.
 *   size  0x320             daObjMc_Metalnet_c_classInit's own literal (800),
 *                           which is also dBgActor_c's: this class adds no
 *                           field, so the two sizes are the same number for the
 *                           same reason and not by coincidence.
 * The coined MetalNet alias that used to sit on this vtable is gone; the ROM's
 * own type string is where the class name now comes from. MetalNetLift (ov064)
 * is a DIFFERENT class with its own RTTI and is untouched by that.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c at ov002
 * 0x0210ae38. Exactly the slots declared below differ; all 33 other words are
 * the base's own, including slot 31 (Kill, 0x020ee55c), so nothing else is
 * overridden and nothing else is redeclared here.
 */
struct daObjMc_Metalnet_c : dBgActor_c {
    /* No field of its own. dBgActor_c ends at 0x31e and rounds to 0x320; unlike
       daObjRc_Guruguru_c and daObjC0_Switch_c this class puts nothing in that
       tail padding, which is why sizeof is the base's own 0x320 unchanged. */

    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these two
       are emitted; with the body out of line mwcc emits D0 ahead of D1 and the
       ROM has D1 first (rombuild refuses the object outright). An inline body
       also drops the D2 variant the cartridge never carried. Declaring it FIRST
       is what makes this TU the vtable's home, so _ZTV and the RTTI pair land
       here rather than in whichever other TU happens to name them. */
    virtual ~daObjMc_Metalnet_c() {}        /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();          /* slot  0 */
    virtual s32   CleanupResources();       /* slot  3 */
    virtual s32   Behavior();               /* slot  6 */
    virtual s32   Render();                 /* slot  9 */
    /* fBase_c's slot, overridden with an empty body -- 4 bytes, a bare `bx lr`
       at 0x02111ea4. The override exists only to stop the base's own
       OnPendingDestroy from running. */
    virtual void  OnPendingDestroy();       /* slot 12 */
};

typedef char daObjMc_Metalnet_c_size_must_be_0x320[sizeof(daObjMc_Metalnet_c) == 0x320 ? 1 : -1];

#endif /* DAOBJMC_METALNET_C_H */
