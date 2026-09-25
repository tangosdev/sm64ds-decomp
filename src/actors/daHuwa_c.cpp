//cpp
/* daHuwa_c, the Spindrift. ov081 0x02123740..0x02124040, twelve functions.
 *
 * The cartridge spells the class 8daHuwa_c at 0x02128840. _ZTI8daHuwa_c at
 * 0x0212884c is the __si_class_type_info record (base _ZTI12dEnemyBase_c),
 * and the word before _ZTV8daHuwa_c (0x0212887c) points at that typeinfo.
 * The header used to coin the class Spindrift.
 *
 * The run is the whole class and nothing else: D1, D0, OnAimedAtWithEgg,
 * three file-local helpers, CleanupResources, Render, Behavior,
 * InitResources, OnTurnIntoEgg, OnYoshiTryEat. daHuwa_c_classInit at
 * 0x02124040 abuts the end and stays out of this compiler input.
 *
 * #pragma defer_codegen off emits .text in source order, so this file is
 * ROM-ascending. One out-of-line destructor is the key function, so this TU
 * emits _ZTV/_ZTI/_ZTS: D1 (0x02123740), D0 (0x02123788), then a D2 the
 * cartridge has no home for (manifest: deadstrip).
 *
 * common.h is first so its flat Matrix4x3 { s32 m[12]; } is seen before
 * daHuwa_c.h reaches math/Matrix.h. func_ov081_021237ec copies that matrix
 * through .m. decl_common.h is not included: it prototypes the three
 * helpers as void(void*), and the definitions that match are void(char*).
 */

#pragma defer_codegen off

#include "common.h"
#include "decl_Enemy.h"
#include "decl_Player.h"
#include "daHuwa_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

/* func_ov081_02123910 calls vtable slot 29 (OnAimedAtWithEgg) through a
 * view with that slot at index 29. The object's own table supplies the
 * function; this type only fixes the index. */
struct HuwaSlot29 {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int m29();
};

/* Render calls ModelAnim slot 5. The argument is an int, not Vector3 const *. */
struct HuwaModelView {
    virtual void m0(); virtual void m1(); virtual void m2();
    virtual void m3(); virtual void m4(); virtual void m5(int);
};

/* Spell the ROM's own symbols. The shards once called these through local
 * stand-in classes whose methods mangled to names that exist nowhere
 * (_ZN7dCcAc_c4InitEP8dActor_ciijj and friends). match.py wildcards every
 * relocated word, so only check_references saw it; these are the real symbols,
 * and the bytes are unchanged. */
extern "C" {
/* Same spellings InitResources matched under: a char, and an int array
 * whose [1] is the animation file pointer. */
extern char data_ov081_02128d60;
extern int data_ov081_02128d68[];

int _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void* c, Vector3* v, unsigned int n, int f, short s);
void Matrix4x3_FromTranslation(struct Matrix4x3* m, int x, int y, int z);
void MulMat4x3Mat4x3(const int* a, const int* b, int* dst);
void SubVec3(Vector3* a, Vector3* b, Vector3* c);
void Vec3_LslInPlace(Vector3* v, int n);
void AddVec3(Vector3* a, Vector3* b, Vector3* c);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
void func_0201267c(int a, void* b);
extern struct Matrix4x3 data_020a0e68;
extern int func_ov002_020e10a8(void*);
extern short Vec3_HorzAngle(void* a, void* b);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, unsigned n, int f, unsigned a, unsigned b, unsigned c);
void Matrix4x3_FromRotationY(void* m, int angle);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* thiz, ShadowModel& sm, Matrix4x3& mtx, Fix12i a, Fix12i b, unsigned int c);
void _Z14ApproachLinearRiii(void *, int, int);
int Vec3_HorzDist(void *a, void *b);
void _Z14ApproachLinearRsss(void *, int, int);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *f, int b, int c, unsigned int d);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *a, int r, int h, unsigned int d, unsigned int e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *a, int r, int h, void *v, void *w);
}

