//cpp
/* dActor_c::PoofDustAt(Vector3 const&) at 0x0200fdfc, 0x40 bytes.
 *
 * Spawns the two-particle poof (0x122, 0x123) at the position handed in.
 *
 * A member, not a static, and the body is why that needs saying: it never
 * touches a field. The ROM decides it at the boundary instead -- the position
 * arrives in r1 (`mov r4, r1`, then every load is off r4), so r0 is written by
 * each caller and read by nobody here. A static taking `const Vector3&` would
 * have received the position in r0. The unused parameter is `this`.
 *
 * Callee 0x02022e98 = Particle::System::NewSimple(u32, Fix12<int> x3). Its
 * mangled name carries by-value class parameters, so it stays spelled out with
 * scalar arguments -- declaring the true types changes how the caller passes
 * them and breaks the bytes. See notes/mwccarm-codegen.md 6az.
 */
#include "dActor_c.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void dActor_c::PoofDustAt(const Vector3 &pos)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x122, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x123, pos.x, pos.y, pos.z);
}
