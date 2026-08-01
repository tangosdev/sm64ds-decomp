//cpp
// @symbol _ZN5Actor18AfterInitResourcesEj
/* Actor::AfterInitResources(u32) at 0x02011244 -- vtable slot 2.
 *
 * Chains to the ActorDerived implementation, then sets the three flag bits that
 * suppress this actor until something clears them.
 *
 * Kept as a .c filename with the //cpp marker rather than renamed to .cpp:
 * the extension is what config/arm9/delinks.txt records, so a rename would turn
 * a src-only change into a config change too.
 */
#include "Actor.h"

void Actor::AfterInitResources(u32 vfSuccess)
{
    ActorDerived::AfterInitResources(vfSuccess);
    /* The (long long)(int) cast is a codegen launder, not a type conversion: the
       ROM materialises the address into a register first (`add r1,r4,#0xb0`)
       and then does the read-modify-write through it. Writing `mFlags |= 0x38`
       folds the offset into the ldr/str and comes out 4 bytes short. */
    *(u32 *)(long long)(int)&mFlags |= 0x38;
}
