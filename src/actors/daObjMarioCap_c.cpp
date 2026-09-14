//cpp
/* Production translation unit for ov002/daObjMarioCap_c -- lost Mario cap
 * (overlay_actors CAP(269); RTTI ov002:0x021095ac names 15daObjMarioCap_c;
 * profile g_profile_OBJ_MARIO_CAP). ov002 is mixed (yoshi egg, switches,
 * stars, push block, player, ...); this is the cap, not those.
 *
 * deslop
 *
 * Twenty-nine of the run's thirty functions, written in reverse ROM order
 * because mwccarm 2004/b56 emits one .text section per function in the
 * reverse of source order. OnYoshiTryEat is the key function -- the first
 * out-of-line virtual daObjMarioCap_c declares after the inline destructor in
 * daObjMarioCap_c.h -- so the compiler owns retail's D1/D0 pair and the
 * complete RTTI/vtable group, and no D2 is retained.
 *
 * Leftover:
 * - classInit stays in src/d_a_obj_mario_cap.c: InitResources (0x020b86d0..
 *   0x020b8b98) sits between the licensed range and the factory and does not
 *   reproduce, so folding `return new daObjMarioCap_c()` would punch a hole
 *   in .text. Leaf operator new is on the class for when that join is legal.
 * - _ZN15daObjMarioCap_c13InitResourcesEv held out (cartridge 0x4c8 at
 *   0x020b86d0..0x020b8b98; pinned 2004/b56 emits 0x4d0, 8 bytes over;
 *   no `complete` marker).
 * - dActor_c::SetRanges stays mangled (header omits it; 6az).
 * - ModelAnim::SetAnim / DropShadowRadHeight / ReflectAngle stay mangled:
 *   Fix12<int> by value has no implicit int conversion (6az).
 * - ModelBase::ApplyOpacity stays mangled: the header takes one u32, the
 *   ROM call site passes two zeros.
 * - dBgCh_Actr::GetFloorResult / GetWallResult stay mangled (not declared).
 * - PMF stand-in (CapStateSelf / Holder / C) -- a PMF on the real
 *   dEnemyBase_c makes mwccarm ICE rather than a diagnostic.
 * - data_ov002_0210de* / 0210df* handles -- symbols.txt has no recovered
 *   names, so they are not coined.
 * - SharedFilePtr header has no fields; CleanupResources still casts the
 *   AnimRec tables.
 * - dBgCh_Gnd stays a 0x50 stack blob: C1/D1 only run on the airborne path
 *   (func_ov002_020b7f7c).
 * - +0xc8 override-matrix pointer lives in dActor_c pad_0c5 (header this
 *   class does not own).
 * - (long long)(int) 20.12 muls in func_ov002_020b781c, and the
 *   (unsigned long long)(c+0x12c) or-into-vulnFlags in func_ov002_020b7b70,
 *   keep the MATCH form (member addressing DIFFs).
 * - func_ov002_020b6fcc angle copy keeps `a = b ? a : a` through a V16.
 * - func_ov002_020b7330 / 020b781c / 020b7b70 / 020b7e1c keep offset soup:
 *   named members and real Spawn/ClosestPlayer/CopyNormalTo DIFF (s16 vs
 *   u16 copies; SetAnim `&mModelAnim` vs `this+0x300` size-DIFF).
 * - common.h first so Matrix4x3 is the flat s32[12] spelling; math/Matrix.h
 *   through ModelAnim.h would scalarize the copies in func_ov002_020b7f7c.
 */

#include "common.h"
#include "daObjMarioCap_c.h"
#include "decl_common.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "SaveData.h"
#include "Sound.h"
#include "SurfaceInfo.h"

/* --------------------------------------------------------------------------
 * Local shapes the merged members need.
 * ------------------------------------------------------------------------ */

/* The animation descriptor pairs these tables hold: a header word and the
 * BCA file pointer read at +4. Spelled as a struct because three members
 * reach the second word by name and two more take the address of the whole
 * record. SharedFilePtr.h commits to no fields, so CleanupResources still
 * casts these to SharedFilePtr*. */
struct AnimRec { void *f0; void *file; };

/* Behavior walks a pointer-to-member stashed at +0x3bc. NOT the real
 * dEnemyBase_c: a PMF on a non-polymorphic single-base class is laid out
 * differently from one on the real class, so the stand-in shape here is
 * codegen, not decoration. Letting the PMF bind to the real dEnemyBase_c
 * makes mwccarm abort with an internal compiler error rather than a
 * diagnostic. */
