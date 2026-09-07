// @symbol _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_
// Particle::System::NewSimple - creates a new particle system at position
#include "types.h"
#include "Particle.h"

extern struct Particle* data_0209ee74;
extern void* _ZN8Particle7Manager9AddSystemEiR7Vector3(void* mgr, u32 uniqueID, Vector3* pos);

void* _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 uniqueID, s32 x, s32 y, s32 z) {
    Vector3 pos;
    s32 xv = x >> 3;
    s32 yv = y >> 3;
    s32 zv = z >> 3;
    pos.x = xv;
    pos.y = yv;
    pos.z = zv;
    void* mgr = data_0209ee74->mManager;
    return _ZN8Particle7Manager9AddSystemEiR7Vector3(mgr, uniqueID, &pos);
}
