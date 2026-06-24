//cpp
typedef int Fix12i;
struct SharedFilePtr; struct BMD_File; struct BTA_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block; struct Actor;
struct Model { int d; };
struct ModelBase { int d; };
struct TextureTransformer { int d; };
struct MeshCollider { int d; };
struct MovingMeshCollider { int d; };
struct MeshColliderBase { int d; };

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase*, BMD_File*, int, int);
extern "C" void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(BMD_File&, BTA_File&);
extern "C" void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(TextureTransformer*, BTA_File&, int, Fix12i, unsigned int);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern "C" KCL_File* _ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    MovingMeshCollider*, KCL_File*, const Matrix4x3&, Fix12i, short, CLPS_Block&);
extern "C" void _ZN16MeshColliderBase6EnableEP5Actor(MeshColliderBase*, Actor*);
extern "C" int _ZN5Event6GetBitEj(unsigned int);

struct D113afc { SharedFilePtr* a; BMD_File* b; };
extern D113afc data_ov032_02113afc;
extern BTA_File data_ov032_02112f64;
extern SharedFilePtr data_ov032_02113af4;
extern CLPS_Block data_ov032_02112fb8;

extern "C" int func_ov032_021127f0(char* thiz)
{
    char* c = thiz;
    {
        BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(SharedFilePtr*)&data_ov032_02113afc);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0xd4), bmd, 1, 0x14);
    }
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*data_ov032_02113afc.b, data_ov032_02112f64);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        (TextureTransformer*)(c + 0x320), data_ov032_02112f64, 0, 0x1000, 0);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    {
        KCL_File* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov032_02113af4);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (MovingMeshCollider*)(c + 0x124), kcl, *(const Matrix4x3*)(c + 0x2ec),
            0x1000, *(short*)(c + 0x8e), data_ov032_02112fb8);
    }
    _ZN16MeshColliderBase6EnableEP5Actor((MeshColliderBase*)(c + 0x124), (Actor*)c);
    return (int)(_ZN5Event6GetBitEj(0xe) == 0);
}
