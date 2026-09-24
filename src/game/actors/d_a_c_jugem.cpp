//cpp
/* Lakitu in the level. He turns to face Mario, takes his control, and
 * talks through message 0x182 until Mario has finished listening.
 * Class name is ROM RTTI: "11daC_Jugem_c" at ov085 0x02130310,
 * __si_class_type_info 0x02130304, vtable 0x02130344.
 * daC_Jugem_c_classInit (0x0212ed54) belongs in this TU; tu_map misses it
 * because that symbol is neither func_ov085_* nor _ZN11daC_Jugem_c*.
 */
#include "daC_Jugem_c.h"
#include "common.h"
#include "dActor_c.h"
#include "Player.h"
#include "Camera.h"
#include "SharedFilePtr.h"

/* Every external call is declared once, with C linkage, under its ROM name.
   Pointer parameters are spelled void *: the mangled names already carry the
   real types. By-value Fix12<int> parameters and return types are kept exact,
   because mwccarm passes the first differently and callers test the second. */
extern "C" {

/* camera */
void  _ZN6Camera9SetFlag_3Ev(void *cam);
void  _ZN6Camera9SetLookAtERK7Vector3(void *cam, const void *v);
void  _ZN6Camera6SetPosERK7Vector3(void *cam, const void *v);

/* math / vector helpers */
void  _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *cur, const void *tgt, int step);
void  _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step);
void  _Z14ApproachLinearRiii(int *cur, int tgt, int step);
int   Vec3_Dist(const void *a, const void *b);
int   Vec3_HorzDist(const void *a, const void *b);
s16   Vec3_HorzAngle(const void *a, const void *b);
s16   Vec3_VertAngle(const void *a, const void *b);
void  Vec3_Sub(void *out, const void *a, const void *b);
void  Vec3_Asr(void *dst, const void *src, int sh);
int   Vec3_ApproachHorz(void *cur, const void *tgt, int maxStep);
int   LenVec3(const void *v);
int   AngleDiff(int a, int b);
u16   DecIfAbove0_Short(u16 *p);
void  Matrix4x3_FromRotationY(void *m, int angY);
void  Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
void  MulVec3Mat4x3(const void *v, const void *m, void *out);

/* dActor_c / dEnemyBase_c / fBase_c, reached by their ROM names.
   The mangled spellings are kept wherever the real declaration takes a
   Fix12<int> or an s8/s16 BY VALUE, because mwccarm passes those differently
   at the call site than the loose spelling would. */
void *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
s16   _ZN8dActor_c18HorzAngleToCPlayerEv(void *thiz);
void  _ZN8dActor_c13SpawnSoundObjEj(void *thiz, u32 id);
void  _ZN8dActor_c9UpdatePosEP5dCc_c(void *thiz, void *cc);
int   _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
          void *thiz, void *sm, void *mtx, int rad, int height, u32 flags);
void  _ZN7fBase_c18MarkForDestructionEv(void *thiz);
void *_ZN7fBase_cnwEj(u32 size);
void  _ZN12dEnemyBase_cC2Ev(void *thiz);

/* Player / Message */
int   _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
          void *thiz, void *actor, u32 id, const void *pos, u32 a, u32 b);
int   _ZN6Player12GetTalkStateEv(void *thiz);
int   _ZN6Player18HasFinishedTalkingEv(void *thiz);
int   _ZN6Player17SetNoControlStateEhih(int thiz, u8 a, int b, u8 c);

/* Sound. The return type is load-bearing: two members test PlaySub's result
   in an `if`, so the `void` spelling one legacy file used cannot be the one.
   The fourth parameter stays Fix12i (wall 6az). */
int   _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, Fix12i d, bool loop);
u32   _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 a, u32 b, const void *pos, u32 d);

/* ModelAnim / TextureSequence -- these two stay mangled for the same
   Fix12<int>-by-value reason. */
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *f, int a, Fix12i b, u32 c);
void  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *thiz, BTP_File &f, int a, Fix12i b, u32 c);
void  _ZN9ModelAnimC1Ev(void *thiz);
void  _ZN15TextureSequenceC1Ev(void *thiz);
void  _ZN11ShadowModelC1Ev(void *thiz);
int   _ZN15TextureSequence6UpdateER15ModelComponents(void *thiz, void *mc);
void  _ZN9Animation7AdvanceEv(void *thiz);

/* other overlays / arm9 */
void  func_ov002_020c3e8c(void);
int   func_ov002_020c3ea0(void *p);
void  func_ov002_020c3f18(void *p);
void  func_ov002_020c3f2c(void *p);
void  func_ov002_020d228c(void *p);
void  func_ov002_020e4374(void *p, int *a, int *b);
void  func_0201f32c(int a);
void  func_02012790(int a);

