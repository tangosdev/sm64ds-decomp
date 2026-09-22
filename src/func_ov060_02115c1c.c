#include "types.h"
enum { false, true };

extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void func_ov060_021123a0(void *a, int b);
extern void func_ov060_021123c8(char *p);
extern void _ZN6Player9DropActorEv(void *p);

void func_ov060_02115c1c(char *self)
{
    char *r4;
    char *r1;
    int cond;

    r4 = (char *)_ZN8dActor_c10FindWithIDEj(*(unsigned int *)(self + 0x108));
    if (*(int *)(r4 + 0x40c) == 0x13) {
        func_ov060_021123a0(r4, 1);
        *(int *)(self + 0x110) = 0;
    }

    cond = (int)((*(int *)(self + 0xb0) & 0x400) != 0);
    if (cond != 0) {
        *(int *)(self + 0x110) = 1;
        *(void **)(self + 0x10c) = 0;
        func_ov060_021123c8(r4);
        *(int *)(r4 + 0x410) = 2;
        return;
    }

    r1 = *(char **)(self + 0x10c);
    if (r1 == 0) goto tail;

    cond = (int)(*(int *)(r1 + 0x358) != 0);
    if (cond == 0) goto do_drop;

    if (*(short *)(r1 + 0x600 + 0x9c) != 0) goto set_default;
    if (*(u16 *)(self + 0x100 + 0x16) == 0) goto tail;
    *(u16 *)(self + 0x116) -= 1;
    if (*(u16 *)(self + 0x100 + 0x16) != 0) goto tail;

do_drop:
    *(int *)(self + 0x110) = 1;
    _ZN6Player9DropActorEv(*(void **)(self + 0x10c));
    *(void **)(self + 0x10c) = 0;
    func_ov060_021123c8(r4);
    *(int *)(r4 + 0x410) = 3;
    goto tail;

set_default:
    *(u16 *)(self + 0x100 + 0x16) = 0x96;

tail:
    *(int *)(self + 0xec) |= 1;
}
