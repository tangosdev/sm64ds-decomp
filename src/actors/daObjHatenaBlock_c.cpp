//cpp
/**
 * Question / item / VS-item / cap blocks (HATENA_BLOCK 20, ITEM_BLOCK 21,
 * VS_ITEM_BLOCK 22, CAP_BLOCK_M/W/L 23-25).
 *
 * One class, six registry factories. Hit from below, kick, pound, mega, or
 * attack pops the block, then the content-type table spawns the prize
 * (star, cap, 1-up, mushroom, flower, shell, bob-omb, coins).
 *
 * daObjHatenaBlock_c_classInit_* are reconstructed (RTTI daObjHatenaBlock_c,
 * those six registry IDs). Retail does not store those spellings.
 * Historical aliases: QuestionBlock_Spawn, ExclamationBlock_Spawn,
 * ExclamationBlockVs_Spawn, CapBlockMario/Wario/Luigi_Spawn.
 *
 * deslop
 * Leftover: common.h first -- Model.h's nested Matrix4x3 would win and
 *   func_ov102_02149e38 / 02149ff0 size-DIFF (this TU, 0x18 / 0x1c).
 * Leftover: ModelAnim::SetAnim and dBgW_KcMbg::SetFile stay mangled in
 *   this TU -- both take Fix12<int> by value (wall 6az); a method call
 *   homes the argument and size-DIFFs InitResources.
 * Leftover: dBgActor_c::IsClsnInRange stays mangled -- Fix12<int> by
 *   value (wall 6az). This TU's Behavior call.
 * Leftover: dActor_c::DropShadowScaleXYZ stays mangled -- Fix12<int> by
 *   value (wall 6az). func_ov102_02149ea4.
 * Leftover: Particle::System::NewSimple stays mangled -- Fix12<int> by
 *   value (wall 6az). Particle.h has no System::NewSimple.
 * Leftover: dActor_c::Earthquake stays mangled -- not on dActor_c.h.
 *   func_ov102_02149c78.
 * Leftover: UntrackStar stays mangled -- takes s8 &, this TU's field is
 *   u8 mStarTracked. CleanupResources.
 * Leftover: KillAndTrackInDeathTable stays mangled in func_ov102_021494cc
 *   -- the real member is void, this helper returns the bl's r0.
 * Leftover: func_020393a4 / func_02039394 poke mMeshCollider (no setter).
 *   This TU's Behavior; naming belongs with dBgW in arm9.
 * Leftover: func_ov102_* helpers stay linker names (offset soup, PMF
 *   dispatch through data_ov102_0214e890 / 0214e870 / 0214e8c0). Not coined.
 * Leftover: func_ov102_0214ad14 / 0214b384 are daBmb_c helpers; this TU's
 *   func_ov102_02149220 calls them. Naming belongs in ov102/daBmb_c.
 * Leftover: func_ov002_020f0438 is ov002; bounce-end calls it on a held
 *   SECRET_COIN (actorID 0x149).
 * Leftover: data_ov102_0214e7d0..808 are this overlay's KCL/BMD/BCA
 *   handles (Init LoadFile / Cleanup Release). ov102 sinit constructs
 *   them; this TU does not own .bss.
 * Leftover: data_ov002_0210d9* / da40 / d954 are ov002 BMD/CLPS
 *   handles this TU loads for caps/contents. Naming belongs in ov002.
 * Leftover: data_ov002_0210da18 / da30 / da58 and gPFlower* stay
 *   char[] -- SharedFilePtr decls tip ov002 plurality (daObjBlockItemTag_c,
 *   Goomboss, daFeather, PowerFlower; S27). Init LoadFile still
 *   treats each slot as the model handle.
 * Leftover: data_ov102_0214e890 / e870 / e8c0 are sinit-owned PMF tables
 *   this TU does not own.
 * Leftover: data_02082214 is the NitroSDK FX_SinCosTable_; bounce squash
 *   indexes it by mBounceAng. Naming belongs with the SDK table.
 * Leftover: data_0209caa0 / data_0209f2d8 / data_0209f2f8 / data_0209f32c
 *   / data_0209f318 / data_0209e650 / data_020a0edc are arm9 globals.
 * Leftover: g_profile_HATENA_BLOCK / ITEM_BLOCK / VS_ITEM_BLOCK /
 *   CAP_BLOCK_* live outside this TU (S14).
 * Leftover: inline destructor (out-of-line emits D0 before D1).
 * Leftover: return new emits homeless _ZN10dBgActor_cD2Ev; licensed
 *   deadstrip (same helper ov045/daObjKm2_Ami_Bou_c records).
 * Leftover: OnAttacked1 / OnHitFromUnderneath valueless nested-if exits
 *   (C++ rejects valueless return in non-void).
 * Leftover: HbSpawnFrame stays file-scope so @class$ does not drift
 *   (Vector3 inline dtor; manifest compiler_only).
 * Leftover: (Vector3 *)&mPosX puns in the spawn helpers -- dActor_c.h
 *   has no Pos() on this tree (S18 uniquifiers). Do not emit extra
 *   Vector3 D1 (S3/S23); the copies here are licensed deadstrip-duplicate.
 */

