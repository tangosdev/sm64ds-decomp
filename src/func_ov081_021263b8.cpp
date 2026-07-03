//cpp
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *file, int a, int b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *thiz);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor, int r, int h, unsigned int a, unsigned int b);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int r, int h, void *v, int b);
extern "C" void func_ov081_021261d4(void *c, void *p);
extern "C" char data_ov081_02128d90;
extern "C" char data_ov081_02128eb4;

extern "C" int func_ov081_021263b8(char *c)
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128d90);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300, f, 1, -1) == 0) return 0;

    _ZN11ShadowModel12InitCylinderEv(c + 0x350);

    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x1e000, 0x1e000, 0x200004, 0x40010);

    *(int*)(c + 0x37c) = *(int*)(c + 0x5c);
    *(int*)(c + 0x380) = *(int*)(c + 0x60);
    *(int*)(c + 0x384) = *(int*)(c + 0x64);
    *(int*)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL) += 0x32000;
    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x144, c, 0x14000, 0x14000, 0, 0);

    func_ov081_021261d4(c, &data_ov081_02128eb4);
    return 1;
}
