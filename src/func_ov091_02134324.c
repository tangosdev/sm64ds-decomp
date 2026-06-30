extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor, int fix, int t, unsigned int a, unsigned int b);
extern void func_ov091_02134044(void *c, void *p);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int fix, int t, void *v, int t2);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *f, int a, int b);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *f);

extern char data_ov091_021356c0[];
extern char data_ov002_0210da40[];
extern char data_ov002_0210d9a0[];
extern char data_ov002_0210d9c0[];
extern char data_ov091_02135674[];
extern char data_ov091_0213567c[];
extern char data_ov091_02135684[];
extern char data_ov091_021356d0[];

int func_ov091_02134324(char *c)
{
    *(int *)(c + 0x374) = *(int *)(c + 8) & 0xff;
    *(int *)(c + 0xa0) = -0x1e000;
    if (*(int *)(c + 0x374) == 0xff)
        *(int *)(c + 0x374) = 0;
    if (*(int *)(c + 0x374) == 1) {
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x3c000, 0x14000, 0x200002, 0);
        func_ov091_02134044(c, data_ov091_021356c0);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x144, c, 0x14000, 0, 0, 0);
        *(int *)(c + 0xb0) = 0;
        return 1;
    }
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x64000, 0x64000, 0x200002, 0x4a010);
    *(int *)(c + 0xb0) = 0x10000000;
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da40);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a0);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9c0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300, _ZN5Model8LoadFileER13SharedFilePtr(data_ov091_02135674), 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov091_0213567c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov091_02135684);
    *(int *)(c + 0x35c) = 0x1000;
    func_ov091_02134044(c, data_ov091_021356d0);
    return 1;
}
