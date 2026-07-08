//cpp
typedef int Fix12i;
struct Matrix4x3 { int w[12]; };
struct ShadowModel;

extern Matrix4x3 _ZN9Matrix3x38IDENTITYE;

struct WithMeshClsn {
    virtual int v0();
};
extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(void* x);
extern "C" void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* thiz, ShadowModel& sm, Matrix4x3& mtx, Fix12i a, Fix12i b, unsigned int c);

extern "C" void func_ov062_02116dbc(char* thiz)
{
    char* c = thiz;
    *(Matrix4x3*)(c + 0x390) = _ZN9Matrix3x38IDENTITYE;
    *(int*)(c + 0x3b4) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3b8) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x3bc) = *(int*)(c + 0x64) >> 3;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144) != 0) {
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, *(ShadowModel*)(c + 0x368), *(Matrix4x3*)(c + 0x390), 0x12c000, 0x32000, 0xf);
    } else {
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, *(ShadowModel*)(c + 0x368), *(Matrix4x3*)(c + 0x390), 0x12c000, 0x3e8000, 0xf);
    }
}