/* this overlay's own data */
extern void          *data_0209f318;
extern int            data_0209caa0[];
extern u8             data_0209d66c;
extern u8             data_0209d6bc;
extern u8             data_0209f284;
extern s8             data_0209f2f8;
extern short          data_02082214[];
extern Matrix4x3      data_020a0e68;
extern int            data_ov085_02130840[];
extern Vector3        data_ov085_0213084c;
extern char           data_ov085_0213074c[]; /* SharedFilePtr; loaded file at +4 */
extern char           data_ov085_02130744[];
extern char           data_ov085_0213073c[];

}

/* The state objects this class drives itself with. Each is a pair of
   pointers-to-member: the setter at 0x0212e728 calls the one at offset 0,
   Behavior calls the one at offset 8. The two views stay separate (JugemHost
   here, BehState in Behavior) rather than merged into one invented struct. */
struct JugemHost;
typedef int (JugemHost::*JugemInitPMF)();
struct JugemHost { char pad[0x1ec]; JugemInitPMF *pp; };

extern "C" {
int func_ov085_0212e728(JugemHost *c, JugemInitPMF *p);
extern JugemInitPMF data_ov085_021307a0;
extern JugemInitPMF data_ov085_021307b0;
extern JugemInitPMF data_ov085_021307c0;
extern JugemInitPMF data_ov085_021307d0;
extern JugemInitPMF data_ov085_021307e0;
extern JugemInitPMF data_ov085_021307f0;
extern JugemInitPMF data_ov085_02130790;
extern JugemInitPMF data_ov085_02130800;
extern JugemInitPMF data_ov085_02130810;
extern JugemInitPMF data_ov085_02130820;
extern JugemInitPMF data_ov085_02130830;
}

/* Source order is ROM-ascending; do not reorder. Out-of-line ~daC_Jugem_c:
   D1 then D0 (deferred codegen would emit D2, D0, D1). */
#pragma defer_codegen off

// @symbol _ZN11daC_Jugem_cD1Ev
// @symbol _ZN11daC_Jugem_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * ONE definition, TWO ROM functions. mwccarm emits the deleting variant
 * D0 and the complete variant D1 from this single declaration, so D0 has
 * no source of its own; with deferred code generation off they land in
 * the cartridge's order, D1 first at 0x0212d528 and D0 at 0x0212d578.
 *
 * The body is empty because there is nothing to write: one vtable store
 * and six destructor calls, every one a consequence of
 * `struct daC_Jugem_c : dEnemyBase_c` and the members that declaration
 * types -- its own vptr, then ShadowModel (0x218), ShadowModel (0x1f0),
 * TextureSequence (0x1d8), ModelAnim (0x174), ModelAnim (0x110) in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c. D0's
 * deallocation is an inline operator delete reached through
 * dEnemyBase_c, this class's immediate base, which is why nothing here
 * mentions a heap.
 */
daC_Jugem_c::~daC_Jugem_c()
{
}

// @symbol func_ov085_0212d5dc
/* recovered: shared common types */
struct Range { int a, b, c, d, e, f; };
extern "C" int func_ov085_0212d5dc(daC_Jugem_c *c) {
  Range r;
  void* cam = *(void**)&data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  r.a = -0x4b0000;
  r.b = 0x19f000;
  r.c = 0x1a90000;
  r.d = -0x4b0000;
  r.e = 0x250000;
  r.f = 0x1d4c000;
  _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&c->mCamLookX, &r, 0x70000);
  _ZN6Camera9SetLookAtERK7Vector3(cam, &c->mCamLookX);
  _ZN6Camera6SetPosERK7Vector3(cam, &c->mCamPosX);
  Vec3_Dist(&c->mCamLookX, &r);
  c->mTimer++;
  if (c->mTimer > 0x64) {
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4b, 0x7f, 0, 0x7222, false) != 0) {
      *(int *)&((Camera *)cam)->mFlags &= ~8;
      c->mHorzSpeed = 0;
      c->mTimer = 0;
      c->unk_2cc = 0;
      c->unk_0a4 = 0;
      c->mVertSpeed = 0;
      c->unk_0ac = 0;
      c->mAngleX = 0;
      if (_ZN8dActor_c13ClosestPlayerEv(c) != 0) {
        func_ov002_020c3e8c();
        data_0209caa0[2] |= 0x80;
      }
      func_ov085_0212e728((JugemHost *)c, &data_ov085_021307e0);
    }
  }
  return 1;
}

// @symbol func_ov085_0212d724
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d724(daC_Jugem_c *c){
  c->unk_2dc = 1;
  c->mTimer = 0;
  return 1;
}
}

