extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
extern void _ZN11ShadowModel10InitCuboidEv(void *);
extern void _ZN11ShadowModel12InitCylinderEv(void *);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *, void *, int, int, void *, int);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *, void *, void *, int, int, unsigned int, unsigned int);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, void *, int, short, void *);
extern void func_ov063_0211d88c(char *);
extern void func_ov063_0211d828(char *);
extern void func_ov063_0211d5f4(char *);

extern char data_ov063_0211ef80;
extern char data_ov063_0211ef90;
extern char data_ov063_0211ef88;
extern char data_ov063_0211ecb8;

int _ZN8MadPiano13InitResourcesEv(char *self) {
    int i;
    char *p;
    void *f;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov063_0211ef80);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0x320, f, 1, -1);
    _ZN11ShadowModel10InitCuboidEv(self + 0x384);
    _ZN11ShadowModel10InitCuboidEv(self + 0x3ac);
    _ZN11ShadowModel12InitCylinderEv(self + 0x3d4);
    f = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov063_0211ef90);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x320, f, 0, 0x1000, 0);
    *(int *)(self + 0x9c) = -0x2000;
    *(int *)(self + 0xa0) = -0x3c000;
    *(int *)(self + 0x6d4) = *(int *)(self + 0x5c);
    *(int *)(self + 0x6d8) = *(int *)(self + 0x60);
    *(int *)(self + 0x6dc) = *(int *)(self + 0x64);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self + 0x50c, self, 0x159000, 0x159000, 0, 0);
    for (i = 0, p = self + 0x48c; i < 2; i++) {
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
            p, self, self + 0x5c, 0x9b000, 0xfa000, 0x200004, 0);
        p += 0x40;
    }
    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov063_0211ef88);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, f, self + 0x2ec, 0x199, *(short *)(self + 0x8e), &data_ov063_0211ecb8);
    func_ov063_0211d88c(self);
    func_ov063_0211d828(self);
    func_ov063_0211d5f4(self);
    return 1;
}
