//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void func_ov024_021114c4(char* t);
extern void func_ov024_02111480(char* c);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void* thiz, void* act);
extern void _ZN5Event8ClearBitEj(unsigned int n);
extern int data_ov024_02113968[];
extern int data_ov024_02113960[];
extern int data_ov024_021129f0[];
extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

int func_ov024_021116ec(char* c)
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov024_02113968);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x320, m, 1, -1);
    func_ov024_021114c4(c);
    func_ov024_02111480(c);
    void* mc = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov024_02113960);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, mc, c + 0x370, 0x199, *(short*)(c + 0x8e), data_ov024_021129f0);
    func_020393d4(c + 0x124, (void*)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    _ZN16MeshColliderBase6EnableEP5Actor(c + 0x124, c);
    *(int*)(c + 0x3a0) = *(int*)(c + 0x5c);
    *(int*)(c + 0x3a4) = *(int*)(c + 0x60);
    *(int*)(c + 0x3a8) = *(int*)(c + 0x64);
    *(short*)(c + 0x3b0) = 0;
    *(unsigned char*)(c + 0x3b6) = 0;
    *(unsigned char*)(c + 0x3b7) = 0;
    *(int*)(c + 0x3ac) = 0;
    *(short*)(c + 0x3b4) = 0;
    _ZN5Event8ClearBitEj(0xe);
    return 1;
}
}
