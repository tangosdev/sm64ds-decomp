//cpp
#include "dActor_c.h"
#include "dBgCh_Gnd.h"

namespace Particle {
/* Deliberately uses raw fixed-point scalars: the ROM name carries by-value
   Fix12<int> parameters, which mwccarm passes differently at the call site.
   See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12i a, Fix12i b, Fix12i c);
}

void dActor_c::HugeLandingDustAt(Vector3 &pos, bool doRaycast)
{
    if (doRaycast) {
        dBgCh_Gnd ground;
        pos.y += 0x32000;
        ground.SetObjAndPos(pos, 0);
        if (ground.DetectClsn())
            pos.y = ground.clsnY;
    }
    pos.y += 0x28000;
    Particle::_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0xb2, pos.x, pos.y, pos.z);
}
