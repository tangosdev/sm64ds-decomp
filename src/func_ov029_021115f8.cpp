//cpp
typedef int Fix12i;
struct SharedFilePtr; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
struct Model { int d; };
struct ModelBase { int d; };
struct MeshCollider { int d; };
struct MovingMeshCollider { int d; };

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase*, BMD_File*, int, int);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern "C" KCL_File* _ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    MovingMeshCollider*, KCL_File*, const Matrix4x3&, Fix12i, short, CLPS_Block&);
extern "C" void func_020393d4(int* p, int v);
extern "C" void func_020393c4(int* p, int v);

extern SharedFilePtr data_ov029_02114250;
extern SharedFilePtr data_ov029_02114248;
extern CLPS_Block data_ov029_0211302c;
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern int func_ov029_021116e4;

extern "C" int func_ov029_021115f8(char* thiz)
{
    char* c = thiz;
    {
        BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_02114250);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0xd4), bmd, 1, -1);
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    {
        KCL_File* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov029_02114248);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (MovingMeshCollider*)(c + 0x124), kcl, *(const Matrix4x3*)(c + 0x2ec),
            0x199, *(short*)(c + 0x8e), data_ov029_0211302c);
    }
    {
        short* p = (short*)(((unsigned long long)((int)c + 0x94)) & 0xFFFFFFFFFFFFFFFFULL);
        short val = *p;
        int* arg0_1 = (int*)(c + 0x124);
        int arg1_1 = (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
        *p = (short)(val + 0x4000);
        func_020393d4(arg0_1, arg1_1);
    }
    {
        int* arg0_2 = (int*)(c + 0x124);
        int arg1_2 = (int)&func_ov029_021116e4;
        func_020393c4(arg0_2, arg1_2);
    }
    *(int*)(c + 0x320) = 0;
    *(short*)(c + 0x324) = 0;
    *(unsigned char*)(c + 0x326) = 0;
    *(unsigned char*)(c + 0x327) = 0;
    return 1;
}
