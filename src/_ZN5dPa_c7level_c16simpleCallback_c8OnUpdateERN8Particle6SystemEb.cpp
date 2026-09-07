//cpp
// @symbol _ZN5dPa_c7level_c16simpleCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

int dPa_c::level_c::simpleCallback_c::OnUpdate(
    Particle::System& system, bool)
{
    value = system.callbackValue;
    return 1;
}
