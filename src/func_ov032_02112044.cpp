//cpp
extern "C" {
struct Vec3 { int x, y, z; };
extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, int f, int g, unsigned int h);
extern int data_020a0e68[];

typedef struct { int w[12]; } M48;

void func_ov032_02112044(char* c)
{
    Vec3 v;
    Vec3_Asr(&v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(M48*)(c + 0x368) = *(M48*)data_020a0e68;
    Matrix4x3_FromTranslation(data_020a0e68, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0x5a000) >> 3, *(int*)(c + 0x64) >> 3);
    *(M48*)(c + 0x3dc) = *(M48*)data_020a0e68;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x3b4, c + 0x3dc, 0xfa000, 0x258000, 0xf);
}
}
