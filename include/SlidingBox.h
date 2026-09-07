#ifndef SLIDINGBOX_H
#define SLIDINGBOX_H

#include "dBgActor_c.h"
#include "dBgCh_Actr.h"

/* The cartridge RTTI calls this class daSlide_Box_c. SlidingBox is the
 * readable compatibility spelling already used by all six named functions.
 * The ROM's __si_class_type_info record proves one direct base, dBgActor_c at
 * offset zero, and the 32-slot vtable differs from dBgActor_c only at slots
 * 0, 3, 6, 9, 16 and 17.
 *
 * daSlide_Box_c_classInit allocates 0x4f8 bytes, constructs dBgActor_c, then the
 * dBgCh_Actr at 0x324. Both destructor variants destroy that member before
 * the inherited dBgActor_c teardown, independently pinning the layout.
 *
 * SM64DS RTTI names the implementation daSlide_Box_c. The reconstructed factory
 * daSlide_Box_c_classInit (historical alias SlidingBox_Spawn) installs this class's
 * cartridge vtable for the SLIDE_BOX registry profile.
 */
struct SlidingBox : dBgActor_c {
    dActor_c *mShip;                    /* 0x320 - actor 0x39 carrying the box */
    dBgCh_Actr mWithMeshClsn;           /* 0x324 */
    Vector3 mBasePos;                   /* 0x4e0 - grounded position on the ship */
    Fix12i mHorzPos;                    /* 0x4ec - travel along the ship's tilt */
    u32 mSoundID;                       /* 0x4f0 - rolling sound handle */
    u8 mState;                          /* 0x4f4 */
    u8 pad_4f5[0x3];

    virtual ~SlidingBox();              /* slots 16, 17 */

    virtual s32 InitResources();        /* slot  0 */
    virtual s32 CleanupResources();     /* slot  3 */
    virtual s32 Behavior();             /* slot  6 */
    virtual s32 Render();               /* slot  9 */

    /* Builds mModel.mat4x3 from the actor angles and position. The anonymous
     * ROM member at 0x021130a4 is called only by InitResources and Behavior,
     * both in this high-confidence TU, and takes this in r0. */
    void UpdateModel();
};

typedef char SlidingBox_size_must_be_0x4f8[
    sizeof(SlidingBox) == 0x4f8 ? 1 : -1];

#endif /* SLIDINGBOX_H */