/* common.h first: see leftover above. */
#include "common.h"
#include "types.h"
#include "daObjHatenaBlock_c.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"

struct CLPS_Block;
struct KCL_File;
struct BCA_File;

struct V3 {
  int x, y, z;
  V3(int a, int b, int d) { x = a; y = b; z = d; }
  V3() {}
};

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void call(dActor_c *player);
};

struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
struct C { char pad[0x3e8]; int idx; };

extern "C" {

extern signed char data_0209f2f8;
extern int data_0209f32c;
extern int data_0209e650;
extern unsigned char data_0209f2d8;
extern char data_0209f318[];
extern char data_020a0edc[];
extern int data_0209caa0[];
extern s16 data_02082214[];
extern Entry data_ov102_0214e890[];
extern PMF data_ov102_0214e870[][4];
extern PMF data_ov102_0214e8c0[][4];

extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov002_0210d9e0;
/* da18 / da30 / da58 / gPFlower*: SharedFilePtr here tips ov002
   plurality (daObjBlockItemTag_c, Goomboss, daFeather, PowerFlower). S27. */
extern char data_ov002_0210da18[];
extern char data_ov002_0210da30[];
extern SharedFilePtr data_ov002_0210da40;
extern char data_ov002_0210da58[];
extern char gPFlowerOpenModelFile[];
extern char gPFlowerCloseModelFile[];
extern CLPS_Block data_ov002_0210d954;

extern SharedFilePtr data_ov102_0214e7d0; /* KCL */
extern SharedFilePtr data_ov102_0214e7d8; /* CAP_BLOCK_L BMD */
extern SharedFilePtr data_ov102_0214e7e0; /* CAP_BLOCK_W BMD */
extern SharedFilePtr data_ov102_0214e7e8; /* HATENA_BLOCK BMD */
extern SharedFilePtr data_ov102_0214e7f0; /* CAP_BLOCK_M BMD */
extern SharedFilePtr data_ov102_0214e7f8; /* HATENA BCA */
extern SharedFilePtr data_ov102_0214e800; /* ITEM/VS BMD */
extern SharedFilePtr data_ov102_0214e808; /* HATENA anim BMD */

extern int RandomIntInternal(int *seed);
extern int Vec3_HorzDist(const void *a, const void *b);
extern int DecIfAbove0_Short(void *p);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void func_020393a4(int *p, int v);
extern void func_02039394(int *p, int v);
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, void *pos, s32 radius);
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *shadow, void *mtx, int fix, int t1, int t2, unsigned int n);
extern void _ZN8dActor_c11UntrackStarERa(void *self, void *p);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *thiz, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_ov102_0214ad14(void *actor);
extern void func_ov002_020f0438(void *actor);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y, int z);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);

int func_ov102_02149078(dActor_c *self);
void func_ov102_02149100(char *c, Vector3 *pos, int n, unsigned int speed, short arg5);
void *func_ov102_02149220(char *c);
void func_ov102_0214953c(char *c, int p1, int p2);
int func_ov102_02149610(char *c);
void func_ov102_02149684(int *dst, int *src);
void func_ov102_02149da8(C *c, int i);
void func_ov102_02149df0(C *c);
void func_ov102_02149e38(char *self);
void func_ov102_02149ea4(char *c);
void func_ov102_02149ff0(char *c);
}

// @symbol daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK
extern "C" daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK()
{
    return new daObjHatenaBlock_c();
}

// @symbol daObjHatenaBlock_c_classInit_HATENA_BLOCK
extern "C" daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_HATENA_BLOCK()
{
    return new daObjHatenaBlock_c();
}

// @symbol daObjHatenaBlock_c_classInit_ITEM_BLOCK
extern "C" daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_ITEM_BLOCK()
{
    return new daObjHatenaBlock_c();
}

