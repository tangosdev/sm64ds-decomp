//cpp
/* arm9/dPa_c -- dPa_c::level_c's particle-callback family, folded from 23
 * legacy shards at .text 0x02021e40..0x020226d4. The run is the callback
 * class block the tree's `dPa_c` name already covers (RTTI _ZTSN5dPa_c7level_c..._cE
 * in config/arm9/symbols.txt): callback_c's two virtuals, simpleCallback_c's
 * pair plus its C1/C2 constructor pair, the scale/edStarKira leaf
 * constructors, and the address-named func_02022260 worker the edStarKira
 * update path funnels through. The span is bounded on both sides by
 * different classes: Particle_SysTracker_Contents ends at 0x02021e40 below,
 * and Particle::SetSelfDestructFlag begins at 0x020226d4 above.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending; the roster reads ROM-ascending.
 *
 *   callback_c::SpawnParticles, callback_c::OnUpdate -- base no-ops.
 *   simpleCallback_c::C1/C2 -- one definition emits both variants.
 *   simpleCallback_c::SpawnParticles, simpleCallback_c::OnUpdate.
 *   scaleCallback_c::C1, scaleCallback_c::SpawnParticles,
 *     scaleCallback_c::OnUpdate.
 *   splashCallback_c::OnUpdate, bubbleCallback_c::OnUpdate,
 *     fitWaterSimpleCallback_c::OnUpdate.
 *   checkYoganCallback_c::SpawnParticles, checkYoganCallback_c::OnUpdate.
 *   edStarKiraCallback_c::C1, func_02022260, edStarKiraCallback_c::
 *     SpawnParticles, edStarKiraCallback_c::OnUpdate.
 *   checkWaterCallback_c::OnUpdate, checkWaterRippleCallback_c::OnUpdate,
 *     fitWaterCallback_c::OnUpdate, clipCallback_c::OnUpdate,
 *     cleanParticleCallback_c::OnUpdate.
 */

#include "dPa_c.h"
#include "Particle__System.h"
#include "dClipper.h"
#include "math/Matrix.h"
#include "dBgCh_Gnd.h"
#include "math/CalcLib.h"

extern "C" {
/* local extern: no header in include/ declares these (dClipper.h only
 * mentions the clipper instance, Player.h only the water height). */
extern dClipper data_0209f43c;
extern Matrix4x3 data_0209b3ec;
extern Matrix4x3 data_0209b41c;
extern s32 data_0209f32c;
void MulVec3Mat4x3(Vector3 *vector, Matrix4x3 *matrix, Vector3 *result);
void func_0204dab4(void *system, void *records, int count, u32 mask, void *callback);
/* This initializer's 0xc-byte layout is evidenced, but its original class
 * name is not. Keep that single unresolved ABI boundary explicit. */
void func_0204dc84(char *record);
void func_02049d60(Particle::Manager *manager, Particle::System *system);
}

// @symbol _ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE
void dPa_c::level_c::callback_c::SpawnParticles(Particle::System&)
{
}

// @symbol _ZN5dPa_c7level_c10callback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::callback_c::OnUpdate(Particle::System&, bool)
{
    return 1;
}

/* Complete- and base-object constructors. One definition emits the C1/C2
 * pair; calls from derived callback construction identify 0x020226a4 as
 * C2 and the matching C1 variant lives at 0x02022680. */
// @symbol _ZN5dPa_c7level_c16simpleCallback_cC1Ev
// @symbol _ZN5dPa_c7level_c16simpleCallback_cC2Ev
dPa_c::level_c::simpleCallback_c::simpleCallback_c()
    : value(0)
{
}

// @symbol _ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE
void dPa_c::level_c::simpleCallback_c::SpawnParticles(
    Particle::System& system)
{
    system.callbackFlags |= 2;
    system.callbackValue = value;
    func_02049d60(data_0209ee74->mManager, &system);
}

// @symbol _ZN5dPa_c7level_c16simpleCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::simpleCallback_c::OnUpdate(
    Particle::System& system, bool)
{
    value = system.callbackValue;
    return 1;
}

/* Four SysTracker member calls identify the complete-object variant. The
 * compiler emits simpleCallback_c C2 and the scaleCallback_c vptr store;
 * the unused C2 twin is a homeless compiler copy with no ROM address. */
// @symbol _ZN5dPa_c7level_c15scaleCallback_cC1Ev
dPa_c::level_c::scaleCallback_c::scaleCallback_c()
    : scale(0x1000), velocity(0)
{
}