// @symbol func_ov085_0212d73c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d73c(daC_Jugem_c *c)
{
    Vector3 v[3];
    void *cam;
    int spd;
    int len;

    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);
    c->mSfxHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(c->mSfxHandle, 3, 0x182, &c->mCamSpacePosX, 0);
    _Z14ApproachLinearRsss(&c->mAngleY, Vec3_HorzAngle(&c->mPosX, &data_ov085_0213084c), 0x200);
    _Z14ApproachLinearRsss(&c->mAngleX, Vec3_VertAngle(&c->mPosX, &data_ov085_0213084c), 0x200);
    _Z14ApproachLinearRiii(&c->mHorzSpeed, 0x28000, 0x2000);
    _Z14ApproachLinearRsss(&c->mPrevAngleY, Vec3_HorzAngle(&c->mPosX, &data_ov085_0213084c), (s16)c->unk_2cc);
    c->mAngleY = c->mPrevAngleY;
    *(int *)(((int)&c->unk_2cc)) += 5;
    if (c->unk_2cc > 0x800)
        c->unk_2cc = 0x800;
    *(int *)(((int)&c->mTimer)) += 1;
    if (c->mTimer < 0x19)
        return 1;
    v[1].x = -0x50c000;
    v[1].y = 0x115000;
    v[1].z = 0x1d15000;
    spd = c->mHorzSpeed >> 1;
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&c->mCamLookX, &c->mPosX, spd);
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&c->mCamPosX, &v[1], spd);
    _ZN6Camera9SetLookAtERK7Vector3(cam, &c->mCamLookX);
    _ZN6Camera6SetPosERK7Vector3(cam, &c->mCamPosX);
    Vec3_Sub(&v[2], &c->mPosX, &data_ov085_0213084c);
    len = LenVec3(&v[2]);
    if (len == 0 || len < 0x7d0000)
        func_ov085_0212e728((JugemHost *)c, &data_ov085_021307a0);
    return 1;
}
}

// @symbol func_ov085_0212d8ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d8ec(daC_Jugem_c *c) {
  volatile Vector3 look, pos; (void)&look; (void)&pos;
  void* cam;
  c->mHorzSpeed = 0;
  c->mTimer = 0;
  c->unk_2cc = 0;
  c->unk_2e0 = 0;
  cam = data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  c->mCamLookX = 0xffc67000;
  c->mCamLookY = 0x6ea000;
  c->mCamLookZ = 0x1212000;
  c->mCamPosX = 0xffb65000;
  c->mCamPosY = 0x1d5000;
  c->mCamPosZ = 0x17fc000;
  _ZN6Camera9SetLookAtERK7Vector3(cam, &c->mCamLookX);
  _ZN6Camera6SetPosERK7Vector3(cam, &c->mCamPosX);
  c->unk_2cc = 0xa0;
  c->mPosX = data_ov085_02130840[0];
  c->mPosY = data_ov085_02130840[1];
  c->mPosZ = data_ov085_02130840[2];
  return 1;
}
}

// @symbol func_ov085_0212d9b8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212d9b8(daC_Jugem_c *c)
{
    void* pl = _ZN8dActor_c13ClosestPlayerEv(c);
    if (pl == 0) return 1;

    *(int *)(((int)&c->mTimer)) += 1;
    *(int *)(((int)&c->unk_2cc)) += 0x500;

    {
        int v = (short)c->unk_2cc;
        int idx = (int)((unsigned int)(v << 16) >> 16) >> 4;
        short e = data_02082214[idx * 2];
        _Z14ApproachLinearRiii(&c->mPosY,
            (int)(((long long)e * 0x1a000 + 0x800) >> 12) + c->mTargetY,
            0x10000000);
    }

    if (c->mTimer == 0x32) {
        func_ov002_020c3ea0(pl);
    }
    c->mAngleY = 0x6000;
    c->mPrevAngleY = c->mAngleY;
    {
        int a1 = 0x4000;
        if (c->mTimer >= 0x4b) a1 = 0x8000;
        _Z14ApproachLinearRiii(&c->mHorzSpeed, a1, 0x1000);
    }

    c->mSfxHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        c->mSfxHandle, 3, 0x182, &c->mCamSpacePosX, 0);

    if (c->mTimer > 0x78) {
        c->mAngleY = 0x4000;
        c->mPrevAngleY = c->mAngleY;
        func_ov085_0212e728((JugemHost *)c, &data_ov085_02130820);
    }
    return 1;
}
}

// @symbol func_ov085_0212db04
// @symbol func_ov085_0212db04
/* recovered: shared common types */

extern "C" {
int func_ov085_0212db04(daC_Jugem_c *c) {
  Vector3 look, pos;
  void* cam;
  c->mTimer = 0;
  c->unk_2cc = 0;
  c->mPosX = -0x5a0000;
  c->mPosY = 0x2c0000;
  c->mPosZ = 0x1c6f000;
  c->mTargetX = c->mPosX;
  c->mTargetY = c->mPosY;
  c->mTargetZ = c->mPosZ;
  cam = data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  look.x = -0x304000;
  look.y = 0x3c1000;
  look.z = 0x1c77000;
  pos.x = -0x540000;
  pos.y = 0xe1000;
  pos.z = 0x19e4000;
  _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
  _ZN6Camera6SetPosERK7Vector3(cam, &pos);
  c->mHorzSpeed = 0;
  return 1;
}
}

