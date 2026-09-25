//cpp
/* daKrb_c -- Goomba (KURIBO 200 / KURIBO_S 201 / KURIBO_L 202), ov084.
 *
 * ov084 is mixed (BOB_OMB_BUDDY / GOOMBA / PIRANHA_PLANT). RTTI names this
 * class daKrb_c; the debug table names KURIBO / KURIBO_S / KURIBO_L. One
 * class, three C-linkage factories. Base is dCapEnemy_c.
 *
 * Factories are `return new daKrb_c()`. `#pragma defer_codegen off` is
 * load-bearing: out-of-line D1 then D0 then homeless D2 matches the
 * cartridge (deferred codegen emits D2, D0, D1).
 *
 * deslop leftovers:
 * - dCcAc_c::Init / dBgCh_Actr::Init / ModelAnim::SetAnim /
 *   MaterialChanger::SetFile / DropShadowRadHeight /
 *   KillByInvincibleChar / Player::Hurt / Bounce /
 *   IsTooFarAwayFromPlayer 6az: this TU passes Fix12<int> by value;
 *   the header method form size-DIFFs. dBgCh Init header Fix12i
 *   mangles as i; ROM is Fix12<int>. Callers: InitResources,
 *   func_ov084_02129ed4 / 0212a774 / 0212abd4 / 0212af74 / Render.
 * - dCapEnemy_c::UpdateCapPos stays mangled in the helpers
 *   (C-linkage offset soup). Named members already call AddCap /
 *   DestroyIfCapNotNeeded / GetCapState / RenderCapModel.
 * - Animation::Advance stays this+0x160 (named mModelAnim.Advance
 *   this-adjusts). Behavior / func_ov084_021298d0.
 * - The death helper uses receiver, attacker, and nullable collision pointers.
 * - func_ov084_* helpers stay offset soup. InitResources /
 *   OnTurnIntoEgg keep `char *c = (char *)this` (named fields CSE
 *   the +8 param word).
 * - data_ov084_02130cf8 BMD; 02130278[7] BCA table; 02130ce0/ce8/cf0
 *   /cc0/cc8/cd0 BCA handles (SetAnim reads [1]); 0213089c / 0213088c
 *   BMA. Init LoadFile/SetFile and Cleanup Release them.
 * - data_ov084_02130258 / 02130208 / 02130228 / 02130238 / 02130248 /
 *   02130268 / 02130204 per-type scale/height/speed/accel/hurt tables.
 *   S14: g_profile_KURIBO / _S / _L stay outside this text-only TU.
 * - `#pragma opt_common_subs off` on Render and `#pragma optimize_for_size
 *   on` are file-global last-wins; not carried (Render MATCHes without
 *   them; optimize_for_size recompiled 17 members).
 * - Flags / Flag / Obj / Sub / Vector3_16_local shadows are load-bearing
 *   (0212934c bitfield, OnTurnIntoEgg GetState, Render m5, UpdateCapPos).
 * - volatile Vector3 backup in Render (frame-slot).
 */

#pragma defer_codegen off

#include "daKrb_c.h"
#include "common.h"
#include "dBgCh_Gnd.h"
#include "types.h"
#include "dBgCh_Actr.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "decl_dCapEnemy_c.h"
#include "MaterialChanger.h"
#include "Player.h"

typedef struct {
    unsigned char b0 : 1;
    unsigned char flag : 1;
} Flags;

struct Vector3_16_local { unsigned short x, y, z; };

typedef struct { unsigned char b0 : 1; } Flag;

struct Obj {
    virtual int f00();
    virtual int f01();
    virtual int f02();
    virtual int f03();
    virtual int f04();
    virtual int f05();
    virtual int f06();
    virtual int f07();
    virtual int f08();
    virtual int f09();
    virtual int f10();
    virtual int f11();
    virtual int f12();
    virtual int f13();
    virtual int f14();
    virtual int f15();
    virtual int f16();
    virtual int f17();
    virtual int GetState();
};

struct Sub {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(Vector3* v);
};

struct BMD_File;
struct BMA_File;

#define I(p,o)    (*(s32*)((char*)(p)+(o)))
#define U16f(p,o) (*(u16*)((char*)(p)+(o)))
#define S16f(p,o) (*(s16*)((char*)(p)+(o)))
#define U8f(p,o)  (*(u8*)((char*)(p)+(o)))
/* SharedFilePtr.h declares no fields; the loaded pointer is the second word. */
#define SHARED_FILE(h) (((void **)&(h))[1])

extern "C" {
/* data_ov084_02130cf8 is the BMD this TU LoadFile / Release. The other
   three handles are never passed to a SharedFilePtr method; this TU
   indexes [1] as the loaded BCA. */
extern void *data_ov084_02130ce0[];
extern void *data_ov084_02130ce8[];
extern void *data_ov084_02130cf0[];
extern SharedFilePtr data_ov084_02130cf8;
int func_02037e20(int* p);
void func_ov084_02129498(char* r0);
extern "C" void func_ov084_02129238(char* c);
extern void func_02012694(unsigned int id, const Vector3 *pos);
extern char* _ZNK10dBgCh_Actr14GetFloorResultEv(void*);
extern int SurfaceInfo_TestFlag0x20(int* p);
extern void func_ov084_021296cc(char *c);
extern int func_02037e38(unsigned int* p);
extern int func_02037e84(int* p);
extern void _ZN5dBgPiD1Ev(void*);
extern int data_02099368[];
extern void LinkSilverStarAndStarMarker(char *a, char *b);
extern u8 data_0209f208[];
extern u8 *data_0209f344;
extern void func_ov084_02129168(char *c, char *actor);
extern void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(void *m, void *f, int a, int fix, unsigned int j);
/* Three-register reconstructed interface; death-state stores also use r3. */
extern void func_ov002_020aea30(void *self, void *actor, void *collision);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int a, int fix, unsigned int j);
extern void func_ov084_0212a580(char *self);
extern void func_ov084_021294d0(char *self);
extern int *data_ov084_0213088c;
extern int data_ov084_02130248[];
extern Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void* thiz, s16* v, void* r6, s32 flag);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, s32 f);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, s32 f, u8 b, u8 cc, u8 d);
extern void* data_ov084_02130cd0[];
extern u8 data_ov084_02130204[];
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int fix, int t, unsigned int j);
extern short data_02082214[];
extern "C" void _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16(void *, const Vector3&, const Vector3_16_local&);
extern int _ZN4cstd4fdivEii(int a, int b);
extern char data_ov084_0213089c;
extern void func_ov084_02129c9c(char *c);
extern void func_ov084_02129cf4(char *self, int a);
extern int _Z14ApproachLinearRiii(int *a, int b, int c);
extern int _Z14ApproachLinearRsss(short *a, short b, short c);
extern int RandomIntInternal(int *seed);
extern int data_ov084_02130228[];
extern int data_ov084_02130268[];
extern int data_0209e650;
extern void func_ov074_0212087c(Vector3 *out, void *player, u8 flag);
extern int ApproachAngle(s16 *cur, s16 target, int divisor, int band, int maxStep);
extern int Vec3_HorzDist(void *a, void *b);
extern unsigned char DecIfAbove0_Byte(void *p);
extern unsigned short DecIfAbove0_Short(void *p);
extern int Math_Function_0203b14c(int *v, int target, int a, int b, int c);
extern int data_ov084_02130cc8[];
extern void func_ov084_0212af74(char *c);
extern void func_ov084_0212abd4(char *c);
extern void UnloadBlueCoinModel(void* p);
extern s8 data_0209f2f8;
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(char* c, int f);
void LoadSilverStarAndNumber(void);
void LoadBlueCoinModel(void* c);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* a, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* a, Fix12i b, Fix12i cc, void* d, Fix12i e);
void func_ov084_021290d4(char *c);
extern SharedFilePtr* data_ov084_02130278[7];
extern int data_ov084_02130258[];
extern int data_ov084_02130208[];
extern int data_ov084_02130238[];
}

// @symbol _ZN7daKrb_cD1Ev
// @symbol _ZN7daKrb_cD0Ev
daKrb_c::~daKrb_c()
{
}

// @symbol func_ov084_021290d4
extern "C" {
inline char *inline_fn(char *arg0)
{
  return arg0 + 0x448;
}

void func_ov084_021290d4(char *c)
{
  int new_var;
  char *new_var2;
  char *base = c + 0x400;
  int new_var3;
  new_var = 0x458;
  if ((*((unsigned short *) (base + 0x58))) != 0)
  {
    /* Comparison stays in the `if`: assigning `== 1` to an int is a bool
       in C++ and materialises moveq/movne (S8). */
    new_var3 = (int)(*((int *) (c + 0x434)));
    if (new_var3 == 1)
    {
      *((unsigned short *) ((c + 0x400) + 0x58)) = 0;
    }
    else
    {
      *((unsigned short *) ((((int) c) + new_var))) = (*((unsigned short *) ((((int) c) + 0x458)))) - 1;
    }
    return;
  }
  if ((*((unsigned char *) ((new_var2 = c) + 0x113))) < 6)
  {
    *((int *) inline_fn(new_var2)) = 0x1f4000;
    return;
  }
  unsigned short v = *((unsigned short *) ((c + 0x400) + 0x56));
  if (v > 0xa)
  {
    *((int *) inline_fn(new_var2)) = 0x1f4000 - (((v - 0xa) * 0x14) << 12);
    if ((*((int *) inline_fn(new_var2))) < 0xa000)
    {
      *((int *) inline_fn(new_var2)) = 0xa000;
    }
    return;
  }
  *((int *) inline_fn(new_var2)) = 0x1f4000;
}
}

