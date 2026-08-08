//cpp
/* Actor::BeforeRender() at 0x02010f78, 0x50 bytes -- vtable slot 10.
 *
 * Asks the base first, then applies two of Actor's own reasons not to draw:
 *
 *   0x20  the actor belongs to an area that is not the showing one
 *   0x08  off screen, as decided by BeforeBehavior's clipper test
 *   0x02  "do not render while off screen"
 *
 * Bit 0x08 alone is not disqualifying -- an actor may be off screen and still
 * want drawing -- so the off-screen path returns 0 only when 0x02 is also set.
 * That is why the test is two branches rather than a single mask compare.
 *
 * mFlags is named in include/Actor.h at 0x0b0; the old source reached it
 * through a shadow `struct Actor { char _pad[0xb0]; u32 flags; }`.
 */
#include "Actor.h"

int Actor::BeforeRender()
{
    u32 flags;

    if (!ActorBase::BeforeRender())
        return 0;

    flags = mFlags;
    if (flags & 0x20)
        goto ret0;
    if (!(flags & 8))
        goto ret1;
    if (!(flags & 2))
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}
