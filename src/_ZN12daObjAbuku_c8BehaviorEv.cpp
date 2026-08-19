//cpp
// @symbol _ZN12daObjAbuku_c8BehaviorEv
/* recovered: renamed to Class_Method, declarations from a shared header.
 * Was func_ov002_020b33dc, vtable slot 6. */
#include "decl_common.h"
/* daObjAbuku_c::Behavior - recovered from vtable slot identity */
typedef long long s64;
extern short data_02082214[];
extern "C" void _Z14ApproachLinearRiii(int* p, int b, int c);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(char* self, void* c);
extern "C" char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern "C" void _ZN6Player4HealEi(char* self, int amt);
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int c, int d, int e, const void* f, void* g);
extern "C" void _ZN5dCc_c5ClearEv(char* cl);
extern "C" void _ZN5dCc_c6UpdateEv(char* cl);

extern "C" int _ZN12daObjAbuku_c8BehaviorEv(char* self)
{
    *(short*)(((int)self + 0x10c)) += 0x400;
    int v = *(volatile unsigned short*)(self + 0x10c);
    int x = v >> 4;
    short tv = data_02082214[2*x + 1];
    *(int*)(self + 0x98) = (int)(((s64)*(int*)(self + 0x108) * tv + 0x800) >> 12);
    _Z14ApproachLinearRiii((int*)(self + 0x108), 0x6000, 0x332);
    _ZN8dActor_c9UpdatePosEP5dCc_c(self, 0);
    unsigned int id = *(unsigned int*)(self + 0xf8);
    if (id) {
        char* found = _ZN8dActor_c10FindWithIDEj(id);
        if (found) {
            int b = (*(unsigned short*)(found + 0xc) == 0xbf);
            if (b) {
                _ZN6Player4HealEi(found, 0x300);
                func_ov002_020b330c(self);
            }
        }
    }
    if (DecIfAbove0_Short((unsigned short*)(self + 0x10e)) == 0 || func_ov002_020b3344(self)) {
        func_ov002_020b330c(self);
    }
    *(int*)(self + 0x110) = (int)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned int*)(self + 0x110), 1, *(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64), 0, 0);
    _ZN5dCc_c5ClearEv(self + 0xd4);
    _ZN5dCc_c6UpdateEv(self + 0xd4);
    return 1;
}