// @symbol func_ov084_02129168
#include "decl_dBgCh_Actr.h"
extern "C" {

extern int _ZN4cstd4fdivEii(int a, int b);
extern int Vec3_HorzLen(void* v);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int b, unsigned int e);
extern void func_02012694(unsigned int id, const Vector3 *pos);

void func_ov084_02129168(char* c, char* actor)
{
    *(short*)(c + 0x452) = 0x3c;
    *(int*)(c + 0xa8) = _ZN4cstd4fdivEii(0xd000 - *(int*)(c + 0x9c), *(int*)(c + 0xd8));
    *(int*)(c + 0x98) = Vec3_HorzLen(c + 0xa4) * -1;
    if (actor != 0)
        *(short*)(c + 0x94) = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(actor + 0x5c));
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, *(void**)((char*)data_ov084_02130cc0 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x434) = 3;
    *(unsigned char*)(c + 0x107) = 0;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(c + 0x1b4);
    _ZN10dBgCh_Actr12Unk_0203589cEv(c + 0x1b4);
    _ZN10dBgCh_Actr22ClearJustHitGroundFlagEv(c + 0x1b4);
    ((dBgCh_Actr *)(c + 0x1b4))->ClearGroundFlag();
    func_02012694(0x13a, (const ::Vector3 *)(c + 0x74));
    *(unsigned char*)(c + 0x467) = 0;
}
}

// @symbol func_ov084_02129238
void func_ov084_02129238(char* c)
{
    if (((dBgCh_Actr *)(c + 0x1b4))->IsOnGround() != 0) return;
    {
        Vector3 pos;
        {
            int vx = *(int*)(c + 0x5c);
            int vz = *(int*)(c + 0x64);
            int vy = *(int*)(c + 0x60) + 0x190000;
            pos.x = vx;
            pos.y = vy;
            pos.z = vz;
        }
        dBgCh_Gnd rg;
        rg.StartDetectingWater();
        rg.StartDetectingToxic();
        rg.StopDetectingOrdinary();
        rg.SetObjAndPos(pos, (dActor_c*)c);
        if (rg.DetectClsn() != 0) {
            if (func_02037e20((int*)&rg.surface) != 0) {
                if (rg.clsnY != (int)0x80000000) {
                    if (*(int*)(c + 0x60) < rg.clsnY) {
                        ((dEnemyBase_c *)c)->SpawnCoin();
                        ((dActor_c *)c)->PoofDust();
                        func_ov084_02129498((char*)c);
                        {
                            Vector3 cap;
                            cap.x = 0;
                            cap.y = 0x6c000;
                            cap.z = 0;
                            ((dCapEnemy_c *)c)->ReleaseCap(cap);
                        }
                        *(int*)(c + 0x5c) = *(int*)(c + 0x41c);
                        *(int*)(c + 0x60) = *(int*)(c + 0x420);
                        *(int*)(c + 0x64) = *(int*)(c + 0x424);
                        ((dCapEnemy_c *)c)->RespawnIfHasCap();
                    }
                }
            }
        }
    }
}

// @symbol func_ov084_0212934c
extern "C" {
void func_ov084_0212934c(char* c)
{
    unsigned int kind;
    int v;
    int type;
    unsigned char *fp;

    if (*(int*)(c + 0x434) != 0)
        return;

    if (!((dBgCh_Actr *)(c + 0x1b4))->IsOnGround())
        return;

    /* kind before type load: forces v=r3 / type1=r1 coloring (short extract form) */
    v = *(int*)(c + 0x3c8);
    kind = (unsigned short)((unsigned)v >> 12);
    type = *(int*)(c + 0x3d0);

    if (type == (int)data_ov084_02130ce8[1]) {
        if (kind <= 4 || (kind >= 0xc && kind <= 0x10)) {
            if (((Flags*)(c + 0x468))->flag)
                return;
            func_02012694(0xd0, (const ::Vector3 *)(c + 0x74));
            fp = (unsigned char *)(((int)c + 0x468));
            *fp |= 2;
            return;
        }
        fp = (unsigned char *)(((int)c + 0x468));
        *fp &= ~2;
        return;
    }

    if (type == (int)data_ov084_02130cf0[1]) {
        if (kind <= 3 || (kind >= 0x10 && kind <= 0x13)) {
            if (((Flags*)(c + 0x468))->flag)
                return;
            func_02012694(0xd0, (const ::Vector3 *)(c + 0x74));
            fp = (unsigned char *)(((int)c + 0x468));
            *fp |= 2;
            return;
        }
        fp = (unsigned char *)(((int)c + 0x468));
        *fp &= ~2;
    }
}
}

// @symbol func_ov084_02129498
extern "C" {
void func_ov084_02129498(char* r0) {
  if ((*(unsigned char*)(r0 + 0x113) & 0xf) < 6)
    ((fBase_c *)r0)->MarkForDestruction();
  else
    ((dActor_c *)r0)->KillAndTrackInDeathTable();
}
}

// @symbol func_ov084_021294d0
extern "C" void func_ov084_021294d0(char* c)
{
    char obj[0x28];
    if (!((dBgCh_Actr *)(c + 0x1b4))->IsOnGround())
        return;

    char* fr = _ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x1b4);
    if (SurfaceInfo_TestFlag0x20((int*)(fr + 4))) {
        func_ov084_021296cc(c);
        ((dEnemyBase_c *)c)->SpawnCoin();
        func_ov084_02129498(c);
        Vector3 v;
        v.x = 0; v.y = 0x6c000; v.z = 0;
        ((dCapEnemy_c *)c)->ReleaseCap(v);
        *(int*)(c + 0x5c) = *(int*)(c + 0x41c);
        *(int*)(c + 0x60) = *(int*)(c + 0x420);
        *(int*)(c + 0x64) = *(int*)(c + 0x424);
        ((dCapEnemy_c *)c)->RespawnIfHasCap();
        return;
    }

    /* Copy the floor result into obj's own surface record, then ask what kind of
     * ground it is. The Goomba dies on some terrain types and acts on others. */
    char* floorResult = _ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x1b4);
    int surfaceType;
    {
    char* surface = obj + 4;
    int normalX = *(int*)(floorResult + 4);
    int normalY = *(int*)(floorResult + 8);
    /* `normalY ? normalX : normalX` is not a typo and not dead: both arms are the
     * same value, and the ternary is what makes mwccarm materialize normalX after
     * the load of normalY instead of before it. Collapsing it to a plain store
     * reorders the pair and the function stops reproducing. */
    *(int*)(surface) = normalY ? normalX : normalX;
    *(int*)(surface + 4) = normalY;
    *(int*)(surface + 8) = *(int*)(floorResult + 0xc);
    *(int*)(surface + 0xc) = *(int*)(floorResult + 0x10);
    *(int*)(surface + 0x10) = *(int*)(floorResult + 0x14);
    *(int*)(obj) = (int)data_02099368;
    *(unsigned short*)(obj + 0x18) = *(unsigned short*)(floorResult + 0x18);
    *(unsigned short*)(obj + 0x1a) = *(unsigned short*)(floorResult + 0x1a);
    *(int*)(obj + 0x1c) = *(int*)(floorResult + 0x1c);
    *(int*)(obj + 0x20) = *(int*)(floorResult + 0x20);
    *(int*)(obj + 0x24) = *(int*)(floorResult + 0x24);
    surfaceType = func_02037e38((unsigned int*)surface);
    }
    if (func_02037e84((int*)(obj + 4)) == 8) {
        if (surfaceType == 6 || surfaceType == 7 || surfaceType == 8 || surfaceType == 9)
            goto action;
    }
    if (surfaceType == 0x13 || surfaceType == 1)
        goto action;
    if ((unsigned)(surfaceType - 4) > 1)
        goto dtor;
action:
    ((dEnemyBase_c *)c)->SpawnCoin();
    func_ov084_02129498(c);
    if ((*(unsigned char*)(c + 0x113) & 0xf) < 6 ||
        *(unsigned char*)(c + 0x464) == 2) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x41c);
        *(int*)(c + 0x60) = *(int*)(c + 0x420);
        *(int*)(c + 0x64) = *(int*)(c + 0x424);
        Vector3 v2;
        v2.x = 0; v2.y = 0x6c000; v2.z = 0;
        ((dCapEnemy_c *)c)->ReleaseCap(v2);
        ((dCapEnemy_c *)c)->RespawnIfHasCap();
    }
