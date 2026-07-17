//cpp
struct Vec3 { int x, y, z; };
struct RaycastGround { char buf[0x50]; };
extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
extern void _ZN13RaycastGroundC1Ev(struct RaycastGround* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RaycastGround* self, const struct Vec3* v, void* actor);
extern int _ZN13RaycastGround10DetectClsnEv(struct RaycastGround* self);
extern void _ZN13RaycastGroundD1Ev(struct RaycastGround* self);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* shadow, void* mtx, int height, int rad, unsigned int x);
void func_ov002_020af4ec(void* self);
}

void func_ov002_020af4ec(void* self)
{
    int rad;
    char* c = (char*)self;
    int height;
    struct RaycastGround rg;
    struct Vec3 v2;
    struct Vec3 v1;

    if ((unsigned)(*(int*)(c + 0x384) - 0xb) <= 1) {
        Matrix4x3_FromRotationY(c + 0x31c, *(short*)(c + 0x8e));
        *(int*)(c + 0x340) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x344) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x348) = *(int*)(c + 0x64) >> 3;
    } else {
        Vec3_Asr(&v1, (struct Vec3*)(c + 0x5c), 3);
        Matrix4x3_FromTranslation(c + 0x31c, v1.x, v1.y, v1.z);
    }

    if (*(unsigned char*)(c + 0x38e) == 0) return;

    if ((unsigned)(*(int*)(c + 0x384) - 0xb) <= 1) {
        height = 0x50000;
        rad = 0x50000;
    } else if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144)) {
        int y = *(int*)(c + 0x60);
        int z = *(int*)(c + 0x64);
        int adjustedY;
        int x = *(int*)(c + 0x5c);
        adjustedY = y + 0x28000;
        v2.x = x;
        v2.y = adjustedY;
        v2.z = z;
        _ZN13RaycastGroundC1Ev(&rg);
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v2, 0);
        rad = v2.y;
        if (_ZN13RaycastGround10DetectClsnEv(&rg)) {
            rad = *(int*)((char*)&rg + 0x44);
        }
        rad = *(int*)(c + 0x60) - rad;
        if (rad <= 0x1000) rad = 0x1000;
        height = (*(int*)(c + 0x114) - 0xa000) * 2 - (int)(((long long)rad * 0x180 + 0x800) >> 12);
        if (height < 0xa000) height = 0xa000;
        rad += 0x3c000;
        _ZN13RaycastGroundD1Ev(&rg);
    } else {
        rad = 0x3c000;
        height = (*(int*)(c + 0x114) - 0xa000) * 2;
    }

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x350, c + 0x31c, height, rad, 0xf);
}