// @symbol func_ov085_0212dbdc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212dbdc(daC_Jugem_c *c)
{
    Player *p = (Player *)_ZN8dActor_c13ClosestPlayerEv(c);
    int* pp;
    if (p == 0)
        return 1;
    pp = &c->mTimer;
    *pp = *pp + 1;
    switch (c->mTimer) {
    case 1:
        func_0201f32c(2);
        break;
    case 0x14:
        func_ov002_020c3f2c(p);
        break;
    case 0x28:
        data_0209d66c = 1;
        break;
    case 0x8c:
        func_0201f32c(3);
        break;
    case 0xee:
        data_0209d66c = 1;
        break;
    case 0xf0:
        {
            s16 a = c->mSavedAngleY;
            s16 b;
            p->mAngleX = 0;
            p->mAngleY = a;
            p->mAngleZ = 0;
            b = c->mSavedAngleY;
            p->mPrevAngleX = 0;
            p->mPrevAngleY = b;
            p->mPrevAngleZ = 0;
            func_ov085_0212e728((JugemHost *)c, &data_ov085_021307f0);
        }
        break;
    }
    return 1;
}
}

// @symbol func_ov085_0212dcfc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212dcfc(daC_Jugem_c *p)
{
    p->mTalkPlayer = 0;
    p->mTimer = 0;
    return 1;
}
}

// @symbol func_ov085_0212dd10
extern "C" {

int func_ov085_0212dd10(daC_Jugem_c *c)
{
    Player *p = (Player *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (!p) return 1;
    {
        short v = c->mAngleY;
        short w = v + 0x8000;
        int* t = &c->mTimer;
        p->mAngleX = 0;
        p->mAngleY = w;
        p->mAngleZ = 0;
        *t += 1;
    }
    if (c->mTimer == 0x5a) func_ov002_020c3f18(p);
    {
        int s = c->mTimer;
        if (s > 0x57 && s < 0x5b)
            _Z14ApproachLinearRsss(&c->mAngleX, 0x2000, 0x400);
        else
            _Z14ApproachLinearRsss(&c->mAngleX, 0x1000, 0x400);
    }
    if (c->mTimer > 0x78)
        func_ov085_0212e728((JugemHost *)c, &data_ov085_021307c0);
    return 1;
}
}

// @symbol func_ov085_0212ddc4
// @symbol func_ov085_0212ddc4
/* recovered: shared common types */

extern "C" {
int func_ov085_0212ddc4(daC_Jugem_c *c) {
  Vector3 look, pos;
  void* cam;
  c->mTimer = 0;
  c->mTalkPlayer = 0;
  c->mAngleZ = 0;
  cam = data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  look.x = 0xffadd000;
  look.y = 0x17e000;
  look.z = 0x1a29000;
  pos.x = 0xffa54000;
  pos.y = 0x1f4000;
  pos.z = 0x1ccf000;
  _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
  _ZN6Camera6SetPosERK7Vector3(cam, &pos);
  return 1;
}
}

// @symbol func_ov085_0212de5c
extern "C" int func_ov085_0212de5c(daC_Jugem_c *c)
{
    Player *p = ((dActor_c *)c)->ClosestPlayer();
    if (p == 0)
        return 1;
    {
        short t = c->mAngleY + 0x8000;
        p->mAngleX = 0;
        p->mAngleY = t;
        p->mAngleZ = 0;
    }
    c->mAngleY = ((dActor_c *)c)->HorzAngleToCPlayer();
    c->unk_2dc = 0;
    c->mAngleX = 0x1000;
    c->mAngleZ = 0x800;
    {
        int *pp = (int *)(((int)&c->mTimer));
        int n = *pp + 1;
        *pp = n;
    }
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4b, 0x14, 0x7f, 0x15666, false);
    switch (c->mTimer) {
    case 1:
        p->unk_723 = 1;
        break;
    case 0x78:
        func_0201f32c(1);
        break;
    case 0x82:
        p->unk_723 = 0;
        break;
    case 0xb4:
        data_0209d66c = 1;
        break;
    case 0xd2:
        func_ov085_0212e728((JugemHost *)c, &data_ov085_021307b0);
        break;
    }
    return 1;
}

