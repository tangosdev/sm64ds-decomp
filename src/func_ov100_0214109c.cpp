//cpp
typedef int Fix12;
struct Vector3 { int x,y,z; };
struct Vector3_16 { short x,y,z; };
extern "C" {
extern int Actor_Spawn(unsigned a, unsigned b, const struct Vector3 *pos, const struct Vector3_16 *rot, int i, int j);
extern void ActorBase_MarkForDestruction(void *c);
extern void Actor_SetRanges(void *c, Fix12 a, Fix12 b, Fix12 d, Fix12 e);

void func_ov100_0214109c(void *t) {
    char *c = (char*)t;
    int st = *(int*)(c+0x3e8);
    if (st == 0x14) {
        if (*(unsigned char*)(c+0x3f0) != 1) {
            *(int*)(c+0x3e0) = 0;
            *(unsigned char*)(c+0x3f1) = 0;
            return;
        }
        int sub = *(signed char*)(c+0xcc);
        Actor_Spawn(0x114, 0, (struct Vector3*)(c+0x5c), 0, sub, -1);
        ActorBase_MarkForDestruction(c);
        return;
    }
    if (st <= 0x14) return;
    (*(int *)(((int)c + 0x3e0) & 0xFFFFFFFFFFFFFFFF)) += 0x40;
    if (*(int*)(c+0x3e0) < 0x800) return;
    Actor_SetRanges(c, 0x32000, 0x32000, 0x1000000, 0x320000);
    *(short*)(c+0x3ee) = 0;
    *(int*)(c+0x3e8) = 0;
    *(int*)(c+0x3e4) = 7;
}
}