// @symbol daObjHatenaBlock_c_classInit_CAP_BLOCK_M
extern "C" daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_CAP_BLOCK_M()
{
    return new daObjHatenaBlock_c();
}

// @symbol daObjHatenaBlock_c_classInit_CAP_BLOCK_L
extern "C" daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_CAP_BLOCK_L()
{
    return new daObjHatenaBlock_c();
}

// @symbol daObjHatenaBlock_c_classInit_CAP_BLOCK_W
extern "C" daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_CAP_BLOCK_W()
{
    return new daObjHatenaBlock_c();
}

// @symbol _ZN18daObjHatenaBlock_c13InitResourcesEv
int daObjHatenaBlock_c::InitResources()
{
    void *r5 = 0;
    switch (actorID - 0x14) {
    case 0:
        r5 = Model::LoadFile(data_ov102_0214e7e8);
        mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov102_0214e808), 1, 0x19);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &mModelAnim, Animation::LoadFile(data_ov102_0214e7f8), 0, 0x1000, 0);
        Model::LoadFile(data_ov002_0210d9e0);
        break;
    case 1:
    case 2:
        r5 = Model::LoadFile(data_ov102_0214e800);
        break;
    case 3:
        r5 = Model::LoadFile(data_ov102_0214e7f0);
        Model::LoadFile(data_ov002_0210da40);
        Model::LoadFile(data_ov002_0210d9e0);
        break;
    case 5:
        r5 = Model::LoadFile(data_ov102_0214e7d8);
        Model::LoadFile(data_ov002_0210d9a0);
        Model::LoadFile(data_ov002_0210d9e0);
        break;
    case 4:
        r5 = Model::LoadFile(data_ov102_0214e7e0);
        Model::LoadFile(data_ov002_0210d9c0);
        Model::LoadFile(data_ov002_0210d9e0);
        break;
    }

    mModel.SetFile((BMD_File *)r5, 1, -1);
    mShadowModel.InitCuboid();
    func_ov102_02149da8((C *)this, 0);
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov102_02149ff0((char *)this);
    func_ov102_02149e38((char *)this);
    mShadowMat = mModel.mat4x3;
    {
        void *kcl = dBgW_Kc::LoadFile(data_ov102_0214e7d0);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, &data_ov002_0210d954);
    }
    mHomePosY = mPosY;
    mContentType = (unsigned char)param1;
    if (mContentType == 0xff)
        mContentType = 0;
    if (mContentType == 1) {
        mStarId = (unsigned char)(param1 >> 8);
        if (mStarId == 0xff)
            mStarId = 0;
        mStarTracked = TrackStar(mStarId, 2);
    }

    {
        int b16 = !(actorID != 0x16);
        if (b16 != 0) {
            mContentType = 0;
            Model::LoadFile(*(SharedFilePtr *)data_ov002_0210da58);
        }
    }

    {
        int b14 = !(actorID != 0x14);
        if (b14 == 0) {
            int b15 = !(actorID != 0x15);
            if (b15 == 0)
                goto end;
        }
    }

    switch (mContentType) {
    case 0:
    case 1:
        break;
    case 3:
        Model::LoadFile(*(SharedFilePtr *)data_ov002_0210da18);
        break;
    case 2:
        Model::LoadFile(data_ov002_0210d9d8);
        break;
    case 4:
        Model::LoadFile(*(SharedFilePtr *)data_ov002_0210da30);
        break;
    case 7:
        Model::LoadFile(*(SharedFilePtr *)gPFlowerOpenModelFile);
        Model::LoadFile(*(SharedFilePtr *)gPFlowerCloseModelFile);
        Model::LoadFile(data_ov002_0210d9e0);
        break;
    case 5:
        Model::LoadFile(*(SharedFilePtr *)data_ov002_0210da58);
        Model::LoadFile(*(SharedFilePtr *)gPFlowerOpenModelFile);
        Model::LoadFile(*(SharedFilePtr *)gPFlowerCloseModelFile);
        break;
    case 6:
        Model::LoadFile(*(SharedFilePtr *)gPFlowerOpenModelFile);
        Model::LoadFile(*(SharedFilePtr *)gPFlowerCloseModelFile);
        break;
    }
end:
    return 1;
}