// @symbol func_ov085_0212df84
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" int func_ov085_0212df84(daC_Jugem_c *c)
{
    Vector3 look, pos;
    void* cam;
    Player* player;
    c->mTargetX = -0x5a0000;
    c->mTargetY = 0x1c0000;
    c->mTargetZ = 0x1a66000;
    player = (Player *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (player) {
        c->mSavedAngleY = player->mAngleY;
        func_ov002_020d228c(player);
    }
    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);
    look.x = 0xff883000;
    look.y = 0x2ef000;
    look.z = 0x1a36000;
    pos.x = 0xffb18000;
    pos.y = 0x18c000;
    pos.z = 0x1a89000;
    _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
    _ZN6Camera6SetPosERK7Vector3(cam, &pos);
    c->mStateTimer = 0x79;
    c->mPosX = c->mTargetX;
    c->mPosY = c->mTargetY;
    c->mPosZ = c->mTargetZ;
    c->mTalkPlayer = 0;
    return 1;
}

// @symbol func_ov085_0212e078
extern "C" {

int func_ov085_0212e078(daC_Jugem_c *c)
{
    _Z14ApproachLinearRsss(&c->mPrevAngleY, Vec3_HorzAngle(&c->mPosX, &c->mTargetX), 0x800);
    _Z14ApproachLinearRsss(&c->mAngleY, c->mPrevAngleY, 0x800);
    c->mSfxHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        c->mSfxHandle, 3, 0x182, &c->mCamSpacePosX, 0);
    if (AngleDiff(c->mPrevAngleY, Vec3_HorzAngle(&c->mPosX, &c->mTargetX)) < 0x2000) {
        Vec3_ApproachHorz(&c->mPosX, &c->mTargetX, 0x1e000);
        _Z14ApproachLinearRiii(&c->mPosY, c->mTargetY, 0x1e000);
    }
    if (*(unsigned short *)&c->mStateTimer == 0) {
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4a, 0x7f, 0, 0x7222, 0) != 0) {
            if (c->mTalkPlayer->HasFinishedTalking() == 1) {
                _ZN7fBase_c18MarkForDestructionEv(c);
                data_0209f284 = 0;
            }
        }
    }
    return 1;
}
}

// @symbol func_ov085_0212e180
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e180(daC_Jugem_c *c) {
    short r3 = c->mAngleY;
    c->mPrevAngleY = r3;
    c->mStateTimer = 0x46;
    return 1;
}
}

// @symbol func_ov085_0212e19c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e19c(daC_Jugem_c *c)
{
    _Z14ApproachLinearRsss(&c->mAngleY, ((dActor_c *)c)->HorzAngleToCPlayer(), 0x800);
    switch (c->mTalkStep) {
    case 0:
        {
            c->mTalkPlayer->mStateFlags |= 0x400;
        }
        if (c->mTalkPlayer->ShowMessage(*(fBase_c *)c, 0x182, (Vector3 *)&c->mPosX, 1, 0) == 1) {
            *(int *)(((int)&c->mTalkStep)) += 1;
        }
        break;
    case 1:
        if (data_0209d6bc == 7) { *(int *)(((int)&c->mTalkStep)) += 1; }
        break;
    case 2:
        if (data_0209d6bc == 7)
            break;
        func_02012790(0x24);
        data_0209f284 = 1;
        { *(int *)(((int)&c->mTalkStep)) += 1; }
        break;
    case 3:
        if (data_0209d6bc == 9) { *(int *)(((int)&c->mTalkStep)) += 1; }
        break;
    case 4:
        if (c->mTalkPlayer->GetTalkState() == 2)
            func_ov085_0212e728((JugemHost *)c, &data_ov085_02130830);
        break;
    }
    return 1;
}
}

// @symbol func_ov085_0212e2ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e2ec(daC_Jugem_c *r0) {
    int *p = data_0209caa0;
    p[2] |= 0x20000;
    r0->mTalkStep = 0;
    return 1;
}
}

// @symbol func_ov085_0212e310
// @symbol func_ov085_0212e310
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {




int func_ov085_0212e310(daC_Jugem_c *c)
{
    Vector3 in;
    Vector3 out;
    Player *p;

    p = (Player *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (p == 0) {
        return 1;
    }

    p->unk_744 = c->mPosX;
    p->unk_748 = c->mPosY;
    p->unk_74c = c->mPosZ;

    if (*(unsigned short *)&c->mStateTimer == 0) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4a, 0x14, 0x7f, 0x15666, 0);
    }

    c->mSfxHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(c->mSfxHandle, 3, 0x182, (Vector3 *)&c->mCamSpacePosX, 0);

    in.x = 0;
    in.y = 0;
    in.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.z = 0xc8000;
    Matrix4x3_FromRotationY(&data_020a0e68, 0);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    out.x += p->mPosX;
    out.y += p->mPosY + 0x70000;
    out.z += p->mPosZ;

    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&c->mPosX, &out, 0x14000);
    _Z14ApproachLinearRsss(&c->mPrevAngleY, Vec3_HorzAngle(&c->mPosX, &out), 0x800);
    c->mAngleY = c->mPrevAngleY;

    if (Vec3_Dist(&c->mPosX, &out) < 0x14000) {
        c->mTalkPlayer = p;
        func_ov085_0212e728((JugemHost *)c, &data_ov085_02130810);
    }

    return 1;
}
}

