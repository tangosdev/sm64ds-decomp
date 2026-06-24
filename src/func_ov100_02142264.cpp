//cpp
extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* thiz);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, int f, int g, unsigned int h);
extern int data_02082128[];

typedef struct { int w[12]; } M48;

void func_ov100_02142264(char* c)
{
    *(M48*)(c + 0x2e8) = *(M48*)data_02082128;
    *(int*)(c + 0x30c) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x310) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x314) = *(int*)(c + 0x64) >> 3;
    *(M48*)(c + 0x344) = *(M48*)(c + 0x2e8);
    int k = 0x32;
    *(int*)(c + 0x36c) = (*(int*)(c + 0x3ac) * k + *(int*)(c + 0x60)) >> 3;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110) != 0)
        k += 0x28;
    else
        k += 0x190;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x31c, c + 0x344, *(int*)(c + 0x3ac) * 0xc8, k * *(int*)(c + 0x3ac), 0xf);
}
}
