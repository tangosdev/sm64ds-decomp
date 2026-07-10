typedef struct { int x, y, z; } Vec3;
typedef struct { int m[12]; } Mtx43;

#define LA(p) (((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL)

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *, void *, int, int, void *, int);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void *);
extern void _ZN7PathPtr6FromIDEj(void *, unsigned int);
extern int _ZNK7PathPtr8NumNodesEv(void *);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *, void *, unsigned int);
extern void func_ov092_021313b0(void *);
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void func_ov092_02131a88(char *self);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, void *, int, short, void *);
extern void func_020393d4(void *p, void *v);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *, void *, void *, int, int, unsigned int, unsigned int);

extern char data_ov092_02132540;
extern char data_ov092_02132548;
extern char data_ov092_02132220;
extern char data_ov092_02132294;
extern Mtx43 data_020a0e68;
extern char _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;

int _ZN6ToxBox13InitResourcesEv(char *self)
{
    void *f;
    Vec3 tmp;
    unsigned int idx;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov092_02132540);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, f, 1, -1);

    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self + 0x324, self, 0xfa000, 0, 0, 0);
    _ZN12WithMeshClsn13SetLimMovFlagEv(self + 0x324);

    idx = *(int *)(self + 8) & 3;
    *(unsigned char *)(self + 0x574) = (unsigned char)idx;
    idx = *(unsigned char *)(self + 0x574);
    if (idx != 3) {
        int v = ((int *)&data_ov092_02132294)[idx];
        *(int *)(self + 0x56c) = v;
        *(int *)(self + 0x570) = 0;
        *(int *)(self + 0x568) = *(int *)(*(int *)(self + 0x56c));
    } else {
        _ZN7PathPtr6FromIDEj(self + 0x58c, (*(unsigned int *)(self + 8) >> 8) & 0xf);
        *(int *)(self + 0x578) = _ZNK7PathPtr8NumNodesEv(self + 0x58c);
        *(int *)(self + 0x57c) = 0;
        _ZNK7PathPtr7GetNodeER7Vector3j(self + 0x58c, self + 0x580, *(unsigned int *)(self + 0x57c));
        func_ov092_021313b0(self);
    }

    {
        *(int *)LA(self + 0x60) += 0xfa000;
        *(int *)(self + 0x558) = *(int *)(self + 0x5c);
        *(int *)(self + 0x55c) = *(int *)(self + 0x60);
        *(int *)(self + 0x560) = *(int *)(self + 0x64);
        Vec3_Asr(&tmp, (Vec3 *)(self + 0x5c), 3);
    }

    Matrix4x3_FromTranslation(&data_020a0e68, tmp.x, tmp.y, tmp.z);
    *(Mtx43 *)(self + 0xf0) = data_020a0e68;
    *(Mtx43 *)(self + 0x528) = *(Mtx43 *)(self + 0xf0);

    func_ov092_02131a88(self);

    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov092_02132548);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, f, self + 0x2ec, 0x1000, *(short *)(self + 0x8e), &data_ov092_02132220);
    func_020393d4(self + 0x124, &_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    *(unsigned char *)(self + 0x575) = (unsigned char)((*(short *)(self + 0x8c) >> 0xe) & 3);
    {
        unsigned char *b = (unsigned char *)LA(self + 0x575);
        *b = (unsigned char)(*b | ((*(short *)(self + 0x8e) >> 0xc) & 0xc));
        *b = (unsigned char)(*b | ((*(short *)(self + 0x90) >> 0xa) & 0x30));
    }

    {
        int stk[3];
        stk[0] = 0;
        stk[1] = -0xfa000;
        stk[2] = 0;
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
            self + 0x4e8, self, (void *)&stk[0], 0xc8000, 0x190000, 2, 0x6003c0);
    }

    *(int *)(self + 0x320) = 0;
    return 1;
}
