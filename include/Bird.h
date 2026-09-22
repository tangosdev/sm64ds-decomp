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

    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~Bird();   /* no slot */
#else
    virtual ~Bird();   /* D1 and D0 */
#endif

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Bird_size_must_be_0x184[
    sizeof(Bird) == 0x184 ? 1 : -1];
#endif

#endif /* BIRD_H */
