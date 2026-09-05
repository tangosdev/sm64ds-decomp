/* The cartridge's RTTI names this Tick Tock Clock conveyor class
 * daObjCtMecha04_c. Both the large and small spawn entries construct the same
 * class and select the resource set through actorID in InitResources.
 *
 * The inheritance and ownership are visible in both lifecycle directions:
 * the factories construct dBgActor_c, then TextureTransformer at 0x320 and
 * ShadowModel at 0x334; the destructor tears those members down in reverse
 * order before inlining dBgActor_c's destruction. The allocation literal in
 * both factories is 0x3a0, independently fixing the complete object size. */
#ifndef DAOBJCTMECHA04_C_H
#define DAOBJCTMECHA04_C_H
#include "types.h"
#ifdef __cplusplus
#include "dBgActor_c.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"

struct daObjCtMecha04_c : dBgActor_c {
    /* Inline on purpose: this directly included declaration makes mwccarm emit
     * the measured D1-then-D0 pair without a homeless D2. */
    virtual ~daObjCtMecha04_c() {}

    TextureTransformer mTextureTransformer; /* 0x320 */
    ShadowModel mShadowModel;                /* 0x334 */
    /* UpdateShadow builds this from mAngleY, then writes the actor's X/Z and
     * ground Y into its translation before passing it to DropShadowScaleXYZ. */
    Matrix4x3 mShadowMat;                    /* 0x35c */
    s32 mBeltSpeed;                         /* 0x38c */
    s32 mTargetBeltSpeed;                   /* 0x390 */
    s32 mGroundY;                           /* 0x394 */
    s32 mSoundHandle;                       /* 0x398 */
    s16 mDirectionTimer;                    /* 0x39c */
    u8  mVariant;                           /* 0x39e */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    /* Descriptive reconstruction; the ROM proves ownership and the no-argument
     * call shape, not the original English identifier. */
    void UpdateShadow();
    /* Descriptive reconstruction. Member/free form, target type, and the
     * dActor_c& spelling encoded in the symbol are not recoverable from ROM. */
    void MoveActorOnBelt(dActor_c &actor);
    /* Descriptive reconstruction. Static/member/free form and every pointer
     * type encoded in the symbol are inferred from the callback ABI. */
    static void AfterClsnCallback(dBgW *collider, dActor_c *owner,
                                  dActor_c *other);
};

typedef char daObjCtMecha04_c_size_must_be_0x3a0[
    sizeof(daObjCtMecha04_c) == 0x3a0 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCTMECHA04_C_H */
