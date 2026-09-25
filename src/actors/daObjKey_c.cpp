//cpp
/* daObjKey_c, ov089. Matched run 0x02131b18..0x021327d0, ten functions.
 *
 * NAME: _ZTS10daObjKey_c is the cartridge string "10daObjKey_c" at
 * 0x02132b58. _ZTI10daObjKey_c at 0x02132b4c reads [__si_class_type_info,
 * 0x02132b58, _ZTI12dEnemyBase_c], so the single base is dEnemyBase_c, and
 * the word before _ZTV10daObjKey_c (0x02132ba8) is that _ZTI. The class was
 * coined Key before the ROM name was read (factory alias Key_Spawn).
 *
 * PARTIAL: this is not the whole class. The destructors stay shards. D1 at
 * 0x02130f00 and D0 at 0x02130f50 sit in front of the unmatched
 * func_ov089_0213162c, so one file cannot contain both them and this run.
 * func_ov089_02130fb4, UnloadKeyModels, LoadKeyModels, func_ov089_0213115c
 * and func_ov089_021311c0 stay shards for the same reason. The D1 shard
 * holds the out-of-line destructor, the key function, so it emits
 * _ZTV/_ZTI/_ZTS; this TU defines no destructor and emits no vtable.
 *
 * Source order is ROM-ascending. `#pragma defer_codegen off` emits each
 * function where it stands.
 */

#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "daObjKey_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "decl_dBgCh_Actr.h"

#pragma defer_codegen off

#define L(p) (p)
#define LAUNDER(p) (p)
#define PMFTABLE data_ov089_02132cec

struct C { virtual void dummy(); };
typedef void (C::*PMF)();
struct PmfEntry { PMF pmf; };
extern PmfEntry data_ov089_02132cec[];

extern "C" {
extern void dBgCh_Actr_UpdateContinuous_Veneer(char *p);
extern char *data_0209f318;
extern int data_0209b454;
extern void _ZN6Camera9SetFlag_3Ev(char *cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(char *cam, struct Vector3 *v);
extern void _ZN6Camera6SetPosERK7Vector3(char *cam, struct Vector3 *v);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int c, int d, int e, void *f, void *g);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char *p);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(char *p);
extern char *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_02012694(int a, char *v, int c);
extern int func_ov002_020c3dbc(int a);
extern void _ZN5Event6SetBitEj(unsigned int a);
extern void _ZN7fBase_c18MarkForDestructionEv(void *c);
extern int _ZN6Player17SetNoControlStateEhih(void *p, unsigned char a, int b, unsigned char d);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int a);
extern void func_ov089_0213115c(char *c, int i);
extern int data_0209caa0[];
extern unsigned char data_0209f2ac;
void Matrix4x3_FromRotationY(void *m, short angle);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *a, void *sm, void *mtx, int rad, int h, unsigned int x);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern char data_ov089_021328b4[];
extern int data_ov002_02110964;
extern void _ZN5Event8ClearBitEj(unsigned int b);
extern int data_ov089_02132c60[];
extern int data_ov089_02132c40[];
extern int data_ov089_02132c70[];
extern int data_ov089_02132c48[];
extern int data_ov089_02132b40[];
extern int data_ov089_02132ca4[];
extern char data_020a0e68;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, int ang);
extern void MulMat4x3Mat4x3(void *d, void *a, void *b);
extern void SubVec3(void *d, void *a, void *b);
extern void Vec3_LslInPlace(void *v, int sh);
extern void AddVec3(void *d, void *a, void *b);
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void *c, void *w);
extern void _ZN5dCc_c5ClearEv(void *c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *cyl);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *c, void *v);
extern void _ZN5dCc_c6UpdateEv(void *c);
extern void LoadKeyModels(int idx);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *bmd, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *thiz, void *actor, void *pos, int r, int s, unsigned int a, unsigned int b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int a, int b, void *v, void *w);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *pos);
extern int _ZN11ShadowModel12InitCylinderEv(void *thiz);
extern char data_ov002_0211094c;
extern int data_0209cef0;
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void*); };