dtor:
    _ZN5dBgPiD1Ev(obj);
}

// @symbol func_ov084_021296b0
extern "C" {
void func_ov084_021296b0(int *a, int *b)
{
    a[263] = b[0]; a[264] = b[1]; a[265] = b[2];
}
}

// @symbol func_ov084_021296cc
extern "C" {
void func_ov084_021296cc(char *c)
{
    if (*(u8 *)(c + 0x464) == 1) {
        char *a;
        char *b;
        ((dActor_c *)c)->UntrackStar(*(s8 *)(c + 0x465));
        a = (char *)dActor_c::Spawn(
            0xb4, 0x50, *(Vector3 *)(c + 0x41c), 0,
            *(signed char *)(c + 0xcc), -1);
        b = (char *)dActor_c::Spawn(
            0xb3, 0x10, *(Vector3 *)(c + 0x5c), 0,
            *(signed char *)(c + 0xcc), -1);
        if (a != 0 && b != 0) {
            *(int *)(b + 0x434) = *(int *)(a + 4);
            LinkSilverStarAndStarMarker(a, b);
            ((dActor_c *)c)->SpawnSoundObj(1);
        }
        *(int *)(c + 8) = *(const int *)((const char *)c + 8) & 0xff0f;
        return;
    }
    if (*(u8 *)(c + 0x464) != 2)
        return;
    if (*(u8 *)(c + 0x466) != data_0209f344[data_0209f208[0]])
        return;
    ((dActor_c *)c)->UntrackStar(*(s8 *)(c + 0x465));
    dActor_c::Spawn(
        0xb4, *(u8 *)(c + 0x466) | 0x30, *(Vector3 *)(c + 0x5c), 0,
        *(signed char *)(c + 0xcc), -1);
    dActor_c::Spawn(
        0xb3, *(u8 *)(c + 0x466) | 0x30, *(Vector3 *)(c + 0x5c), 0,
        *(signed char *)(c + 0xcc), -1);
    *(u8 *)(c + 0x464) = 3;
    *(int *)(c + 8) = *(const int *)((const char *)c + 8) & 0xff0f;
    ((dActor_c *)c)->SpawnSoundObj(1);
}
}

// @symbol func_ov084_02129864
extern "C" {
void func_ov084_02129864(char *c){
    if(*(unsigned char*)(c+0x464)!=2) return;
    if(*(signed char*)(c+0x465)>=0) return;
    unsigned int b3=*(unsigned char*)data_0209f208;
    char *p=(char*)data_0209f344;
    unsigned int m=*(unsigned char*)(p + b3);
    unsigned int v=*(unsigned char*)(c+0x466);
    if(v!=m) return;
    *(unsigned char*)(c+0x465)=(unsigned char)((dActor_c *)c)->TrackStar(v,1);
}
}

// @symbol func_ov084_021298d0
extern "C" {
void func_ov084_02129498(char* r0);
void func_02012694(unsigned int id, const Vector3 *pos);
extern int data_ov084_02130218[];

int func_ov084_021298d0(char* c){
    int deathState = ((dEnemyBase_c *)c)->UpdateDeath(*(dBgCh_Actr *)(c + 0x1b4));
    if ((unsigned int)(*(int*)(c + 0x10c) - 2) > 4) goto L_a4;
    *(int*)(c + 0x3cc) = 0x1000;
    ((Animation *)(c + 0x3c0))->Advance();
    if (*(int*)(c + 0x460) != 3) goto L_a4;

    /* SpawnCoin only when linked actor is type 0xc6; flag+cylinder always when state==3 */
    unsigned int id = *(unsigned int*)(c + 0x1a4);
    if (id != 0) {
        char* r = (char*)dActor_c::FindWithID(id);
        if (r != 0) {
            int b = (*(unsigned short*)(r + 0xc) == 0xc6);
            if (b != 0) {
                ((dEnemyBase_c *)c)->SpawnCoin();
                func_ov084_02129498(c);
            }
        }
    }
    /* cast launder: force add r2,r5,#0x198 materialization (sibling ov084 idiom) */
    *(int*)(((int)c + 0x198)) |= 0x20000;
    ((dCc_c *)(c + 0x180))->Clear();
    ((dCc_c *)(c + 0x180))->Update();

L_a4:
    if (deathState == 0) goto L_end;
    func_02012694(data_ov084_02130218[*(int*)(c + 0x460)], (const ::Vector3 *)(c + 0x74));
    func_ov084_021296cc(c);
    /* ROM: copy+respawn when (deathPhase < 6) OR (byte_464 == 2) */
    if ((*(unsigned char*)(c + 0x113) & 0xf) < 6 || *(unsigned char*)(c + 0x464) == 2) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x41c);
        *(int*)(c + 0x60) = *(int*)(c + 0x420);
        *(int*)(c + 0x64) = *(int*)(c + 0x424);
        ((dCapEnemy_c *)c)->RespawnIfHasCap();
    }
    if ((*(unsigned char*)(c + 0x113) & 0xf) < 6) {
        ((dActor_c *)c)->UntrackInDeathTable();
    }
L_end:
    return deathState;
}
}

// @symbol func_ov084_02129a00
extern "C" {
int func_ov084_02129a00(char *self) {
    int eatState = ((dEnemyBase_c *)self)->UpdateYoshiEat(*(dBgCh_Actr *)(self + 0x1b4));
    if (eatState == 0)
        goto ret0;
    if (eatState == 1) {
        Vector3 v;
        char *actor = *(char **)(self + 0xd0);
        v.x = 0;
        v.y = 0x6c000;
        v.z = 0;
        if (((dCapEnemy_c *)self)->GetCapEatenOffIt(v) != 0) {
            func_ov084_02129168(self, actor);
            *(int *)(self + 0x98) = -0xf000;
            *(int *)(self + 0xa8) = 0x14000;
            MaterialChanger::Prepare(*(BMD_File *)(SHARED_FILE(data_ov084_02130cf8)), *(BMA_File *)&data_ov084_0213088c);
            _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(self + 0x3fc, &data_ov084_0213088c, 0x40000000, 0x1000, 0);
            *(int *)(self + 0x404) = 0;
            ((dCc_c *)(self + 0x180))->Clear();
            return 0;
        }
    } else if (eatState == 3) {
        if (((dBgCh_Actr *)(self + 0x1b4))->IsOnGround())
            *(int *)(((int)self + 0x98)) >>= 1;
    }

    if (((dEnemyBase_c *)self)->SpawnParticlesIfHitOtherObj(*(dCc_c *)(self + 0x180)) != 0) {
        void *actor = dActor_c::FindWithID(*(int *)(self + 0x1a4));
        *(int *)(self + 0x10c) = 7;
        func_ov002_020aea30(self, actor, self + 0x1b4);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
        *(int *)(((int)self + 0x198)) |= 1;
        return 1;
    }

    func_ov084_0212a580(self);
    ((dCc_c *)(self + 0x180))->Clear();
    if (*(u8 *)(self + 0x107) != 0) {
        func_ov084_021294d0(self);
        {
            u16 s = *(u16 *)(self + 0x104);
            if (s == 0) {
                ((dCc_c *)(self + 0x180))->Update();
            } else if (s == 5) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
                *(s16 *)(((int)self + 0x94)) += 0x8000;
                *(int *)(self + 0x98) = -*(int *)(self + 0x98);
                MaterialChanger::Prepare(*(BMD_File *)(SHARED_FILE(data_ov084_02130cf8)), *(BMA_File *)&data_ov084_0213088c);
                _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(self + 0x3fc, &data_ov084_0213088c, 0x40000000, 0x1000, 0);
                *(int *)(self + 0x404) = 0;
            }
        }
        ((Animation *)(self + 0x3c0))->Advance();
        if (((dBgCh_Actr *)(self + 0x1b4))->JustHitGround())
            func_ov084_02129168(self, 0);
    }

    if (*(int *)(self + 0x460) != 3)
        goto ret1;
    if (eatState < 3)
        goto ret1;
    if (*(int *)(self + 0x60) >= *(int *)(self + 0x420) - 0x3e8000)
        goto ret1;
    ((fBase_c *)self)->MarkForDestruction();
    return 1;
ret1:
    return 1;
ret0:
    return 0;
}
}

// @symbol func_ov084_02129c9c
extern "C" {
void func_ov084_02129c9c(char *c)
{
  char *new_var;
  func_02012694(0x118, (const ::Vector3 *)(c + 0x74));
  *((int *) (c + 0x434)) = 2;
  *((int *) (c + 0x98)) = 0;
  new_var = c;
  *((int *) (new_var + 0xa8)) = data_ov084_02130248[*((int *) (new_var + 0x460))];
  ((dBgCh_Actr *)(new_var + 0x1b4))->ClearGroundFlag();
  *((int *) ((char *)(((int)(new_var + 0x198)) + 0))) |= 4;
}
}

