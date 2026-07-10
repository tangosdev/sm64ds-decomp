typedef int Fix12;
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, Fix12 a, int b, unsigned int c, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, Fix12 a, int b, void *v, int c);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void *self);
extern void *data_ov063_0211edec;
int _ZN7BooCage13InitResourcesEv(char *c) {
    void *m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov063_0211edec);
    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300, m, 1, -1)) return 0;
    if (!_ZN11ShadowModel12InitCylinderEv(c + 0x350)) return 0;
    *(int*)(c + 0x9c) = -0x4000;
    *(int*)(c + 0xa0) = -0x46000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x64000, 0x64000, 0x200004, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x144, c, 0x64000, 0x64000, 0, 0);
    _ZN12WithMeshClsn13SetLimMovFlagEv(c + 0x144);
    *(int*)(c + 0x378) = 0;
    *(short*)(c + 0x37c) = 0;
    *(unsigned char*)(c + 0x37e) = 0;
    return 1;
}
