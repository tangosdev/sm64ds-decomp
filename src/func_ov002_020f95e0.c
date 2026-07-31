// @symbol func_ov002_020f95e0
// @emits daSoundObj_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daSoundObj_c::InitResources - recovered from vtable slot identity */
#pragma opt_loop_invariants off


extern void *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void *prev);
extern void _ZN9ActorBase18MarkForDestructionEv(void *thiz);
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);

int daSoundObj_c_InitResources(void *self)
{
    char *c = (char *)self;
    void *a;
    int g;

    if (*(unsigned int *)(c + 8) > 7)
        return 0;

    *(int *)(c + 0xd4) = *(int *)((char *)data_ov002_0210c080 + *(unsigned int *)(c + 8) * 0xc);
    *(int *)(c + 0xd8) = *(int *)((char *)data_ov002_0210c084 + *(unsigned int *)(c + 8) * 0xc);
    *(short *)(c + 0xde) = *(unsigned short *)((char *)data_ov002_0210c088 + *(unsigned int *)(c + 8) * 0xc);
    *(unsigned char *)(c + 0xe0) = *(unsigned char *)((char *)data_ov002_0210c08a + *(unsigned int *)(c + 8) * 0xc);

    a = 0;
    g = data_0208e430;

    if (g == 0x20 || g == 0x29 || g == 0x21 || g == 0x1e || g == 0x50 ||
        (g >= 0x19 && g <= 0x1d) || g == 0x4f || g == 0x22 || g == 0x2a || g == 0x21)
    {
        while (1)
        {
            a = _ZN5Actor15FindWithActorIDEjPS_(0x167, a);
            if (a == 0)
                break;
            if (a != self)
            {
                _ZN9ActorBase18MarkForDestructionEv(a);
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(g, 0x7f, 0, 0x7f000, 0);
            }
        }
    }

    *(short *)(c + 0xdc) = 0;
    *(char *)(c + 0xcc) = -1;
    return 1;
}
