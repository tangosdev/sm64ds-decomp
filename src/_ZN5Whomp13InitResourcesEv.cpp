//cpp
#include "types.h"
// @symbol _ZN5Whomp13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Whomp.h"
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
extern SFP data_ov079_02128178;
extern void *data_ov079_02128170[];
extern void *data_ov079_021275ec[];
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

int Whomp::InitResources()
{
    u16 id;
    int r6;
    u8 idx;
    void *bmd;
    void *kcl;
    void *anim;
    int i;

    id = mActorID;
    int b = (id == 0xa5);
    if (b) {
        mIsKing = 1;
        bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov079_02128168);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x2cc, bmd, 1, -1);
        r6 = 0;
        do {
            anim = data_ov079_02127600[r6];
            _ZN9Animation8LoadFileER13SharedFilePtr(anim);
            r6 += 1;
        } while (r6 < 6);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov079_02128178);
    } else {
        mIsKing = 0;
        bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov079_02128170);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x2cc, bmd, 1, -1);
        r6 = 0;
        do {
            anim = data_ov079_021275ec[r6];
            _ZN9Animation8LoadFileER13SharedFilePtr(anim);
            r6 += 1;
        } while (r6 < 5);
    }

    idx = mIsKing;
    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov079_02127bf0[idx]);
    (void)kcl;

    if (_ZN11ShadowModel10InitCuboidEv((char *)&mShadowModel) == 0) {
        return 0;
    }

    if (mIsKing != 0) {
        unk_401 = 3;
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(data_ov079_02128168.unk4, data_ov079_02128178.unk4);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            ((char *)this) + 0x330, data_ov079_02128178.unk4, 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(((char *)this) + 0x330, 0x40000000);
        unk_33c = 0x1000;
        unk_338 = 0;
        unk_409 = (u8)(mParam & 0xf);
        unk_408 = _ZN5Actor9TrackStarEjj(((char *)this), unk_409, 2);
    } else {
        unk_401 = 1;
    }

    func_ov079_02124188(((char *)this));
    func_ov079_02124008(((char *)this));

    idx = mIsKing;
    if (idx == 0) {
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            ((char *)this) + 0x418, *(void **)((char *)data_ov079_02127bf0[idx] + 4), ((char *)this) + 0x5e0,
            0x199, unk_08e, &data_ov079_02127ba0);
    } else {
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            ((char *)this) + 0x418, *(void **)((char *)data_ov079_02127bf0[idx] + 4), ((char *)this) + 0x5e0,
            0x1000, unk_08e, &func_021135d4);
    }

    func_01ffb0bc((char *)&mMovingMeshCollider);
    func_020393d4(((char *)this) + 0x418, (void *)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    {
        s32 sp[3];
        sp[0] = 0;
        sp[1] = 0;
        sp[2] = 0x1000;
        func_01ffb07c(((char *)this) + 0x418, sp);
    }

    func_020396d0(((char *)this) + 0x418, 0xb50);

    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x110, ((char *)this), 0x32000, 0x32000, 0, 0);

    unk_3b0 = 0;
    *(s16 *)(((char *)this) + 0x300 + 0xb8) = unk_094;
    unk_40c = 0;
    *(s16 *)(((char *)this) + 0x300 + 0xfc) = 0;

    idx = mIsKing;
    anim = *(void **)((char *)data_ov079_021275ec[idx * 5 + 3] + 4);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char *)this) + 0x2cc, anim, 0, 0x1000, 0);

    unk_3bc = mPosX;
    unk_3c0 = mPosY;
    unk_3c4 = mPosZ;
    *(s16 *)(((char *)this) + 0x300 + 0xe6) = unk_08c;
    *(s16 *)(((char *)this) + 0x300 + 0xe8) = unk_08e;
    *(s16 *)(((char *)this) + 0x300 + 0xea) = unk_090;

    if (mIsKing != 0) {
        unk_09c = -0x8000;
    } else {
        unk_09c = -0x4800;
    }
    unk_0a0 = -0x31000;

    unk_402 = 0;
    unk_403 = 0;

    if (mIsKing != 0) {
        unk_406 = 4;
    } else {
        unk_406 = 6;
    }

    unk_410 = 0;
    unk_404 = 1;
    unk_407 = 0;
    unk_40b = 0;
    unk_100 = 0;

    i = 0;
    do {
        ((WithArr *)((char *)this))->arr[i] = 0;
        i += 1;
    } while (i < 4);

    {
        s32 cnt = data_0209f21c;
        if (cnt > 0) {
            i = 0;
            do {
                s32 v = data_0209f394[i];
                ((WithArr *)((char *)this))->arr[i] = v;
                i += 1;
            } while (i < cnt);
        }
    }

    if (mIsKing != 0
        && data_0209f2f8 == 7
        && data_0209f220 != 1
        && IsStarCollectedInCurLevel(1) != 0) {
        _ZN9ActorBase18MarkForDestructionEv(((char *)this));
        return 0;
    }

    return 1;
}
