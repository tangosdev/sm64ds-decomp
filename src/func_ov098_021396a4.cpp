//cpp
struct Vec3 { int x, y, z; };
struct RaycastGround { char buf[0x68 - 0x18]; };
extern "C" {
extern void _ZN13RaycastGroundC1Ev(struct RaycastGround *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RaycastGround *self, const struct Vec3 *v, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(struct RaycastGround *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mtx, int fix, int t1, int t2, unsigned int n);
extern void _ZN13RaycastGroundD1Ev(struct RaycastGround *self);
void func_ov098_021396a4(void *self);
}

void func_ov098_021396a4(void *self)
{
    char *c = (char*)self;
    struct RaycastGround rg;
    struct Vec3 v;
    int r5;
    int r4;

    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    v.y -= 0xa000;
    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v, 0);
    *(int*)(c + 0x5ec) = v.y;
    if (_ZN13RaycastGround10DetectClsnEv(&rg)) {
        *(int*)(c + 0x5ec) = *(int*)((char*)&rg + 0x44);
    }
    r5 = *(int*)(c + 0x60) - *(int*)(c + 0x5ec);
    if (r5 <= 0x1000) r5 = 0x1000;
    r4 = 0x64000 - (int)(((long long)r5 * 0x180 + 0x800) >> 12);
    if (r4 < 0xa000) r4 = 0xa000;
    Matrix4x3_FromRotationY(c + 0x530, *(short*)(c + 0x8e));
    *(int*)(c + 0x554) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x558) = (*(int*)(c + 0x60) - 0x14000) >> 3;
    *(int*)(c + 0x55c) = *(int*)(c + 0x64) >> 3;
    _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x508, c + 0x530, r4, r5 + 0x28000, r4, 0xf);
    _ZN13RaycastGroundD1Ev(&rg);
}
