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
 * dMgTrmpRingObjAdapter_c::Clear), which holds the jumper fields the drawn line
 * writes (TrmpLine_PushJumper in unit020cd744.cpp, whose `Cannon` view has the
 * same prefix). The constructor, the non-virtual destructor and the four slot
 * overrides are the class's own, out of line in
 * src/minigames/d_mg_trmpln2_mario.cpp; GetPos, the first declared, is the key
 * function, so that file emits this vtable and typeinfo. Every state handler
 * and helper that receives the record in r0 is a member below; only the
 * module-level pool walkers stay free. The handler pointers the cartridge's
 * state records hold are {code, 0} pairs, i.e. member pointers with no
 * this-adjustment. Unnamed regions are read or written by one site at most, or
 * sit inside an embedded object; pad_68 is only ever stored 0 (two sites, no
 * reader). */
struct dMgTrmpln2Mario_c : dMgTrmpRingObjAdapter_c {
    dMgTrmpln2Mario_c();
    ~dMgTrmpln2Mario_c();

    virtual Vector3 *GetPos();      /* 0x020c8e88 */
    virtual Vector3 *GetPrevPos();  /* 0x020c8e80 */
    virtual Vector3 *GetVelocity(); /* 0x020c8dd4 */
    virtual int IsActive();         /* 0x020c8ddc -- neither missed nor hidden */

    /* out of line, in ROM order */
    void Render();                     /* 0x020c8e90 */
    void UpdateMatrix();               /* 0x020c8ecc */
    void Update();                     /* 0x020c8f20 -- dispatch, gravity, trail */
    void ApplyGravity();               /* 0x020c9024 */
    void StateKnockedOut();            /* 0x020c905c */
    void EnterByKind();                /* 0x020c9098 */
    void StateBounce();                /* 0x020c91ac */
    void BounceOffLineA();             /* 0x020c94e0 */
    void StateWallBounce();            /* 0x020c97bc */
    void BounceOffLineB();             /* 0x020c9aa0 */
    void EnterBounce();                /* 0x020c9c8c */
    void StateRespawnWait();           /* 0x020c9d7c */
    void EnterRespawnWait();           /* 0x020c9e7c */
    void StateMissed();                /* 0x020c9efc */
    void EnterMissed();                /* 0x020c9fe4 */
    void StateFall();                  /* 0x020ca070 */
    void EnterFall();                  /* 0x020ca2ec */
    void StateAppearWait();            /* 0x020ca310 */
    void EnterAppearWait(short timer); /* 0x020ca374 */
    void StateHidden();                /* 0x020ca39c */
    void EnterHidden();                /* 0x020ca3a8 */
    int Load();                        /* 0x020ca430 -- anim banks, then hidden */

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
    s32 pad_68;             /* 0x68 -- only ever stored 0 */
    s16 mTimer;             /* 0x6c -- state countdown */
    s16 mVisible;           /* 0x6e -- Render draws only while set */
    s32 mState[2];          /* 0x70 -- the current state's {code, 0} record */
    dExtAnmModel_c mAnmModel; /* 0x78 -- 0x64 bytes: UpdateMatrix sets its mat4x3, the
                             * states drive its dExtFrameCtrl_c base and read its file */
};

#ifndef SM64DS_PLATFORM_PC
typedef char dMgTrmpln2Mario_c_size_must_be_0xdc[sizeof(dMgTrmpln2Mario_c) == 0xdc ? 1 : -1];
#endif

#endif
