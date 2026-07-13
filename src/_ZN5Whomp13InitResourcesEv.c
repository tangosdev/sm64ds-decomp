//cpp
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct SharedFilePtr;
struct BMD_File;
struct BTP_File;
struct BCA_File;
struct KCL_File;
struct Matrix4x3;
struct CLPS_Block;
struct Vector3_16;
struct Actor;
struct ClsnResult;
struct Vector3;

extern "C" {
    void *_ZN5Model8LoadFileER13SharedFilePtr(void *shared);
    void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *mb, void *bmd, int a, int b);
    void *_ZN9Animation8LoadFileER13SharedFilePtr(void *shared);
    void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *shared);
    void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *shared);
    int _ZN11ShadowModel10InitCuboidEv(void *self);
    void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
    void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, unsigned int b);
    void _ZN9Animation8SetFlagsEi(void *self, int flags);
    u8 _ZN5Actor9TrackStarEjj(void *self, unsigned int a, unsigned int b);
    void func_ov079_02124188(void *self);
    void func_ov079_02124008(void *self);
    void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        void *mc, void *kcl, void *mtx, int fix, s16 s, void *clps);
    void func_01ffb0bc(void *self);
    void func_020393d4(void *p, void *v);
    void func_01ffb07c(void *self, s32 *sp);
    void func_020396d0(void *self, int v);
    void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *c, void *d);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
    int IsStarCollectedInCurLevel(int a);
    void _ZN9ActorBase18MarkForDestructionEv(void *self);
}

struct SFP { void *unk0; void *unk4; };
extern SFP data_ov079_02128168;
extern void *data_ov079_02127600[];
extern SFP data_ov079_02128178;
extern void *data_ov079_02128170[];
extern void *data_ov079_021275ec[];
extern void *data_ov079_02127bf0[];
extern void *data_ov079_02127ba0;
extern void *func_021135d4;
extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
extern u8 data_0209f21c;
extern s32 data_0209f394[];
extern signed char data_0209f2f8;
extern u8 data_0209f220;

struct Sub18 {
    char pad[0x39c];
    s32 unk39c;
};

struct WithArr {
    char pad[0x39c];
    s32 arr[8];
};

extern "C" int _ZN5Whomp13InitResourcesEv(char *self) {
    u16 id;
    int r6;
    u8 idx;
    void *bmd;
    void *kcl;
    void *anim;
    int i;

    id = *(u16 *)(self + 0xc);
    int b = (id == 0xa5);
    if (b) {
        *(u8 *)(self + 0x414) = 1;
        bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov079_02128168);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0x2cc, bmd, 1, -1);
        r6 = 0;
        do {
            anim = data_ov079_02127600[r6];
            _ZN9Animation8LoadFileER13SharedFilePtr(anim);
            r6 += 1;
        } while (r6 < 6);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov079_02128178);
    } else {
        *(u8 *)(self + 0x414) = 0;
        bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov079_02128170);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0x2cc, bmd, 1, -1);
        r6 = 0;
        do {
            anim = data_ov079_021275ec[r6];
            _ZN9Animation8LoadFileER13SharedFilePtr(anim);
            r6 += 1;
        } while (r6 < 5);
    }

    idx = *(u8 *)(self + 0x414);
    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov079_02127bf0[idx]);
    (void)kcl;

    if (_ZN11ShadowModel10InitCuboidEv(self + 0x344) == 0) {
        return 0;
    }

    if (*(u8 *)(self + 0x414) != 0) {
        *(u8 *)(self + 0x401) = 3;
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(data_ov079_02128168.unk4, data_ov079_02128178.unk4);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            self + 0x330, data_ov079_02128178.unk4, 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(self + 0x330, 0x40000000);
        *(s32 *)(self + 0x33c) = 0x1000;
        *(s32 *)(self + 0x338) = 0;
        *(u8 *)(self + 0x409) = (u8)(*(s32 *)(self + 8) & 0xf);
        *(u8 *)(self + 0x408) = _ZN5Actor9TrackStarEjj(self, *(u8 *)(self + 0x409), 2);
    } else {
        *(u8 *)(self + 0x401) = 1;
    }

    func_ov079_02124188(self);
    func_ov079_02124008(self);

    idx = *(u8 *)(self + 0x414);
    if (idx == 0) {
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x418, *(void **)((char *)data_ov079_02127bf0[idx] + 4), self + 0x5e0,
            0x199, *(s16 *)(self + 0x8e), &data_ov079_02127ba0);
    } else {
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x418, *(void **)((char *)data_ov079_02127bf0[idx] + 4), self + 0x5e0,
            0x1000, *(s16 *)(self + 0x8e), &func_021135d4);
    }

    func_01ffb0bc(self + 0x418);
    func_020393d4(self + 0x418, (void *)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    {
        s32 sp[3];
        sp[0] = 0;
        sp[1] = 0;
        sp[2] = 0x1000;
        func_01ffb07c(self + 0x418, sp);
    }

    func_020396d0(self + 0x418, 0xb50);

    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self + 0x110, self, 0x32000, 0x32000, 0, 0);

    *(s32 *)(self + 0x3b0) = 0;
    *(s16 *)(self + 0x300 + 0xb8) = *(s16 *)(self + 0x94);
    *(u8 *)(self + 0x40c) = 0;
    *(s16 *)(self + 0x300 + 0xfc) = 0;

    idx = *(u8 *)(self + 0x414);
    anim = *(void **)((char *)data_ov079_021275ec[idx * 5 + 3] + 4);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x2cc, anim, 0, 0x1000, 0);

    *(s32 *)(self + 0x3bc) = *(s32 *)(self + 0x5c);
    *(s32 *)(self + 0x3c0) = *(s32 *)(self + 0x60);
    *(s32 *)(self + 0x3c4) = *(s32 *)(self + 0x64);
    *(s16 *)(self + 0x300 + 0xe6) = *(s16 *)(self + 0x8c);
    *(s16 *)(self + 0x300 + 0xe8) = *(s16 *)(self + 0x8e);
    *(s16 *)(self + 0x300 + 0xea) = *(s16 *)(self + 0x90);

    if (*(u8 *)(self + 0x414) != 0) {
        *(s32 *)(self + 0x9c) = -0x8000;
    } else {
        *(s32 *)(self + 0x9c) = -0x4800;
    }
    *(s32 *)(self + 0xa0) = -0x31000;

    *(u8 *)(self + 0x402) = 0;
    *(u8 *)(self + 0x403) = 0;

    if (*(u8 *)(self + 0x414) != 0) {
        *(u8 *)(self + 0x406) = 4;
    } else {
        *(u8 *)(self + 0x406) = 6;
    }

    *(s32 *)(self + 0x410) = 0;
    *(u8 *)(self + 0x404) = 1;
    *(u8 *)(self + 0x407) = 0;
    *(u8 *)(self + 0x40b) = 0;
    *(u16 *)(self + 0x100) = 0;

    i = 0;
    do {
        ((WithArr *)self)->arr[i] = 0;
        i += 1;
    } while (i < 4);

    {
        s32 cnt = data_0209f21c;
        if (cnt > 0) {
            i = 0;
            do {
                s32 v = data_0209f394[i];
                ((WithArr *)self)->arr[i] = v;
                i += 1;
            } while (i < cnt);
        }
    }

    if (*(u8 *)(self + 0x414) != 0
        && data_0209f2f8 == 7
        && data_0209f220 != 1
        && IsStarCollectedInCurLevel(1) != 0) {
        _ZN9ActorBase18MarkForDestructionEv(self);
        return 0;
    }

    return 1;
}
