#ifndef DAOBJMC_METALNET_C_H
#define DAOBJMC_METALNET_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Peach's Castle chain-link net (profile MC_METALNET / METAL_NET 339). A
 * climbable mesh with no state of its own. ov009 is mixed -- CASTLE_WATER
 * (338) and FLAG (342) sit next door -- this is the net, not those.
 * MetalNetLift (ov064, _ZTV12MetalNetLift at 0x0211bc68) is a different class.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov009 0x02113aa4  "18daObjMc_Metalnet_c"
 *   _ZTI  ov009 0x02113a98  __si_class_type_info (vptr 0x0209a764); +8 ->
 *                           _ZTI10dBgActor_c (ov002 0x021089ec), so the DIRECT
 *                           base is dBgActor_c and nothing else.
 *   _ZTV  ov009 0x02113ae0  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x02111f40).
 *   size  0x320             daObjMc_Metalnet_c_classInit's own literal (800),
 *                           which is also dBgActor_c's: this class adds no
 *                           field.
 *   profile g_profile_MC_METALNET at ov009 0x02113abc.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c at ov002
 * 0x0210ae38. Exactly the slots declared below differ; all other words are
 * the base's own, including slot 31 (Kill, 0x020ee55c). Slot 12 is the one
 * this family usually leaves alone: fBase_c::OnPendingDestroy at 0x02043ac0
 * is replaced by a 4-byte empty body at 0x02111ea4.
 */
struct daObjMc_Metalnet_c : dBgActor_c {
    /* No field of its own. dBgActor_c ends at 0x31e and rounds to 0x320. */

    /* MEASURED -- INLINE ON PURPOSE. Out of line, mwccarm 2004/b56 emits D0
       before D1 (the reverse of the cartridge's 0x02111dc4 D1 / 0x02111e08 D0)
       and a homeless D2, and objisolate rejects the whole translation unit.
       Defined in the class body it emits D1 then D0 and no D2. Safe here
       because the class is a leaf: nothing derives from it. */
    virtual ~daObjMc_Metalnet_c() {}        /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();          /* slot  0 */
    virtual s32   CleanupResources();       /* slot  3 */
    virtual s32   Behavior();               /* slot  6 */
    virtual s32   Render();                 /* slot  9 */
    /* fBase_c's slot, overridden with an empty body -- 4 bytes, a bare `bx lr`
       at 0x02111ea4. The override exists only to stop the base's own
       OnPendingDestroy from running. */
    virtual void  OnPendingDestroy();       /* slot 12 */

    /* Leaf operator new until #2570 puts the same allocator on fBase_c.
       Parameter is size_t (unsigned long on this compiler). `return new`
       relocates to `_Znwm` without this. */
    static void *operator new(unsigned long size);
};

inline void *daObjMc_Metalnet_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

typedef char daObjMc_Metalnet_c_size_must_be_0x320[sizeof(daObjMc_Metalnet_c) == 0x320 ? 1 : -1];

#endif /* DAOBJMC_METALNET_C_H */