// @symbol func_ov084_02129cf4
extern "C" {
void func_ov084_02129cf4(char *c, Fix12i distThresh)
{
    struct Vector3 ppos;

    *(void **)(c + 0x438) = ((dActor_c *)c)->ClosestPlayer();

    if (*(void **)(c + 0x438) == 0
        || (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c)) > distThresh
            && *(unsigned char*)(c+0x113) >= 6)) {
        *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c));
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }

    {
        int *ppos_src = (int *)(int)(*(char **)(c + 0x438) + 0x5c);
        ppos.x = ppos_src[0];
        ppos.y = ppos_src[1];
        ppos.z = ppos_src[2];
    }

    if (*(unsigned char*)(c+0x113) < 6) {
        if (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c)) > distThresh
            && !((dBgCh_Actr *)(c+0x1b4))->IsOnWall()) {
            *(int*)(c+0x440) = 0x61a8000;
            *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c));
            return;
        }

        if (Vec3_Dist((struct Vector3*)(c+0x5c), &ppos) < *(int*)(c+0x448)) {
            *(int*)(c+0x440) = Vec3_Dist((struct Vector3*)(c+0x5c), &ppos);
            if (*(unsigned short*)(c+0x400+0x58) != 0) {
                *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), &ppos);
                return;
            }
            *(short*)(c+0x400+0x5a) = Vec3_HorzAngle(&ppos, (struct Vector3*)(c+0x5c));
            return;
        }
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }

    if (Vec3_Dist((struct Vector3*)(c+0x41c), &ppos) > distThresh) {
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }
    *(int*)(c+0x440) = Vec3_Dist((struct Vector3*)(c+0x5c), &ppos);
    *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), &ppos);
}
}

// @symbol func_ov084_02129ed4
extern "C" {
/* Collision reaction: something touched this Goomba, decide what it did.
 *
 * `flags` is the collision word at 0x1a0 and `other` the actor found through the
 * hit id at 0x1a4. Type ids are the debug-table profile numbers this class is
 * registered under -- 0xc8 KURIBO, 0xc9 KURIBO_S, 0xca KURIBO_L, and 0xbf for a
 * Player -- so `myType` is which of the three Goomba sizes WE are, not what hit
 * us. The outcome is either death (cap released, then KillByInvincibleChar with
 * a knockback direction), a stomp (Player::Bounce), or hurting the player.
 *
 * `variantMatch` and `typeMatch` are scratch booleans, and materializing them
 * instead of testing inline is deliberate: mwccarm emits the comparison into a
 * register and then re-tests it, which is the shape the cartridge has. Folding
 * either back into its `if` collapses the pair. See notes/matching-style.md 3. */
void func_ov084_02129ed4(void* c)
{
    s16 killDirNormal[3];
    s16 killDirVariant[3];
    s16 killDirPlayer[3];
    volatile Vector3 playerPos;
    volatile Vector3 playerPosJump;
    Vector3 capReleaseOnHit;
    Vector3 capReleaseOnKill;
    Vector3 hurtOriginFirstHit;
    Vector3 hurtOriginRepeat;
    Vector3 hurtOriginJumped;
    Vector3 capReleaseOnExit;
    void* other;
    u32 flags;
    s32 turnAround;
    s32 hurtKnockback;
    u16 myType;
    s32 variantMatch;
    s32 typeMatch;
    u32 id;

    id = *(u32*)((char*)c + 0x1a4);
    if (id == 0) return;
    other = dActor_c::FindWithID(id);
    if (other == 0) return;

    flags = I(c, 0x1a0);
    hurtKnockback = 0xc000;
    I(c, 0x46c) = flags;
    myType = U16f(c, 0xc);
    turnAround = 0;
    if (I(c, 0x460) == 3) hurtKnockback = 0x5000;
    variantMatch = (s32)(myType == 0xc9);

    if (variantMatch == 0 && (flags & 0x10)) {
        capReleaseOnHit.x = 0; capReleaseOnHit.y = 0x6c000; capReleaseOnHit.z = 0;
        ((dCapEnemy_c *)c)->ReleaseCap(capReleaseOnHit);
        typeMatch = (s32)(U16f(c, 0xc) == 0xc8);
        if (typeMatch != 0) {
            killDirNormal[0] = -0x2000; killDirNormal[1] = 0; killDirNormal[2] = 0;
            _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, killDirNormal, other, 0x41000);
            return;
        }
        killDirVariant[0] = -0x1800; killDirVariant[1] = 0; killDirVariant[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, killDirVariant, other, 0x96000);
        return;
    }

    if (flags & 0x20) {
        I(c, 0x10c) = 1;
        if (I(c, 0x460) == 2) U8f(c, 0x108) = 3;
        I(c, 0x80) = 0x1000;
        I(c, 0x84) = 0x1000;
        I(c, 0x88) = 0x1000;
        func_02012694(0xe0, (const ::Vector3 *)((char*)c + 0x74));
        goto block_68;
    }

    if (flags & 0x40000) {
        turnAround = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130cd0[1], 0x40000000, 0x1000, 0);
        I(c, 0x10c) = 4;
        goto block_68;
    }

    variantMatch = (s32)(myType == 0xca);
    if (variantMatch == 0) {
        if (flags & 0x20000) {
            I(c, 0x10c) = 7;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
            *(s32*)(((int)c + 0x198) & 0xffffffffffffffffULL) |= 1;
            goto block_68;
        }
        if (flags & 0x2400) {
            I(c, 0x10c) = 5;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
            goto block_68;
        }
        if (flags & 0x4000) {
            I(c, 0x10c) = 6;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130cd0[1], 0x40000000, 0x1000, 0);
            goto block_68;
        }
        if (flags & 0x380) {
            turnAround = 1;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130cd0[1], 0x40000000, 0x1000, 0);
            I(c, 0x10c) = 3;
            goto block_68;
        }
        if (flags & 0x40) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
            I(c, 0x10c) = 2;
            turnAround = 1;
            goto block_68;
        }
        if (!(flags & 0x8000)) {
            typeMatch = (s32)(U16f(other, 0xc) == 0xbf);
            if (typeMatch != 0) {
                if (U8f(other, 0x6f9) != 0) {
                    capReleaseOnKill.x = 0; capReleaseOnKill.y = 0x6c000; capReleaseOnKill.z = 0;
                    ((dCapEnemy_c *)c)->ReleaseCap(capReleaseOnKill);
                    killDirPlayer[0] = 0x2000; killDirPlayer[1] = 0; killDirPlayer[2] = 0;
                    _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, killDirPlayer, other, 0x41000);
                    return;
                }
                { Vector3* pp = (Vector3*)(((int)other + 0x5c) & 0xffffffffffffffffULL); playerPos.x = pp->x; playerPos.y = pp->y; playerPos.z = pp->z; }
                if (((Player *)other)->IsOnShell() != 0) {
                    I(c, 0x10c) = 5;
                    turnAround = 1;
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
                    goto block_68;
                }
                if (((dActor_c *)c)->JumpedOnByPlayer(*(dCc_c *)((char*)c + 0x180), *(Player *)other) != 0) {
                    _ZN6Player6BounceE5Fix12IiE(other, 0x28000);
                    func_02012694(0xe0, (const ::Vector3 *)((char*)c + 0x74));
                    I(c, 0x10c) = 1;
                    I(c, 0x80) = 0x1000;
                    I(c, 0x84) = 0x1000;
                    I(c, 0x88) = 0x1000;
                    goto block_68;
                }
                if (U8f(other, 0x6fb) != 0) return;
                if (I(c, 0x434) == 0) {
                    if (I(c, 0x460) == 0) {
                        ((dActor_c *)c)->SmallPoofDust();
                        hurtOriginFirstHit.x = I(c, 0x5c); hurtOriginFirstHit.y = I(c, 0x60); hurtOriginFirstHit.z = I(c, 0x64);
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &hurtOriginFirstHit, 0, hurtKnockback, 1, 0, 1);
                        func_ov084_02129498((char*)c);
                        func_02012694(0x110, (const ::Vector3 *)((char*)c + 0x74));
                        return;
                    }
                    if ((I(c, 0x1a0) & 0x400000) == 0) return;
                    hurtOriginRepeat.x = I(c, 0x5c); hurtOriginRepeat.y = I(c, 0x60); hurtOriginRepeat.z = I(c, 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &hurtOriginRepeat, data_ov084_02130204[I(c, 0x460)], hurtKnockback, 1, 0, 1);
                    I(c, 0x434) = 1;
                    return;
                }
                goto block_68;
            }
            goto block_68;
        }
        goto block_68;
    }

    typeMatch = (s32)(U16f(other, 0xc) == 0xbf);
    if (typeMatch != 0) {
        { Vector3* pp = (Vector3*)(((int)other + 0x5c) & 0xffffffffffffffffULL); playerPosJump.x = pp->x; playerPosJump.y = pp->y; playerPosJump.z = pp->z; }
        if (((dActor_c *)c)->JumpedOnByPlayer(*(dCc_c *)((char*)c + 0x180), *(Player *)other) != 0) {
            _ZN6Player6BounceE5Fix12IiE(other, 0x28000);
            func_02012694(0xe0, (const ::Vector3 *)((char*)c + 0x74));
            I(c, 0x10c) = 1;
            I(c, 0x80) = 0x1000;
            I(c, 0x84) = 0x1000;
            I(c, 0x88) = 0x1000;
            goto block_68;
        }
        if (U8f(other, 0x6fb) != 0) return;
        if (I(c, 0x434) == 0) {
            I(c, 0x434) = 1;
            if ((I(c, 0x1a0) & 0x400000) == 0) return;
            hurtOriginJumped.x = I(c, 0x5c); hurtOriginJumped.y = I(c, 0x60); hurtOriginJumped.z = I(c, 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &hurtOriginJumped, data_ov084_02130204[I(c, 0x460)], hurtKnockback, 1, 0, 1);
            return;
        }
        goto block_68;
    }

