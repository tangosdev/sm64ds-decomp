//cpp
// @symbol func_ov014_02112ea8
/* recovered: shared common types */
#include "common.h"
typedef int Fix12;

struct Sound { static void PlayBank3(unsigned int id, const Vector3 &v); };
namespace Particle { struct System { static void *NewSimple(unsigned int t, Fix12 x, Fix12 y, Fix12 z); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void * _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int t, Fix12 x, Fix12 y, Fix12 z);
 }
struct dBgW { int pad; int IsEnabled(); void Disable(); };
struct ActorS {
    char pad0[0x5c];
    int px, py, pz;
    char pad1[0x74 - 0x68];
    Vector3 v74;
    char pad2[0x124 - 0x80];
    dBgW col;
    char pad3[0x31e - 0x128];
    unsigned char flag;
    void PoofDustAt(const Vector3 &v);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8dActor_c10PoofDustAtERK7Vector3(void *, const Vector3 &v);


extern "C" void func_ov014_02112ea8(ActorS *a)
{
    Vector3 v[2];
    Sound::PlayBank3(0xf, a->v74);
    {
        int ty = a->py;
        int tz = a->pz;
        int tx = a->px;
        v[0].x = tx;
        v[0].y = ty + 0x12c000;
        v[0].z = tz;
    }
    Particle::_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1e, v[0].x, v[0].y, v[0].z);
    v[1].x = v[0].x;
    v[1].y = v[0].y;
    v[1].z = v[0].z;
    _ZN8dActor_c10PoofDustAtERK7Vector3(a, v[1]);
    a->flag = 1;
    if (a->col.IsEnabled())
        a->col.Disable();
}
