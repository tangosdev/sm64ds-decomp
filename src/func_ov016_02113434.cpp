//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void func_ov016_021130a4(char* t);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* act, int fix, int t, void* vr, int t2);
extern int data_ov016_02114e74[];
extern int data_ov016_02114e6c[];
extern int data_ov016_02113bac[];
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

int func_ov016_02113434(char* c)
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_02114e74);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, -1);
    func_ov016_021130a4(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    void* mc = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov016_02114e6c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, mc, c + 0x2ec, 0x199, *(short*)(c + 0x8e), data_ov016_02113bac);
    func_020393d4(c + 0x124, (void*)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x324, c, 0x14000, 0x14000, 0, 0);
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x14000;
    *(int*)(c + 0x320) = 0;
    *(unsigned char*)(c + 0x4f4) = 0;
    *(int*)(c + 0x4f0) = 0;
    return 1;
}
}