struct CapStateSelf { char pad[0x800]; };
typedef void (CapStateSelf::*CapStatePmf)();
struct Holder { char pad[8]; CapStatePmf fn; };

/* func_ov002_020b7f2c's own view of the same slot, with the class shaped so
 * the member pointer it stores and immediately calls is laid out the way the
 * ROM's bytes read it. */
struct C;
typedef int (C::*PMF)();
struct C { char pad[0x3bc]; PMF *pp; };

struct Flags3eb {
    u8 f0 : 1;
    u8 f1 : 1;
};

/* --------------------------------------------------------------------------
 * ROM symbols. Mangled spellings live inside extern "C" so the C++ front end
 * does not mangle them a second time.
 * ------------------------------------------------------------------------ */
extern "C" {

/* -- this TU's own members, forward-declared: mwcc lays .text down in reverse
      source order, so nearly every intra-TU call is a forward reference. -- */
void  func_ov002_020b6fcc(void *self);
int   func_ov002_020b71e8(void);
int   func_ov002_020b71f0(int *p);
int   func_ov002_020b7200(char *c);
int   func_ov002_020b7330(char *self);
int   func_ov002_020b74d0(char *c);
int   func_ov002_020b76ec(char *self);
int   func_ov002_020b781c(char *c);
int   func_ov002_020b7b70(char *c);
int   func_ov002_020b7c30(void *c);
int   func_ov002_020b7cdc(int *p);
int   func_ov002_020b7cec(char *c);
int   func_ov002_020b7d58(char *p);
int   func_ov002_020b7d6c(char *c);
int   func_ov002_020b7d94(void);
int   func_ov002_020b7d9c(char *c);
int   func_ov002_020b7e08(char *p);
/* func_ov002_020b7e1c is declared by decl_common.h; do not restate it. */
int   func_ov002_020b7f24(void);
int   func_ov002_020b7f2c(C *c, PMF *p);
void  func_ov002_020b7f7c(char *c);

/* -- other modules -- */
void  func_02013a88(void);
short func_02010844(void *self, void *v, short angle);
void  func_020167a4(void *p);
int   func_02037e58(void *p);
void  func_ov002_020f030c(int x);

unsigned short DecIfAbove0_Short(unsigned short *p);

int   Vec3_HorzLen(void *v);
void  Vec3_MulScalarInPlace(void *v, int s);
void  Vec3_Add(void *out, void *a, void *b);
void  Vec3_Asr(int *out, void *v, int shift);

void  Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
void  Matrix4x3_FromRotationY(void *m, int angle);

void  _Z11UpdateAngleRssis(void *p, short target, int step, short limit);
void  _Z14ApproachLinearRiii(int *x, int target, int step);

short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
int   _ZN4cstd4fdivEii(int a, int b);

void  _ZN9ModelBase12ApplyOpacityEj(void *self, int a, int b);

int   _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *self);
char *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void  _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *si, void *out);

void  _ZN7fBase_c18MarkForDestructionEv(void *self);
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
          u32 actorID, u32 param1, const struct Vector3 *pos,
          const struct Vector3_16 *rot, int areaID, int deathTableID);
int   _ZN8SaveData16HasPlayerLostCapEv(void);
void  _ZN8SaveData13PlayerLoseCapEv(void);

void  _ZN9dBgCh_GndC1Ev(char *ray);
void  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(char *ray, int *pos, int actor);
int   _ZN9dBgCh_Gnd10DetectClsnEv(char *ray);
void  _ZN9dBgCh_GndD1Ev(char *ray);

/* SetRanges carries Fix12<int> by value; dActor_c.h deliberately omits it
   (notes/mwccarm-codegen.md 6az). A call is unaffected. */
void  _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int offsetY, int radius,
                                                int clipDistance, int farDistance);
void  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
          void *self, void *shadow, void *matrix, int radius, int depth, int opacity);
short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int nx, int nz, short ang);
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int flags,
                                                  int speed, unsigned int startFrame);

/* -- data -- */
extern Matrix4x3      data_020a0e68;
extern short          data_02082214[];
extern int            data_02092138;
extern u8             data_0209f2d8[];

extern struct AnimRec data_ov002_0210de00, data_ov002_0210de08;
extern struct AnimRec data_ov002_0210de10, data_ov002_0210de18;
extern struct AnimRec data_ov002_0210de20, data_ov002_0210de28;
extern struct AnimRec data_ov002_0210de30, data_ov002_0210de38;
extern struct AnimRec data_ov002_0210de40, data_ov002_0210de48;
extern struct AnimRec data_ov002_0210de50, data_ov002_0210de58;
extern struct AnimRec data_ov002_0210de60;

