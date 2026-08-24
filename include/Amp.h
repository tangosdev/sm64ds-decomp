#ifndef AMP_H
#define AMP_H

#include "Model.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"
#include "dActor_c.h"

/* Amp_Spawn allocates 0x434 bytes, constructs dActor_c, then constructs the
 * seven owned subobjects below at 0xd4..0x218. D1 destroys those subobjects in
 * reverse order before chaining to dActor_c. Behavior identifies the Vector3
 * at 0x410 as the moving cylinder's actor-relative offset.
 */
struct Amp : dActor_c {
    u8                        pad_0d0[0x4];
    ModelAnim                 mModelAnim;                    /* 0x0d4 */
    Model                     mModel;                        /* 0x138 */
    TextureSequence           mTextureSequence;              /* 0x188 */
    TextureTransformer        mTextureTransformer;           /* 0x19c */
    ShadowModel               mShadowModel;                  /* 0x1b0 */
    dCcAcPos_c mdCcAcPos_c;    /* 0x1d8 */
    dBgCh_Actr              mWithMeshClsn;                 /* 0x218 */
    /* Matrix4x3, on three witnesses: InitResources block-copies the identity
       matrix IDENTITY_MATRIX4X3 (0x30 bytes) into it; the state handlers write the
       position (>>3) at +0x24/+0x28/+0x2c -- exactly the translation row; and
       func_ov070_021206b0-family passes &this->0x3d4 as the matrix argument of
       the shadow call. Same role as Lakitu's 0x3f8 translation words. */
    Matrix4x3                 mMat4x3;                       /* 0x3d4 */
    u8                        pad_404[0xc];
    Vector3                   mCylinderOffset;               /* 0x410 */
    u8                        pad_41c[0x4];
    s32                       mState;                        /* 0x420 */
    u8                        pad_424[0x10];

    virtual ~Amp();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

typedef char Amp_size_must_be_0x434[
    sizeof(Amp) == 0x434 ? 1 : -1];

#endif /* AMP_H */
