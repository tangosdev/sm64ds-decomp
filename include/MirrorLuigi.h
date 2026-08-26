/* Mirror Luigi's camera-space reflection actor.
 *
 * The cartridge's RTTI record names this class `daLuigi_c` and gives it one
 * zero-offset dActor_c base. The repository's configured function symbols use
 * the readable compatibility spelling `MirrorLuigi`; compiler-emitted
 * `_ZTI11MirrorLuigi` / `_ZTS11MirrorLuigi` are therefore isolation-only
 * passengers, not names for the ROM metadata at ov055 0x02111aa4..0x02111abc.
 *
 * The 0x20c allocation literal in MirrorLuigi_Spawn fixes the total size. Its
 * factory constructs the four owned member regions in declaration order and
 * D1/D0 destroy them in reverse order, independently proving the layout below:
 * ModelAnim @ 0x0d4, Model @ 0x138, ShadowModel @ 0x188, and two
 * TextureSequence objects @ 0x1b0. The remaining tail is a state descriptor
 * pointer followed by the shadow matrix used by Behavior.
 *
 * The ROM vtable has exactly dActor_c's 31 slots. Slots 0, 3, 6, 9, 12, 16
 * and 17 point at the overrides declared here; every other destination is
 * inherited unchanged.
 */
#ifndef MIRRORLUIGI_H
#define MIRRORLUIGI_H

#include "dActor_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "math/Matrix.h"

struct MirrorLuigiState;

struct MirrorLuigi : dActor_c {
    u8 pad_0d0[0x4];
    ModelAnim mModelAnim;                     /* 0x0d4 */
    Model mModel;                             /* 0x138 */
    ShadowModel mShadowModel;                 /* 0x188 */
    TextureSequence mTextureSequences[2];     /* 0x1b0 */
    MirrorLuigiState *mState;                 /* 0x1d8 */
    Matrix4x3 mShadowMatrix;                  /* 0x1dc */

    virtual ~MirrorLuigi();                   /* slots 16, 17 */
    virtual s32 InitResources();              /* slot  0 */
    virtual s32 CleanupResources();           /* slot  3 */
    virtual s32 Behavior();                   /* slot  6 */
    virtual s32 Render();                     /* slot  9 */
    virtual void OnPendingDestroy();          /* slot 12 */
};

typedef char MirrorLuigi_size_must_be_0x20c[
    sizeof(MirrorLuigi) == 0x20c ? 1 : -1];

#endif /* MIRRORLUIGI_H */
