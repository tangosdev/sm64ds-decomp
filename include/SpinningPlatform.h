#ifndef SPINNINGPLATFORM_H
#define SPINNINGPLATFORM_H

#include "dBgActor_c.h"
#include "ShadowModel.h"

/* A clock-stage platform that rotates around all three axes and projects a
 * height-sensitive cuboid shadow onto the floor below it.
 *
 * The cartridge's __si_class_type_info record calls this class
 * daObjCtMecha11_c and names dBgActor_c as its sole base at offset zero. The
 * matched method symbols use the readable compatibility spelling
 * SpinningPlatform, and `_ZTV16SpinningPlatform` is an existing symbol alias
 * for the same 32-slot ROM table at 0x02112bcc. Compiler-emitted
 * SpinningPlatform RTTI in an isolated method object is only a passenger; it
 * must not be mistaken for the cartridge's daObjCtMecha11_c identity.
 *
 * dBgActor_c's non-POD data ends at 0x31e, so the Itanium layout reuses its two
 * bytes of tail padding for the first derived member. The factory allocation,
 * destructor member offsets, and shadow-update body close the object exactly
 * at 0x380. Field names below are descriptive compatibility names; offsets and
 * widths are the ROM-evidenced claims. */
struct SpinningPlatform : dBgActor_c {
    s8  mRandDirection;             /* 0x31e */
    u8  pad_31f;                    /* 0x31f */
    u16 mRandTimer;                 /* 0x320 */
    u16 mRandFrames;                /* 0x322 */
    s32 mFloorPosY;                 /* 0x324 */
    ShadowModel mShadowModel;       /* 0x328 */
    Matrix4x3 mShadowMat;           /* 0x350 */

    virtual ~SpinningPlatform();    /* slots 16, 17 */

    virtual int InitResources();    /* slot  0 */
    virtual int CleanupResources(); /* slot  3 */
    virtual int Behavior();         /* slot  6 */
    virtual int Render();           /* slot  9 */

    int UpdateShadow();
    void UpdateModel();
};

typedef char SpinningPlatform_size_must_be_0x380[
    sizeof(SpinningPlatform) == 0x380 ? 1 : -1];

#endif
