typedef unsigned short u16;

extern void func_ov063_0211adfc(char *p);
extern void func_ov063_02118eac(void *c);
extern void func_ov063_02118e5c(void *c);
extern void func_ov063_02118ea0(void *c);
extern int _ZN5Actor15FindWithActorIDEjPS_(unsigned int j, int p);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern void func_0201267c(unsigned int id, const void *v);

void func_ov063_02118b98(char *c)
{
    u16 v;

    *(u16 *)(((long long)(int)(c + 0x5d4))) &= ~0x40;
    func_ov063_0211adfc(c);

    v = *(u16 *)(c + 0x100);
    if (v > 0xf) goto bigblock;
    if (v != 0xf) return;

    {
        int v2 = *(int *)(c + 0x5a4);
        if (v2 == 0) {
            func_ov063_02118eac(c);
        } else if (v2 == 1) {
            func_ov063_02118e5c(c);
        } else {
            func_ov063_02118ea0(c);
        }
    }

    *(int *)(c + 0x5c) = *(int *)(c + 0x51c);
    *(int *)(c + 0x60) = *(int *)(c + 0x520);
    *(int *)(c + 0x64) = *(int *)(c + 0x524);
    return;

bigblock:
    if (*(int *)(c + 0x5a4) != 0) goto special;
    if (v <= 0x3c) return;
    if (*(int *)(c + 0x580) >= 0x258000) return;
    {
        int r1 = 0;
        for (;;) {
            r1 = _ZN5Actor15FindWithActorIDEjPS_(0x41, r1);
            if (r1 == 0) goto after_strb;
            if (((unsigned int)*(int *)(r1 + 8) >> 8 & 3) == 0) break;
        }
        *(char *)(r1 + 0x155) = 1;
    }
after_strb:
    _ZN9ActorBase18MarkForDestructionEv(c);
    func_0201267c(0xd5, c + 0x74);
    return;

special:
    _ZN9ActorBase18MarkForDestructionEv(c);
    func_0201267c(0xd5, c + 0x74);
}
