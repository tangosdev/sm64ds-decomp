/* daDpLift_c -- the DP_LIFT actor, reconstructed from its factory, destructor
 * pair, vtable and five matched methods.
 *
 * NAME: the cartridge spells this class daDpLift_c. In ov025 (base 0x021111a0)
 * the typeinfo word of the vtable header, at 0x021139d0 (address point - 4),
 * is 0x02113994. That __si_class_type_info record reads [0x0209a764
 * (_ZTVN3abi20__si_class_type_infoE), 0x021139a0, 0x021089ec], and the
 * string at 0x021139a0 is "10daDpLift_c\0". The tree previously called the
 * class PyramidLift, a coined name present only on the vtable address.
 *
 * BASE: dBgActor_c, direct -- the record's third word is _ZTI10dBgActor_c at
 * ov002 0x021089ec, with subobject offset zero.
 *
 * SIZE 0x3fc is the literal passed by daDpLift_c_classInit (historically
 * PyramidLift_Spawn) to fBase_c::operator
 * new. dBgActor_c occupies 0x000..0x31f. The factory constructs the Model at
 * 0x320 and a ten-element, 0xc-stride array at 0x37c. The D1/D0 pair destroys
 * that array through _ZN7Vector3D1Ev, then destroys the Model and inherited
 * dBgActor_c members in reverse construction order. Together those independent
 * witnesses close every non-padding span below.
 *
 * THE VTABLE at ov025 0x021139d4 has the same 32 slots as dBgActor_c. Only
 * slots 0, 3, 6, 9, 16 and 17 differ, exactly the overrides declared here.
 */
#ifndef DADPLIFT_C_H
#define DADPLIFT_C_H

#include "dBgActor_c.h"

struct daDpLift_c : dBgActor_c {
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

    /* OUT OF LINE, DECLARED FIRST. src/actors/daDpLift_c.cpp defines it
       first under `#pragma defer_codegen off`, which emits D1 then D0 -- the
       cartridge's order -- then a D2 the cartridge has no home for. */
    virtual ~daDpLift_c();

    virtual s32 InitResources();       /* slot  0 */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 Behavior();            /* slot  6 */
    virtual s32 Render();              /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daDpLift_c_size_must_be_0x3fc[sizeof(daDpLift_c) == 0x3fc ? 1 : -1];
#endif

#endif /* DADPLIFT_C_H */
