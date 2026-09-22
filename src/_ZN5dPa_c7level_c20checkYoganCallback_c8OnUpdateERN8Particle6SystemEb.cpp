//cpp
// @symbol _ZN5dPa_c7level_c20checkYoganCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

int dPa_c::level_c::checkYoganCallback_c::OnUpdate(Particle::System& system,
                                                    bool active)
{
    if (!active && system.particles.head == 0)
        return 0;

    return simpleCallback_c::OnUpdate(system, active);
}
