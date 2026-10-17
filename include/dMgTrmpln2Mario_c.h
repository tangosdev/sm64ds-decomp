#ifndef DMGTRMPLN2MARIO_C_H
#define DMGTRMPLN2MARIO_C_H

#include "types.h"

/* dMgTrmpln2Mario_c, the Mario of Trampoline2 (ROM RTTI _ZTI17dMgTrmpln2Mario_c;
 * constructor 0x020ca78c, D1 0x020ca604; 0xdc bytes, the stride of the array at
 * sTrmp2Marios).
 *
 * A layout view only: src/minigames/d_mg_trmpln2_mario.cpp is still free
 * functions over a char* record (see that file's leftovers note), and reads this
 * through its MARIO2() cast. The fields are named from those functions; the
 * first five are the jumper fields the drawn line writes (TrmpLine_PushJumper in
 * unit020cd744.cpp, whose `Cannon` view has the same prefix). Unnamed regions are
 * read or written by one site at most, or sit inside an embedded object. */
struct dMgTrmpln2Mario_c {
    void *mVtable;          /* 0x00 */
    Vector3 mContactNormal; /* 0x04 -- the drawn line's normal */
    s32 mContactOffset;     /* 0x10 -- signed distance from the line's centre */
    s32 mContactHalfWidth;  /* 0x14 -- the line's half width */
    u16 mContact;           /* 0x18 -- 0 none, 1 hit, 2 hard hit, 3 the line's end */
    u8 pad_1a[0xa];         /* 0x1a */
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
    u8 mModelAnim[0x1c];    /* 0x78 -- a ModelAnim */
    u8 mMatrix[0x30];       /* 0x94 -- a Matrix4x3, set by UpdateMatrix */
    u8 pad_c4[0x4];         /* 0xc4 */
    u8 mFrameCtrl[0x14];    /* 0xc8 -- a dExtFrameCtrl_c */
};

#ifndef SM64DS_PLATFORM_PC
typedef char dMgTrmpln2Mario_c_size_must_be_0xdc[sizeof(dMgTrmpln2Mario_c) == 0xdc ? 1 : -1];
#endif

#endif
