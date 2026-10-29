#ifndef DMGTRMPLN2MARIO_C_H
#define DMGTRMPLN2MARIO_C_H

#include "types.h"
#include "dExtAnmModel_c.h"
#include "dMgTrmpObjAdapter_c.h"

/* dMgTrmpln2Mario_c, the Mario of Trampoline2 (ROM RTTI _ZTI17dMgTrmpln2Mario_c;
 * constructor 0x020ca78c, D1 0x020ca604; 0xdc bytes, the stride of
 * dScMgTrampoline2_c::mMarios).
 *
 * Built on dMgTrmpRingObjAdapter_c (its vtable 0x0213b2e0 ends in
 * dMgTrmpRingObjAdapter_c::Clear), which holds the jumper fields the drawn line writes
 * (TrmpLine_PushJumper in unit020cd744.cpp, whose `Cannon` view has the same
 * prefix). The constructor and the non-virtual destructor are the class's own,
 * out of line in src/minigames/d_mg_trmpln2_mario.cpp; the other helpers there
 * are still free functions over a char* record (see that file's leftovers note)
 * and read this through its MARIO2() cast. The four slot overrides are declared
 * and not defined as methods: the slot functions are those helpers. Unnamed
 * regions are read or written by one site at most, or sit inside an embedded
 * object; pad_68 is only ever stored 0 (two sites, no reader). */
struct dMgTrmpln2Mario_c : dMgTrmpRingObjAdapter_c {
    dMgTrmpln2Mario_c();
    ~dMgTrmpln2Mario_c();

    virtual Vector3 *GetPos();      /* Trmp2Mario_GetPos 0x020c8e88 */
    virtual Vector3 *GetPrevPos();  /* Trmp2Mario_GetPrevPos 0x020c8e80 */
    virtual Vector3 *GetVelocity(); /* Trmp2Mario_GetVelocity 0x020c8dd4 */
    virtual int IsActive();         /* Trmp2Mario_Reset 0x020c8ddc */

    Vector3 mPos;           /* 0x24 */
    Vector3 mPrevPos;       /* 0x30 -- mPos before this frame's move */
    Vector3 mVel;           /* 0x3c */
    s32 mGravityStep;       /* 0x48 -- ApproachLinear step of mVel.y toward the fall speed */
    s32 mBounceVy;          /* 0x4c -- mVel.y as the bounce left it */
    u8 pad_50[0x2];         /* 0x50 */
    s16 mRotY;              /* 0x52 -- leans toward mVel.x, +/-0x2000 */
    u8 pad_54[0x2];         /* 0x54 */
    s16 mScreenPos[2];      /* 0x56 -- Camera_ProjectToScreen16 output */
    u8 pad_5a[0x2];         /* 0x5a */
    s32 mParticleId;        /* 0x5c -- Particle::System unique ID */
    s32 mAnim;              /* 0x60 -- anim number; also the bank-1 voice */
    s32 mWallBumpTimer;     /* 0x64 -- counts to 5 before the next wall bump reacts */
    u8 pad_68[0x4];         /* 0x68 */
    s16 mTimer;             /* 0x6c -- state countdown */
    s16 mVisible;           /* 0x6e -- Render draws only while set */
    s32 mState[2];          /* 0x70 -- the current state's member-pointer record */
    dExtAnmModel_c mAnmModel; /* 0x78 -- 0x64 bytes: UpdateMatrix sets its mat4x3, the
                             * states drive its dExtFrameCtrl_c base and read its file */
};

#ifndef SM64DS_PLATFORM_PC
typedef char dMgTrmpln2Mario_c_size_must_be_0xdc[sizeof(dMgTrmpln2Mario_c) == 0xdc ? 1 : -1];
#endif

#endif