block_68:
    if (I(c, 0x10c) != 0) {
        capReleaseOnExit.x = 0; capReleaseOnExit.y = 0x6c000; capReleaseOnExit.z = 0;
        ((dCapEnemy_c *)c)->ReleaseCap(capReleaseOnExit);
    }
    func_ov002_020aea30(c, other, (char*)c + 0x1b4);
    if (turnAround != 0) {
        S16f(c, 0x8e) = (s16)(S16f(c, 0x94) + 0x8000);
    }
    if (I(c, 0x460) != 3) return;
    if ((I(c, 0x46c) & 0x40) || (I(c, 0x46c) & 0x380)) {
        *(s32*)(((int)c + 0x98) & 0xffffffffffffffffULL) += I(other, 0x98);
    }
    if (I(c, 0x46c) & 0x400) {
        *(s32*)(((int)c + 0x98) & 0xffffffffffffffffULL) += 0x20;
    }
}
}

// @symbol func_ov084_0212a580
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void func_ov084_0212a580(char* c){
    Vector3_16_local rotation;
    Vector3 pos;
    Vector3 arg;
    Vector3_16_local arg16;

    Matrix4x3_FromRotationY(c + 0x38c, *(short*)(c + 0x8e));
    *(int*)(c + 0x3b0) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3b4) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x3b8) = *(int*)(c + 0x64) >> 3;
    rotation.x = *(short*)(c + 0x8c);
    rotation.y = *(short*)(c + 0x8e);
    rotation.z = *(short*)(c + 0x90);
    if ((*(int*)(c + 0xb0) & 0x40000 ? 1 : 0) == 0) {
        if (((dBgCh_Actr *)(c + 0x1b4))->IsOnGround()) {
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x3d4, c + 0x38c, *(int*)(c + 0x80) * 0x50, 0x1e000, 0xf);
        } else {
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x3d4, c + 0x38c, *(int*)(c + 0x80) * 0x50, 0x96000, 0xf);
        }
    }
    pos.x = 0;
    pos.z = 0;
    pos.y = 0x6c000;
    pos.x += ((short*)data_02082214)[(*(unsigned short*)(c + 0x8e) >> 4) * 2] * 10;
    pos.z += ((short*)data_02082214)[(*(unsigned short*)(c + 0x8e) >> 4) * 2 + 1] * 10;
    arg.x = ((int*)&pos)[0];
    arg.y = ((int*)&pos)[1];
    arg.z = ((int*)&pos)[2];
    arg16.x = ((unsigned short*)&rotation)[0];
    arg16.y = ((unsigned short*)&rotation)[1];
    arg16.z = ((unsigned short*)&rotation)[2];
    /* equal-arm ternary forces arg16 setup (r2) before arg (r1) — matches ROM call-arg order */
    _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16((dCapEnemy_c*)c, arg, c ? arg16 : arg16);
}

// @symbol func_ov084_0212a6f8
extern "C" {
void func_ov084_0212a6f8(char *c)
{
    *(int *)(c + 0x98) = 0;
    if (*(unsigned short *)(c + 0x400 + 0x54)) {
        *(unsigned short *)(((int)c + 0x454)) -= 1;
    }
    if (*(unsigned short *)(c + 0x400 + 0x54))
        return;
    *(int *)(c + 0x434) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
}
}

// @symbol func_ov084_0212a774
extern "C" {
void func_ov084_0212a774(char *c)
{
    Vector3 v;
    u16 h = *(u16 *)(c + 0x400 + 0x52);

    if (h == 0) {
        ((Animation *)(c + 0x3fc))->Advance();
        if (((Animation *)(c + 0x3c0))->Finished() == 0)
            return;
        *(s32 *)(c + 0xb0) = *(s32 *)(c + 0x44c);
        *(s32 *)(c + 0x434) = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
        *(int *)(c + 0x444) = data_ov084_02130228[*(s32 *)(c + 0x460)];
        ((dBgCh_Actr *)(c + 0x1b4))->ClearLimMovFlag();
        {
            s32 *f198 = (s32 *)(((long long)(int)(c + 0x198)));
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
            *f198 = *f198 & ~0x20000;
        }
        MaterialChanger::Prepare(*(BMD_File *)(SHARED_FILE(data_ov084_02130cf8)), *(BMA_File *)&data_ov084_0213089c);
        _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(c + 0x3fc, &data_ov084_0213089c, 0x40000000, 0x1000, 0);
        *(s32 *)(c + 0x404) = 0;
        return;
    }
    if (h <= 0x3c) {
        *(u16 *)(((long long)(int)(c + 0x452))) -= 1;
        if (*(u16 *)(c + 0x400 + 0x52) == 0) {
            func_ov084_021296cc(c);
            ((dEnemyBase_c *)c)->SpawnCoin();
            func_ov084_02129498(c);
            v.x = 0;
            v.y = 0x6c000;
            v.z = 0;
            ((dCapEnemy_c *)c)->ReleaseCap(v);
            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x41c);
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x420);
            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x424);
            ((dCapEnemy_c *)c)->RespawnIfHasCap();
        }
    }
    if (((dBgCh_Actr *)(c + 0x1b4))->JustHitGround() != 0) {
        int a8;
        int cnt;
        if (*(u16 *)(c + 0x452) > 0x3c)
            *(u16 *)(c + 0x452) = 0x1e;
        a8 = *(s32 *)(c + 0xa8);
        cnt = *(u16 *)(c + 0x452);
        {
            int aa = a8 < 0 ? -a8 : a8;
            if (aa <= cnt * 0x500) {
                *(s32 *)(c + 0xa8) = (cnt << 0xa) - *(s32 *)(c + 0x9c);
            } else {
                *(s32 *)(c + 0xa8) = _ZN4cstd4fdivEii((a8 * -0x50) / 100, *(s32 *)(c + 0xd8));
            }
        }
        {
            s32 *p98 = (s32 *)(((long long)(int)(c + 0x98)));
            *p98 >>= 1;
        }
        {
            int v98 = *(s32 *)(c + 0x98);
            int av = v98 < 0 ? -v98 : v98;
            if (av < 0x5000) {
                if (v98 < 0)
                    *(s32 *)(c + 0x98) = -0x5000;
                else
                    *(s32 *)(c + 0x98) = 0x5000;
            }
        }

        if (*(u8 *)(c + 0x467) == 0) {
            func_02012694(0x13a, (const ::Vector3 *)(c + 0x74));
        } else if (*(u8 *)(c + 0x467) <= 2) {
            func_02012694(0x13b, (const ::Vector3 *)(c + 0x74));
        }
        *(u8 *)(((long long)(int)(c + 0x467))) += 1;
    } else {
        if (((dBgCh_Actr *)(c + 0x1b4))->IsOnGround() != 0) {
            ((dBgCh_Actr *)(c + 0x1b4))->ClearLimMovFlag();
            *(s32 *)(c + 0x98) = 0;
            *(s32 *)(c + 0xa8) = 0;
            if (*(u16 *)(c + 0x452) > 0x3c)
                *(u16 *)(c + 0x452) = 0x1e;
        }
    }
    *(u8 *)(c + 0x107) = 1;
    if (((dEnemyBase_c *)c)->SpawnParticlesIfHitOtherObj(*(dCc_c *)(c + 0x180)) != 0) {
        void *a = dActor_c::FindWithID(*(u32 *)(c + 0x1a4));
        *(s32 *)(c + 0x10c) = 7;
        func_ov002_020aea30(c, a, c + 0x1b4);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
        {
            s32 *f198 = (s32 *)(((long long)(int)(c + 0x198)));
            *f198 |= 1;
        }
    }
    *(u8 *)(c + 0x107) = 0;
}
}

// @symbol func_ov084_0212aab0
void ApproachLinear(short &v, short t, short step);

