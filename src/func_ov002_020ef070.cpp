//cpp
typedef struct { int x, y, z; } Vector3;
typedef struct {
    void *tag;
    int f04, f08, f0c, f10, f14;
    unsigned short f18, f1a;
    int f1c, f20, f24;
} ClsnResultTmp;

extern "C" {
extern void _ZN11RaycastLineC1Ev(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, void *a, void *b, void *act);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void func_ov002_020d8838(void *actor);
extern unsigned _ZNK10ClsnResult9GetClsnIDEv(void *self);
extern void *_ZN5Actor10FindWithIDEj(unsigned id);
extern void _ZN10ClsnResultD1Ev(void *self);

extern int data_02099368;
extern short data_02082214[];
}

extern "C" int func_ov002_020ef070(void *unused, char *actor)
{
    Vector3 v1, v2;
    ClsnResultTmp tmp;
    char rl[0x78];

    _ZN11RaycastLineC1Ev(rl);

    Vector3 *pos = (Vector3 *)(((long long)(int)(actor + 0x5c)) & 0xffffffffffffffffLL);
    int x = pos->x;
    v1.x = x;
    int y = pos->y;
    v1.y = y;
    int z = pos->z;
    v2.x = x;
    v2.z = z;
    v1.z = z;
    v1.y = y + 0x3c000;
    v2.y = y + 0x3c000;

    int scale = 0x64;
    if (*(int *)(actor + 8) == 2) scale = 0x82;
    v2.x = scale * data_02082214[(*(unsigned short *)(actor + 0x8e) >> 4) << 1] + v2.x;
    v2.z = scale * data_02082214[((*(unsigned short *)(actor + 0x8e) >> 4) << 1) + 1] + v2.z;

    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &v1, &v2, actor);
    if (_ZN11RaycastLine10DetectClsnEv(rl)) {
        int t = (*(unsigned short *)(actor + 0xc) == 0xbf);
        if (t != false) {
            func_ov002_020d8838(actor);
        }
        {
            int *dst = &tmp.f04;
            int w0 = *(int *)(rl + 0x14);
            int w1 = *(int *)(rl + 0x18);
            dst[0] = w1 ? w0 : w0;
            dst[1] = w1;
            dst[2] = *(int *)(rl + 0x1c);
            dst[3] = *(int *)(rl + 0x20);
            dst[4] = *(int *)(rl + 0x24);
            tmp.tag = &data_02099368;
            tmp.f18 = *(unsigned short *)(rl + 0x28);
            tmp.f1a = *(unsigned short *)(rl + 0x2a);
            tmp.f1c = *(int *)(rl + 0x2c);
            tmp.f20 = *(int *)(rl + 0x30);
            tmp.f24 = *(int *)(rl + 0x34);
            if (_ZNK10ClsnResult9GetClsnIDEv(&tmp) != 0xffffffff) {
                void *a = _ZN5Actor10FindWithIDEj(_ZNK10ClsnResult9GetClsnIDEv(&tmp));
                if (a) {
                    (*(void (**)(void *, char *))(*(int *)a + 0x5c))(a, actor);
                    _ZN10ClsnResultD1Ev(&tmp);
                    _ZN11RaycastLineD1Ev(rl);
                    return 1;
                }
            }
            _ZN10ClsnResultD1Ev(&tmp);
        }
    }
    _ZN11RaycastLineD1Ev(rl);
    return 0;
}
