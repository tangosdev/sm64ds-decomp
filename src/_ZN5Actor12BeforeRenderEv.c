#include "types.h"
/* Actor::BeforeRender — checks flags before rendering.
 * Calls ActorBase::BeforeRender(); if it returns 0, returns 0.
 * Otherwise checks: WRONG_AREA (bit5=0x20) -> return 0,
 * then OFF_SCREEN (bit3=0x8) + NO_RENDER_IF_OFF_SCREEN (bit1=0x2) -> return 0,
 * else return 1.
 *
 * Callee: 0x02043ac8 = ActorBase::BeforeRender()
 *   _ZN9ActorBase12BeforeRenderEv
 */
struct Actor {
    char _pad[0xb0];
    u32 flags;
};

extern int _ZN9ActorBase12BeforeRenderEv(struct Actor* self);

int _ZN5Actor12BeforeRenderEv(struct Actor* this_)
{
    u32 flags;
    if (!_ZN9ActorBase12BeforeRenderEv(this_))
        return 0;
    flags = this_->flags;
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
