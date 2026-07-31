// @symbol func_ov002_020e7fcc
/* recovered: shared common types, declarations from a shared header */
#include "decl_Animation.h"
/* recovered: shared common types */
#include "common.h"
typedef unsigned int u32;
typedef int Fix12i;


struct Callback;

extern int _ZN9Animation8FinishedEv(void* anim);
extern void func_ov002_020e8244(struct Vector3* out, void* thiz);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 effect, Fix12i x, Fix12i y, Fix12i z, const void* rot, struct Callback* cb);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 effect, Fix12i x, Fix12i y, Fix12i z);

void func_ov002_020e7fcc(char* c)
{
    void* obj;
    struct Vector3 v1;
    struct Vector3 v2;

    obj = *(void**)(c + 0x31c);

    if (!_ZN9Animation8FinishedEv(c + 0x35c)
        && (u32)((*(u32*)(c + 0x364) << 4) >> 0x10) >= 2
        && *(int*)((char*)obj + 0xc) != 0) {
        func_ov002_020e8244(&v1, c);
        *(u32*)(c + 0x4b4) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32*)(c + 0x4b4), 0x2f, v1.x, v1.y, v1.z, 0, 0);
        return;
    }

    if (!_ZNK9Animation12WillHitFrameEi(c + 0x35c, 0x75)) return;
    func_ov002_020e8244(&v2, c);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x30, v2.x, v2.y, v2.z);
}