/* One written destructor. The compiler emits D1 then D0 and anchors
 * _ZTV8daHuwa_c. The members D1 used to destroy by hand are typed members of
 * daHuwa_c now (see daHuwa_c.h for the two layout witnesses), so the compiler
 * emits the same chain. D0 is the deleting destructor: it destroys through
 * this class and its bases, which is why more than one vptr store appears,
 * then frees through an inline operator delete, which is why nothing here
 * mentions a heap. */
// @symbol _ZN8daHuwa_cD1Ev
// @symbol _ZN8daHuwa_cD0Ev
daHuwa_c::~daHuwa_c()
{
}

/* Vtable slot 29: _ZTV8daHuwa_c + 0x74 -> 0x021237e4. */
// @symbol _ZN8daHuwa_c16OnAimedAtWithEggEv
s32 daHuwa_c::OnAimedAtWithEgg() {
    return 245760;
}

// @symbol func_ov081_021237ec
extern "C" void func_ov081_021237ec(char* c)
{
    Vector3 t;
    Vector3 v;
    Vector3 b;

    t.x = *(int*)(c + 0x5c);
    t.y = *(int*)(c + 0x60);
    t.z = *(int*)(c + 0x64);
    t.y += *(int*)(c + 0x1a4) - 0x50000;
    v = t;
    ((dActor_c *)c)->PoofDustAt(v);

    b.x = *(int*)(c + 0x5c);
    b.y = *(int*)(c + 0x60);
    b.z = *(int*)(c + 0x64);
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &b, 3, 0xa000, 0);

    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    MulMat4x3Mat4x3((const int*)(*(char**)(c + 0x124) + 0x30), data_020a0e68.m, data_020a0e68.m);

    t.x = data_020a0e68.m[9];
    t.y = data_020a0e68.m[10];
    t.z = data_020a0e68.m[11];
    SubVec3(&t, (Vector3*)(c + 0x5c), &t);
    Vec3_LslInPlace(&t, 3);
    AddVec3(&t, (Vector3*)(c + 0x5c), &t);

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x53, t.x, t.y, t.z);
    func_0201267c(0xd5, c + 0x74);
    ((dActor_c *)c)->KillAndTrackInDeathTable();
}

// @symbol func_ov081_02123910
extern "C" void func_ov081_02123910(char* c)
{
    void* other;
    unsigned int id;
    int flags;
    int isPlayer;
    Vector3_16 kv;
    Vector3 pos;

    id = *(unsigned int*)(c + 0x1c0);
    if (id == 0)
        return;
    other = dActor_c::FindWithID(id);
    if (other == 0)
        return;

    flags = *(int*)(c + 0x1bc);
    if ((flags & 0x10) != 0) {
        int tmp;
        kv.x = (short)-0x2000;
        kv.y = 0;
        kv.z = 0;
        tmp = ((HuwaSlot29*)c)->m29();
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, &kv, other, tmp);
        return;
    }

    if ((flags & 0x26000) != 0) {
        func_ov081_021237ec(c);
        return;
    }

    isPlayer = (int)(*(unsigned short*)((char*)other + 0xc) == 0xbf);
    if (isPlayer == 0)
        return;

    if (*(unsigned char*)((char*)other + 0x6f9) == 0) {
        if (_ZN6Player9IsOnShellEv(other) == 0) {
            flags = *(int*)(c + 0x1bc);
            if ((flags & 0x40000) == 0)
                goto cont;
        }
    }
    func_ov081_021237ec(c);
    return;

