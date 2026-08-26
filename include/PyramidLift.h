/* PyramidLift, reconstructed from its factory, destructor pair, vtable and
 * five matched methods.
 *
 * BASE: dBgActor_c, direct. The cartridge's __si_class_type_info record at
 * ov025 0x02113994 points to _ZTI10dBgActor_c at ov002 0x021089ec with
 * subobject offset zero. The ROM RTTI name at 0x021139a0 is `10daDpLift_c`,
 * while every configured method name uses the compatibility spelling
 * `PyramidLift`. Consequently a compiler object whose key function emits
 * `_ZTI11PyramidLift`/`_ZTS11PyramidLift` is not ROM metadata and must remain
 * discarded by per-function object isolation. `_ZTV11PyramidLift` is likewise
 * a compiler-facing alias for the ROM-owned daDpLift_c address point; its slot
 * shape and targets are nevertheless checked directly below.
 *
 * SIZE 0x3fc is the literal passed by PyramidLift_Spawn to fBase_c::operator
 * new. dBgActor_c occupies 0x000..0x31f. The factory constructs the Model at
 * 0x320 and a ten-element, 0xc-stride array at 0x37c. The D1/D0 pair destroys
 * that array through _ZN7Vector3D1Ev, then destroys the Model and inherited
 * dBgActor_c members in reverse construction order. Together those independent
 * witnesses close every non-padding span below.
 *
 * THE VTABLE at ov025 0x021139d4 has the same 32 slots as dBgActor_c. Only
 * slots 0, 3, 6, 9, 16 and 17 differ, exactly the overrides declared here.
 */
#ifndef PYRAMIDLIFT_H
#define PYRAMIDLIFT_H

#include "dBgActor_c.h"

struct PyramidLift : dBgActor_c {
    Model mModel2;                    /* 0x320 */
    s32 mBasePosX;                    /* 0x370 */
    s32 mBasePosY;                    /* 0x374 */
    s32 mBasePosZ;                    /* 0x378 */
    Vector3 mBulletPositions[10];     /* 0x37c */
    u16 mShakeTimer;                  /* 0x3f4 */
    u8  mState;                       /* 0x3f6 */
    u8  mHadClsn;                     /* 0x3f7 */
    u8  mNextBullet;                  /* 0x3f8 */
    u8  pad_3f9[0x3];

    /* INLINE ON PURPOSE. The ROM orders the independently enrolled variants
       D1 then D0 and has no D2 home. Explicit use in the two destructor files
       makes mwcc emit the required variant while objisolate discards the
       helper and the other variant. */
    virtual ~PyramidLift() {}

    virtual s32 InitResources();       /* slot  0 */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 Behavior();            /* slot  6 */
    virtual s32 Render();              /* slot  9 */
};

typedef char PyramidLift_size_must_be_0x3fc[sizeof(PyramidLift) == 0x3fc ? 1 : -1];

#endif /* PYRAMIDLIFT_H */
