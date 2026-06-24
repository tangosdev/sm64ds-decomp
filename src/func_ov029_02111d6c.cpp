//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);
extern int data_ov029_0211428c[];
extern int data_ov029_02114284[];
extern int data_ov029_0211306c[];
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);
extern void func_ov029_02111e60(void);

int func_ov029_02111d6c(char* c)
{
    void* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_0211428c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    void* mc = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov029_02114284);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, mc, c + 0x2ec, 0x1000, *(short*)(c + 0x8e), data_ov029_0211306c);
    func_020393d4(c + 0x124, (void*)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(c + 0x124, (void*)&func_ov029_02111e60);
    *(unsigned char*)(c + 0x32b) = 0;
    *(unsigned char*)(c + 0x32a) = *(unsigned char*)(c + 0x32b);
    *(short*)(c + 0x328) = 0;
    if (*(int*)(c + 8) & 1)
        *(unsigned char*)(c + 0x32c) = 3;
    else
        *(unsigned char*)(c + 0x32c) = 0;
    *(int*)(c + 0x320) = *(int*)(c + 0x60);
    return 1;
}
}
