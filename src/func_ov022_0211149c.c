typedef struct { int a, b, c; } SF3;
extern int _ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, int f, int a, int b);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, int kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393d4(void *p, void *v);
extern void func_020393c4(void *p, void *v);
extern SF3 data_ov022_02113cc8;
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_[];
extern void func_ov022_02111564(void*);
int func_ov022_0211149c(char *c) {
    int f = _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov022_02113cc8.a);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr((void*)data_ov022_02113cc8.b);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, k, c+0x2ec, 0x199, *(short*)(c+0x8e), (void*)data_ov022_02113cc8.c);
    func_020393d4(c+0x124, _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(c+0x124, func_ov022_02111564);
    *(short*)(c+0x96) = -0x100;
    if (*(short*)(c+0x90) != 0) *(short*)(c+0x96) = *(short*)(c+0x90);
    return 1;
}