void func_ov089_02131dcc(char *c, int a);
void func_ov089_02131df4(char *c, char *player);
}
/* -------------------------------------------------------------------------- */
/* func_ov089_02131b18, 0x02131b18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov089_02131b18
extern "C" {
void func_ov089_02131b18(char *c)
{
    char *cam = data_0209f318;
    struct Vector3 v;
    u8 st;

    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x260);

    st = *(u8 *)(c + 0x442);
    if (st == 0)
        goto case0;
    if (st == 1)
        goto case1;
    if (st == 2)
        goto case2;
    return;

case0:
    {
        int *b0 = (int *)(int)L(c + 0xb0);
        int *s1 = (int *)(int)L(cam + 0x80);
        int *s2 = (int *)(int)L(cam + 0x8c);
        *b0 |= 0x4000000;
        data_0209b454 |= 0x4000000;
        *(int *)(c + 0x44c) = s1[0];
        *(int *)(c + 0x450) = s1[1];
        *(int *)(c + 0x454) = s1[2];
        *(int *)(c + 0x458) = s2[0];
        *(int *)(c + 0x45c) = s2[1];
        *(int *)(c + 0x460) = s2[2];
        _ZN6Camera9SetFlag_3Ev(cam);
        v.x = *(int *)(c + 0x5c);
        v.y = *(int *)(c + 0x60);
        v.z = *(int *)(c + 0x64);
        v.y = v.y + 0x64000;
        _ZN6Camera9SetLookAtERK7Vector3(cam, &v);
        {
            u8 *p = (u8 *)(int)L(c + 0x442);
            *p = *p + 1;
        }
        return;
    }

case1:
    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    v.y = v.y + 0x64000;
    _ZN6Camera9SetLookAtERK7Vector3(cam, &v);
    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    v.y = v.y + 0x1c000;
    *(void **)(c + 0x464) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(c + 0x464), 0x81, v.x, v.y, v.z, 0, 0);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x260)) {
        int t = (-*(int *)(c + 0xa8)) >> 1;
        *(int *)(c + 0xa8) = t;
        func_02012694(0x36, c + 0x74, t);
        return;
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x260) == 0)
        return;
    _ZN6Camera9SetLookAtERK7Vector3(cam, (struct Vector3 *)(int)L(c + 0x44c));
    _ZN6Camera6SetPosERK7Vector3(cam, (struct Vector3 *)(int)L(c + 0x458));
    {
        int *cf = (int *)(int)L(cam + 0x154);
        int *b0 = (int *)(int)L(c + 0xb0);
        u8 *stp;
        int *p238;
        *cf &= ~8;
        *b0 &= ~0x4000000;
        data_0209b454 &= ~0x4000000;
        stp = (u8 *)(int)L(c + 0x442);
        *stp = *stp + 1;
        p238 = (int *)(int)L(c + 0x238);
        *p238 &= ~1;
        *(void **)(c + 0x464) = 0;
    }
    return;

case2:
    {
        u32 id = *(u32 *)(c + 0x244);
        char *found;
        if (id == 0)
            return;
        found = _ZN8dActor_c10FindWithIDEj(id);
        if (found == 0)
            return;
        if ((*(int *)(c + 0x240) & 0x400000) == 0)
            return;
        func_ov089_02131df4(c, found);
        {
            u8 *p = (u8 *)(int)L(c + 0x442);
            *p = *p + 1;
        }
        return;
    }
}
}

/* -------------------------------------------------------------------------- */
/* func_ov089_02131dcc, 0x02131dcc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov089_02131dcc
extern "C" {
void func_ov089_02131dcc(char *c, int a){
  func_ov002_020c3dbc(a);
  _ZN5Event6SetBitEj(0x1d);
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* func_ov089_02131df4, 0x02131df4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov089_02131df4
extern "C" {
void func_ov089_02131df4(char* c, char* player)
{
    if (data_0209caa0[1] & (2 << *(int*)(c + 0x444)))
        data_0209f2ac = 0;
    else
        data_0209f2ac = 1;
    data_0209caa0[1] |= (2 << *(int*)(c + 0x444));

    if (*(int*)(c + 0x444) <= 1) {
        _ZN6Player17SetNoControlStateEhih(player, 3, -1, 0);
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x17);
    } else if (*(int*)(c + 0x444) != 7) {
        _ZN6Player17SetNoControlStateEhih(player, 3, -1, 0);
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x17);
    }

    func_ov089_0213115c(c, 3);
    *(char**)(c + 0x110) = player;
    {
        char* pl = *(char**)(c + 0x110);
        /* force separate base materialization like ROM: r2=c+0xb0, r3=pl+0x5c */
        int* fl = (int*)(unsigned)(c + 0xb0);
        int* s = (int*)(unsigned)(pl + 0x5c);
        int t0 = s[0];
        int ev = 0x1d;
        *(int*)(c + 0x5c) = t0;
        *(int*)(c + 0x60) = s[1];
        *(int*)(c + 0x64) = s[2];
        {
            int ang = *(short*)(*(char**)(c + 0x110) + 0x8e);
            *(short*)(c + 0x8e) = ang;
        }
        *fl &= ~0x40000;
        _ZN5Event6SetBitEj(ev);
    }
}
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c13OnTurnIntoEggER6Player, 0x02131f04 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c13OnTurnIntoEggER6Player
/* Vtable slot 19, verified against ov089 relocs.txt: _ZTV10daObjKey_c
 * (0x02132ba8) + 0x4c -> 0x02131f04 (former name func_ov089_02131f04). */
