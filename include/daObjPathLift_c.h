/* Derives from dPathLiftActor_c (include/PathLift.h), which itself derives
 * from dBgActor_c. The base identity is ROM-proven by
 * _ZTI16dPathLiftActor_c/_ZTS16dPathLiftActor_c at ov002:0x0210af0c.
 *
 *   _ZTI15daObjPathLift_c  ov100 0x02148538
 *   _ZTS15daObjPathLift_c  ov100 0x02148544
 *   _ZTV15daObjPathLift_c  ov100 0x0214857c
 *
 * overlay_actors maps ov100 profile 31 to PATH_LIFT. ov100 is mixed
 * (PATH_LIFT / ROLLING_IRON_BALL / BUTTERFLY / UNCHAINED_CHOMP / FISH /
 * DOOR / STAR_DOOR) -- this is the path lift, not the star gate.
 *
 * SIZE 0x4b4, the literal daObjPathLift_c_classInit factory passes to
 * fBase_c::operator new. dPathLiftActor_c ends at 0x450; the D1 destructor
 * destroys only a dExtShadowModel_c at 0x450 before storing the base vtable and
 * running dPathLiftActor_c's destructor (dExtSimpleModel_c[3] array, inlined per
 * include/PathLift.h), so dExtShadowModel_c is this class's only member with a
 * constructor/destructor of its own.
 *
 * mWaitTimer, mRespawnAfterFall, mPathDirection and mPathSpeed are NOT this
 * class's own fields -- they are all < 0x450, inside dPathLiftActor_c's own
 * storage. This class's methods reach them as inherited members.
 *
 * mShadowMat is the drop-shadow matrix UpdateShadow fills and hands
 * to dActor_c::DropShadowScaleXYZ; 0x478 + 0x30 = 0x4a8. Kept as s32[12]:
 * a Matrix4x3 member (Vector3 translation) is non-trivial and changes the
 * factory.
 */
#ifndef DAOBJPATHLIFT_C_H
#define DAOBJPATHLIFT_C_H
#include "types.h"

#ifdef __cplusplus

#include "PathLift.h"
#include "dExtShadowModel_c.h"

struct daObjPathLift_c : dPathLiftActor_c {
    dExtShadowModel_c mShadowModel; /* 0x450 */
    s32 mShadowMat[12];             /* 0x478 -- [9..11] the translation, pos >> 3 */
    u32 mSoundHandle;               /* 0x4a8 */
    s32 mGroundY;                   /* 0x4ac */
    u8 mTimer;                      /* 0x4b0 */

    virtual ~daObjPathLift_c() {}

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();

    /* ov100 0x0214700c: mModel's matrix from the pitch and yaw plus
       position >> 3; returns the z word, which no caller reads. */
    int UpdateModelMatrix();
    /* ov100 0x02146e70: place mShadowModel under the lift (skipped in
       level 13 and when the lift is below its ground). */
    void UpdateShadow();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjPathLift_c_size_must_be_0x4b4[sizeof(daObjPathLift_c) == 0x4b4 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif
