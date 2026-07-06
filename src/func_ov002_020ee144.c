typedef int Fix12;

extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern int  _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int  _ZN11ShadowModel12InitCylinderEv(void *self);
extern int  _ZN11ShadowModel10InitCuboidEv(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int n, int speed, unsigned int flags);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, int r, int h, unsigned int d, unsigned int e);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int r, int h, void *p, int q);
extern unsigned char _ZN5Actor9TrackStarEjj(void *self, unsigned int a, unsigned int b);
extern int  func_ov002_020ec654(void *p);
extern int  func_ov002_020ec628(void *p);
extern int  func_ov002_020ec610(void *p);
extern void LoadBlueCoinModel(void *p);

extern char data_ov002_0210e6b0[];
extern char data_ov002_0210eb78[];
extern char *data_ov002_021000a0[];

#pragma opt_strength_reduction off

int func_ov002_020ee144(void *c)
{
    char *o = (char *)c;
    int idx;
    int i;

    *(unsigned char *)(o + 0x428) = (unsigned char)((unsigned int)*(int *)(o + 8) >> 4);
    idx = 0;
    if (func_ov002_020ec654(c) != 0)
        idx = 1;
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e6b0);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210eb78);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(o + 0x300,
            *(void **)(data_ov002_021000a0[idx] + 4), 1, -1) == 0)
        return 0;

    if (func_ov002_020ec654(c) == 0) {
        if (_ZN11ShadowModel12InitCylinderEv(o + 0x364) == 0)
            return 0;
    } else {
        if (_ZN11ShadowModel10InitCuboidEv(o + 0x364) == 0)
            return 0;
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(o + 0x300,
        *(void **)(data_ov002_0210eb78 + 4), 0, 0x1000, 0);

    *(char *)(o + 0xcc) = -1;
    *(int *)(o + 0x3c0) = *(int *)(o + 0x5c);
    *(int *)(o + 0x3c4) = *(int *)(o + 0x60);
    *(int *)(o + 0x3c8) = *(int *)(o + 0x64);
    *(int *)(o + 0x9c) = -0x2000;
    *(int *)(o + 0xa0) = -0x3c000;
    *(int *)(o + 0x3f0) = *(int *)(o + 8) & 3;

    switch (*(int *)(o + 0x3f0)) {
    case 0:
    case 1:
        *(int *)(o + 0x80) = 0x1000;
        *(int *)(o + 0x84) = 0x1000;
        *(int *)(o + 0x88) = 0x1000;
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(o + 0x110, c, 0x46000, 0x8c000, 0x200002, 0xa08000);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(o + 0x144, c, 0x28000, 0x28000, 0, 0);
        break;
    case 2:
        *(int *)(o + 0x80) = 0x2000;
        *(int *)(o + 0x84) = 0x2000;
        *(int *)(o + 0x88) = 0x2000;
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(o + 0x110, c, 0x78000, 0xa0000, 0x200002, 0xa08000);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(o + 0x144, c, 0x64000, 0x64000, 0, 0);
        break;
    default:
        break;
    }

    for (i = 0; i < 5; i++)
        *(int *)(o + i * 4 + 0x3fc) = 0;

    *(short *)(o + 0x3ea) = *(short *)(o + 0x8c);
    *(short *)(o + 0x3ec) = *(short *)(o + 0x8e);
    *(short *)(o + 0x3ee) = *(short *)(o + 0x90);
    *(short *)(o + 0x3e4) = *(short *)(o + 0x8c);
    *(short *)(o + 0x3e6) = *(short *)(o + 0x8e);
    *(short *)(o + 0x3e8) = *(short *)(o + 0x90);
    *(unsigned char *)(o + 0x41f) = 0xf;

    if (func_ov002_020ec628(c) != 0)
        LoadBlueCoinModel(c);
    if (func_ov002_020ec610(c) != 0)
        *(unsigned char *)(o + 0x427) = _ZN5Actor9TrackStarEjj(c, 0, 1);
    return 1;
}
