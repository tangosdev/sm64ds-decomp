#ifndef AMBIENTSOUNDEFFECTS_H
#define AMBIENTSOUNDEFFECTS_H

#include "dActor_c.h"

/* AmbientSoundEffects_Spawn allocates 0xd8 bytes and constructs only the
 * dActor_c base. Behavior stores the active long-sound handle in the sole
 * derived word at 0xd4; the effect selector is inherited param1 at 0x08.
 */
struct AmbientSoundEffects : dActor_c {
    u8  pad_0d0[0x4];
    u32 mSoundHandle;       /* 0x0d4 */

    virtual ~AmbientSoundEffects() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

typedef char AmbientSoundEffects_size_must_be_0xd8[
    sizeof(AmbientSoundEffects) == 0xd8 ? 1 : -1];

#endif /* AMBIENTSOUNDEFFECTS_H */
