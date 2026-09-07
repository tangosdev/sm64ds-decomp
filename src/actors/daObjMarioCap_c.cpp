//cpp
/* Production translation unit for ov002/daObjMarioCap_c.
 *
 * Twenty-nine of the run's thirty functions, written in reverse ROM order
 * because mwccarm 2004/b56 emits one .text section per function in the
 * reverse of source order. OnYoshiTryEat is the key function -- the first
 * out-of-line virtual daObjMarioCap_c declares after the inline destructor in
 * daObjMarioCap_c.h -- so the compiler owns retail's D1/D0 pair and the
 * complete RTTI/vtable group, and no D2 is retained.
 *
 * _ZN15daObjMarioCap_c13InitResourcesEv (0x020b86d0, the top of the run) is
 * NOT here: its src/ shard does not reproduce under the pinned compiler and
 * carries no `complete` marker in delinks.txt. It sits above every function
 * below it, so holding it out leaves a contiguous 0x020b6f18..0x020b86d0
 * licensed range with no hole. It stays enrolled as its own shard.
 */

#include "daObjMarioCap_c.h"
#include "decl_common.h"
#include "common.h"
#include "Player.h"
#include "SharedFilePtr.h"

/* --------------------------------------------------------------------------
 * Local shapes the merged members need.
 * ------------------------------------------------------------------------ */

/* The animation descriptor pairs these tables hold: a header word and the
 * BCA file pointer read at +4. Spelled as a struct because three members
 * reach the second word by name and two more take the address of the whole
 * record. */
struct AnimRec { void *f0; void *file; };

/* Behavior walks a pointer-to-member stashed at +0x3bc. NOT the real
 * dEnemyBase_c: a PMF on a non-polymorphic single-base class is laid out
 * differently from one on the real class, so the stand-in shape here is
 * codegen, not decoration. Letting the PMF bind to the real dEnemyBase_c
 * makes mwccarm abort with an internal compiler error rather than a
 * diagnostic. */
struct MistPmfSelf { char pad[0x800]; };
typedef void (MistPmfSelf::*MistPmf)();
struct Holder { char pad[8]; MistPmf fn; };

/* func_ov002_020b7f2c's own view of the same slot, with the class shaped so
 * the member pointer it stores and immediately calls is laid out the way the
 * ROM's bytes read it. */
struct C;
typedef int (C::*PMF)();
struct C { char pad[0x3bc]; PMF *pp; };

/* The model object reached through +0x300 is called virtually at slot 3. */
struct VObj {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03(); /* slot 3 = 0xc */
};

struct Flags3eb {
    u8 f0 : 1;
    u8 f1 : 1;
};

/* FLAT Matrix4x3, deliberately local and deliberately not named Matrix4x3.
 * math/Matrix.h reaches this TU through daObjMarioCap_c.h -> ModelAnim.h and
 * spells Matrix4x3 as `Matrix3x3 r; Vector3 t;`, and func_ov002_020b7f7c
 * whole-struct-assigns one: under the structured spelling mwcc splits the copy
 * into a 9-word ldm/stm plus a CSE'd 3-word tail and the function comes out
 * long. The array member is what the cartridge's block move needs. */
struct Mat12 { s32 m[12]; };

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

void  Matrix4x3_FromTranslation(struct Mat12 *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(struct Mat12 *m, int x, int y, int z);
void  Matrix4x3_FromRotationY(void *m, int angle);

void  _Z11UpdateAngleRssis(void *p, short target, int step, short limit);
void  _Z14ApproachLinearRiii(int *x, int target, int step);

int   _ZN8SaveData16HasPlayerLostCapEv(void);
void  _ZN8SaveData13PlayerLoseCapEv(void);

void *_ZN8dActor_c13ClosestPlayerEv(void *self);
void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
          u32 actorID, u32 param1, const struct Vector3 *pos,
          const struct Vector3_16 *rot, int areaID, int deathTableID);