// @symbol func_ov085_0212e480
/* A daC_Jugem_c state function: it writes this class's mTimer and mStateTimer. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e480(daC_Jugem_c *p)
{
    p->mPosZ = 2457600;
    p->mTimer = 0;
    p->mStateTimer = 40;
    return 1;
}
}

// @symbol func_ov085_0212e4a4
typedef struct V3 { int x, y, z; } V3;
/* This member's legacy file carried `#pragma opt_common_subs off`. That
   pragma is file-global last-wins in mwccarm, so it is bracketed here
   rather than left open -- with deferred code generation off the bracket
   binds, and every other member is compiled without it. */
#pragma opt_common_subs off
extern "C" {  /* .c-derived member: C linkage for the whole block */


int func_ov085_0212e4a4(daC_Jugem_c *self)
{
    Player *p = (Player *)_ZN8dActor_c13ClosestPlayerEv(self);
    if (p != 0) {
        V3 v = *(V3 *)&p->mPosX;
        if ((data_0209caa0[2] & 0x10000) != 0 &&
            v.z > -0x28000 &&
            p->SetNoControlState(0x12, -1, 0) != 0) {
            p->unk_744 = self->mPosX;
            p->unk_748 = self->mPosY;
            p->unk_74c = self->mPosZ;
            self->mPosX = v.x;
            self->mPosY = v.y;
            self->mPosZ = v.z;
            *(int *)(((long long)(int)&self->mPosX)) -= 0x3e8000;
            self->mPosY = p->mGroundY + 0x3e8000;
            self->mTargetX = self->mPosX;
            self->mTargetY = self->mPosY;
            self->mTargetZ = self->mPosZ;
            *(signed char *)&self->unk_2dc = 0;
            func_ov085_0212e728((JugemHost *)self, &data_ov085_02130800);
        }
    }
    return 1;
}
}
#pragma opt_common_subs on

// @symbol func_ov085_0212e59c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e59c(daC_Jugem_c *p)
{
    p->unk_2dc = 1; return 1;
}
}

// @symbol func_ov085_0212e5ac
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e5ac(daC_Jugem_c *self)
{
    Vector3 in, out, plpos;
    Camera *cam;
    Vector3* src;
    Player *p;

    cam = (Camera *)data_0209f318;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    src = (Vector3 *)(((int)&cam->pos));
    plpos.x = src->x;
    plpos.y = src->y;
    plpos.z = src->z;
    if (data_0209f2f8 == 0x2f) {
        self->mPosX = 0;
    } else {
        self->mPosX = 0x1086000;
        if (self->unk_2cc == 0) {
            p = (Player *)_ZN8dActor_c13ClosestPlayerEv(self);
            if (p != 0 && p->mPosX > 0x1086000) {
                _ZN8dActor_c13SpawnSoundObjEj(self, 0);
                self->unk_2cc = 1;
            }
        }
    }
    self->mPosY = plpos.y;
    self->mPosZ = plpos.z;
    in.z = Vec3_HorzDist(&plpos, &self->mPosX);
    Matrix4x3_FromRotationY(&data_020a0e68, Vec3_HorzAngle(&plpos, &self->mPosX));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    *(int *)(((int)&self->mPosX)) += out.x;
    if (data_0209f2f8 == 0x2f) {
        if (self->mPosX > 0x79e000) {
            self->mPosX = 0x79e000;
        }
    }
    *(int *)(((int)&self->mPosZ)) += out.z;
    self->mPrevAngleY = 0x8000 - cam->mAngleY;
    /* The halfword after Camera::mAngleY at 0x17e (Camera.h has no member for
       it). Possibly the pitch, but only this one negated use suggests so. */
    self->mPrevAngleX = -*(short *)((char *)&cam->mAngleY + 2);
    return 1;
}
}

// @symbol func_ov085_0212e720
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_0212e720(void)
{
    return 1;
}
}

