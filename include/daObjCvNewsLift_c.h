/* daObjCvNewsLift_c is a dBgActor_c with four visible platform models and four
 * moving-mesh colliders. Its constructor and destructor provide both halves
 * of that ownership evidence: the factory builds the arrays in declaration
 * order, while D1/D0 destroy them in reverse order before the inherited
 * dBgActor_c members.
 *
 * Field names below 0xc40 follow the arrays' types. The names from 0xc40 on
 * are inferred from how src/actors/daObjCvNewsLift_c.cpp uses each field;
 * the ROM keeps none of them. */
#ifndef DAOBJCVNEWSLIFT_C_H
#define DAOBJCVNEWSLIFT_C_H
#include "types.h"
#include "dBgActor_c.h"

struct daObjCvNewsLift_c : dBgActor_c {
    Model mPlatformModels[4];            /* 0x320 */
    Matrix4x3 mPlatformMats[4];          /* 0x460 - the platform colliders' matrices */
    dBgW_KcMbg mPlatformColliders[4];    /* 0x520 */
    Vector3 mHomePos;                    /* 0xc40 - spawn position, restored on reset */
    s32 mRotation[4];                    /* 0xc4c - current tilt quaternion */
    s32 mTargetRotation[4];              /* 0xc5c - slerped toward each frame */
    s32 mGroundY;                        /* 0xc6c - floor height below the spawn point */
    u32 mMotorSound;                     /* 0xc70 - Sound::PlayLong handle */
    u16 mResetTimer;                     /* 0xc74 - 300 while ridden; the lift resets at 0 */
    u16 mWobblePhase;                    /* 0xc76 - sine-table phase of the bump wobble */
    u16 mWobbleTimer;                    /* 0xc78 - frames left in the bump wobble */
    s8 mLoweredPlatform;                 /* 0xc7a - platform the player stands on, or -1 */
    s8 mPrevLoweredPlatform;             /* 0xc7b */
    u8 mTiltHoldTimer;                   /* 0xc7c - frames before the tilt target clears */
    u8 mBumped;                          /* 0xc7d - the lift ran into a wall */
    u8 mPlayerOnMesh;                    /* 0xc7e - set by the main mesh's callback */
    u8 pad_c7f;                          /* 0xc7f */

    /* --- vtable overrides --- */
    /* Inline on purpose: the out-of-line form emits D0 ahead of D1, the
       reverse of the cartridge, and a homeless D2. InitResources is then the
       key function, so the vtable is emitted with the class's TU. */
    virtual ~daObjCvNewsLift_c() {}
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();

    /* --- non-virtual. The ROM keeps none of these names; all are coined. --- */
    void UpdateClsnTransforms();
    void UpdateModelTransforms();
    void OnPlatform3Ride(dActor_c *player);
    void OnPlatform2Ride(dActor_c *player);
    void OnPlatform1Ride(dActor_c *player);
    void OnPlatform0Ride(dActor_c *player);
    void OnMainMeshRide(dActor_c *player);

    /* Collision callbacks, stored in each collider by func_020393c4. */
    static void Platform3Callback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other);
    static void Platform2Callback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other);
    static void Platform1Callback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other);
    static void Platform0Callback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other);
    static void MainMeshCallback(dBgW *clsn, daObjCvNewsLift_c *self, dActor_c *other);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjCvNewsLift_c_size_must_be_0xc80[sizeof(struct daObjCvNewsLift_c) == 0xc80 ? 1 : -1];
#endif

#endif