// @symbol _ZN18daObjHatenaBlock_c8BehaviorEv
int daObjHatenaBlock_c::Behavior()
{
    func_ov102_02149df0((C *)this);
    if (mState != 2) {
        UpdatePos(0);
        if (mPosY <= mHomePosY) mPosY = mHomePosY;
        func_ov102_02149ff0((char *)this);
        func_ov102_02149ea4((char *)this);
    }
    func_020393a4((int *)&mMeshCollider, 0x8c000);
    func_02039394((int *)&mMeshCollider, 0x46000);
    if (mState != 0) {
        if (mMeshCollider.IsEnabled() != 0) {
            mMeshCollider.Disable();
        }
        goto end;
    }
    if ((data_0209caa0[1] & 0x80000000) == 0) {
        int b = (int)(actorID == 0x14);
        if (b != 0) {
            mModelAnim.Advance();
            if (mMeshCollider.IsEnabled() != 0) {
                mMeshCollider.Disable();
            }
            goto end;
        }
    }
    {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) {
            if (mMeshCollider.IsEnabled() == 0) {
                mMeshCollider.Enable(this);
            }
            goto end;
        }
    }
    if (data_0209f2f8 == 0x1c) {
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x460000, 0x46000) != 0) {
            func_ov102_02149e38((char *)this);
        }
        goto end;
    }
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x118000, 0x46000) != 0) {
        func_ov102_02149e38((char *)this);
    }
end:
    return 1;
}

// @symbol _ZN18daObjHatenaBlock_c6RenderEv
int daObjHatenaBlock_c::Render()
{
    if (mState == 2)
        goto done;
    if ((*(int*)((char*)data_0209caa0 + 4) & 0x80000000) == 0) {
        int b = (actorID == 0x14);
        if (b != 0) {
            mModelAnim.Render(0);
            goto done;
        }
    }
    {
        mModel.Render((const Vector3 *)&mScaleX);
    }
done:
    return 1;
}

// @symbol _ZN18daObjHatenaBlock_c16CleanupResourcesEv
int daObjHatenaBlock_c::CleanupResources()
{
    int b, b2, b3;
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    b = (int)(actorID == 0x16);
    if (b)
        ((SharedFilePtr *)data_ov002_0210da58)->Release();

    b2 = (int)(actorID == 0x14);
    if (b2)
        goto dosw;
    b3 = (int)(actorID == 0x15);
    if (b3) {
    dosw:
        switch (mContentType) {
        case 1:
            _ZN8dActor_c11UntrackStarERa((char *)this, (char *)this + 0x3f0);
            break;
        case 3:
            ((SharedFilePtr *)data_ov002_0210da18)->Release();
            break;
        case 2:
            data_ov002_0210d9d8.Release();
            break;
        case 4:
            ((SharedFilePtr *)data_ov002_0210da30)->Release();
            break;
        case 7:
            ((SharedFilePtr *)gPFlowerOpenModelFile)->Release();
            ((SharedFilePtr *)gPFlowerCloseModelFile)->Release();
            data_ov002_0210d9e0.Release();
            break;
        case 5:
            ((SharedFilePtr *)data_ov002_0210da58)->Release();
            ((SharedFilePtr *)gPFlowerOpenModelFile)->Release();
            ((SharedFilePtr *)gPFlowerCloseModelFile)->Release();
            break;
        case 6:
            ((SharedFilePtr *)gPFlowerOpenModelFile)->Release();
            ((SharedFilePtr *)gPFlowerCloseModelFile)->Release();
            break;
        }
    }

    switch (actorID - 0x14) {
    case 0:
        data_ov102_0214e7e8.Release();
        data_ov102_0214e808.Release();
        data_ov102_0214e7f8.Release();
        data_ov002_0210d9e0.Release();
        break;
    case 1:
    case 2:
        data_ov102_0214e800.Release();
        break;
    case 3:
        data_ov102_0214e7f0.Release();
        data_ov002_0210da40.Release();
        data_ov002_0210d9e0.Release();
        break;
    case 5:
        data_ov102_0214e7d8.Release();
        data_ov002_0210d9a0.Release();
        data_ov002_0210d9e0.Release();
        break;
    case 4:
        data_ov102_0214e7e0.Release();
        data_ov002_0210d9c0.Release();
        data_ov002_0210d9e0.Release();
    }
    data_ov102_0214e7d0.Release();
    return 1;
}

// @symbol func_ov102_02149ff0
extern "C" {
void func_ov102_02149ff0(char *c)
{
    Matrix4x3_FromRotationY(c + 0xf0, *(short *)(c + 0x8e));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = (*(int *)(c + 0x60) + *(int *)(c + 0x3dc)) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;
    if (data_0209caa0[1] & 0x80000000)
        return;
    int b = (int)(*(unsigned short *)(c + 0xc) == 0x14);
    if (b == 0)
        return;
    *(struct Matrix4x3 *)(c + 0x33c) = *(struct Matrix4x3 *)(c + 0xf0);
}
}

