//cpp
struct M48 { int w[12]; };
extern "C" {
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, int fix12, int t, unsigned int a, unsigned int b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int fix12, int t, void* vec, int last);
extern void func_ov070_02122044(void* c, int a);
extern int _ZN9Matrix3x38IDENTITYE[];

int func_ov070_02122124(char* c)
{
    if (_ZN11ShadowModel12InitCylinderEv(c + 0xd4) == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0xfc, c, 0x37000, 0x78000, 0x200002, 0x8000);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x130, c, 0x32000, 0x32000, 0, 0);
    *(int*)(c + 0x9c) = -0x400;
    *(int*)(c + 0xa0) = -0x5000;
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;
    func_ov070_02122044(c, 0);
    *(struct M48*)(c + 0x2ec) = *(struct M48*)_ZN9Matrix3x38IDENTITYE;
    return 1;
}
}
