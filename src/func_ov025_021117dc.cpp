//cpp
typedef int Fix12i;
struct SharedFilePtr; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
struct Model { int d; };
struct ModelBase { int d; };
struct MeshCollider { int d; };
struct MovingMeshCollider { int d; };

extern "C" void func_ov025_02111344(char* t);
extern "C" void func_ov025_021112e0(char* self);
extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase*, BMD_File*, int, int);
extern "C" KCL_File* _ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    MovingMeshCollider*, KCL_File*, const Matrix4x3&, Fix12i, short, CLPS_Block&);
extern "C" void func_020393d4(int* p, int v);

extern SharedFilePtr data_ov025_02113a68;
extern SharedFilePtr data_ov025_02113a60;
extern CLPS_Block data_ov025_02112c28;
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;

extern "C" int func_ov025_021117dc(char* thiz)
{
    char* c = thiz;
    func_ov025_02111344(c);
    func_ov025_021112e0(c);
    {
        BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov025_02113a68);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0xd4), bmd, 1, -1);
    }
    {
        KCL_File* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov025_02113a60);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (MovingMeshCollider*)(c + 0x124), kcl, *(const Matrix4x3*)(c + 0x2ec),
            0x1000, *(short*)(c + 0x8e), data_ov025_02112c28);
    }
    func_020393d4((int*)(c + 0x124), (int)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    *(short*)(c + 0x8e) = 0;
    *(int*)(c + 0x320) = *(int*)(c + 0x60);
    *(short*)(c + 0x324) = 0;
    *(short*)(c + 0x326) = 0;
    *(unsigned char*)(c + 0x328) = 0;
    *(unsigned char*)(c + 0x329) = 0;
    *(int*)(c + 0x330) = 0;
    *(int*)(c + 0x32c) = *(int*)(c + 0x330);
    return 1;
}
