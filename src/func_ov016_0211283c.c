extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void func_ov016_021126a8(char* t);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern void func_020393d4(int* p, int v);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
extern int SublevelToLevel(int i);
extern int IsStarCollected(int a, int b);
extern void* data_ov016_021149d4[];
extern void* data_ov016_021136e4[];
extern void* data_ov016_021136dc[];
extern void* _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern unsigned char STAR_ID;
int func_ov016_0211283c(char* c) {
    void* sp8[2];
    unsigned int idx;
    void* f;
    int b;
    sp8[0] = data_ov016_021149d4[0];
    sp8[1] = data_ov016_021149d4[1];
    b = (int)(*(unsigned short*)(c+0xc) == 0x39);
    if (b != 0) *(unsigned char*)(c+0x31e) = 0;
    else *(unsigned char*)(c+0x31e) = 1;
    idx = *(unsigned char*)(c+0x31e);
    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_021136e4[idx]);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
    func_ov016_021126a8(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    idx = *(unsigned char*)(c+0x31e);
    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov016_021136dc[idx]);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, f, c+0x2ec, 0x1000, *(short*)(c+0x8e), sp8[idx]);
    if (*(unsigned char*)(c+0x31e) == 0) {
        func_020393d4((int*)(c+0x124), (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    }
    _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
    *(int*)(c+0x324) = 0;
    *(unsigned short*)(c+0x328) = 0;
    if (STAR_ID > 1) {
        if (IsStarCollected(SublevelToLevel(8), 1) != 0) {
            if (*(unsigned char*)(c+0x31e) == 0) goto ret1;
            return 0;
        }
    }
    if (*(unsigned char*)(c+0x31e) == 0) return 0;
ret1:
    return 1;
}
