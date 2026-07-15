#pragma opt_strength_reduction off
typedef unsigned char u8;
typedef unsigned short u16;
struct Vec3 { int x, y, z; };
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int func_ov034_02112650(char *sl, char *p, char *a);
extern void _ZN6Player6BounceE5Fix12IiE(void *self, int f);
extern void func_0201267c(int a, void *b);
extern void func_ov034_021125b8(char *sl, int i);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, struct Vec3 *v, unsigned int b, int c, unsigned int d, unsigned int e, unsigned int f);
void func_ov034_02112688(char *sl)
{
    int off; int zero; int i; char *a; char *p; int one; int two; int cc; int eight;
    volatile struct Vec3 v0; struct Vec3 v; struct Vec3 hv;
    off = 0x478; p = sl + off; i = 0; zero = 0; one = 1; two = 2; cc = 0xc000; eight = 8;
    for (; i < 5; i++, p += 0x40) {
        unsigned int id; int flag; u16 type; struct Vec3 *src;
        id = *(unsigned int *)(sl + (i << 6) + 0x49c);
        if (id == 0) continue;
        a = _ZN5Actor10FindWithIDEj(id);
        if (a == 0) continue;
        type = *(u16 *)(a + 0xc);
        { int t = (int)(type == 0xbf); flag = t ? one : zero; }
        if (flag == 0) continue;
        src = (struct Vec3 *)(((long long)(int)(a + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        v0.x = src->x; v0.y = src->y; v0.z = src->z;
        if (func_ov034_02112650(sl, p, a) != 0) {
            _ZN6Player6BounceE5Fix12IiE(a, 0x28000);
            func_0201267c(0x11c, sl + 0x74);
            if ((unsigned int)i > 1) { *(u8 *)(sl + i + 0x702) = 0x1e; return; }
            *(u8 *)(sl + 0x702) = 0x1e;
            *(char **)(sl + 0x8c8) = a;
            func_ov034_021125b8(sl, 5);
            return;
        }
        if (i == 0) {
            v.x = *(int *)(sl + 0x5c); v.y = *(int *)(sl + 0x60); v.z = *(int *)(sl + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, two, cc, one, zero, one);
            func_ov034_021125b8(sl, eight);
        }
    }
    {
        int z2; int j; int o2; int z3;
        z2 = 0;
        z3 = z2;
        j = 0;
        o2 = 1;
        for (; j < 5; j++) {
            unsigned int id; int flag; u16 type;
            id = *(unsigned int *)(sl + (j << 6) + 0x5dc);
            if (id == 0) continue;
            a = _ZN5Actor10FindWithIDEj(id);
            if (a == 0) continue;
            type = *(u16 *)(a + 0xc);
            { int t = (int)(type == 0xbf); flag = t ? o2 : z2; }
            if (flag == 0) continue;
            hv.x = *(int *)(sl + 0x5c); hv.y = *(int *)(sl + 0x60); hv.z = *(int *)(sl + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &hv, z3, 0x5000, o2, z3, o2);
        }
    }
}
