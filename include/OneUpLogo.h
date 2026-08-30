#ifndef ONEUPLOGO_H
#define ONEUPLOGO_H

#include "Model.h"
#include "TextureSequence.h"
#include "dActor_c.h"

/* OneUpLogo_Spawn allocates 0x150 bytes, constructs dActor_c, then constructs
 * Model at 0xd4 and TextureSequence at 0x124. D1 destroys those two subobjects
 * in reverse order before chaining to dActor_c. InitResources and Behavior
 * close the remaining fields at 0x138..0x14e.
 */
struct OneUpLogo : dActor_c {
    u8              pad_0d0[0x4];
    Model           mModel;             /* 0x0d4 */
    TextureSequence mTextureSequence;   /* 0x124 */
    u32             mFollowActorID;     /* 0x138 */
    Vector3         mInitialPos;        /* 0x13c */
    s32             mFollowYOffset;     /* 0x148 */
    u16             mDelay;             /* 0x14c */
    u8              mState;             /* 0x14e */
    u8              pad_14f;

    virtual ~OneUpLogo() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

typedef char OneUpLogo_size_must_be_0x150[
    sizeof(OneUpLogo) == 0x150 ? 1 : -1];

#endif /* ONEUPLOGO_H */
