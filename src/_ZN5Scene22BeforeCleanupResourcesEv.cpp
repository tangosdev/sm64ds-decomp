//cpp
/* Scene::BeforeCleanupResources() at 0x0202e5f0, 0x3c bytes -- vtable slot 4.
 *
 * Chains to ActorBase's, gives up if it fails, and otherwise tears down the object
 * at 0x0209b53c through 0x02011974. The early exit is a real early return, not a
 * predicated fall-through: the ROM ends the failing path with its own
 * `addeq sp,#4; ldmeq sp!,{lr}; bxeq lr` rather than branching to the tail. */
#include "Scene.h"

/* `extern` on every one of these, and the braced block does not supply it: an
   `extern "C" { u32 x; }` is a linkage specification around a DEFINITION in C++,
   not a declaration, and it silently gives this TU its own copy of the ROM's bss. */
extern "C" {
extern void *data_0209b53c;
extern void func_02011974(void *object);
}

int Scene::BeforeCleanupResources()
{
    if (!ActorBase::BeforeCleanupResources())
        return 0;
    func_02011974(&data_0209b53c);
    return 1;
}
