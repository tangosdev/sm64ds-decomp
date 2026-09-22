//cpp
typedef int Fix12i;
typedef short s16;
struct Matrix4x3;
struct ShadowModel;

extern "C" void Matrix4x3_FromRotationY(void* m, int angle);

extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* thiz, ShadowModel& sm, Matrix4x3& mtx, Fix12i a, Fix12i b, unsigned int c);

extern "C" void func_ov081_02123b20(char* thiz)
{
    char* c = thiz;
    Matrix4x3_FromRotationY(c + 0x12c, *(short*)(c + 0x8e));
    *(int*)(c + 0x150) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x154) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x158) = *(int*)(c + 0x64) >> 3;
    {
        int b = (int)((*(int*)(c + 0xb0) & 0x40000) != 0);
        if (b != 0) return;
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1d0) != 0) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, *(ShadowModel*)(c + 0x174), *(Matrix4x3*)(c + 0x12c), 0x50000, 0x1e000, 0xf);
    } else {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, *(ShadowModel*)(c + 0x174), *(Matrix4x3*)(c + 0x12c), 0x50000, 0x96000, 0xf);
    }
}
