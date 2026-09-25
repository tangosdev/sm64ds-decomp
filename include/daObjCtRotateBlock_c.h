#ifndef DAOBJCTROTATEBLOCK_C_H
#define DAOBJCTROTATEBLOCK_C_H

#include "dBgActor_c.h"
#include "ShadowModel.h"

/* daObjCtRotateBlock_c -- the rotating cube on Tick Tock Clock.
 *
 * NAME: the cartridge spells this class daObjCtRotateBlock_c. The typeinfo
 * word of the vtable header, at ov065 0x0211d024 (address point - 4), is
 * 0x0211cfac. That __si_class_type_info record reads [0x0209a764
 * (_ZTVN3abi20__si_class_type_infoE), 0x0211cfb8, 0x021089ec], and the string
 * at 0x0211cfb8 is "20daObjCtRotateBlock_c". The tree previously called the
 * class TtcRotatingCube, a coined name present only on the vtable address.
 *
 * BASE: dBgActor_c, direct, at offset zero -- the record's third word is
 * _ZTI10dBgActor_c at ov002 0x021089ec.
 *
 * MEMBERS: the factories construct a Model at 0x320 and a ShadowModel at
 * 0x380; D1/D0 destroy them in reverse (_ZN11ShadowModelD1Ev at +0x380,
 * _ZN5ModelD1Ev at +0x320) before dBgActor_c's inlined destructor takes down
 * its dBgW_KcMbg (0x124) and Model (0xd4).
 *
 * SIZE 0x3d8: both daObjCtRotateBlock_c_classInit_CT_MECHA01 (historical
 * alias TtcRotatingCube_Spawn) and daObjCtRotateBlock_c_classInit_CT_MECHA02
 * (historical alias TtcRotatingPrism_Spawn) pass 0x3d8 to fBase_c::operator
 * new, construct this exact base/member chain, and install the same vtable.
 * Their actor IDs select mVariant during InitResources, so they are two actor
 * entries for one class rather than evidence for two C++ types.
 *
 * THE VTABLE at ov065 0x0211d028 has the same 32 slots as dBgActor_c. Only
 * slots 0, 3, 6, 9, 16 and 17 differ, exactly the overrides declared here.
 */
struct daObjCtRotateBlock_c : dBgActor_c {
    Model mRotatingModel;      /* 0x320 the part that turns */
    s32 mOffsetY;              /* 0x370 hop added to mPosY */
    u16 mWaitTimer;            /* 0x374 DecIfAbove0_Short countdown */
    u8 mState;                 /* 0x376 0 wait, 1 hop, 2 turn */
    u8 mVariant;               /* 0x377 actor 0x6c or 0x6d */
    s16 mTargetAngleZ;         /* 0x378 ApproachLinear target */
    u8 mUnevenGround;          /* 0x37a the two floor probes disagreed */
    u8 mPad37b;                /* 0x37b */
    s32 mFloorY;               /* 0x37c ground height under the cube */
    ShadowModel mShadowModel;  /* 0x380 */
    Matrix4x3 mShadowMat;      /* 0x3a8 */

    /* OUT OF LINE, DECLARED FIRST: the key function. The TU defines it first
       under `#pragma defer_codegen off`, which emits D1 then D0 -- the
       cartridge's order -- then a D2 the cartridge has no home for. */
    virtual ~daObjCtRotateBlock_c();      /* slots 16, 17 */

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

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjCtRotateBlock_c_size_must_be_0x3d8[
    sizeof(daObjCtRotateBlock_c) == 0x3d8 ? 1 : -1];
#endif

#endif