// @symbol func_ov102_02149ea4
extern "C" {
void func_ov102_02149ea4(char *c)
{
    int r4, r5, r2v;
    int b0, b1;

    b0 = (*(int*)(c + 0xb0) & 8) ? 1 : 0;
    if (b0 != 0) {
        b1 = (*(volatile unsigned char *)&data_0209f2d8 == 1) ? 1 : 0;
        if (b1 == 0) goto skipcall;
    }
    *(int*)(c + 0x3e4) = func_ov102_02149610(c);
skipcall:
    r5 = *(int*)(c + 0x60) - *(int*)(c + 0x3e4);
    if (r5 <= 0x1000) r5 = 0x1000;
    r4 = (int)(((long long)r5 * 0x180 + 0x800) >> 12);
    r4 = 0xb4000 - r4;
    r2v = r5 + 0x214000;
    if (r4 < 0xa000) r4 = 0xa000;
    if (r2v < 0x200000) r2v = 0x200000;
    *(int*)(c + 0xb4) = -((int)((r5 + 0x14000) + ((unsigned)(r5 + 0x14000) >> 31)) >> 1);
    *(int*)(c + 0xb8) = (int)(r2v + ((unsigned)r2v >> 31)) >> 4;
    r4 = (int)(((long long)r4 * *(int*)(c + 0x80) + 0x800) >> 12);
    Matrix4x3_FromRotationY(c + 0x3ac, *(short*)(c + 0x8e));
    *(int*)(c + 0x3d0) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3d4) = (*(int*)(c + 0x60) - 0x20000) >> 3;
    *(int*)(c + 0x3d8) = *(int*)(c + 0x64) >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x384, c + 0x3ac, r4, r5 + 0x14000, r4, 0xf);
}
}

// @symbol func_ov102_02149e38
extern "C" {

struct MMC { char p[0x124]; };
struct HbMbgObj { char p[0x2ec]; Matrix4x3 m; };
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(MMC*, Matrix4x3&, short);
void func_ov102_02149e38(char* self){
    HbMbgObj* o = (HbMbgObj*)self;
    o->m = *(Matrix4x3*)(self + 0xf0);
    *(int*)(self+0x310) = *(int*)(self+0x5c);
    *(int*)(self+0x314) = *(int*)(self+0x60) + *(int*)(self+0x3dc);
    *(int*)(self+0x318) = *(int*)(self+0x64);
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s((MMC*)(self+0x124), o->m, *(short*)(self+0x8e));
}
}

// @symbol func_ov102_02149df0
extern "C" void func_ov102_02149df0(C *c) { int j = c->idx; (c->*data_ov102_0214e890[j].pmf[1])(); }

// @symbol func_ov102_02149da8
extern "C" void func_ov102_02149da8(C *c, int i) { c->idx = i; int j = c->idx; (c->*data_ov102_0214e890[j].pmf[0])(); }

// @symbol func_ov102_02149d80
extern "C" {
void func_ov102_02149d80(void *c) {
    *(s32*)((char*)c + 0x3dc) = 0;
    *(s32*)((char*)c + 0x80) = 0x1000;
    *(s32*)((char*)c + 0x84) = 0x1000;
    *(s32*)((char*)c + 0x88) = 0x1000;
    *(s32*)((char*)c + 0x9c) = -0x8000;
}
}

// @symbol func_ov102_02149ccc
extern "C" void func_ov102_02149ccc(char *self)
{
    dActor_c *player;
    if (!(data_0209caa0[1] & 0x80000000)) {
        int b = (int)(*(unsigned short *)(self + 0xc) == 0x14);
        if (b)
            return;
    }
    player = (dActor_c *)((dActor_c *)self)->ClosestPlayer();
    if (*(unsigned char *)((char *)player + 0x703) == 0)
        return;
    if (Vec3_HorzDist(self + 0x5c, (char *)player + 0x5c) >= 0xc8000)
        return;
    if (*(int *)(self + 0x60) <= *(int *)((char *)player + 0x60))
        return;
    ((Obj *)self)->call(player);
}

// @symbol func_ov102_02149c78
extern "C" {
void func_ov102_02149c78(void *c)
{
    s32 vec[3];
    void *r4 = c;
    vec[0] = *(s32*)((char*)r4 + 0x5c);
    vec[1] = *(s32*)((char*)r4 + 0x60);
    vec[2] = *(s32*)((char*)r4 + 0x64);
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(r4, vec, 0x5dc000);
    *(u16*)((char*)r4 + 0x3ec) = 0x4000;
    *(u16*)((char*)r4 + 0x3ee) = 7;
}
}

// @symbol func_ov102_021498e0
extern "C" void func_ov102_021498e0(C *self)
{
    char *c = (char *)self;
    Vector3 pos;
    int ch;
    u16 typ;
    void *held;
    func_ov102_02149684((int *)&pos, (int *)c);
    if (DecIfAbove0_Short((s16 *)(c + 0x3ee)) != 0) {

        s16 *pang = (s16 *)((int)(c + 0x3ec));
        u16 ang = *(u16 *)(c + 0x3ec);
        s16 s = data_02082214[(ang >> 4) * 2];
        int conf = 0x999;
        int t = (int)s + 0x1000;
        int y = (int)(((s64)t * conf + 0x800) >> 12);
        *(int *)(c + 0x84) = y + 0x666;
        ang = *(u16 *)(c + 0x3ec);
        s = data_02082214[(ang >> 4) * 2];
        int u = 0x1000 - (int)s;
        *(int *)(c + 0x80) = (int)(((s64)u * 0x1000 + 0x800) >> 12) + 0x1000;
        *(int *)(c + 0x88) = *(int *)(c + 0x80);
        ang = *(u16 *)(c + 0x3ec);
        s = data_02082214[(ang >> 4) * 2];
        *(int *)(c + 0x3dc) = (0x1000 - (int)s) * 0xd;
        *pang += 0x1000;

        return;
    }
    held = *(void **)(c + 0x3f4);
    if (held != 0) {
        if (*(u16 *)((char *)held + 0xc) == 0x149)
            func_ov002_020f0438(held);
        *(void **)(c + 0x3f4) = 0;
    }
    Sound::PlayBank3(0, *(Vector3 *)(c + 0x74));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd, pos.x, pos.y, pos.z);
    typ = *(u16 *)(c + 0xc);
    switch (typ - 0x14) {
    case 1: case 2:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10, pos.x, pos.y, pos.z); break;
    case 0:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc, pos.x, pos.y, pos.z); break;
    case 3: case 4: case 5:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(9, pos.x, pos.y, pos.z); break;
    }
    ch = (int)*(u8 *)(c + 0x3f2);
    if (ch < 0) ch = 0; else if (ch > 3) ch = 3;
    typ = *(u16 *)(c + 0xc);
    switch (typ - 0x14) {
    case 1: {
        PMF (*tbl)[4] = data_ov102_0214e8c0;
        u8 content = *(u8 *)(c + 0x3f3);
        (self->*tbl[content][0])();
        break;
    }
    case 2: {
        PMF (*tbl)[4] = data_ov102_0214e870;
        u8 content = *(u8 *)(c + 0x3f3);
        if (ch >= 4) ch = 0;
        (self->*tbl[content][ch])();
        break;
    }
    case 0:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0 || data_0209f2f8 == 0x1f) {
            PMF (*tbl)[4] = data_ov102_0214e8c0;
            u8 content = *(u8 *)(c + 0x3f3);
            if (ch >= 4) ch = 0;
            (self->*tbl[content][ch])();
        } else {
            func_ov102_02149220(c);
        }
        break;
    case 3:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) func_ov102_0214953c(c, 0, 0x12);
        else func_ov102_02149220(c);
        break;
    case 5:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) func_ov102_0214953c(c, 1, 0x12);
        else func_ov102_02149220(c);
        break;
    case 4:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) func_ov102_0214953c(c, 2, 0x12);
        else func_ov102_02149220(c);
        break;
    }
    func_ov102_02149da8((C *)c, 2);
}

// @symbol func_ov102_021498c4
extern "C" {
void func_ov102_021498c4(void *p) {
    *(short *)((char *)p + 0x3ee) = 0x12c;
    *(int *)((char *)p + 0x9c) = -0x8000;
}
}

// @symbol func_ov102_02149878
extern "C" {
int func_ov102_02149878(char *c)
{
    int r = DecIfAbove0_Short(c + 0x3ee);
    if (r != 0) return r;
    r = ((dActor_c *)c)->DistToCPlayer();
    if (r <= 0x64000) return r;
    func_ov102_02149da8((C *)c, 0);
}
}

// @symbol _ZN18daObjHatenaBlock_c15OnGroundPoundedER8dActor_c
void daObjHatenaBlock_c::OnGroundPounded(dActor_c &other)
{
    if (mState == 1) return;
    int r = func_ov102_02149078(this);
    if (r != 0) return;
    mHitterParam = other.param1;
    func_ov102_02149da8((C *)this, 1);
}

