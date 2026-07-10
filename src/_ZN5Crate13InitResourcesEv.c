extern int _ZN5Model8LoadFileER13SharedFilePtr(char* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* thiz, int f, int a, int b);
extern void _ZN11ShadowModel10InitCuboidEv(char* thiz);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(char* f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(char* thiz, int f, char* m, int fix, short s, int blk);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* thiz, char* actor, int b, int d, void* v, int f);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(char* thiz);
extern void func_ov098_02138b28(char* c, int i);
extern void func_ov098_02138ce0(char* c);
extern char data_ov098_0213c4c8[];

int _ZN5Crate13InitResourcesEv(char* c)
{
    char* f = 0;
    if (*(unsigned short*)(c + 0xc) == 0xc2) f = data_ov098_0213c4c8;
    if (f == 0) return 0;
    *(int*)(c + 0x4e8) = *(int*)(c + 0x5c);
    *(int*)(c + 0x4ec) = *(int*)(c + 0x60);
    *(int*)(c + 0x4f0) = *(int*)(c + 0x64);
    *(short*)(c + 0x500) = *(short*)(c + 0x8c);
    *(short*)(c + 0x502) = *(short*)(c + 0x8e);
    *(short*)(c + 0x504) = *(short*)(c + 0x90);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(*(char**)(f)), 1, -1);
    _ZN11ShadowModel10InitCuboidEv(c + 0x508);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(char**)(f + 4)),
        c + 0x2ec, 0x199, *(short*)(c + 0x8e), *(int*)(f + 8));
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x320, c, 0x28000, 0x28000, 0, 0);
    _ZN12WithMeshClsn19StartDetectingWaterEv(c + 0x320);
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    func_ov098_02138b28(c, 0);
    *(int*)(c + 0xd0) = 0;
    func_ov098_02138ce0(c);
    return 1;
}
