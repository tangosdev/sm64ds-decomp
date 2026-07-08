// NONMATCHING: different op / idiom (div=21). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct BMD_File BMD_File;
typedef struct KCL_File KCL_File;
typedef struct Matrix4x3 Matrix4x3;
typedef struct CLPS_Block CLPS_Block;
extern SharedFilePtr data_ov025_02113ae0;
extern SharedFilePtr SNUFIT_BULLET_MODEL_PTR;
extern SharedFilePtr data_ov025_02113ad8;
extern CLPS_Block data_ov025_02112d08;
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern int func_ov025_021125dc;
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
extern KCL_File* _ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, KCL_File* k, Matrix4x3* m, Fix12 f, short s, CLPS_Block* b);
extern void func_020393d4(int* p, int v);
extern void func_020393c4(int* p, int v);

struct E12 { int a, b, c; };

int func_ov025_02112498(char* c) {
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov025_02113ae0), 1, -1);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x320, _ZN5Model8LoadFileER13SharedFilePtr(&SNUFIT_BULLET_MODEL_PTR), 1, -1);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c+0x124, _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov025_02113ad8),
        (Matrix4x3*)(c+0x2ec), 0x199, *(short*)(c+0x8e), &data_ov025_02112d08);
    func_020393d4((int*)(c+0x124), (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4((int*)(c+0x124), (int)&func_ov025_021125dc);
    *(int*)(c+0x370) = *(int*)(c+0x5c);
    *(int*)(c+0x374) = *(int*)(c+0x60);
    *(int*)(c+0x378) = *(int*)(c+0x64);
    *(unsigned char*)(c+0x3f6) = 0;
    *(unsigned char*)(c+0x3f7) = 0;
    {
        char* ip = c;
        int n = 0;
        int k = 0x1cc000;
        do {
            n++;
            *(int*)(ip+0x37c) = *(int*)(c+0x5c);
            *(int*)(ip+0x380) = *(int*)(c+0x60);
            *(int*)(ip+0x384) = *(int*)(c+0x64);
            *(int*)(ip+0x380) -= n*k;
            ip += 0xc;
        } while (n < 10);
    }
    return 1;
}
