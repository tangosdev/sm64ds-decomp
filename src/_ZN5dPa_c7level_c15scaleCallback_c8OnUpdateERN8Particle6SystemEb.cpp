//cpp
// @symbol _ZN5dPa_c7level_c15scaleCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

extern "C" u8 DecIfAbove0_Byte(u8* value);

int dPa_c::level_c::scaleCallback_c::OnUpdate(Particle::System& system,
                                               bool active)
{
    simpleCallback_c::OnUpdate(system, active);
    return DecIfAbove0_Byte(&timer) != 0;
}
