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

struct DataT { SharedFilePtr* a; SharedFilePtr* b; CLPS_Block* c; };
extern DataT data_ov052_021124d4;

extern "C" int func_ov052_02111348(char* thiz)
{
    char* c = thiz;
    {
        BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(*data_ov052_021124d4.a);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0xd4), bmd, 1, -1);
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    {
        KCL_File* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*data_ov052_021124d4.b);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (MovingMeshCollider*)(c + 0x124), kcl, *(const Matrix4x3*)(c + 0x2ec),
            0x1000, *(short*)(c + 0x8e), *data_ov052_021124d4.c);
    }
    *(int*)(c + 0x320) = *(int*)(c + 0x60);
    {
        int v = *(int*)(c + 8);
        int b = v & 0xff;
        if (b == 0xff || v == 0) {
            *(int*)(c + 0x324) = 0x64000;
        } else {
            *(int*)(c + 0x324) = b * 0xa000;
        }
    }
    *(short*)(c + 0x31e) = *(short*)(c + 0x8c);
    return 1;
}