void daObjKey_c::OnTurnIntoEgg(Player &player)
{
    unsigned isMatch = (actorID == 0x11a);
    if (isMatch)
        return func_ov089_02131df4((char *)this, (char *)&player);
    return func_ov089_02131dcc((char *)this, (int)&player);
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c13OnYoshiTryEatEv, 0x02131f4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c13OnYoshiTryEatEv
s32 daObjKey_c::OnYoshiTryEat() {
    return 4;
}

/* -------------------------------------------------------------------------- */
/* func_ov089_02131f54, 0x02131f54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov089_02131f54
extern "C" {
void func_ov089_02131f54(void* c){
  char* r4=(char*)c;
  Matrix4x3_FromRotationY(r4+0x130, *(short*)(r4+0x8e));
  *(int*)(r4+0x154)=*(int*)(r4+0x5c)>>3;
  *(int*)(r4+0x158)=*(int*)(r4+0x60)>>3;
  *(int*)(r4+0x15c)=*(int*)(r4+0x64)>>3;
  if (*(int*)(data_ov089_021328b4 + (*(int*)(r4+0x444)<<2)) != 0 && *(int*)(r4+0x448)==0) {
    Matrix4x3_FromRotationY(r4+0x194, *(short*)(r4+0x8e));
    *(int*)(r4+0x1b8)=*(int*)(r4+0x5c)>>3;
    *(int*)(r4+0x1bc)=(*(int*)(r4+0x60)+0x64000)>>3;
    *(int*)(r4+0x1c0)=*(int*)(r4+0x64)>>3;
  }
  *(Matrix4x3*)(r4+0x1f0)=IDENTITY_MATRIX4X3;
  *(int*)(r4+0x214)=*(int*)(r4+0x5c)>>3;
  *(int*)(r4+0x218)=*(int*)(r4+0x60)>>3;
  *(int*)(r4+0x21c)=*(int*)(r4+0x64)>>3;
  if (*(int*)(r4+0x174) == (&data_ov002_02110964)[1]) {
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(r4, r4+0x1c8, r4+0x1f0, 0x96000, 0x3e8000, 0xf);
  }
}
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c16CleanupResourcesEv, 0x02132084 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c16CleanupResourcesEv
int daObjKey_c::CleanupResources()
{
    UnloadKeyModels(*(int *)((char *)&mState));
    ((SharedFilePtr *)&data_ov002_02110964)->Release();
    if (*(int *)((char *)&mState) != 7) {
        ((SharedFilePtr *)(data_ov089_02132c60))->Release();
        ((SharedFilePtr *)(data_ov089_02132c40))->Release();
        ((SharedFilePtr *)(data_ov089_02132c70))->Release();
        ((SharedFilePtr *)(data_ov089_02132c48))->Release();
    }
    _ZN5Event8ClearBitEj(0x1d);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c6RenderEv, 0x021320f0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c6RenderEv
int daObjKey_c::Render()
{
  int b = (int)((mFlags & 0x40000) != 0);
  if (b) return 1;
  if (unk_448 != 0) {
    ((Sub*)((char*)&mModelAnim))->m(0);
  } else {
    ((Sub*)((char*)&mModelAnim))->m((char*)&mScaleX);
    if (*(int*)((char*)data_ov089_021328b4 + (mState << 2)) != 0 && unk_448 == 0) {
      ((Sub*)((char*)&mModel))->m(0);
    }
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c8BehaviorEv, 0x02132194 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c8BehaviorEv
int daObjKey_c::Behavior()
{
    int vec[3];
    int p7[3];
    int pe[3];
    int v = unk_448;

    if (v != 0) {
        if (v == 3) {
            {
                char* o = *(char**)((char*)&unk_110);
                if (o != 0) {
                    int* s = (int*)(int)LAUNDER(o + 0x5c);
                    mPosX = s[0];
                    mPosY = s[1];
                    mPosZ = s[2];
                    {
                        char* o2 = *(char**)((char*)&unk_110);
                        int ang = *(short*)(o2 + 0x8e);
                        mAngleY = ang;
                    }
                }
            }
            if (mModelAnim.Finished() == 0) {
                Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
                MulMat4x3Mat4x3(mModelAnim.data.transforms, &data_020a0e68, &data_020a0e68);
                {
                    char* m = &data_020a0e68;
                    int t0 = *(int*)(m + 0x24);
                    int t1 = *(int*)(m + 0x28);
                    int t2 = *(int*)(m + 0x2c);
                    vec[2] = t2;
                    vec[0] = t0;
                    vec[1] = t1;
                }
                SubVec3(vec, ((char*)this) + 0x5c, vec);
                Vec3_LslInPlace(vec, 3);
                AddVec3(vec, ((char*)this) + 0x5c, vec);
                vec[1] = *(int*)((char*)mModelAnim.data.bones + 0xc) * 0x23 + vec[1];
                vec[1] = vec[1] - 0x48000;
                *(void**)((char*)&unk_464) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unk_464, 0x82, vec[0], vec[1], vec[2], 0, 0);
                *(void**)((char*)&unk_468) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unk_468, 0x83, vec[0], vec[1], vec[2], 0, 0);
            }
        }

        mModelAnim.Advance();
        func_ov089_02131f54(((char*)this));
        if (mModelAnim.Finished()) {
            int b = (actorID == 0x11a);
            if (b != 0) {
                if (mModelAnim.file != (BCA_File *)data_ov089_02132c40[1])
                    _ZN7fBase_c18MarkForDestructionEv(((char*)this));
            }
        }
        return 1;
    }

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((char*)this), ((char*)this) + 0x260)) {
        func_ov089_02131f54(((char*)this));
        _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
        return 1;
    }
    mEatingPlayer = 0;
    if (mSpinSpeed > 0x400) {
        short* q = (short*)(int)LAUNDER((char*)&mSpinSpeed);
        *q = *q - 0x100;
    } else if (mSpinSpeed == 0) {
        mSpinSpeed = 0x400;
    }
    {
        short* ang = (short*)(int)LAUNDER((char*)&mAngleY);
        *ang = *ang + mSpinSpeed;
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), 0);
    (((C*)((char*)this))->*PMFTABLE[mState].pmf)();
    func_ov089_02131f54(((char*)this));
    _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
    if (mState == 7) {
        p7[0] = data_ov089_02132b40[0];
        p7[1] = data_ov089_02132b40[1];
        p7[2] = data_ov089_02132b40[2];
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x220, p7);
    } else {
        pe[0] = data_ov089_02132ca4[0];
        pe[1] = data_ov089_02132ca4[1];
        pe[2] = data_ov089_02132ca4[2];
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x220, pe);
    }
    _ZN5dCc_c6UpdateEv((char*)&mdCcAcPos_c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c13InitResourcesEv, 0x021324a4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c13InitResourcesEv
int daObjKey_c::InitResources()
{
    int v[3];
    int v2[3];
    int kind = param1 & 7;
    mState = kind;
    LoadKeyModels(mState);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_02110964);

    if (mState == 7) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x114, *(void**)(&data_ov002_0211094c + 4), 1, 1) == 0)
            return 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this) + 0x114, *(void**)((char *)&data_ov002_02110964 + 4), 0x40000000, 0x1000, 0);
        mScaleX = 0x2000;
        mScaleY = 0x2000;
        mScaleZ = 0x2000;
        v[0] = data_ov089_02132b40[0];
        v[1] = data_ov089_02132b40[1];
        v[2] = data_ov089_02132b40[2];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char*)this) + 0x220, ((char*)this), v, 0x50000, 0xfa000, 0x800003, 0x8000);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x260, ((char*)this), 0xa0000, 0, 0, 0);
        mSpinSpeed = 0x400;
        mVertAccel = 0;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x57, ((char*)this) + 0x74);
    } else {
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c60);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c40);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c70);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c48);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x114, *(void**)((char*)data_ov089_02132894[mState] + 4), 1, 1) == 0)
            return 0;
        {
            void* m = (void*)*(int *)(data_ov089_021328b4 + (mState << 2));
            if (m != 0) {
                if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x178, *(void**)((char*)m + 4), 1, -1) == 0)
                    return 0;
            }
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this) + 0x114, *(void**)((char *)&data_ov002_02110964 + 4), 0x40000000, 0x1000, 0);
        mScaleX = 0x3000;
        mScaleY = 0x3000;
        mScaleZ = 0x3000;
        mVertSpeed = 0x23000;
        v2[0] = data_ov089_02132ca4[0];
        v2[1] = data_ov089_02132ca4[1];
        v2[2] = data_ov089_02132ca4[2];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char*)this) + 0x220, ((char*)this), v2, 0x50000, 0x64000, 0x800003, 0x8000);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x260, ((char*)this), 0x48000, 0, 0, 0);
        mVertAccel = -0x2000;
        mSpinSpeed = 0;
    }

    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN10dBgCh_Actr13SetLimMovFlagEv((char*)&mWithMeshClsn);
    unk_41c = mPosX;
    unk_420 = mPosY;
    unk_424 = mPosZ;
    mStateTimer = 0;
    unk_442 = 0;
    unk_443 = 0;
    mTerminalVelocity = -0x32000;
    unk_448 = 0;
    unk_110 = 0;
    unk_46c = 0;
    unk_468 = unk_46c;
    unk_464 = unk_468;
    if (data_0209cef0 == 0)
        _ZN5Event8ClearBitEj(0x1d);
    return 1;
}
