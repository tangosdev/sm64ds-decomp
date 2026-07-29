typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vec3 { int x, y, z; };

extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern int func_ov060_02111c68(char *c);
extern int _ZNK9Animation13GetFrameCountEv(void *anim);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, void *pos, u32 d);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 param, void *pos, void *rot, int a, int b);

extern short data_02082214[];
extern u16 data_ov060_02119364[];

void func_ov060_0211747c(char *self)
{
    char *o;
    int n;
    int ang;
    int i;
    u16 *t;
    int s1;
    int s0;
    volatile int v[3];
    int a;
    int b;
    int *p;

    o = (char *)_ZN5Actor10FindWithIDEj(*(u32 *)(self + 0x2cc));
    if (o == 0) {
        _ZN9ActorBase18MarkForDestructionEv(self);
        return;
    }

    if (*(int *)(o + 0x40c) != 0xf)
        return;

    n = func_ov060_02111c68(o);
    if (n < 0)
        return;

    ang = *(u16 *)(o + 0x8e);
    i = (ang >> 4) * 2;
    p = (int *)(o + 0x5c);
    v[0] = p[0];
    v[1] = p[1];
    s1 = data_02082214[i + 1];
    s0 = data_02082214[i];
    v[2] = p[2];
    t = data_ov060_02119364;

    if (n == _ZNK9Animation13GetFrameCountEv(o + 0x124))
        n = 0;

    t += n * 5;

    if (*(u32 *)(self + 0x388) != 0x180)
        *(u32 *)(self + 0x384) = 0;
    *(u32 *)(self + 0x388) = 0x180;
    *(u32 *)(self + 0x384) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(u32 *)(self + 0x384), 3, *(u32 *)(self + 0x388), self + 0x74, 0);

    a = t[0];
    b = t[2] + 0x14;
    *(int *)(self + 0x5c) = v[0] + (b * s0 + a * s1);
    *(int *)(self + 0x60) = v[1] + ((t[1] - 0x5a) << 12);
    *(int *)(self + 0x64) = v[2] + (b * s1 - a * s0);
    *(short *)(self + 0x92) = (short)(t[4] + 0x818);
    *(short *)(self + 0x94) = (short)(t[3] + ang - 0x4700);

    if (n & 1)
        return;

    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x118, 5, self + 0x5c, self + 0x92,
                                                 *(signed char *)(self + 0xcc), -1);
}
