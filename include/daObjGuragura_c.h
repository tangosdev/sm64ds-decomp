#ifndef DAOBJGURAGURA_C_H
#define DAOBJGURAGURA_C_H

#include "types.h"

/* The abstract base of the tilting platforms: the slabs that wobble under your
 * weight and settle back when you step off. `guragura` is the wobble itself.
 *
 * A LAYER THE TREE DID NOT HAVE. This header used to be a flat struct under
 * `u8 pad_000[0x330]`, emitted by `tools/rtti_vtables.py --emit-headers` because
 * that pass knew offsets and not sizeof(base). include/Platform.h has since
 * settled sizeof(Platform) = 0x320, so the class can be spelled as what it is. The
 * regenerate line is gone with the generated body: that tool deletes only files
 * that still carry it, and this one is hand-written now.
 *
 *   _ZTI15daObjGuragura_c  ov002 0x0210905c
 *   _ZTS15daObjGuragura_c  ov002 0x02109068   "15daObjGuragura_c"
 *   vtable                 ov002 0x02109084, 32 slots, same count as the base
 *   kind                   __si_class_type_info, ONE base, subobject offset 0
 *   base                   dBgActor_c, ov002 0x021089ec -- the tree's Platform
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null. Its
 * own overrides are slots 6 (Behavior), 9 (Render), 16 (D1) and 17 (D0).
 *
 * TWO DESCENDANTS: daObjKm2_Gura_c (TiltingPlatformBfs) and daObjFl_Gura_c
 * (TiltingPlatformLll). Each one's destructor stores this class's vtable between
 * its own and _ZTV8Platform.
 *
 * SIZE 0x350, the literal both factories pass to ActorBase::operator new.
 *
 * TWO QUATERNIONS, AND THEY CLOSE ON EACH OTHER. This class's own Behavior,
 * `func_ov002_020b616c`, ends with
 *
 *     Quaternion_SLerp(this+0x320, this+0x330, 0x199, this+0x320)
 *
 * so 0x320 is the current orientation and 0x330 the target, both 4 x s32 = 0x10.
 * 0x320 + 0x10 = 0x330 and 0x330 + 0x10 = 0x340, and the same function writes the
 * target as exactly four words from data_02092768[0..3]. The generated body had
 * those four words as four separate s32 at 0x330/0x334/0x338/0x33c and no 0x320 at
 * all; they are one quaternion.
 *
 *   0x34c  cleared at the end of every Behavior. Something else sets it; nothing
 *          this class's own methods do.
 *   0x34d  the settle delay. While DecIfAbove0_Byte is still counting it down the
 *          target quaternion is left alone; when it hits zero the target is
 *          reloaded from the table.
 *
 * 0x34d + 1 = 0x34e, and the quaternions force 4-byte alignment, so sizeof rounds
 * to 0x350 -- the factories' literal. 0x340..0x34c is UNOBSERVED, not inherited:
 * nothing this class's own methods touch lands there and neither leaf declares
 * anything there. Field NAMES are coined from what the code does; nothing in the
 * ROM names them.
 */

#ifdef __cplusplus

#include "Platform.h"

struct daObjGuragura_c : Platform {
    /* Field NAMES are placeholders. Offsets, widths and types are observed. */
    s32 mTilt[4];           /* 0x320 - quaternion, the live orientation */
    s32 mTiltTarget[4];     /* 0x330 - quaternion, what it lerps toward */
    u8  pad_340[0xc];
    u8  mBumped;            /* 0x34c - cleared every frame by Behavior */
    u8  mSettleDelay;       /* 0x34d - frames before the target is reloaded */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/Platform.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN15daObjGuragura_cD1Ev (which does exist out of line, at ov002
       0x020b6030, still under its func_ov002_ name). An out-of-line declaration
       here would make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjGuragura_c() {}
};

typedef char daObjGuragura_c_size_must_be_0x350[sizeof(daObjGuragura_c) == 0x350 ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base sub-object to
   inherit Platform's fields from and so spells the layout flat. Same arrangement
   as include/Platform.h. */
struct daObjGuragura_c {
    u8  pad_000[0x320];
    s32 mTilt[4];           /* 0x320 */
    s32 mTiltTarget[4];     /* 0x330 */
    u8  pad_340[0xc];
    u8  mBumped;            /* 0x34c */
    u8  mSettleDelay;       /* 0x34d */
};

#endif /* __cplusplus */

#endif /* DAOBJGURAGURA_C_H */
