#ifndef HEYHO_H
#define HEYHO_H

#include "types.h"

/* Heyho, one Shy Guy of the Jump minigames: the 0xf0-byte record the Heyho_*
 * helpers in src/minigames/d_mg3d_heyho_obj_adapter.cpp step and draw. The
 * name is coined (no RTTI names this record; dMg3DHeyhoObjAdapter_c is the
 * separate 0x4c adapter). A layout view only: the helpers are free functions
 * over a char* record and read it through their HEYHO() cast. Fields are named
 * from those helpers; the unnamed regions are written or read in one place at
 * most, or sit inside an embedded object. */
struct Heyho {
    void *mVtable;       /* 0x00 */
    Vector3 mStartVel;   /* 0x04 -- the velocity a Heyho_StartMotion copies in */
    s32 mRangeLeft;      /* 0x10 -- move limits around mGoal */
    s32 mRangeRight;     /* 0x14 */
    s32 mRangeBottom;    /* 0x18 */
    s32 mRangeTop;       /* 0x1c */
    Vector3 mGoal;       /* 0x20 -- the point the gliding motions steer to */
    s32 mMoveKind;       /* 0x2c -- the path a motion takes (switched on) */
    s32 mMotion[2];      /* 0x30 -- the current motion's member-pointer record */
    u8 mModelAnim[0x1c]; /* 0x38 -- a ModelAnim */
    u8 mMatrix[0x30];    /* 0x54 -- a Matrix4x3, set by Heyho_UpdateMatrix */
    u8 pad_84[0x4];      /* 0x84 */
    u8 mFrameCtrl[0x14]; /* 0x88 -- a dExtFrameCtrl_c */
    Vector3 mPos;        /* 0x9c */
    Vector3 mVel;        /* 0xa8 */
    u8 pad_b4[0x18];     /* 0xb4 */
    Vector3 mScale;      /* 0xcc -- 0x1400 on each axis at rest */
    u8 pad_d8[0x4];      /* 0xd8 */
    s32 mSpeed;          /* 0xdc -- LenVec3(mVel) before mVel is normalised */
    u8 pad_e0[0x6];      /* 0xe0 */
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
