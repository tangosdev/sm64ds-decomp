struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
struct Actor;

extern void func_02005ed8(unsigned char *t);
extern void Vec3_Add(struct Vector3 *out, const struct Vector3 *a, const struct Vector3 *b);
extern struct Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int a1, unsigned int a2, const struct Vector3 *a3,
    const struct Vector3_16 *a4, int a5, int a6);

struct Actor *_ZN8CapEnemy10ReleaseCapERK7Vector3(unsigned char *c, const struct Vector3 *v)
{
    struct Actor *ret = 0;

    if ((c[0x113] & 7) < 6) {
        func_02005ed8(c);
        if (c[0x113] < 6u) {
            struct Vector3 out;
            Vec3_Add(&out, (const struct Vector3 *)(c + 0x5c), v);
            ret = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x10d, 0x1012 | (c[0x113] << 8), &out,
                (const struct Vector3_16 *)(c + 0x8c),
                *(signed char *)(c + 0xcc), -1);
            if (c[0x110] != 0) {
                *(unsigned char *)(((int)c + 0x113) & 0xFFFFFFFFFFFFFFFFull) |= 8;
            } else {
                *(unsigned char *)(((int)c + 0x113) & 0xFFFFFFFFFFFFFFFFull) |= 0x80;
            }
        } else {
            ret = (struct Actor *)c;
        }
    }
    return ret;
}