cont:
    if ((flags & 0x26fe0) != 0) {
        if (func_ov002_020e10a8(other) == 0) {
            func_ov081_021237ec(c);
            return;
        }
    }

    if (((dActor_c *)c)->JumpedOnByPlayer(*(dCc_c *)(c + 0x19c), *(Player *)other) != 0) {
        _ZN6Player10SpinBounceE5Fix12IiE(other, 0x28000);
        func_ov081_021237ec(c);
        return;
    }

    if (*(unsigned char*)((char*)other + 0x6fb) != 0)
        return;
    if ((*(int*)(c + 0x1bc) & 0x400000) == 0)
        return;

    {
        unsigned char one = 1;
        int a;
        short* p;
        *(unsigned char*)(c + 0x39a) = one;
        a = 0xa000;
        p = (short*)(c + 0x100);
        *p = 0;
        *(int*)(c + 0x98) = -a;
    }
    *(short*)(c + 0x94) = Vec3_HorzAngle(c + 0x5c, (char*)other + 0x5c);

    pos.x = *(int*)(c + 0x5c);
    pos.y = *(int*)(c + 0x60);
    pos.z = *(int*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &pos, 2, 0xc000, 1, 0, 1);
}

// @symbol func_ov081_02123b20
extern "C" void func_ov081_02123b20(char* thiz)
{
    char* c = thiz;
    Matrix4x3_FromRotationY(c + 0x12c, *(short*)(c + 0x8e));
    *(int*)(c + 0x150) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x154) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x158) = *(int*)(c + 0x64) >> 3;
    {
        int b = (int)((*(int*)(c + 0xb0) & 0x40000) != 0);
        if (b != 0) return;
    }
    if (((dBgCh_Actr *)(c + 0x1d0))->IsOnGround() != 0) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, *(ShadowModel*)(c + 0x174), *(Matrix4x3*)(c + 0x12c), 0x50000, 0x1e000, 0xf);
    } else {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, *(ShadowModel*)(c + 0x174), *(Matrix4x3*)(c + 0x12c), 0x50000, 0x96000, 0xf);
    }
}

// @symbol _ZN8daHuwa_c16CleanupResourcesEv
int daHuwa_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov081_02128d60)->Release();
    ((SharedFilePtr *)data_ov081_02128d68)->Release();
    return 1;
}

// @symbol _ZN8daHuwa_c6RenderEv
int daHuwa_c::Render()
{
  int b = (mFlags & 0x40000) ? 1 : 0; if (b) return 1;
  HuwaModelView* o = (HuwaModelView*)((char*)&mModelAnim);
  o->m5(0);
  return 1;
}