// @symbol _ZN5dPa_c7level_c15scaleCallback_c14SpawnParticlesERN8Particle6SystemE
void dPa_c::level_c::scaleCallback_c::SpawnParticles(Particle::System& system)
{
    system.callbackScale = scale;
    system.callbackVelocity = velocity;
    simpleCallback_c::SpawnParticles(system);
    timer = 0x10;
}

// @symbol _ZN5dPa_c7level_c15scaleCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::scaleCallback_c::OnUpdate(Particle::System& system,
                                               bool active)
{
    simpleCallback_c::OnUpdate(system, active);
    return DecIfAbove0_Byte(&timer) != 0;
}

// @symbol _ZN5dPa_c7level_c16splashCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::splashCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    Particle::ParticleNode *particle = system.particles.head;

    while (particle != 0) {
        const s32 x = particle->offsetAsr3.x + particle->positionAsr3.x;
        const s32 y = particle->offsetAsr3.y + particle->positionAsr3.y;
        const s32 z = particle->offsetAsr3.z + particle->positionAsr3.z;

        if (particle->velocityAsr3.y < 0 && (y << 3) < data_0209f32c) {
            particle->age = particle->lifetime;
            Particle::System::NewRipple(x << 3, data_0209f32c + 0x3000, z << 3);
        }
        particle = particle->next;
    }

    return simpleCallback_c::OnUpdate(system, active);
}

// @symbol _ZN5dPa_c7level_c16bubbleCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::bubbleCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    Particle::ParticleNode *particle = system.particles.head;

    while (particle != 0) {
        const s32 y = particle->offsetAsr3.y + particle->positionAsr3.y;
        const s32 x = particle->offsetAsr3.x + particle->positionAsr3.x;
        const s32 z = particle->offsetAsr3.z + particle->positionAsr3.z;

        if ((y << 3) > data_0209f32c) {
            particle->age = particle->lifetime;
            Particle::System::NewRipple(x << 3, data_0209f32c + 0x3000, z << 3);
        }
        particle = particle->next;
    }

    return simpleCallback_c::OnUpdate(system, active);
}

// @symbol _ZN5dPa_c7level_c24fitWaterSimpleCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::fitWaterSimpleCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    s32 waterY;
    Particle::ParticleNode *particle;

    particle = system.particles.head;
    waterY = data_0209f32c >> 3;

    while (particle != 0) {
        particle->offsetAsr3.y = waterY - particle->positionAsr3.y;
        particle = particle->next;
    }

    return simpleCallback_c::OnUpdate(system, active);
}

// @symbol _ZN5dPa_c7level_c20checkYoganCallback_c14SpawnParticlesERN8Particle6SystemE
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
        if (rg.DetectClsn() == 0 || rg.surface.clps.GetSurfaceType() != 1) {
            particle->age = particle->lifetime;
        } else {
            particle->offsetAsr3.y =
                ((rg.clsnY + 0x7000) >> 3) - particle->positionAsr3.y;
        }
    }
}

// @symbol _ZN5dPa_c7level_c20checkYoganCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::checkYoganCallback_c::OnUpdate(Particle::System& system,
                                                    bool active)
{
    if (!active && system.particles.head == 0)
        return 0;

    return simpleCallback_c::OnUpdate(system, active);
}

// @symbol _ZN5dPa_c7level_c20edStarKiraCallback_cC1Ev
dPa_c::level_c::edStarKiraCallback_c::edStarKiraCallback_c()
{
    trackingRecordStorage_c *record = trackingRecords;
    int i = 0;
    do {
        func_0204dc84((char *)record);
        ++i;
        ++record;
    } while (i < 0x40);

    unk308 = 0x1000;
    unk30c = 0x200;
}

/* The projection func_0204dab4 runs for each tracked particle in place of
 * its default func_0204dcc0: the record's 2D point and its 20.12 scale. */
extern "C" void func_02022260(int *position, short *screenX, short *screenY, int *scale) {
    *screenX = position[0] >> 9;
    *screenY = position[1] >> 9;
    *scale = 0xfff;
}

// @symbol _ZN5dPa_c7level_c20edStarKiraCallback_c14SpawnParticlesERN8Particle6SystemE
void dPa_c::level_c::edStarKiraCallback_c::SpawnParticles(
    Particle::System& system)
{
    system.mDefinition->data->callbackParam = unk308;
    system.callbackParam = unk30c;
    simpleCallback_c::SpawnParticles(system);
}