void  _ZN8dActor_c13SmallPoofDustEv(void *self);
void  _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int nx, int nz, short ang);
void  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
          void *self, void *shadow, void *matrix, int radius, int depth, int opacity);
/* SetRanges and SetAnim carry Fix12<int> by value; their definitions stay
   mangled free functions, and dActor_c.h deliberately omits SetRanges for the
   same reason. A call is unaffected -- see notes/mwccarm-codegen.md 6az. */
void  _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int offsetY, int radius,
                                                int clipDistance, int farDistance);
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int flags,
                                                  int speed, unsigned int startFrame);

void  _ZN7fBase_c18MarkForDestructionEv(void *self);

int   _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *self, void *wm, unsigned int sel);
int   _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void *self, void *wm);

int   _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
int   _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *self);
char *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void  _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *si, void *out);

void  _ZN9dBgCh_GndC1Ev(char *ray);
void  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(char *ray, int *pos, int actor);
int   _ZN9dBgCh_Gnd10DetectClsnEv(char *ray);
void  _ZN9dBgCh_GndD1Ev(char *ray);

int   _ZN9Animation8FinishedEv(void *self);
void  _ZN9Animation7AdvanceEv(void *self);
int   _ZN5Model6RenderEPK7Vector3(void *self, const void *pos);
void  _ZN9ModelBase12ApplyOpacityEj(void *self, int a, int b);

void  _ZN5dCc_c5ClearEv(void *self);
void  _ZN5dCc_c6UpdateEv(void *self);

int   _ZN6Player15IsCollectingCapEv(void *p);
void  _ZN6Player15InitVanishLuigiEv(void *p);
void  _ZN6Player14InitMetalWarioEv(void *p);
int   _ZN6Player12Unk_020c9e5cEh(void *p, int a);
int   _ZN6Player17SetNoControlStateEhih(void *p, int a, int b, int c);
void  _ZN6Player18SetNewHatCharacterEjjb(void *p, unsigned int a, unsigned int b, int c);

void  _ZN5Sound9PlayBank3EjRK7Vector3(u32 soundID, const struct Vector3 *pos);
short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
int   _ZN4cstd4fdivEii(int a, int b);

