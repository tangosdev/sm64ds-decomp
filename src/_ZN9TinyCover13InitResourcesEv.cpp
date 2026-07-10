//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* bmd, void* bta);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* thiz, void* bta, int a, int b, unsigned int e);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void* thiz, void* act);
extern int _ZN5Event6GetBitEj(unsigned int n);
extern int data_ov033_021124f0[];
extern int data_ov033_02111bc8[];
extern int data_ov033_021124e8[];
extern int data_ov033_02111c1c[];

int _ZN9TinyCover13InitResourcesEv(char* c)
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov033_021124f0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, 0x14);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*(void**)((char*)data_ov033_021124f0 + 4), data_ov033_02111bc8);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0x320, data_ov033_02111bc8, 0, 0x1000, 0);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    void* mc = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov033_021124e8);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, mc, c + 0x2ec, 0x1000, *(short*)(c + 0x8e), data_ov033_02111c1c);
    _ZN16MeshColliderBase6EnableEP5Actor(c + 0x124, c);
    *(int*)(c + 0x334) = *(int*)(c + 0x60) - 0x3c000;
    return _ZN5Event6GetBitEj(0xe) == 0;
}
}
