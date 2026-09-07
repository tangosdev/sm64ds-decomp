#ifndef BIRD_H
#define BIRD_H

#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dActor_c.h"

/* daSBird_c_classInit allocates 0x184 bytes, constructs dActor_c, then constructs a
 * ModelAnim at 0xd4 and a ShadowModel at 0x138. D1 destroys those members in
 * reverse order before chaining to dActor_c. InitResources snapshots the
 * inherited actor position into 0x160 and Behavior dispatches through the
 * state index at 0x17c.
 *
 * SM64DS RTTI names the implementation daSBird_c. The reconstructed factory
 * daSBird_c_classInit (historical alias Bird_Spawn) installs this class's
 * cartridge vtable for the SBIRD registry profile.
 */
struct Bird : dActor_c {
    u8          pad_0d0[0x4];
    ModelAnim   mModelAnim;      /* 0x0d4 */
    ShadowModel mShadowModel;    /* 0x138 */
    Vector3     mHomePos;        /* 0x160 */
    u8          pad_16c[0xc];
    u32         mOwnerID;        /* 0x178 */
    s32         mState;          /* 0x17c */
    u8          mIsLeader;         /* 0x180 */
    u8          pad_181[0x3];

    virtual ~Bird();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

typedef char Bird_size_must_be_0x184[
    sizeof(Bird) == 0x184 ? 1 : -1];

#endif /* BIRD_H */