// @symbol func_ov085_0212e728
extern "C" int func_ov085_0212e728(JugemHost *c, JugemInitPMF *p)
{ c->pp = p; JugemInitPMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

// @symbol func_ov085_0212e778
extern "C" {
struct Vec3 { int x, y, z; };

typedef struct { int w[12]; } M48;

void func_ov085_0212e778(daC_Jugem_c *c)
{
    Vec3 v;
    Vec3_Asr(&v, &c->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
    *(M48 *)&c->mModelAnim1.mat4x3 = *(M48 *)&data_020a0e68;
    Matrix4x3_FromTranslation(&data_020a0e68, c->mPosX >> 3, (c->mPosY - 0x38000) >> 3, c->mPosZ >> 3);
    *(M48 *)&c->mShadowMat1 = *(M48 *)&data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, &c->mShadowModel1, &c->mShadowMat1, 0x46000, 0x258000, 0xf);
}
}

// @symbol func_ov085_0212e858
/* recovered: shared common types */
/* The matrix type below has to be local, and it is why this member was the
   last one to come good. Three of its statements copy a whole Matrix4x3,
   and the ROM does each as one flat 12-word block copy. common.h spells
   Matrix4x3 flat, `{ s32 m[12] }`, but math/Matrix.h spells the same type
   `{ Matrix3x3 r; Vector3 t; }` and this TU reaches THAT one first, through
   ModelAnim.h. The nested spelling is non-POD, so the copies scalarize
   member-wise: the function grows 0x238 -> 0x288, takes an extra
   callee-saved register and a 4-byte-larger frame. The sibling member at
   0x0212e778 was already written against a flat local type for this reason;
   this is the same fix. Canonicalising either header is a measured codegen
   hazard, not a tidy-up, so neither is touched.

   The vector locals, by contrast, were MEASURED not to need one. types.h's
   Vector3 declares an empty destructor under C++ -- deliberately, so that
   the arrays the ROM destroys elsewhere have something to destroy -- and a
   flat local copy of it was tried here first, on the theory that the empty
   destructor would pin each local to a stack home. It moved the size by
   nothing; the whole 0x50 was the matrix. The shared spelling stands. */
struct M48e858 { int w[12]; };
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov085_0212e858(daC_Jugem_c *c)
{
    struct Vector3 p;
    struct Vector3 z1;
    struct Vector3 off;
    struct Vector3 pp;
    struct Vector3 t;
    Player *pl;
    int p1, p2;
    signed char lvl;
    struct Vector3 *ps;

    Vec3_Asr(&t, &c->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
    *(M48e858 *)&c->mModelAnim1.mat4x3 = *(M48e858 *)&data_020a0e68;

    Matrix4x3_FromTranslation(&data_020a0e68,
        c->mPosX >> 3,
        (c->mPosY - 0x38000) >> 3,
        c->mPosZ >> 3);
    *(M48e858 *)&c->mShadowMat1 = *(M48e858 *)&data_020a0e68;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, &c->mShadowModel1, &c->mShadowMat1, 0x46000, 0x258000, 0xf);

    pl = (Player *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (pl == 0) return;

    /* early load of level, zero stacks, then player pos copy interleaved with cmp */
    lvl = data_0209f2f8;
    p.x = 0;
    p.y = 0;
    p.z = 0;
    z1.x = 0;
    z1.y = 0;
    z1.z = 0;
    off.x = 0;
    off.y = 0;
    off.z = 0;
    ps = (struct Vector3 *)&pl->mPosX;
    pp.x = ps->x;
    pp.y = ps->y;
    pp.z = ps->z;
    if (lvl == 0x2f) {
        p.x = 0;
    } else {
        p.x = 0x1086000;
    }
    p.y = pp.y;
    p.z = pp.z;

    z1.z = Vec3_HorzDist(&pp, &p);
    {
        short ang = Vec3_HorzAngle(&pp, &p);
        Matrix4x3_FromRotationY(&data_020a0e68, ang);
    }
    MulVec3Mat4x3(&z1, &data_020a0e68, &off);

    p.x = p.x + off.x;
    p.y = pp.y;
    p.z = p.z + off.z;
    Matrix4x3_FromTranslation(&data_020a0e68, p.x >> 3, p.y >> 3, p.z >> 3);

    func_ov002_020e4374(pl, &p1, &p2);

    *(M48e858 *)&c->mShadowMat2 = *(M48e858 *)&data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, &c->mShadowModel2, &c->mShadowMat2, p2, p1, 0xf);
}
}

// @symbol _ZN11daC_Jugem_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Three releases, all this overlay's own, in the reverse of no particular
 * order -- it is simply the ROM's, and is reproduced verbatim.
 */
int daC_Jugem_c::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_0213074c))->Release();
    ((SharedFilePtr *)(&data_ov085_02130744))->Release();
    ((SharedFilePtr *)(&data_ov085_0213073c))->Release();
    return 1;
}

// @symbol _ZN11daC_Jugem_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * Empty -- the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy, not to do anything itself.
 */
void daC_Jugem_c::OnPendingDestroy()
{
}

// @symbol _ZN11daC_Jugem_c6RenderEv
/* recovered: named members + shared header, real C++ method */
/* The model-component slot Render drives is reached through its own vtable;
   this is that vtable's shape as the legacy file recovered it. */
struct Sub { virtual int g0(); virtual int g1(); virtual int g2();
             virtual int g3(); virtual int g4(); virtual int g5(void*); };
