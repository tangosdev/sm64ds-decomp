//cpp
// @symbol _ZN5Crate4KillEv
/* recovered: named members + real C++ method */
/* Crate::Kill() -- vtable slot 31. Puffs a simple particle and a dust poof at
 * the crate's own position raised 0x28000, plays the break sample from the
 * camera-space position, and parks the crate in state 6. */
#include "Crate.h"
#include "decl_Actor.h"
#include "decl_common.h"

extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3 *pos);
void Crate_SetState(char *c, int i);
}

void Crate::Kill()
{
    Vector3 vec;
    Vector3 vec2;
    int x, y, z;
    func_ov098_02138e08((char *)this);
    x = mPosX;
    y = mPosY + 0x28000;
    z = mPosZ;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xe, vec.x, vec.y, vec.z);
    ((int *)&vec2)[0] = ((int *)&vec)[0];
    ((int *)&vec2)[1] = ((int *)&vec)[1];
    ((int *)&vec2)[2] = ((int *)&vec)[2];
    _ZN8dActor_c19DisappearPoofDustAtERK7Vector3((char *)this, &vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, (Vector3 *)&mCamSpacePosX);
    Crate_SetState((char *)this, 6);
}
