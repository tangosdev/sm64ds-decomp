extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern int _ZNK7PathPtr8NumNodesEv(void *self);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *v, unsigned int idx);
extern int Vec3_Equal(void *a, void *b);
extern void func_ov100_0214233c(void *self);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v0, int v1);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void *self);

extern char data_ov100_02148668;
extern int data_02092138;
extern signed char data_0209f2f8;

int func_ov100_02142de0(char *o)
{
    int kind;
    int d;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(o + 0x2cc,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_02148668), 1, -1);
    if (_ZN11ShadowModel12InitCylinderEv(o + 0x31c) == 0)
        return 0;
    *(int *)(o + 0x9c) = -0x4000;
    *(int *)(o + 0xa0) = -0x46000;
    *(unsigned char *)(o + 0x3d0) = *(unsigned int *)(o + 8) & 0xf;
    *(unsigned int *)(o + 8) = *(unsigned int *)(o + 8) >> 4;
    kind = *(unsigned char *)(o + 0x3d0);

    if (kind == 2 || kind == 4) {
        _ZN7PathPtr6FromIDEj(o + 0x3f4, *(unsigned int *)(o + 8) & 0xf);
        *(int *)(o + 0x3d4) = _ZNK7PathPtr8NumNodesEv(o + 0x3f4);
        *(int *)(o + 0x3d8) = 0;
        *(int *)(o + 0x3dc) = *(int *)(o + 0x5c);
        *(int *)(o + 0x3e0) = *(int *)(o + 0x60);
        *(int *)(o + 0x3e4) = *(int *)(o + 0x64);
        _ZNK7PathPtr7GetNodeER7Vector3j(o + 0x3f4, o + 0x3e8, *(int *)(o + 0x3d8));
        if (Vec3_Equal(o + 0x5c, o + 0x3e8)) {
            *(int *)(((int)o + 0x3d8) & 0xFFFFFFFFFFFFFFFFLL) += 1;
            _ZNK7PathPtr7GetNodeER7Vector3j(o + 0x3f4, o + 0x3e8, *(int *)(o + 0x3d8));
        }
        func_ov100_0214233c(o);
        *(short *)(o + 0x94) = *(short *)(o + 0x3ba);
        *(int *)(o + 0x3c8) = *(int *)&data_02092138;
        d = *(signed char *)&data_0209f2f8;
        if (d == 0x19) {
            *(int *)(o + 0x98) = 0xa000;
            *(int *)(o + 0x3ac) = 0x800;
            *(int *)(o + 0x3b0) = 0x800;
            *(int *)(o + 0x3b4) = 0x800;
            _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(o + 0x374, o, 0x1e000, 0x1e000, 0x200004, 0x3c0);
            _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(o + 0x110, o, 0x1e000, 0x1e000, 0, 0);
            *(int *)(o + 0x3c8) = -0x640000;
        } else {
            *(int *)(o + 0x3ac) = 0x1000;
            *(int *)(o + 0x3b0) = 0x1000;
            *(int *)(o + 0x3b4) = 0x1000;
            _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(o + 0x374, o, 0x64000, 0x64000, 0x200004, 0x3c0);
            _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(o + 0x110, o, 0x64000, 0x64000, 0, 0);
            d = *(volatile signed char *)&data_0209f2f8;
            if (d == 0x18) {
                *(int *)(o + 0x98) = 0x19000;
                *(int *)(o + 0x3c8) = 0xfec78000;
            } else if (d == 0x16) {
                *(int *)(o + 0x98) = 0xa000;
                *(int *)(o + 0x3c8) = 0xff63c000;
            } else {
                if (d == 6)
                    *(int *)(o + 0x98) = 0x14000;
                else
                    *(int *)(o + 0x98) = 0xa000;
            }
        }
        _ZN5Actor9UpdatePosEP12CylinderClsn(o, 0);
        _ZN12WithMeshClsn13SetLimMovFlagEv(o + 0x110);
        goto end;
    }

    if (kind == 1) {
        *(unsigned int *)(((int)o + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        *(int *)(o + 0x3ac) = 0x1000;
        *(int *)(o + 0x3b0) = 0x1000;
        *(int *)(o + 0x3b4) = 0x1000;
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(o + 0x374, o, 0x64000, 0x64000, 0x200004, 0x3c0);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(o + 0x110, o, 0x64000, 0x64000, 0, 0);
        _ZN12WithMeshClsn13SetLimMovFlagEv(o + 0x110);
        goto end;
    }

    if (kind == 0) {
        d = *(signed char *)&data_0209f2f8;
        if (d == 0x19) {
            *(int *)(o + 0x3c0) = 0x200000;
            *(int *)(o + 0x3c4) = 0x1770000;
        } else {
            *(int *)(o + 0x3c0) = 0x400000;
            if (d == 0x18)
                *(int *)(o + 0x3c4) = 0x1b58000;
            else if (d == 6)
                *(int *)(o + 0x3c4) = 0x11f8000;
            else if (d == 0x16)
                *(int *)(o + 0x3c4) = 0x1b58000;
            else
                *(int *)(o + 0x3c4) = 0x1964000;
        }
    }

end:
    *(short *)(o + 0x100) = 0;
    *(unsigned char *)(o + 0x108) = 0;
    *(int *)(o + 0x3cc) = 0;
    *(int *)(o + 0x3a8) = 0;
    *(unsigned char *)(o + 0x3d2) = 0;
    return 1;
}
