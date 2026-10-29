#ifndef HEYHO_H
#define HEYHO_H

#include "types.h"
#include "dExtAnmModel_c.h"

/* Heyho, one Shy Guy of the Jump minigames: the 0xf0-byte record the Heyho_*
 * helpers in src/minigames/d_mg3d_heyho_obj_adapter.cpp step and draw. The
 * name is coined (no RTTI names this record; dMg3DHeyhoObjAdapter_c is the
 * separate 0x4c adapter). The constructor and destructor are the class's own
 * (src/minigames/d_mg3d_heyho_obj_adapter.cpp), so dScMgJump_c and
 * dScMgJump2_c hold typed Heyho arrays and the compiler writes their
 * __cxa_vec_ctor / __cxa_vec_cleanup calls. The other helpers are free
 * functions over a char* record and read it through their HEYHO() cast.
 * Fields are named from those helpers; the unnamed regions are written or
 * read in one place at most, or sit inside an embedded object. */
struct Heyho {
    Heyho();
    ~Heyho();

    s32 mBaseSize;       /* 0x00 -- word 0 of the path record a Heyho_StartMotion copies in: 0x1000,
                          * 0x1998 in sHeyhoLayoutFlyInPaths; read only to seed mSize (was mVtable:
                          * no vtable is stored or called through here) */
    Vector3 mStartVel;   /* 0x04 -- the velocity a Heyho_StartMotion copies in */
    s32 mRangeLeft;      /* 0x10 -- move limits around mGoal */
    s32 mRangeRight;     /* 0x14 */
    s32 mRangeBottom;    /* 0x18 */
    s32 mRangeTop;       /* 0x1c */
    Vector3 mGoal;       /* 0x20 -- the point the gliding motions steer to */
    s32 mMoveKind;       /* 0x2c -- the path a motion takes (switched on) */
    s32 mMotion[2];      /* 0x30 -- the current motion's member-pointer record */
    dExtAnmModel_c mAnmModel; /* 0x38 -- 0x64 bytes: Heyho_UpdateMatrix sets its mat4x3,
                           * Heyho_Update advances its dExtFrameCtrl_c base */
    Vector3 mPos;        /* 0x9c */
    Vector3 mVel;        /* 0xa8 */
    Vector3 mWaypoint;   /* 0xb4 -- the point VShape/Diagonal/Chase step mPos to and Leave flies to;
                          * picked by the VShape/Diagonal/Chase StartMotion* and Motion* functions
                          * and by Heyho_React */
    u8 pad_c0[0xc];      /* 0xc0 */
    Vector3 mScale;      /* 0xcc -- 0x1400 on each axis at rest */
    s32 mSize;           /* 0xd8 -- copied from mBaseSize; scales the drawn model (Heyho_Draw), the
                          * hit radius (Heyho_CheckHitA/B) and the settle particles (Heyho_Settle) */
    s32 mSpeed;          /* 0xdc -- LenVec3(mVel) before mVel is normalised */
    s32 mSeparated;      /* 0xe0 -- 1 once Heyho_SeparateAll pushed this Heyho off a neighbour this
                          * frame; Heyho_MotionChase then pulls mWaypoint back toward mPos */
    u8 pad_e4[0x2];      /* 0xe4 */
    s16 mLean;           /* 0xe6 -- leans toward the direction of travel, +/-0x3000 */
    u8 pad_e8[0x2];      /* 0xe8 */
    s16 mPhase;          /* 0xea -- motion phase, stepped by 0x200 */
    s16 mDelay;          /* 0xec -- counts down to 0 */
    u8 pad_ee[0x2];      /* 0xee */
};

#ifndef SM64DS_PLATFORM_PC
typedef char Heyho_size_must_be_0xf0[sizeof(Heyho) == 0xf0 ? 1 : -1];
#endif

#endif
