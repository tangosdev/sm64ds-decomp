/* daPiano_c's constructor and destructor sequences independently establish a
 * dBgActor_c base followed by the typed members named below. */
#ifndef DAPIANO_C_H
#define DAPIANO_C_H
#include "types.h"
#include "dBgActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

/* dBgActor_c owns the Model at 0xd4, dBgW_KcMbg at 0x124, and collision
 * matrix through 0x31d. The class's own first member therefore starts at
 * 0x320, exactly where the factory constructs it. */
struct daPiano_c : dBgActor_c {
    /* Named by _ZN9ModelAnimD1Ev at +0x320, a relocation the ROM build checks. */
    ModelAnim mModelAnim;            /* 0x320 */
    /* Named by _ZN11ShadowModelD1Ev at +0x384 (D0/D1), a ROM-checked relocation. */
    ShadowModel mShadowModel1;            /* 0x384 */
    /* Named by _ZN11ShadowModelD1Ev at +0x3ac (D0/D1), a ROM-checked relocation. */
    ShadowModel mShadowModel2;            /* 0x3ac */
    /* Named by _ZN11ShadowModelD1Ev at +0x3d4 (D0/D1), a ROM-checked relocation. */
    ShadowModel mShadowModel3;            /* 0x3d4 */
    /* func_ov063_0211d5f4 builds one rotation+position matrix per shadow (from
     * mPos and mAngleY) and hands them to DropShadowRadHeight/ScaleXYZ
     * alongside mShadowModel1-3. */
    Matrix4x3 mShadowMats[3];            /* 0x3fc */
    /* The factory's array-construction helper and both destructor variants
       agree on two 0x40-byte dCcAcPos_c objects starting at 0x48c. */
    dCcAcPos_c mCylinderClsn[2];            /* 0x48c */
    /* Named by _ZN10dBgCh_ActrD1Ev at +0x50c (D0/D1), a ROM-checked relocation. */
    dBgCh_Actr mWithMeshClsn;            /* 0x50c */
    /* PMF dispatch index into data_ov063_0211efbc: func_ov063_0211ddac writes
     * it on transitions, func_ov063_0211ddf4 reads it every frame. */
    s32 mStateIdx;            /* 0x6c8 */
    s32 mMinPosY;            /* 0x6cc */
    /* Chase target: func_ov063_0211dbb8 stores the nearest moving player slot
     * from data_0209f394 here; func_ov063_0211d8cc steers toward it. */
    dActor_c *mTarget;            /* 0x6d0 */
    Vector3 mHomePos;            /* 0x6d4 */
    /* Attack windup: func_ov063_0211dba4 sets 30 on entering state 1,
     * func_ov063_0211d8cc counts it down and bites at zero. */
    u8 mAttackTimer;            /* 0x6e0 */
    u8 pad_6e1[3];
    /* Inline and empty: mwccarm emits D1 then D0 -- the cartridge's order at
     * 0x0211d4b8 and 0x0211d54c -- and no homeless D2. Slots 16 (D1) and 17 (D0). */
    virtual ~daPiano_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daPiano_c_size_must_be_0x6e4[sizeof(struct daPiano_c) == 0x6e4 ? 1 : -1];
#endif

#endif