/* -- data -- */
extern struct Mat12   data_020a0e68;
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
/* ROM ordinal 28 -- _ZN15daObjMarioCap_c8BehaviorEv, 0x020b84a8, size 0x228   */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c8BehaviorEv
int daObjMarioCap_c::Behavior()
{
    char *c = (char *)((MistPmfSelf *)this);

    if (*(u8 *)(c + 0x400) != 0xff) {
        if (((Flags3eb *)(c + 0x3eb))->f1 == 0) {
            *(u8 *)(c + 0x3ff) = 1;
        } else if (*(u8 *)(c + 0x3ff) == 1) {
            *(int *)(c + 0x80) = 0;
            *(int *)(c + 0x84) = 0;
            *(int *)(c + 0x88) = 0;
            *(u8 *)(c + 0x3ff) = 0;
            *(int *)(c + 0x40c) = 0x2000;
            *(u8 *)(c + 0x402) = 1;
            *(int *)(c + 0xa8) = 0xf000;
            _ZN8dActor_c13SmallPoofDustEv(((MistPmfSelf *)this));
        }
    }

    if (*(u8 *)(c + 0x3ff) == 1) {
        return 1;
    }

    if (*(u8 *)(c + 0x400) != 0xff && *(u8 *)(c + 0x402) != 0) {
        _Z14ApproachLinearRiii((int *)(c + 0x40c), 0x1000, 0x200);
        _Z14ApproachLinearRiii((int *)(c + 0x80), *(int *)(c + 0x40c), 0x400);
        *(int *)(c + 0x88) = *(int *)(c + 0x80);
        *(int *)(c + 0x84) = *(int *)(c + 0x88);
        _ZN8dActor_c9UpdatePosEP5dCc_c(((MistPmfSelf *)this), c + 0x110);
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((MistPmfSelf *)this), c + 0x144, 0);
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144) != 0) {
            if (*(int *)(c + 0x80) == 0x1000) {
                *(u8 *)(c + 0x402) = 0;
            }
        }
    }

    {
        Holder *q = *(Holder **)(c + 0x3bc);
        if (q->fn != 0) {
            (((MistPmfSelf *)this)->*(q->fn))();
        }
    }

    func_ov002_020b7f7c(c);
    _ZN9Animation7AdvanceEv(c + 0x350);

    if (*(int *)(c + 0x360) != 0) {
        func_020167a4(c + 0x300);
        ((VObj *)(c + 0x300))->v03();
    }

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((MistPmfSelf *)this), c + 0x144) != 0) {
        return 1;
    }

    {
        int v = *(int *)(c + 0x3f0);
        if (v != 4 && v != 0x11 && v != 6 && v != 8 && v != 0xc && v != 0xa
            && v != 0x13 && v != 0xf && v != 0x14 && v != 0x15 && v != 0x16 && v != 0xd) {
            _ZN8dActor_c9UpdatePosEP5dCc_c(((MistPmfSelf *)this), c + 0x110);
            _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((MistPmfSelf *)this), c + 0x144, 0);
        }
    }

    _ZN5dCc_c5ClearEv(c + 0x110);
    _ZN5dCc_c6UpdateEv(c + 0x110);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- _ZN15daObjMarioCap_c6RenderEv, 0x020b83c4, size 0xe4      */
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
            _ZN5Model6RenderEPK7Vector3(((char *)this) + 0x300, ((char *)this) + 0x80);
        }
    } else {
        _ZN5Model6RenderEPK7Vector3(((char *)this) + 0x300, ((char *)this) + 0x80);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- _ZN15daObjMarioCap_c16OnPendingDestroyEv, 0x020b8394      */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c16OnPendingDestroyEv
void daObjMarioCap_c::OnPendingDestroy()
{
    if (unk_400 == 0xff)
        return;
    mCapIcon.Unlink();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN15daObjMarioCap_c16CleanupResourcesEv, 0x020b8284      */
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
/* ROM ordinal 24 -- _ZN15daObjMarioCap_c13OnYoshiTryEatEv, 0x020b8270         */
/*                                                                            */
/* The key function: the first out-of-line virtual this class declares after   */
/* the inline destructor, so the vtable and RTTI group land in this TU.        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c13OnYoshiTryEatEv
s32 daObjMarioCap_c::OnYoshiTryEat() {
    char* c = (char*)this;
  if(*(int*)(c+0x3f0)==2) return 0;
  return 4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- _ZN15daObjMarioCap_c13OnTurnIntoEggER6Player, 0x020b81e0  */
/*                                                                            */
/* Vtable slot 19. The ROM returns nothing: dActor_c declares slot 19 as `int` */
/* and the cartridge simply leaves r0 alone on the way out, so the declared    */
/* return type is the ancestor's and no value is produced here.                */
/* 0x3fe falls inside the header's pad_3f8[0x7]; 0x3c0 is unk_3c0, where the   */
/* player that just ate the cap is stashed for the follow-up state.            */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c13OnTurnIntoEggER6Player
int daObjMarioCap_c::OnTurnIntoEgg(Player &player)
{
    if (player.SetNoControlState(8, -1, 0) == 1) {
        player.SetNewHatCharacter(mModelIndex & 0xff, 1, 0);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov002_0210de30.file,
                                                    0x40000000, 0x1000, 0);
        ((char *)this)[0x3fe] = 1;
    }
    *(Player **)&unk_3c0 = &player;
    func_ov002_020b7f2c((C *)this, (PMF *)&data_ov002_0210df54);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov002_020b7f7c, 0x020b7f7c, size 0x264               */
/*                                                                            */
/* Render-matrix update: syncs render angles outside fixed-angle states, uses  */
/* the override matrix at +0xc8 if set, otherwise builds translation*rotation  */
/* into the shared matrix and copies it in; then for grounded states computes  */
/* the drop shadow (raycast below when airborne).                              */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7f7c
extern "C" void func_ov002_020b7f7c(char *c)
{
    int v2[3];
    int v[3];
    char ray[0x50];
    int m = *(int *)(c + 0x3f0);
    if (m != 5 && m != 0x12 && m != 0x10 && m != 0xb && m != 7 && m != 9 &&
        m != 0x14 && m != 0x15 && m != 0x16) {
        *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x92);
        *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
        *(s16 *)(c + 0x90) = *(s16 *)(c + 0x96);
    }

    if (*(struct Mat12 **)(c + 0xc8) != 0) {
        *(struct Mat12 *)(c + 0x31c) = **(struct Mat12 **)(c + 0xc8);
        return;
    }

    if ((unsigned int)(*(int *)(c + 0x3f0) - 6) <= 1)
        _ZN9ModelBase12ApplyOpacityEj(c + 0x300, 0, 0);

    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(s16 *)(c + 0x8c),
                                           *(s16 *)(c + 0x8e), *(s16 *)(c + 0x90));
    *(struct Mat12 *)(c + 0x31c) = data_020a0e68;

    {
        char *s = *(char **)(c + 0x3bc);
        if (s == &data_ov002_0210df54)
            return;
        if (s == &data_ov002_0210df74)
            return;
        if (s == &data_ov002_0210df44)
            return;
    }

    if (*(int *)(c + 0x80) < 0x500)
        return;

    {
        int y = *(int *)(c + 0x60);
        int off;
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144) == 0) {
            v2[0] = *(int *)(c + 0x5c);
            v2[1] = *(int *)(c + 0x60);
            v2[2] = *(int *)(c + 0x64);
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
            int t = *(int *)(c + 0x3f4);
            if (t == 0)
                goto neg;
            if (t == 1) {
            neg:
                off = -10;
            }
        }
        Matrix4x3_FromRotationY(c + 0x38c, *(s16 *)(c + 0x8e));
        *(int *)(c + 0x3b0) = *(int *)(c + 0x5c) >> 3;
        *(int *)(c + 0x3b4) = (y + (off << 12)) >> 3;
        *(int *)(c + 0x3b8) = *(int *)(c + 0x64) >> 3;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x364, c + 0x38c, 0x50000, 0x32000, 0xf);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov002_020b7f2c, 0x020b7f2c, size 0x50                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7f2c
