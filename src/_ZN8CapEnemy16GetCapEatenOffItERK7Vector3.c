struct Vector3 { int x, y, z; };
struct Actor;

extern struct Actor *_ZN8CapEnemy10ReleaseCapERK7Vector3(unsigned char *c, const struct Vector3 *v);
extern void func_ov001_020ab228(unsigned char *a, unsigned char *b, unsigned char idx, int e, unsigned char f);

int _ZN8CapEnemy16GetCapEatenOffItERK7Vector3(unsigned char *c, const struct Vector3 *v)
{
    struct Vector3 local;
    unsigned char *p;
    local.x = v->x;
    local.y = v->y;
    local.z = v->z;
    p = (unsigned char *)_ZN8CapEnemy10ReleaseCapERK7Vector3(c, &local);
    if (p != 0 && p != c) {
        unsigned char idx;
        *(int *)(p + 0xd0) = *(int *)(c + 0xd0);
        *(unsigned char **)(*(unsigned char **)(c + 0xd0) + 0x360) = p;
        *(int *)(((int)p + 0xb0)) |= 0x20000;
        *(int *)(((int)c + 0xb0)) &= ~0x20000;
        *(int *)(c + 0xd0) = 0;
        if (c[0x110] == 0) {
            idx = c[0x113] & 7;
            func_ov001_020ab228(c + 0x164, c, idx, 0, 0);
        }
        return 1;
    }
    return 0;
}
