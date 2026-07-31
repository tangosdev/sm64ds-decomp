//cpp
// @symbol func_ov075_02114300
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;


struct Matrix4x3;
struct Sub { char pad[0x24]; char m[0x30]; };
extern "C" void MulVec3Mat4x3(Matrix4x3* m, Vector3* in, Vector3* out);
extern "C" void Vec3_MulScalarInPlace(Vector3* v, Fix12i s);
extern "C" unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned a, unsigned b, Fix12i c, int d, int e, const Vector3_16* f, void* g);
struct C {
    char pad0[0x14];
    char* mtxbase;
    char pad18[0x1c-0x18];
    Vector3 v1c;
    char pad28[0x118-0x28];
    Fix12i field_118;
    char pad11c[0x14c-0x11c];
    unsigned field_14c;
};
extern "C" void func_ov075_02114300(C* c)
{
    if (c->field_118 >= 0x1c2000) return;
    Vector3 tmp;
    Sub* s = (Sub*)(c->mtxbase + 0x2d0);
    MulVec3Mat4x3((Matrix4x3*)s->m, &c->v1c, &tmp);
    Vec3_MulScalarInPlace(&tmp, 0x8000);
    tmp.y += 0x28000;
    c->field_14c = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(c->field_14c, 0x140, tmp.x, tmp.y, tmp.z, (Vector3_16*)0, (void*)0);
}
