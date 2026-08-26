#ifndef TTCROTATINGCUBE_H
#define TTCROTATINGCUBE_H

#include "dBgActor_c.h"
#include "ShadowModel.h"

/* The cartridge RTTI calls this class daObjCtRotateBlock_c. TtcRotatingCube is
 * the readable compatibility spelling carried by all six ROM-addressed
 * virtuals. The __si_class_type_info record proves one direct dBgActor_c base
 * at offset zero, and the 32-slot table has exactly the same extent as that
 * base. Slots 0, 3, 6, 9, 16 and 17 are the only overrides. symbols.txt keeps
 * `_ZTV15TtcRotatingCube` and the cartridge-derived
 * `_ZTV20daObjCtRotateBlock_c` at the same address; real member definitions use
 * the readable spelling while relocation isolation binds that spelling back to
 * the cartridge table.
 *
 * Both TtcRotatingCube_Spawn and TtcRotatingPrism_Spawn allocate 0x3d8 bytes,
 * construct this exact base/member chain, and install the same vtable. Their
 * actor IDs select mVariant during InitResources, so they are two actor entries
 * for one class rather than evidence for two C++ types.
 */
struct TtcRotatingCube : dBgActor_c {
    Model mRotatingModel;                 /* 0x320 */
    s32 mOffsetY;                         /* 0x370 */
    u16 mWaitTimer;                       /* 0x374 */
    u8 mState;                            /* 0x376 */
    u8 mVariant;                          /* 0x377 */
    s16 mTargetAngleZ;                    /* 0x378 */
    u8 mUnevenGround;                     /* 0x37a */
    u8 mPad37b;                           /* 0x37b */
    s32 mFloorY;                          /* 0x37c */
    ShadowModel mShadowModel;             /* 0x380 */
    Matrix4x3 mShadowMat;                 /* 0x3a8 */

    virtual ~TtcRotatingCube();           /* slots 16, 17 */

    virtual s32 InitResources();          /* slot  0 */
    virtual s32 CleanupResources();       /* slot  3 */
    virtual s32 Behavior();               /* slot  6 */
    virtual s32 Render();                 /* slot  9 */

    /* These three routines are anonymous in the image. Their names describe
     * their exclusive calls and field effects; they are not claimed as
     * cartridge-authenticated EAD spellings. */
    s32 UpdateShadow();
    void UpdateClsn();
    void UpdateModel();
};

typedef char TtcRotatingCube_size_must_be_0x3d8[
    sizeof(TtcRotatingCube) == 0x3d8 ? 1 : -1];

#endif
