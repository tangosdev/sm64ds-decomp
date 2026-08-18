//cpp
/* Actor::TriplePoofDustAt(Vector3 const&) at 0x0200fd74, 0x54 bytes.
 *
 * PoofDustAt with a third particle: 0x124, 0x125, 0x126.
 *
 * Member for the same reason as its siblings -- the position arrives in r1 and
 * r0 is never read, so the unused first argument is `this`. See
 * src/_ZN5Actor10PoofDustAtERK7Vector3.cpp for the full argument.
 */
#include "Actor.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void Actor::TriplePoofDustAt(const Vector3 &pos)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x124, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x125, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x126, pos.x, pos.y, pos.z);
}
