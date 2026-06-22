// NONMATCHING: base materialization / addressing (div=5). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
struct Actor;

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
    *(int *)((char *)r + 0xb0) &= ~1;
    *(int *)((char *)r + 0xb0) &= ~0x10000000;
    return r;
}
