#ifndef STARDOOR_H
#define STARDOOR_H

#include "CommonModel.h"
#include "dActor_c.h"

struct StarDoorCallback;

/* daStarGate_c_classInit allocates 0x118 bytes, constructs dActor_c, and constructs a
 * CommonModel at 0xd4. D1 destroys that model before chaining to dActor_c.
 * Render writes the model matrix at 0xe0..0x10f, while Behavior reads the
 * callback pointer at 0x110; together those uses close the derived layout.
 *
 * SM64DS RTTI names the implementation daStarGate_c. The reconstructed
 * factory daStarGate_c_classInit (historical alias
 * StarDoor_Spawn) constructs it for the STAR_GATE
 * registry profile.
 */
struct StarDoor : dActor_c {
    u8                pad_0d0[0x4];
    CommonModel       mModel;       /* 0x0d4 */
    StarDoorCallback *mCallback;    /* 0x110 */
    u8                pad_114[0x4];

    virtual ~StarDoor();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

typedef char StarDoor_size_must_be_0x118[
    sizeof(StarDoor) == 0x118 ? 1 : -1];

#endif /* STARDOOR_H */
