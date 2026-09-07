//cpp
// @symbol _ZN5dPa_c7level_c14clipCallback_c8OnUpdateERN8Particle6SystemEb
#include "Clipper.h"
#include "dPa_c.h"
#include "math/Matrix.h"
#include "Particle__System.h"

extern "C" Clipper data_0209f43c;
extern "C" Matrix4x3 data_0209b3ec;
extern "C" Matrix4x3 data_0209b41c;
extern "C" void MulVec3Mat4x3(
    const Vector3 *vector, const Matrix4x3 *matrix, Vector3 *result);
/* Fix12<int> by value is the measured 2004/b56 caller-side ABI wall. */
extern "C" int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
    Clipper *clipper, Matrix4x3 *matrix, Vector3 *source,
    int scale, Vector3 *result);

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

        result = _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
            &data_0209f43c, &data_0209b3ec, &worldPos, 0x8000, &clipPos);
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