extern "C" void func_ov084_0212aab0(char *c)
{
    if (((dBgCh_Actr *)(c + 0x1b4))->JustHitGround()) {
        int s = *(int *)(c + 0x460);
        switch (s) {
        case 2: ((dActor_c *)c)->HugeLandingDust(true); break;
        case 1: ((dActor_c *)c)->LandingDust(true); break;
        }
    }
    if (((dBgCh_Actr *)(c + 0x1b4))->IsOnGround()) {
        *(int *)(c + 0x434) = 0;
        int *p = (int *)(((int)c + 0x198));
        *p &= ~4;
    } else {
        ApproachLinear(*(short *)(c + 0x94), *(short *)(c + 0x45c), 0x800);
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
}

// @symbol func_ov084_0212ab48
extern "C" {
void func_ov084_0212ab48(char *c)
{
    unsigned char *p;
    int b;
    func_ov084_02129c9c(c);
    b = (int)(*(unsigned short *)(c + 0xc) == 0xca);
    if (b != 0) {
        *(int *)(c + 0xa8) = (int)(((long long)*(int *)(c + 0xa8) * 0x1800 + 0x800) >> 0xc);
    }
    *(short *)(c + 0x45c) = *(short *)(c + 0x45a);
    p = (unsigned char *)(((int)c + 0x468));
    *p &= ~1;
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
}
}

// @symbol func_ov084_0212abd4
extern "C" {
void func_ov084_0212abd4(char *self)
{
    short step = 0x200;
    func_ov084_02129cf4(self, 0x3e8000);
    _Z14ApproachLinearRiii((int *)(self + 0x98), *(int *)(self + 0x444), 0x500);
    if (((Flag *)(self + 0x468))->b0) {
        if (_Z14ApproachLinearRsss((short *)(self + 0x94), *(s16 *)(self + 0x45a), step)) {
            *(unsigned char *)(((int)self + 0x468)) &= ~1;
            return;
        }
        {
            unsigned char *p = (unsigned char *)(((int)self + 0x468));
            *p = (*p & ~1) | 1;
        }
        return;
    }
    if (*(u16 *)(self + 0x458) != 0) {
        if (*(unsigned char *)(self + 0x113) < 6) {
            if (*(int *)(self + 0x444) <= data_ov084_02130228[*(int *)(self + 0x460)]) {
                func_ov084_02129c9c(self);
            } else {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130cf0[1], 0, 0x1000, 0);
            }
            step = 0x800;
            *(int *)(self + 0x444) = data_ov084_02130268[*(int *)(self + 0x460)];
            *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
        } else {
            *(int *)(self + 0x444) = data_ov084_02130228[*(int *)(self + 0x460)];
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
            *(s16 *)(self + 0x45c) = Vec3_HorzAngle((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x41c));
            step = 0x400;
        }
        _Z14ApproachLinearRsss((short *)(self + 0x94), *(s16 *)(self + 0x45c), step);
        return;
    }
    if (*(int *)(self + 0x440) >= 0x61a8000) {
        *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
        *(s16 *)(self + 0x450) = 0x19;
    }
    {
        int bit = ((dEnemyBase_c *)self)->AngleAwayFromWallOrCliff(*(dBgCh_Actr *)(self + 0x1b4), *(s16 *)(self + 0x45c));
        unsigned char *p = (unsigned char *)(((int)self + 0x468));
        bit &= 1;
        *p = (*p & ~1) | bit;
    }
    if (!((Flag *)(self + 0x468))->b0) {
        if (*(int *)(self + 0x440) < *(int *)(self + 0x448) ||
            (*(unsigned char *)(self + 0x113) < 6 &&
             Vec3_Dist((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x41c)) > 0x3e8000)) {
            if (*(int *)(self + 0x444) <= data_ov084_02130228[*(int *)(self + 0x460)]) {
                func_ov084_02129c9c(self);
            } else {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130cf0[1], 0, 0x1000, 0);
            }
            if (*(unsigned char *)(self + 0x113) >= 6 || *(u16 *)(self + 0x458) != 0) {
                *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
            } else {
                step = 0x600;
                *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x45a);
            }
            *(int *)(self + 0x444) = data_ov084_02130268[*(int *)(self + 0x460)];
        } else {
            *(int *)(self + 0x444) = data_ov084_02130228[*(int *)(self + 0x460)];
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);
            
            if (*(u16 *)(self + 0x450) != 0) {
                *(u16 *)(((int)self + 0x450)) =
                    *(u16 *)(((int)self + 0x450)) - 1;
            } else if (((unsigned)RandomIntInternal(&data_0209e650) >> 16) & 3) {
                *(s16 *)(self + 0x45c) = *(s16 *)(self + 0x94) + (s16)((unsigned)RandomIntInternal(&data_0209e650) >> 16);
                *(u16 *)(self + 0x450) = 0x64;
            } else {
                *(s16 *)(self + 0x45c) = (s16)((unsigned)RandomIntInternal(&data_0209e650) >> 16);
                func_ov084_02129c9c(self);
            }

        }
    }
    if (*(unsigned char *)(self + 0x113) >= 6) {
        if (*(u16 *)(self + 0x456) > 0x1e)
            *(u16 *)(self + 0x458) = *(u16 *)(self + 0x456);
    }
    _Z14ApproachLinearRsss((short *)(self + 0x94), *(s16 *)(self + 0x45c), step);
}
}

// @symbol func_ov084_0212af74
extern "C" {
void func_ov084_0212af74(char *c)
{
    Vector3 targetPos;
    void *player;
    s32 dist;
    s32 flag;
    s16 ang;
    u32 rnd;
    s32 lvl;
    u32 id;

    id = *(u32 *)(c + 0x43c);
    if (id == 0) {
        *(s32 *)(c + 0x460) = 1;
        return;
    }
    player = dActor_c::FindWithID(id);
    if (player == 0) {
        *(s32 *)(c + 0x460) = 1;
        return;
    }

    if (*(s32 *)(c + 0x444) == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, (void *)data_ov084_02130cc8[1], 0, 0x1000, 0);
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, (void *)data_ov084_02130ce8[1], 0, 0x1000, 0);
    }

    func_ov074_0212087c(&targetPos, player, *(u8 *)(c + 0x474));

    if (ApproachAngle((s16 *)(c + 0x94), *(s16 *)(c + 0x45a), 4, 0x1000, 0x400) == 0 &&
        ((dBgCh_Actr *)(c + 0x1b4))->IsOnGround() != 0)
    {
        Vec3_HorzDist(c + 0x5c, c + 0x41c);
        dist = Vec3_HorzDist(c + 0x5c, &targetPos);

        if ((*(s32 *)((char *)player + 0x5cc) == 4 && *(s16 *)((char *)player + 0x5f6) == 0) ||
            DecIfAbove0_Byte(c + 0x475) != 0 ||
            dist > 0x3e8000)
        {
            ang = ((dActor_c *)c)->HorzAngleToCPlayer();
            flag = 1;
            if (((dActor_c *)c)->DistToCPlayer() < 0x3e8000 &&
                AngleDiff(ang, *(s16 *)(c + 0x8e)) < 0x3000)
            {
                *(s16 *)(c + 0x45a) = ((dActor_c *)c)->HorzAngleToCPlayer();
                *(s16 *)(c + 0x45c) = *(s16 *)(c + 0x45a);
                lvl = *(s32 *)(c + 0x460);
                if (*(s32 *)(c + 0x444) == data_ov084_02130228[lvl]) {
                    *(s32 *)(c + 0x444) = data_ov084_02130268[lvl];
                }
                lvl = *(s32 *)(c + 0x460);
                if (*(s32 *)(c + 0x444) != data_ov084_02130268[lvl]) {
                    flag = 0;
                    *(s32 *)(c + 0xa8) = data_ov084_02130248[lvl];
                    *(s32 *)(c + 0x98) = 0;
                    *(s32 *)(c + 0x444) = data_ov084_02130268[*(s32 *)(c + 0x460)];
                }
                if (((dBgCh_Actr *)(c + 0x1b4))->JustHitGround() != 0) {
                    ((dActor_c *)c)->LandingDust(1);
                }
            } else {
                if (DecIfAbove0_Short(c + 0x454) == 0) {
                    rnd = RandomIntInternal(&data_0209e650);
                    *(s16 *)(c + 0x454) = (s16)((rnd >> 0x1b) + 0x1e);
                    *(s16 *)(c + 0x45a) = (s16)(rnd >> 0x10);
                }
                *(s32 *)(c + 0x444) = 0;
            }
            if (flag != 0) {
                _Z14ApproachLinearRiii((int *)(c + 0x98), *(s32 *)(c + 0x444), 0x500);
            }
        } else {
            *(s16 *)(c + 0x45a) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &targetPos);
            if (Vec3_HorzDist(c + 0x5c, &targetPos) < 0x32000) {
                *(s32 *)(c + 0x444) = *(s32 *)((char *)player + 0x5e8) >> 2;
            } else if (Vec3_HorzDist(c + 0x5c, &targetPos) < 0x64000) {
                *(s32 *)(c + 0x444) = *(s32 *)((char *)player + 0x5e8) >> 1;
            } else if (Vec3_HorzDist(c + 0x5c, &targetPos) < 0x96000) {
                *(s32 *)(c + 0x444) = *(s32 *)((char *)player + 0x5e8);
            } else {
                s32 idx = *(s32 *)(c + 0x460);
                s32 v = data_ov084_02130268[idx];
                *(s32 *)(c + 0x444) = v + *(s32 *)((char *)player + 0x5e8);
            }
            Math_Function_0203b14c((int *)(c + 0x98), *(s32 *)(c + 0x444), 0x800, 0x10000, 4);
        }
    }

    if (*(s32 *)(c + 0x60) < *(s32 *)(c + 0x420) - 0x3e8000) {
        ((fBase_c *)c)->MarkForDestruction();
    }

    if (*(s32 *)((char *)player + 0x5cc) != 4)
        return;
    if (*(s16 *)((char *)player + 0x5f6) == 0)
        *(u8 *)(c + 0x475) = 0x1e;
}
}

