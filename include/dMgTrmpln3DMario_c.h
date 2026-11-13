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
 * dMgTrmpObjAdapter_c::Clear), which holds the jumper fields the drawn line
 * writes, the same as dMgTrmpln2Mario_c's (TrmpLine_PushJumper,
 * unit020cd744.cpp). The constructor, the non-virtual destructor and the four
 * slot overrides are the class's own, out of line in
 * src/minigames/d_mg_trmpln2_mario.cpp; GetPos, the first declared, is the key
 * function, so that file emits this vtable and typeinfo. Every state handler
 * and helper that receives the record in r0 is a member below; only the
 * module-level pool walkers and the two entry points the scene calls with a
 * byte view (EnterWalkIn, IsOffstage) stay free. The handler pointers the
 * cartridge's state records hold are {code, 0} pairs, i.e. member pointers
 * with no this-adjustment. Unnamed regions are read or written by one site at
 * most, or sit inside an embedded object. */
struct dMgTrmpln3DMario_c : dMgTrmpObjAdapter_c {
    dMgTrmpln3DMario_c();
    ~dMgTrmpln3DMario_c();

    virtual Vector3 *GetPos();      /* 0x020cafac */
    virtual Vector3 *GetPrevPos();  /* 0x020cafa4 */
    virtual Vector3 *GetVelocity(); /* 0x020caf44 */
    virtual int IsActive();         /* 0x020caf4c -- anything but missed */

    /* out of line, in ROM order */
    void Render();           /* 0x020cafb4 */
    void UpdateMatrix();     /* 0x020cafdc */
    void Update();           /* 0x020cb030 -- dispatch, gravity, trail */
    void ApplyGravity();     /* 0x020cb134 */
    void StateKnockedOut();  /* 0x020cb16c */
    void EnterByKind();      /* 0x020cb1a8 */
    void StateGoal();        /* 0x020cb2b4 */
    void EnterGoal();        /* 0x020cb528 */
    void StateLand();        /* 0x020cb5c4 */
    void EnterLand();        /* 0x020cb690 */
    void StateFallToLedge(); /* 0x020cb72c */
    void EnterFallToLedge(); /* 0x020cb814 */
    void StateHardBounce();  /* 0x020cb838 */
    void BounceOffLineA();   /* 0x020cbaec */
    void StateBounce();      /* 0x020cbd7c */
    void BounceOffLineB();   /* 0x020cbfd8 */
    void JumpOff();          /* 0x020cc198 */
    void StateMissed();      /* 0x020cc2ac */
    void EnterMissed();      /* 0x020cc37c */
    void StateFall();        /* 0x020cc408 */
    void EnterFall();        /* 0x020cc618 */
    void StateWalkBack();    /* 0x020cc63c */
    void EnterWalkBack();    /* 0x020cc698 */
    void StateOnLedge();     /* 0x020cc724 */
    void EnterOnLedge();     /* 0x020cc8c8 */
    void StateWalkIn();      /* 0x020cc9b8 */
    void StateOnGround();    /* 0x020ccae0 */
    void EnterOnGround();    /* 0x020ccc8c */
    void StateOffstage();    /* 0x020ccd64 */
    void EnterOffstage();    /* 0x020ccd78 */
    int Load();              /* 0x020cce0c -- anim banks, then offstage */

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
    s32 pad_5c;             /* 0x5c -- only ever stored 0 */
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
