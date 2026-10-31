#ifndef DMGTRMPLN3DMARIO_C_H
#define DMGTRMPLN3DMARIO_C_H

#include "types.h"
#include "dExtAnmModel_c.h"
#include "dMgTrmpObjAdapter_c.h"

/* dMgTrmpln3DMario_c, the Mario of Trampoline (ROM RTTI _ZTI18dMgTrmpln3DMario_c;
 * constructor 0x020cd12c, D1 0x020ccfc8; 0xd0 bytes, the stride the trampoline
 * scene's __cxa_vec_ctor builds four of).
 *
 * Built on dMgTrmpObjAdapter_c directly (its vtable 0x0213b2c4 ends in
 * dMgTrmpObjAdapter_c::Clear), which holds the jumper fields the drawn line writes,
 * the same as dMgTrmpln2Mario_c's (TrmpLine_PushJumper, unit020cd744.cpp). The
 * constructor, the non-virtual destructor and the four slot overrides are the
 * class's own, out of line in d_mg_trmpln2_mario.cpp; GetPos, the first declared,
 * is the key function, so that file emits this vtable and typeinfo. The other
 * helpers there are still free functions over a char* record, read through its
 * MARIO3D() cast. */
struct dMgTrmpln3DMario_c : dMgTrmpObjAdapter_c {
    dMgTrmpln3DMario_c();
    ~dMgTrmpln3DMario_c();

    virtual Vector3 *GetPos();      /* 0x020cafac */
    virtual Vector3 *GetPrevPos();  /* 0x020cafa4 */
    virtual Vector3 *GetVelocity(); /* 0x020caf44 */
    virtual int IsActive();         /* 0x020caf4c -- anything but missed */

    Vector3 mPos;           /* 0x1c */
    Vector3 mPrevPos;       /* 0x28 -- mPos before this frame's move */
    Vector3 mVel;           /* 0x34 */
    s32 mGravityStep;       /* 0x40 -- ApproachLinear step of mVel.y toward the fall speed */
    s32 mBounceVy;          /* 0x44 -- mVel.y as the bounce left it */
    u8 pad_48[0x2];         /* 0x48 */
    s16 mRotY;              /* 0x4a */
    u8 pad_4c[0x2];         /* 0x4c */
    s16 mScreenPos[2];      /* 0x4e -- Camera_ProjectToScreen16 output */
    s16 mHeightRow;         /* 0x52 -- row of the height table sTrmpRowHeights */
    s32 mParticleId;        /* 0x54 -- Particle::System unique ID */
    s32 mAnim;              /* 0x58 -- anim number; also the bank-1 voice */
    u8 pad_5c[0x4];         /* 0x5c */
    s16 mTimer;             /* 0x60 -- state countdown */
    u8 pad_62[0x2];         /* 0x62 */
    s32 mState[2];          /* 0x64 -- the current state's member-pointer record */
    dExtAnmModel_c mAnmModel; /* 0x6c -- 0x64 bytes: UpdateMatrix sets its mat4x3, the
                             * states drive its dExtFrameCtrl_c base and read its file */
};

#ifndef SM64DS_PLATFORM_PC
typedef char dMgTrmpln3DMario_c_size_must_be_0xd0[sizeof(dMgTrmpln3DMario_c) == 0xd0 ? 1 : -1];
#endif

#endif