// @symbol _ZN5dPa_c7level_c20edStarKiraCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::edStarKiraCallback_c::OnUpdate(Particle::System& system,
                                                    bool active)
{
    func_0204dab4(&system, trackingRecords, 0x40, 0,
                  (void*)func_02022260);
    return simpleCallback_c::OnUpdate(system, active);
}

// @symbol _ZN5dPa_c7level_c20checkWaterCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::checkWaterCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    Particle::ParticleNode *particle = system.particles.head;

    if (active) {
        system.callbackFlags &= ~2;
    } else {
        system.callbackFlags |= 2;
        if (particle == 0)
            return 0;
    }

    while (particle != 0) {
        if (((particle->offsetAsr3.y + particle->positionAsr3.y) << 3)
            > data_0209f32c) {
            particle->age = particle->lifetime;
        }
        particle = particle->next;
    }
    return 1;
}

// @symbol _ZN5dPa_c7level_c26checkWaterRippleCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::checkWaterRippleCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    Particle::ParticleNode *particle = system.particles.head;

    if (active) {
        system.callbackFlags &= ~2;
    } else {
        system.callbackFlags |= 2;
        if (particle == 0)
            return 0;
    }

    while (particle != 0) {
        s32 x = particle->offsetAsr3.x + particle->positionAsr3.x;
        s32 y = particle->offsetAsr3.y + particle->positionAsr3.y;
        s32 z = particle->offsetAsr3.z + particle->positionAsr3.z;

        if (particle->velocityAsr3.y < 0 && (y << 3) < data_0209f32c) {
            particle->age = particle->lifetime;
            Particle::System::NewRipple(x << 3, data_0209f32c + 0x3000, z << 3);
        }
        particle = particle->next;
    }
    return 1;
}

// @symbol _ZN5dPa_c7level_c18fitWaterCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::fitWaterCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    Particle::ParticleNode *particle = system.particles.head;

    if (active) {
        system.callbackFlags &= ~2;
    } else {
        system.callbackFlags |= 2;
        if (particle == 0)
            return 0;
    }

    const s32 waterY = (waterOffset + data_0209f32c) >> 3;
    while (particle != 0) {
        particle->offsetAsr3.y = waterY - particle->positionAsr3.y;
        particle = particle->next;
    }
    return 1;
}

// @symbol _ZN5dPa_c7level_c14clipCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::clipCallback_c::OnUpdate(
    Particle::System& system, bool active)
{
    Particle::ParticleNode *particle;
    Vector3 worldPos;
    Vector3 clipPos;
    Vector3 adjustment;
    int result;

    if (enabled == 0) {
        system.callbackFlags |= 2;
        particle = system.particles.head;
        while (particle != 0) {
            particle->age = particle->lifetime;
            particle = particle->next;
        }
        return 1;
    }

    system.callbackFlags &= ~2;
    particle = system.particles.head;
    while (particle != 0) {
        worldPos.x = particle->offsetAsr3.x + particle->positionAsr3.x;
        worldPos.y = particle->offsetAsr3.y + particle->positionAsr3.y;
        worldPos.z = particle->offsetAsr3.z + particle->positionAsr3.z;

        result = data_0209f43c.TransformAndClip(data_0209b3ec, worldPos, 0x8000, clipPos);
        if (result > 0x100000) {
            if (clipPos.y < -0x40000) {
                particle->age = particle->lifetime;
            } else {
                if (clipPos.y > 0x40000) {
                    clipPos.y -= 0x50000;
                } else if (clipPos.z > 0) {
                    clipPos.z -= 0x80000;
                } else {
                    clipPos.x = clipPos.x < 0 ? 0x20000 : -0x20000;
                }

                MulVec3Mat4x3(&clipPos, &data_0209b41c, &adjustment);
                particle->offsetAsr3.x += adjustment.x - worldPos.x;
                particle->offsetAsr3.y += adjustment.y - worldPos.y;
                particle->offsetAsr3.z += adjustment.z - worldPos.z;
            }
        }
        particle = particle->next;
    }
    return 1;
}

// @symbol _ZN5dPa_c7level_c23cleanParticleCallback_c8OnUpdateERN8Particle6SystemEb
int dPa_c::level_c::cleanParticleCallback_c::OnUpdate(
    Particle::System& system, bool done)
{
    Particle::ParticleNode *particle;

    if (!done) {
        particle = system.particles.head;
        while (particle != 0) {
            particle->age = particle->lifetime;
            return 0;
        }
    }
    return 1;
}
