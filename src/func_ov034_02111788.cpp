//cpp
struct Vec3 { int x, y, z; };
struct Vec3D { int x, y, z; ~Vec3D() {} };

extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void _ZN5Actor10PoofDustAtERK7Vector3(void *self, const Vec3 *v);
extern "C" int Math_Function_0203b14c(void *base, int a, int b, int c, int d);
extern void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(void *self, signed char *id, unsigned int starID, const Vec3 *pos, unsigned int how);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern "C" void func_ov034_021125b8(void *c, int i);

extern "C" int data_ov034_021138c4[];

extern "C" void func_ov034_02111788(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    Vec3D starPos;
    Vec3 v;
    int a, b;
    int i;

    if (DecIfAbove0_Byte(c + 0x8da) == 0) {
        unsigned int idx = *(unsigned char *)(c + 0x8dd);
        if (idx < 5) {
            Vec3 *vp = (Vec3 *)(c + 0x3cc + idx * 0xc);
            v.x = vp->x;
            v.y = vp->y;
            v.z = vp->z;
            _ZN5Actor10PoofDustAtERK7Vector3(c, &v);
            (*(unsigned char *)(((int)c + 0x8dd) & 0xFFFFFFFFFFFFFFFF))++;
            *(unsigned char *)(c + 0x8da) = 5;
        }
    }

    {
        int *p8 = (int *)c;
        int *p7 = (int *)(c + 0x464);
        int *p6 = (int *)(c + 0x408);
        for (i = 0; i < 5; i++) {
            if (i > (int)*(unsigned char *)(c + 0x8dd)) break;
            p8[0x104] = p8[0x102];
            p8[0x103] = p8[0x104];
            a = Math_Function_0203b14c(p7, data_ov034_021138c4[i] >> 1, 0x800, 0x20000, 0x200) & 0xff;
            b = Math_Function_0203b14c(p6, 0x800, 0x800, 0x2000, 0x200) & 0xff;
            p8 = (int *)((char *)p8 + 0xc);
            p7 = p7 + 1;
            p6 = (int *)((char *)p6 + 0xc);
        }
    }

    if (a != 0) return;
    if (b != 0) return;

    if (*(unsigned char *)(c + 0x8dc) < 5) {
        (*(unsigned char *)(((int)c + 0x8dc) & 0xFFFFFFFFFFFFFFFF))++;
        return;
    }

    {
        int px = *(int *)(c + 0x5c);
        int pz = *(int *)(c + 0x64);
        int py = *(int *)(c + 0x60) + 0x64000;
        starPos.x = px;
        starPos.y = py;
        starPos.z = pz;
    }
    _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(c, (signed char *)(c + 0x8e3), *(unsigned char *)(c + 0x8e2), (const Vec3 *)(c + 0x5c), 4);
    _ZN5Sound22StopLoadedMusic_Layer3Ev();
    func_ov034_021125b8(c, 3);
}
