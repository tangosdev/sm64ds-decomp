/* daPiano_c's constructor and destructor sequences independently establish a
 * dBgActor_c base followed by the typed members named below. */
#ifndef DAPIANO_C_H
#define DAPIANO_C_H
#include "types.h"
#include "dBgActor_c.h"
#include "dExtAnmModel_c.h"
#include "dExtShadowModel_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

/* dBgActor_c owns the dExtSimpleModel_c at 0xd4, dBgW_KcMbg at 0x124, and collision
 * matrix through 0x31d. The class's own first member therefore starts at
 * 0x320, exactly where the factory constructs it. */
struct daPiano_c : dBgActor_c {
    /* Named by _ZN14dExtAnmModel_cD1Ev at +0x320, a relocation the ROM build checks. */
    dExtAnmModel_c mAnmModel; /* 0x320 */
    /* Named by _ZN17dExtShadowModel_cD1Ev at +0x384 (D0/D1), a ROM-checked relocation. */
    dExtShadowModel_c mShadowModel1; /* 0x384 */
    /* Named by _ZN17dExtShadowModel_cD1Ev at +0x3ac (D0/D1), a ROM-checked relocation. */
    dExtShadowModel_c mShadowModel2; /* 0x3ac */
    /* Named by _ZN17dExtShadowModel_cD1Ev at +0x3d4 (D0/D1), a ROM-checked relocation. */
    dExtShadowModel_c mShadowModel3; /* 0x3d4 */
    /* UpdateShadows builds one rotation+position matrix per shadow (from
     * mPos and mAngleY) and hands them to DropShadowRadHeight/ScaleXYZ
     * alongside mShadowModel1-3. */
    Matrix4x3 mShadowMats[3]; /* 0x3fc */
    /* The factory's array-construction helper and both destructor variants
       agree on two 0x40-byte dCcAcPos_c objects starting at 0x48c. */
    dCcAcPos_c mCylinderClsn[2]; /* 0x48c */
    /* Named by _ZN10dBgCh_ActrD1Ev at +0x50c (D0/D1), a ROM-checked relocation. */
    dBgCh_Actr mWithMeshClsn; /* 0x50c */
    /* PMF dispatch index into data_ov063_0211efbc: SetState writes it on
     * transitions, CallStateBehavior reads it every frame. */
    s32 mStateIdx; /* 0x6c8 */
    s32 mMinPosY;  /* 0x6cc */
    /* Chase target: StateSearch stores the first qualifying moving player slot
     * from data_0209f394 here; StateAttack steers toward it. */
    dActor_c *mTarget; /* 0x6d0 */
    Vector3 mHomePos;  /* 0x6d4 */
    /* Attack windup: InitStateAttack sets 30 on entering state 1,
     * StateAttack counts it down and bites at zero. */
    u8 mAttackTimer; /* 0x6e0 */
    u8 pad_6e1[3];
    /* Inline and empty: mwccarm emits D1 then D0 -- the cartridge's order at
     * 0x0211d4b8 and 0x0211d54c -- and no homeless D2. Slots 16 (D1) and 17 (D0). */
    virtual ~daPiano_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    /* --- non-virtual --- */
    /* The two-state PMF table stores the four state routines as member
       pointers. */
    void StateAttack();             /* ov063 0x0211d8cc */
    void InitStateAttack();         /* ov063 0x0211dba4 */
    void StateSearch();             /* ov063 0x0211dbb8 */
    void InitStateSearch();         /* ov063 0x0211dd78 */
    int IsTargetMoving(dActor_c *); /* ov063 0x0211dd84 */
    void SetState(int);             /* ov063 0x0211ddac */
    void CallStateBehavior();       /* ov063 0x0211ddf4 */
    void UpdateModelMatrix();       /* ov063 0x0211d88c */
    void UpdateClsnMatrix();        /* ov063 0x0211d828 */
    void UpdateShadows();           /* ov063 0x0211d5f4 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daPiano_c_size_must_be_0x6e4[sizeof(struct daPiano_c) == 0x6e4 ? 1 : -1];
#endif

#endif
