extern void* data_ov064_0211adbc[];
extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern void _ZN19CylinderClsnWithPos4InitERK7Vector35Fix12IiES4_jj(void* thiz, void* v, int f1, int f2, unsigned int a, unsigned int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(int* p, int v);

int func_ov064_02118480(char* c) {
    int i;
    char* p;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov064_0211adbc[0]), 1, -1);
    p = c + 0x360;
    for (i = 0; i < 8; i++) {
        _ZN19CylinderClsnWithPos4InitERK7Vector35Fix12IiES4_jj(p, c + 0x5c, 0x4b000, 0x96000, 0x200002, 0);
        p += 0x3c;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124,
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov064_0211adbc[1]),
        c + 0x2ec, 0x199, *(short*)(c + 0x8e), data_ov064_0211adbc[2]);
    func_020393d4((int*)(c + 0x124),
        (int)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    return 1;
}
