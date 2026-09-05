//cpp
// @symbol _ZN8Particle6System9NewSimpleEjiii
#include "Particle__System.h"

void Particle::System::NewSimple(u32 definitionID, Fix12i x,
                                 Fix12i y, Fix12i z)
{
    Vector3 pos;
    s32 xv = x >> 3;
    s32 yv = y >> 3;
    s32 zv = z >> 3;
    pos.x = xv;
    pos.y = yv;
    pos.z = zv;
    data_0209ee74->mManager->AddSystem(definitionID, pos);
}
