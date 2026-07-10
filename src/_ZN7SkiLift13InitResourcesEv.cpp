//cpp
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *act, int a, int b, unsigned int c2, unsigned int d);
extern int *func_ov022_021123d0(int *t, int r1);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, int a, int b, void *c2, void *d);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *act);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void func_ov018_02111d28(char *c, int r1);
extern void _ZN13RaycastGroundD1Ev(void *self);
extern int data_ov036_02113c00[];
extern int data_ov018_02112c0c[];
extern int data_ov056_02112c04[];
int _ZN7SkiLift13InitResourcesEv(char *c) {
    void *m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov036_02113c00);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, m, 1, 1);
    for (int i = 0; i < 2; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov018_02112c0c[i]);
    for (int i = 0; i < 2; i++) {
        void *t = (void*)data_ov056_02112c04[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(t);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)data_ov036_02113c00[1], (void*)((int*)t)[1]);
    }
    if (_ZN11ShadowModel12InitCylinderEv(c+0x14c) == 0) return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c+0x174, c, 0x104000, 0x12c000, 0x4800004, 0x900000);
    func_ov022_021123d0((int*)c, 0);
    *(int*)(c+0x9c) = -0x2000;
    *(int*)(c+0xa0) = -0x3c000;
    *(int*)(c+0x80) = 0x1000;
    *(int*)(c+0x84) = 0x1000;
    *(int*)(c+0x88) = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x1a8, c, 0x32000, 0x32000, 0, 0);
    char rg[0x54];
    int v[3];
    v[0] = *(int*)(c+0x5c);
    v[1] = *(int*)(c+0x60);
    v[2] = *(int*)(c+0x64);
    v[1] += 0x14000;
    _ZN13RaycastGroundC1Ev(rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, v, 0);
    if (_ZN13RaycastGround10DetectClsnEv(rg))
        *(int*)(c+0x60) = *(int*)(rg+0x44);
    else
        *(int*)(c+0x60) = v[1];
    *(int*)(c+0x364) = *(int*)(c+0x5c);
    *(int*)(c+0x368) = *(int*)(c+0x60);
    *(int*)(c+0x36c) = *(int*)(c+0x64);
    *(int*)(c+0x374) = 0;
    func_ov018_02111d28(c, 0);
    _ZN13RaycastGroundD1Ev(rg);
    return 1;
}
}