// @symbol _ZN8daHuwa_c8BehaviorEv
int daHuwa_c::Behavior()
{
    int r = ((dEnemyBase_c *)(((char *)this)))->UpdateKillByInvincibleChar(*(dBgCh_Actr *)(((char *)this) + 0x1d0), *(ModelAnim *)(((char *)this) + 0x110), 3);
    if (r != 0) {
        if (r == 2)
            func_ov081_021237ec(((char *)this));
        return 1;
    }

    if (((dEnemyBase_c *)(((char *)this)))->UpdateYoshiEat(*(dBgCh_Actr *)(((char *)this) + 0x1d0)) != 0) {
        if (_ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c(((char *)this), ((char *)this) + 0x19c) != 0)
            func_ov081_021237ec(((char *)this));
        func_ov081_02123b20(((char *)this));
        ((dCc_c *)((char *)&mdCcAc_c))->Clear();
        if (mEatenByYoshi != 0 && unk_104 == 0) {
            ((dCc_c *)((char *)&mdCcAc_c))->dCc_c::Update();
        }
        return 1;
    }

    ((dActor_c *)(((char *)this)))->MakeVanishLuigiWork(*(dCc_c *)(((char *)this) + 0x19c));
    func_ov081_02123910(((char *)this));

    switch (unk_39a) {
    case 0: {
            _Z14ApproachLinearRiii(((char *)this) + 0x98, 0x4000, 0x1000);
            void *cp = ((dActor_c *)(((char *)this)))->ClosestPlayer();
            if (cp != 0) {
                int *src = (int *)((int)cp + 0x5c);
                int v3[3];
                v3[0] = src[0];
                v3[1] = src[1];
                v3[2] = src[2];
                if (Vec3_HorzDist(((char *)this) + 0x38c, (void *)v3) > 0x3e8000) {
                    unk_398 = Vec3_HorzAngle(((char *)this) + 0x5c, ((char *)this) + 0x38c);
                } else if (Vec3_HorzDist(((char *)this) + 0x5c, (void *)v3) > 0x12c000) {
                    unk_398 = Vec3_HorzAngle(((char *)this) + 0x5c, (void *)v3);
                }
                goto after_st0;
            }
            unk_398 = Vec3_HorzAngle(((char *)this) + 0x5c, ((char *)this) + 0x38c);
        after_st0:
            _Z14ApproachLinearRsss(((char *)this) + 0x8e, unk_398, 0x200);
            mPrevAngleY = mAngleY;
        }
        break;
    case 1:
        *(unsigned short *)((int)((char *)this) + 0x100) += 1;
        if (*(unsigned short *)((int)((char *)this) + 0x100) >= 0x14)
            unk_39a = 0;
        break;
    }

    ((Animation *)((char *)(Animation *)&mModelAnim))->Advance();
    ((dActor_c *)(((char *)this)))->UpdatePos(0);
    ((dEnemyBase_c *)(((char *)this)))->UpdateWMClsn(*(dBgCh_Actr *)(((char *)this) + 0x1d0), 0);

    if (((dEnemyBase_c *)(((char *)this)))->IsGoingOffCliff(*(dBgCh_Actr *)(((char *)this) + 0x1d0), 0x3c000, 0x2888, 1, 1, 0x32000) != 0) {
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;
    }
    func_ov081_02123b20(((char *)this));
    ((dCc_c *)((char *)&mdCcAc_c))->Clear();
    ((dCc_c *)((char *)&mdCcAc_c))->dCc_c::Update();
    return 1;
}

// @symbol _ZN8daHuwa_c13InitResourcesEv
int daHuwa_c::InitResources()
{
    char *s = (char*)((dActor_c *)this);
    void *mf = Model::LoadFile(*(SharedFilePtr *)&data_ov081_02128d60);
    ((ModelBase *)(s + 0x110))->ModelBase::SetFile((BMD_File*)mf, 1, -1);
    Animation::LoadFile(*(SharedFilePtr *)data_ov081_02128d68);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(s + 0x110, (BCA_File*)data_ov081_02128d68[1], 0, 0x1000, 0);
    if (((ShadowModel *)(s + 0x174))->InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(s + 0x19c, ((dActor_c *)this), 0x3c000, 0x78000, 0x200000, 0xa6efe0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(s + 0x1d0, ((dActor_c *)this), 0x3c000, 0x3c000, 0, 0);
    ((dBgCh_Actr *)(s + 0x1d0))->StartDetectingWater();
    *(int*)(s + 0x38c) = *(int*)(s + 0x5c);
    *(int*)(s + 0x390) = *(int*)(s + 0x60);
    *(int*)(s + 0x394) = *(int*)(s + 0x64);
    *(char*)(s + 0x39a) = 0;
    *(int*)(s + 0x9c) = -0x2000;
    *(int*)(s + 0xa0) = -0x3c000;
    return 1;
}

/* Vtable slot 19: _ZTV8daHuwa_c + 0x4c -> 0x02123fd8 (formerly
 * func_ov081_02123fd8). */
// @symbol _ZN8daHuwa_c13OnTurnIntoEggER6Player
void daHuwa_c::OnTurnIntoEgg(Player &player)
{
    if (!player.IsCollectingCap())
        player.RegisterEggCoinCount(3, 0, 0);
    else
        GivePlayerCoins(player, 3, 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN8daHuwa_c13OnYoshiTryEatEv
s32 daHuwa_c::OnYoshiTryEat() {
    return 6;
}