int daC_Jugem_c::Render()
{
  if (unk_2dc == 1) return 1;
  _ZN15TextureSequence6UpdateER15ModelComponents(&mTextureSequence, &mModelAnim1.data);
  ((Sub*)((char*)&mModelAnim1))->g5(0);
  return 1;
}

// @symbol _ZN11daC_Jugem_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* Behavior calls the state object's SECOND pointer-to-member, the one at
   offset 8; the setter at 0x0212e728 calls the one at offset 0. */
struct BehC { char pad[0x10000]; };
typedef void (BehC::*BehPMF)();
struct BehState { char pad[8]; BehPMF fn; };
int daC_Jugem_c::Behavior()
{
  DecIfAbove0_Short((unsigned short *)&mStateTimer);
  BehState* st=(BehState *)mState;
  if(st->fn) (((BehC*)this)->*st->fn)();
  _ZN8dActor_c9UpdatePosEP5dCc_c(this, 0);
  _ZN9Animation7AdvanceEv(static_cast<Animation *>(&mModelAnim1));
  _ZN9Animation7AdvanceEv(&mTextureSequence);
  if((BehState *)mState==(BehState*)&data_ov085_021307d0){
    mAngleX=mPrevAngleX;
    mAngleY=mPrevAngleY;
    mAngleZ=mPrevAngleZ;
  }
  if((BehState *)mState==(BehState*)&data_ov085_021307e0) return 1;
  if(unk_2d0==0) func_ov085_0212e858(this);
  else func_ov085_0212e778(this);
  return 1;
}

// @symbol _ZN11daC_Jugem_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* SetAnim and SetFile stay mangled because their real signatures carry
   Fix12<int> (wall 6az). Everything else the members' own headers declare.

   Prepare is one of those now. It reads as a member call because it is a
   STATIC member -- the ROM passes it two argument registers and no this,
   which is why the mangled extern used to be the only spelling that fit.
   TextureSequence.h carries the disassembly that settles it. */
int daC_Jugem_c::InitResources()
{
  BMD_File* bmd = (BMD_File*)Model::LoadFile(*(SharedFilePtr*)data_ov085_0213074c);
  mModelAnim1.SetFile(bmd, 1, -1);
  Animation::LoadFile(*(SharedFilePtr*)data_ov085_02130744);
  TextureSequence::LoadFile(*(SharedFilePtr*)data_ov085_0213073c);
  mShadowModel1.InitCylinder();
  mShadowModel2.InitCylinder();
  TextureSequence::Prepare(
      **(BMD_File**)(data_ov085_0213074c + 4), **(BTP_File**)(data_ov085_0213073c + 4));
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
      &mModelAnim1, *(BCA_File**)(data_ov085_02130744 + 4), 0, 0x1000, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
      &mTextureSequence, **(BTP_File**)(data_ov085_0213073c + 4), 0, 0x1000, 0);
  mModelAnim1.speed = 0x1000;
  unk_2d0 = param1 & 0xff;
  if (unk_2d0 == 0xff)
    unk_2d0 = 0;
  switch (unk_2d0) {
  case 0:
    func_ov085_0212e728((JugemHost *)this, &data_ov085_021307d0);
    break;
  case 1:
    {
      int v = data_0209caa0[2];
      if (v & 0x20000)
        return 0;
      if (v & 0x10000)
        data_0209caa0[2] = v & ~0x10000;
      unk_2dc = 1;
      if (!(data_0209caa0[2] & 0x80))
        func_ov085_0212e728((JugemHost *)this, &data_ov085_02130790);
      else
        func_ov085_0212e728((JugemHost *)this, &data_ov085_021307e0);
    }
    break;
  }
  return 1;
}

// @symbol daC_Jugem_c_classInit
/* The registry factory behind the C_JUGEM profile. It allocates 0x2e8 -- this
 * class's own sizeof -- and installs this class's vtable, which is the second
 * of the two independent witnesses the header's layout is read from.
 *
 * Reconstructed source-style name: SM64DS proves daC_Jugem_c through RTTI,
 * allocation size, vtable identity and the C_JUGEM registry profile; later EAD
 * lineage supplies classInit. The exact original spelling is not preserved.
 * Historical alias: LakituBro_Spawn.
 *
 * `return new daC_Jugem_c()` is the whole body: the synthesized constructor is
 * what stores the vptr and runs the two ModelAnim, one TextureSequence and two
 * ShadowModel constructors the ROM calls, in that order, and `operator new`
 * is fBase_c's. Spelling the vptr store by hand instead would be wrong here
 * as well as longer -- this TU emits _ZTV11daC_Jugem_c itself, so the store
 * has to be the compiler's own. */
extern "C" daC_Jugem_c *daC_Jugem_c_classInit(void)
{
    return new daC_Jugem_c();
}

