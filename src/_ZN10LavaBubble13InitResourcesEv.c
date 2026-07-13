extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(char* thiz, char* actor, int b, int d, unsigned int e, unsigned int f);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* thiz, char* actor, int b, int d, void* v, int f);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(char* thiz);
extern void func_ov064_021187ec(char* c, void* p);
extern char data_ov064_0211c7c8[];
extern char data_ov064_0211c7b8[];

int _ZN10LavaBubble13InitResourcesEv(char* c)
{
    *(unsigned char*)(c + 0x310) = (*(int*)(c + 8) & 1) ^ 1;
    if (*(unsigned char*)(c + 0x310) == 0) {
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0, 0, 1, 0);
    } else {
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x32000, 0x50000, 0x200002, 0x8000);
    }
    *(int*)(c + 0x304) = *(int*)(c + 0x5c);
    *(int*)(c + 0x308) = *(int*)(c + 0x60);
    *(int*)(c + 0x30c) = *(int*)(c + 0x64);
    if (*(unsigned char*)(c + 0x310) == 0) {
        *(int *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
    } else {
        *(int*)(c + 0x9c) = -0x4000;
        *(int*)(c + 0xa0) = -0x3c000;
    }
    *(short*)(c + 0x100) = 0;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x144, c, 0x32000, 0x32000, 0, 0);
    _ZN12WithMeshClsn13SetLimMovFlagEv(c + 0x144);
    if (*(unsigned char*)(c + 0x310) != 0) {
        func_ov064_021187ec(c, data_ov064_0211c7c8);
    } else {
        func_ov064_021187ec(c, data_ov064_0211c7b8);
    }
    return 1;
}
