//cpp
typedef int Fix12i;
struct Vector3 { Fix12i x, y, z; };
struct Actor;
struct RaycastGround {
    char buf[0x50];
    RaycastGround();
    ~RaycastGround();
    void SetObjAndPos(const Vector3 &pos, Actor *actor);
    int DetectClsn();
};
namespace Particle { struct System {
    static void NewSimple(unsigned int id, Fix12i a, Fix12i b, Fix12i c);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12i a, Fix12i b, Fix12i c);
 }

extern "C" void _ZN5Actor13LandingDustAtER7Vector3b(Actor *self, Vector3 *pos, bool b)
{
    if (b) {
        RaycastGround rc;
        *(Fix12i*)(((int)pos + 4) & 0xFFFFFFFFFFFFFFFFULL) += 0x32000;
        rc.SetObjAndPos(*pos, 0);
        if (rc.DetectClsn())
            pos->y = *(Fix12i*)((char*)&rc + 0x44);
    }
    *(Fix12i*)(((int)pos + 4) & 0xFFFFFFFFFFFFFFFFULL) += 0x5a000;
    Particle::_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb1, pos->x, pos->y, pos->z);
}
