//cpp
// @symbol _ZN5dPa_c7level_c20edStarKiraCallback_c8OnUpdateERN8Particle6SystemEb
#include "dPa_c.h"
#include "Particle__System.h"

extern "C" void func_0204dab4(void* system, void* records, int count,
                                u32 mask, void* callback);
extern "C" void func_02022260();

int dPa_c::level_c::edStarKiraCallback_c::OnUpdate(Particle::System& system,
                                                    bool active)
{
    func_0204dab4(&system, trackingRecords, 0x40, 0,
                  (void*)func_02022260);
    return simpleCallback_c::OnUpdate(system, active);
}
