//cpp
/* dActor_c::DisappearPoofDustAt(Vector3 const&) at 0x0200fd04, 0x3c bytes.
 *
 * The two-particle poof used when something vanishes: 0x127, 0x128.
 *
 * Member for the same reason as its siblings -- the position arrives in r1 and
 * r0 is never read, so the unused first argument is `this`. See
 * src/_ZN8dActor_c10PoofDustAtERK7Vector3.cpp for the full argument.
 */
#include "dActor_c.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void dActor_c::DisappearPoofDustAt(const Vector3 &pos)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x127, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x128, pos.x, pos.y, pos.z);
}
