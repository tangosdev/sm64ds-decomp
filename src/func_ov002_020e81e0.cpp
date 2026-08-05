//cpp
// @symbol func_ov002_020e81e0
/* recovered: shared common types */
#include "common.h"



struct Obj {
    char pad5c[0x5c];
    int f5c;
    int f60;
    int f64;
    char pad68[0x4b4 - 0x68];
    void *f4b4;
};

namespace Particle {
struct Callback;
struct System {
    static System *New(unsigned int a, unsigned int b, int c, int d, int e,
                       const Vector3 *p, Callback *cb);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" System * _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int c, int d, int e, const Vector3 *p, Callback *cb);

}

extern "C" void func_ov002_020e81e0(Obj *self) {
    Vector3 v;
    v.x = self->f5c;
    v.y = self->f60;
    v.z = self->f64;
    v.y += 0xd000;
    self->f4b4 = Particle::_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(volatile unsigned int *)&self->f4b4, 0x113, *(volatile int *)&v.x, *(volatile int *)&v.y, v.z, 0, 0);
}
