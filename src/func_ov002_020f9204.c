extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, int fix12, int t, unsigned int a, unsigned int b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int fix12, int t, void* vec, int last);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(void* thiz);

int func_ov002_020f9204(char* c)
{
    if (_ZN11ShadowModel12InitCylinderEv(c + 0x300) == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x28000, 0x50000, 0x200002, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x144, c, 0x32000, 0x32000, 0, 0);
    _ZN12WithMeshClsn19StartDetectingWaterEv(c + 0x144);
    *(unsigned short*)(c + 0x100) = 0;
    *(unsigned short*)(c + 0x36a) = 0;
    *(int*)(c + 0x360) = 0;
    *(int*)(c + 0x364) = 0x5dc000;
    *(unsigned char*)(c + 0x36d) = *(int*)(c + 8) & 7;
    *(int*)(c + 0x370) = 0;
    *(int*)(c + 0x374) = 0;
    {
        unsigned char v = *(unsigned char*)(c + 0x36d);
        if (v != 0 && v != 4) {
            *(unsigned int*)(((int)(c + 0x12c)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x8000;
        }
    }
    return 1;
}
