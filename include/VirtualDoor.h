#ifndef VIRTUALDOOR_H
#define VIRTUALDOOR_H

#include "dActor_c.h"

/* VirtualDoor_Spawn allocates 0xd4 bytes, constructs dActor_c, and installs
 * _ZTV11VirtualDoor. Its D1 chains directly to dActor_c::~dActor_c, proving
 * the inheritance and leaving only the final four bytes as derived storage.
 * All state used by the door is inherited actor state.
 */
struct VirtualDoor : dActor_c {
    u8 pad_0d0[0x4];

    virtual ~VirtualDoor() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

typedef char VirtualDoor_size_must_be_0xd4[
    sizeof(VirtualDoor) == 0xd4 ? 1 : -1];

#endif /* VIRTUALDOOR_H */