// @symbol _ZN18daObjHatenaBlock_c11OnAttacked1ER8dActor_c
int daObjHatenaBlock_c::OnAttacked1(dActor_c &other)
{
    int v = mState;
    if (v != 1) {
        if (!func_ov102_02149078(this)) {
            int val = other.param1;
            mHitterParam = val;
            func_ov102_02149da8((C *)this, 1);
        }
    }
}

// @symbol _ZN18daObjHatenaBlock_c8OnKickedER8dActor_c
void daObjHatenaBlock_c::OnKicked(dActor_c &other)
{
    if (mState == 1) return;
    int r = func_ov102_02149078(this);
    if (r != 0) return;
    mHitterParam = other.param1;
    func_ov102_02149da8((C *)this, 1);
}

// @symbol _ZN18daObjHatenaBlock_c15OnHitByMegaCharER6Player
void daObjHatenaBlock_c::OnHitByMegaChar(Player &player)
{
    if (mState == 1) return;
    if (func_ov102_02149078(this) != 0) return;
    player.IncMegaKillCount();
    mHitterParam = player.param1;
    func_ov102_02149da8((C *)this, 1);
}

// @symbol _ZN18daObjHatenaBlock_c19OnHitFromUnderneathER8dActor_c
int daObjHatenaBlock_c::OnHitFromUnderneath(dActor_c &other)
{
    if (mState != 1) {
        mVertAccel = -0x8000;
        mVertSpeed = 0x1e000;
        if (!func_ov102_02149078(this)) {
            mHitterParam = other.param1;
            func_ov102_02149da8((C *)this, 1);
        }
    }
}

// @symbol func_ov102_02149684
extern "C" {
void func_ov102_02149684(int* dst, int* src){
  int v3 = src[0x19];
  int v2 = src[0x18] + 0x32000;
  dst[0] = src[0x17];
  dst[1] = v2;
  dst[2] = v3;
}
}

// @symbol func_ov102_02149610
extern "C" int func_ov102_02149610(char *c){
  V3 pos(*(int*)(c+0x5c), *(int*)(c+0x60)+0x28000, *(int*)(c+0x64));
  dBgCh_Gnd rg;
  rg.SetObjAndPos(*(Vector3*)&pos, 0);
  rg.mProbeHeight = 0x3e8000;
  int r = *(int*)(c+0x60);
  if (rg.DetectClsn()) r = rg.clsnY;
  return r;
}

struct HbSpawnFrame { Vector3 pos; int vel[3]; };

// @symbol func_ov102_0214953c
extern "C" {
void func_ov102_0214953c(char* c, int p1, int p2)
{
    HbSpawnFrame f;
    int rnd;
    char* o;
    char* g;
    func_ov102_02149684((int*)&f.pos, (int *)c);
    o = (char*)dActor_c::Spawn(
        0x10d, (unsigned int)(p2 | (p1 << 8)), f.pos, 0, *(signed char*)(c + 0xcc), -1);
    if (o == 0) return;
    g = *(char**)data_0209f318;
    f.vel[0] = 0;
    f.vel[1] = 0x11000;
    f.vel[2] = 0;
    rnd = RandomIntInternal(&data_0209e650);
    {
        int vsum = rnd + (*(short*)(g + 0x17c) + 0x8000);
        *(short*)(o + 0x92) = 0;
        *(short*)(o + 0x94) = (short)vsum;
        *(short*)(o + 0x96) = 0;
        *(int*)(o + 0x98) = 0x3320;
        *(int*)(o + 0xa4) = f.vel[0];
        *(int*)(o + 0xa8) = f.vel[1];
        *(int*)(o + 0xac) = f.vel[2];
    }
}
}

// @symbol func_ov102_021494cc
extern "C" {
int func_ov102_021494cc(char* c){
  int s[3];
  func_ov102_02149684(s, (int*)c);
  int r2 = (*(unsigned int*)(c+8) >> 8) & 0xff;
  if(r2 == 0xff) r2 = 1;
  int w[3];
  w[0] = s[0]; w[1] = s[1]; w[2] = s[2];
  func_ov102_02149100(c, (Vector3 *)w, r2, 0x1800, 0);
  return _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}
}

// @symbol func_ov102_02149478
extern "C" {
void func_ov102_02149478(char* c){
  char local[12];
  func_ov102_02149684((int *)local, (int *)c);
  dActor_c::Spawn(0xb2, *(unsigned char*)(c+0x3f1)|0x40, *(Vector3 *)local, 0, *(signed char*)(c+0xcc), -1);
  ((fBase_c *)c)->MarkForDestruction();
}
}

