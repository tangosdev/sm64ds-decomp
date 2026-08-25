/* Derives from PathLift (include/PathLift.h), which itself derives from
 * dBgActor_c. RTTI's own class for this level is `dPathLiftActor_c`
 * (ov002:0x0210af0c), but every mangled symbol in the ROM spells the base
 * `PathLift` (_ZTV8PathLift, _ZN8PathLiftD1Ev, ...), so PathLift.h already
 * uses that name and this header follows it.
 *
 * SIZE 0x4b4, the literal PathLift_Spawn (src/PathLift_Spawn.cpp) passes to
 * fBase_c::operator new. PathLift ends 0x450; the D1 destructor
 * (func_ov100_02146d7c, this class's own) destroys only a ShadowModel at
 * 0x450 before storing PathLift's own vtable and running PathLift's
 * destructor (Model[3] array, inlined per include/PathLift.h), so
 * ShadowModel is this class's only member with a constructor/destructor of
 * its own.
 *
 * unk_428, unk_42c, unk_43c and unk_440 are NOT this class's own fields --
 * they are all < 0x450, inside PathLift's own generic tail padding
 * (include/PathLift.h's pad_42c[0x20] and PathPtr's tail neighbours), the
 * same idiom include/dBgActor_c.h documents for ArmedRotatingPlatform and
 * include/Crate.h now documents for its own 0xd0 reuse. This class's methods
 * reach them by raw offset instead of declaring colliding members. */
#ifndef DAOBJPATHLIFT_C_H
#define DAOBJPATHLIFT_C_H
#include "types.h"

#ifdef __cplusplus

#include "PathLift.h"
#include "ShadowModel.h"

struct daObjPathLift_c : PathLift {
    ShadowModel mShadowModel;   /* 0x450 */
    u8  pad_478[0x30];
    u32 mSoundHandle;                /* 0x4a8 */
    s32 mGroundY;                /* 0x4ac */
    u8  mTimer;                /* 0x4b0 */

    virtual ~daObjPathLift_c();

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
};

typedef char daObjPathLift_c_size_must_be_0x4b4[sizeof(daObjPathLift_c) == 0x4b4 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct daObjPathLift_c {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x8];
    s32 mHorzSpeed;            /* 0x098 */
    u8  pad_09c[0x38c];
    u16 unk_428;            /* 0x428 */
    u8  pad_42a[0x2];
    u8  unk_42c;            /* 0x42c */
    u8  pad_42d[0xf];
    s32 unk_43c;            /* 0x43c */
    s32 unk_440;            /* 0x440 */
    u8  pad_444[0x64];
    u32 mSoundHandle;            /* 0x4a8 */
    s32 mGroundY;            /* 0x4ac */
    u8  mTimer;            /* 0x4b0 */
};

#endif /* __cplusplus */

#endif
