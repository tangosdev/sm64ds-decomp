#pragma opt_strength_reduction off
typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct BMD_File BMD_File;
typedef struct KCL_File KCL_File;
typedef struct Matrix4x3 Matrix4x3;
typedef struct CLPS_Block CLPS_Block;
extern SharedFilePtr data_ov025_02113ae0;
extern SharedFilePtr data_ov002_0210d9f0;
extern SharedFilePtr data_ov025_02113ad8;
extern CLPS_Block data_ov025_02112d08;
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern int func_ov025_021125dc;
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
extern KCL_File* _ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, KCL_File* k, Matrix4x3* m, Fix12 f, short s, CLPS_Block* b);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);

int _ZN11PyramidLift13InitResourcesEv(char* c) {
    BMD_File* bmd;
    KCL_File* kcl;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov025_02113ae0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, bmd, 1, -1);
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9f0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x320, bmd, 1, -1);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov025_02113ad8);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c+0x124, kcl, (Matrix4x3*)(c+0x2ec), 0x199, *(short*)(c+0x8e), &data_ov025_02112d08);
    func_020393d4(c+0x124, &_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(c+0x124, &func_ov025_021125dc);
    {
        int n;
        char *ip;
        int k;
        *(int*)(c+0x370) = *(int*)(c+0x5c);
        n = 0;
        *(int*)(c+0x374) = *(int*)(c+0x60);
        ip = c;
        *(int*)(c+0x378) = *(int*)(c+0x64);
        *(unsigned char*)(c+0x3f6) = (unsigned char)n;
        *(unsigned char*)(c+0x3f7) = (unsigned char)n;
        k = 0x1cc000;
        do {
            int *py;
            int prod;
            n = n + 1;
            *(int*)(ip+0x37c) = *(int*)(c+0x5c);
            *(int*)(ip+0x380) = *(int*)(c+0x60);
            prod = n * k;
            *(int*)(ip+0x384) = *(int*)(c+0x64);
            py = (int*)(((int)ip + 0x380) & 0xFFFFFFFFFFFFFFFFLL);
            *py = *py - prod;
            ip = ip + 0xc;
        } while (n < 10);
    }
    return 1;
}
