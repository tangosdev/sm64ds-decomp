//cpp
// @symbol _ZN11VolcanoFire8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "VolcanoFire.h"
typedef int Fix12i;
struct Vector3_16f;
struct dCc_c;

struct C;
typedef void (C::*PMF)();
struct Holder { int pad[2]; PMF pmf; };  // pmf at offset 8

extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(void* a, dCc_c* c);
extern "C" void _ZN5dCc_c5ClearEv(void* c);
extern "C" void _ZN5dCc_c6UpdateEv(void* c);
extern "C" unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, Fix12i c, Fix12i d, Fix12i e, const Vector3_16f* f);

int VolcanoFire::Behavior()
{
    char* c = ((char*)this);
    C* o = (C*)c;
    DecIfAbove0_Short((unsigned short*)(c + 0x110));
    {
        Holder* h = *(Holder**)(c + 0x108);
        if (*(int*)((char*)h + 8) != 0) {
            (o->*(h->pmf))();
        }
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (dCc_c*)(c + 0xd4));
    _ZN5dCc_c5ClearEv(c + 0xd4);
    _ZN5dCc_c6UpdateEv(c + 0xd4);
    {
        int b = (int)((*(int*)(c + 0xb0) & 8) != 0);
        if (b == 0) {
            *(int*)(c + 0x114) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                *(unsigned int*)(c + 0x114), 0x129, *(int*)(c + 0x5c),
                *(int*)(c + 0x60), *(int*)(c + 0x64), 0);
        }
    }
    return 1;
}
