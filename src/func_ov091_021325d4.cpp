//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern "C" {
    void* _ZN5Model8LoadFileER13SharedFilePtr(void* shared);
    void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* mb, void* bmd, int a, int b);
    void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
    void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
    void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* shared);
    void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* mc, void* kcl, void* mtx, int f, s16 s, void* clps);
    void func_020393d4(void* p, void* v);
}

struct SFP { void* file; void* b1; void* b2; };
extern SFP data_ov091_02135024[];   // model SFP table, stride 0xc
extern char data_ov091_02135028[];   // collider sfp table, stride 0xc
extern char data_ov091_0213502c[];   // clps table, stride 0xc
extern u16 data_ov091_02134514[];
extern u16 data_ov091_02134504[];
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();

extern "C" int func_ov091_021325d4(void* self)
{
    u8* c = (u8*)self;
    u16 t = *(u16*)(c+0xc);
    switch (t) {
        case 0x37: *(u8*)(c+0x322) = 6; break;
        case 0x7c: *(u8*)(c+0x322) = 3; break;
        case 0x93: *(u8*)(c+0x322) = 4; break;
        case 0x9b: *(u8*)(c+0x322) = 2; break;
        case 0x8a: *(u8*)(c+0x322) = 0; break;
        case 0x9a: *(u8*)(c+0x322) = 1; break;
        case 0x92: *(u8*)(c+0x322) = 5; break;
    }

    void* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov091_02135024[*(u8*)(c+0x322)].file);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((void*)(c+0xd4), bmd, 1, -1);

    *(u16*)(c+0x94) = (u16)(*(s16*)(c+0x8e) + data_ov091_02134514[*(u8*)(c+0x322)]);

    if (*(s16*)(c+0x8c) != 0) {
        *(u16*)(c+0x94) = (u16)(*(s16*)(c+0x8e) + *(s16*)(c+0x8c));
    }

    *(u16*)(c+0x320) = data_ov091_02134504[*(u8*)(c+0x322)];
    *(s32*)(c+0x98) = 0xa000;
    *(s32*)(c+0x324) = *(s32*)(c+0x5c);
    *(s32*)(c+0x328) = *(s32*)(c+0x60);
    *(s32*)(c+0x32c) = *(s32*)(c+0x64);
    _ZN8Platform21UpdateModelPosAndRotYEv(self);
    _ZN8Platform19UpdateClsnPosAndRotEv(self);

    if (*(u8*)(c+0x322) == 6) {
        int oi = *(u8*)(c+0x322) * 0xc;
        void* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void**)(data_ov091_02135028+oi));
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (void*)(c+0x124), kcl, (void*)(c+0x2ec), 0x1000, *(s16*)(c+0x8e), *(void**)(data_ov091_0213502c+oi));
    } else {
        int oi = *(u8*)(c+0x322) * 0xc;
        void* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void**)(data_ov091_02135028+oi));
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (void*)(c+0x124), kcl, (void*)(c+0x2ec), 0x199, *(s16*)(c+0x8e), *(void**)(data_ov091_0213502c+oi));
    }
    func_020393d4((void*)(c+0x124), (void*)_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    return 1;
}