// @symbol func_ov084_0212b2dc
extern "C" {
void func_ov084_0212b2dc(char *c) {
    if (*(int *)(c + 0x460) == 3) {
        func_ov084_0212af74(c);
    } else {
        func_ov084_0212abd4(c);
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
}
}

// @symbol _ZN7daKrb_c16OnAimedAtWithEggEv
int daKrb_c::OnAimedAtWithEgg()
{
    unsigned short v = *(unsigned short*)((char*)this + 0xc);
    int b = (v == (unsigned short)0xc8) ? 1 : 0;
    if (b) return 0x41000;
    int b2 = (v == (unsigned short)0xca) ? 1 : 0;
    if (b2) return 0x96000;
    return 0x14000;
}

// @symbol _ZN7daKrb_c13OnTurnIntoEggER6Player
void daKrb_c::OnTurnIntoEgg(Player &playerRef)
{
    char *self = (char *)this;
    char *player = (char *)&playerRef;
    Obj *o = (Obj *)self;
    int b5;
    bool b4;

    if ((*(unsigned char *)(self + 0x113) & 0xf) < 6 || mRewardType == 2) {
        *(int *)(self + 0x5c) = *(int *)(self + 0x41c);
        *(int *)(self + 0x60) = *(int *)(self + 0x420);
        *(int *)(self + 0x64) = *(int *)(self + 0x424);
        ((dCapEnemy_c *)self)->RespawnIfHasCap();
    }

    if (o->GetState() == 6) {
        if (((Player *)player)->IsCollectingCap()) {
            if (*(unsigned char *)(self + 0x108) == 1)
                ((dActor_c *)self)->GivePlayerCoins(*(Player *)player, 1, 0);
            func_ov084_021296cc(self);
        } else {
            b5 = 0;
            b4 = b5;
            if (*(unsigned char *)(self + 0x108) == 1)
                b5 = 1;
            if (mRewardType == 1) {
                ((dActor_c *)self)->UntrackStar(*(s8 *)(self + 0x465));
                b4 = 1;
                dActor_c::Spawn(0xb4, 0x50, *(Vector3 *)(self + 0x41c), 0, *(signed char *)(self + 0xcc), -1);
                /* unsigned on the load side only: spelling both sides identically
                   lets mwccarm CSE the field address (add r2,r7,#8 + [r2]),
                   one instruction the ROM does not have -- it wants [r7,#8] direct */
                *(int *)(self + 8) = *(unsigned int *)(self + 8) & 0xff0f;
            } else if (mRewardType == 2) {
                if (mStarID == data_0209f344[data_0209f208[0]]) {
                    ((dActor_c *)self)->UntrackStar(*(s8 *)(self + 0x465));
                    mRewardType = 3;
                    b4 = 1;
                }
            }
            ((Player *)player)->RegisterEggCoinCount(b5, b4, 0);
        }
    } else if (o->GetState() == 4) {
        if (*(unsigned char *)(self + 0x108) == 1)
            ((dActor_c *)self)->GivePlayerCoins(*(Player *)player, 1, 0);
    }

    func_ov084_02129498(self);
}

// @symbol _ZN7daKrb_c16CleanupResourcesEv
int daKrb_c::CleanupResources()
{
  int i;
  if (mGoombaType == 2)
    UnloadBlueCoinModel(((char*)this));
  data_ov084_02130cf8.Release();
  for (i = 0; i < 7; ++i)
    ((SharedFilePtr *)(data_ov084_02130278[i]))->Release();
  if ((unsigned char)(mRewardType + 0xff) <= 1)
    UnloadSilverStarAndNumber();
  UnloadCapModel();
  if (mGoombaType == 3) {
    unsigned int id = mTargetUniqueID;
    if (id != 0) {
      char* a = (char*)dActor_c::FindWithID(id);
      if (a != 0) {
        unsigned char *p = (unsigned char*)(((int)a + 0x602));
        *p = *p - 1;
      }
    }
  }
  return 1;
}

// @symbol _ZN7daKrb_c16OnPendingDestroyEv
void daKrb_c::OnPendingDestroy()
{
}

// @symbol _ZN7daKrb_c6RenderEv
int daKrb_c::Render()
{
    int locked;
    volatile Vector3 backup;

    locked = (mFlags & 0x40000) != 0;
    if (locked || mIsDormant != 0) return 1;

    backup.x = mScaleX;
    backup.y = mScaleY;
    backup.z = mScaleZ;

    if (mDeathState == 1) {
        mScaleX = (int)(((long long)mScaleX * data_ov084_02130258[mGoombaType] + 0x800) >> 12);
        mScaleY = (int)(((long long)mScaleY * data_ov084_02130258[mGoombaType] + 0x800) >> 12);
        mScaleZ = (int)(((long long)mScaleZ * data_ov084_02130258[mGoombaType] + 0x800) >> 12);
    }

    {
        Sub* b = (Sub*)((char*)&mModelAnim);
        b->m5((Vector3*)((char*)&mScaleX));
    }

    mScaleX = backup.x;
    mScaleY = backup.y;
    mScaleZ = backup.z;
    ((MaterialChanger *)(((char*)this) + 0x3fc))->Update(*(ModelComponents *)(((char*)this) + 0x378));
    _ZN11dCapEnemy_c14RenderCapModelEPK7Vector3(((char*)this), 0);
    return 1;
}

// @symbol _ZN7daKrb_c8BehaviorEv
int daKrb_c::Behavior()
{
    Vector3 v1;
    Vector3 v2;
    int r;
    int st;

    func_ov084_02129864(((char*)this));
    func_ov084_021290d4(((char*)this));
    r = _ZN11dCapEnemy_c11GetCapStateEv(((char*)this));
    if (r == 0)
        return 1;
    if (r == 1) {
        *(u32*)((char*)&mFlags) |= 0x10000000;
        PoofDust();
    }
    if (mGoombaType != 3 && mState != 3 &&
        mEatenByYoshi == 0 && mDeathState == 0 &&
        _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x5dc000) != 0)
    {
        _ZN11dCapEnemy_c12Unk_02005d94Ev(((char*)this));
        return 1;
    }

    if (mDeathState != 0) {
        r = UpdateKillByInvincibleChar(*(dBgCh_Actr *)(((char*)this) + 0x1b4), *(ModelAnim *)(((char*)this) + 0x370), 3);
        if (r != 0) {
            if (r == 2) {
                func_ov084_02129498(((char*)this));
                v1.x = 0;
                v1.y = 0x6c000;
                v1.z = 0;
                ReleaseCap(v1);
                mPosX = mHomePos.x;
                mPosY = mHomePos.y;
                mPosZ = mHomePos.z;
                mAngleX = 0;
                mAngleY = 0;
                mAngleZ = 0;
                RespawnIfHasCap();
                func_ov084_021296cc(((char*)this));
            }
            return 1;
        }
        if (func_ov084_021298d0(((char*)this)) == 0)
            func_ov084_0212a580(((char*)this));
        return 1;
    }

    if (func_ov084_02129a00(((char*)this)) != 0)
        return 1;

    if (mState >= 3 ||
        (mGoombaType == 3 && (int)mModelAnim.file == data_ov084_02130cc8[1]))
    {
        mModelAnim.speed = 0x1000;
    } else {
        int v = _ZN4cstd4fdivEii(mHorzSpeed, mScaleX * 2);
        if (v > 0x3000)
            v = 0x3000;
        mModelAnim.speed = v;
    }

    MakeVanishLuigiWork(*(dCc_c *)(((char*)this) + 0x180));

    if (mState != 2) {
        func_ov084_0212934c(((char*)this));
        ((Animation *)&mModelAnim)->Advance();
    }

    st = mState;
    {
        int* q = &data_ov084_02130d74[st * 2];
        int adj = q[1];
        char* thiz = ((char*)this) + (adj >> 1);
        void (*fn)(char*);
        if (adj & 1)
            fn = *(void(**)(char*))(*(char**)thiz + q[0]);
        else
            fn = (void(*)(char*))q[0];
        fn(thiz);
    }

    {
        u16* hp = (u16*)((char*)&mStateTimer);
        *hp += 1;
        if (st != mState)
            *hp = 0;
    }

    func_ov084_02129ed4(((char*)this));

    if (mCapId < 6)
        UpdatePos(0);
    else
        UpdatePos((dCc_c *)(((char*)this) + 0x180));

    if (mDeathState == 0 && mState != 2 && mState != 3) {
        if (IsGoingOffCliff(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 0x32000, 0x1f49, 0, 1, 0x32000) != 0) {
            mPosX = mSafePos.x;
            mPosY = mSafePos.y;
            mPosZ = mSafePos.z;
        } else {
            mSafePos.x = mPosX;
            mSafePos.y = mPosY;
            mSafePos.z = mPosZ;
        }
    }

    {
        int lvl = mGoombaType;
        if (lvl == 0) {
            UpdateWMClsn(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 0);
        } else if (data_0209f2f8 == 6 || data_0209f2f8 == 0x1b) {
            if (unk_444 == data_ov084_02130228[lvl] && mDeathState != 7)
                UpdateWMClsn(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 3);
            else
                UpdateWMClsn(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 2);
        } else {
            UpdateWMClsn(*(dBgCh_Actr *)(((char*)this) + 0x1b4), 2);
        }
    }

    func_ov084_021294d0(((char*)this));
    ((dCc_c *)&mdCcAc_c)->Clear();
    if (mDeathState == 0)
        ((dCc_c *)&mdCcAc_c)->Update();
    func_ov084_0212a580(((char*)this));
    func_ov084_02129238(((char*)this));

    if (mState == 0) {
        int b = (mFlags & 8) ? 1 : 0;
        if (b == 0) {
            if (Vec3_Dist((Vector3*)((char*)&mPosX), &mStuckCheckPos) < 0xa000) {
                *(u16*)((char*)&mStuckTimer) += 1;
                if (mCapId < 6 && mStuckTimer == 0x1e) {
                    func_ov084_02129c9c(((char*)this));
                    mTimer458 = 0x5a;
                }
                if (mStuckTimer >= 0x12c && mTimer458 == 0) {
                    func_ov084_021296cc(((char*)this));
                    SpawnCoin();
                    func_ov084_02129498(((char*)this));
                    v2.x = 0;
                    v2.y = 0x6c000;
                    v2.z = 0;
                    ReleaseCap(v2);
                    mPosX = mHomePos.x;
                    mPosY = mHomePos.y;
                    mPosZ = mHomePos.z;
                    RespawnIfHasCap();
                    return 1;
                }
            } else {
                mStuckTimer = 0;
                mStuckCheckPos.x = mPosX;
                mStuckCheckPos.y = mPosY;
                mStuckCheckPos.z = mPosZ;
            }
            goto done;
        }
    }

    if (mTimer458 == 0)
        mStuckTimer = 0;
done:
    return 1;
}

