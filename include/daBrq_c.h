#ifndef DABRQ_C_H
#define DABRQ_C_H

#include "Model.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"
#include "dActor_c.h"

struct daBrq_c;

typedef s32 (daBrq_c::*BrqStateHandler)();

struct BrqStateHandlers {
    BrqStateHandler enter;
    BrqStateHandler update;
};

typedef char BrqStateHandlers_size_must_be_0x10[
    sizeof(BrqStateHandlers) == 0x10 ? 1 : -1];

/* daBrq_c is the ROM-proven class identity: ov070 owns _ZTS7daBrq_c at
 * 0x0212323c, _ZTI7daBrq_c at 0x02123248, and the public vtable address point
 * at 0x02123278. daBrq_c_classInit (historical alias Amp_Spawn) allocates
 * 0x434 bytes, constructs dActor_c, then
 * constructs the seven owned subobjects below at 0xd4..0x218. D1 destroys
 * those subobjects in reverse order before chaining to dActor_c. Behavior
 * identifies the Vector3 at 0x410 as the moving cylinder's actor-relative
 * offset.
 */
struct daBrq_c : dActor_c {
    u8                        pad_0d0[0x4];
    ModelAnim                 mModelAnim;                    /* 0x0d4 */
    Model                     mModel;                        /* 0x138 */
    TextureSequence           mTextureSequence;              /* 0x188 */
    TextureTransformer        mTextureTransformer;           /* 0x19c */
    ShadowModel               mShadowModel;                  /* 0x1b0 */
    dCcAcPos_c                mdCcAcPos_c;                  /* 0x1d8 */
    dBgCh_Actr                mWithMeshClsn;                /* 0x218 */
    /* Matrix4x3, on three witnesses: InitResources block-copies the identity
       matrix IDENTITY_MATRIX4X3 (0x30 bytes) into it; the state handlers write the
       position (>>3) at +0x24/+0x28/+0x2c -- exactly the translation row; and
       func_ov070_021206b0-family passes &this->0x3d4 as the matrix argument of
       the shadow call. Same role as Lakitu's 0x3f8 translation words. */
    Matrix4x3                 mMat4x3;                       /* 0x3d4 */
    Vector3                   mOrbitCenter;                  /* 0x404 */
    Vector3                   mCylinderOffset;               /* 0x410 */
    BrqStateHandlers         *mStateHandlers;                /* 0x41c */
    s32                       mState;                        /* 0x420 */
    s32                       mTurnSpeed;                    /* 0x424 */
    u32                       mSoundHandle;                  /* 0x428 */
    s16                       mOrbitAngle;                   /* 0x42c */
    s16                       mSpinAngle;                    /* 0x42e */
    u8                        mStateTimer;                   /* 0x430 */
    u8                        pad_431[0x3];

    virtual ~daBrq_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

private:
    /* These state-machine spellings are inferred aliases. Class ownership,
     * bodies, call graph, and the two-PMF table layout are proven; SetState's
     * signed parameter spelling is not distinguishable from other 32-bit forms. */
    void EnterState();
    void UpdateState();
    void SetState(s32 state);
    s32 EnterCooldownState();
    s32 UpdateCooldownState();
    s32 EnterActiveState();
    s32 UpdateActiveState();
    s32 EnterDefeatedState();
    s32 UpdateDefeatedState();
    s32 HandlePlayerCollision();
    void UpdateModelTransform();
};

typedef char daBrq_c_size_must_be_0x434[
    sizeof(daBrq_c) == 0x434 ? 1 : -1];

#endif /* DABRQ_C_H */
