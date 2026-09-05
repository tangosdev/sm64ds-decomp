//cpp
// @symbol _ZN5dPa_c7level_c20checkYoganCallback_c14SpawnParticlesERN8Particle6SystemE
#include "dPa_c.h"
#include "dBgCh_Gnd.h"
#include "Particle__System.h"

extern "C" int func_02037e38(unsigned int *p);

void dPa_c::level_c::checkYoganCallback_c::SpawnParticles(
    Particle::System& system)
{
    simpleCallback_c::SpawnParticles(system);

    for (Particle::ParticleNode *particle = system.particles.head;
         particle != 0; particle = particle->next) {
        int sx = particle->offsetAsr3.x + particle->positionAsr3.x;
        int sy = particle->offsetAsr3.y + particle->positionAsr3.y;
        int sz = particle->offsetAsr3.z + particle->positionAsr3.z;
        dBgCh_Gnd rg;
        int vy = (sy << 3) + 0x12c000;
        Vector3 v;
        v.x = vy ? sx << 3 : sx << 3;
        v.y = vy;
        v.z = sz << 3;
        rg.SetObjAndPos(v, 0);
        if (rg.DetectClsn() == 0)
            goto Lac;
        if (func_02037e38((u32*)&rg.surface) != 1) {
        Lac:
            particle->age = particle->lifetime;
        } else {
            particle->offsetAsr3.y =
                ((rg.clsnY + 0x7000) >> 3) - particle->positionAsr3.y;
        }
    }
}
