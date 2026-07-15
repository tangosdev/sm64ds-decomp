//cpp
typedef short s16;
typedef long long s64;
struct Vector3 { int x, y, z; };
struct Mtx43 { int a[12]; };
extern "C" {
void Matrix4x3_ApplyInPlaceToTranslation(Mtx43* m, int x, int y, int z);
void Vec3_LslInPlace(Vector3* v, int sh);
void Matrix4x3_FromRotationXYZExt(void* m, int x, int y, int z);
void Matrix4x3_FromRotationY(void* m, int angle);
void _ZN13RaycastGroundC1Ev(void* self);
void _ZN13RaycastGroundD1Ev(void* self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* self, const Vector3* pos, void* actor);
int _ZN13RaycastGround10DetectClsnEv(void* self);
void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void* self, void* sm, void* mtx, int r, int t5, int t6, unsigned int u);
}
extern Mtx43 data_020a0e68;
extern s16 data_02082214[];
extern "C" void func_ov080_02124c3c(char* c);
void func_ov080_02124c3c(char* c)
{
    Vector3 t;
    Vector3 pos;
    char rg[0x54];
    int flags = *(int*)(c + 0xb0);
    int gb = (flags & 0x40000) != 0;
    if (gb != false) return;
    void* p = *(void**)(c + 0x374);
    if (p != 0) {
        int gb2 = (flags & 0x4000) != 0;
        if (gb2 != false) {
            if (*(int*)((char*)p + 0xc8) != 0) {

                int tx = 0xc000, ty = 0x2000, tz = 0;
                ((int*)&t)[0] = tx; ((int*)&t)[1] = ty; ((int*)&t)[2] = tz;
                p = *(void**)(c + 0x374);
                data_020a0e68 = *(Mtx43*)(*(void**)((char*)p + 0xc8));
                Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int*)&t)[0], ((int*)&t)[1], ((int*)&t)[2]);

                *(int*)(c + 0x5c) = data_020a0e68.a[9];
                *(int*)(c + 0x60) = data_020a0e68.a[10];
                *(int*)(c + 0x64) = data_020a0e68.a[11];
                Vec3_LslInPlace((Vector3*)(c + 0x5c), 3);
            }
        }
    }
    Matrix4x3_FromRotationXYZExt(c + 0xf0, *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));
    *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;
    pos.x = *(int*)(c + 0x5c);
    pos.y = *(int*)(c + 0x60);
    pos.z = *(int*)(c + 0x64);
    pos.y = pos.y + 0x14000;
    _ZN13RaycastGroundC1Ev(rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &pos, 0);
    int h = pos.y;
    if (_ZN13RaycastGround10DetectClsnEv(rg)) {
        h = *(int*)(rg + 0x44);
    }
    Matrix4x3_FromRotationY(c + 0x33c, *(s16*)(c + 0x8e));
    *(int*)(c + 0x360) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x364) = h >> 3;
    *(int*)(c + 0x368) = *(int*)(c + 0x64) >> 3;
    {
        s16 a = *(s16*)(c + 0x8c);
        int sv = data_02082214[((unsigned short)(short)(a << 1) >> 4) * 2];
        if (sv < 0) sv = -sv;
        int result = (int)(((s64)sv * 0x28000 + 0x800) >> 12);
        _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            c, c + 0x124, c + 0x33c, 0x96000, 0x32000, result + 0x96000, 0xf);
    }
    _ZN13RaycastGroundD1Ev(rg);
}