/* data_ov002_0210df04..df84 are declared by decl_common.h as `char`; only
   df54 is missing there. Keep the family's spelling. */
extern char           data_ov002_0210df54;

extern struct AnimRec *data_ov002_020ff0a0[];
extern struct AnimRec *data_ov002_020ff0b8[];
extern int            *data_ov002_020ff0c4[];

}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c8BehaviorEv
int daObjMarioCap_c::Behavior()
{
    if (unk_400 != 0xff) {
        if (((Flags3eb *)&mCapIcon.mFlags)->f1 == 0) {
            unk_3ff = 1;
        } else if (unk_3ff == 1) {
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            unk_3ff = 0;
            unk_40c = 0x2000;
            unk_402 = 1;
            mVertSpeed = 0xf000;
            SmallPoofDust();
        }
    }

    if (unk_3ff == 1) {
        return 1;
    }

    if (unk_400 != 0xff && unk_402 != 0) {
        _Z14ApproachLinearRiii(&unk_40c, 0x1000, 0x200);
        _Z14ApproachLinearRiii(&mScaleX, unk_40c, 0x400);
        mScaleZ = mScaleX;
        mScaleY = mScaleZ;
        UpdatePos(&mdCcAc_c);
        UpdateWMClsn(mWithMeshClsn, 0);
        if (mWithMeshClsn.IsOnGround() != 0) {
            if (mScaleX == 0x1000) {
                unk_402 = 0;
            }
        }
    }

    {
        Holder *q = *(Holder **)&unk_3bc;
        if (q->fn != 0) {
            (((CapStateSelf *)this)->*(q->fn))();
        }
    }

    func_ov002_020b7f7c((char *)this);
    mModelAnim.Advance();

    if (mModelAnim.file != 0) {
        func_020167a4(&mModelAnim);
        mModelAnim.UpdateVerts();
    }

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        return 1;
    }

    {
        int v = mType;
        if (v != 4 && v != 0x11 && v != 6 && v != 8 && v != 0xc && v != 0xa
            && v != 0x13 && v != 0xf && v != 0x14 && v != 0x15 && v != 0x16 && v != 0xd) {
            UpdatePos(&mdCcAc_c);
            UpdateWMClsn(mWithMeshClsn, 0);
        }
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c6RenderEv
int daObjMarioCap_c::Render()
{
    int b = (mFlags & 0x40000) ? 1 : 0;
    if (b) return 1;
    if (unk_3ff == 1 || mScaleX < 0x100) return 1;

    int t = mType;
    if (t == 4 || t == 17 || t == 5 || t == 18 || t == 16 || t == 11 || (unsigned)(t - 6) <= 3) {
        if (!(unk_3ec & 1) || unk_3bc == (int)&data_ov002_0210df54) {
            mModelAnim.Model::Render((const Vector3 *)&mScaleX);
        }
    } else {
        mModelAnim.Model::Render((const Vector3 *)&mScaleX);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c16OnPendingDestroyEv
void daObjMarioCap_c::OnPendingDestroy()
{
    if (unk_400 == 0xff)
        return;
    mCapIcon.Unlink();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c16CleanupResourcesEv
int daObjMarioCap_c::CleanupResources()
{
  int i = mModelIndex;
  if (i >= 3) return 1;
  ((SharedFilePtr *)(data_ov002_020ff0ac[i]))->Release();
  switch (mType) {
  case 0xf:
    ((SharedFilePtr *)(&data_ov002_0210de50))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de60))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de48))->Release();
    break;
  case 0x14:
  case 0x15:
  case 0x16:
    ((SharedFilePtr *)(&data_ov002_0210de28))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de08))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de20))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de40))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de10))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de00))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de58))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de18))->Release();
    break;
  default:
    ((SharedFilePtr *)(&data_ov002_0210de30))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de38))->Release();
    break;
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/*                                                                            */
/* The key function: the first out-of-line virtual this class declares after   */
/* the inline destructor, so the vtable and RTTI group land in this TU.        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c13OnYoshiTryEatEv
s32 daObjMarioCap_c::OnYoshiTryEat() {
  if (mType == 2) return 0;
  return 4;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c13OnTurnIntoEggER6Player
void daObjMarioCap_c::OnTurnIntoEgg(Player &player)
{
    if (player.SetNoControlState(8, -1, 0) == 1) {
        player.SetNewHatCharacter(mModelIndex & 0xff, 1, 0);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov002_0210de30.file,
                                                    0x40000000, 0x1000, 0);
        unk_3fe = 1;
    }
    *(Player **)&unk_3c0 = &player;
    func_ov002_020b7f2c((C *)this, (PMF *)&data_ov002_0210df54);
}

/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Render-matrix update: syncs render angles outside fixed-angle states, uses  */
/* the override matrix at +0xc8 if set, otherwise builds translation*rotation  */
/* into the shared matrix and copies it in; then for grounded states computes  */
/* the drop shadow (raycast below when airborne).                              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7f7c
extern "C" void func_ov002_020b7f7c(char *c)
{
    daObjMarioCap_c *self = (daObjMarioCap_c *)c;
    int v2[3];
    int v[3];
    char ray[0x50];
    int m = self->mType;
    if (m != 5 && m != 0x12 && m != 0x10 && m != 0xb && m != 7 && m != 9 &&
        m != 0x14 && m != 0x15 && m != 0x16) {
        self->mAngleX = self->mPrevAngleX;
        self->mAngleY = self->mPrevAngleY;
        self->mAngleZ = self->mPrevAngleZ;
    }

    /* +0xc8 is inside dActor_c pad_0c5; this class does not own that header. */
    if (*(Matrix4x3 **)(c + 0xc8) != 0) {
        self->mModelAnim.mat4x3 = **(Matrix4x3 **)(c + 0xc8);
        return;
    }

    if ((unsigned int)(self->mType - 6) <= 1)
        _ZN9ModelBase12ApplyOpacityEj(&self->mModelAnim, 0, 0);

    Vec3_Asr(v, &self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, self->mAngleX,
                                           self->mAngleY, self->mAngleZ);
    self->mModelAnim.mat4x3 = data_020a0e68;

    {
        char *s = *(char **)&self->unk_3bc;
        if (s == &data_ov002_0210df54)
            return;
        if (s == &data_ov002_0210df74)
            return;
        if (s == &data_ov002_0210df44)
            return;
    }

    if (self->mScaleX < 0x500)
        return;

    {
        int y = self->mPosY;
        int off;
        if (self->mWithMeshClsn.IsOnGround() == 0) {
            v2[0] = self->mPosX;
            v2[1] = self->mPosY;
            v2[2] = self->mPosZ;
            v2[1] = v2[1] + 0x28000;
            _ZN9dBgCh_GndC1Ev(ray);
            _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(ray, v2, 0);
            y = v2[1];
            if (_ZN9dBgCh_Gnd10DetectClsnEv(ray) != 0)
                y = *(int *)(ray + 0x44);
            _ZN9dBgCh_GndD1Ev(ray);
        }

        off = 0;
        {
            int t = self->mModelIndex;
            if (t == 0)
                goto neg;
            if (t == 1) {
            neg:
                off = -10;
            }
        }
        Matrix4x3_FromRotationY(&self->unk_38c, self->mAngleY);
        self->unk_38c.m[9] = self->mPosX >> 3;
        self->unk_38c.m[10] = (y + (off << 12)) >> 3;
        self->unk_38c.m[11] = self->mPosZ >> 3;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            self, &self->mShadowModel, &self->unk_38c, 0x50000, 0x32000, 0xf);
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7f2c
extern "C" int func_ov002_020b7f2c(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 0; return (c->**q)(); }

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7f24
extern "C" int func_ov002_020b7f24(void)
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7e1c
extern "C" int func_ov002_020b7e1c(char* self) {
    /* Named dActor_c::Spawn / mStateTimer / SaveData:: DIFFs one word. */
    if (*(int*)(self + 0xc8) == 0 && !_ZN8SaveData16HasPlayerLostCapEv()) {
        if (*(unsigned short*)(self + 0x100) == 0 && _ZNK10dBgCh_Actr10IsOnGroundEv(self + 0x144)) {
            if (_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d, *(int*)(self + 8) | 0x12, (const struct Vector3 *)(self + 0x5c),
                    (const struct Vector3_16 *)0, *(signed char*)(self + 0xcc), -1)) {
                *(unsigned short*)(self + 0x100) = 3;
            }
        }
        if (DecIfAbove0_Short((unsigned short*)(self + 0x100)) == 1) {
            _ZN7fBase_c18MarkForDestructionEv(self);
        }
        return 1;
    }
    if (*(int*)(self + 0xc8) == 0) {
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(self, 0x32000, 0x32000, 0x1000000, 0x1000000);
    }
    func_ov002_020b6fcc(self);
    if (data_02092138 > *(int*)(self + 0x60)) {
        _ZN8SaveData13PlayerLoseCapEv();
        _ZN7fBase_c18MarkForDestructionEv(self);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7e08
extern "C" int func_ov002_020b7e08(char *p)
{
    ((daObjMarioCap_c *)p)->mStateTimer = 200;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7d9c
extern "C" int func_ov002_020b7d9c(char* c)
{
    daObjMarioCap_c *self = (daObjMarioCap_c *)c;
    if (data_02092138 > self->mPosY
        || self->mWithMeshClsn.IsOnGround()
        || !DecIfAbove0_Short((unsigned short*)&self->mStateTimer)
        || (self->mFlags & 8))
    {
        self->MarkForDestruction();
        return 1;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7d94
extern "C" int func_ov002_020b7d94(void)
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7d6c
extern "C" int func_ov002_020b7d6c(char* c) {
    if (*(int*)(c+0xc8) == 0)
        func_ov002_020b6fcc(c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7d58
extern "C" int func_ov002_020b7d58(char *p)
{
    ((daObjMarioCap_c *)p)->mStateTimer = 200;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7cec
extern "C" int func_ov002_020b7cec(char* c)
{
    daObjMarioCap_c *self = (daObjMarioCap_c *)c;
    if (data_02092138 > self->mPosY
        || self->mWithMeshClsn.IsOnGround()
        || !DecIfAbove0_Short((unsigned short*)&self->mStateTimer)
        || (self->mFlags & 8))
    {
        self->MarkForDestruction();
        return 1;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7cdc
extern "C" int func_ov002_020b7cdc(int *p)
{
    /* p[39] is mVertAccel (0x9c). int* index is the MATCH form. */
    p[39] = 0; return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7c30
extern "C" int func_ov002_020b7c30(void* c) {
  daObjMarioCap_c *self = (daObjMarioCap_c *)c;
  if (self->mVertAccel != 0) {
    self->UpdatePos(&self->mdCcAc_c);
    self->UpdateWMClsn(self->mWithMeshClsn, 0);
    self->mdCcAc_c.vulnFlags |= 0x8000;
    if (self->mWithMeshClsn.IsOnGround()) {
      self->mHorzSpeed = 0;
      func_ov002_020b6fcc(c);
      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(c, 0x32000, 0x32000, 0x1000000, 0x1000000);
    }
  }
  if (data_02092138 > self->mPosY) {
    SaveData::PlayerLoseCap();
    self->MarkForDestruction();
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7b70
extern "C" int func_ov002_020b7b70(char* c)
{
    /* mStateTimer/unk_404 member stores DIFF (s16 vs u16 copy). Keep offsets. */
    int state;
    int* p;
    int val;
    int ret;

    state = *(int*)(c + 0x3f0);
    if (state == 5 || state == 7 || state == 9) {
        *(u16*)(c + 0x100) = 0xd2;
        if ((int)(data_0209f2d8[0] == 1) == 0) *(u16*)(c + 0x100) = 0x78;
        *(u16*)(c + 0x404) = *(u16*)(c + 0x100);
    }

    state = *(int*)(c + 0x3f0);
    if (state == 0x12) {
        *(u16*)(c + 0x100) = 0x12c;
        *(u16*)(c + 0x404) = *(u16*)(c + 0x100);
    }

    state = *(int*)(c + 0x3f0);
    if (state == 0xb) {
        *(u16*)(c + 0x100) = 0xb4;
        *(u16*)(c + 0x404) = *(u16*)(c + 0x100);
    }

    /* (unsigned long long) is the MATCH form; mdCcAc_c.vulnFlags |= DIFFs. */
    p = (int *)((unsigned long long)(c + 0x12c));
    val = *p;
    ret = 1;
    val |= 0x8000;
    *p = val;
    return ret;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b781c
extern "C" int func_ov002_020b781c(char *c)
{
    /* Named-field / CopyNormalTo method form DIFFs 3 words. Keep MATCH offsets. */
    struct Vector3 v1;
    struct Vector3 v2;
    struct Vector3 out;
    int st;
    char *fr;
    int r6;
    int r4v;
    int spd;
    short ang;
    short a1;
    short a2;
    int b;
    int j;
    int s;
    int co;

    st = *(int *)(c + 0x3f0);
    if (st == 4 || st == 0x11 || st == 6 || st == 8 || st == 0x10) {
        func_ov002_020b6fcc(c);
        return 1;
    }
    if (DecIfAbove0_Short((unsigned short *)(c + 0x100)) == 0)
        return 1;
    if (*(u16 *)(c + 0x100) == 1) {
        b = (int)((*(int *)(c + 0xb0) & 0x60000) != 0);
        if (b == 0) {
            _ZN7fBase_c18MarkForDestructionEv(c);
            return 1;
        }
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144) == 0) {
        *(int *)(c + 0x408) = *(int *)(c + 0x98);
        return 1;
    }
    func_ov002_020b6fcc(c);
    if (*(u16 *)(c + 0x100) < *(u16 *)(c + 0x404) >> 1) {
        *(int *)(c + 0x3ec) = (*(u16 *)(c + 0x100) & 4) >> 2;
        if (*(u16 *)(c + 0x100) < *(u16 *)(c + 0x404) >> 2)
            *(int *)(c + 0x3ec) = (*(u16 *)(c + 0x100) & 2) >> 1;
    }
    if (*(int *)(c + 0x3f0) == 0x12) {
        *(int *)(c + 0x98) = 0;
        return 1;
    }
    fr = _ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x144);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, c + 0xd4);
    r6 = func_02037e58(fr + 4);
    *(short *)(c + 0x3fc) = _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(c + 0xd4), *(int *)(c + 0xdc));
    r4v = func_ov002_020f02c8(r6);
    func_ov002_020f030c(r6);
    spd = *(int *)(c + 0x98);
    j = (*(u16 *)(c + 0x94) >> 4) * 2;
    s = data_02082214[j];
    co = data_02082214[j + 1];
    /* (long long)(int) is the MATCH form; a plain 32-bit mul DIFFs. */
    v1.x = (int)(((long long)spd * s + 0x800) >> 12);
    v1.y = 0;
    v1.z = (int)(((long long)spd * co + 0x800) >> 12);
    j = (*(u16 *)(c + 0x3fc) >> 4) * 2;
    s = data_02082214[j];
    co = data_02082214[j + 1];
    v2.x = (int)(((long long)r4v * s + 0x800) >> 12);
    v2.y = 0;
    v2.z = (int)(((long long)r4v * co + 0x800) >> 12);
    Vec3_MulScalarInPlace(&v2, Vec3_HorzLen(c + 0xd4));
    Vec3_Add(&out, &v1, &v2);
    ang = _ZN4cstd5atan2E5Fix12IiES1_(out.x, out.z);
    *(int *)(c + 0x98) = Vec3_HorzLen(&out);
    if (*(int *)(c + 0x98) > 0xf000)
        *(int *)(c + 0x98) = 0xf000;
    *(short *)(c + 0x94) = ang;
    *(int *)(c + 0xa8) = -(_ZN4cstd4fdivEii(
        (int)(((long long)*(int *)(c + 0xd4) * *(int *)(c + 0xa4) + 0x800) >> 12)
      + (int)(((long long)*(int *)(c + 0xdc) * *(int *)(c + 0xac) + 0x800) >> 12),
        *(int *)(c + 0xd8)) + 0x8000);
    a1 = func_02010844(c, c + 0xd4, *(short *)(c + 0x8e));
    a2 = func_02010844(c, c + 0xd4, *(short *)(c + 0x8e) - 0x4000);
    _Z11UpdateAngleRssis(c + 0x8c, a1, 4, 0x1000);
    _Z11UpdateAngleRssis(c + 0x90, a2, 4, 0x1000);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b76ec
extern "C" int func_ov002_020b76ec(char *selfv)
{
    daObjMarioCap_c *self = (daObjMarioCap_c *)selfv;
    self->unk_0a4 = 0;
    self->mVertSpeed = 0;
    self->unk_0ac = 0;
    self->mVertAccel = 0;
    self->unk_401 = 0;
    self->unk_3ec = 0;
    *(int *)(selfv + 0xc8) = 0;
    self->unk_403 = 1;

    /* (int)self + off is the MATCH form; mFlags &= / vulnFlags &= CSE. */
    *(u32 *)(((int)selfv + 0xb0)) &= ~0x40000u;
    *(u32 *)(((int)selfv + 0x12c)) &= ~0x8000u;

    if (self->unk_3c0 == 0) {
        *(void **)&self->unk_3c0 = self->ClosestPlayer();
    }

    {
        unsigned b = (data_0209f2d8[0] == 1);
        if (b == 0) {
            Player *p = *(Player **)&self->unk_3c0;
            if (p->mCharacter == 3) {
                if (self->mModelIndex == (int)p->param1) {
                    struct Vector3 v;
                    Sound::PlayBank3(0x6e, *(Vector3 *)&self->mCamSpacePosX);
                    GiveLives(1);
                    v.x = self->mPosX;
                    v.y = self->mPosY;
                    v.z = self->mPosZ;
                    v.y += 0x64000;
                    dActor_c::Spawn(0x14b, 8, v, (Vector3_16 *)0, self->mAreaId, -1);
                }
            }
        }
    }

    self->mStateTimer = 0x96;
    self->mScaleX = 0x1000;
    self->mScaleY = 0x1000;
    self->mScaleZ = 0x1000;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b74d0
extern "C" int func_ov002_020b74d0(char *c) {
    daObjMarioCap_c *self = (daObjMarioCap_c *)c;
    if (DecIfAbove0_Short((unsigned short *)&self->mStateTimer) == 0) {
        self->MarkForDestruction();
        return 1;
    }

    if (*(void **)&self->unk_3c0 == 0) {
        *(void **)&self->unk_3c0 = self->ClosestPlayer();
        return 1;
    }

    *(int *)(c + 0xc8) = 0;

    {
        char *p = *(char **)&self->unk_3c0;
        char *q = (char *)(((int)(p + 0x8c)));
        self->mPrevAngleX = *(s16 *)(q + 0);
        self->mPrevAngleY = *(s16 *)(q + 2);
        self->mPrevAngleZ = *(s16 *)(q + 4);
    }
    self->mAngleX = self->mPrevAngleX;
    self->mAngleY = self->mPrevAngleY;
    self->mAngleZ = self->mPrevAngleZ;

    {
        char *p = *(char **)&self->unk_3c0;
        char *q = (char *)(((int)(p + 0x5c)));
        self->mPosX = *(int *)(q + 0);
        self->mPosY = *(int *)(q + 4);
        self->mPosZ = *(int *)(q + 8);
    }

    switch (self->unk_401) {
    case 1:
        if ((*(Player **)&self->unk_3c0)->SetNoControlState(0xf, -1, 0) == 1) {
            (*(Player **)&self->unk_3c0)->SetNewHatCharacter(self->mModelIndex & 0xff, 0, 0);
            func_02013a88();
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov002_0210de38.file, 0x40000000, 0x1000, 0);
            self->unk_401 = 2;
        }
        break;
    case 2:
        if (self->mModelAnim.Finished() != 0) {
            self->MarkForDestruction();
        }
        break;
    }

    if (self->unk_401 != 0) {
        return 1;
    }

    if (self->unk_3fe == 0) {
        if ((*(Player **)&self->unk_3c0)->Unk_020c9e5c(8) == 1) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov002_0210de30.file, 0x40000000, 0x1000, 0);
            self->unk_3fe = 1;
        }
    }

    if (self->unk_3fe == 1) {
        if (self->mModelAnim.Finished() != 0) {
            if (self->mType == 4) {
                dActor_c::Spawn(0x10d, self->param1, *(const Vector3 *)&self->unk_3c4, (const Vector3_16 *)0, self->mAreaId, -1);
            }
            self->MarkForDestruction();
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7330
extern "C" int func_ov002_020b7330(char* self)
{
    /* ClosestPlayer() / mAngle* / &mModelAnim SetAnim DIFFs size. Keep MATCH offsets. */
    int state;

    *(int*)(self + 0x3ec) = 0;
    *(unsigned char*)(self + 0x3fe) = 0;

    state = *(int*)(self + 0x3f0);
    if (state == 0xa || state == 0xf) {
        char* closest = (char*)_ZN8dActor_c13ClosestPlayerEv(self);
        if (closest != 0) {
            s16* src = (s16*)(closest + 0x8c);
            *(s16*)(self + 0x92) = src[0];
            *(s16*)(self + 0x94) = src[1];
            *(s16*)(self + 0x96) = src[2];
            *(s16*)(self + 0x8c) = *(s16*)(self + 0x92);
            *(s16*)(self + 0x8e) = *(s16*)(self + 0x94);
            *(s16*)(self + 0x90) = *(s16*)(self + 0x96);
            *(int*)(self + 0x3c0) = (int)closest;
        }
    }

    switch (*(int*)(self + 0x3f0)) {
    case 0xa:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_0210de30.file, 0x40000000, 0x1000, 0);
        break;
    case 0xf:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_020ff0a0[*(int*)(self + 0x3f4)]->file, 0x40000000, 0x1000, 0);
        break;
    case 0x14:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_020ff0b8[*(int*)(self + 0x3f4)]->file, 0x40000000, 0x1000, 0);
        break;
    case 0x15:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_0210de58.file, 0x40000000, 0x1000, 0);
        break;
    case 0x16:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_0210de18.file, 0x40000000, 0x1000, 0);
        break;
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7200
extern "C" int func_ov002_020b7200(char* c)
{
    daObjMarioCap_c *self = (daObjMarioCap_c *)c;
    short* sp;
    int* ip;

    if (self->mType == 0xa || self->mType == 0xf) {
        if (self->unk_3c0 != 0) {
            sp = (short*)(
                (char*)self->unk_3c0 + 0x8c);
            self->mPrevAngleX = sp[0];
            self->mPrevAngleY = sp[1];
            self->mPrevAngleZ = sp[2];
            self->mAngleX = self->mPrevAngleX;
            self->mAngleY = self->mPrevAngleY;
            self->mAngleZ = self->mPrevAngleZ;

            ip = (int*)(
                (char*)self->unk_3c0 + 0x5c);
            self->mPosX = ip[0];
            self->mPosY = ip[1];
            self->mPosZ = ip[2];
        }
    }

    if (self->mModelAnim.Finished() != 0) {
        switch (self->mType) {
        case 0xa:
        case 0xf:
        case 0x16:
            self->MarkForDestruction();
            break;

        case 0x15:
            break;

        case 0x14:
            if (self->unk_3fe == 0) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    &self->mModelAnim,
                    (void*)data_ov002_020ff0c4[self->mModelIndex][1],
                    0, 0x1000, 0);
                self->unk_3fe = 1;
            }
            break;
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b71f0
extern "C" int func_ov002_020b71f0(int *p)
{
    /* p[251] is unk_3ec (0x3ec). int* index is the MATCH form. */
    p[251] = 0; return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b71e8
extern "C" int func_ov002_020b71e8(void)
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b6fcc
extern "C" void func_ov002_020b6fcc(void* selfv)
{
    daObjMarioCap_c *self = (daObjMarioCap_c *)selfv;
    struct V16 { u16 x, y, z; } v;
    int normal[3];
    int state;

    if (self->mWithMeshClsn.IsOnWall() != 0) {
        void* wr = _ZNK10dBgCh_Actr13GetWallResultEv(&self->mWithMeshClsn);
        ((SurfaceInfo *)((char*)wr + 4))->CopyNormalTo(*(Vector3 *)&normal[0]);
        self->mPrevAngleY = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
            self, normal[0], normal[2], self->mPrevAngleY);
    }

    if (self->mdCcAc_c.otherOwner == 0) return;

    *(void**)&self->unk_3c0 = dActor_c::FindWithID(self->mdCcAc_c.otherOwner);
    if (*(void**)&self->unk_3c0 == 0) return;

    {
        int t = ((*(Player **)&self->unk_3c0)->actorID == 0xbf);
        if (t == false) return;
    }

    if ((self->mdCcAc_c.hitFlags & 0x8000) != 0) return;
    if ((*(Player **)&self->unk_3c0)->IsCollectingCap() != 0) return;

    if ((*(Player **)&self->unk_3c0)->param1 == 3) {
        if ((*(Player **)&self->unk_3c0)->mObjInMouth != 0) return;
    }

    state = self->mType;
    if ((unsigned)(state - 6) <= 1) {
        (*(Player **)&self->unk_3c0)->InitVanishLuigi();
        self->MarkForDestruction();
        return;
    }
    if ((unsigned)(state - 8) <= 1) {
        (*(Player **)&self->unk_3c0)->InitMetalWario();
        self->MarkForDestruction();
        return;
    }

    if (*(int*)((char*)selfv + 0xc8) != 0) {
        if (state != 0) return;
    }

    if (SaveData::HasPlayerLostCap() != 0) {
        func_ov002_020b7f2c((C*)self, (PMF*)&data_ov002_0210df54);
        self->unk_401 = 1;
        return;
    }

    if ((*(Player **)&self->unk_3c0)->SetNoControlState(8, -1, 0) == 0) return;

    (*(Player **)&self->unk_3c0)->SetNewHatCharacter(
        self->mModelIndex & 0xff, 0, 0);

    {
        /* `a = b ? a : a` through a V16 is the MATCH form. */
        char* found = *(char**)&self->unk_3c0;
        int a = *(u16*)(found + 0x8c);
        int b = *(u16*)(found + 0x8e);
        a = b ? a : a;
        v.x = a;
        v.y = b;
        v.z = *(u16*)(found + 0x90);
        self->mPrevAngleX = *(s16*)&v.x;
        self->mPrevAngleY = *(s16*)&v.y;
        self->mPrevAngleZ = *(s16*)&v.z;
        self->mAngleX = self->mPrevAngleX;
        self->mAngleY = self->mPrevAngleY;
        self->mAngleZ = self->mPrevAngleZ;
    }

    func_ov002_020b7f2c((C*)self, (PMF*)&data_ov002_0210df54);
}
