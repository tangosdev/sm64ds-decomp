#ifndef HEYHO_H
#define HEYHO_H

#include "types.h"
#include "dExtAnmModel_c.h"

/* Heyho, one Shy Guy of the Jump minigames: the 0xf0-byte record whose members
 * in src/minigames/d_mg3d_heyho_obj_adapter.cpp step and draw it. The
 * name is coined (no RTTI names this record; dMg3DHeyhoObjAdapter_c is the
 * separate 0x4c adapter). The constructor and destructor are the class's own
 * (src/minigames/d_mg3d_heyho_obj_adapter.cpp), so dScMgJump_c and
 * dScMgJump2_c hold typed Heyho arrays and the compiler writes their
 * __cxa_vec_ctor / __cxa_vec_cleanup calls. mMotion holds the current motion
 * as a pointer to member, which Update calls each frame. The Heyho_* pool
 * functions step every record. Fields are named from the members; the unnamed
 * regions are written or read in one place at most, or sit inside an embedded
 * object. */
/* A Heyho's move limits, as offsets from HeyhoPath::goal (VShape and Diagonal
 * read them as absolute corners). */
struct HeyhoRange {
    s32 left;   /* 0x0 */
    s32 right;  /* 0x4 */
    s32 bottom; /* 0x8 */
    s32 top;    /* 0xc */
};

/* One entry of the Heyho path tables (sHeyhoLayoutFlyInPaths, sHeyhoFlyInPaths,
 * sHeyhoSettlePaths): where a Heyho flies in to and how it moves once there.
 * StartMotionFlyIn and StartMotionRegroup copy one whole into Heyho::mPath. */
struct HeyhoPath {
    s32 baseSize;     /* 0x00 -- 0x1000, 0x1998 in sHeyhoLayoutFlyInPaths; seeds Heyho::mSize */
    VecFx32 startVel; /* 0x04 -- the velocity the motion starts with */
    HeyhoRange range; /* 0x10 -- move limits around goal */
    VecFx32 goal;     /* 0x20 -- the point the gliding motions steer to */
    s32 moveKind;     /* 0x2c -- the motion taken once there (switched on) */
};

struct Heyho {
    Heyho();
    ~Heyho();

    int LoadResources();
    void StartMotionFlyIn(const HeyhoPath *);
    void MotionFlyIn();
    void StartMotionRegroup(const HeyhoPath *);
    void MotionRegroup();
    void React();
    void MotionLeave();
    void StartMotionHover();
    void MotionHover();
    void StartMotionPatrolBob();
    void MotionPatrolBob();
    void StartMotionBobVertical();
    void MotionBobVertical();
    void StartMotionBounceNear();
    void MotionBounceNear();
    void StartMotionVShape();
    void MotionVShape();
    void StartMotionBounceInRange();
    void MotionBounceInRange();
    void StartMotionDiagonal();
    void MotionDiagonal();
    void StartMotionDiagonalMirror();
    void MotionDiagonalMirror();
    void StartMotionCircleUpper();
    void MotionCircleUpper();
    void StartMotionCircleLower();
    void MotionCircleLower();
    void StartMotionChase();
    void MotionChase();
    void StartMotionHitSpin();
    void MotionHitSpin();
    void Settle();
    void SetIdle();
    void StateIdle();
    void Update();
    void Draw();
    void UpdateMatrix();
    void CheckHitB();
    void CheckHitA();
    int IsChasing();

    HeyhoPath mPath; /* 0x00 -- the path record StartMotionFlyIn/StartMotionRegroup copy in */
    void (Heyho::*mMotion)(); /* 0x30 -- the current motion; Update calls it */
    dExtAnmModel_c mAnmModel; /* 0x38 -- 0x64 bytes: UpdateMatrix sets its mat4x3,
                           * Update advances its dExtFrameCtrl_c base */
    Vector3 mPos;        /* 0x9c */
    Vector3 mVel;        /* 0xa8 */
    Vector3 mWaypoint;   /* 0xb4 -- the point VShape/Diagonal/Chase step mPos to and Leave flies to;
                          * picked by the VShape/Diagonal/Chase StartMotion* and Motion* functions
                          * and by React */
    u8 pad_c0[0xc];      /* 0xc0 */
    Vector3 mScale;      /* 0xcc -- 0x1400 on each axis at rest */
    s32 mSize;           /* 0xd8 -- copied from mPath.baseSize; scales the drawn model (Draw), the
                          * hit radius (CheckHitA/B) and the settle particles (Settle) */
    s32 mSpeed;          /* 0xdc -- |mVel| before mVel is normalised */
    s32 mSeparated;      /* 0xe0 -- 1 once Heyho_SeparateAll pushed this Heyho off a neighbour this
                          * frame; MotionChase then pulls mWaypoint back toward mPos */
    u8 pad_e4[0x2];      /* 0xe4 */
    s16 mLean;           /* 0xe6 -- leans toward the direction of travel, +/-0x3000 */
    u8 pad_e8[0x2];      /* 0xe8 */
    s16 mPhase;          /* 0xea -- motion phase, stepped by 0x200 */
    s16 mDelay;          /* 0xec -- counts down to 0 */
    u8 pad_ee[0x2];      /* 0xee */
};

#ifndef SM64DS_PLATFORM_PC
typedef char HeyhoPath_size_must_be_0x30[sizeof(HeyhoPath) == 0x30 ? 1 : -1];
typedef char Heyho_size_must_be_0xf0[sizeof(Heyho) == 0xf0 ? 1 : -1];
#endif

#endif
