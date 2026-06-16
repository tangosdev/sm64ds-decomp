//cpp
typedef unsigned short u16;
extern int func_ov004_020aea78(void *a, void *b, void *c, void *d);
extern "C" void _ZN5Enemy12KillByAttackER5Actor(void *self, void *actor, void *c2, void *c3)
{
    void *r8 = self;
    void *r7 = actor;
    void *r6 = c2;
    void *r5 = c3;
    u16 sentinel = 0xffff;
    do {
        func_ov004_020aea78(r8, r7, r6, r5);
        u16 v = *(u16*)((char*)r8 + 6);
        r8 = (char*)r8 + 8;
        if (v == sentinel) break;
    } while (1);
}
