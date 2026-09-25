#ifndef DAOBJCTMECHA11_C_H
#define DAOBJCTMECHA11_C_H

#include "dBgActor_c.h"
#include "ShadowModel.h"

/* daObjCtMecha11_c -- Tick Tock Clock's spinning platform (profile
 * CT_MECHA11). It rolls on X at a step taken from the clock-setting table;
 * setting 2 picks a sign and holds still for the first five frames of each
 * burst. A cuboid shadow sits on the floor under it and scales with the tilt.
 *
 * NAME: the cartridge spells this class daObjCtMecha11_c. The typeinfo word
 * of the vtable header, at ov035 0x02112bc8 (address point - 4), is
 * 0x02112b88. That __si_class_type_info record reads [0x0209a764
 * (_ZTVN3abi20__si_class_type_infoE), 0x02112b94, 0x021089ec], and the string
 * at 0x02112b94 is "16daObjCtMecha11_c". The tree previously called the class
 * SpinningPlatform, a coined name present only on the vtable address.
 *
 * BASE: dBgActor_c, direct, at offset zero -- the record's third word is
 * _ZTI10dBgActor_c at ov002 0x021089ec.
 *
 * MEMBERS: the ShadowModel at 0x328 is named twice -- the factory calls
 * _ZN11ShadowModelC1Ev at +0x328, and D1/D0 call _ZN11ShadowModelD1Ev there
 * before dBgActor_c's inlined destructor takes down its dBgW_KcMbg (0x124)
 * and Model (0xd4). dBgActor_c's non-POD data ends at 0x31e, so the Itanium
 * layout reuses its two bytes of tail padding for the first derived member.
 * Field names are descriptive; offsets and widths are the ROM-evidenced claims.
 *
 * SIZE 0x380 is the literal daObjCtMecha11_c_classInit (historically
 * SpinningPlatform_Spawn) passes to fBase_c::operator new; the shadow-update
 * body's mShadowMat closes the object exactly there.
 *
 * THE VTABLE at ov035 0x02112bcc has the same 32 slots as dBgActor_c. Only
 * slots 0, 3, 6, 9, 16 and 17 differ, exactly the overrides declared here.
 */
struct daObjCtMecha11_c : dBgActor_c {
    s8  mRandDirection;       /* 0x31e +1 or -1 while the clock setting is random */
    u8  pad_31f;              /* 0x31f */
    u16 mRandTimer;           /* 0x320 frames left in this burst */
    u16 mRandFrames;          /* 0x322 the burst length just rolled */
    s32 mFloorPosY;           /* 0x324 floor the spawn probe hit, else the probe Y */
    ShadowModel mShadowModel; /* 0x328 */
    Matrix4x3 mShadowMat;     /* 0x350 shadow on the floor, yaw only */

    /* OUT OF LINE, DECLARED FIRST: the key function. The TU defines it first
       under `#pragma defer_codegen off`, which emits D1 then D0 -- the
       cartridge's order -- then a D2 the cartridge has no home for. */
    virtual ~daObjCtMecha11_c();    /* slots 16, 17 */

    virtual int InitResources();    /* slot  0 */
    virtual int CleanupResources(); /* slot  3 */
    virtual int Behavior();         /* slot  6 */
    virtual int Render();           /* slot  9 */

    int UpdateShadow();
    void UpdateModel();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjCtMecha11_c_size_must_be_0x380[
    sizeof(daObjCtMecha11_c) == 0x380 ? 1 : -1];
#endif

#endif