// @symbol func_ov102_02149428
extern "C" {
void func_ov102_02149428(char* c){
  struct Vector3 pos;
  func_ov102_02149684((int *)&pos, (int *)c);
  dActor_c::Spawn(0x114, 0, pos, 0, *(signed char*)(c+0xcc), -1);
  ((dActor_c *)c)->KillAndTrackInDeathTable();
}
}

// @symbol func_ov102_021493dc
extern "C" void func_ov102_021493dc(void* c) {
    Vector3 v;
    func_ov102_02149684((int *)&v, (int *)c);
    signed char cc = *(signed char*)((char*)c+0xcc);
    dActor_c::Spawn(0x115, 0, v, (const Vector3_16*)0, cc, -1);
}

// @symbol func_ov102_02149384
extern "C" {

void* func_ov102_02149384(void* c){
  Vector3 v;
  func_ov102_02149684((int *)&v, (int *)c);
  void* a=dActor_c::Spawn(0x11d,0,v,0,*(signed char*)((char*)c+0xcc),-1);
  if(a) *(unsigned char*)((char*)a+0x3c6)=0xb4;
  return a;
}
}

// @symbol func_ov102_021492d4
extern "C" {
void func_ov102_021492d4(char* c) {
  struct Vector3 pos;
  struct Vector3_16 rot;
  func_ov102_02149684((int*)&pos, (int*)c);
  rot.x = *(u16*)(data_020a0edc);
  rot.y = *(u16*)(data_020a0edc+2);
  rot.z = *(u16*)(data_020a0edc+4);
  if ((int)(*(unsigned char*)(&data_0209f2d8) == 1) != 0) {
    rot.y = *(s16*)(c+0x8e);
  } else {
    rot.y = *(s16*)(*(char**)data_0209f318 + 0x17c) + 0x4000;
  }
  dActor_c::Spawn(0x159, 0, pos, &rot, *(signed char*)(c+0xcc), -1);
}
}

// @symbol func_ov102_02149288
extern "C" void func_ov102_02149288(char* c){
    Vector3 v;
    func_ov102_02149684((int *)&v, (int *)c);
    dActor_c::Spawn(0x132u, 0u, v, 0, *(signed char*)(c+0xcc), -1);
}

// @symbol func_ov102_02149220
extern "C" {
extern void* func_ov102_0214b384(void*, int);
void* func_ov102_02149220(char* c){
  Vector3 v;
  func_ov102_02149684((int *)&v, (int *)c);
  void* a = dActor_c::Spawn(0xce, 4, v, 0, *(signed char*)(c+0xcc), -1);
  if(a == 0) return a;
  func_ov102_0214ad14(a);
  return func_ov102_0214b384(a, 0x3c);
}
}

// @symbol func_ov102_02149100
extern "C" {
void func_ov102_02149100(char *c, Vector3 *pos, int n, unsigned int speed, short arg5)
{
    char *a;
    int dir;
    unsigned int rnd;
    unsigned int q;
    int prevDir = 0xff;
    int i = 0;

    if (n <= 0) return;

    do {
        a = (char *)dActor_c::Spawn(0x120, 2, *pos, 0, *(signed char*)(c+0xcc), -1);
        if (a != 0) {
            do {
                rnd = (unsigned int)RandomIntInternal(&data_0209e650);
                dir = (int)((rnd >> 0x10) << 0x1b) >> 0x10;
            } while (dir == prevDir);
            rnd = (unsigned int)RandomIntInternal(&data_0209e650);
            *(short*)(a+0x92) = 0;
            q = rnd >> 0x10;
            speed = (speed * (q % 50 + 100)) / 100;
            prevDir = dir;
            *(short*)(a+0x94) = arg5 + dir;
            *(short*)(a+0x96) = 0;
            *(unsigned int*)(a+0x98) = speed;
        }
        i++;
    } while (i < n);
}
}

// @symbol func_ov102_02149078
extern "C" {
int func_ov102_02149078(dActor_c *self)
{
    if (data_0209f2f8 == 0x15) {
        if ((int)(*(int*)((char*)self + 0x3e0) - 0x32000) <= data_0209f32c)
            return 1;
    } else {
        if (data_0209f2f8 == 0x21)
            return 0;
        if (*(unsigned char*)((char*)self->ClosestPlayer() + 0x706))
            return 1;
    }
    return 0;
}
}
