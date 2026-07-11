//cpp
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
struct Actor;

extern "C" {
extern void func_02005ed8(unsigned char *t);
extern struct Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    unsigned int a1, unsigned int a2, const struct Vector3 *a3,
    const struct Vector3_16 *a4, int a5, int a6);

struct Actor *_ZN8CapEnemy15RespawnIfHasCapEv(unsigned char *c)
{
    struct Actor *r;
    func_02005ed8(c);
    if ((c[0x113] & 0xf) >= 6) return 0;
    r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        *(unsigned short *)(c + 0xc), *(unsigned int *)(c + 8),
        (const struct Vector3 *)(c + 0x5c),
        (const struct Vector3_16 *)(c + 0x8c),
        *(signed char *)(c + 0xcc), -1);
    if (!r) return r;
    *(unsigned char *)((char *)r + 0x111) = 1;
    *(int *)((char *)r + 0xf4) = *(int *)((char *)r + 0xb0);
    *(unsigned char *)((char *)r + 0x108) = 0;
    {
        int *p = (int *)((unsigned long long)((char *)r + 0xb0) & 0xFFFFFFFFFFFFFFFFULL);
        *p &= ~1;
        *p &= ~0x10000000;
    }
    return r;
}
}
