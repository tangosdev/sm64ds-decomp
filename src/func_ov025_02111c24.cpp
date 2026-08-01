//cpp
// @symbol func_ov025_02111c24
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named */
#include "daDkk_c.h"
// recovered name: daDkk_c_InitResources
/* recovered: shared common types, renamed to Class_Method */
/* daDkk_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern int func_ov091_02133254(void* c);
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, void* a, void* b, void* act);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZN11RaycastLine10GetClsnPosEv(void* out, void* self);
extern void _ZN11RaycastLineD1Ev(void* self);
int func_ov025_02111c24(char* c)
{
    struct daDkk_c *self = (struct daDkk_c *)(void *)c;
    *(void**)(c + 0x320) = data_ov025_02113814;
    int r = func_ov091_02133254(c);
    if (*(int*)(c + 8) & 1) {
        self->unk_398 = 6;
    } else {
        self->unk_398 = 0;
        char rl[0x78];
        Vector3 va;
        Vector3 vb;
        Vector3 p1;
        Vector3 p2;
        _ZN11RaycastLineC1Ev(rl);
        int x = self->unk_05c;
        vb.x = x;
        int y = self->unk_060;
        vb.y = y;
        int z = self->unk_064;
        va.x = x;
        vb.z = z;
        va.y = y;
        va.z = z;
        vb.y = y + 0x7d0000;
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &va, &vb, c);
        if (_ZN11RaycastLine10DetectClsnEv(rl) != 0) {
            _ZN11RaycastLine10GetClsnPosEv(&p1, rl);
            _ZN11RaycastLine10GetClsnPosEv(&p2, rl);
            self->unk_390 = p2.y - 0x190000;
        }
        _ZN11RaycastLineD1Ev(rl);
    }
    return r;
}
}
