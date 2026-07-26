typedef unsigned char u8;
typedef signed char s8;

typedef struct { int x, y, z; } Vector3;

extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *o);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3 *pos);
extern int GiveRedCoins(int i, int amt);
extern s8 NumRedCoins(void);
extern void _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(void *o, const Vector3 *v,
                                                    unsigned int n, int b,
                                                    unsigned short t, void *a);
extern unsigned int func_02012790(unsigned int a);
extern void GiveCoins(int idx, int amount);
extern void _ZN6Player4HealEi(void *p, int amt);
extern void *func_ov002_020b1328(void *r5);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int id, unsigned int b,
                                                          const Vector3 *pos, const void *r,
                                                          int e, int f);
extern void _ZN9PowerStar13AddStarMarkerEv(void *o);

#define LAUNDER(p) ((int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov002_020b16c4(void *cc, void *pp)
{
    char *c = (char *)cc;
    char *pl = (char *)pp;
    volatile Vector3 a;
    Vector3 w;
    Vector3 b;
    char *ps;
    char *st;
    Vector3 *q;
    int x, y, z;
    u8 v;

    *(short *)(c + 0x3a8) = 0;
    _ZN5Actor24KillAndTrackInDeathTableEv(c);
    if (*(u8 *)(pl + 0x706) != 0)
        _ZN5Sound9PlayBank3EjRK7Vector3(0x12, (const Vector3 *)(c + 0x74));
    else
        _ZN5Sound9PlayBank3EjRK7Vector3(0x11, (const Vector3 *)(c + 0x74));

    v = *(u8 *)(c + 0x3ab);
    if (v != 0 && v <= 7) {
        GiveRedCoins(*(u8 *)(pl + 0x6d8), 1);
        x = *(int *)(c + 0x5c);
        a.x = x;
        y = *(int *)(c + 0x60);
        a.y = y;
        z = *(int *)(c + 0x64);
        a.z = z;
        y += 0x64000;
        a.y = y;
        b.x = x;
        b.y = y;
        b.z = z;
        _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(c, &b, NumRedCoins(), 0, 0, 0);
        func_02012790(NumRedCoins() + 0x2f);
    }

    GiveCoins(*(u8 *)(pl + 0x6d8), 2);
    _ZN6Player4HealEi(pl, 0x200);
    if (NumRedCoins() != 8) return;
    ps = (char *)func_ov002_020b1328(c);
    if (ps == 0) return;
    q = (Vector3 *)LAUNDER(ps + 0x5c);
    x = q->x;
    *(volatile int *)&w.x = x;
    y = q->y;
    *(volatile int *)&w.y = y;
    z = q->z;
    *(volatile int *)&w.z = z;
    y += 0x78000;
    *(volatile int *)&w.y = y;
    st = (char *)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0xb2, *(u8 *)(c + 0x3ab) | 0x40, &w, 0, *(s8 *)(ps + 0xcc), -1);
    if (st == 0) return;
    if (*(s8 *)(c + 0xcc) != *(s8 *)(st + 0xcc))
        *(s8 *)(st + 0xcc) = -1;
    _ZN9PowerStar13AddStarMarkerEv(st);
    *(unsigned short *)LAUNDER(st + 0x4a2) |= 0x1000;
    *(int *)(st + 0x434) = *(int *)(ps + 4);
    *(u8 *)LAUNDER(ps + 0x1db) |= 4;
}
