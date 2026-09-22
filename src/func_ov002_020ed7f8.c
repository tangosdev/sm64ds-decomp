typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct dBgCh_Gnd { char buf[0x68 - 0x18]; } dBgCh_Gnd;

extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *self, const Vec3 *v, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern int func_ov002_020cf700(void *g);
extern int func_ov002_020d0d2c(void *g);
extern int func_ov002_020ec654(unsigned char *p);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *shadow, void *mtx, int fix, int t, unsigned int n);
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mtx, int fix, int t1, int t2, unsigned int n);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *self);

void func_ov002_020ed7f8(void *self);

void func_ov002_020ed7f8(void *self)
{
    char *c = (char*)self;
    dBgCh_Gnd rg;
    Vec3 v;
    int r5;
    int r4;
    int b;

    if (*(unsigned char*)(*(char**)(c + 0x38c) + 0x6f5) < 1)
        return;

    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    v.y += 0x28000;
    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, 0);
    r4 = v.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg))
        r4 = *(int*)((char*)&rg + 0x44);
    r5 = *(int*)(c + 0x60) - r4;
    if (r5 <= 0x1000) r5 = 0x1000;
    r4 = 0x50000 - (int)(((long long)r5 * 0x180 + 0x800) >> 12);
    if (r4 < 0xa000) r4 = 0xa000;
    Matrix4x3_FromRotationY(c + 0x390, *(short*)(c + 0x8e));
    *(int*)(c + 0x3b4) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3b8) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x3bc) = *(int*)(c + 0x64) >> 3;
    b = (*(int*)(c + 0xb0) & 0x40000) ? 1 : 0;
    if (b == 0
        && *(int*)(*(char**)(c + 0x38c) + 0x37c) == 0
        && !func_ov002_020cf700(*(void**)(c + 0x38c))
        && !func_ov002_020d0d2c(*(void**)(c + 0x38c)))
    {
        if (func_ov002_020ec654((unsigned char*)c) == 0) {
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x364, c + 0x390, r4, r5 + 0x28000, 0xf);
        } else {
            _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
                c, c + 0x364, c + 0x390, r4, r5 + 0x28000, r4, 0xf);
        }
    }
    _ZN9dBgCh_GndD1Ev(&rg);
}