// @symbol _ZN7daKrb_c13InitResourcesEv
int daKrb_c::InitResources()
{
    char *c = (char *)this;
    int i;

    mRewardType = (*(unsigned int*)(c + 8) >> 4) & 0xf;
    mStarTracked = -1;
    *(unsigned char*)(c + 0x112) = (*(unsigned int*)(c + 8) >> 8) & 0xf;
    mStarID = (*(unsigned int*)(c + 8) >> 0xc) & 0xf;

    if (mRewardType == 1)
    {
        mStarTracked = ((dActor_c *)c)->TrackStar(mStarID, 1);
        LoadSilverStarAndNumber();
    }
    else if (mRewardType == 2)
    {
        LoadSilverStarAndNumber();
    }

    Model::LoadFile(data_ov084_02130cf8);
    for (i = 0; i < 7; i++)
        Animation::LoadFile(*(SharedFilePtr *)(data_ov084_02130278[i]));

    ((dCapEnemy_c *)c)->AddCap((unsigned char)(*(int*)(c + 8) & 0xf));

    if ((*(unsigned char*)(c + 0x113) & 0xf) < 6)
        /* The load and the store must not be spelled the same way: mwcc CSEs the
           field address across an RMW and the ROM re-issues it. See
           notes/mwccarm-codegen.md. */
        ((int*)c)[2] = *(int*)(c + 8) & 0xf0ff;

    if (((dCapEnemy_c *)c)->DestroyIfCapNotNeeded() == 0)
        return 0;

    if (((ModelBase *)(c + 0x370))->SetFile((BMD_File *)(SHARED_FILE(data_ov084_02130cf8)), 1, -1) == 0)
        return 0;

    if (((ShadowModel *)(c + 0x3d4))->InitCylinder() == 0)
        return 0;

    MaterialChanger::Prepare(*(BMD_File*)SHARED_FILE(data_ov084_02130cf8), *(BMA_File*)&data_ov084_0213089c);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(c + 0x3fc, &data_ov084_0213089c, 0x40000000, 0x1000, 0);

    *(unsigned char*)(c + 0x108) = 1;

    {
        int id = *(unsigned short*)(c + 0xc);
        int cond = (id == 0xc9);
        if (cond != false)
        {
            mGoombaType = 0;
        }
        else
        {
            cond = (id == 0xc8);
            if (cond != false)
            {
                if (*(int*)(c + 8) == 0xeeee || *(int*)(c + 8) == 0xeeef)
                {
                    mGoombaType = 3;
                    *(int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~2;
                    if (*(int*)(c + 8) == 0xeeee)
                        *(unsigned char*)(c + 0x108) = 0;
                }
                else
                {
                    mGoombaType = 1;
                }
            }
            else
            {
                mGoombaType = 2;
                LoadBlueCoinModel(c);
            }
        }
    }

    {
        int scale = data_ov084_02130258[mGoombaType];
        *(int*)(c + 0x80) = scale;
        *(int*)(c + 0x84) = scale;
        *(int*)(c + 0x88) = scale;
    }
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x180, c, *(int*)(c + 0x80) * 0x3c, data_ov084_02130208[mGoombaType], 0x200000, 0xa6efe0);

    if (mGoombaType == 2)
        *(int*)(((int)c + 0x19c) & 0xFFFFFFFFFFFFFFFF) &= ~0x8000;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(c + 0x1b4, c, *(int*)(c + 0x80) * 0x3c, *(int*)(c + 0x80) * 0x3c, 0, 0);
    ((dBgCh_Actr *)(c + 0x1b4))->StartDetectingWater();

    mSoundLatchFlags = 0;
    mState = 0;
    *(int*)(c + 0x10c) = 0;
    unk_438 = 0;
    mDistToPlayer = 0x7fffffff;
    mInitAngleY = *(short*)(c + 0x94);
    unk_444 = data_ov084_02130228[mGoombaType];
    mHeadingHoldTimer = 0;
    mTargetUniqueID = 0;
    mWanderRerollTimer = 0;
    mStuckTimer = 0;
    *(int*)(c + 0x428) = *(int*)(c + 0x5c);
    *(int*)(c + 0x42c) = *(int*)(c + 0x60);
    *(int*)(c + 0x430) = *(int*)(c + 0x64);
    mTimer458 = 0;
    func_ov084_021290d4(c);

    *(int*)(c + 0x41c) = *(int*)(c + 0x5c);
    *(int*)(c + 0x420) = *(int*)(c + 0x60);
    *(int*)(c + 0x424) = *(int*)(c + 0x64);
    *(int*)(c + 0x9c) = data_ov084_02130238[mGoombaType];
    *(int*)(c + 0xa0) = -0x32000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov084_02130ce8[1], 0, 0x1000, 0);

    unk_467 = 0;
    mSavedParam = *(int*)(c + 8);
    return 1;
}

// @symbol _ZN7daKrb_c13OnYoshiTryEatEv
int daKrb_c::OnYoshiTryEat()
{
    unsigned short v = *(unsigned short*)((char*)this + 0xc);
    int b = (v == (unsigned short)0xc8) ? 1 : 0;
    if (b) return 0x6;
    int b2 = (v == (unsigned short)0xc9) ? 1 : 0;
    if (b2) return 0x4;
    return 0x0;
}

// @symbol daKrb_c_classInit_KURIBO_L
extern "C" daKrb_c *daKrb_c_classInit_KURIBO_L()
{
    return new daKrb_c();
}

// @symbol daKrb_c_classInit_KURIBO_S
extern "C" daKrb_c *daKrb_c_classInit_KURIBO_S()
{
    return new daKrb_c();
}

// @symbol daKrb_c_classInit_KURIBO
extern "C" daKrb_c *daKrb_c_classInit_KURIBO()
{
    return new daKrb_c();
}

