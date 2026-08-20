#ifndef BOBOMBBUDDY_H
#define BOBOMBBUDDY_H

#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dActor_c.h"

/* BobOmbBuddy_Spawn allocates 0x1f0 bytes, constructs dActor_c, then constructs
 * dCcAc_c at 0xd4, ModelAnim at 0x108, and ShadowModel at 0x16c.
 * D1 destroys the three members in reverse order before chaining to dActor_c.
 */
struct BobOmbBuddy : dActor_c {
    u8                 pad_0d0[0x4];
    dCcAc_c mdCcAc_c;  /* 0x0d4 */
    ModelAnim          mModelAnim;           /* 0x108 */
    ShadowModel        mShadowModel;         /* 0x16c */
    u8                 pad_194[0x4];
    s32                unk_198;              /* 0x198 */
    u8                 pad_19c[0x54];

    virtual ~BobOmbBuddy();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

typedef char BobOmbBuddy_size_must_be_0x1f0[
    sizeof(BobOmbBuddy) == 0x1f0 ? 1 : -1];

#endif /* BOBOMBBUDDY_H */
