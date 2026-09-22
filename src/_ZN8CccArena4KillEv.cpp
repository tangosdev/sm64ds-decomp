//cpp
// @symbol _ZN8CccArena4KillEv
/* recovered: real C++ override -- slot 31, attributed by the vtable.
   _ZTV8CccArena + 4*31 = 0x021231e8 + 0x7c = 0x02123264;
   config/arm9/overlays/ov073/relocs.txt: 0x02123264 -> 0x021223f4.
   Not the key function (see include/CccArena.h). Matched byte-for-byte;
   the body is a decompilation verified against the ROM, not an inferred
   stub. */
#include "CccArena.h"

extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN8dActor_c10PoofDustAtERK7Vector3(void* self, const Vector3& vec);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3& pos);
}

void CccArena::Kill()
{
    char *c = (char *)this;
    Vector3 vec;
    Vector3 vec2;
    vec.x = *(int*)(c + 0x5c);
    vec.y = *(int*)(c + 0x60);
    vec.z = *(int*)(c + 0x64);
    vec.y += 0x32000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa, vec.x, vec.y, vec.z);
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    _ZN8dActor_c10PoofDustAtERK7Vector3(c, vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3*)(c + 0x74));
}
