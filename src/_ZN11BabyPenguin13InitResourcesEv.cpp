//cpp
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *file, int a, int b);
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *thiz);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor, int r, int h, unsigned int a, unsigned int b);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int r, int h, void *v, int b);
extern "C" void func_ov072_02121d50(void *c);
extern "C" void func_ov072_021210c4(void *c);
extern "C" char data_ov072_02122cb4;
extern "C" int data_ov072_02122004[];

extern "C" int _ZN11BabyPenguin13InitResourcesEv(char *c)
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov072_02122cb4);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
    int i;
    for (i = 0; i < 5; i++) {
        _ZN9Animation8LoadFileER13SharedFilePtr((void *)data_ov072_02122004[i]);
    }
    if (_ZN11ShadowModel12InitCylinderEv(c + 0x138) == 0) return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x160, c, 0x28000, 0x50000, 0x800004, 0x9000);
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x350) = *(int*)(c + 0x5c);
    *(int*)(c + 0x354) = *(int*)(c + 0x60);
    *(int*)(c + 0x358) = *(int*)(c + 0x64);
    *(int*)(c + 0x80) = 0x400;
    *(int*)(c + 0x84) = 0x400;
    *(int*)(c + 0x88) = 0x400;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x194, c, 0x32000, 0x32000, 0, 0);
    *(int*)(c + 0xd0) = 0;
    *(int*)(c + 0x360) = 0;
    func_ov072_02121d50(c);
    func_ov072_021210c4(c);
    return 1;
}