extern "C" int func_ov002_020b7f2c(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 0; return (c->**q)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov002_020b7f24, 0x020b7f24, size 0x8                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7f24
extern "C" int func_ov002_020b7f24(void)
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov002_020b7e1c, 0x020b7e1c, size 0x108               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7e1c
extern "C" int func_ov002_020b7e1c(char* self) {
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
/* ROM ordinal 18 -- func_ov002_020b7e08, 0x020b7e08, size 0x14                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7e08
extern "C" int func_ov002_020b7e08(char *p)
{
    *(short *)(p + 0x100) = 200;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov002_020b7d9c, 0x020b7d9c, size 0x6c                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7d9c
extern "C" int func_ov002_020b7d9c(char* c)
{
    if (data_02092138 > *(int*)(c + 0x60)
        || _ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144)
        || !DecIfAbove0_Short((unsigned short*)(c + 0x100))
        || (*(int*)(c + 0xb0) & 8))
    {
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov002_020b7d94, 0x020b7d94, size 0x8                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7d94
extern "C" int func_ov002_020b7d94(void)
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov002_020b7d6c, 0x020b7d6c, size 0x28                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7d6c
extern "C" int func_ov002_020b7d6c(char* c) {
    if (*(int*)(c+0xc8) == 0)
        func_ov002_020b6fcc(c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov002_020b7d58, 0x020b7d58, size 0x14                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7d58
extern "C" int func_ov002_020b7d58(char *p)
{
    *(short *)(p + 0x100) = 200;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov002_020b7cec, 0x020b7cec, size 0x6c                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7cec
extern "C" int func_ov002_020b7cec(char* c)
{
    if (data_02092138 > *(int*)(c + 0x60)
        || _ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144)
        || !DecIfAbove0_Short((unsigned short*)(c + 0x100))
        || (*(int*)(c + 0xb0) & 8))
    {
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov002_020b7cdc, 0x020b7cdc, size 0x10                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7cdc
extern "C" int func_ov002_020b7cdc(int *p)
{
    p[39] = 0; return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov002_020b7c30, 0x020b7c30, size 0xac                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7c30
extern "C" int func_ov002_020b7c30(void* c) {
  if (*(int*)((char*)c + 0x9c) != 0) {
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char*)c + 0x110);
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, (char*)c + 0x144, 0);
    *(unsigned*)((char*)c + 0x12c) |= 0x8000;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char*)c + 0x144)) {
      *(int*)((char*)c + 0x98) = 0;
      func_ov002_020b6fcc(c);
      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(c, 0x32000, 0x32000, 0x1000000, 0x1000000);
    }
  }
  if (data_02092138 > *(int*)((char*)c + 0x60)) {
    _ZN8SaveData13PlayerLoseCapEv();
    _ZN7fBase_c18MarkForDestructionEv(c);
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov002_020b7b70, 0x020b7b70, size 0xc0                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7b70
extern "C" int func_ov002_020b7b70(char* c)
{
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

    p = (int *)((unsigned long long)(c + 0x12c));
    val = *p;
    ret = 1;
    val |= 0x8000;
    *p = val;
    return ret;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov002_020b781c, 0x020b781c, size 0x354                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b781c
extern "C" int func_ov002_020b781c(char *c)
{
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
/* ROM ordinal 8 -- func_ov002_020b76ec, 0x020b76ec, size 0x130                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b76ec
extern "C" int func_ov002_020b76ec(char *self)
{
    *(int *)(self + 0xa4) = 0;
    *(int *)(self + 0xa8) = 0;
    *(int *)(self + 0xac) = 0;
    *(int *)(self + 0x9c) = 0;
    *(u8 *)(self + 0x401) = 0;
    *(int *)(self + 0x3ec) = 0;
    *(int *)(self + 0xc8) = 0;
    *(u8 *)(self + 0x403) = 1;

    *(u32 *)(((int)self + 0xb0)) &= ~0x40000u;
    *(u32 *)(((int)self + 0x12c)) &= ~0x8000u;

    if (*(int *)(self + 0x3c0) == 0) {
        *(void **)(self + 0x3c0) = _ZN8dActor_c13ClosestPlayerEv(self);
    }

    {
        unsigned b = (data_0209f2d8[0] == 1);
        if (b == 0) {
            char *p = *(char **)(self + 0x3c0);
            if (*(u8 *)(p + 0x6d9) == 3) {
                if (*(int *)(self + 0x3f4) == *(int *)(p + 8)) {
                    struct Vector3 v;
                    _ZN5Sound9PlayBank3EjRK7Vector3(0x6e, (struct Vector3 *)(self + 0x74));
                    GiveLives(1);
                    v.x = *(int *)(self + 0x5c);
                    v.y = *(int *)(self + 0x60);
                    v.z = *(int *)(self + 0x64);
                    v.y += 0x64000;
                    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x14b, 8, &v, (struct Vector3_16 *)0, *(signed char *)(self + 0xcc), -1);
                }
            }
        }
    }

    *(u16 *)(self + 0x100) = 0x96;
    *(int *)(self + 0x80) = 0x1000;
    *(int *)(self + 0x84) = 0x1000;
    *(int *)(self + 0x88) = 0x1000;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov002_020b74d0, 0x020b74d0, size 0x21c                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b74d0
extern "C" int func_ov002_020b74d0(char *c) {
    if (DecIfAbove0_Short((unsigned short *)(c + 0x100)) == 0) {
        _ZN7fBase_c18MarkForDestructionEv(c);
        return 1;
    }

    if (*(void **)(c + 0x3c0) == 0) {
        *(void **)(c + 0x3c0) = _ZN8dActor_c13ClosestPlayerEv(c);
        return 1;
    }

    *(int *)(c + 0xc8) = 0;

    {
        char *p = *(char **)(c + 0x3c0);
        char *q = (char *)(((int)(p + 0x8c)));
        *(s16 *)(c + 0x92) = *(s16 *)(q + 0);
        *(s16 *)(c + 0x94) = *(s16 *)(q + 2);
        *(s16 *)(c + 0x96) = *(s16 *)(q + 4);
    }
    *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x92);
    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
    *(s16 *)(c + 0x90) = *(s16 *)(c + 0x96);

    {
        char *p = *(char **)(c + 0x3c0);
        char *q = (char *)(((int)(p + 0x5c)));
        *(int *)(c + 0x5c) = *(int *)(q + 0);
        *(int *)(c + 0x60) = *(int *)(q + 4);
        *(int *)(c + 0x64) = *(int *)(q + 8);
    }

    switch (*(u8 *)(c + 0x401)) {
    case 1:
        if (_ZN6Player17SetNoControlStateEhih(*(void **)(c + 0x3c0), 0xf, -1, 0) == 1) {
            _ZN6Player18SetNewHatCharacterEjjb(*(void **)(c + 0x3c0), *(u32 *)(c + 0x3f4) & 0xff, 0, 0);
            func_02013a88();
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov002_0210de38.file, 0x40000000, 0x1000, 0);
            *(u8 *)(c + 0x401) = 2;
        }
        break;
    case 2:
        if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
            _ZN7fBase_c18MarkForDestructionEv(c);
        }
        break;
    }

    if (*(u8 *)(c + 0x401) != 0) {
        return 1;
    }

    if (*(u8 *)(c + 0x3fe) == 0) {
        if (_ZN6Player12Unk_020c9e5cEh(*(void **)(c + 0x3c0), 8) == 1) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov002_0210de30.file, 0x40000000, 0x1000, 0);
            *(u8 *)(c + 0x3fe) = 1;
        }
    }

    if (*(u8 *)(c + 0x3fe) == 1) {
        if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
            if (*(int *)(c + 0x3f0) == 4) {
                _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x10d, *(u32 *)(c + 8), (const struct Vector3 *)(c + 0x3c4), (const struct Vector3_16 *)0, *(signed char *)(c + 0xcc), -1);
            }
            _ZN7fBase_c18MarkForDestructionEv(c);
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov002_020b7330, 0x020b7330, size 0x1a0                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7330
extern "C" int func_ov002_020b7330(char* self)
{
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
/* ROM ordinal 5 -- func_ov002_020b7200, 0x020b7200, size 0x130                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b7200
extern "C" int func_ov002_020b7200(char* c)
{
    short* sp;
    int* ip;

    if (*(int*)(c + 0x3f0) == 0xa || *(int*)(c + 0x3f0) == 0xf) {
        if (*(int*)(c + 0x3c0) != 0) {
            sp = (short*)(
                (char*)*(int*)(c + 0x3c0) + 0x8c);
            *(short*)(c + 0x92) = sp[0];
            *(short*)(c + 0x94) = sp[1];
            *(short*)(c + 0x96) = sp[2];
            *(short*)(c + 0x8c) = *(short*)(c + 0x92);
            *(short*)(c + 0x8e) = *(short*)(c + 0x94);
            *(short*)(c + 0x90) = *(short*)(c + 0x96);

            ip = (int*)(
                (char*)*(int*)(c + 0x3c0) + 0x5c);
            *(int*)(c + 0x5c) = ip[0];
            *(int*)(c + 0x60) = ip[1];
            *(int*)(c + 0x64) = ip[2];
        }
    }

    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        switch (*(int*)(c + 0x3f0)) {
        case 0xa:
        case 0xf:
        case 0x16:
            _ZN7fBase_c18MarkForDestructionEv(c);
            break;

        case 0x15:
            break;

        case 0x14:
            if (*(unsigned char*)(c + 0x3fe) == 0) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    c + 0x300,
                    (void*)data_ov002_020ff0c4[*(int*)(c + 0x3f4)][1],
                    0, 0x1000, 0);
                *(unsigned char*)(c + 0x3fe) = 1;
            }
            break;
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020b71f0, 0x020b71f0, size 0x10                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b71f0
extern "C" int func_ov002_020b71f0(int *p)
{
    p[251] = 0; return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020b71e8, 0x020b71e8, size 0x8                  */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b71e8
extern "C" int func_ov002_020b71e8(void)
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020b6fcc, 0x020b6fcc, size 0x21c                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b6fcc
extern "C" void func_ov002_020b6fcc(void* selfv)
{
    char* self = (char*)selfv;
    struct V16 { u16 x, y, z; } v;
    int normal[3];
    int state;

    if (_ZNK10dBgCh_Actr8IsOnWallEv(self + 0x144) != 0) {
        void* wr = _ZNK10dBgCh_Actr13GetWallResultEv(self + 0x144);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)wr + 4, &normal[0]);
        *(s16*)(self + 0x94) = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
            self, normal[0], normal[2], *(s16*)(self + 0x94));
    }

    if (*(int*)(self + 0x134) == 0) return;

    *(void**)(self + 0x3c0) = _ZN8dActor_c10FindWithIDEj(*(int*)(self + 0x134));
    if (*(void**)(self + 0x3c0) == 0) return;

    {
        int t = (*(u16*)(*(char**)(self + 0x3c0) + 0xc) == 0xbf);
        if (t == false) return;
    }

    if ((*(int*)(self + 0x130) & 0x8000) != 0) return;
    if (_ZN6Player15IsCollectingCapEv(*(void**)(self + 0x3c0)) != 0) return;

    if (*(int*)(*(char**)(self + 0x3c0) + 8) == 3) {
        if (*(int*)(*(char**)(self + 0x3c0) + 0x360) != 0) return;
    }

    state = *(int*)(self + 0x3f0);
    if ((unsigned)(state - 6) <= 1) {
        _ZN6Player15InitVanishLuigiEv(*(void**)(self + 0x3c0));
        _ZN7fBase_c18MarkForDestructionEv(self);
        return;
    }
    if ((unsigned)(state - 8) <= 1) {
        _ZN6Player14InitMetalWarioEv(*(void**)(self + 0x3c0));
        _ZN7fBase_c18MarkForDestructionEv(self);
        return;
    }

    if (*(int*)(self + 0xc8) != 0) {
        if (state != 0) return;
    }

    if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
        func_ov002_020b7f2c((C*)self, (PMF*)&data_ov002_0210df54);
        *(u8*)(self + 0x401) = 1;
        return;
    }

    if (_ZN6Player17SetNoControlStateEhih(*(void**)(self + 0x3c0), 8, -1, 0) == 0) return;

    _ZN6Player18SetNewHatCharacterEjjb(
        *(void**)(self + 0x3c0), *(int*)(self + 0x3f4) & 0xff, 0, 0);

    {
        char* found = *(char**)(self + 0x3c0);
        int a = *(u16*)(found + 0x8c);
        int b = *(u16*)(found + 0x8e);
        a = b ? a : a;
        v.x = a;
        v.y = b;
        v.z = *(u16*)(found + 0x90);
        *(s16*)(self + 0x92) = *(s16*)&v.x;
        *(s16*)(self + 0x94) = *(s16*)&v.y;
        *(s16*)(self + 0x96) = *(s16*)&v.z;
        *(s16*)(self + 0x8c) = *(s16*)(self + 0x92);
        *(s16*)(self + 0x8e) = *(s16*)(self + 0x94);
        *(s16*)(self + 0x90) = *(s16*)(self + 0x96);
    }

    func_ov002_020b7f2c((C*)self, (PMF*)&data_ov002_0210df54);
}
