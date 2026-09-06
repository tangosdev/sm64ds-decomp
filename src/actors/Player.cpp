//cpp
/* Player -- ov002.  Reconstructed translation unit: ROM ordinals 0..300 of the
 * contiguous linker run 0x020bd828..0x020e6a08, i.e. 0x020bd828..0x020cfea4, 301 functions.
 *
 * Assembled from the 301 one-function legacy sources listed at the foot of the
 * file.  Each member keeps its OWN declarations, inside its own body: mwccarm
 * 2004/b56 gives a block-scope declaration inside an `extern "C"` region C
 * linkage (measured), so 301 independently-recovered views of one ROM symbol
 * coexist without one having to be chosen over the others, and no call site
 * had to be rewritten to fit a merged spelling.  Only what a function body
 * cannot hold -- a namespace, a class with static or declared-but-undefined
 * members, a type named in a member's own signature -- is hoisted below, and
 * those tags are made unique.
 *
 * SOURCE ORDER IS ROM-ASCENDING AND `#pragma defer_codegen off` IS LOAD-BEARING.
 * Do not reorder the members and do not delete that pragma.  Ordinal 35,
 * _ZN6Player7SetAnimEji5Fix12IiEj, reproduces only under `#pragma
 * opt_propagation off`, and under mwccarm's default DEFERRED codegen that
 * pragma is file-global: left plain it costs 46 other members their match, and
 * bracketed in push/pop it bound to nothing, because the state that binds is
 * the state at end of file.  With codegen undeferred the bracket binds where it
 * is written -- and undeferred codegen emits .text in SOURCE order, not
 * reversed, which is why this file runs low address first.  Re-measured by the
 * builder at this revision, deferred codegen unless stated: 255/301 with a
 * file-global `opt_propagation off`; 300/301 with it deleted outright; 300/301
 * bracketed in push/pop -- identical, because the bracket buys nothing while
 * codegen is deferred; 301/301 as written.  Deleting only `defer_codegen off`
 * still scores 300/301 on bytes but reverses the emitted .text order, and it is
 * the pre-link object audit, not the byte compare, that refuses that.
 *
 * Two members carry a source change the C-to-C++ flip forced, both the same
 * construct: a two-int struct copy that C compiles as load/load/store/store and
 * C++ scalarizes into load/store/load/store.  The array-wrapper view
 * (`struct P2w { int w[2]; }`) restores the ROM's shape in func_ov002_020c14b8
 * and func_ov002_020c25a8.  Nothing else in the 301 bodies was rewritten.
 *
 * See config/tu_manifest.d/ov002/Player.json for the boundary evidence, and
 * notes/data/class-facts/Player.json for what the cartridge proves about the
 * class -- including that its own RTTI name is `daPly_c`, not `Player`.
 */

#pragma defer_codegen off

#include "types.h"
#include "common.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "Particle__System.h"
#include "SurfaceInfo.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Actr.h"

/* ---------------------------------------------------------------------------
 * Shadow types that cannot live inside a function body.
 * ------------------------------------------------------------------------- */
struct Ret;
struct V3Quad { Vector3 a; Vector3 d; Vector3 e; Vector3 b; };
struct Fix12v { int v; Fix12v(int a) : v(a) {} };
namespace Sound { void PlayBank0(unsigned int, const Vector3 &); }
struct V3a {
 int x, y, z;
 void set(int x_, int y_, int z_) { x = x_; y = y_; z = z_; }
};
namespace Particle { extern "C" void _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int, unsigned int, int, int, int, const void *, void *); }
struct Obj1eb4 {
 char pad0[0x5c];
 Vector3 p5c; /* 0x5c */
 char pad1[0x74 - 0x68];
 Vector3 p74; /* 0x74 */
 char pad2[0x8c - 0x80];
 short p8c; /* 0x8c */
 short p8e; /* 0x8e */
 short p90; /* 0x90 */
 char pad3[0x94 - 0x92];
 short p94; /* 0x94 */
 char pad4[0x98 - 0x96];
 int p98; /* 0x98 */
 char pad5[0x674 - 0x9c];
 int p674; /* 0x674 */
 char pad6[0x6de - 0x678];
 unsigned char b6de; /* 0x6de */
 char pad7[0x6e3 - 0x6df];
 unsigned char b6e3; /* 0x6e3 */
};
struct Obj2fec {
 char pad0[0x730];
 unsigned char idx;
 char pad1[3];
 int f734;
 int f738;
};
typedef struct {
 char _pad[0x734];
 int unk734;
 int unk738;
} Arg0;
typedef struct {
 int unk0, unk4, unk8, unkC;
} Arg1;
struct State3i { int a, b, c; };
struct V3 {
 int x, y, z;
 V3(int x, int y, int z) : x(x), y(y), z(z) {}
 V3() {}
};
struct PlayerRaw {
 char pad[0x6e3];
 unsigned char state;
};
struct Ret { int pad; int y; };
struct Arg {
 virtual void *m0();
 virtual void *m1();
 virtual struct Ret *m2();
};
struct CameraRaw {
 char pad60[0x60];
 int f60; // 0x60
 char pad_to6d8[0x6d8 - 0x64];
 unsigned char f6d8; // 0x6d8
 char pad_to6e5[0x6e5 - 0x6d9];
 unsigned char f6e5; // 0x6e5
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN6Player8CanPauseEv, 0x020bd828, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player8CanPauseEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::CanPause()
{
    extern struct State;
    extern State data_ov002_0211067c;
    extern State data_ov002_021106ac;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
  if(*(unsigned char*)((char*)&mIsUnderwater)){
    if(_ZN6Player7IsStateERNS_5StateE(((void*)this), &data_ov002_0211067c)) goto ret1a;
    if(_ZN6Player7IsStateERNS_5StateE(((void*)this), &data_ov002_021106ac)) goto ret1a;
    goto ret0;
ret1a:
    return 1;
  }
  if(*(unsigned char*)((char*)&mIsAirborne)) goto ret0;
  if(*(unsigned char*)((char*)&mIsTakingDamage)) goto ret0;
  if(*(unsigned char*)((char*)&mIsNoControl)) goto ret0;
  return 1;
ret0:
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- func_ov002_020bd8ac, 0x020bd8ac, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bd8ac
extern "C" {
int func_ov002_020bd8ac(unsigned char *p)
{
    return p[1801] == 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020bd8c0, 0x020bd8c0, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bd8c0
extern "C" {
void func_ov002_020bd8c0(char* c, unsigned int r1){
    extern int _ZN5Sound8EndMusicEjj(unsigned char);
    extern int _ZN5Sound8SetMusicEjj(unsigned char, unsigned int);
  unsigned int r2=*(unsigned int*)(c+0x678);
  if(r2==0){
    _ZN5Sound8EndMusicEjj(*(unsigned char*)(c+0x6d8));
    *(unsigned int*)(c+0x680)=0;
  } else if(*(unsigned int*)(c+0x680)==r1 && r2!=r1){
    _ZN5Sound8EndMusicEjj(*(unsigned char*)(c+0x6d8));
    _ZN5Sound8SetMusicEjj(*(unsigned char*)(c+0x6d8), *(unsigned int*)(c+0x678));
    *(unsigned int*)(c+0x680)=*(unsigned int*)(c+0x678);
  }
  *(unsigned int*)(c+0x67c)=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020bd928, 0x020bd928, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bd928
extern "C" {
void func_ov002_020bd928(char* c, unsigned int r4){
    extern int _ZN5Sound8EndMusicEjj(unsigned char);
    extern int _ZN5Sound8SetMusicEjj(unsigned char, unsigned int);
  unsigned int r2=*(unsigned int*)(c+0x678);
  if(r2==r4){ *(unsigned int*)(c+0x67c)=r4; return; }
  if((r2|*(unsigned int*)(c+0x67c))!=0){
    _ZN5Sound8EndMusicEjj(*(unsigned char*)(c+0x6d8));
  }
  *(unsigned int*)(c+0x67c)=r4;
  *(unsigned int*)(c+0x680)=r4;
  _ZN5Sound8SetMusicEjj(*(unsigned char*)(c+0x6d8),r4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020bd984, 0x020bd984, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bd984
extern "C" {
void func_ov002_020bd984(char* c, unsigned int r1){
    extern int _ZN5Sound8EndMusicEjj(unsigned char);
    extern int _ZN5Sound8SetMusicEjj(unsigned char, unsigned int);
  unsigned int r2=*(unsigned int*)(c+0x67c);
  if(r2==0){
    _ZN5Sound8EndMusicEjj(*(unsigned char*)(c+0x6d8));
    *(unsigned int*)(c+0x680)=0;
  } else if(*(unsigned int*)(c+0x680)==r1 && r2!=r1){
    _ZN5Sound8EndMusicEjj(*(unsigned char*)(c+0x6d8));
    _ZN5Sound8SetMusicEjj(*(unsigned char*)(c+0x6d8), *(unsigned int*)(c+0x67c));
    *(unsigned int*)(c+0x680)=*(unsigned int*)(c+0x67c);
  }
  *(unsigned int*)(c+0x678)=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov002_020bd9ec, 0x020bd9ec, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bd9ec
extern "C" {
void func_ov002_020bd9ec(char* c, unsigned int r4){
    extern int _ZN5Sound8EndMusicEjj(unsigned char);
    extern int _ZN5Sound8SetMusicEjj(unsigned char, unsigned int);
  unsigned int r2=*(unsigned int*)(c+0x67c);
  if(r2==r4){ *(unsigned int*)(c+0x678)=r4; return; }
  if((*(unsigned int*)(c+0x678)|r2)!=0){
    _ZN5Sound8EndMusicEjj(*(unsigned char*)(c+0x6d8));
  }
  *(unsigned int*)(c+0x678)=r4;
  *(unsigned int*)(c+0x680)=r4;
  _ZN5Sound8SetMusicEjj(*(unsigned char*)(c+0x6d8),r4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov002_020bda48, 0x020bda48, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bda48
extern "C" {
void func_ov002_020bda48(char* c)
{
    extern void func_ov002_020e032c(void*);
    extern void func_ov002_020bdef0(void*);
    extern void func_ov002_020bdd9c(void*);
    extern void func_ov002_020d80d0(void*);
    extern void func_ov002_020bdd2c(void*);
    extern void func_ov002_020cc1f4(void*);
    extern void func_ov002_020de968(void*);
    *(unsigned short*)(c+0x6ae) = 0;
    func_ov002_020e032c(c);
    func_ov002_020bdef0(c);
    func_ov002_020bdd9c(c);
    func_ov002_020d80d0(c);
    func_ov002_020bdd2c(c);
    func_ov002_020cc1f4(c);
    func_ov002_020de968(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN6Player8HasNoCapEv, 0x020bda98, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player8HasNoCapEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::HasNoCap()
{
    extern signed char data_0209f2f8;
    extern unsigned char data_0209f2d8;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int _ZN8SaveData16HasPlayerLostCapEv(void);
    extern int data_ov002_0211058c;
    if(data_0209f2f8 == 0x1f) return 0;
    int r = (data_0209f2d8 == 2);
    if(!r){
        if(!_ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_0211058c)){
            r = (data_0209f2d8 == 1);
            if(!r){
                if(_ZN8SaveData16HasPlayerLostCapEv()) return 1;
            }
        }
    }
    if(param1 == 3) return 0;
    r = mHasNoCap != 0;
    return r;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov002_020bdb50, 0x020bdb50, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bdb50
extern "C" {
void func_ov002_020bdb50(char* c, int arg) {
    extern void func_ov002_020bdc18(void*);
    extern void Player_ReleaseHeldActor(void*);
    extern void func_ov002_020d71a0(void*);
    extern void func_ov002_020d5cec(void);
    extern void func_ov002_020bdb50(void*, int);
    func_ov002_020bdc18(c);
    Player_ReleaseHeldActor(c);
    if (arg != 0) return;
    if (*(int*)(c+0x360) == 0) return;
    func_ov002_020d71a0(c);
    {
        char* obj = *(char**)(c+0x360);
        char* vt = *(char**)obj;
        int r = (*(int(**)(char*))(vt+0x48))(obj);
        if (r == 0) return;
    }
    {
        char* obj = *(char**)(c+0x360);
        int b1 = (*(unsigned short*)(obj+0xc) == 0xbf);
        if (b1) {
            func_ov002_020d5cec();
            (*(unsigned short *)(((int)c + 0x6ce))) &= ~2;
        } else {
            char* vt = *(char**)obj;
            (*(void(**)(char*, char*))(vt+0x4c))(obj, c);
        }
    }
    *(int*)(c+0x360) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov002_020bdc18, 0x020bdc18, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bdc18
extern "C" {
void func_ov002_020bdc18(char* c){
    extern int func_ov002_020edca4(void*);
  int i;
  for(i=0;i<5;i++){
    int* a=*(int**)(c+0x588);
    if(a[i]!=0){
      func_ov002_020edca4((void*)a[i]);
      a=*(int**)(c+0x588);
      a[i]=0;
    }
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN6Player16IncMegaKillCountEv, 0x020bdc58, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16IncMegaKillCountEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::IncMegaKillCount()
{
    struct Vec3
    {
      int x;
      int y;
      int z;
    };
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void*);
    extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int p, void*, void*, int a, int b);
    extern void GiveLives(int);
    extern void func_ov002_020f0e54(void*, void*);
  struct Vec3 v;
  char *new_var2;
  int new_var3;
  char *a;
  char *st;
  int y;
  char *new_var;
  if ((*((unsigned char *) ((char *)&mIsMega))) == 0)
  {
    return;
  }
  ++mMegaKillCount;
  new_var3 = -1;
  st = ((char *)this);
  st = st + 0x600;
  if ((*((unsigned short *) (st + 0xd0))) >= 8)
  {
    *((unsigned short *) (st + 0xd0)) = 8;
    _ZN5Sound9PlayBank3EjRK7Vector3(0x6e, ((char *)this) + 0x74);
    GiveLives(1);
  }
  v.x = *((int *) ((char *)&mPosX));
  y = *((int *) ((char *)&mPosY));
  v.y = y;
  new_var = ((char *)this);
  v.z = *((int *) (new_var + 0x64));
  v.y = y + 0x190000;
  new_var2 = (char *) _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x14b, mMegaKillCount, &v, 0, *((signed char *) ((char *)&mAreaId)), new_var3);
  a = new_var2;
  if (a == 0)
  {
    return;
  }
  *((unsigned short *) ((a + 0x100) + 0x4c)) = 0;
  func_ov002_020f0e54(a, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov002_020bdd2c, 0x020bdd2c, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bdd2c
extern "C" {
void func_ov002_020bdd2c(void* c) {
    extern void func_ov002_020db8bc(void*, unsigned char val);
    extern void func_ov002_020bd984(void*, int x);
    extern void* data_0209f318;
    extern void func_ov002_020bdd2c(void*);
  if (*(unsigned char*)((char*)c+0x703) == 1) {
    func_ov002_020db8bc((unsigned char*)c, 3);
    {
      int* base = (int*)(((int)data_0209f318 + 0x154));
      *base &= ~0x80;
    }
    func_ov002_020bd984(c, 0x31);
    *(int*)((char*)c+0x638) = 0;
    *(int*)((char*)c+0x634) = *(int*)((char*)c+0x638);
    *(int*)((char*)c+0x628) = 0;
    *(short*)((char*)c+0x6d0) = 0;
  }
  *(unsigned char*)((char*)c+0x703) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov002_020bdd9c, 0x020bdd9c, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bdd9c
extern "C" {
void func_ov002_020bdd9c(char* c){
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern void _ZN10ModelAnim24CopyERKS_Pcj(void*, void*, void*, unsigned int n);
    extern void func_ov002_020bd984(void*, int n);
    extern char* data_ov002_020ff480[];
    extern void func_ov002_020bdd9c(void*);
  if(*(unsigned char*)(c+0x6f9) != 1) return;
  *(unsigned char*)(c+0x6f9) = 0;
  unsigned int v = *(unsigned int*)(c+8);
  int id = _ZNK6Player14GetBodyModelIDEjb(c, v & 0xff, 0);
  char* t = data_ov002_020ff480[*(unsigned int*)(c+0x63c) + v];
  _ZN10ModelAnim24CopyERKS_Pcj( *(void**)(c + (id<<2) + 0xdc), *(void**)(c+0xec), *(char**)(t+4), 0);
  func_ov002_020bd984(c, 0x34);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN6Player14InitMetalWarioEv, 0x020bde14, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player14InitMetalWarioEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::InitMetalWario()
{
    extern void func_ov002_020bda48(void*);
    extern u32 _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, bool b);
    extern void func_ov002_020bd9ec(void*, u32 a);
    extern void func_ov002_020c43c4(void*, u32 a);
    extern char *data_ov002_020ff480[];
    extern int func_ov002_020bea7c(void*);
    extern void _ZN6Player18TurnOffToonShadingEj(void*, u32);
    extern void _ZN10ModelAnim24CopyERKS_Pcj(void*, void*, void*, u32);
    u32 v;
    u32 id;

    func_ov002_020bda48(((char *)this));
    if (func_ov002_020bea7c(((char *)this)) != 0) {
        u32 b = mHatCharacter;
        if (b != 2)
            return;
        _ZN6Player18TurnOffToonShadingEj(((char *)this), b);
        _ZN6Player18TurnOffToonShadingEj(((char *)this), mPrevCharacter);
        mCapFlags = 0;
        param1 = mHatCharacter;
        mHasNoCap = 0;
    }

    mIsMetal = 1;

    v = param1;
    id = _ZNK6Player14GetBodyModelIDEjb(((char *)this), v & 0xff, 0);
    _ZN10ModelAnim24CopyERKS_Pcj( *(void **)(((char *)this) + id * 4 + 0xdc), *(void **)(((char *)this) + v * 4 + 0xdc), *(char **)((char *)data_ov002_020ff480[mCharFileBase + v] + 4), 0);

    mPowerupTimer = 0x258;
    func_ov002_020bd9ec(((char *)this), 0x34);
    func_ov002_020c43c4(((char *)this), 4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov002_020bdef0, 0x020bdef0, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bdef0
extern "C" {
void func_ov002_020bdef0(char* c){
    extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
    extern int func_ov002_020bd984(void*, int);
    extern void func_ov002_020bdef0(void*);
  if(*(unsigned char*)(c+0x6fb)==0) return;
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x254, (char*)*(int*)(c+0xe0)+8);
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x268, (char*)*(int*)(c+0x158)+8);
  *(int*)(c+0x25c)=0;
  *(int*)(c+0x270)=0;
  *(unsigned char*)(c+0x6fb)=0;
  func_ov002_020bd984(c,0x33);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN6Player15InitVanishLuigiEv, 0x020bdf4c, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15InitVanishLuigiEv
/* recovered: named members + shared header, real C++ method
 *
 * Starts the vanish cap: sets the flag, arms mPowerupTimer to 0x258 (the same 600
 * frames mBalloonTimer uses), then the sound and effect calls.
 *
 * Note the original wrote mPowerupTimer as `c + 0x600 + 0xae` rather than 0x6ae --
 * the split is how the compiler addressed it, not a second field.
 */
extern "C" {
void Player::InitVanishLuigi()
{
    extern int func_ov002_020bda48(void*);
    extern int func_ov002_020bd9ec(void*, int);
    extern int func_ov002_020c43c4(void*, int);
    func_ov002_020bda48(this);
    mIsVanish = 1;
    mPowerupTimer = 0x258;
    func_ov002_020bd9ec(this, 0x33);
    func_ov002_020c43c4(this, 3);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov002_020bdf8c, 0x020bdf8c, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bdf8c
extern "C" {
void func_ov002_020bdf8c(char* c){
    extern void func_ov002_020bda48(void*);
    extern void _ZN6Player17SetNoControlStateEhih(void*, unsigned char a, int b, unsigned char d);
    extern void func_02012790(int a);
    extern void func_ov002_020bd9ec(void*, unsigned int r4);
    extern char* data_0209f318;
    extern void func_ov002_020bdf8c(int);
  func_ov002_020bda48(c);
  *(unsigned char*)(c+0x6f2) = 0;
  if(*(unsigned char*)(c+0x703) == 0){
    _ZN6Player17SetNoControlStateEhih(c, 0xd, -1, 0);
    func_02012790(0x2e);
  }
  *(unsigned char*)(c+0x703) = 1;
  *(unsigned short*)(c+0x600+0xc2) = 0x258;
  (*(unsigned int *)(((int)data_0209f318 + 0x154))) |= 0x80;
  func_ov002_020bd9ec(c, 0x31);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov002_020be008, 0x020be008, size 0xf0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020be008
extern "C" {
void func_ov002_020be008(char* self){
    extern unsigned char data_0209f2d8;
    extern int data_ov002_021104b4;
    extern signed char data_02092114;
    extern int func_ov002_020e3078(void*, void*);
    extern void _ZN6Player18SetNewHatCharacterEjjb(void*, unsigned int a, unsigned int b, int c);
    extern void _ZN6Player17SetNoControlStateEhih(void*, unsigned char a, int b, unsigned char c);
    extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int a2, void*, void*, int a5, int a6);
  int b = (int)(data_0209f2d8 == 1);
  if (b != 0) return;
  if (func_ov002_020e3078(self, &data_ov002_021104b4) == 0) return;
  {
    int r5 = data_02092114;
    if (r5 < 0) return;
    if (r5 == *(int*)(self+8)) return;
    _ZN6Player18SetNewHatCharacterEjjb(self, r5 & 0xff, 0, 0);
    _ZN6Player17SetNoControlStateEhih(self, 8, -1, 0);
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as( 0x10d, (r5 << 8) | 0xa, self+0x5c, 0, *(signed char*)(self+0xcc), -1);
    data_02092114 = -1;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN6Player18SetNewHatCharacterEjjb, 0x020be0f8, size 0xe8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player18SetNewHatCharacterEjjb
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::SetNewHatCharacter(unsigned int p1, unsigned int p2, bool p3)
{
    struct State;
    extern State data_ov002_02110604;
    extern State data_ov002_0211013c;
    extern void func_ov002_020bdb50(void*, unsigned int p2);
    extern void func_ov002_020bda48(void*);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, State& s);
    extern void func_ov002_020d9c70(void*);
    extern void func_ov002_020da95c(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, State& s);
    extern void func_02012790(int n);
    extern void func_ov002_020e6350(void*);
  unsigned int old = param1;
  if (p1 == old) return;
  mPrevCharacter = (unsigned char)old;
  mHatCharacter = (unsigned char)p1;
  *(unsigned short*)((char*)(((int)((char*)this) + 0x700)) + 0x3c) = 1;
  if (p1 != mCharacter) {
    *(unsigned short*)((char*)(((int)((char*)this) + 0x73c))) |= 0x8000;
  }
  func_ov002_020bdb50(((char*)this), p2);
  func_ov002_020bda48(((char*)this));
  if (_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_02110604)) {
    func_ov002_020d9c70(((char*)this));
    func_ov002_020da95c(((char*)this));
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
  }
  if (p3 == 0) {
    if (p1 != mCharacter) func_02012790(0xb);
    else func_02012790(0xc);
  }
  param1 = mHatCharacter;
  func_ov002_020e6350(((char*)this));
  param1 = mPrevCharacter;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN6Player16SetRealCharacterEj, 0x020be1e0, size 0x144 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16SetRealCharacterEj
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::SetRealCharacter(unsigned int chr_)
{
    extern void _ZN6Player18SetNewHatCharacterEjjb(void*, u32 a, u32 b, bool c);
    extern void _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
    extern void _ZN6Player4HealEi(void*, int hp);
    extern u32 _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, bool b);
    extern SharedFilePtr *data_ov002_020ff480[];
    extern u8 data_02092128[];
    extern u8 data_0209caa0[];
    extern void func_ov002_020e6330(void*);
    extern void _ZN10ModelAnim24CopyERKS_Pcj(void*, void*, void*, u32);
    extern void _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(void*, u32, int, int, unsigned short);
    u32 chr = (u32)chr_;

    u32 cur = param1;
    u32 base = mCharFileBase;
    u32 m1, m2;

    ((SharedFilePtr *)(data_ov002_020ff480[base + (cur & 3)]))->Release();
    _ZN6Player18SetNewHatCharacterEjjb(((char *)this), chr, 0, 1);
    param1 = chr;
    mCharacter = (u8)chr;
    data_02092128[mPlayerNo] = (u8)param1;
    data_0209caa0[0x41] = (u8)param1;
    _ZN9Animation8LoadFileER13SharedFilePtr(*data_ov002_020ff480[mCharFileBase + (param1 & 3)]);
    func_ov002_020e6330(((char *)this));
    _ZN6Player4HealEi(((char *)this), 0x880);
    mCapFlags = 0;

    m1 = _ZNK6Player14GetBodyModelIDEjb(((char *)this), chr, 0);
    m2 = _ZNK6Player14GetBodyModelIDEjb(((char *)this), param1 & 0xff, 0);
    _ZN10ModelAnim24CopyERKS_Pcj( *(void **)(((char *)this) + m1 * 4 + 0xdc), *(void **)(((char *)this) + m2 * 4 + 0xdc), *(char **)((char *)data_ov002_020ff480[mCharFileBase + chr] + 4), 0);
    m1 = _ZNK6Player14GetBodyModelIDEjb(((char *)this), chr, 0);
    _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt( *(void **)(((char *)this) + m1 * 4 + 0xdc), *(int *)((char *)data_ov002_020ff480[chr + 0xc4] + 4), 0, 0x1000, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN6Player18TurnOffToonShadingEj, 0x020be324, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player18TurnOffToonShadingEj
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::TurnOffToonShading(unsigned int j)
{
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int, int);
    extern int _ZN5Model14SetPolygonModeEi(void*, int);
    extern int func_ov002_020e6b74(void*, int);
  void* m;
  m = mBodyModels[_ZNK6Player14GetBodyModelIDEjb(((char*)this), j, 0)];
  _ZN5Model14SetPolygonModeEi(m, 0);
  func_ov002_020e6b74(m, unk_27c[j]);
  m = unk_154[j];
  _ZN5Model14SetPolygonModeEi(m, 0);
  func_ov002_020e6b74(m, unk_28c[j]);
  m = unk_154[j + 4];
  _ZN5Model14SetPolygonModeEi(m, 0);
  func_ov002_020e6b74(m, unk_28c[j + 4]);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov002_020be3b0, 0x020be3b0, size 0x6cc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020be3b0
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void func_ov002_020be3b0(char* c)
{
    extern u32 _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, char b);
    extern void _ZN5Model14SetPolygonModeEi(void*, int mode);
    extern int func_ov002_020becf4(void*, int v, int arg2);
    extern void func_ov002_020e6b74(void*, void*);
    extern void func_ov002_020d80d0(void*);
    extern void _ZN9Animation8LoadFileER13SharedFilePtr(void*);
    extern void _ZN13SharedFilePtr7ReleaseEv(void*);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern unsigned int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const struct Vector3* v);
    extern void* data_ov002_020ff480[];
    extern u8 data_02092128[];
    extern int data_ov002_021104b4;
    extern u8 data_ov002_020ff14c[];
    extern void func_ov002_020e6b3c(void*);
    extern void _ZN10ModelAnim24CopyERKS_Pcj(void*, void*, void*, u32);
    extern void _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(void*, u32, int, int, unsigned short);
    extern void*data_ov002_020ff790[];
    extern void func_ov002_020bdb50(void*, int);
    extern int __aeabi_idiv(int, int);
    extern void _ZN6Player18TurnOffToonShadingEj(void*, u32);
    extern char data_ov002_02110804[];
    extern void func_ov002_020e69c0(void*);
    extern void func_ov002_020e6a78(void*, int);
    void* case3_m1;
    void* case3_m2;
    int var_r5;
    int var_r4;
    u16 flags;
    int idx;

    flags = *(u16*)(c + 0x73c);
    var_r5 = 3;
    var_r4 = 0;
    if (flags & 0x8000) {
        var_r5 = 2;
        var_r4 = 5;
    }
    idx = flags & 7;

    switch (idx) {
    case 0:
        return;
    case 1: {
        void* m1 = *(void**)(c + (_ZNK6Player14GetBodyModelIDEjb(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0xdc);
        _ZN5Model14SetPolygonModeEi(m1, 2);
        {
            void* m2 = *(void**)(c + (func_ov002_020becf4(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0x154);
            _ZN5Model14SetPolygonModeEi(m2, 2);
        }
        *(u16*)(c + 0x740) = 0;
        *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4];
        *(u16*)(((long long)(int)(c + 0x73c))) = (u16)(*(u16*)(((long long)(int)(c + 0x73c))) + 1);
        *(u32*)(c + 8) = *(u8*)(c + 0x6dc);
        break;
    }
    case 2:
        *(u16*)(((long long)(int)(c + 0x740))) = (u16)(*(u16*)(((long long)(int)(c + 0x740))) + 0x199);
        if ((u32)*(u16*)(c + 0x740) >= 0x1000) {
            *(u16*)(c + 0x740) = 0x1000;
        }
        if (*(u16*)(c + 0x73e) == 0) {
            *(u16*)(((long long)(int)(c + 0x73c))) = (u16)(*(u16*)(((long long)(int)(c + 0x73c))) + 1);
            *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4 + 1];
        }
        *(u32*)(c + 8) = *(u8*)(c + 0x6dc);
        break;
    case 3: {
        if ((*(u16*)(c + 0x73e) % var_r5) == 0) {
            case3_m1 = *(void**)(c + (_ZNK6Player14GetBodyModelIDEjb(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0xdc);
            case3_m2 = *(void**)(c + (func_ov002_020becf4(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0x154);
            if ((*(u16*)(c + 0x73e) % (var_r5 * 2)) == 0) {
                func_ov002_020e6b74(case3_m1, *(int**)(c + (*(u32*)(c + 8) * 4) + 0x27c));
                func_ov002_020e6b74(case3_m2, *(int**)(c + (func_ov002_020becf4(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0x28c));
            } else {
                func_ov002_020e6b3c(case3_m1);
                func_ov002_020e6b3c(case3_m2);
            }
        }
        if (*(u16*)(c + 0x73e) == 0) {
            void* t1;
            void* t2;
            func_ov002_020e6b3c(case3_m1);
            func_ov002_020e6b3c(case3_m2);
            *(u16*)(((long long)(int)(c + 0x73c))) = (u16)(*(u16*)(((long long)(int)(c + 0x73c))) + 1);
            *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4 + 2];
            t1 = *(void**)(c + (_ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dd), 0) * 4) + 0xdc);
            _ZN5Model14SetPolygonModeEi(t1, 2);
            func_ov002_020e6b3c(t1);
            t2 = *(void**)(c + (func_ov002_020becf4(c, *(u8*)(c + 0x6dd), 0) * 4) + 0x154);
            _ZN5Model14SetPolygonModeEi(t2, 2);
            func_ov002_020e6b3c(t2);
            *(u8*)(c + 0x6eb) = 0;
            *(u8*)(c + 0x6ea) = *(u8*)(c + 0x6eb);
            *(u16*)(c + 0x6ba) = *(u8*)(c + 0x6ea);
            *(u8*)(c + 0x6ec) = 0;
            func_ov002_020d80d0(c);
            *(u8*)(c + 0x6f4) = 0;
            *(u8*)(c + 0x714) = 0;
            _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_020ff480[*(u32*)(c + 0x63c) + *(u8*)(c + 0x6dd)]);
            {
                u32 mm1 = _ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dd), 0);
                _ZN10ModelAnim24CopyERKS_Pcj( *(void**)(c + mm1 * 4 + 0xdc), *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dc), 0) * 4 + 0xdc), *(char**)((char*)data_ov002_020ff480[*(u32*)(c + 0x63c) + *(u8*)(c + 0x6dd)] + 4), 0);
            }
            *(u8*)(((long long)(int)(c + 0x718))) = (u8)(*(u8*)(((long long)(int)(c + 0x718))) | 0x40);
        }
        *(u32*)(c + 8) = *(u8*)(c + 0x6dc);
        break;
    }
    case 4:
        if ((*(u16*)(c + 0x73e) % var_r5) == 0) {
            if ((*(u16*)(c + 0x73e) % (var_r5 * 2)) == 0) {
                *(u32*)(c + 8) = *(u8*)(c + 0x6dc);
            } else {
                *(u32*)(c + 8) = *(u8*)(c + 0x6dd);
            }
        }
        if (*(u16*)(c + 0x73e) == 0) {
            u32 mm;
            *(u16*)(((long long)(int)(c + 0x73c))) = (u16)(*(u16*)(((long long)(int)(c + 0x73c))) + 1);
            *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4 + 3];
            *(u32*)(c + 8) = *(u8*)(c + 0x6dd);
            data_02092128[*(u8*)(c + 0x6d8)] = (u8)(*(u32*)(c + 8));
            if (*(u8*)(c + 0x6dd) == *(u8*)(c + 0x6d9)) {
                *(u8*)(c + 0x71a) = 0;
            }
            func_ov002_020e6b74( *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dd), 0) * 4 + 0xdc), *(int**)(c + *(volatile u8*)(c + 0x6dd) * 4 + 0x27c));
            func_ov002_020e6b74( *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dc), 0) * 4 + 0xdc), *(int**)(c + *(volatile u8*)(c + 0x6dc) * 4 + 0x27c));
            {
                char* p1 = c + func_ov002_020becf4(c, *(u8*)(c + 0x6dd), 0) * 4;
                func_ov002_020e6b74(*(void**)(p1 + 0x154), *(int**)(p1 + 0x28c));
            }
            {
                char* p2 = c + func_ov002_020becf4(c, *(u8*)(c + 0x6dc), 0) * 4;
                func_ov002_020e6b74(*(void**)(p2 + 0x154), *(int**)(p2 + 0x28c));
            }
            mm = *(u32*)(c + 8);
            _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt( *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, (u8)mm, 0) * 4 + 0xdc), *(u32*)((char*)data_ov002_020ff790[mm] + 4), 0, 0x1000, 0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff480[*(u32*)(c + 0x63c) + *(u8*)(c + 0x6dc)]);
            *(u8*)(((long long)(int)(c + 0x718))) = (u8)(*(u8*)(((long long)(int)(c + 0x718))) & ~0x40);
            func_ov002_020bdb50(c, 0);
        } else {
            u8 a, b;
            u32 idxA;
            b = *(u8*)(c + 0x6dc);
            if (b == *(u32*)(c + 8)) {
                a = *(u8*)(c + 0x6dd);
            } else {
                a = b;
                b = *(u8*)(c + 0x6dd);
            }
            idxA = _ZNK6Player14GetBodyModelIDEjb(c, a, 0);
            _ZN10ModelAnim24CopyERKS_Pcj( *(void**)(c + idxA * 4 + 0xdc), *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, b, 0) * 4 + 0xdc), *(char**)((char*)data_ov002_020ff480[*(u32*)(c + 0x63c) + a] + 4), 0);
        }
        break;
    case 5:
        if (*(u16*)(c + 0x73e) == 0) {
            *(u16*)(((long long)(int)(c + 0x73c))) = (u16)(*(u16*)(((long long)(int)(c + 0x73c))) + 1);
            *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4 + 4];
            if (*(u8*)(c + 0x6dd) != *(u8*)(c + 0x6d9) && _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021104b4) == 0) {
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0x21, (struct Vector3*)(c + 0x74));
            }
        }
        break;
    case 6: {
        int divv = data_ov002_020ff14c[var_r4 + 4];
        *(u16*)(((long long)(int)(c + 0x740))) = (u16)(*(u16*)(((long long)(int)(c + 0x740))) - __aeabi_idiv(0x1000, divv));
        if (*(s16*)(c + 0x740) < 0) {
            *(u16*)(c + 0x740) = 0;
        }
        if (*(u16*)(c + 0x73e) == 0) {
            *(u16*)(((long long)(int)(c + 0x73c))) = (u16)(*(u16*)(((long long)(int)(c + 0x73c))) + 1);
        }
        break;
    }
    case 7:
        _ZN6Player18TurnOffToonShadingEj(c, *(u8*)(c + 0x6dd));
        _ZN6Player18TurnOffToonShadingEj(c, *(u8*)(c + 0x6dc));
        *(u16*)(c + 0x73c) = 0;
        break;
    }

    if (*(u16*)(c + 0x73e) != 0) {
        *(u16*)(((long long)(int)(c + 0x73e))) = (u16)(*(u16*)(((long long)(int)(c + 0x73e))) - 1);
    }
    {
        char* dummy = data_ov002_02110804;
        func_ov002_020e69c0(dummy);
        func_ov002_020e6a78(dummy, *(u16*)(c + 0x740));
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov002_020bea7c, 0x020bea7c, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bea7c
extern "C" {
int func_ov002_020bea7c(char* c){
    extern int func_ov002_020bea7c(void*);
  char* p=c+0x700;
  return *(unsigned short*)(p+0x3c)!=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- _ZN6Player15IsCollectingCapEv, 0x020bea94, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15IsCollectingCapEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsCollectingCap()
{
  unsigned short v = mCapFlags;
  if(v!=0 && (v&7)<5) return 1;
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov002_020beabc, 0x020beabc, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020beabc
extern "C" {
void func_ov002_020beabc(Player *self)
{
    extern int func_ov002_020becf4(void*, unsigned int a, int b);
    char *p = (char *)self;
    *(short *)(p + 0x73c) = 5;
    *(short *)(p + 0x73e) = 0x2f;
    *(short *)(p + 0x740) = 0x1000;
    *(unsigned char *)(p + 0x6dc) = *(unsigned char *)(p + 0x6d9);
    *(unsigned char *)(p + 0x6dd) = *(unsigned char *)(p + 0x6dc);
    unsigned int id = self->GetBodyModelID(*(unsigned char *)(p + 0x6d9), 0);
    ((Model *)*(int *)(p + (id << 2) + 0xdc))->SetPolygonMode(2);
    unsigned int id2 = func_ov002_020becf4(self, *(int *)(p + 8) & 0xff, 0);
    ((Model *)*(int *)(p + (id2 << 2) + 0x154))->SetPolygonMode(2);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov002_020beb38, 0x020beb38, size 0x9c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020beb38
extern "C" {
int func_ov002_020beb38(char* c){
    extern unsigned char data_020a0e40;
    extern unsigned short data_0209f49e;
    extern unsigned short data_020a0e5a;
    extern int AngleDiff(int a, int b);
    extern int func_ov002_020beb38(void*);
  unsigned char idx = data_020a0e40;
  unsigned char count = 0;
  unsigned short f = *(unsigned short*)((char*)&data_0209f49e + idx*0x18);
  if (f & 1) count++;
  if (f & 2) count++;
  if (f & 0x400) count++;
  if (f & 0x800) count++;
  if (*(unsigned short*)((char*)&data_020a0e5a + idx*4) & 0xf0) count++;
  if (AngleDiff(*(short*)(c+0x6d4), *(short*)(c+0x6d2)) >= 0x4000) count += 8;
  return count;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov002_020bebd4, 0x020bebd4, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bebd4
extern "C" {
void func_ov002_020bebd4(void){
    extern int data_ov002_020ff480[];
  int i;
  for(i=0x44;i<0x1c8;i+=4){
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i]))->Release();
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i+1]))->Release();
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i+2]))->Release();
    ((SharedFilePtr *)((void*)data_ov002_020ff480[i+3]))->Release();
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov002_020bec2c, 0x020bec2c, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bec2c
extern "C" {
void func_ov002_020bec2c(void){
    extern int data_ov002_020ff480[];
    extern int _ZN9Animation8LoadFileER13SharedFilePtr(void*);
  int i;
  for(i=0x44;i<0x1c8;i+=4){
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov002_020ff480[i]);
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov002_020ff480[i+1]);
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov002_020ff480[i+2]);
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov002_020ff480[i+3]);
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov002_020bec84, 0x020bec84, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bec84
extern "C" {
int func_ov002_020bec84(void* p, unsigned int i) {
    extern int _ZN6Player6IsAnimEj(void*, unsigned int);
    extern unsigned int data_ov002_020ff220[];
    return _ZN6Player6IsAnimEj(p, data_ov002_020ff220[i]);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov002_020bec9c, 0x020bec9c, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bec9c
extern "C" {
int func_ov002_020bec9c(void*c,unsigned int a,int b,int d,unsigned short e){
    extern int data_ov002_020ff220[];
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned short);
  return _ZN6Player7SetAnimEji5Fix12IiEj(c,data_ov002_020ff220[a],b,d,e);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZNK6Player14GetBodyModelIDEjb, 0x020becc8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZNK6Player14GetBodyModelIDEjb
/* recovered: named members + shared header, real C++ method
 *
 * Metal cap overrides whichever body model the caller asked for. Which metal
 * flag is consulted depends on the second argument -- mIsMetal for the player
 * itself, unk_6fa for the other one.
 *
 * const, and the _ZNK in the symbol is what says so: declared without it the
 * compiler emits _ZN..., which names a symbol that does not exist.
 */
extern "C" {
unsigned int Player::GetBodyModelID(unsigned int a, bool b_) const
{
    if (b_ == 0) {
        if (mIsMetal) a = 4;
    } else {
        if (unk_6fa) a = 4;
    }
    return a;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov002_020becf4, 0x020becf4, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020becf4
extern "C" {
int func_ov002_020becf4(char* self, int v, int arg2){
    extern int _ZN6Player8HasNoCapEv(void*);
    int cond;
    int r1;
    if (arg2 == 0) {
        if (*(unsigned char*)(self + 0x6f9)) v = 9;
        if (*(unsigned char*)(self + 0x6fd)) v = 8;
        cond = _ZN6Player8HasNoCapEv(self);
    } else {
        if (*(unsigned char*)(self + 0x6fa)) v = 9;
        if (*(unsigned char*)(self + 0x6fe)) v = 8;
        cond = *(unsigned char*)(self + 0x701);
    }
    r1 = 0;
    if ((unsigned int)v >= 4) {
        if (cond) r1 = 1;
    } else if (v == 3) {
        if (*(unsigned char*)(self + 0x714)) r1 = 4;
    } else {
        if (cond) r1 = 4;
    }
    return (r1 + v) & 0xff;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN6Player12FinishedAnimEv, 0x020bed98, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12FinishedAnimEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::FinishedAnim()
{
    extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int, char);
    extern int _ZN9Animation8FinishedEv(void*);
  unsigned int id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), param1&0xff, 0);
  char* m = *(char**)(((char*)this) + id*4 + 0xdc);
  return _ZN9Animation8FinishedEv(m+0x50) != 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- Player_AdvanceAnims, 0x020bedd4, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol Player_AdvanceAnims
extern "C" {
void Player_AdvanceAnims(char* self){
    extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern void _ZN9Animation7AdvanceEv(void*);
    extern int _ZN6Player6IsAnimEj(void*, unsigned int a);
  unsigned int id = _ZNK6Player14GetBodyModelIDEjb(self, *(int*)(self+8) & 0xff, 0);
  _ZN9Animation7AdvanceEv((char*)*(int*)(self + (id << 2) + 0xdc) + 0x50);
  if (_ZN6Player6IsAnimEj(self, 0x9a) || _ZN6Player6IsAnimEj(self, 0x19) || _ZN6Player6IsAnimEj(self, 0xac) || _ZN6Player6IsAnimEj(self, 0x6b) || _ZN6Player6IsAnimEj(self, 0x85) || _ZN6Player6IsAnimEj(self, 0x94) || _ZN6Player6IsAnimEj(self, 0x98) || _ZN6Player6IsAnimEj(self, 0x99)) {
    *(int*)(self + *(int*)(self+8) * 0x14 + 0x1e4) = 0;
  }
  _ZN9Animation7AdvanceEv(self + 0x1dc + *(int*)(self+8) * 0x14);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- _ZN6Player6IsAnimEj, 0x020beecc, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player6IsAnimEj
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsAnim(unsigned int a)
{
    extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int, char);
    extern int data_ov002_020ff480[];
  int ip = param1;
  int* p = (int*)data_ov002_020ff480[ip + a*4];
  int v = p[1];
  unsigned int id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), ip&0xff, 0);
  int* m = *(int**)(((char*)this) + id*4 + 0xdc);
  int r=(m[24]==v); return r?1:0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- _ZN6Player7SetAnimEji5Fix12IiEj, 0x020bef2c, size 0x210 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player7SetAnimEji5Fix12IiEj
#define LAUNDER16(p) ((u16 *)(int)(p))
#pragma push
#pragma opt_propagation off
extern "C" {
#pragma opt_propagation off
void _ZN6Player7SetAnimEji5Fix12IiEj(char *self, u32 a, int b, Fix12i c, u16 d)
{
    extern void _ZN9Animation8LoadFileER13SharedFilePtr(void*);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, int b);
    extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int b, Fix12i c, u32 d);
    extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int b, Fix12i c, u32 d);
    extern u8 data_0209f2d8;
    extern void *data_ov002_020ff480[];
    extern void *data_ov002_020ff2f0[];
    u32 charId;
    void *sfp;
    u32 sl;
    int field8;
    u32 newVal;
    int id;
    int flag;
    int param2;
    void *modelPtr;

    flag = (data_0209f2d8 == 1);
    if (flag) {
        if ((int)a < 0x11 || (int)a >= 0x72) {
            u16 *f = LAUNDER16(self + 0x6ce);
            *f = (u16)(*f | 0x80);
            return;
        }
    }

    newVal = a << 2;
    if (newVal != *(u32 *)(self + 0x63c)) {
        ((SharedFilePtr *)(data_ov002_020ff480[*(u32 *)(self + 0x63c) + (*(int *)(self + 8) & 3)]))->Release();
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_020ff480[newVal + (*(int *)(self + 8) & 3)]);

        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        modelPtr = ((void **)(self + 0xdc))[id];
        *(int *)((char *)modelPtr + 0x60) = 0;

        if ((*(u16 *)(self + 0x73c) & 7) == 4) {
            charId = *(u8 *)(self + 0x6dd);
            if (charId == *(u32 *)(self + 8))
                charId = *(u8 *)(self + 0x6dc);

            ((SharedFilePtr *)(data_ov002_020ff480[*(u32 *)(self + 0x63c) + charId]))->Release();
            sfp = data_ov002_020ff480[newVal + charId];
            _ZN9Animation8LoadFileER13SharedFilePtr(sfp);

            id = _ZNK6Player14GetBodyModelIDEjb(self, charId & 0xff, 0);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((void **)(self + 0xdc))[id], *(void **)((char *)sfp + 4), b, c, d);
            {
                u16 *g = LAUNDER16(self + 0x73e);
                *g = (u16)(*g + 1);
            }
        }
    }

    field8 = *(int *)(self + 8);
    sl = a * 4;
    id = _ZNK6Player14GetBodyModelIDEjb(self, (u32)(field8 & 0xff), 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj( ((void **)(self + 0xdc))[id], *(void **)((char *)data_ov002_020ff480[(field8 & 3) + (a * 4)] + 4), b, c, d);

    field8 = *(int *)(self + 8);
    if ((int)sl >= 0x60) sl = 0x60;
    sl += (field8 & 3);
    param2 = 0;
    if (((int)sl >> 2) == 1) param2 = 0x40000000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj( self + 0x1dc + field8 * 0x14, *(void **)((char *)data_ov002_020ff2f0[sl] + 4), param2, 0x1000, 0);

    *(u32 *)(self + 0x63c) = newVal;
}
}
#pragma pop
#undef LAUNDER16

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov002_020bf13c, 0x020bf13c, size 0xe8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf13c
/* matched pure C — was HAND-ASM regperm floor; y-early preload cracks r4/r5 */
extern "C" {
void func_ov002_020bf13c(char* c)
{
    typedef struct { int x, y, z; } Vec3b;
    extern short data_02082214[];
    extern void func_ov002_020bf13c(void*);
    volatile int pad[4];
    int vel; int* dst; short s, cs; int ang; int r6, flags;
    Vec3b v;
    (void)&pad;
    if (*(int*)(c + 8) != 3) return;
    dst = *(int**)(c + 0x578);
    if (dst == 0) return;
    vel = *(int*)(c + 0x98);
    ang = *(short*)(c + 0x8e);
    {
        int idx = (unsigned short)(short)(ang + 0x8000) >> 4;
        s = data_02082214[idx * 2];
        cs = data_02082214[idx * 2 + 1];
    }
    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    v.x = (short)s * (short)0x78 + v.x;
    v.z = (short)cs * (short)0x78 + v.z;
    r6 = vel >> 5;
    if (r6 > 0x1000) r6 = 0x1000;
    if (r6 < 0) r6 = 0;
    {
        unsigned char f6de = *(unsigned char*)(c + 0x6de);
        int ip = (r6 + 0x1000) >> 8;
        flags = ip & 0xff;
        if (f6de != 0)
            flags = (flags | 0x80) & 0xff;
        if (vel != 0)
            flags = (flags | 0x40) & 0xff;
        dst[0] = v.x;
        dst[1] = v.y + 0xc000;
        dst[2] = v.z;
        *(unsigned char*)(*(char**)(c + 0x57c)) = (unsigned char)flags;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov002_020bf224, 0x020bf224, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf224
/* func_ov002_020bf224 — scale b by an s16 factor from a 0x18-stride table
 * indexed by byte data_020a0e40 (fx12 rounded multiply), clamped below by c.
 * No callees; data: data_020a0e40 (u8 index), data_0209f4a0 (s16 table).
 */
extern "C" {
int func_ov002_020bf224(int a, int b, int c)
{
    struct Entry {
        s16 val;             /* 0x0 */
        char _pad[0x16];
    };
    extern u8 data_020a0e40;
    extern struct Entry data_0209f4a0[];
    s64 p = (s64)b * data_0209f4a0[data_020a0e40].val;
    b = (int)((p + 0x800) >> 12);
    if (b < c)
        b = c;
    return b;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov002_020bf27c, 0x020bf27c, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf27c
extern "C" {
int func_ov002_020bf27c(char* c, int r1){
  int v=*(int*)(c+8);
  int b;
  if(*(unsigned char*)(c+0x703)!=0) return r1;
  b=(int)(v==3);
  if(b!=0) return r1*0x5a/100;
  if(v==1) r1>>=2;
  return r1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov002_020bf2d8, 0x020bf2d8, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf2d8
extern "C" {
void func_ov002_020bf2d8(void*c, Fix12i a){
    extern unsigned short data_ov002_020ff140[];
  unsigned int t = data_ov002_020ff140[*(int*)((char*)c+8)];
  *(int*)((char*)c+0xa8) = (Fix12i)(((long long)a * (int)t + 0x800) >> 12);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- Player_ScaleByCharFactor, 0x020bf30c, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol Player_ScaleByCharFactor
extern "C" {
Fix12i Player_ScaleByCharFactor(void*c, Fix12i a){
    extern unsigned short data_ov002_020ff170[];
  int idx = *(int*)((char*)c+8) & 0xff;
  unsigned int t = data_ov002_020ff170[idx];
  return (Fix12i)(((long long)a * (int)t + 0x800) >> 12);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov002_020bf340, 0x020bf340, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf340
extern "C" {
void func_ov002_020bf340(int dummy, int*p, int delta, int limit){
  int neg = -limit;
  *p = *p + delta;
  if(*p < neg){ *p = neg; return; }
  if(*p > limit){ *p = limit; }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov002_020bf36c, 0x020bf36c, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf36c
extern "C" {
void func_ov002_020bf36c(char* self, void* arg){
    typedef int Fix12i;
    extern void func_ov002_020bfa74(void*);
    extern void func_ov002_020c0108(void*, int x);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void*, void*);
    extern void func_ov002_020ce798(void*);
    extern void func_ov002_020bf36c(void*, void*);
    Fix12i saved;
    unsigned char f = *(unsigned char*)(self + 0x709);
    if (f == 0)
        func_ov002_020bfa74(self);
    if ((*(unsigned char*)(self + 0x6e9) & 1) || *(unsigned char*)(self + 0x706))
        func_ov002_020c0108(self, 1);
    if (*(int*)(self + 0x37c) != 0)
        return;
    saved = *(Fix12i*)(self + 0x98);
    if (*(unsigned char*)(self + 0x6e9) & 1)
        *(Fix12i*)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i*)(self + 0x558) + 0x800) >> 12);
    _ZN8dActor_c9UpdatePosEP5dCc_c(self, arg);
    *(Fix12i*)(self + 0x98) = saved;
    func_ov002_020ce798(self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov002_020bf40c, 0x020bf40c, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf40c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020bf40c(char* c){
    struct dBgCh_Gnd { char pad[0x50]; };
    extern unsigned char NumStars(void);
    extern void AddVec3(void*, void*, void*);
    extern void _ZN9dBgCh_GndC1Ev(void*);
    extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void*, void*, void*);
    extern int _ZN9dBgCh_Gnd10DetectClsnEv(void*);
    extern void _ZN9dBgCh_GndD1Ev(void*);
    extern void* data_0209f318;
    extern struct Vector3 data_ov002_0210a590;
    extern void func_0200d858(void*, const struct Vector3*);
  struct Vector3 pos;
  struct Vector3 off;
  struct dBgCh_Gnd rg;
  if(NumStars() < 0x50 || *(int*)(c+8) != 0){
    AddVec3((struct Vector3*)(c+0x5c), &data_ov002_0210a590, (struct Vector3*)(c+0x5c));
    off.x = data_ov002_0210a590.x;
    off.y = data_ov002_0210a590.y;
    off.z = data_ov002_0210a590.z;
    func_0200d858(data_0209f318, &off);
    _ZN9dBgCh_GndC1Ev(&rg);
    {
      int py = *(int*)(c+0x60);
      int pz = *(int*)(c+0x64);
      pos.x = *(int*)(c+0x5c);
      pos.y = py + 0xa0000;
      pos.z = pz;
    }
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &pos, c);
    if(_ZN9dBgCh_Gnd10DetectClsnEv(&rg)){
      *(int*)(c+0x644) = *(int*)((char*)&rg + 0x44);
    }
    _ZN9dBgCh_GndD1Ev(&rg);
    return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- _ZN6Player4HealEi, 0x020bf4e4, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player4HealEi
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Heal(int amt)
{
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int data_ov002_0211010c[];
    extern int data_ov002_02110124[];
    extern int GiveHealth(int, int);
  int r = _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_0211010c);
  if(r) return r;
  r = _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_02110124);
  if(r) return r;
  return GiveHealth(mPlayerNo, amt);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- _ZN6Player9GetHealthEv, 0x020bf548, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player9GetHealthEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::GetHealth()
{
    extern short data_02092144[];
  unsigned char i = mPlayerNo;
  return (signed char)(data_02092144[i] >> 8);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- func_ov002_020bf56c, 0x020bf56c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf56c
extern "C" {
int func_ov002_020bf56c(void* a, int b){
    extern int func_ov002_020c031c(void*);
    extern int func_ov002_020bf56c(void*, int);
  int m;
  switch (func_ov002_020c031c(a)) {
  case 4:
  case 5:
    m = 0x199;
    break;
  case 3:
    m = 0x599;
    break;
  default:
  case 0:
    m = 0x1000;
    break;
  case 1:
  case 2:
    m = 0x1000;
    break;
  }
  return (int)(((long long)b * m + 0x800) >> 12);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- func_ov002_020bf5e0, 0x020bf5e0, size 0x220 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf5e0
extern "C" {
void func_ov002_020bf5e0(char *self)
{
    typedef struct {
        int x, y, z;
    } V3b;
    extern int _ZN6Player6IsAnimEj(void*, unsigned int animId);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int frame);
    extern void Vec3_RotateYAndTranslate(void*, void*, short angle, void*);
    extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void*);
    extern u32 func_02022d00(u32 uniqueID, u32 effectID, int x, int y, int z, void*);
    extern int data_0209f32c;
    extern void func_ov002_020bf5e0(void*);
    int flag;
    int id;
    char *anim;
    V3b out;
    V3b src;
    int target;
    int newId;
    int state;

    if (*(u8 *)(self + 0x703) == 0)
        return;

    flag = 0;
    if (_ZN6Player6IsAnimEj(self, 0x9e) || _ZN6Player6IsAnimEj(self, 0xa4)) {
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0xe))
            goto set_flag;
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (!_ZNK9Animation12WillHitFrameEi(anim, 0x1c))
            goto flag_done;
set_flag:
        flag = 1;
    }
flag_done:
    if (!flag)
        return;

    id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 0x1c)) {
        src.x = 0x40000;
    } else {
        src.x = -0x40000;
    }
    src.y = 0x1b000;
    src.z = 0x40000;

    Vec3_RotateYAndTranslate(&out.x, (int *)(self + 0x5c), *(short *)(self + 0x8e), &src.x);

    if (*(u8 *)(self + 0x707) != 0 || *(u8 *)(self + 0x706) != 0) {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x64, out.x, out.y, out.z);
    } else {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x63, out.x, out.y, out.z);
    }

    _ZN5Sound9PlayBank0EjRK7Vector3(0xd0, self + 0x74);

    if (*(u8 *)(self + 0x707) != 0 && *(u8 *)(self + 0x706) == 0) {
        newId = func_02022d00(*(u32 *)(self + 0x628), 0x62, *(int *)(self + 0x5c), data_0209f32c, *(int *)(self + 0x64), 0);
        *(u32 *)(self + 0x628) = newId;
    }

    state = *(int *)(self + 0x66c);
    target = -1;
    if (state == 6)
        goto state_case6;
    if (state == 8)
        goto state_case8;
    if (state == 9) {
        target = 0xe6;
    }
    goto state_merge;
state_case8:
    target = 0xe4;
    goto state_merge;
state_case6:
    target = 0xe5;
state_merge:
    ;

    if (target == -1)
        return;

    src.y = 0;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(target, out.x, out.y, out.z);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- func_ov002_020bf800, 0x020bf800, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf800
extern "C" {
void func_ov002_020bf800(char *c, Vector3_16f v)
{
    typedef unsigned int u32;
    typedef short s16;
    extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int c, int d, int e, void const *f, void*);
    int xv = *(int *)(c + 0x5c) - v.x * 0x2bc;
    int zv = *(int *)(c + 0x64) - v.z * 0x2bc;
    int yv = *(int *)(c + 0x60) - v.y * 0x2bc;
    Fix12v a(xv), b(yv), d(zv);
    *(u32 *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(c + 0x628), 0xe7, *(int *)&a, *(int *)&b, *(int *)&d, &v, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- func_ov002_020bf88c, 0x020bf88c, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf88c
extern "C" {
void func_ov002_020bf88c(char* c){
    extern void func_02022b04(int x, int y, int z);
    extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);
    extern int data_0208e430;
    extern void func_ov002_020bf88c(void*);
    if (*(unsigned char*)(c + 0x707)) return;
    if (data_0208e430 == 0x2e) return;
    if (*(unsigned char*)(c + 0x703))
        func_02022b04(*(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    else
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 50 -- func_ov002_020bf90c, 0x020bf90c, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf90c
extern "C" {
void func_ov002_020bf90c(char* c)
{
    extern void func_02022b04(Fix12i x, Fix12i y, Fix12i z);
    extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(Fix12i x, Fix12i y, Fix12i z);
    extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5);
    extern int data_0208e430;
    if (*(int*)(c + 0x98) == 0) return;
    if (*(u8*)(c + 0x6de) != 0) return;
    if (*(u8*)(c + 0x707) != 0) return;
    if (data_0208e430 == 0x2e) return;

    if (*(u8*)(c + 0x703) != 0)
        func_02022b04(*(Fix12i*)(c + 0x5c), *(Fix12i*)(c + 0x60), *(Fix12i*)(c + 0x64));
    else
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(Fix12i*)(c + 0x5c), *(Fix12i*)(c + 0x60), *(Fix12i*)(c + 0x64));

    *(int*)(c + 0x620) = func_0201226c(
        *(int*)(c + 0x620),
        0,
        *(int*)(c + 0x66c) + 0xe2,
        (int)(c + 0x74),
        *(int*)(c + 0x98),
        0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 51 -- func_ov002_020bf9d4, 0x020bf9d4, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bf9d4
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
// func_ov002_020bf9d4 at 0x020bf9d4
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
void func_ov002_020bf9d4(char* c){
    extern int func_0200fccc(void*, int r1);
    extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
    extern int data_0208e430;
  Vector3 v;
  if (data_0208e430 == 0x2e) return;
  if (*(unsigned char*)(c + 0x703) == 0){
    func_0200fccc(c, 0);
    return;
  }
  ((int*)&v)[0] = *(int*)(c + 0x5c);
  ((int*)&v)[1] = *(int*)(c + 0x60);
  ((int*)&v)[2] = *(int*)(c + 0x64);
  if (*(unsigned char*)(c + 0x707) != 0){
    v.y += 0x19000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5c, v.x, v.y, v.z);
  } else {
    v.y += 0x23000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5b, v.x, v.y, v.z);
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 52 -- func_ov002_020bfa74, 0x020bfa74, size 0x44c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bfa74
/* Player on a CLPS surface of type 0xb (a current/conveyor tile): the tile's
 * CLPS word names a two-node path whose direction is the push vector. Spawns the
 * flow particle (func_ov002_020bf800), keeps the surface loop sound alive, and
 * unless the player is metal, uncontrolled or in a blocked state, pushes the
 * player along the path: grounded states get a position nudge scaled by
 * data_ov002_020ff210[def->unk_004], airborne states get their horizontal
 * velocity bent toward the path direction. Returns 1 when the push applied. */
#define FX_MUL(a, b) ((s32)(((s64)(a) * (s64)(b) + 0x800) >> 12))
#define SIN(a) data_02082214[((u16)(a) >> 4) * 2]
#define COS(a) data_02082214[((u16)(a) >> 4) * 2 + 1]
extern "C" {
int func_ov002_020bfa74(Player *self)
{
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern unsigned int func_0203ad54(void*);
    extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
    extern s16 Vec3_VertAngle(const Vector3 *v0, const Vector3 *v1);
    extern s32 Vec3_HorzLen(const Vector3 *v);
    extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
    extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 bank, u32 id, const void *pos, s16 e);
    extern void func_ov002_020bf800(void*, Vector3_16f v);
    extern int func_ov002_020e0478(void*);
    extern s16 data_02082214[];
    extern s32 data_ov002_020ff210[];
    extern int data_ov002_0211013c[];
    extern int data_ov002_02110574[];
    extern int data_ov002_02110184[];
    extern int data_ov002_02110514[];
    extern int data_ov002_021104e4[];
    extern int data_ov002_02110364[];
    extern int data_ov002_0211064c[];
    extern int data_ov002_021101b4[];
    extern int data_ov002_021102d4[];
    s32 n;
    s32 i;
    u32 idx;
    s16 horz;
    s16 vert;
    s32 k;
    s32 scale;

    if (self->mSurfaceType != 0xb) return 0;
    if (self->mClpsWord1Lo != 0xff) {
        PathPtr path;
        Vector3_16f dir;
        Vector3 nodes[2];
        Vector3 vel;
        path.FromID(self->mClpsWord1Lo);
        n = path.NumNodes();
        for (i = 0; i < n; i++) {
            path.GetNode(nodes[i], i);
        }
        idx = (u8)func_0203ad54(&path);
        if (idx >= 4) idx = 1;
        horz = Vec3_HorzAngle(&nodes[0], &nodes[1]);
        vert = Vec3_VertAngle(&nodes[0], &nodes[1]);
        dir.y = SIN(vert);
        dir.x = FX_MUL(COS(vert), SIN(horz));
        dir.z = FX_MUL(COS(vert), COS(horz));
        func_ov002_020bf800(self, dir);
        self->mSurfaceSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(self->mSurfaceSoundHandle, 3, 0x90, &self->mCamSpacePosX, 0);
        if (self->mIsMetal != 0 || self->mIsNoControl != 0 || func_ov002_020e0478(self) != 0) return 0;
        if (self->mIsAirborne == 0) {
            if (_ZN6Player7IsStateERNS_5StateE(self, data_ov002_0211013c) || _ZN6Player7IsStateERNS_5StateE(self, &Player::ST_WAIT) || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_02110574) || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_02110184) || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_02110514) || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_021104e4) || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_02110364) || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_0211064c)) {
                if (self->mHorzSpeed <= 0) {
                    k = 0x8000;
                } else {
                    scale = data_ov002_020ff210[idx];
                    k = FX_MUL(scale, self->mHorzSpeed);
                }
                self->mPosX += FX_MUL(k, dir.x);
                self->mPosZ += FX_MUL(k, dir.z);
            }
        } else {
            if (_ZN6Player7IsStateERNS_5StateE(self, data_ov002_021101b4) || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_021102d4)) return 1;
            vel.x = FX_MUL(self->mHorzSpeed, SIN(self->mPrevAngleY));
            vel.x += FX_MUL(dir.x, 0x2800);
            vel.z = FX_MUL(self->mHorzSpeed, COS(self->mPrevAngleY));
            vel.z += FX_MUL(dir.z, 0x2800);
            self->mPrevAngleY = _ZN4cstd5atan2E5Fix12IiES1_(vel.x, vel.z);
            self->mHorzSpeed = Vec3_HorzLen(&vel);
        }
        return 1;
    }
    return 0;
}
}
#undef FX_MUL
#undef SIN
#undef COS

/* -------------------------------------------------------------------------- */
/* ROM ordinal 53 -- func_ov002_020bfec0, 0x020bfec0, size 0x248 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bfec0
extern "C" {
int func_ov002_020bfec0(char* self)
{
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int _ZN6Player7IsInAirEv(void*);
    extern int func_ov002_020e3078(void*, void*);
    extern int func_ov002_020c5dec(void*, int arg1);
    extern char data_ov002_02110124;
    extern char data_ov002_02110154;
    extern char data_ov002_0211049c;
    extern char data_ov002_02110424;
    extern char data_ov002_021104cc;
    int state = *(int*)(self + 0x664);
    int inc;

    if (state == 1) {
        if (*(u8*)(self + 0x6f9) != 0) state = 8;
    }

    if (state < 6 || state > 9) {
        if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110124)) {
            *(int*)(self + 0x68c) = 0;
        }
        return 0;
    }

    if (*(u8*)(self + 0x6de) != 0 || _ZN6Player7IsInAirEv(self) || *(int*)(self + 0x37c) != 0 || *(u8*)(self + 0x703) != 0 || *(u8*)(self + 0x709) != 0) {
        return 0;
    }

    *(u8*)(self + 0x6eb) |= 1;

    {
        int flag = *(int*)(self + 0x354);
        flag = (flag != 0);
        if (flag) goto reset_timer;
        if (*(u16*)(self + 0x6bc) == 0) goto after_timer_check;
    reset_timer:
        *(int*)(self + 0x68c) = 0;
        return 0;
    after_timer_check:
        ;
    }

    inc = 0x400;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110154) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211049c)) {
        inc = 0x800;
    }

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110424)) {
        if (func_ov002_020e3078(self, &data_ov002_021104cc) == 0) {
            *(u16*)(self + 0x6c4) = 2;
        }
    }

    if (*(int*)(self + 0x68c) < 0x1100) *(int*)(self + 0x68c) = 0x1100;

    switch (state) {
    case 6:
        *(int*)(self + 0x68c) += inc;
        if (*(int*)(self + 0x68c) >= 0xa000) *(int*)(self + 0x68c) = 0xa000;
        goto ret0;
    case 7:
        *(int*)(self + 0x68c) += inc;
        if (*(int*)(self + 0x68c) >= 0x3c000) *(int*)(self + 0x68c) = 0x3c000;
        goto ret0;
    case 8:
        *(int*)(self + 0x68c) += inc;
        if (*(int*)(self + 0x68c) < 0xa0000) goto ret0;
        /* fallthrough */
    case 9:
        func_ov002_020c5dec(self, 3);
        return 1;
    default:
        *(int*)(self + 0x68c) = 0;
        goto ret0;
    }
ret0:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 54 -- func_ov002_020c0108, 0x020c0108, size 0x214 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c0108
#define AT(p, off) ((void *)(int)((char *)(p) + (off)))
extern "C" {
int func_ov002_020c0108(char *self, int p1)
{
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef short s16;
    typedef unsigned int u32;
    typedef int s32;
    typedef struct { s32 x, y, z; } Vec3i;
    typedef struct { void *a; u32 b; } PathPtr;
    extern void _ZN7PathPtrC1Ev(void*);
    extern void _ZN7PathPtr6FromIDEj(void*, u32 id);
    extern int _ZNK7PathPtr8NumNodesEv(void*);
    extern void _ZNK7PathPtr7GetNodeER7Vector3j(void*, void*, u32 idx);
    extern u32 func_0203ad54(void*);
    extern int Vec3_HorzAngle(const Vec3i *v0, const Vec3i *v1);
    extern int Vec3_VertAngle(const Vec3i *v1, const Vec3i *v0);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern char data_ov002_02110304;
    extern char data_ov002_0211025c;
    extern s32 data_ov002_020ff200[];
    extern s16 data_02082214[];
    extern void func_ov002_020c0108(void*, int);
    if (*(u8*)(self + 0x6f9) != 0 || (*(u8*)(self + 0x709) != 0 && p1 != 0) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110304) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211025c))
        return 0;

    if (*(s32*)(self + 0x664) == 0xb)
        return 0;

    if (*(u32*)(self + 0x670) != 0xff) {
        s32 s;
        s32 iX, iY;
        s32 w;
        int idx;
        int numNodes;
        int i;
        int aH, aV;
        Vec3i nodes[3];
        PathPtr path;

        _ZN7PathPtrC1Ev(&path);
        _ZN7PathPtr6FromIDEj(&path, *(u32*)(self + 0x670));
        numNodes = _ZNK7PathPtr8NumNodesEv(&path);

        for (i = 0; i < numNodes; i++) {
            _ZNK7PathPtr7GetNodeER7Vector3j(&path, &nodes[i], i);
        }

        idx = func_0203ad54((unsigned char **)&path) & 0xff;
        if ((u32)idx >= 4)
            idx = 1;

        aH = Vec3_HorzAngle(&nodes[0], &nodes[1]);
        aV = Vec3_VertAngle(&nodes[0], &nodes[1]);

        iY = (u16)aV >> 4;
        iX = (u16)aH >> 4;

        w = data_ov002_020ff200[idx] << 12;

        *(s32 *)AT(self, 0x60) += (s32)(((long long)w * data_02082214[iY * 2] + 0x800) >> 12);
        s = (s32)(((long long)w * data_02082214[iY * 2 + 1] + 0x800) >> 12);
        *(s32 *)AT(self, 0x5c) += (s32)(((long long)s * data_02082214[iX * 2] + 0x800) >> 12);
        *(s32 *)AT(self, 0x64) += (s32)(((long long)s * data_02082214[iX * 2 + 1] + 0x800) >> 12);

        return 1;
    }

    return 0;
}
}
#undef AT

/* -------------------------------------------------------------------------- */
/* ROM ordinal 55 -- func_ov002_020c031c, 0x020c031c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c031c
extern "C" {
int func_ov002_020c031c(void*c){
    extern int data_ov002_02110514[];
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
  int r4 = *(int*)((char*)c+0x658);
  if(_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110514) && *(int*)((char*)c+0x558) > 0x800){
    if(r4==0) r4=2;
  }
  return r4;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 56 -- func_ov002_020c0364, 0x020c0364, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c0364
extern "C" {
void func_ov002_020c0364(char* c, u32 arg)
{
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int AngleDiff(int a, int b);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 a, int b, Fix12i c, u32 d);
    extern char data_ov002_02110334;
    extern char data_ov002_0211031c;
    int flag;

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110334)) return;
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211031c)) return;

    flag = 0;
    if (arg == 3) {
        if (AngleDiff(*(s16*)(c + 0x69a), *(s16*)(c + 0x8e)) < 0x4000)
            flag = 1;
    } else {
        if (arg & 1)
            flag = 1;
    }

    if (flag) {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110334);
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211031c);
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x43, 0x40000000, 0x1000, 0);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 57 -- func_ov002_020c0434, 0x020c0434, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c0434
extern "C" {
int func_ov002_020c0434(char* c){
    extern int func_ov002_020c04ac(void*);
  unsigned short f = *(unsigned short*)(c+0x600+0xce) & 1;
  if(f){
    if(*(int*)(c+0x658)==5) return 1;
    if(*(int*)(c+0x98)<0) return 1;
    if(func_ov002_020c04ac(c)) return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 58 -- func_ov002_020c04ac, 0x020c04ac, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c04ac
extern "C" {
int func_ov002_020c04ac(char*c){
    extern int AngleDiff(short a, short b);
  return AngleDiff(*(short*)(c+0x69a), *(short*)(c+0x8e)) <= 0x4000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 59 -- func_ov002_020c04e0, 0x020c04e0, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c04e0
extern "C" {
int func_ov002_020c04e0(char *c)
{
    extern int Vec3_HorzLen(void*);
    extern int AngleDiff(int a, int b);
    extern int func_ov002_020c031c(void*);
    extern int func_ov002_020f035c(u32 sel, int r1);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern char data_ov002_0211010c;
    extern char data_ov002_02110304;
    extern int func_ov002_020c04e0(void*);
    int horzLen;
    int angDiff;
    int sel;
    int accelBase;
    int speed;
    int accel;
    int ret;

    horzLen = Vec3_HorzLen(c + 0x554);
    angDiff = AngleDiff(*(s16 *)(c + 0x69a), *(s16 *)(c + 0x94));

    if (*(u16 *)(c + 0x6b8) != 0) {
        if (*(int *)(c + 0x658) == 0)
            return *(int *)(c + 0x658);
    }

    sel = func_ov002_020c031c(c);
    ret = func_ov002_020f035c(sel, *(int *)(c + 0x558));
    if (ret == 0)
        return ret;

    sel = 0;
    if (!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211010c))
        sel = func_ov002_020c031c(c);
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110304)) {
        if (sel == 4)
            sel = 0;
    }

    switch (sel) {
    case 4:
    case 5:
        accelBase = 0x54cc;
        break;
    case 3:
        accelBase = 0x2b33;
        break;
    case 0:
    default:
        accelBase = 0x1b33;
        break;
    case 1:
    case 2:
        accelBase = 0;
        break;
    }

    speed = *(int *)(c + 0x98) - 0x13000;
    if (speed < 0x1000)
        speed = 0x1000;
    if (speed >= 0x1800)
        speed = 0x1800;

    accel = (int)(((long long)accelBase * speed + 0x800) >> 12);

    if (angDiff < 0x4000) {
        int *p = (int *)(((int)(c + 0x98)));
        int v = *p + (int)(((long long)accel * horzLen + 0x800) >> 12);
        *p = v;
        return v;
    } else {
        int *p = (int *)(((int)(c + 0x98)));
        int v = *p - (int)(((long long)accel * horzLen + 0x800) >> 12);
        *p = v;
        return v;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 60 -- func_ov002_020c0688, 0x020c0688, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c0688
extern "C" {
int func_ov002_020c0688(char* c){
  if (*(unsigned char*)(c+0x6de) == 0) return 1;
  if (*(unsigned char*)(c+0x70e) != 0) {
    int d = *(int*)(c+0x60) - *(int*)(c+0x644);
    if (d < 0xb4000 && d >= 0) {
      *(int*)(c+0x60) = *(int*)(c+0x644);
      return 1;
    }
  } else if (*(unsigned char*)(c+0x6e4) != 0) {
    int d = *(int*)(c+0x60) - *(int*)(c+0x644);
    if (d < 0x28000) {
      *(int*)(c+0x60) = *(int*)(c+0x644);
      return 1;
    }
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 61 -- func_ov002_020c06fc, 0x020c06fc, size 0x5c0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c06fc
#define FX(a, k) ((int)(((long long)(a) * (k) + 0x800) >> 12))
extern "C" {
int func_ov002_020c06fc(char *c, int arg)
{
    struct Vec3 { s32 x, y, z; };
    extern int func_ov002_020c031c(void*);
    extern int func_ov002_020f030c(unsigned int sel);
    extern int func_ov002_020f02c8(unsigned int sel);
    extern int func_ov002_020f035c(unsigned int sel, int r1);
    extern int Vec3_HorzLen(void*);
    extern int _ZN4cstd5atan2E5Fix12IiES1_(int x, int z);
    extern int AngleDiff(int a, int b);
    extern void _ZN9dBgCh_LinC1Ev(void*);
    extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void*, void*, void*, void*);
    extern int _ZN9dBgCh_Lin10DetectClsnEv(void*);
    extern void _ZN9dBgCh_LinD1Ev(void*);
    extern void ApproachAngle(void*, short target, int divisor, int band, int maxStep);
    extern unsigned char data_020a0e40;
    extern short data_0209f4a0[];
    extern char data_0209f4ac[];
    extern short data_02082214[];
    struct Frame {
        struct Vec3 v0;
        struct Vec3 v1;
        struct Vec3 v2;
        struct Vec3 v3;
        char ray[0x78];
    } f;
    int a;
    int t;
    int b;
    int v;
    int spd;
    int r5v;
    int r4v;
    int ang;

    if (*(u8 *)(c + 0x6de) != 0)
        return 0;
    if ((*(int *)(c + 0x558) | *(int *)(c + 0x98)) == 0)
        return 0;

    a = 0;
    b = a;
    if (*(s16 *)((char *)data_0209f4a0 + data_020a0e40 * 0x18) >= 0x200) {
        s16 d = (s16)(*(s16 *)(c + 0x6d2) - *(s16 *)(c + 0x94));
        int j = ((u16)d >> 4) * 2;
        a = data_02082214[j + 1];
        b = data_02082214[j];
    }

    t = func_ov002_020f030c(func_ov002_020c031c(c));
    t = t + FX(*(s16 *)((char *)data_0209f4a0 + data_020a0e40 * 0x18), FX(a, 0x52LL));

    v = func_ov002_020f02c8(func_ov002_020c031c(c));
    {
        int j = (*(u16 *)(c + 0x94) >> 4) * 2;
        spd = *(int *)(c + 0x98);
        r5v = FX(spd, (int)data_02082214[j]);
        r4v = FX(spd, (int)data_02082214[j + 1]);
    }

    if (*(u8 *)(c + 0x70e) != 0) {
        if (*(u8 *)(data_0209f4ac + data_020a0e40 * 0x18) != 0) {
            int m = (b < 0) ? -b : b;
            s16 s2;
            if (m < 0x100)
                b = 0;
            s2 = *(s16 *)((char *)data_0209f4a0 + data_020a0e40 * 0x18);
            if (s2 < 0x400)
                *(u16 *)(c + 0x69c) = FX(b, 0x64LL);
            else if (s2 < 0x800)
                *(u16 *)(c + 0x69c) = FX(b, 0xC8LL);
            else if (s2 < 0xC00)
                *(u16 *)(c + 0x69c) = FX(b, 0x190LL);
            else
                *(u16 *)(c + 0x69c) = FX(b, 0x300LL);
        } else {
            *(u16 *)(c + 0x69c) = FX(b, 0x220LL);
        }
    } else {
        *(u16 *)(c + 0x69c) = (int)((((long long)b << 8) + 0x800) >> 12);
    }

    f.v0.x = FX(v, (int)data_02082214[(*(u16 *)(c + 0x69a) >> 4) * 2]);
    f.v0.z = FX(v, (int)data_02082214[(*(u16 *)(c + 0x69a) >> 4) * 2 + 1]);
    {
        int len = Vec3_HorzLen(c + 0x554);
        f.v0.x = FX(f.v0.x, len);
        f.v0.z = FX(f.v0.z, len);
    }

    if ((*(u8 *)(c + 0x6e9) & 2) != 0 && (u16)(*(u16 *)(c + 0x6ce) & 1) == 0 && *(int *)(c + 0x98) <= 0x10000) {
        int r6b = _ZN4cstd5atan2E5Fix12IiES1_(f.v0.x, f.v0.z);
        _ZN9dBgCh_LinC1Ev(f.ray);
        {
            int tz, ty, tx;
            int p5c = *(int *)(c + 0x5c);
            int p64 = *(int *)(c + 0x64);
            int j = ((u16)r6b >> 4) * 2;
            int p60 = *(int *)(c + 0x60);
            struct Vec3 t;
            t.x = p5c; t.y = p60 + 0x32000; t.z = p64;
            f.v1.x = t.x; f.v1.y = t.y; f.v1.z = t.z;
            tx = data_02082214[j] * 0x46 + *(int *)(c + 0x5c);
            tz = data_02082214[j + 1] * 0x46 + *(int *)(c + 0x64);
            ty = *(int *)(c + 0x60) + 0x32000;
            f.v2.x = tx; f.v2.y = ty; f.v2.z = tz;
        }
        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(f.ray, &f.v1, &f.v2, c);
        if (_ZN9dBgCh_Lin10DetectClsnEv(f.ray) != 0) {
            f.v0.x = 0;
            f.v0.z = 0;
        }
        _ZN9dBgCh_LinD1Ev(f.ray);
    }

    {
        int s1 = r5v + f.v0.x;
        int s2 = r4v + f.v0.z;
        int fx = FX(s1, t);
        int fz = FX(s2, t);
        f.v3.x = fx;
        f.v3.z = fz;
        ang = _ZN4cstd5atan2E5Fix12IiES1_(fx, fz);
    }
    *(int *)(c + 0x98) = Vec3_HorzLen(&f.v3);
    if (*(int *)(c + 0x98) > 0x64000)
        *(int *)(c + 0x98) = 0x64000;
    *(s16 *)(c + 0x94) = (s16)(ang + *(s16 *)(c + 0x69c));
    {
        s16 cur = *(s16 *)(c + 0x94);
        if (AngleDiff(cur, *(s16 *)(c + 0x8e)) >= 0x4000)
            cur = (s16)(*(s16 *)(c + 0x94) + 0x8000);
        ApproachAngle((s16 *)(c + 0x8e), cur, 8, 0x4000, 0x10);
    }
    {
        int s = *(int *)(c + 0x98);
        if (s < 0)
            s = -s;
        if (s < arg) {
            if (func_ov002_020f035c(func_ov002_020c031c(c), *(int *)(c + 0x558)) == 0) {
                *(int *)(c + 0x98) = 0;
                return 0;
            }
            if (*(u8 *)(c + 0x70e) != 0)
                *(int *)(c + 0x98) = arg;
        }
    }
    return 1;
}
}
#undef FX

/* -------------------------------------------------------------------------- */
/* ROM ordinal 62 -- func_ov002_020c0cbc, 0x020c0cbc, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c0cbc
extern "C" {
int func_ov002_020c0cbc(char* c){
    extern int func_ov002_020c031c(void*);
    extern int func_ov002_020c0cbc(void*);
  int t;
  int d;
  if((*(unsigned char*)(c+0x6e9) & 1) == 0) return 0;
  d = *(int*)(c+0x644) - *(int*)(c+0x60);
  if(d < 0) d = -d;
  if(d >= 0x50000) return 0;
  switch(func_ov002_020c031c(c)){
    case 4: t = 0xfc1; break;
    case 5: t = 0xfff; break;
    case 3: t = 0xf04; break;
    default:
    case 0: t = 0xc9b; break;
    case 1:
    case 2: t = 0; break;
  }
  if(*(int*)(c+0x558) <= t){
    (*(unsigned short *)(((int)c + 0x6ce))) |= 1;
    return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 63 -- func_ov002_020c0d90, 0x020c0d90, size 0x224 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c0d90
extern "C" {
int func_ov002_020c0d90(char* c, int y)
{
    extern u32 func_02022a4c(Fix12i x, Fix12i y, Fix12i z);
    extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void*);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020c7f10(void*);
    extern void func_ov002_020ce9c8(void*);
    extern void func_ov002_020be008(void*);
    extern int data_0209f32c;
    extern int data_ov002_021102d4;
    extern int data_ov002_021106ac;
    extern int data_ov002_021106c4;
    extern int data_ov002_021102a4;
    extern int data_ov002_0211067c;
    int flag = (*(u8*)(c + 0x707) != 0);

    *(u8*)(c + 0x707) = 0;
    if (y == (int)0x80000000) return 0;

    if (*(u8*)(c + 0x706) != 0) flag = 1;
    if (y >= *(int*)(c + 0x60) + 0xa000) {
        *(u8*)(c + 0x707) = 1;
    }

    if (flag != (*(u8*)(c + 0x707) != 0)) {
        if (*(u8*)(c + 0x6de) != 0) {
            if (*(u8*)(c + 0x703) == 0) {
                func_02022a4c(*(int*)(c + 0x5c), y, *(int*)(c + 0x64));
            } else {
                *(int*)(c + 0x628) = func_02022d00(*(int*)(c + 0x628), 0x56, *(int*)(c + 0x5c), data_0209f32c, *(int*)(c + 0x64), 0);
            }
        }
    }

    if (((y - 0x64000) >> 12) < (*(int*)(c + 0x60) >> 12)) return 0;
    if (*(int*)(c + 0xa8) > 0) return 0;

    if (*(u8*)(c + 0x6fd) != 0 || (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102d4) && *(u8*)(c + 0x6e3) < 2) || *(u8*)(c + 0x706) != 0 || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021106ac) || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021106c4) || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102a4)) {
        return 0;
    }

    if (func_ov002_020c7f10(c)) return 1;

    *(u16*)(c + 0x6b8) = 0xf;
    if (*(u8*)(c + 0x6f9) != 0 || *(u8*)(c + 0x703) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021106ac);
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
        *(u16*)(c + 0x6a8) = 0x14;
    }

    func_ov002_020ce9c8(c);
    func_ov002_020be008(c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 64 -- func_ov002_020c0fb4, 0x020c0fb4, size 0x280 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c0fb4
extern "C" {
int func_ov002_020c0fb4(char *c)
{
    typedef struct Vec3i
    {
      int x;
      int y;
      int z;
    } Vec3i;
    extern int func_ov002_020c5d0c(void*);
    extern int func_ov002_020dec70(void*);
    extern int func_ov002_020bf40c(void*);
    extern void _ZN5Timer10ResetTimerEv(void*);
    extern void _ZN5Timer10StartTimerEv(void*);
    extern void _ZN5Timer9StopTimerEv(void*);
    extern unsigned long long _ZN5Timer7GetTimeEv(void*);
    extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, const Vec3i *, const void *, int, int);
    extern int func_ov002_020d5338(void*);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
    extern int func_ov002_020bfec0(void*);
    extern int func_ov002_020c0d90(void*, int y);
    extern int func_ov002_020d06c0(void*);
    extern int func_ov002_020c0cbc(void*);
    extern unsigned char data_ov002_02111184[];
    extern unsigned char data_0209d4c8[];
    extern signed char data_0209f2f8;
  int r;
  switch (*((int *) (c + 0x664)))
  {
    case 4:

    case 5:
      if ((r = func_ov002_020c5d0c(c)) != 0)
    {
      return r;
    }
      break;

    case 19:
      if ((r = func_ov002_020dec70(c)) != 0)
    {
      return r;
    }
      if ((r = func_ov002_020c5d0c(c)) != 0)
    {
      return r;
    }
      break;

    case 18:
      if ((r = func_ov002_020bf40c(c)) != 0)
    {
      return r;
    }
      break;

    case 15:
      if (data_ov002_02111184[0] != 0)
    {
      break;
    }
      _ZN5Timer10ResetTimerEv(data_0209d4c8);
      data_ov002_02111184[0] = 1;
      _ZN5Timer10StartTimerEv(data_0209d4c8);
      break;

    case 16:
      if (data_0209d4c8[8] == 0)
    {
      break;
    }
      _ZN5Timer9StopTimerEv(data_0209d4c8);
    {
      unsigned long long t = _ZN5Timer7GetTimeEv(data_0209d4c8);
      if (((t << 6) / 0x1ff6210) > 0x14)
      {
        break;
      }
    }
    {
      Vec3i pos;
      int px;
      int py;
      int pz;
      py = *((int *) (c + 0x60));
      pz = *((int *) (c + 0x64));
      px = *((int *) (c + 0x5c));
      py = (py + 0x82000) & 0xFFFFFFFFFFFFFFFF;
      pos.x = px;
      pos.y = py;
      pos.z = pz;
      _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, 0x42, &pos, 0, *((signed char *) (c + 0xcc)), -1);
    }
      break;

  }

  if (data_0209f2f8 == 0x26)
  {
    if ((*((int *) (c + 0x60))) < (-0x3e8000))
    {
      *((int *) (c + 0x60)) = -0x3e8000;
      if ((r = func_ov002_020d5338(c)) != 0)
      {
        return r;
      }
    }
  }
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x380))
  {
    if ((*((int *) (c + 0x668))) == 1)
    {
      int d = (*((int *) (c + 0x60))) - (*((int *) (c + 0x648)));
      if (d < 0)
      {
        d = -d;
      }
      if (d < 0x2000)
      {
        if ((r = func_ov002_020d5338(c)) != 0)
        {
          return r;
        }
      }
    }
    if ((r = func_ov002_020bfec0(c)) != 0)
    {
      return r;
    }
  }
  if ((r = func_ov002_020c0d90(c, *((int *) (c + 0x64c)))) != 0)
  {
    return r;
  }
  if ((r = func_ov002_020d06c0(c)) != 0)
  {
    return r;
  }
  return func_ov002_020c0cbc(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 65 -- func_ov002_020c1234, 0x020c1234, size 0x108 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c1234
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
void func_ov002_020c1234(char* self) {
    extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int, unsigned int, unsigned int, int, bool);
    extern signed char data_0209f2f8;
    extern int data_0209b49c;
    extern void func_ov002_020c1234(void*);
    unsigned int a1, a2;
    int fx;
    if (data_0209f2f8 != 0xc && data_0209f2f8 != 0x2e) return;
    switch (*(int*)(self + 0x66c)) {
    case 0xa:
        a1 = 0x2d;
        a2 = 0x28;
        fx = (data_0209b49c >> 12 > 0x28) ? 0x8b33 : 0xd1e;
        *(unsigned char*)(self + 0x71c) = 1;
        break;
    case 0xb:
        *(unsigned char*)(self + 0x71c) = 1;
        a1 = 0;
        a2 = 0x7f;
        fx = 0x8b33;
        break;
    default:
        fx = 0x1451;
        a1 = 0x7f;
        a2 = 0;
        break;
    }
    if (*(unsigned char*)(self + 0x71c) == 0) return;
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x35, a1, a2, fx, 0) == 0) return;
    if (*(int*)(self + 0x66c) == 0xa) return;
    if (*(int*)(self + 0x66c) != 0xb) *(unsigned char*)(self + 0x71c) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 66 -- func_ov002_020c133c, 0x020c133c, size 0x17c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c133c
extern "C" {
void func_ov002_020c133c(char *a)
{
    typedef unsigned char u8;
    typedef struct Vec3i { int x, y, z; } Vec3i;
    extern int func_ov002_020d82f0(void*);
    extern int _ZN6Player9GetHealthEv(void*);
    extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, Vec3i const *v, unsigned int d);
    extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void*, Vec3i const *v, unsigned int b, int c, unsigned int d, unsigned int e, unsigned int f);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, Vec3i const *v);
    extern int func_ov002_020d91e0(void*, int damage, int doPre);
    extern int func_ov002_020c5dec(void*, int r1);
    if (*(u8 *)(a + 0x6f9) != 0) return;
    {
        int lim = *(int *)(a + 0x650);
        if (lim == (int)0x80000000) goto set0;
        if (*(int *)(a + 0x60) >= lim - 0x64000) goto set0;
    }
    if (!func_ov002_020d82f0(a)) return;
    if (*(u8 *)(a + 0x6de) != 0) return;
    if (_ZN6Player9GetHealthEv(a) <= 2)
        *(unsigned int *)(a + 0x624) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int *)(a + 0x624), 2, 0x27, (Vec3i *)(a + 0x74), 0);
    if (*(u8 *)(a + 0x71d) != 0) {
        u8 *p = (u8 *)(a + 0x71d);
        *p = *p - 1;
        return;
    }
    {
        Vec3i v;
        v.x = *(int *)(a + 0x5c);
        v.y = *(int *)(a + 0x60);
        v.z = *(int *)(a + 0x64);
        if (!_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, 0, 0, 1, 0, 0)) return;
    }
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(a + 0x6d9), 6, (Vec3i *)(a + 0x74));
    if (func_ov002_020d91e0(a, 0x100, 0) != 0 && _ZN6Player9GetHealthEv(a) == 0)
        func_ov002_020c5dec(a, 5);
    *(u8 *)(a + 0x71d) = 0x40;
    return;
set0:
    *(u8 *)(a + 0x71d) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 67 -- func_ov002_020c14b8, 0x020c14b8, size 0x234 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c14b8
extern "C" {
void func_ov002_020c14b8(void *arg0)
{
    typedef struct { int a, b; } Pair2i;
    typedef struct {
        void *tag;
        int f04, f08, f0c, f10, f14;
        unsigned short f18, f1a;
        int f1c, f20, f24;
    } ClsnResultTmp;
    typedef struct { int x, y, z; } Vector3i;
    extern void _ZN9dBgCh_GndC1Ev(void*);
    extern int _ZN6Player12Unk_020c9e5cEh(void*, unsigned char state);
    extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void*, void*, void*);
    extern int _ZN9dBgCh_Gnd10DetectClsnEv(void*);
    extern void func_ov002_020c16ec(void*, void*);
    extern void _ZN5dBgPiD1Ev(void*);
    extern void _ZN5dBgCh19StartDetectingWaterEv(void*);
    extern void _ZN5dBgCh19StartDetectingToxicEv(void*);
    extern void _ZN5dBgCh21StopDetectingOrdinaryEv(void*);
    extern int SurfaceInfo_TestFlag0x20(void*);
    extern int func_02037e20(void*);
    extern void func_ov002_020c133c(void*);
    extern void _ZN9dBgCh_GndD1Ev(void*);
    extern int data_02099368;
    extern int data_0209f32c;
    extern void func_ov002_020c14b8(void*);
    char *c = (char *)arg0;
    char rg[0x50];
    Vector3i pos;
    ClsnResultTmp tmp;
    int savedY;

    _ZN9dBgCh_GndC1Ev(rg);

    savedY = 0x80000000;
    if (_ZN6Player12Unk_020c9e5cEh(c, 7) || _ZN6Player12Unk_020c9e5cEh(c, 0xa) || _ZN6Player12Unk_020c9e5cEh(c, 0xb) || _ZN6Player12Unk_020c9e5cEh(c, 0xe))
        savedY = *(int *)(c + 0x60);
    *(int *)(c + 0x648) = 0x80000000;
    *(int *)(c + 0x644) = *(int *)(c + 0x648);
    *(int *)(c + 0x64c) = 0x80000000;
    {
        int t = *(int *)(c + 0x60) + 0xa0000;
        if (t >= *(int *)(c + 0x650))
            *(int *)(c + 0x650) = 0x80000000;
    }
    *(int *)(c + 0x668) = 0;
    *(int *)(c + 0x664) = *(int *)(c + 0x668);
    *(int *)(c + 0x658) = 0;
    {
        int z = *(int *)(c + 0x64);
        int y = *(int *)(c + 0x60) + 0xa0000;
        int x = *(int *)(c + 0x5c);
        pos.x = x;
        pos.y = y;
        pos.z = z;
    }
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, &pos, c);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(rg)) {
        int *dst = &tmp.f04;
        { struct P2w { int w[2]; }; *(P2w *)dst = *(P2w *)(rg + 0x14); }
        dst[2] = *(int *)(rg + 0x1c);
        dst[3] = *(int *)(rg + 0x20);
        dst[4] = *(int *)(rg + 0x24);
        tmp.tag = &data_02099368;
        tmp.f18 = *(unsigned short *)(rg + 0x28);
        tmp.f1a = *(unsigned short *)(rg + 0x2a);
        tmp.f1c = *(int *)(rg + 0x2c);
        tmp.f20 = *(int *)(rg + 0x30);
        tmp.f24 = *(int *)(rg + 0x34);
        func_ov002_020c16ec(c, &tmp);
        _ZN5dBgPiD1Ev(&tmp);
        *(int *)(c + 0x648) = *(int *)(rg + 0x44);
        *(int *)(c + 0x644) = *(int *)(c + 0x648);
        *(int *)(c + 0x668) = *(int *)(c + 0x664);
    }

    pos.y = *(int *)(c + 0x60) + 0x12c000;
    _ZN5dBgCh19StartDetectingWaterEv(rg);
    _ZN5dBgCh19StartDetectingToxicEv(rg);
    _ZN5dBgCh21StopDetectingOrdinaryEv(rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, &pos, c);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(rg)) {
        if (SurfaceInfo_TestFlag0x20((int *)(rg + 0x14)) != 0) {
            *(int *)(c + 0x64c) = *(int *)(rg + 0x44);
            data_0209f32c = *(int *)(c + 0x64c);
        }
        if (func_02037e20((int *)(rg + 0x14)) != 0) {
            *(int *)(c + 0x650) = *(int *)(rg + 0x44);
        }
    }

    *(unsigned short *)(c + 0x6ce) &= ~1;
    *(unsigned short *)(c + 0x6ce) &= ~0x10;
    func_ov002_020c133c(c);

    if (savedY != (int)0x80000000)
        *(int *)(c + 0x644) = savedY;

    _ZN9dBgCh_GndD1Ev(rg);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 68 -- func_ov002_020c16ec, 0x020c16ec, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c16ec
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void func_ov002_020c16ec(char *o, char *s)
{
    struct Obj16ec {
        char pad554[0x554];
        int f554;   // 0x554
        int f558;   // 0x558
        int f55c;   // 0x55c
        char pad_to658[0x658 - 0x560];
        int f658;   // 0x658
        int f65c;   // 0x65c
        int f660;   // 0x660
        int f664;   // 0x664
        int f66c;   // 0x66c
        int f670;   // 0x670
    };
    extern int func_02037e38(void*);
    extern int func_02037e58(void*);
    extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void*, void*);
    extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
    extern int func_02037e90(void*);
    extern int func_02037e48(void*);
    extern int func_02037e68(void*);
    extern int func_02037e84(void*);
    struct Vector3 v;
    *(int *)(o + 0x670) = func_02037e90((int *)(s + 4));
    *(int *)(o + 0x664) = func_02037e38((unsigned int *)(s + 4));
    *(int *)(o + 0x65c) = func_02037e48((unsigned int *)(s + 4));
    *(int *)(o + 0x660) = func_02037e68((unsigned int *)(s + 4));
    *(int *)(o + 0x66c) = func_02037e84((int *)(s + 4));
    if (*(int *)(o + 0x658) == 1)
        return;
    *(int *)(o + 0x658) = func_02037e58((unsigned int *)(s + 4));
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((struct SurfaceInfo *)(s + 4), &v);
    *(int *)(o + 0x554) = v.x;
    *(int *)(o + 0x558) = v.y;
    *(int *)(o + 0x55c) = v.z;
    *(short *)(o + 0x69a) = _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(o + 0x554), *(int *)(o + 0x55c));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 69 -- func_ov002_020c179c, 0x020c179c, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c179c
/* recovered: shared common types */
// func_ov002_020c179c at 0x020c179c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020c179c(char* self, int angle) {
    extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
    extern short data_02082214[];
    Vector3 v1;
    Vector3 v2;
    int idx = (unsigned short)(short)(*(short*)(self + 0x94) + angle);
    int y1 = *(int*)(self + 0x60);
    int y2 = y1;
    idx >>= 4;
    short dx = data_02082214[idx * 2];
    short dy = data_02082214[idx * 2 + 1];
    int ox = dx * 5;
    int oy = dy * 5;
    dBgCh_Gnd rg;
    {
        int z = *(int*)(self + 0x64) + oy;
        int y = *(int*)(self + 0x60) + 0x64000;
        int x = *(int*)(self + 0x5c) + ox;
        v1.x = x;
        v1.y = y;
        v1.z = z;
    }
    rg.SetObjAndPos(v1, (dActor_c*)self);
    if (rg.DetectClsn()) y1 = rg.clsnY;
    {
        int z = *(int*)(self + 0x64) - oy;
        int x = *(int*)(self + 0x5c) - ox;
        int y = *(int*)(self + 0x60) + 0x64000;
        v2.x = x;
        v2.y = y;
        v2.z = z;
    }
    rg.SetObjAndPos(v2, (dActor_c*)self);
    if (rg.DetectClsn()) y2 = rg.clsnY;
    {
        int r = _ZN4cstd5atan2E5Fix12IiES1_(y1 - y2, 0xa000);
        return r;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 70 -- func_ov002_020c18b0, 0x020c18b0, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c18b0
extern "C" {
void func_ov002_020c18b0(char *self, int p1)
{
    typedef struct { int x, y, z; } Vec3i;
    extern int func_ov002_020c179c(void*, int angle);
    extern int func_02010844(void*, void*, short angle);
    extern int Player_ScaleByCharFactor(void*, int a);
    extern int __aeabi_idiv(int a, int b);
    extern short data_02082214[];
    if (*(unsigned char *)(self + 0x6de) != 0) return;
    if (*(int *)(self + 0x558) == 0) return;
    {
        int a = func_ov002_020c179c(self, 0);
        int idx = (unsigned short)a >> 4;
        int sinv = data_02082214[idx * 2 + 1];
        int f554 = *(int *)(self + 0x554);
        int f55c = *(int *)(self + 0x55c);
        Vec3i v;
        v.x = f554;
        v.y = sinv;
        v.z = f55c;
        *(short *)(self + 0x8c) = (short)func_02010844(self, &v, *(short *)(self + 0x8e));
        *(short *)(self + 0x90) = (short)func_02010844(self, &v, (short)(*(short *)(self + 0x8e) - 0x4000));
    }
    if (p1 == 0) return;
    {
        int r0v = Player_ScaleByCharFactor(self, 0x28000);
        int divr = __aeabi_idiv(*(int *)(self + 0x98) << 12, r0v);
        *(short *)(self + 0x8c) = (short)(int)(((long long)*(short *)(self + 0x8c) * divr + 0x800) >> 12);
        *(short *)(self + 0x90) = (short)(int)(((long long)*(short *)(self + 0x90) * divr + 0x800) >> 12);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 71 -- func_ov002_020c19d0, 0x020c19d0, size 0x108 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c19d0
extern "C" {
int func_ov002_020c19d0(char* self, int arg1, int arg2) {
    extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void*, void*);
    extern s16 data_02082214[];
    V3a v1;
    V3a v2;
    V3a nrm;
    int angle = *(unsigned short*)(self + 0x8e);
    dBgCh_Lin line;
    int k = angle >> 4;
    int cs = data_02082214[k * 2];
    int sn = data_02082214[k * 2 + 1];
    v1.set(*(int*)(self + 0x5c), *(int*)(self + 0x60) + (arg2 << 12), *(int*)(self + 0x64));
    v2.set(arg1 * cs + *(int*)(self + 0x5c), *(int*)(self + 0x60) + (arg2 << 12), arg1 * sn + *(int*)(self + 0x64));
    line.SetObjAndLine(*(Vector3*)&v1, *(Vector3*)&v2, (dActor_c*)self);
    if (!line.DetectClsn()) {
        return 0;
    }
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(&line.surface, &nrm);
    if (nrm.y != 0) {
        return 0;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 72 -- func_ov002_020c1ad8, 0x020c1ad8, size 0x1ac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c1ad8
/* func_ov002_020c1ad8 at 0x020c1ad8 (ov002)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
extern "C" {
int func_ov002_020c1ad8(char* self, int ang)
{
    extern int func_ov002_020c0434(void*);
    extern void func_ov002_020c0364(void*, unsigned int arg);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int func_ov002_020c19d0(void*, int arg1, int arg2);
    extern int AngleDiff(int a, int b);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int data_ov002_0211013c;
    extern int data_ov002_0211043c;
    extern u8 data_020a0e40;
    extern s16 data_0209f4a0[];
    int d;
    int a = ang;

    if (*(u8*)(self + 0x6de) != 0) return 0;
    if (func_ov002_020c0434(self) != 0) {
        func_ov002_020c0364(self, 3);
        return 0;
    }
    if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c)) {
        if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c)) return 0;
    }
    if (func_ov002_020c19d0(self, 0x50, 0x32) == 0) return 0;

    a = (int)((long long)(a + 0x8000));
    d = AngleDiff(*(s16*)(self + 0x6d2), (s16)a);
    if (d < 0x4000) {
        if (d < 0x1555) {
            if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c)) {
                if (*(u8*)(self + 0x6e3) == 2) return 0;
            }
            if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) == 0) return 0;
            *(u8*)(self + 0x6e3) = 2;
        } else {
            if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c)) {
                if (*(u8*)(self + 0x6e3) != 2) return 0;
            }
            *(u8*)(self + 0x6e3) = 0;
            if ((s16)a == (s16)(*(s16*)(self + 0x8e) + d)) {
                *(u8*)(self + 0x6e3) = 1;
            }
        }
        *(u16*)(self + 0x8e) = a;
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211043c);
        return 1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 73 -- func_ov002_020c1c84, 0x020c1c84, size 0x1c0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c1c84
extern "C" {
int func_ov002_020c1c84(char* self, int arg)
{
    extern int func_ov002_020cf700(void*);
    extern int func_ov002_020e0478(void*);
    extern int AngleDiff(int a, int b);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020c19d0(void*, int a, int b);
    extern char data_ov002_02110694;
    extern char data_ov002_021103ac;
    extern char data_ov002_021101b4;
    int a;
    int p;

    if ((u16)(*(u16*)(self + 0x6ce) & 0x200) || !*(u8*)(self + 0x6de) || func_ov002_020cf700(self) || *(u8*)(self + 0x703) || func_ov002_020e0478(self) || *(s32*)(self + 0xd0) || *(u8*)(self + 0x706))
        return 0;

    if (AngleDiff(arg, *(s16*)(self + 0x94)) < 0x6000)
        return 0;

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110694) || *(u8*)(self + 0x708) || *(u8*)(self + 0x709))
        return 0;

    if (*(s32*)(self + 0x98) < 0x3000)
        return 0;

    if (*(s32*)(self + 0xa8) > 0)
        return 0;

    a = arg + 0x8000;
    p = *(s16*)(self + 0x94);
    *(s16*)(self + 0x69c) = (s16)(p - (s16)(p - a) * 2);
    *(s16*)(self + 0x8e) = (s16)a;
    *(s16*)(self + 0x94) = *(s16*)(self + 0x8e);

    if (*(s32*)(self + 8) == 0) {
        if (func_ov002_020c19d0(self, 0x3c, 0x78))
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021103ac);
    } else {
        if (func_ov002_020c19d0(self, 0x64, 0x96)) {
            *(s32*)(self + 0x98) = -0x12000;
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021101b4);
        } else {
            *(s32*)(self + 0x98) = 0;
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 74 -- func_ov002_020c1e44, 0x020c1e44, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c1e44
extern "C" {
int func_ov002_020c1e44(void* a, int x) {
  *(short*)((char*)a+0x94) = (short)((x << 1) - *(short*)((char*)a+0x94) + 0x8000);
  *(int*)((char*)a+0x98) = (int)(((long long)*(int*)((char*)a+0x98) * 0xe00 + 0x800) >> 12);
  if (*(int*)((char*)a+0x98) < 0x4000) *(int*)((char*)a+0x98) = 0x4000;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 75 -- func_ov002_020c1eb4, 0x020c1eb4, size 0x158 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c1eb4
/* recovered: shared common types */
#define LAUNDER(p) ((unsigned char *)(int)(p))
extern "C" {
void func_ov002_020c1eb4(Obj1eb4 *self, int dmg)
{
    typedef int Fx12;
    extern void Vec3_RotateYAndTranslate(void*, const Vector3 *origin, int angle, const Vector3 *offset);
    extern short data_02082214[];
    extern Player::State data_ov002_02110094;
    extern int func_ov002_020d91e0(void*, int damage, int doPre);
    extern void func_ov002_020c1eb4(void*, int);
    Vector3 out;
    Vector3 offset;
    Vector3_16 vec;

    Sound::PlayBank0(0x13, self->p74);
    self->p8e = self->p94;
    self->p90 = 0;
    self->p8c = self->p90;
    self->b6e3 = 0;
    if (self->b6de) {
        unsigned char *q = LAUNDER((char *)self + 0x6e3);
        *q = *q + 2;
    }
    self->p8e = (short)(dmg * 2 - self->p8e);
    {
        unsigned char *p = LAUNDER((char *)self + 0x6e3);
        self->p94 = (short)(self->p8e + 0x8000);
        self->p98 = 0xa000;
        *p = *p | 0x20;
    }
    self->p674 = 0;
    ((Player *)self)->ChangeState(data_ov002_02110094);

    offset.x = 0;
    offset.y = 0x32000;
    offset.z = 0x28000;
    Vec3_RotateYAndTranslate(&out, &self->p5c, (short)(dmg + 0x8000), &offset);

    {
        int idx = ((unsigned short)dmg >> 4);
        vec.x = data_02082214[idx * 2];
        vec.z = data_02082214[idx * 2 + 1];
        vec.y = 0;
        Particle::_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(0, 0xc8, out.x, out.y, out.z, &vec, 0);
    }
    func_ov002_020d91e0((char *)self, 0, 1);
}
}
#undef LAUNDER

/* -------------------------------------------------------------------------- */
/* ROM ordinal 76 -- func_ov002_020c200c, 0x020c200c, size 0x12c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c200c
extern "C" {
int func_ov002_020c200c(char* self, int arg)
{
    extern int func_ov002_020c1e44(void*, int x);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int _ZN6Player22IsBeingShotOutOfCannonEv(void*);
    extern void func_ov002_020eedc0(void*, void*);
    extern void func_ov002_020c1eb4(void*, int arg);
    extern char data_ov002_021105bc;
    extern char data_ov002_0211031c;
    extern char data_ov002_02110334;
    extern char data_ov002_0211055c;
    extern char data_ov002_02110214;
    if (*(unsigned char*)(self + 0x70e) == 1)
        return func_ov002_020c1e44(self, arg);

    if (*(unsigned char*)(self + 0x709))
        return 0;

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021105bc) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211031c) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110334) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211055c) || _ZN6Player22IsBeingShotOutOfCannonEv(self) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110214))
    {
        if (*(int*)(self + 0x98) <= 0x10000)
            return 0;

        if (_ZN6Player22IsBeingShotOutOfCannonEv(self))
            func_ov002_020eedc0(self + 0x380, self);
    }
    else
    {
        return 0;
    }

    func_ov002_020c1eb4(self, arg);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 77 -- func_ov002_020c2138, 0x020c2138, size 0x138 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c2138
// func_ov002_020c2138 at 0x020c2138
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020c2138(char* self){
    typedef int Fix12i;
    extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12i, Fix12i);
    extern int AngleDiff(int a, int b);
    extern int func_ov002_020c200c(void*, int ang);
    extern int func_ov002_020c1c84(void*, int ang);
    extern int func_ov002_020c1ad8(void*, int ang);
int ang;
if (*(unsigned char*)(self + 0x706) || *(unsigned char*)(self + 0x6fd)) return 0;
if (*(int*)(self + 0x564)) return 0;
if (*(int*)(self + 0x37c) || *(unsigned char*)(self + 0x708)) return 0;
ang = _ZN4cstd5atan2E5Fix12IiES1_(*(int*)(self + 0x560), *(int*)(self + 0x568));
if (AngleDiff(ang, *(short*)(self + 0x94)) < 0x6000) return 0;
if (func_ov002_020c200c(self, ang)) return 1;
if ((*(int*)(self+0x358) ? 1 : 0) || (*(int*)(self+0x354) ? 1 : 0)) return 0;
if (func_ov002_020c1c84(self, ang)) return 1;
return func_ov002_020c1ad8(self, ang);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 78 -- func_ov002_020c2270, 0x020c2270, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c2270
/* recovered: shared common types */
extern "C" {
int func_ov002_020c2270(void* c, void* p1, void* p2) {
    extern int func_02037e58(void*);
    extern void _ZN9dBgCh_Lin10GetClsnPosEv(void*, void*);
    extern int func_ov002_020c2270(void*, void*, void*);
    dBgCh_Lin line;
    Vector3 clsnPos;
    line.SetObjAndLine(*(Vector3*)p1, *(Vector3*)p2, (dActor_c*)c);
    if (line.DetectClsn()) {
        if (func_02037e58((unsigned int*)&line.surface) == 1) {
            _ZN9dBgCh_Lin10GetClsnPosEv(&clsnPos, &line);
            *(int*)((char*)c + 0x5c) = clsnPos.x;
            *(int*)((char*)c + 0x60) = clsnPos.y;
            *(int*)((char*)c + 0x64) = clsnPos.z;
            *((unsigned char*)c + 0x70f) = 1;
            return 1;
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 79 -- func_ov002_020c231c, 0x020c231c, size 0x28c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c231c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020c231c(char *self)
{
    extern void SubVec3(void*, void*, void*);
    extern void AddVec3(void*, void*, void*);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int _ZN6Player6IsAnimEj(void*, u32 id);
    extern char data_ov002_0211013c;
    extern char data_ov002_02110304;
    extern char data_ov002_02110364;
    extern char data_ov002_021104e4;
    extern char data_ov002_02110514;
    extern char data_ov002_021100dc;
    extern s16 data_02082214[];
    extern void func_ov002_020c14b8(void*);
    extern char data_ov002_02110184;
    extern int func_ov002_020c2270(void*, void*, void*);
    struct Vector3 tmp;
    struct Vector3 pos;
    u8 f70f;
    s32 p60;
    s32 d;
    s32 p644;
    s32 xx;
    s32 zz;
    int idx;

    f70f = *(u8 *)(self + 0x70f);

    if (*(s32 *)(self + 0x644) == (s32)0x80000000) {
        if (f70f == 0)
            return 0;

        SubVec3((struct Vector3 *)(self + 0x5c), (struct Vector3 *)(self + 0x548), &tmp);
        tmp.x >>= 1;
        tmp.y >>= 1;
        tmp.z >>= 1;
        AddVec3((struct Vector3 *)(self + 0x548), &tmp, (struct Vector3 *)(self + 0x5c));
        func_ov002_020c14b8(self);

        if (*(s32 *)(self + 0x644) == (s32)0x80000000)
            return 0;
    }

    if (*(s32 *)(self + 0x64c) != (s32)0x80000000)
        return 0;

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110184) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110304) || (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110364) && (_ZN6Player6IsAnimEj(self, 0x33) | _ZN6Player6IsAnimEj(self, 0x87)) == 0) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021104e4) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110514) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021100dc))
    {
        if (*(s32 *)(self + 0x658) == 1 && *(s32 *)(self + 0x558) < 0xfff) {
            p644 = *(s32 *)(self + 0x644);
            p60 = *(s32 *)(self + 0x60);
            d = p60 - p644;
            if (d < 0) d = -d;
            if (d < 0xc8000) {
                *(s32 *)(self + 0x60) = p644;
                *(u8 *)(self + 0x70f) = 1;
                return 1;
            }

            if (f70f == 0) {
                idx = (u16)(s16)(*(s16 *)(self + 0x94) + 0x8000) >> 4;
                zz = data_02082214[idx * 2 + 1] * 0x50 + *(s32 *)(self + 0x64);
                xx = data_02082214[idx * 2] * 0x50 + *(s32 *)(self + 0x5c);
                pos.x = xx;
                pos.y = p60;
                pos.z = zz;
                if (func_ov002_020c2270(self, (struct Vector3 *)(self + 0x5c), &pos))
                    return 1;
            }
        }

        if (f70f == 1) {
            d = *(s32 *)(self + 0x60) - *(s32 *)(self + 0x644);
            if (d < 0) d = -d;
            if (d < 0xc8000)
                *(s32 *)(self + 0x60) = *(s32 *)(self + 0x644);
        }
    }

    *(u8 *)(self + 0x70f) = 0;
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 80 -- func_ov002_020c25a8, 0x020c25a8, size 0x42c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c25a8
extern "C" {
int func_ov002_020c25a8(void *arg0, int arg1)
{
    typedef struct {
        int x, y, z;
    } Vector3i;
    typedef struct {
        int a, b;
    } Pair2i;
    typedef struct {
        void *tag;
        int f04;
        int f08;
        int f0c;
        int f10;
        int f14;
        unsigned short f18;
        unsigned short f1a;
        int f1c;
        int f20;
        int f24;
    } ClsnResultTmp;
    extern void func_02037318(void*);
    extern void _ZN10dBgCh_Actr18StopDetectingWaterEv(void*);
    extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void*);
    extern void func_020357e0(void*);
    extern void func_02035800(void*);
    extern void func_02035770(void*);
    extern void func_02035784(void*);
    extern void func_020357a0(void*);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern void func_020357c0(void*);
    extern void func_020355e8(void*);
    extern void func_020355fc(void*);
    extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void*);
    extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void*);
    extern void func_020355c8(void*);
    extern void func_020355b4(void*);
    extern void func_020383e4(void*);
    extern void func_ov002_020eed24(void*, void*);
    extern void _ZN10dBgCh_Actr15ClearGroundFlagEv(void*);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
    extern int _ZNK10dBgCh_Actr8IsOnWallEv(void*);
    extern int func_02035638(void*);
    extern void func_020371fc(void*);
    extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void*);
    extern void func_ov002_020c16ec(void*, void*);
    extern void _ZN5dBgPiD1Ev(void*);
    extern int func_02037e58(void*);
    extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void*, void*);
    extern int _ZN4cstd4fdivEii(int a, int b);
    extern int func_020356c8(void*);
    extern void func_ov002_020cef84(void*);
    extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void*);
    extern int func_ov002_020d0580(void*);
    extern int func_ov002_020c2138(void*);
    extern char data_ov002_02110514;
    extern char data_ov002_021100f4;
    extern char data_ov002_021102ec;
    extern int data_02099368;
    char *c = (char*)arg0;
    int ret;
    Vector3i v;
    Vector3i wn;
    ClsnResultTmp tmp;
    char *fr;
    int *dst;

    func_02037318(c + 0x380);
    _ZN10dBgCh_Actr18StopDetectingWaterEv(c + 0x380);

    {
        int b1;
        b1 = *(int*)(c + 0x354);
        b1 = (b1 != 0);
        if (b1 || *(unsigned char*)(c + 0x6ec) || *(unsigned char*)(c + 0x6fd))
            _ZN10dBgCh_Actr19StartDetectingWaterEv(c + 0x380);
    }

    func_020357e0(c + 0x380);

    if (*(unsigned char*)(c + 0x6fb))
        func_02035800(c + 0x380);

    func_02035770(c + 0x380);

    if (*(unsigned char*)(c + 0x703))
        func_02035784(c + 0x380);

    func_020357a0(c + 0x380);

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110514))
        func_020357c0(c + 0x380);

    func_020355e8(c + 0x380);

    {
        int b2;
        b2 = *(int*)(c + 0x354);
        b2 = (b2 != 0);
        if (b2)
            func_020355fc(c + 0x380);
    }

    _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x380);

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021100f4) && *(int*)(c + 0xa8) > 0)
        _ZN10dBgCh_Actr13SetLimMovFlagEv(c + 0x380);

    func_020355c8(c + 0x380);

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102ec))
        func_020355b4(c + 0x380);

    func_020383e4(c + 0x380);

    if (*(unsigned char*)(c + 0x703))
        func_ov002_020eed24(c + 0x380, c);

    if (*(int*)(c + 0xa8) > 0x1e000)
        _ZN10dBgCh_Actr15ClearGroundFlagEv(c + 0x380);

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x380))
        *(unsigned char*)(c + 0x6e9) |= 1;

    if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x380))
        *(unsigned char*)(c + 0x6e9) |= 2;

    if (func_02035638(c + 0x380))
        *(unsigned char*)(c + 0x6e9) |= 4;

    if (*(unsigned char*)(c + 0x706) == 0)
        func_020371fc(c + 0x380);

    if (*(unsigned char*)(c + 0x6e9) & 1) {
        fr = (char*)_ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x380);
        dst = &tmp.f04;

        { struct P2w { int w[2]; }; *(P2w *)dst = *(P2w *)(fr + 0x4); }
        dst[2] = *(int*)(fr + 0xc);
        dst[3] = *(int*)(fr + 0x10);
        dst[4] = *(int*)(fr + 0x14);
        tmp.tag = &data_02099368;
        tmp.f18 = *(unsigned short*)(fr + 0x18);
        tmp.f1a = *(unsigned short*)(fr + 0x1a);
        tmp.f1c = *(int*)(fr + 0x1c);
        tmp.f20 = *(int*)(fr + 0x20);
        tmp.f24 = *(int*)(fr + 0x24);

        func_ov002_020c16ec(c, &tmp);
        _ZN5dBgPiD1Ev(&tmp);

        *(int*)(c + 0x644) = *(int*)(c + 0x60);
        ret = func_02037e58(fr + 4);

        if (*(unsigned char*)(c + 0x706) == 0 && ret != 1) {
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, &v);
            if (v.y != 0) {
                int t = _ZN4cstd4fdivEii( (int)(((long long)v.x * *(int*)(c + 0xa4) + 0x800) >> 12) + (int)(((long long)v.z * *(int*)(c + 0xac) + 0x800) >> 12), v.y);
                t = -(t + 0x8000);
                if (t < 0)
                    *(int*)(c + 0xa8) = t;
            }
        }
        *(unsigned char*)(c + 0x6de) = 0;
        *(unsigned char*)(c + 0x712) = 0;
    } else {
        if (func_020356c8(c + 0x380))
            *(int*)(c + 0x684) = *(int*)(c + 0x60);
        *(unsigned char*)(c + 0x6de) = 1;
        *(unsigned char*)(c + 0x6df) = 0;
        if (func_02035638(c + 0x380) && *(int*)(c + 0xa8) > 0)
            *(int*)(c + 0xa8) = -0x1000;
        func_ov002_020cef84(c);
    }

    if (arg1 != 0) {
        *(unsigned char*)(c + 0x6e9) |= 1;
        *(unsigned char*)(c + 0x6de) = 0;
        *(unsigned char*)(c + 0x712) = 0;
    }

    {
        int wallFlag = *(unsigned char*)(c + 0x6e9) & 2;
        if (wallFlag == 0)
            return wallFlag;
    }

    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(c + 0x380) + 4, &wn);
    *(int*)(c + 0x560) = wn.x;
    *(int*)(c + 0x564) = wn.y;
    *(int*)(c + 0x568) = wn.z;

    if (*(unsigned char*)(c + 0x706) == 0) {
        *(int*)(c + 0x5c) -= *(int*)(c + 0x560) * 2;
        *(int*)(c + 0x64) -= *(int*)(c + 0x568) * 2;
    }

    ret = func_ov002_020d0580(c);
    if (ret != 0)
        return ret;

    ret = func_ov002_020c2138(c);
    if (ret != 0)
        return ret;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 81 -- func_ov002_020c29d4, 0x020c29d4, size 0x134 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c29d4
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
void func_ov002_020c29d4(Player *self)
{
    struct State {};
    extern int _ZN6Player7IsStateERNS_5StateE(void*, State &s);
    extern int Player_ScaleByCharFactor(void*, int a);
    extern int func_02037e38(void*);
    extern dBgPi *_ZNK10dBgCh_Actr14GetFloorResultEv(const dBgCh_Actr *self);
    extern State data_ov002_0211013c;
    extern State data_ov002_021101b4;
    extern void func_ov002_020c29d4(void*);
    char *base = (char *)self;
    if (*(int *)(base + 8) != 1) return;
    if (*(unsigned char *)(base + 0x703) != 0) return;
    unsigned char flags = *(unsigned char *)(base + 0x6eb);
    if ((flags & 1) && !(flags & 0x80)) {
        if (*(int *)(base + 0x98) >= Player_ScaleByCharFactor(self, 0x28000)) {
            if (_ZN6Player7IsStateERNS_5StateE(self, data_ov002_0211013c)) {
                *(short *)(base + 0x6bc) = 0x1e;
            } else if (_ZN6Player7IsStateERNS_5StateE(self, data_ov002_021101b4) && *(int *)(base + 0xa8) < 0 && (*(int *)(base + 0x684) - *(int *)(base + 0x60)) < 0x64000) {
                *(short *)(base + 0x6bc) = 0x1e;
            }
        }
        *(unsigned char *)(base + 0x6eb) = 0x80;
    } else if (flags & 0x80) {
        if (((dBgCh_Actr *)(base + 0x380))->IsOnGround()) {
            int r = func_02037e38((unsigned int *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv((dBgCh_Actr *)(base + 0x380)) + 4));
            if (r >= 6 && r <= 9) return;
            *(unsigned char *)(base + 0x6eb) = 0;
            *(short *)(base + 0x6bc) = 0;
        } else {
            *(unsigned char *)(base + 0x6eb) = 0;
            *(short *)(base + 0x6bc) = 0;
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 82 -- func_ov002_020c2b08, 0x020c2b08, size 0x2b0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c2b08
extern "C" {
void func_ov002_020c2b08(void *arg0)
{
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
    extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void*);
    extern int SurfaceInfo_TestFlag0x20(void*);
    extern int Player_ScaleByCharFactor(void*, int a);
    extern void _ZN9dBgCh_GndC1Ev(void*);
    extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void*, void*, void*);
    extern int _ZN9dBgCh_Gnd10DetectClsnEv(void*);
    extern void _ZN9dBgCh_GndD1Ev(void*);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern char data_ov002_0211013c;
    char *c = (char *)arg0;
    char rg[0x54];
    int v[3];

    if (*(int *)(c + 8) != 1)
        return;
    if (*(u8 *)(c + 0x703) != 0)
        return;

    if (*(u8 *)(c + 0x6ea) == 0) {
        int limit;

        if (!_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x380))
            return;
        if (!SurfaceInfo_TestFlag0x20((int *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x380) + 4)))
            return;

        limit = Player_ScaleByCharFactor(c, 0x20000);
        if (*(int *)(c + 0x98) >= limit)
            *(u16 *)(c + 0x6ba) = 0x3c;
        else
            *(u16 *)(c + 0x6ba) = 0x1e;
        *(u8 *)(c + 0x6ea) = 1;
        return;
    }

    if (*(u16 *)(c + 0x6ba) != 0) {
        if (!_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x380))
            goto clear_timer;
        if (!SurfaceInfo_TestFlag0x20((int *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x380) + 4)))
            goto clear_timer;

        *(u8 *)(c + 0x707) = 1;

        if (*(u16 *)(c + 0x6ba) != 0) {
            if (*(int *)(c + 0x98) >= 0x1000)
                goto big_block;
        }

        *(u16 *)(c + 0x6ba) = 0;
        *(int *)(c + 0x60) += *(int *)(c + 0x690);
        if (*(int *)(c + 0x60) < *(int *)(c + 0x644))
            *(int *)(c + 0x60) = *(int *)(c + 0x644);
        return;

    big_block:
        {
            int limit2 = Player_ScaleByCharFactor(c, 0x28000);

            if (*(int *)(c + 0x98) < limit2) {
                if (*(u16 *)(c + 0x6ba) >= 0x1e)
                    *(u16 *)(c + 0x6ba) = 0x1e;
            }

            *(int *)(c + 0x55c) = 0;
            {
                int t = *(int *)(c + 0x55c);
                *(int *)(c + 0x554) = t;
                *(int *)(c + 0x558) = 0x1000;
            }

            {
                int yy = *(int *)(c + 0x60);
                int zz = *(int *)(c + 0x64);
                int xx = *(int *)(c + 0x5c);
                int yy2 = yy + 0x64000;
                v[0] = xx;
                v[1] = yy2;
                v[2] = zz;
            }

            _ZN9dBgCh_GndC1Ev(rg);
            _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, v, c);

            if (_ZN9dBgCh_Gnd10DetectClsnEv(rg) != 0) {
                *(int *)(c + 0x644) = *(int *)(rg + 0x44);
                if (*(int *)(c + 0x60) + *(int *)(c + 0x690) < *(int *)(rg + 0x44))
                    *(u16 *)(c + 0x6ba) = 0;
            }

            _ZN9dBgCh_GndD1Ev(rg);
            return;
        }

    clear_timer:
        *(u16 *)(c + 0x6ba) = 0;
        return;
    }

    *(u8 *)(c + 0x6ec) = 0;
    if (!_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x380))
        return;
    if (SurfaceInfo_TestFlag0x20((int *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x380) + 4)) != 0)
        return;
    if (*(u8 *)(c + 0x707) != 0)
        return;

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211013c)) {
        *(u8 *)(c + 0x6ea) = 0;
        *(u8 *)(c + 0x6ec) = 1;
    }
    return;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 83 -- func_ov002_020c2db8, 0x020c2db8, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c2db8
extern "C" {
void func_ov002_020c2db8(unsigned char* c) {
    extern int func_ov002_020c14b8(void*);
    extern int func_ov002_020c231c(void*);
    extern void func_02035798(void*, int);
    extern int func_ov002_020c25a8(void*, int);
    extern int func_ov002_020c1234(void*);
    extern int func_ov002_020c0fb4(void*);
    extern int func_ov002_020c5d60(void*);
    extern int func_ov002_020c61ac(void*);
    extern int func_ov002_020eea84(void*, void*);
    extern int func_ov002_020c2b08(void*);
    extern int func_ov002_020c29d4(void*);
    extern void func_ov002_020c2db8(void*);
  func_ov002_020c14b8(c);
  c[0x6e9] = 0;
  if (c[0x716] != 0) return;
  c[0x70e] = 0;
  if (*(int*)(c+0x658) == 5) c[0x70e] = 1;
  if (func_ov002_020c231c(c) == 0) {
    func_02035798(c+0x380, 0x1000);
    func_ov002_020c25a8(c, 0);
  } else {
    func_02035798(c+0x380, 0);
    func_ov002_020c25a8(c, 1);
  }
  func_ov002_020c1234(c);
  func_ov002_020c0fb4(c);
  func_ov002_020c5d60(c);
  func_ov002_020c61ac(c);
  func_ov002_020eea84(c+0x380, c);
  func_ov002_020c2b08(c);
  func_ov002_020c29d4(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 84 -- func_ov002_020c2e78, 0x020c2e78, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c2e78
extern "C" {
void func_ov002_020c2e78(char* c){
    extern unsigned char IsPlayerWarping(int a0);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern void func_02020388(int handle);
    extern char data_ov002_02110154[];
    extern void func_ov002_020c2e78(void*);
  unsigned char w = IsPlayerWarping(*(unsigned char*)(c+0x6d8));
  if(w==0) return;
  if(!_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110154)){
    func_02020388(*(unsigned char*)(c+0x6d8));
    return;
  }
  *(unsigned char*)(c+0x6e8) = w;
  unsigned short *p = (unsigned short*)(((int)c + 0x6ce));
  *p |= 8;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 85 -- _ZN6Player7CanWarpEv, 0x020c2ef0, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player7CanWarpEv
/* recovered: named members + shared header, real C++ method
 *
 * Warping is allowed from exactly two states. One of them is now spelt as the
 * static member it is -- Player::ST_WAIT, which the compiler mangles to
 * _ZN6Player7ST_WAITE for us -- instead of a hand-written extern. The other is
 * still reached by its ov002 address; it has no recovered name yet.
 *
 * The file's own `struct State;` stand-in is gone with it: IsState really
 * takes a Player::State&, which is what its mangling says, and the local
 * forward declaration only ever existed to give the extern something to point
 * at.
 */
extern "C" {
int Player::CanWarp()
{
    extern Player::State data_ov002_021102a4;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    if (_ZN6Player7IsStateERNS_5StateE((void *)this, &ST_WAIT) || _ZN6Player7IsStateERNS_5StateE((void *)this, &data_ov002_021102a4))
        return 1;
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 86 -- _ZN6Player7IsInAirEv, 0x020c2f3c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player7IsInAirEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsInAir()
{
  if(mIsAirborne) return mIsInAirState!=0;
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 87 -- func_ov002_020c2f64, 0x020c2f64, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c2f64
extern "C" {
void func_ov002_020c2f64(void* c) {
    extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
  if (*(unsigned char*)((char*)c+0x6df)) return;
  *(unsigned char*)((char*)c+0x6df) = 1;
  if (*(unsigned char*)((char*)c+0x6f9)) {
    _ZN5Sound9PlayBank0EjRK7Vector3(0xa1, (char*)c+0x74);
    return;
  }
  if (*(unsigned char*)((char*)c+0x703)) {
    _ZN5Sound9PlayBank0EjRK7Vector3(0xd2, (char*)c+0x74);
    return;
  }
  _ZN5Sound9PlayBank0EjRK7Vector3(*(int*)((char*)c+0x66c)+0x40, (char*)c+0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 88 -- func_ov002_020c2fec, 0x020c2fec, size 0x15c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c2fec
extern "C" {
int func_ov002_020c2fec(struct Obj2fec *obj, int *out)
{
    struct Anim {
        short a;
        short x;
        short y;
        short z;
    };
    extern struct Anim data_ov002_0210a8b8[];
    extern void func_ov002_020c3160(void*, void*);
    extern int func_ov002_020c2fec(void*, void*);
    int tmp[4];
    int i;
    int ret;
    struct Anim *row;

    ret = 0;
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    func_ov002_020c3160(obj, tmp);

    row = &data_ov002_0210a8b8[obj->idx];
    for (i = 0; i < 4; i++) {
        out[0] += tmp[i] * row[i].x;
        out[1] += tmp[i] * row[i].y;
        out[2] += tmp[i] * row[i].z;
    }

    obj->f734 += (row[0].a << 12) / 1000;
    if (obj->f734 >= 0x1000) {
        obj->idx++;
        obj->f734 -= 0x1000;
        switch (obj->f738) {
        case 5:
            ret = 1;
            break;
        case 3:
            if (data_ov002_0210a8b8[obj->idx + 2].a == 0) {
                obj->f738 = 4;
            }
            break;
        default:
            obj->f738++;
            break;
        }
    }
    return ret;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 89 -- func_ov002_020c3148, 0x020c3148, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3148
extern "C" {
void func_ov002_020c3148(char *p)
{
    extern void func_ov002_020c3148(void*);
    *(char *)(p + 0x730) = 0;
    *(int *)(p + 0x734) = 0;
    *(int *)(p + 0x738) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 90 -- func_ov002_020c3160, 0x020c3160, size 0x644 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3160
extern "C" {
static inline int FX(int a, int b) {
    return (int)(((s64)(a) * (b) + 0x800) >> 12);
}

void func_ov002_020c3160(Arg0 *self, Arg1 *out)
{
    typedef long long s64;
    int t = self->unk734;
    int u = 0x1000 - t;
    int u2 = FX(u, u);
    int u3 = FX(u2, u);
    int t2 = FX(t, t);
    int t3 = FX(t2, t);

    switch (self->unk738) {
    case 0: break;
    case 1: {
        int a = FX(t, 0x3000);
        int b = FX(t3, 0x1c00);
        int c = FX(t2, 0x4800);
        out->unk0 = u3;
        out->unk4 = a + (b - c);
        out->unk8 = FX(t2, 0x1800) - FX(t3, 0xeaa);
        out->unkC = FX(t3, 0x2aa);
        break;
    }
    case 2: {
        int k1400 = 0x1400;
        s64 p955 = (s64)t3 * 0x955;
        s64 p1400 = (s64)t2 * k1400;
        out->unk0 = FX(u3, 0x400);
        {
            int b = FX(t, 0x400);
            int c = (int)((p955 + 0x800) >> 12);
            int d = (int)((p1400 + 0x800) >> 12);
            out->unk4 = b + (c - d) + 0x955;
        }
        out->unk8 = FX(t, 0x800) + (FX(t2, 0x800) - FX(t3, 0x800)) + 0x2aa;
        out->unkC = FX(t3, 0x2aa);
        break;
    }
    case 3: {
        out->unk0 = FX(u3, 0x2aa);
        out->unk4 = FX(t3, 0x800) - t2 + 0xaaa;
        out->unk8 = FX(t, 0x800) + (FX(t2, 0x800) - FX(t3, 0x800)) + 0x2aa;
        out->unkC = FX(t3, 0x2aa);
        break;
    }
    case 4: {
        out->unk0 = FX(u3, 0x2aa);
        out->unk4 = FX(u, 0x800) + (FX(u2, 0x800) - FX(u3, 0x800)) + 0x2aa;
        out->unk8 = FX(u, 0x400) + (FX(u3, 0x955) - FX(u2, 0x1400)) + 0x955;
        out->unkC = FX(t3, 0x400);
        break;
    }
    case 5: {
        out->unk0 = FX(u3, 0x2aa);
        out->unk4 = FX(u2, 0x1800) - FX(u3, 0xeaa);
        out->unk8 = FX(u, 0x3000) + (FX(u3, 0x1c00) - FX(u2, 0x4800));
        out->unkC = t3;
        break;
    }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 91 -- func_ov002_020c37a4, 0x020c37a4, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c37a4
/* recovered: shared common types */
extern "C" {
void func_ov002_020c37a4(char* self, char* arg){
    extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern void MulVec3Mat4x3(void*, void*, void*);
    extern void Vec3_MulScalar(void*, void*, int s);
    extern void Vec3_RotateYAndTranslate(void*, void*, int ang, void*);
    extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int c, int d, int e, const void* f, void*);
    extern void func_ov002_020c37a4(void*, void*);
  unsigned int id = _ZNK6Player14GetBodyModelIDEjb(self, *(int*)(self+8) & 0xff, 0);
  int r6 = *(int*)(*(int*)(self + (id << 2) + 0xdc) + 0x14) + 0x180;
  unsigned int id2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int*)(self+8) & 0xff, 0);
  struct Vector3 v;
  struct Vector3 s;
  MulVec3Mat4x3((void*)(r6 + 0x24), (void*)(*(int*)(self + (id2 << 2) + 0xdc) + 0x1c), &v);
  Vec3_MulScalar(&s, &v, 0x8000);
  v = s;
  Vec3_RotateYAndTranslate(&v, &v, *(short*)(self+0x8e), arg);
  *(void**)(self+0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    *(unsigned int*)(self+0x628), 0x34, v.x, v.y, v.z, 0, 0);
  *(void**)(self+0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    *(unsigned int*)(self+0x62c), 0x35, v.x, v.y, v.z, 0, 0);

}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 92 -- func_ov002_020c38a0, 0x020c38a0, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c38a0
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
#define LA(p) ((int)(p))
extern "C" {
void func_ov002_020c38a0(char *c)
{
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int cc, unsigned int d);
    extern int Vec3_HorzLen(void*);
    extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
    extern void _Z15ApproachLinear2Rsss(void*, short a, short b);
    extern void func_ov002_020c37a4(void*, void*);
    extern void func_ov002_020c3148(void*);
    extern int func_ov002_020c2fec(void*, void*);
    V3Quad s;
    int hlen;
    int ang;
    unsigned char *p;

    s.a.x = 0; s.a.y = 0; s.a.z = 0x60000;
    s.b.x = 0; s.b.y = 0; s.b.z = 0x60000;
    func_ov002_020c37a4(c, &s.b);
    s.d.x = 0; s.d.y = 0; s.d.z = 0;
    switch (*(unsigned char *)(c + 0x6e5)) {
    case 0:
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xc1, 0, 0x1000, 0);
        *(unsigned char *)(c + 0x743) = 0;
        func_ov002_020c3148(c);
        *(int *)(c + 0x690) = -0x20000;
        p = (unsigned char *)LA(c + 0x6e5);
        *p = *p + 1;
        /* fallthrough */
    case 1:
        if (func_ov002_020c2fec(c, &s.d) != 0) {
            p = (unsigned char *)LA(c + 0x6e5);
            *p = *p + 1;
            return;
        }
        s.e.x = s.d.x - *(int *)(c + 0x5c);
        s.e.y = s.d.y - *(int *)(c + 0x60);
        s.e.z = s.d.z - *(int *)(c + 0x64);
        hlen = Vec3_HorzLen(&s.e);
        ang = _ZN4cstd5atan2E5Fix12IiES1_(s.e.x, s.e.z);
        *(int *)(c + 0x5c) = s.d.x;
        *(int *)(c + 0x60) = s.d.y;
        *(int *)(c + 0x64) = s.d.z;
        if ((s.e.x | s.e.z) == 0) {
            *(short *)(c + 0x8c) = 0;
            *(short *)(c + 0x8e) = 0x7fff;
            return;
        }
        *(short *)(c + 0x8c) = -_ZN4cstd5atan2E5Fix12IiES1_(s.e.y, hlen);
        _Z15ApproachLinear2Rsss((short *)(c + 0x90), 0x2000, 0x10);
        *(short *)(c + 0x8e) = ang;
        break;
    case 2:
        break;
    }
}
}
#undef LA

/* -------------------------------------------------------------------------- */
/* ROM ordinal 93 -- func_ov002_020c3a48, 0x020c3a48, size 0x194 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3a48
extern "C" {
int func_ov002_020c3a48(char *self)
{
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int frame);
    extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void*);
    extern void func_ov002_020c37a4(void*, void*);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void func_ov002_020c3cf0(void*);
    extern void Player_AdvanceAnims(void*);
    unsigned int r5;
    char *anim;
    int id;

    if (*(unsigned char *)(self + 0x6e5) == 0) {
        *(int *)(self + 0x688) = 0;
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = *(char **)(self + (id << 2) + 0xdc) + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x5a)) {
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int *)(self + 0x5c), *(int *)(self + 0x60), *(int *)(self + 0x64));
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(self + 0x6d9), 0, self + 0x74);
            *(unsigned char *)(self + 0x6e5) += 1;
        }
    } else {
        int buf[6];
        buf[0] = 0;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
        buf[4] = 0;
        buf[5] = 0;
        func_ov002_020c37a4(self, (char *)(buf + 3));
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((long long)(int)(*(char **)(self + (id << 2) + 0xdc) + 0x50));
        r5 = (unsigned int)(*(int *)(anim + 8) << 4) >> 0x10;
        if (r5 == 0x6d || r5 == 0x8d) {
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int *)(self + 0x5c), *(int *)(self + 0x60), *(int *)(self + 0x64));
        }
        switch (r5) {
        case 0x77:
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(self + 0x6d9), 2, self + 0x74);
            break;
        case 0x97:
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(self + 0x6d9), 4, self + 0x74);
            break;
        }
        *(int *)(self + 0x688) -= 0x21000;
        if (_ZN6Player12FinishedAnimEv(self)) {
            func_ov002_020c3cf0(self);
            return 1;
        }
    }

    *(int *)(self + 0x5c) = 0;
    *(int *)(self + 0x64) = *(int *)(self + 0x688);
    Player_AdvanceAnims(self);
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 94 -- func_ov002_020c3bdc, 0x020c3bdc, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3bdc
extern "C" {
void func_ov002_020c3bdc(char* c){
    typedef int Fix12i;
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, Fix12i c, unsigned int d);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void func_0200eec8(void);
    extern void func_ov002_020c3cf0(void*);
    extern void Player_AdvanceAnims(void*);
  if (*(unsigned char*)(c + 0x6e5) == 0){
    *(unsigned char*)(c + 0x6ff) = 1;
    *(short*)(c + 0x6ae) = 0x708;
    *(short*)(c + 0x94) = -0x8000;
    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    *(int*)(c + 0x64) = 0;
    *(int*)(c + 0x5c) = *(int*)(c + 0x64);
    *(int*)(c + 0x98) = 0;
    *(int*)(c + 0x9c) = -0x2000;
    if (*(unsigned char*)(c + 0x6de) == 0){
      _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x55, 0x40000000, 0x1000, 0);
      *(unsigned char *)(((int)c + 0x6e5)) += 1;
    }
  } else {
    if (_ZN6Player12FinishedAnimEv(c)){
      func_0200eec8();
      func_ov002_020c3cf0(c);
      _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xc0, 0x40000000, 0x1000, 0);
      *(unsigned char*)(c + 0x743) = 3;
      *(unsigned char*)(c + 0x716) = 1;
      *(unsigned char*)(c + 0x713) = 0;
      *(int*)(c + 0xa8) = 0;
      *(int*)(c + 0x9c) = 0;
      *(int*)(c + 0x60) = 0x15e000;
    }
  }
  Player_AdvanceAnims(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 95 -- func_ov002_020c3cf0, 0x020c3cf0, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3cf0
extern "C" {
void func_ov002_020c3cf0(char* self)
{
    *(unsigned char*)(self + 0x6e3) = 0;
    *(unsigned char*)(self + 0x6e5) = *(unsigned char*)(self + 0x6e3);
    (*(unsigned char*)(((int)self + 0x6e6)))++;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 96 -- _ZN6Player17St_EndingFly_MainEv, 0x020c3d1c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17St_EndingFly_MainEv
/* recovered: shared header, real C++ method
 *
 * ov002, 0x020c3d1c, size 0x50. The per-tick handler for the ending flight,
 * dispatching on a phase counter at 0x6e6 to one of three ov002 workers.
 *
 * THE FALLTHROUGH IS DELIBERATE. Phase 1 runs its worker and, when that
 * returns non-zero, falls into phase 2's -- so the frame that finishes phase 1
 * also starts phase 2 rather than idling. Written as a `case 1:` with no
 * `break` on the non-zero path, which is what the ROM's branch structure does.
 *
 * The three workers stay placeholders: they are ov002 functions with no
 * recovered name, and naming them from this call site alone would be a guess.
 *
 * This file carried the placeholder name func_ov002_020c3d1c until the symbol
 * was moved here from ov007, where it had been sitting on an unrelated 0x50
 * function that ov007 calls from 46 places. See the commit and Player.h.
 */
extern "C" {
int Player::St_EndingFly_Main()
{
    extern int func_ov002_020c3bdc(void*);
    extern int func_ov002_020c3a48(void*);
    extern int func_ov002_020c38a0(void*);
    switch (mStatePhase) {
    case 0:
        func_ov002_020c3bdc((char *)this);
        break;
    case 1:
        if (func_ov002_020c3a48((char *)this) == 0) break;
    case 2:
        func_ov002_020c38a0((char *)this);
        break;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 97 -- _ZN6Player17St_EndingFly_InitEv, 0x020c3d6c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17St_EndingFly_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_EndingFly_Init()
{
    extern int Player_DisableInteraction(void*);
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
  Player_DisableInteraction(((char*)this));
  mIsControlDisabled=1;
  mStatePhase=0;
  mStateWork=mStatePhase;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x54,0x40000000,0x1000,0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 98 -- func_ov002_020c3dbc, 0x020c3dbc, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3dbc
extern "C" {
void func_ov002_020c3dbc(void* player) {
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern char data_ov002_0211058c[];
    _ZN6Player11ChangeStateERNS_5StateE(player, data_ov002_0211058c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 99 -- _ZN6Player21St_OpeningWakeUp_MainEv, 0x020c3dd0, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player21St_OpeningWakeUp_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_OpeningWakeUp_Main()
{
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, int a, int b, int c, u32 d);
    extern void Player_AdvanceAnims(void*);
    switch (mStateStep) {
    case 0:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0xb4, 0, 0x1000, 0);
            *(u8*)(int)((char*)&mStateStep) += 1;
        }
        break;
    case 1:
        break;
    case 2:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0xb5, 0, 0x1000, 0);
            *(u8*)(int)((char*)&mStateStep) += 1;
        }
        break;
    case 3:
        break;
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 100 -- func_ov002_020c3e8c, 0x020c3e8c, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3e8c
extern "C" {
void func_ov002_020c3e8c(void* player) {
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern char data_ov002_0211013c[];
    _ZN6Player11ChangeStateERNS_5StateE(player, data_ov002_0211013c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 101 -- func_ov002_020c3ea0, 0x020c3ea0, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3ea0
extern "C" {
int func_ov002_020c3ea0(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0xb3,0x40000000,0x1000,0);
  *(unsigned char*)(c+0x6e3)=2;
  return 2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 102 -- _ZN6Player21St_OpeningWakeUp_InitEv, 0x020c3ed8, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player21St_OpeningWakeUp_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_OpeningWakeUp_Init()
{
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
  mSleepStage=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0xb2,0x40000000,0x1000,0);
  mStateStep=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 103 -- func_ov002_020c3f18, 0x020c3f18, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3f18
extern "C" {
void func_ov002_020c3f18(char* p) {
    extern void func_ov002_020d2e74(void*);
    p[0x6e6] = 1;
    func_ov002_020d2e74(p);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 104 -- func_ov002_020c3f2c, 0x020c3f2c, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c3f2c
extern "C" {
void func_ov002_020c3f2c(void* player) {
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern char data_ov002_02110544[];
    _ZN6Player11ChangeStateERNS_5StateE(player, data_ov002_02110544);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 105 -- _ZN6Player15St_Respawn_MainEv, 0x020c3f40, size 0x148 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15St_Respawn_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Respawn_Main()
{
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 a, int b, int f, u32 g);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_0209f250;
    extern int data_ov002_02110154[];
    extern void FUN_02029980(void);
    extern void FUN_02029934(void);
    switch (mStateStep) {
    case 0: {
        u16 t = mStateTimer;
        if (t != 0) {
            if (t == 1) {
                mVertAccel = -0x4000;
                if (mPlayerNo == data_0209f250) {
                    FUN_02029980();
                    mStateWaitTimer = 0x26;
                }
            }
        } else {
            if (mIsAirborne == 0) {
                u32 zero = 0;
                _ZN6Player7SetAnimEji5Fix12IiEj(((char *)this), 0x55, 0x40000000, 0x1000, zero);
                mStateStep += 1;
            }
        }
        break;
    }
    case 1:
        if (_ZN6Player12FinishedAnimEv(((char *)this)) != 0) {
            int z = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char *)this), 0x47, z, 0x1000, z);
            mStateStep += 1;
        }
        break;
    case 2:
        if (mStateWaitTimer == 0) {
            if (mPlayerNo == data_0209f250) {
                FUN_02029934();
            }
            _ZN6Player11ChangeStateERNS_5StateE(((char *)this), data_ov002_02110154);
        }
        break;
    }
    Player_AdvanceAnims(((char *)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 106 -- _ZN6Player15St_Respawn_InitEv, 0x020c4088, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15St_Respawn_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * Restores the spawn point saved by Player::InitResources, lifts the player
 * 0x3e8000 above it, re-seats the mesh collider at the new position, then
 * resets speed/angle state and starts the respawn animation.
 */
extern "C" {
int Player::St_Respawn_Init()
{
    extern void Player_DisableInteraction(void*);
    extern void func_02035860(void*, void*);
    extern void func_0200cf40(void*);
    extern unsigned int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern unsigned char data_0209f250;
    extern char* data_0209f318;
    Player_DisableInteraction(this);
    mPosX = mSpawnPosX;
    mPosY = mSpawnPosY;
    mPosZ = mSpawnPosZ;
    mPosY += 0x3e8000;
    func_02035860(&mMeshClsn, &mPosX);
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    mPrevAngleY = mSpawnAngleY;
    mAngleY = mPrevAngleY;
    mIsBodyClsnEnabled = 1;
    unk_716 = 0;
    if (mPlayerNo == data_0209f250) {
        func_0200cf40(*(char**)((int)&data_0209f318));
    }
    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x54, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    mPeakY = mPosY;
    mHorzSpeed = 0;
    mVertSpeed = mHorzSpeed;
    mVertAccel = 0;
    mStateTimer = 0xa;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 107 -- func_ov002_020c4188, 0x020c4188, size 0x23c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c4188
extern "C" {
int func_ov002_020c4188(char* self)
{
    extern int* data_0209f318;
    extern int data_0209b454;
    extern short data_ov002_020ff26c[];
    extern unsigned char data_0209d660;
    extern void func_0200d3f8(void*, unsigned char playerID, void*);
    extern void _ZN6Camera9SetFlag_3Ev(void*);
    extern void func_0201f32c(int arg0);
    extern void* _ZN8dActor_c13SpawnSoundObjEj(void*, unsigned int a);
    extern void func_0200d81c(void*, int playerID);
    extern void func_ov002_020e444c(void*);
    extern int func_ov002_020c4188(void*);
    int* p;

    if (*(u8*)(self + 0x71e) == 0) {
        return 0;
    }

    p = data_0209f318;

    switch (*(u8*)(self + 0x71f)) {
    case 0:
        if (*(u8*)(self + 0x71e) == 6) {
            (*(u8*)((int)(self + 0x71f)))++;
            goto case1;
        }
        if (*(u8*)(self + 0x71e) < 7) {
            func_0200d3f8(p, *(u8*)(self + 0x6d8), 0);
        } else {
            _ZN6Camera9SetFlag_3Ev(p);
        }
        (*(u8*)((int)(self + 0x71f)))++;
        if (*(u8*)(self + 0x71e) >= 7) {
            break;
        }
        return 0;
    case 1:
    case1:
        *(int*)((int)(self + 0xb0)) |= 0x800000;
        data_0209b454 |= 0x800000;
        *(u8*)(self + 0x720) = 0xa;
        (*(u8*)((int)(self + 0x71f)))++;
        break;
    case 2:
        if (*(u8*)(self + 0x720) != 0) {
            (*(u8*)((int)(self + 0x720)))--;
            break;
        }
        func_0201f32c(data_ov002_020ff26c[*(u8*)(self + 0x71e) - 1]);
        if (*(u8*)(self + 0x71e) == 6) {
            *(void**)(self + 0x364) = _ZN8dActor_c13SpawnSoundObjEj(self, 5);
        }
        (*(u8*)((int)(self + 0x71f)))++;
        break;
    case 3:
        if (data_0209d660 != 0) {
            break;
        }
        (*(u8*)((int)(self + 0x71f)))++;
        *(int*)((int)(self + 0xb0)) &= ~0x800000;
        data_0209b454 &= ~0x800000;
        if (*(u8*)(self + 0x71e) >= 7) {
            *(int*)(((int)p + 0x154)) &= ~8;
        }
        func_0200d81c(p, *(u8*)(self + 0x6d8));
        if (*(u8*)(self + 0x71e) == 6) {
            int* obj = *(int**)(self + 0x364);
            if (obj[0x35] == 0x2a) {
                *(short*)((char*)obj + 0xde) = 0;
            }
        }
        break;
    case 4:
        if (p[0x55] & 0x8000) {
            break;
        }
        *(u8*)(self + 0x71e) = 0;
        *(u8*)(self + 0x71f) = 0;
        *(int*)((int)(self + 0xb0)) &= ~0x800000;
        data_0209b454 &= ~0x800000;
        break;
    }

    func_ov002_020e444c(self);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 108 -- func_ov002_020c43c4, 0x020c43c4, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c43c4
extern "C" {
void func_ov002_020c43c4(char* self, unsigned int idx) {
    extern unsigned char data_0209f2d8;
    extern int data_0209caa0;
    int b = (int)(data_0209f2d8 == 1);
    if (b != 0) return;
    switch (idx) {
    case 1:
        if ((&data_0209caa0)[2] & 0x400) return;
        (&data_0209caa0)[2] |= 0x400;
        break;
    case 2:
        if ((&data_0209caa0)[2] & 0x800) return;
        (&data_0209caa0)[2] |= 0x800;
        break;
    case 3:
        if ((&data_0209caa0)[2] & 0x1000) return;
        (&data_0209caa0)[2] |= 0x1000;
        break;
    case 4:
        if ((&data_0209caa0)[2] & 0x2000) return;
        (&data_0209caa0)[2] |= 0x2000;
        break;
    case 5:
        if ((&data_0209caa0)[2] & 0x4000) return;
        (&data_0209caa0)[2] |= 0x4000;
        break;
    case 6:
        if ((&data_0209caa0)[2] & 0x10000000) return;
        (&data_0209caa0)[2] |= 0x10000000;
        break;
    case 0:
        break;
    }
    *(unsigned char*)(self + 0x71e) = (unsigned char)idx;
    *(unsigned char*)(self + 0x71f) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 109 -- func_ov002_020c44c4, 0x020c44c4, size 0x29c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c44c4
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020c44c4(char* self)
{
    extern u8 data_0209f2d8;
    extern int data_0209caa0[];
    extern s8 data_0209f2f8;
    extern u8 data_0209f264;
    extern void _ZN7Vector3D1Ev(void);
    extern void func_020731dc(int a, int b, void*);
    extern void Vec3_RotateYAndTranslate(void*, void*, short angle, void*);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void*, void*, unsigned int id, const void* pos, unsigned int a, unsigned int b);
    extern void func_02012694(unsigned int id, const struct Vector3* v);
    extern unsigned int func_02012790(unsigned int a);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int fix, unsigned int d);
    extern void func_ov002_020c43c4(void*, unsigned int idx);
    extern unsigned int _ZN8SaveData26CountStarsCollectedInLevelEj(unsigned int);
    extern int SublevelToLevel(int);
    extern u32 data_ov002_0210e14c;
    extern struct Vector3 data_ov002_0210f3b0;
    extern void*data_ov002_0210f350;
    extern void func_ov100_02145070(int);
    extern int func_ov100_02145014(void);
    struct Vector3 pos;
    int state;
    int r4val;
    u8 f2d8;
    int b0, b1;

    f2d8 = data_0209f2d8;
    r4val = -1;
    b0 = (int)(f2d8 == 1);
    if (b0 != 0) {
        goto cont;
    }
    if ((data_0209caa0[2] & 0x80) == 0) {
        goto ret0;
    }
    b1 = (int)(f2d8 == 2);
    if (b1 == 0) {
        goto cont;
    }
ret0:
    return 0;

cont:
    if (_ZN8SaveData26CountStarsCollectedInLevelEj(SublevelToLevel(data_0209f2f8)) != 0) {
        return 0;
    }

    state = data_0209f2f8;

    switch (state) {
    case 2:
        if ((data_0209caa0[2] & 0x200) == 0) {
            data_0209caa0[2] |= 0x200;
            if ((data_ov002_0210e14c & 1) == 0) {
                data_ov002_0210f3b0.x = 0;
                data_ov002_0210f3b0.y = 0x96000;
                data_ov002_0210f3b0.z = 0x64000;
                func_020731dc((int)&data_ov002_0210f3b0, (int)_ZN7Vector3D1Ev, &data_ov002_0210f350);
                data_ov002_0210e14c |= 1;
            }
            Vec3_RotateYAndTranslate((int*)&pos, (int*)(self + 0x5c), *(s16*)(self + 0x8e), (int*)&data_ov002_0210f3b0);
            _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(self, self, 0x177, &pos, 2, 2);
            func_ov100_02145070(0x1e);
            func_ov100_02145014();
            func_02012694(0xb8, (struct Vector3*)(self + 0x74));
            *(u8*)(self + 0x725) = 1;
            return 1;
        }
        break;
    case 6:
        r4val = 7;
        break;
    case 7:
        r4val = 8;
        break;
    case 10:
        if (data_0209f264 == 0) {
            r4val = 9;
        }
        break;
    case 14:
        if (data_0209f264 == 0) {
            r4val = 10;
        }
        break;
    case 35:
        if (data_0209f264 == 0) {
            r4val = 11;
            func_02012790(0x18);
        }
        break;
    case 31:
        if (data_0209caa0[1] & 0x80000000) {
            return 0;
        }
        r4val = 12;
        break;
    }

    if (r4val == -1) {
        return 0;
    }

    _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x47, 0, 0x1000, 0);
    func_ov002_020c43c4(self, r4val & 0xff);
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 110 -- _ZN6Player15St_Talk_CleanupEv, 0x020c4760, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15St_Talk_CleanupEv
/* recovered: named members + shared header, real C++ method
 *
 * Leaving the talk state: clear the "in conversation" bit (0x800000) on the
 * actor being talked to, on the player, and on the global at data_0209b454;
 * clear bit 3 on data_0209f318+0x154; and restore music layer 1 if this state
 * had overridden it.
 *
 * mTalkActor is declared u8 in Player.h but holds an actor pointer -- +0xb0
 * is read through it, which is fBase_c's mFlags. Reached through a cast
 * here rather than retyping the field, because Player::St_Talk_Main reaches
 * it the same way and the two should change together.
 */
extern "C" {
int Player::St_Talk_Cleanup()
{
    extern u32 data_0209b454;
    extern char* data_0209f318;
    extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int);
    void* p = *(void**)((char*)&mTalkActor);
    if (p) {
        *(u32*)((char*)p + 0xb0) &= ~0x800000;
        *(void**)((char*)&mTalkActor) = 0;
    }
    mFlags &= ~0x800000;
    data_0209b454 &= ~0x800000;
    *(u32*)((char*)data_0209f318 + 0x154) &= ~8;
    if (unk_725) {
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x39);
        unk_725 = 0;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 111 -- func_ov002_020c47f4, 0x020c47f4, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c47f4
/* recovered: shared common types */
extern "C" {
int func_ov002_020c47f4(char* c)
{
    typedef int Fix12i;
    extern short Vec3_VertAngle(const Vector3* v1, const Vector3* v0);
    extern int _Z15ApproachLinear2Rsss(short& dst, short to, short step);
    extern short Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
    extern int ApproachAngle(void*, short targetAngle, short invFactor, short maxDelta, short minDelta);
    extern int func_ov002_020c47f4(void*);
    Vector3 vec;
    int r, a;
    Fix12i y;
    vec.x = *(Fix12i*)(c + 0x5c);
    y = *(Fix12i*)(c + 0x60);
    vec.y = y;
    vec.z = *(Fix12i*)(c + 0x64);
    vec.y = y + 0x64000;
    *(short*)(c + 0x762) = 0;
    *(short*)(c + 0x764) = 0;
    r = _Z15ApproachLinear2Rsss(*(short*)(c + 0x69c), (short)(-Vec3_VertAngle((Vector3*)(c + 0x744), &vec)), 0x200);
    *(short*)(c + 0x762) = *(short*)(c + 0x69c);
    a = ApproachAngle((short*)(c + 0x8e), (short)(Vec3_HorzAngle((Vector3*)(c + 0x744), &vec) + 0x8000), 4, 0x1000, 0x200);
    *(char*)(c + 0x742) = 3;
    if (r == 0) goto ret0;
    if (a == 0) return 1;
ret0:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 112 -- _ZN6Player12St_Talk_MainEv, 0x020c48e0, size 0x2f8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12St_Talk_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Talk_Main()
{
    extern int _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 vol, int t);
    extern int _ZN6Player6IsAnimEj(void*, u32 a);
    extern void func_0201f32c(s16 a);
    extern void func_0200d3f8(void*, u8 playerID, void*);
    extern int _Z15ApproachLinear2Rsss(void*, s16 target, s16 step);
    extern void func_0200d81c(void*, u8 playerID);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void Player_AdvanceAnims(void*);
    extern int data_0209b454;
    extern char* data_0209f318;
    extern u8 data_0209d660;
    extern int data_ov002_0211022c[];
    extern int data_ov002_02110514[];
    extern int data_ov002_0211013c[];
    extern int func_ov002_020c47f4(void*);
    extern void func_0201fc88(s16);
    extern void func_0200d7a4(void*, u8);
    extern int func_ov100_02144fcc(void);
    if (unk_722 != 0) {
        if (_ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x7222) != 0)
            unk_722 = 0;
    }

    if (mNoCtrlKind == 0) {
        char* p = *(char**)((char*)&mTalkActor);
        if (p != 0)
            *(u32*)(((int)p + 0xb0)) |= 0x800000;
        mFlags |= 0x800000;
        data_0209b454 |= 0x800000;
        mNoCtrlKind = 1;
    }

    switch (mStateStep) {
    case 0:
        if (mStateWork != 0)
            func_ov002_020c47f4(((char*)this));
        break;
    case 1:
        if (!_ZN6Player6IsAnimEj(((char*)this), 0x64)) {
            if (!func_ov002_020c47f4(((char*)this)))
                break;
        }
        mStateStep = 2;
        if (mStateWork == 0)
            func_0201fc88(mAttachOffsetY);
        else
            func_0201f32c(mAttachOffsetY);
        {
            char* cam = data_0209f318;
            switch ((mStateArg >> 2) & 3) {
            case 0:
                func_0200d3f8(cam, mPlayerNo, ((char*)this) + 0x744);
                break;
            case 1:
                func_0200d7a4(cam, mPlayerNo);
                break;
            }
        }
        break;
    case 2:
        if (data_0209d660 != 0)
            break;
        switch (mStateArg & 3) {
        case 0:
            mStateStep = 4;
            break;
        case 1:
            mStateStep = 3;
            break;
        case 2:
            mStateStep = 8;
            break;
        }
        break;
    case 8:
        if (func_ov100_02144fcc() != 0)
            mStateStep = 4;
        break;
    case 4:
    case 5:
        if (_Z15ApproachLinear2Rsss((s16*)((char*)&unk_762), 0, 0x200) != 0) {
            if (mStateStep != 5)
                mStateStep = 6;
            else
                mStateStep = 7;
            mStateWork = 0;
            mPrevAngleY = mAngleY;
            char* cam = data_0209f318;
            func_0200d81c(cam, mPlayerNo);
            *(u32*)(((int)cam + 0x154)) &= ~8;
            unk_742 = 4;
        }
        break;
    case 7:
        if (mStateWaitTimer != 0)
            break;
    case 6:
        if ((*(u32*)(data_0209f318 + 0x154) & 0x8000) != 0)
            break;
        if (mIsOpeningBigDoor != 0) {
            mNoCtrlKind = 0x13;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211022c);
            mStateStep = 0x13;
        } else if (_ZN6Player6IsAnimEj(((char*)this), 0x64)) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110514);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 113 -- _ZN6Player12St_Talk_InitEv, 0x020c4bd8, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12St_Talk_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Talk_Init()
{
    extern int Player_DisableInteraction(void*);
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
  mIsControlDisabled=1;
  Player_DisableInteraction(((char*)this));
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x47,0,0x1000,0);
  mAngleYSpeed=0;
  mHorzSpeed=0;
  mNoCtrlKind=0;
  mVertSpeed=0;
  mVertAccel=mVertSpeed;
  unk_716=1;
  mIsBodyClsnEnabled=0;
  mPosX=mPreClsnPosX;
  mPosY=mPreClsnPosY;
  mPosZ=mPreClsnPosZ;
  mStateWork=0;
  mIsOpeningBigDoor=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 114 -- _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh, 0x020c4c60, size 0x260 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh
/* recovered: named members + shared header, real C++ method
 *
 * Starts a message. Already talking is success only at sub-step 0 or 3;
 * otherwise a short list of states may be interrupted, plus the no-control
 * state for all but a handful of mNoCtrlKind values. The last two parameters
 * are unsigned char -- the ROM reads all three of their uses with ldrb, and
 * the symbol was corrected from jj to hh two commits ago.
 */
extern "C" {
int Player::ShowMessage2(fBase_c & actor_, unsigned int msg, const Vector3 * pos, unsigned char d, unsigned char e)
{
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void func_020731dc(void*, void*, void*);
    extern void Vec3_RotateYAndTranslate(void*, const Vector3 *in, int angle, const Vector3 *src);
    extern void _ZN6Camera9SetFlag_3Ev(void*);
    extern Player::State data_ov002_0211046c;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110364;
    extern Player::State data_ov002_0211043c;
    extern Player::State data_ov002_02110154;
    extern Player::State data_ov002_02110424;
    extern Player::State data_ov002_021104e4;
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_0211022c;
    extern int data_ov002_0210e15c;
    extern int data_ov002_0210f224[3];
    extern int data_ov002_0210f1ac;
    extern void _ZN7Vector3D1Ev(void);
    extern void *data_0209f318;
    fBase_c *actor = &actor_;
    int r7;

    if (IsState(data_ov002_0211046c)) {
        u8 sub = mStateStep;
        if (sub == 0 || sub == 3)
            goto finish;
        return 0;
    }

    r7 = 0;
    if (IsState(data_ov002_0211013c) || IsState(data_ov002_02110364) || IsState(data_ov002_0211043c) || IsState(data_ov002_02110154) || IsState(data_ov002_02110424) || IsState(data_ov002_021104e4) || IsState(data_ov002_02110514)) {
        r7 = 1;
    }

    if (IsState(data_ov002_0211022c)) {
        u8 v = mNoCtrlKind;
        if (!(v == 0 || v == 1 || v == 3 || v == 2 || v == 0xd)) {
            if (v != 8)
                r7 = 1;
        }
    }

    if (!r7)
        return 0;

    _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211046c);
    *(fBase_c **)(&mTalkActor) = actor;

finish:
    mStateWork = 0;
    mStateArg = d & 3;
    *(u8 *)(((long long)(&mStateArg))) |= (e & 3) << 2;

    if (!(data_ov002_0210e15c & 1)) {
        data_ov002_0210f224[0] = 0;
        data_ov002_0210f224[1] = 0x64000;
        data_ov002_0210f224[2] = 0x64000;
        func_020731dc(data_ov002_0210f224, (void *)_ZN7Vector3D1Ev, &data_ov002_0210f1ac);
        data_ov002_0210e15c |= 1;
    }

    Vec3_RotateYAndTranslate((Vector3 *)(&unk_744), (Vector3 *)(&mPosX), mAngleY, (Vector3 *)data_ov002_0210f224);

    if (pos) {
        *(Vector3 *)(&unk_744) = *pos;
    }

    mHorzSpeed = 0;
    mStateStep = 1;
    *(unsigned int*)&mAttachOffsetY = msg;

    if (e == 2) {
        _ZN6Camera9SetFlag_3Ev(data_0209f318);
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 115 -- _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh, 0x020c4ec0, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh
/* recovered: named members + shared header, real C++ method
 *
 * Thin wrapper: forwards to ShowMessage2 and, if that took, clears mStateWork
 * to 1.
 *
 * The last two parameters are unsigned char. That is what the ROM does -- both
 * are read with ldrb -- and now what the symbol says, since the preceding
 * commit corrected it from jj to hh. While it said jj this function could not
 * be a real method at all: the compiler mangles FROM the declared types.
 */
extern "C" {
int Player::ShowMessage(fBase_c & a_, unsigned int b, const Vector3 * v, unsigned char d_, unsigned char e_)
{
    extern int _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(void*, void*, unsigned int b, const Vector3* v, unsigned int d, unsigned int e);
    if (_ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(this, &a_, b, v, d_, e_)) {
        mStateWork = 1;
        return 1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 116 -- _ZN6Player18HasFinishedTalkingEv, 0x020c4efc, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player18HasFinishedTalkingEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::HasFinishedTalking()
{
    extern struct State;
    extern State data_ov002_0211046c;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
  if(_ZN6Player7IsStateERNS_5StateE(((char*)this),&data_ov002_0211046c) && mStateStep==3){
    mStateStep=4;
    return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 117 -- _ZN6Player12Unk_020c4f40Et, 0x020c4f40, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12Unk_020c4f40Et
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020c4f40(unsigned short x)
{
    extern struct State;
    extern State data_ov002_0211046c;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
  if(_ZN6Player7IsStateERNS_5StateE(((char*)this),&data_ov002_0211046c) && mStateStep==3){
    mStateWaitTimer=x;
    mStateStep=5;
    return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 118 -- _ZN6Player9StartTalkER7fBase_cb, 0x020c4fa0, size 0x230 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player9StartTalkER7fBase_cb
/* recovered: named members + shared header, real C++ method
 *
 * Entering conversation. Already talking to this actor with mStateStep 0 is
 * success; talking to anything else is refusal. Otherwise only a short list of
 * states may start a conversation, and when b_ is false the player must also
 * be pressing a button and be walking slowly, crouching or waiting.
 *
 * The third parameter is bool -- the ROM name ends in `b`, and a real method
 * is mangled FROM the declared types.
 */
extern "C" {
int Player::StartTalk(fBase_c & actor_, bool b_)
{
    extern Player::State data_ov002_0211046c;
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110154;
    extern Player::State data_ov002_021104e4;
    extern Player::State data_ov002_02110514;
    extern Player::State data_ov002_02110364;
    extern Player::State data_ov002_02110424;
    extern Player::State data_ov002_021105a4;
    extern Player::State data_ov002_0211043c;
    extern u8 data_020a0e40;
    extern u8 data_0209f49e[];
    fBase_c *actor = &actor_;
    if (IsState(data_ov002_0211046c)) {
        if (actor == *(fBase_c **)&mTalkActor && mStateStep == 0)
            return 1;
        return 0;
    }

    if (!(IsState(data_ov002_0211013c) || IsState(data_ov002_02110154) || IsState(data_ov002_021104e4) || IsState(data_ov002_02110514) || IsState(data_ov002_02110364) || IsState(data_ov002_02110424) || IsState(data_ov002_021105a4) || IsState(data_ov002_0211043c)))
        return 0;

    if (b_ == 0) {
        if (!(*(u16 *)(data_0209f49e + (u8)data_020a0e40 * 0x18) & 3))
            goto ret0;

        if (!((IsState(data_ov002_0211013c) && mHorzSpeed < 0x8000) || IsState(data_ov002_0211043c) || IsState(data_ov002_02110154)))
            goto ret0;

        mStateStep = 0;
        ChangeState(data_ov002_0211046c);
        *(fBase_c **)&mTalkActor = actor;
        return 1;
    } else {
        if (IsState(data_ov002_02110424) || IsState(data_ov002_021105a4)) {
            *(u16 *)(((long long)((char *)this + 0x6ce))) |= 0x40;
            return 0;
        }

        mStateStep = 0;
        ChangeState(data_ov002_0211046c);
        *(fBase_c **)&mTalkActor = actor;
        return 1;
    }
ret0:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 119 -- func_ov002_020c51d0, 0x020c51d0, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c51d0
extern "C" {
int func_ov002_020c51d0(char* c, State3i* st){
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern State3i data_ov002_0211046c;
    extern int func_ov002_020c51d0(void*, void*);
  if (!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211046c)) return 0;
  if (*(unsigned char*)(c+0x6e3) != 0) return 0;
  *(State3i*)(c+0x744) = *st;
  *(unsigned char*)(c+0x6e5) = 1;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 120 -- func_ov002_020c5244, 0x020c5244, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c5244
extern "C" {
int func_ov002_020c5244(void)
{
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 121 -- _ZN6Player12GetTalkStateEv, 0x020c524c, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12GetTalkStateEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::GetTalkState()
{
    extern struct State;
    extern State data_ov002_0211046c;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
  if(!_ZN6Player7IsStateERNS_5StateE(((char*)this),&data_ov002_0211046c)) return -1;
  unsigned char v=mStateStep;
  if(v==0) return 0;
  if(v==3) return 2;
  if(v==5||v==7) return 3;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 122 -- _ZN6Player21St_StuckInGround_MainEv, 0x020c52bc, size 0x188 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player21St_StuckInGround_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_StuckInGround_Main()
{
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 anim, int a, Fix12i b, u32 d);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, int b);
    extern void func_ov002_020c5444(void*);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_020a0e40;
    extern u16 data_0209f49e[];
    extern int data_ov002_0211013c[];
    extern u32 data_ov002_0210a578[];
    extern u32 data_ov002_0210a584[];
    extern int _ZNK9Animation12WillHitFrameEi(void*, int);
    extern u8 data_ov002_020ff0ec[];
    switch (mStateWork) {
    case 0:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a578[mStateStep], 0x40000000, 0x1000, 0);
            *(u8*)(((int)((char*)this) + 0x6e5)) =
                *(u8*)(((int)((char*)this) + 0x6e5)) + 1;
            return 1;
        }
        break;
    case 1:
        if (_ZN6Player12FinishedAnimEv(((char*)this)) == 0) {
            if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2) == 0)
                break;
        }
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a584[mStateStep], 0x40000000, 0x1000, 0);
        *(u8*)(((int)((char*)this) + 0x6e5)) =
            *(u8*)(((int)((char*)this) + 0x6e5)) + 1;
        break;
    case 2:
        {
            void* anim = *(void**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0) << 2) + 0xdc);
            if (_ZNK9Animation12WillHitFrameEi((char*)anim + 0x50, data_ov002_020ff0ec[mStateStep])) {
                func_ov002_020c5444(((char*)this));
                _ZN5Sound9PlayBank0EjRK7Vector3(0xb4, ((char*)this) + 0x74);
            }
        }
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            mAngleY = mPrevAngleY;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 123 -- func_ov002_020c5444, 0x020c5444, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c5444
/* recovered: shared common types */
extern "C" {
void func_ov002_020c5444(char* c){
    extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
  Vector3 v;
  ((int*)&v)[0] = *(int*)(c + 0x5c);
  ((int*)&v)[1] = *(int*)(c + 0x60);
  ((int*)&v)[2] = *(int*)(c + 0x64);
  ((int*)&v)[1] = *(int*)(c + 0x60) + 0x14000;
  unsigned char b = *(unsigned char*)(c + 0x70c);
  if (b == 0) {
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xdb, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xdc, v.x, v.y, v.z);
    return;
  }
  if (b == 1) {
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd8, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd9, v.x, v.y, v.z);
    return;
  }
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc7, v.x, v.y, v.z);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 124 -- _ZN6Player21St_StuckInGround_InitEv, 0x020c54e8, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player21St_StuckInGround_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_StuckInGround_Init()
{
    extern int Player_DisableInteraction(void*);
    extern int Player_ReleaseHeldActor(void*);
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
    extern int func_ov002_020c5444(void*);
    extern int data_ov002_0210a560[];
  Player_DisableInteraction(((void*)this));
  Player_ReleaseHeldActor(((void*)this));
  unsigned char idx = *(unsigned char*)((char*)&mStateStep);
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), data_ov002_0210a560[idx], 0x40000000, 0x1000, 0);
  _ZN5Sound9PlayBank0EjRK7Vector3(*(int*)((char*)&mGroundSoundType)+0x80, (char*)((void*)this)+0x74);
  func_ov002_020c5444(((void*)this));
  *(int*)((char*)&mVertSpeed)=0;
  *(int*)((char*)&mHorzSpeed)=0;
  *(unsigned char*)((char*)&mStateWork)=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 125 -- _ZN6Player24St_BowserEarthquake_MainEv, 0x020c5560, size 0x12c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player24St_BowserEarthquake_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_BowserEarthquake_Main()
{
    extern void _Z14ApproachLinearRiii(void*, int value, int speed);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void Player_AdvanceAnims(void*);
    extern short data_02082214[];
    extern char data_ov002_0211013c[];
    if (mStateStep == 0) {
        _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), 0, 0x1800);
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
    } else {
        int t = mStateTimer;
        int m = t % 16;
        int idx = (unsigned short)(short)(m << 12) >> 4;

        mHorzSpeed = 0;

        int radius = 0x4000 + ((0x6000 - ((t << 12) / 8)) << 3);

        mVertSpeed = 0;

        int sinVal = data_02082214[idx * 2];

        if (sinVal >= 0) {
            mPosY = mGroundY + (int)(((long long)radius * sinVal + 0x800) >> 12);
        } else {
            mPosY = mGroundY - (int)(((long long)radius * sinVal + 0x800) >> 12);
        }

        if (mStateTimer == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 126 -- _ZN6Player24St_BowserEarthquake_InitEv, 0x020c568c, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player24St_BowserEarthquake_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_BowserEarthquake_Init()
{
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
  if(mStateStep==0){
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x12,0x40000000,0x1000,0);
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x47,0,0x1000,0);
    mStateTimer=0x30;
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 127 -- func_ov002_020c56f0, 0x020c56f0, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c56f0
extern "C" {
int func_ov002_020c56f0(unsigned char* c, unsigned char arg)
{
    extern int func_ov002_020d82f0(void*);
    extern int func_ov002_020d91e0(void*, int a, int b);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int data_ov002_0211010c;
    extern int data_ov002_021103c4;
    extern int func_ov002_020c56f0(void*, int);
    if (func_ov002_020d82f0(c) == 0) return 0;
    c[0x6e3] = arg;
    if (c[0x6e3] == 0 && func_ov002_020d91e0((char*)c, 0x300, 1) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE((char*)c, &data_ov002_0211010c);
        return 1;
    }
    _ZN6Player11ChangeStateERNS_5StateE((char*)c, &data_ov002_021103c4);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 128 -- _ZN6Player15St_DeadPit_MainEv, 0x020c5780, size 0x240 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15St_DeadPit_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DeadPit_Main()
{
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, void*, u32 d);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void*);
    extern void Vec3_RotateYAndTranslate(void*, void*, s16 angle, void*);
    extern void func_ov002_020c9718(void*);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_0209f2d8;
    extern int data_ov002_021104fc[];
    extern void KillPlayer(void);
    extern void func_ov002_020c0108(void*, int);
    switch (mStateStep) {
    case 0:
        break;
    case 1: {
        int b = (data_0209f2d8 == 1);
        if (b) {
            if (mStateWaitTimer == 0) {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021104fc);
                return 1;
            }
        }
        return 1;
    }
    case 2:
    case 5:
        if (mIsAirborne == 0) {
            mHorzSpeed = 0;
            mVertSpeed = 0;
        }
        if (mStateWork == 0) {
            if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
                mStateWork = 1;
                KillPlayer();
            }
        }
        break;
    case 3: {
        func_ov002_020c0108(((char*)this), 0);
        u32 snd = _ZN5Sound8PlayLongEjjjRK7Vector3s(mLoopingSoundHandle, 0, 0x10a, (int*)((char*)&mCamSpacePosX), 0);
        int* p = (int*)((char*)&mSinkDepth);
        mLoopingSoundHandle = snd;
        *p += 0x5000;
        if (mStateWork == 0) {
            if (mSinkDepth >= 0xb4000) {
                mStateWork = 1;
                KillPlayer();
            } else {
                *p += 0x2000;
                if (mStateArg == 0) {
                    if (mSinkDepth >= 0x64) {
                        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x25, (int*)((char*)&mCamSpacePosX));
                        mStateArg = 1;
                    }
                }
            }
        }
        break;
    }
    case 6: {
        int src[3];
        src[0] = 0;
        src[1] = -0x1e000;
        src[2] = 0x64000;
        int* q = *(int**)((char*)&mAttachedActor);
        s16 angle = *(s16*)((char*)q + 0x8e);
        Vec3_RotateYAndTranslate((int*)((char*)&mPosX), (int*)((char*)q + 0x5c), angle, src);
        if (mStateTimer < 30) {
            mOpacity = 0;
        }
    }
    /* fallthrough */
    case 4:
        if (mStateTimer == 1) {
            KillPlayer();
            mOpacity = 0;
        }
        break;
    case 7:
    case 8:
        func_ov002_020c9718((u8*)((char*)this));
        if (mStateWork == 0) {
            if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
                mStateWork = 1;
                KillPlayer();
                mIsUnderwater = 1;
            }
        }
        break;
    case 9:
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 129 -- _ZN6Player15St_DeadPit_InitEv, 0x020c59c0, size 0x308 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15St_DeadPit_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DeadPit_Init()
{
    extern u8 data_0209f2d8;
    extern u8 data_0209f250;
    extern int Player_ReleaseHeldActor(void*);
    extern void func_ov002_020d5cec(void*);
    extern void Player_DisableInteraction(void*);
    extern void HitDeathPlane(int arg);
    extern void FUN_020299f4(void);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void*);
    Player_ReleaseHeldActor(((char*)this));
    if (*(char**)((char*)&mObjInMouth) != 0) {
        int b = (*(u16*)(*(char**)((char*)&mObjInMouth) + 0xc) == 0xbf);
        if (b) {
            func_ov002_020d5cec(*(char**)((char*)&mObjInMouth));
            mStateFlags &= ~2;
            *(int*)(*(char**)((char*)&mObjInMouth) + 0xd0) = 0;
        }
        mUseAltBodyModel = 0;
        *(int*)(((int)*(char**)((char*)&mObjInMouth) + 0xb0)) |= 0x80000;
        *(int*)(((int)*(char**)((char*)&mObjInMouth) + 0xb0)) &= ~0x40000;
        *(int*)(((int)*(char**)((char*)&mObjInMouth) + 0xb0)) &= ~0x20000;
        *(char**)((char*)&mObjInMouth) = 0;
    }
    Player_DisableInteraction(((char*)this));
    {
        int t = 0;
        mIsBodyClsnEnabled = t;
        mStateWork = t;
        switch (mStateStep) {
        case 0:
            break;
        case 1:
            if (data_0209f2d8 == 1) t = 1;
            if (t == 0) {
                int a = 2;
                if (mSurfaceType == 5) a = 0;
                HitDeathPlane(a);
            } else {
                if (mPlayerNo == data_0209f250) FUN_020299f4();
                mStateWaitTimer = 0x1e;
            }
            unk_716 = 1;
            break;
        case 2:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 7, 0x40000000, 0x1000, t);
            mVertSpeed = 0xa000;
            mHorzSpeed >>= 1;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xb, ((char*)this) + 0x74);
            break;
        case 3:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x82, 0x40000000, 0x1000, t);
            mVertSpeed = 0;
            mHorzSpeed = 0;
            mStateArg = 0;
            break;
        case 4:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 2, 0x40000000, 0x1000, t);
            mStateTimer = 0x64;
            mVertSpeed = 0;
            mHorzSpeed = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xb, ((char*)this) + 0x74);
            break;
        case 5:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 6, 0x40000000, 0x1000, t);
            mVertSpeed = 0;
            mHorzSpeed = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xb, ((char*)this) + 0x74);
            break;
        case 6:
            mStateTimer = 0x3c;
            mVertSpeed = t;
            mHorzSpeed = t;
            mVertAccel = t;
            break;
        case 7:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x84, 0x40000000, 0x1000, t);
            mVertAccel = 0;
            mTerminalVelocity = -0x1800;
            mVertSpeed = 0;
            mHorzSpeed = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x34, ((char*)this) + 0x74);
            break;
        case 8:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x83, 0x40000000, 0x1000, t);
            mVertAccel = 0;
            mTerminalVelocity = -0x1800;
            break;
        case 9:
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x36, ((char*)this) + 0x74);
            break;
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 130 -- _ZN6Player14EnterWhirlpoolEv, 0x020c5cc8, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player14EnterWhirlpoolEv
/* recovered: named members + shared header, real C++ method */
/* _ZN6Player14EnterWhirlpoolEv @ 0x20c5cc8 (ov002) -- tail-call veneer to func_ov002_020c5dec (0x20c5dec) with r1=9.
 */
extern "C" {
void Player::EnterWhirlpool()
{
    extern void func_ov002_020c5dec(void*, int);
    func_ov002_020c5dec(((void*)this), 9);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 131 -- func_ov002_020c5cd8, 0x020c5cd8, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c5cd8
extern "C" {
int func_ov002_020c5cd8(char *c, int arg) {
    extern int func_ov002_020c5dec(void*, int n);
    if (func_ov002_020c5dec(c, 6)) {
        *(int*)(c + 0x364) = arg;
        return 1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 132 -- func_ov002_020c5d0c, 0x020c5d0c, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c5d0c
extern "C" {
int func_ov002_020c5d0c(char *c)
{
    extern int func_ov002_020c5dec(void*, int a);
    u16 *p;
    if (*(int *)(c + 0x60) - *(int *)(c + 0x644) >= 0x100000)
        return 0;
    p = (u16 *)(((int)c + 0x6ce));
    *p |= 0x400;
    return func_ov002_020c5dec(c, 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 133 -- func_ov002_020c5d60, 0x020c5d60, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c5d60
extern "C" {
void func_ov002_020c5d60(void* c) {
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int func_ov002_020c5dec(void*, int);
    extern char data_ov002_02110124[];
    extern char data_ov002_02110064[];
  if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110124)) {
    if (*(unsigned char*)((char*)c+0x6e3) == 1) return;
  }
  if (*(int*)((char*)c+0x60) >= (int)0xf8ad0000) return;
  if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110064)) return;
  (*(unsigned short *)(((int)c + 0x6ce))) |= 0x400;
  func_ov002_020c5dec(c, 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 134 -- func_ov002_020c5dec, 0x020c5dec, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c5dec
extern "C" {
int func_ov002_020c5dec(char* c, int r1){
    extern int data_ov002_02110124[];
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
  if (r1 != 1) {
    if (*(unsigned char*)(c+0x709) != 0) return 0;
  }
  *(unsigned char*)(c+0x6e3) = (unsigned char)r1;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110124);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 135 -- _ZN6Player15St_DeadHit_MainEv, 0x020c5e34, size 0x1dc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15St_DeadHit_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DeadHit_Main()
{
    extern void func_ov002_020d94cc(void*);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void*);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void*);
    extern void _Z14ApproachLinearRiii(void*, int a, int b);
    extern u32 _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, bool b);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 a, int b, int d, u32 e);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_ov002_0211117c;
    extern int _ZNK9Animation12WillHitFrameEi(void*, int);
    extern u8 data_ov002_02109db8[];
    extern u32 data_ov002_0210a07c[];
    extern void KillPlayer(void);
    u32 t;

    if (mIsAirborne == 0) {
        if (mStateArg == 0) {
            if (mPrevVertSpeed < 0) {
                mVertSpeed = -mPrevVertSpeed / 3;
                if (mVertSpeed <= 0x1000)
                    mVertSpeed = 0;
            }
            t = mStateStep & 0xf0;
            if (t == 0 || t == 0x10)
                func_ov002_020d94cc(((char *)this));
            _ZN5Sound9PlayBank0EjRK7Vector3(mGroundSoundType + 0x50, ((char *)this) + 0x74);
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x11, ((char *)this) + 0x74);
            mStateArg = 1;
        }
        _Z14ApproachLinearRiii((int *)((char *)&mHorzSpeed), 0, 0x1800);
    }

    switch (mStateWork) {
    case 0:
        if (_ZNK9Animation12WillHitFrameEi( (void *)((char *)mBodyModels[_ZNK6Player14GetBodyModelIDEjb(((char *)this), param1 & 0xff, 0)] + 0x50), data_ov002_02109db8[mStateStep & 1]) != 0)
            mStateWork = 1;
        break;
    case 1:
        if (mIsAirborne != 0) break;
        if (mStateArg == 0) break;
        if (mHorzSpeed != 0) break;
        _ZN6Player7SetAnimEji5Fix12IiEj(((char *)this), data_ov002_0210a07c[mStateStep & 1], 0x40000000, 0x1000, 0);
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xb, ((char *)this) + 0x74);
        mStateWork = 2;
        break;
    case 2:
        if (_ZN6Player12FinishedAnimEv(((char *)this)) != 0) {
            if (data_ov002_0211117c == 0) {
                KillPlayer();
                mStateWork = 3;
            }
        }
        break;
    }

    if (mStateWork != 1)
        Player_AdvanceAnims(((char *)this));
    mPrevVertSpeed = mVertSpeed;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 136 -- _ZN6Player15St_DeadHit_InitEv, 0x020c6010, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15St_DeadHit_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DeadHit_Init()
{
    extern void Player_DisableInteraction(void*);
    extern char* data_0209f318;
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned, int, int, unsigned);
    extern int data_ov002_0210a424[];
    extern void func_0200d89c(void*);
  Player_DisableInteraction(((char*)this));
  mIsBodyClsnEnabled=0;
  int idx=mStateStep&1;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),data_ov002_0210a424[idx],0x40000000,0x1000,0);
  mStateArg=0;
  mStateWork=0;
  func_0200d89c(data_0209f318);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 137 -- func_ov002_020c607c, 0x020c607c, size 0x130 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c607c
extern "C" {
int func_ov002_020c607c(char *self, int p1, int p2, int *outptr)
{
    extern void _ZN9dBgCh_Lin10GetClsnPosEv(void*, void*);
    *(int *)(self + 0x36c) = 0;
    *outptr = (int)0x80000000;

    dBgCh_Lin rc;

    V3 a(*(int *)(self + 0x5c), p1, *(int *)(self + 0x64));
    V3 b(*(int *)(self + 0x5c), p2, *(int *)(self + 0x64));
    V3 pos;
    rc.SetObjAndLine(*(Vector3*)&a, *(Vector3*)&b, (dActor_c *)self);
    if (rc.DetectClsn() != 0) {
        _ZN9dBgCh_Lin10GetClsnPosEv((Vector3*)&pos, &rc);
        *outptr = pos.y;
        if (rc.GetClsnID() != -1) {
            dActor_c *found = dActor_c::FindWithID((u32)rc.GetClsnID());
            *(int *)(self + 0x36c) = (int)found;
            if (found != 0) {
                int flag = (found->mFlags & 0x1000000) ? 1 : 0;
                if (flag != 0) {
                    return 1;
                }
                flag = (found->mFlags & 0x2000000) ? 1 : 0;
                if (flag != 0) {
                    return 2;
                }
            }
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 138 -- func_ov002_020c61ac, 0x020c61ac, size 0x2d0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c61ac
extern "C" {
int func_ov002_020c61ac(char *c) {
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void func_ov002_020de968(void*);
    extern void func_ov002_020bdd2c(void*);
    extern int func_ov002_020c607c(void*, int a, int b, void*);
    extern void _ZN9dBgCh_GndC1Ev(void*);
    extern void _ZN5dBgCh19StartDetectingWaterEv(void*);
    extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void*, void*, void*);
    extern int _ZN9dBgCh_Gnd10DetectClsnEv(void*);
    extern void _ZN9dBgCh_GndD1Ev(void*);
    extern char data_ov002_021102ec;
    extern char data_ov002_021106dc;
    extern char data_ov002_021106f4;
    extern char data_ov002_021102bc;
    extern char data_ov002_0210ffec;
    extern char data_ov002_0211001c;
    extern char data_ov002_021101b4;
    int outA;
    int outB;
    int v[3];
    int obj[0x14];
    int saved;
    int a;
    int b;

    if ((*(unsigned char *)(c + 0x6e9) & 5) == 0)
        return 0;
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102ec) || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021106dc) || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021106f4) || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102bc) || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0210ffec) || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211001c) || *(unsigned char *)(c + 0x706) != 0)
        return 0;
    if ((*(unsigned char *)(c + 0x6e9) & 5) == 5) {
        if (*(unsigned char *)(c + 0x6fd) != 0 || *(unsigned char *)(c + 0x703) != 0) {
            int d;
            int yv, zv, xv;
            zv = *(int *)(c + 0x64);
            d = 0xc8000;
            if (*(unsigned char *)(c + 0x6fd) != 0)
                d = 0x96000;
            yv = *(int *)(c + 0x60) + d;
            xv = *(int *)(c + 0x5c);
            v[0] = xv;
            v[1] = yv;
            v[2] = zv;
            _ZN9dBgCh_GndC1Ev(obj);
            _ZN5dBgCh19StartDetectingWaterEv(obj);
            _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(obj, v, c);
            if (_ZN9dBgCh_Gnd10DetectClsnEv(obj)) {
                *(int *)(c + 0x644) = *(int *)((char *)obj + 0x44);
                *(int *)(c + 0x60) = *(int *)(c + 0x644);
            }
            if (*(unsigned char *)(c + 0x6fd) != 0) {
                func_ov002_020de968(c);
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
            } else {
                func_ov002_020bdd2c(c);
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
                _ZN9dBgCh_GndD1Ev(obj);
                return 1;
            }
            _ZN9dBgCh_GndD1Ev(obj);
        }
    }
    a = func_ov002_020c607c(c, *(int *)(c + 0x60) - 0x20000, *(int *)(c + 0x60) + 0xa0000, &outA);
    saved = *(int *)(c + 0x36c);
    if (outA == 0x80000000)
        return 0;
    b = func_ov002_020c607c(c, *(int *)(c + 0x60) + 0x1e000, *(int *)(c + 0x60) - 0x28000, &outB);
    if (outB == 0x80000000)
        return 0;
    if (saved == *(int *)(c + 0x36c))
        return 0;
    if (outA + 0x20000 < outB)
        return 0;
    {
        int t = outA - outB;
        if (t < 0)
            t = -t;
        if (t >= 0xa0000)
            return 0;
    }
    if ((a | b) == 0)
        return 0;
    *(int *)(c + 0x60) = outB;
    *(unsigned char *)(c + 0x6f2) = 0;
    _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021102ec);
    if (a != 0) {
        *(unsigned char *)(c + 0x6e5) = 1;
        *(unsigned char *)(c + 0x6e6) = (unsigned char)a;
    } else if (b != 0) {
        *(unsigned char *)(c + 0x6e5) = 2;
        *(unsigned char *)(c + 0x6e6) = (unsigned char)b;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 139 -- func_ov002_020c647c, 0x020c647c, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c647c
/* recovered: shared common types */
// func_ov002_020c647c at 0x020c647c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020c647c(char* c, int arg1) {
    extern void _ZN9dBgCh_Lin10GetClsnPosEv(void*, void*);
    extern int func_ov002_020c647c(void*, int);
    Vector3 v1;
    Vector3 v2;
    Vector3 clsnPos;
    dBgCh_Lin line;
    int z1 = *(int*)(c + 0x64);
    int x1 = *(int*)(c + 0x5c);
    v1.x = x1;
    v1.y = arg1 - 0x1e000;
    v1.z = z1;
    int x2 = *(int*)(c + 0x5c);
    int z2 = *(int*)(c + 0x64);
    v2.x = x2;
    v2.z = z2;
    v2.y = arg1 + 0xaa000;
    int r5 = -64;
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&clsnPos, &line);
        int d = (clsnPos.y - *(int*)(c + 0x60)) / 0x1000;
        if (d > 0xa0) d = 0xa0;
        if (d < 0) d = 0;
        r5 = d;
    }
    return r5;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 140 -- func_ov002_020c6538, 0x020c6538, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c6538
extern "C" {
int func_ov002_020c6538(char* c) {
    extern int func_ov002_020c6538(void*);
    dBgCh_Gnd rg;
    Vector3 v;
    int y;
    v.x = *(int*)(c+0x5c);
    y = *(int*)(c+0x60);
    v.y = y;
    v.z = *(int*)(c+0x64);
    v.y = y + 0x32000;
    rg.SetObjAndPos(v, (dActor_c*)c);
    if (rg.DetectClsn() == 0) {
        return 0;
    }
    {
        int hit = rg.clsnY;
        int d = hit - *(int*)(c+0x60);
        if (d < 0) d = -d;
        if (d >= 0x64000) {
            return 0;
        }
        *(int*)(c+0x60) = hit;
        *(int*)(c+0x688) = *(int*)(c+0x60);
        return 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 141 -- _ZN6Player17St_Squish_CleanupEv, 0x020c65f8, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17St_Squish_CleanupEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Squish_Cleanup()
{
    int val = 0x1000;
    if (mIsMega != 0)
        val = 0x3000;

    mScaleX = val;
    mScaleY = val;
    mScaleZ = val;

    *(int *)((char *)&mdCcAcPos_c.flags) &= ~4;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 142 -- _ZN6Player14St_Squish_MainEv, 0x020c662c, size 0x2dc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player14St_Squish_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Squish_Main()
{
    extern u8 data_ov002_0211117c;
    extern char data_ov002_0211013c;
    extern char data_ov002_021101b4;
    extern int func_ov002_020d91e0(void*, int a, int b);
    extern void func_ov002_020db8bc(void*, int a);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int _ZN6Player9GetHealthEv(void*);
    extern int func_ov002_020c6908(void*);
    extern int func_ov002_020c647c(void*, int);
    extern int func_ov002_020c6538(void*);
    extern void KillPlayer(void);
    switch (mStateStep) {
    case 0:
        if (func_ov002_020c6908(((char *)this)) != 0)
            return 1;
        {
            int r = func_ov002_020c647c(((char *)this), mAttachOffsetY);
            if (r >= 0) {
                mScaleY = (r << 12) / 160;
                if (r >= 0x20)
                    break;
                if (mStatePhase != 2)
                    break;
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 7, ((char *)this) + 0x74);
                mScaleY = 0x100;
                mStateStep = 2;
                mStateArg = 1;
                if (mIsMetal == 0) {
                    if (func_ov002_020d91e0(((char *)this), 0x300, 1) != 0)
                        mStateArg = 2;
                }
                {
                    char *a = *(char **)((char *)&mAttachedActor);
                    if (a != 0) {
                        u16 id = *(u16 *)(a + 0xc);
                        int b1 = (id == 0xa4);
                        int b2;
                        if (b1 || (b2 = (id == 0xa5)) != 0)
                            *(u8 *)(a + 0x407) = 1;
                    }
                }
            } else {
                mStateTimer = 0xa;
                (*(u8 *)(int)((char *)&mStateStep))++;
            }
        }
        break;
    case 1:
        if (mStateTimer != 0) {
            (*(int *)(int)((char *)&mScaleY)) += 0x100;
            if (mScaleY > 0x1000)
                mScaleY = 0x1000;
        } else {
            mScaleY = 0x1000;
            _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
        }
        break;
    case 2:
        if (func_ov002_020c6908(((char *)this)) != 0)
            return 1;
        if (func_ov002_020c647c(((char *)this), mAttachOffsetY) < 0) {
            if (mStateArg == 1) {
                func_ov002_020db8bc(((char *)this), 1);
                _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
            } else {
                (*(u8 *)(int)((char *)&mStateStep))++;
                mStateArg = 0;
            }
        }
        break;
    case 3:
        if (mStateWork == 2 && func_ov002_020c6538(((char *)this)) == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_021101b4);
            return 1;
        }
        if (mStateArg != 0)
            break;
        if (_ZN6Player9GetHealthEv(((char *)this)) != 0) {
            func_ov002_020db8bc(((char *)this), 1);
            _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
        }
        if (data_ov002_0211117c != 0)
            break;
        KillPlayer();
        (*(u8 *)(int)((char *)&mStateArg))++;
        break;
    case 4:
        if (mStateTimer != 0)
            break;
        if (mStateArg != 1) {
            mStateStep = 3;
            break;
        }
        func_ov002_020db8bc(((char *)this), 1);
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
        break;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 143 -- func_ov002_020c6908, 0x020c6908, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c6908
extern "C" {
int func_ov002_020c6908(char* c){
    struct State;
    extern State data_ov002_021101b4;
    extern int func_ov002_020c6538(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020c6908(void*);
  if (*(unsigned char*)(c+0x6e5) == 1) {
    int old;
    *(int*)(c+0x60) = *(int*)(c+0x688);
    old = *(int*)(c+0x60);
    if (func_ov002_020c6538(c) == 0) {
      _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
      return 1;
    }
    *(int*)(c+0x60) = old;
    goto ret0;
  } else if (*(unsigned char*)(c+0x6e5) == 2) {
    if (func_ov002_020c6538(c) == 0) {
      _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
      return 1;
    }
    goto ret0;
  }
ret0:
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 144 -- _ZN6Player14St_Squish_InitEv, 0x020c69a0, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player14St_Squish_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Squish_Init()
{
    typedef int Fix12i;
    extern void Player_ReleaseHeldActor(void*);
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
  Player_ReleaseHeldActor(((void*)this));
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), 0x47, 0, 0x1000, 0);
  *(unsigned char*)((char*)&mStateWork) = 0;
  *(unsigned char*)((char*)&mStatePhase) = 0;
  *(int*)((char*)&mHorzSpeed) = 0;
  *(int*)((char*)&mVertSpeed) = 0;
  *(int*)((char*)&mVertAccel) = 0;
  *(unsigned char*)((char*)&mStateStep) = 0;
  *(unsigned char*)((char*)&unk_716) = 1;
  *(int*)((char*)&mAttachOffsetY) = *(int*)((char*)&mPosY);
  {
    int *p = (int *)(((int)((void*)this) + 0x2ec));
    *p |= 4;
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 145 -- _ZN6Player12Unk_020c6a10Ej, 0x020c6a10, size 0xcc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12Unk_020c6a10Ej
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020c6a10(unsigned int arg_)
{
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int func_ov002_020d91e0(void*, u32 a, int b);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern char data_ov002_021102ec;
    extern int func_ov002_020d82f0(void*);
    u32 arg = (u32)arg_;

    int flag;

    if (!(mClsnFlags & 1)) return 0;
    if (_ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_021102ec)) return 0;
    if (!func_ov002_020d82f0(((char*)this))) return 0;

    flag = 1;
    if (mIsMetal == 0) {
        if (func_ov002_020d91e0(((char*)this), arg << 8, 1))
            flag = 0;
    }

    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 7, ((char*)this) + 0x74);
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_021102ec);
    mScaleY = 0x100;
    mStateStep = 4;
    mStateTimer = 0x1e;
    mStateArg = flag;
    unk_6f2 = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 146 -- func_ov002_020c6adc, 0x020c6adc, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c6adc
extern "C" {
int func_ov002_020c6adc(char* c)
{
    extern int data_ov002_021102a4[];
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020c6adc(void*);
    unsigned short v = (unsigned short)(*(unsigned short*)(c + 0x6ce) & 8);
    if (v == 0)
        return 0;
    *(unsigned short*)(c + 0x6ce) &= ~8;
    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021102a4);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 147 -- _ZN6Player16St_Teleport_MainEv, 0x020c6b3c, size 0x288 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16St_Teleport_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Teleport_Main()
{
    typedef struct Obj { s16 x, y, z; u16 param; } Obj;
    extern Obj* GetTeleportDestObj(int i);
    extern void func_02035860(void*, void*);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void*);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void*);
    extern void _ZN9dBgCh_GndC1Ev(void*);
    extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void*, void*, void*);
    extern int _ZN9dBgCh_Gnd10DetectClsnEv(void*);
    extern void _ZN9dBgCh_GndD1Ev(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern u32 func_02012790(u32 a);
    extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16* p, u16 a, s16 b);
    extern void Player_AdvanceAnims(void*);
    extern char* data_0209f318;
    extern int data_0209f32c;
    extern u8 data_0209f284;
    extern int data_ov002_0211067c[];
    extern int data_ov002_0211013c[];
    extern void func_0200d1e4(void*);
    extern void func_02020388(int);
    switch (mStateStep) {
    case 0:
        if (mOpacity > 1) {
            (*(u8*)(((int)((char*)this) + 0x6f5)))--;
            break;
        }
        (*(u8*)(((int)((char*)this) + 0x6e5)))++;
        if (mStateWork >= 0x10) {
            Obj* obj = GetTeleportDestObj((u8)(mTeleportId - 1));
            int tx = obj->x << 12;
            int tz = obj->z << 12;
            int ty = obj->y << 12;
            mPosX = tx;
            mPosY = ty;
            mPosZ = tz;
            mAreaId = obj->param & 0xf;
            mAngleY = ((obj->param >> 4) & 0xf) << 12;
            mPrevAngleY = mAngleY;
            mGroundY = 0x80000000;
            unk_64c = 0x80000000;
            func_02035860(((char*)this) + 0x380, ((char*)this) + 0x5c);
            mOpacity = 0;
            func_0200d1e4(data_0209f318);
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x19, ((char*)this) + 0x74);
            _ZN5Sound9PlayBank0EjRK7Vector3(0x19, ((char*)this) + 0x74);
            mStateStep = 1;
        }
        break;
    case 1:
        if (mStateWork != 0) {
            if (mStateWork >= 0x10) {
                if (data_0209f32c > mPosY + 0x64000)
                    mIsUnderwater = 1;
            }
            (*(u8*)(((int)((char*)this) + 0x6e5)))--;
            break;
        }
        (*(u8*)(((int)((char*)this) + 0x6f5)))++;
        func_02020388(mPlayerNo);
        if (mOpacity >= 0x1f) {
            int hit = 0;
            mOpacity = 0x1f;
            if (data_0209f32c >= mPosY + 0x64000) {
                Vector3 v;
                char rc[0x50];
                _ZN9dBgCh_GndC1Ev(rc);
                {
                    int vz = mPosZ;
                    int vy = data_0209f32c + 0x64000;
                    int vx = mPosX;
                    v.x = vx;
                    v.y = vy;
                    v.z = vz;
                }
                _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rc, &v, ((char*)this));
                if (_ZN9dBgCh_Gnd10DetectClsnEv(rc))
                    hit = 1;
                _ZN9dBgCh_GndD1Ev(rc);
            }
            if (hit)
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211067c);
            else
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            data_0209f284 = 1;
            mTeleportTimer = 0x3c;
            func_02012790(0x24);
        }
        break;
    }
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16*)0x4000050, 0x3f, mStateWork);
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 148 -- _ZN6Player16St_Teleport_InitEv, 0x020c6dc4, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16St_Teleport_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Teleport_Init()
{
    extern void Player_DisableInteraction(void*);
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned, int, int, unsigned);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned, void*);
  Player_DisableInteraction(((char*)this));
  mStateWork=0;
  mStateStep=0;
  mHorzSpeed=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x47,0,0x1000,0);
  _ZN5Sound9PlayBank0EjRK7Vector3(0x19,(char*)((char*)this)+0x74);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 149 -- func_ov002_020c6e14, 0x020c6e14, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c6e14
/* func_ov002_020c6e14 at 0x020c6e14
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern "C" {
int func_ov002_020c6e14(void* actor)
{
    extern unsigned char data_0209f2ac;
    extern short data_ov002_020ff1a0[];
    extern void* data_0209f318[];
    extern int NumStars(void);
    extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void*, void*, unsigned int msg, const void* pos, unsigned int a, unsigned int b);
    extern void _ZN6Camera9SetFlag_3Ev(void*);
    int idx;

    if (data_0209f2ac != 0) {
        idx = -1;
        switch (NumStars()) {
        case 1:  idx = 0; break;
        case 3:  idx = 1; break;
        case 8:  idx = 2; break;
        case 12: idx = 3; break;
        case 30: idx = 4; break;
        case 50: idx = 5; break;
        case 80: idx = 6; break;
        }

        if (idx == -1)
            return 0;

        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(actor, actor, data_ov002_020ff1a0[idx], 0, 0, 2)) {
            _ZN6Camera9SetFlag_3Ev(data_0209f318[0]);
            return 1;
        }
    }

    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 150 -- _ZN6Player21St_LevelEnter_CleanupEv, 0x020c6f34, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player21St_LevelEnter_CleanupEv
/* recovered: named members + shared header, real C++ method */
/* Player::St_LevelEnter_Cleanup() at 0x020c6f34 (ov002).
 * Cleanup hook of the ST_LEVEL_ENTER player state-machine state; a no-op
 * that returns VS_FAIL (1). The implicit Player* this is unused.
 */
extern "C" {
s32 Player::St_LevelEnter_Cleanup()
{
    (void)this;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 151 -- _ZN6Player18St_LevelEnter_MainEv, 0x020c6f3c, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player18St_LevelEnter_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Level-entry cutscene, per frame. mStateWork indexes a table of
 * POINTERS TO MEMBER FUNCTION -- the same ARM/Itanium pmf representation
 * Player::State uses -- and the selected step runs against the player. Six of
 * the mStateStep values additionally publish the player to the global at
 * data_0209f318+0x118, presumably as the camera's focus.
 */
extern "C" {
int Player::St_LevelEnter_Main()
{
    struct ObjLE { char pad[0x118]; void *f118; };
    extern void (Player::*data_ov002_0211075c[])();
    extern ObjLE *data_0209f318;
    extern void Player_AdvanceAnims(void*);
    switch (mStateStep) {
    case 8: case 9: case 11: case 12: case 16: case 18:
        data_0209f318->f118 = this;
        break;
    case 10: case 13: case 14: case 15: case 17:
    default:
        break;
    }
    (this->*data_ov002_0211075c[mStateWork])();
    if (mStateArg == 0) Player_AdvanceAnims(this);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 152 -- func_ov002_020c6fe4, 0x020c6fe4, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c6fe4
extern "C" {
void func_ov002_020c6fe4(char* c)
{
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, u32 b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int frame);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void*);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 a, int b, Fix12i c, u32 d);
    extern u8 data_0209f20c;
    int id;

    id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
    if (_ZNK9Animation12WillHitFrameEi(*(char**)(c + id * 4 + 0xdc) + 0x50, 0x58))
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0x35, c + 0x74);

    id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
    if (_ZNK9Animation12WillHitFrameEi(*(char**)(c + id * 4 + 0xdc) + 0x50, 0x75))
        data_0209f20c = 1;

    if (_ZN6Player12FinishedAnimEv(c)) {
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
        *(u8*)(c + 0x6e5) = 2;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 153 -- func_ov002_020c70ac, 0x020c70ac, size 0xe8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c70ac
extern "C" {
void func_ov002_020c70ac(char* c) {
    if (*(u16*)(c+0x6a4) != 0) {
        *(int*)(c+0xa8) = 0;
        *(int*)(c+0x98) = 0;
        return;
    }
    *(u16*)(c+0x6a6) = 0;
    *(u8*)(c+0x6e5) = 0;
    *(int*)(c+0x9c) = -0x4000;
    switch (*(u8*)(c+0x6e3)) {
    case 12:
        *(s16*)(c+0x8e) = *(s16*)(c+0x8e) + 0x8000;
    case 11:
    case 18:
        *(int*)(c+0x98) = 0x20000;
        *(u16*)(c+0x6a6) = 6;
        break;
    case 9:
        *(s16*)(c+0x8e) = *(s16*)(c+0x8e) + 0x8000;
    case 8:
    case 16:
        *(int*)(c+0xa8) = 0x40000;
        *(int*)(c+0x98) = 0x18000;
        break;
    case 13:
        *(int*)(c+0xa8) = 0x36000;
        *(int*)(c+0x98) = 0xa000;
        *(u8*)(c+0x70c) = 0;
        break;
    case 17:
        *(int*)(c+0xa8) = 0x3c000;
        *(int*)(c+0x98) = 0xa000;
        *(u8*)(c+0x70c) = 0;
        break;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 154 -- func_ov002_020c7194, 0x020c7194, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c7194
extern "C" {
void func_ov002_020c7194(char* c){
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern int func_ov002_020c44c4(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int data_ov002_02110214[];
  if(_ZN6Player12FinishedAnimEv(c) == 0) return;
  *(unsigned char*)(c+0x6e3) = 1;
  if(func_ov002_020c44c4(c) != 0) return;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110214);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 155 -- func_ov002_020c71e0, 0x020c71e0, size 0xc4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c71e0
extern "C" {
void func_ov002_020c71e0(char* c) {
    struct State;
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020c6e14(void*);
    extern int func_ov002_020c44c4(void*);
    extern void func_ov002_020be008(void*);
    extern unsigned char data_0209f2bc[];
    extern unsigned char data_0209f2fc[];
    extern struct State data_ov002_0211067c;
    extern struct State data_ov002_0211013c;
    extern void func_ov002_020c71e0(void*);
    *(unsigned char*)(c+0x711) = 1;
    if (data_0209f2bc[0] != 0) return;
    if (*(unsigned short*)(c+0x6a6) == 0) {
        *(short*)(c+0x94) = *(short*)(c+0x8e);
        if (*(unsigned char*)(c+0x6e3) == 1) {
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
            if (*(signed char*)(c+0x719) < 0 && data_0209f2fc[0] == 1) {
                if (func_ov002_020c6e14(c) != 0) return;
            }
            if (func_ov002_020c44c4(c) != 0) return;
        }
    }
    func_ov002_020be008(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 156 -- func_ov002_020c72a4, 0x020c72a4, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c72a4
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
void func_ov002_020c72a4(void* thisptr)
{
    extern int SurfaceInfo_TestFlag0x20(void*);
    extern int data_0209f32c;
    extern int data_0209212c;
    extern void func_ov002_020c71e0(void*);
    unsigned char* r4 = (unsigned char*)thisptr;
    dBgCh_Gnd rg;
    Vector3 v;
    int z = *(int*)(r4 + 0x64);
    int d = data_0209212c;
    int x = *(int*)(r4 + 0x5c);
    v.x = x;
    v.y = d;
    v.z = z;
    rg.mProbeHeight = d << 1;
    rg.SetObjAndPos(v, (dActor_c*)thisptr);
    rg.StartDetectingWater();
    if (rg.DetectClsn()) {
        if (SurfaceInfo_TestFlag0x20((int*)&rg.surface) != 0) {
            *(int*)(r4 + 0x64c) = rg.clsnY;
            data_0209f32c = *(int*)(r4 + 0x64c);
        }
    }
    rg.mProbeHeight = 0x1f4000;
    func_ov002_020c71e0(thisptr);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 157 -- func_ov002_020c7350, 0x020c7350, size 0x2a0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c7350
extern "C" {
void func_ov002_020c7350(char *c)
{
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned int u32;
    typedef signed char s8;
    typedef struct { int x, y, z; } Vec3i;
    typedef struct { short x, y, z; } Vector3_16;
    extern int _ZN6Player6IsAnimEj(void*, unsigned int id);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int frame);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void*);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int anim, int a, int b, unsigned int d);
    extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void*, void*, int e, int f);
    extern void func_ov089_0213115c(void*, int i);
    extern u8 data_0209f20c;
    extern u8 data_0209f2d8;
    extern char data_0209caa0[];
    int id;
    char *anim;

    if (*(u8 *)(c + 0x6de) != 0)
        return;

    if (_ZN6Player6IsAnimEj(c, 0x10)) {
        id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
        anim = (char *)((int *)(c + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x52)) {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x27, (Vec3i *)(c + 0x74));
        }
    }

    if (_ZN6Player6IsAnimEj(c, 0x85)) {
        id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
        anim = (char *)((int *)(c + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x20)) {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x35, (Vec3i *)(c + 0x74));
        }
    }

    if (_ZN6Player6IsAnimEj(c, 0xa5)) {
        id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
        anim = (char *)((int *)(c + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x23)) {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x37, (Vec3i *)(c + 0x74));
        }
    }

    id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
    anim = (char *)((int *)(c + 0xdc))[id] + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 0x8e)) {
        if (_ZN6Player6IsAnimEj(c, 0xa5)) {
            data_0209f20c = 1;
        }
    }

    if (!_ZN6Player12FinishedAnimEv(c))
        return;

    {
        u8 st = *(u8 *)(c + 0x6e3);
        if (st == 0x10 || st == 0x12) {
            void *actor;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x94, 0x40000000, 0x1000, 0);
            actor = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x11a, *(s8 *)(c + 0x719), (Vec3i *)(c + 0x5c), (Vector3_16 *)(c + 0x8c), *(s8 *)(c + 0xcc), -1);
            if (*(int *)(c + 8) == 2) {
                func_ov089_0213115c(actor, 4);
            } else {
                func_ov089_0213115c(actor, 1);
            }
            *(u8 *)(c + 0x6e5) = 6;
            return;
        }
    }

    if (_ZN6Player6IsAnimEj(c, 0x85)) {
        data_0209f20c = 1;
    }

    {
        u8 v = data_0209f2d8;
        int isOne = (v == 1);
        if (isOne != false)
            goto setanim2;
        if (!(*(int *)(data_0209caa0 + 8) & 0x80))
            goto skip_setanim2;
        {
            int isTwo = (v == 2);
            if (isTwo != false)
                goto skip_setanim2;
        }
    setanim2:
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
    skip_setanim2:
        ;
    }

    *(u8 *)(c + 0x6e5) = 2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 158 -- func_ov002_020c75f0, 0x020c75f0, size 0x248 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c75f0
extern "C" {
void func_ov002_020c75f0(char *c)
{
    extern int _ZN6Player6IsAnimEj(void*, u32 anim);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, u32 id, int b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int frame);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void*);
    extern void func_ov002_020c2f64(void*);
    extern void _ZN6Player4HealEi(void*, int amt);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 anim, int a, Fix12i b, u32 d);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void*);
    extern void func_02012694(u32 id, void*);
    extern u8 data_0209f2fc;
    if (_ZN6Player6IsAnimEj(c, 0x56))
    {
        int model = _ZNK6Player14GetBodyModelIDEjb(c, (*(int *)(c + 8)) & 0xff, 0);
        if (_ZNK9Animation12WillHitFrameEi((char *)(*(int *)(c + model * 4 + 0xdc)) + 0x50, 0))
        {
            _ZN5Sound9PlayBank0EjRK7Vector3(6, c + 0x74);
        }
    }

    if (*(u8 *)(c + 0x6de) == 0)
    {
        func_ov002_020c2f64(c);
        *(u8 *)(c + 0x70c) = 0;
        *(u8 *)(c + 0x6e5) = 1;
        if (data_0209f2fc == 1 || data_0209f2fc == 2)
        {
            _ZN6Player4HealEi(c, 0x880);
        }
        switch (*(u8 *)(c + 0x6e3))
        {
        case 3:
        case 10:
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x51, 0x40000000, 0x1000, 0);
            break;
        case 4:
            *(int *)(c + 0x98) = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x55, 0x40000000, 0x1000, 0);
            break;
        case 6:
        case 7:
            *(u8 *)(c + 0x6de) = 1;
            *(u8 *)(c + 0x6df) = 0;
            *(int *)(((int)c + 0x98)) >>= 2;
            *(int *)(c + 0xa8) = -(*(int *)(c + 0xa8) >> 2);
            *(u8 *)(c + 0x70c) = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x13, 0x40000000, 0x1000, 0);
            break;
        case 5:
        case 8:
        case 9:
        case 11:
        case 12:
        case 15:
            *(int *)(c + 0x98) = 0;
            break;
        case 13:
        case 16:
        case 17:
        case 18:
            *(int *)(c + 0x98) = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x52, 0x40000000, 0x1000, 0);
            break;
        }
        if (data_0209f2fc == 2)
        {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x24, c + 0x74);
        }
    }
    else
    {
        u8 t = *(u8 *)(c + 0x6e3);
        if (t == 0xb || t == 0xc || t == 0x12)
        {
            if (*(u16 *)(c + 0x6a6) == 1)
            {
                func_02012694(0x28, c + 0x74);
            }
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 159 -- _ZN6Player18St_LevelEnter_InitEv, 0x020c7838, size 0x484 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player18St_LevelEnter_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_LevelEnter_Init()
{
    typedef struct dBgCh_Gnd {
        char pre[0x10];
        char result[0x34];
        int clsnY;
        char post[0x8];
    } dBgCh_Gnd;
    extern int _ZN8SaveData16HasPlayerLostCapEv(void);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 anim, int a, int fix, u32 b);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, int b);
    extern void _ZN9Animation8SetFlagsEi(void*, int flags);
    extern void Player_DisableInteraction(void*);
    extern void _ZN9dBgCh_GndC1Ev(void*);
    extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void*, void*, void*);
    extern int _ZN9dBgCh_Gnd10DetectClsnEv(void*);
    extern int func_ov002_020c7cbc(void*);
    extern void func_02012790(int a);
    extern void _ZN6Player16InitWingFeathersEb(void*, int b);
    extern void _ZN9dBgCh_GndD1Ev(void*);
    extern u8 data_0209f2d8;
    extern s16 data_02082214[];
    extern signed char data_0209f2f8;
    extern u8 data_0209f264;
    extern u8 data_0209f2fc;
    extern u32 data_ov002_0210a7e8[];
    dBgCh_Gnd rg;

    u32 anim = data_ov002_0210a7e8[mStateStep];
    if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
        if (anim == 0x85) anim = 0xa5;
    }
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), anim, 0, 0x1000, 0);

    u8 st = mStateStep;
    if (st != 3 && st != 0xa && st != 0) {
        _ZN9Animation8SetFlagsEi( *(char**)((((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), (u8)param1, 0) * 4)) + 0xdc) + 0x50, 0x40000000);
        if (mStateStep == 2) {
            char* p = *(char**)((((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), (u8)param1, 0) * 4)) + 0xdc) + 0x50;
            *(int*)(p + 8) = 0x14000;
        }
    }

    mKeyModelId = -1;
    Player_DisableInteraction(((char*)this));
    mJumpComboStage = 0;
    mHorzSpeed = 0;
    mStateArg = 0;
    mPrevAngleY = mAngleY;
    mStateWork = 0;
    _ZN9dBgCh_GndC1Ev(&rg);

    switch (mStateStep) {
    case 0: {
        Vector3 pos;
        int hit;
        int y = mPosY;
        int z = mPosZ;
        int x = mPosX;
        int w = y + 0xc8000;
        pos.x = x;
        pos.y = w;
        pos.z = z;
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &pos, ((char*)this));
        hit = 0x80000000;
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0) hit = rg.clsnY;
        mPosY = hit;
        mStateWork = 2;
        {
            int b = (data_0209f2d8 == 2);
            if (b == 0) mStateWaitTimer = 0x1e;
            else mStateWaitTimer = 0;
        }
        break;
    }
    case 1:
        mVertAccel = 0;
        mStateWork = 4;
        mIsUnderwater = 1;
        break;
    case 5:
    case 15:
        mStateArg = 1;
        break;
    case 11:
    case 12: {
        s32 idx;
        s16 cosv, sinv;
        if (func_ov002_020c7cbc(((char*)this)) != 0) mStateStep = 0x12;
        *(int*)((char*)&mPosY) += 0x64000;
        idx = ((s32)(u16)(s16)(mAngleY + 0x8000) >> 4) * 2;
        cosv = data_02082214[idx];
        sinv = data_02082214[idx + 1];
        *(int*)((char*)&mPosX) += (int)(((s64)0xa0000 * cosv + 0x800) >> 12);
        *(int*)((char*)&mPosZ) += (int)(((s64)0xa0000 * sinv + 0x800) >> 12);
        mStateTimer = 0x20;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        break;
    }
    case 8:
    case 9:
        if (func_ov002_020c7cbc(((char*)this)) != 0) mStateStep = 0x10;
        mStateTimer = 2;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        break;
    case 13:
    case 17: {
        u8 st2 = mStateStep;
        if (st2 == 0x11) mStateTimer = 2;
        else mStateTimer = 0x20;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        if (data_0209f2f8 == 0x18) func_02012790(0x16);
        else if (data_0209f2f8 == 0x19) func_02012790(0x17);
        break;
    }
    case 2:
        if (param1 != 0) {
            mStateStep = 0xa;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a7e8[mStateStep], 0, 0x1000, 0);
        } else {
            _ZN6Player16InitWingFeathersEb(((char*)this), 0);
            mStateWork = 3;
            mVertSpeed = 0;
            mVertAccel = 0;
        }
        break;
    default:
        break;
    }

    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    {
        u8 c2 = data_0209f264;
        if (data_0209f2f8 == 2) {
            if (c2 == 0 || (c2 == 0xe && data_0209f2fc != 1)) {
                func_02012790(0x1d);
            }
        } else if (data_0209f2f8 == 1 && (c2 == 6 || c2 == 7 || (u32)(c2 - 9) <= 1)) {
            func_02012790(0x1d);
        }
    }

    _ZN9dBgCh_GndD1Ev(&rg);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 160 -- func_ov002_020c7cbc, 0x020c7cbc, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c7cbc
extern "C" {
int func_ov002_020c7cbc(char* self)
{
    extern u8 data_0209f2fc;
    extern signed char data_02092124;
    extern void LoadKeyModels(int idx);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 a, int b, Fix12i c, u32 d);
    if (data_0209f2fc == 1) {
        switch (data_02092124) {
        case 0x24: *(u8*)(self + 0x719) = 0; break;
        case 0x26: *(u8*)(self + 0x719) = 1; break;
        case 0x2d: *(u8*)(self + 0x719) = 2; break;
        case 0x2f: *(u8*)(self + 0x719) = 3; break;
        case 0x31: *(u8*)(self + 0x719) = 4; break;
        }

        if (*(signed char*)(self + 0x719) >= 0) {
            LoadKeyModels(*(signed char*)(self + 0x719));
            *(u8*)(self + 0x718) |= 0x10;
            _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x53, 0x40000000, 0x1000, 0);
            return 1;
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 161 -- func_ov002_020c7dd0, 0x020c7dd0, size 0xb4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c7dd0
extern "C" {
void func_ov002_020c7dd0(PlayerRaw* p, int i) {
    struct State;
    extern unsigned char data_ov002_020ff240[];
    extern unsigned char data_0209f2fc;
    extern int data_ov002_021104b4;
    extern void _ZN6Player4HealEi(void*, int amt);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    unsigned char g;
    *(unsigned char*)((char*)p + 0x6e3) = data_ov002_020ff240[i];
    g = data_0209f2fc;
    if (g == 2) {
        unsigned char s = *(unsigned char*)((char*)p + 0x6e3);
        switch (s) {
        case 4:
            *(unsigned char*)((char*)p + 0x6e3) = 5;
            break;
        case 8:
            *(unsigned char*)((char*)p + 0x6e3) = 9;
            break;
        case 0xb:
            *(unsigned char*)((char*)p + 0x6e3) = 0xc;
            break;
        }
        _ZN6Player4HealEi(p, -0x880);
        _ZN6Player4HealEi(p, 0x100);
    } else if (g == 1) {
        if (*(unsigned char*)((char*)p + 0x6e3) == 4) {
            *(unsigned char*)((char*)p + 0x6e3) = 0xf;
        }
    }
    _ZN6Player11ChangeStateERNS_5StateE(p, (struct State*)&data_ov002_021104b4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 162 -- _ZN6Player15IsEnteringLevelEv, 0x020c7e84, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15IsEnteringLevelEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsEnteringLevel()
{
    extern int _ZN6Player20IsStateEnteringLevelEv(void*);
  if(_ZN6Player20IsStateEnteringLevelEv(((char*)this))==0) return 0;
  unsigned char s=mStateStep;
  if(s==8||s==9||s==0x10) return 1;
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 163 -- _ZN6Player20IsStateEnteringLevelEv, 0x020c7ed4, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player20IsStateEnteringLevelEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsStateEnteringLevel()
{
    extern int data_ov002_021104b4[];
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int data_ov002_0211025c[];
  if(_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_021104b4)) return 1;
  return _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_0211025c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 164 -- func_ov002_020c7f10, 0x020c7f10, size 0xe8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c7f10
extern "C" {
int func_ov002_020c7f10(char* c)
{
    struct State;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, State& s);
    extern void func_ov002_020ce9c8(void*);
    extern void func_ov002_020c7ff8(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int d, unsigned int e);
    extern State data_ov002_0211022c;
  unsigned char v;
  if (!_ZN6Player7IsStateERNS_5StateE(c, data_ov002_0211022c)) goto fail;
  if (*(unsigned char*)(c + 0x6e3) != 0) goto fail;
  v = *(unsigned char*)(c + 0x70a);
  if (v == 0 || v == 1 || v == 0x11 || (unsigned char)(v + 0xfe) <= 1) {
    if (*(unsigned char*)(c + 0x70c) != 0) goto fail;
    func_ov002_020ce9c8(c);
    *(int*)(c + 0x9c) = 0;
    *(unsigned char*)(c + 0x706) = 1;
    *(unsigned char*)(c + 0x70c) = 1;
    func_ov002_020c7ff8(c);
    return 1;
  }
  if (v == 8) {
    func_ov002_020ce9c8(c);
    *(int*)(c + 0x9c) = 0;
    *(unsigned char*)(c + 0x706) = 1;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6b, 0x40000000, 0x1000, 0);
    *(unsigned char*)(c + 0x6e3) = 9;
    goto fail;
  }
fail:
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 165 -- func_ov002_020c7ff8, 0x020c7ff8, size 0x130 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c7ff8
extern "C" {
void func_ov002_020c7ff8(char *self)
{
    extern void *data_0209f318;
    extern void func_0200d4b0(void*, u8 playerID, int x);
    extern void func_0200d508(void*, u8 playerID);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 anim, int a, int fix, u32 e);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void*);
    extern void func_02012790(int x);
    u8 s = *(u8 *)(self + 0x70a);
    void *g = data_0209f318;
    if (s == 0 || s == 1 || s == 0x11) {
        func_0200d4b0(g, *(u8 *)(self + 0x6d8), -1);
    } else if (s == 3) {
        func_0200d4b0(g, *(u8 *)(self + 0x6d8), 5);
    } else {
        func_0200d508(g, *(u8 *)(self + 0x6d8));
    }
    if (*(u8 *)(self + 0x706) != 0) {
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0xac, 0x40000000, 0x1000, 0);
        *(int *)(self + 0x9c) = 0;
    } else {
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x19, 0x40000000, 0x1000, 0);
    }
    {
        u8 t = *(u8 *)(self + 0x70a);
        if (t == 0 || t == 3) {
            *(u8 *)(self + 0x6e3) = 1;
        } else {
            *(u8 *)(self + 0x6e3) = 2;
        }
    }
    *(int *)(self + 0xa8) = 0;
    *(int *)(self + 0x9c) = *(int *)(self + 0xa8);
    *(u8 *)(self + 0x716) = 1;
    *(u8 *)(self + 0x713) = 0;
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(self + 0x6d9), 0x1c, self + 0x74);
    if (*(u8 *)(self + 0x6e6) != 0) return;
    func_02012790(0x2d);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 166 -- _ZN6Player20St_NoControl_CleanupEv, 0x020c8128, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player20St_NoControl_CleanupEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_NoControl_Cleanup()
{
    extern int EndKuppaScript(void);
  int v=0x1000;
  if(mIsMega) v=0x3000;
  mScaleX=v;
  mScaleY=v;
  mScaleZ=v;
  if(mNoCtrlKind==0x11) EndKuppaScript();
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 167 -- _ZN6Player17St_NoControl_MainEv, 0x020c816c, size 0x344 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17St_NoControl_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_NoControl_Main()
{
    extern short GetAngleToCamera(int i);
    extern void func_ov002_020c9718(void*);
    extern void func_ov002_020c8a4c(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int id, int flags, int speed, unsigned int extra);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void Player_AdvanceAnims(void*);
    extern int data_ov002_0211013c[];
    extern int func_0200ee68(void);
    extern void func_ov002_020c965c(void*);
    extern int func_ov002_020c94a4(void*);
    extern void func_ov002_020c92fc(void*);
    extern void func_ov002_020c9288(void*);
    extern void func_ov002_020c924c(void*);
    extern int func_ov002_020c91bc(void*);
    extern int func_ov002_020c9128(void*);
    extern void func_ov002_020c8cb0(void*);
    extern void func_ov002_020c904c(void*);
    extern void func_ov002_020c8f80(void*);
    extern void func_ov002_020c8f0c(void*);
    extern void func_ov002_020c8b54(void*);
    extern void func_ov002_020c897c(void*);
    extern void func_ov002_020c8d14(void*);
    extern void func_ov002_020c8b78(void*);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int);
    extern void func_ov002_020c8714(void*);
    extern int func_ov002_020c8540(void*);
    extern int func_ov002_020c84b0(void*);
    u8 mode = mNoCtrlKind;
    if (mode == 0 || mode == 1 || (mode == 0x11 && mStateStep != 0x18)) {
        if (func_0200ee68() == 0) {
            mAngleY = GetAngleToCamera(mPlayerNo);
            mPrevAngleY = mAngleY;
        }
    }

    switch (mStateStep) {
    case 0:
        func_ov002_020c965c(((char*)this));
        break;
    case 1:
        if (func_ov002_020c94a4(((char*)this)) != 0)
            return 1;
        break;
    case 2:
    case 9:
        func_ov002_020c92fc(((char*)this));
        break;
    case 10:
        func_ov002_020c9288(((char*)this));
        break;
    case 20:
        func_ov002_020c924c(((char*)this));
        return 1;
    case 21:
        if (func_ov002_020c91bc(((char*)this)) != 0)
            return 1;
        break;
    case 3:
    case 6:
        if (mStateWaitTimer == 0)
            mHorzSpeed = 0;
        if (mNoCtrlKind == 6 && mIsUnderwater != 0) {
            mVertAccel = 0;
            mVertSpeed = 0;
        } else {
            func_ov002_020c9718(((char*)this));
        }
        if (mNoCtrlKind == 0 || mNoCtrlKind == 3)
            return 1;
        break;
    case 4:
    case 5:
        if (func_ov002_020c9128(((char*)this)) != 0)
            return 1;
        break;
    case 7:
    case 8:
        func_ov002_020c8cb0(((char*)this));
        break;
    case 11:
        func_ov002_020c904c(((char*)this));
        break;
    case 12:
        func_ov002_020c8f80(((char*)this));
        break;
    case 13:
        func_ov002_020c8f0c(((char*)this));
        break;
    case 14:
        func_ov002_020c8b54(((char*)this));
        break;
    case 15:
        func_ov002_020c8a4c(((char*)this));
        break;
    case 16:
        func_ov002_020c897c(((char*)this));
        break;
    case 17:
        func_ov002_020c8d14(((char*)this));
        break;
    case 18:
        func_ov002_020c8b78(((char*)this));
        break;
    case 19:
        if (mIsOpeningBigDoor == 0)
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        break;
    case 22:
        if (mIsAirborne == 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x98, 0x40000000, 0x1000, 0);
            mStateStep = 0x17;
        }
        break;
    case 23: {
        int id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0);
        char* anim = (char*)mBodyModels[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x1c))
            mHasNoCap = 0;
        if (_ZN6Player12FinishedAnimEv(((char*)this)))
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        break;
    }
    case 24:
        func_ov002_020c8714(((char*)this));
        break;
    case 25:
        if (func_ov002_020c8540(((char*)this)) != 0)
            return 1;
        break;
    case 26:
        if (func_ov002_020c84b0(((char*)this)) != 0)
            return 1;
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 168 -- func_ov002_020c84b0, 0x020c84b0, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c84b0
extern "C" {
unsigned char func_ov002_020c84b0(char *c)
{
    typedef struct
    {
      int x;
      int y;
      int z;
    } Vec3i;
    extern short Vec3_VertAngle(const Vec3i *v1, const Vec3i *v0);
    extern void _Z15ApproachLinear2Rsss(void*, short t, short s);
    extern int func_ov002_020c84b0(void*);
  Vec3i v;
  short ang;
 dummy_label_675641: ;
  v.x = *((int *) (c + 0x5c));
  v.y = *((int *) (c + 0x60));
  v.z = *((int *) (c + 0x64));
  v.y += 0x64000;
  ang = Vec3_VertAngle((Vec3i *) (c + 0x744), &v);
  _Z15ApproachLinear2Rsss((short *) (c + 0x69c), -ang, 0x400);
  *((short *) (c + 0x762)) = 0;
  *((short *) (c + 0x764)) = 0;
  *((short *) (c + 0x762)) = *((short *) (c + 0x69c));
  c[0x742] = 3;
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 169 -- func_ov002_020c8540, 0x020c8540, size 0x1d4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c8540
/* func_ov002_020c8540 at 0x020c8540 (ov002)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
extern "C" {
int func_ov002_020c8540(char* self)
{
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int f);
    extern void func_0201f32c(int arg0);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern u16 data_ov002_020ff138[];
    extern u16 data_ov002_020ff13c[];
    extern u16 data_ov002_020ff110[];
    extern s16 data_ov002_020ff108[];
    extern int data_0209b454;
    extern u8 data_0209d660;
    extern int data_ov002_0211013c;
    extern int func_ov002_020c8540(void*);
    u32 s8;
    int id;
    char* anim;

    s8 = *(u32*)(self + 8);
    if (s8 <= 1) {
        id = _ZNK6Player14GetBodyModelIDEjb(self, s8 & 0xff, 0);
        anim = (char*)((int*)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, data_ov002_020ff138[s8])) {
            *(u8*)(self + 0x71a) = 1;
        }
        s8 = *(u32*)(self + 8);
        id = _ZNK6Player14GetBodyModelIDEjb(self, s8 & 0xff, 0);
        anim = (char*)((int*)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, data_ov002_020ff13c[s8])) {
            *(u8*)(self + 0x71a) = 0;
        }
    }
    if (*(u8*)(self + 0x70c) == 0) {
        s8 = *(u32*)(self + 8);
        id = _ZNK6Player14GetBodyModelIDEjb(self, s8 & 0xff, 0);
        anim = (char*)((int*)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, data_ov002_020ff110[s8])) {
            char* o;
            { int* p = (int*)(self + 0xb0); *p |= 0x800000; }
            o = *(char**)(self + 0x368);
            if (o != 0) {
                int* p = (int*)(o + 0xb0);
                *p &= ~0x800000;
            }
            data_0209b454 |= 0x800000;
            func_0201f32c(data_ov002_020ff108[*(u32*)(self + 8)]);
            *(u8*)(self + 0x70c) = 1;
            return 1;
        }
    } else {
        char* o;
        if (data_0209d660 != 0) return 1;
        data_0209b454 &= ~0x800000;
        { int* p = (int*)(self + 0xb0); *p &= ~0x800000; }
        o = *(char**)(self + 0x368);
        if (o != 0) {
            int* p = (int*)(o + 0xb0);
            *p |= 0x800000;
            *(char**)(self + 0x368) = 0;
        }
        if (_ZN6Player12FinishedAnimEv(self)) {
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211013c);
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 170 -- func_ov002_020c8714, 0x020c8714, size 0x268 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c8714
/* func_ov002_020c8714 @ 0x020c8714 (ov002, size 0x268)
 * Player cap-power ending update: while active, ducks the music, damps the
 * forward speed (or launches on variant 1), and streams the sparkle trail;
 * when the anim finishes either spawns the cap-return actor with the level
 * fanfare (mode 0x10) or spawns the star and hands the player to it.
 */
extern "C" {
int func_ov002_020c8714(char *c)
{
    extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(int, int);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(int, unsigned int, int, int, int, int, int);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(int, int, void*, int, int, int);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(int, void*);
    extern void func_ov002_020c7ff8(void*);
    extern void func_ov002_020e7090(int, void*);
    extern void func_ov002_020c8714(void*);
    volatile int v[3];

    if (*(u8 *)(c + 0x70c) == 0) {
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
        if (*(u16 *)(c + 0x6a4) == 0) {
            *(int *)(c + 0x98) = (int)((*(int *)(c + 0x98) * 0xf60LL + 0x800) >> 12);
            if (*(u8 *)(c + 0x6de) == 0) {
                *(int *)(c + 0x98) = 0;
                *(int *)(c + 0xa8) = 0;
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x51, 0x40000000, 0x1000, 0);
                *(u8 *)(c + 0x70c) = 1;
            }
        } else if (*(u16 *)(c + 0x6a4) == 1) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x56, 0, 0x1000, 0);
            *(int *)(c + 0x98) = 0x1a000;
            *(int *)(c + 0xa8) = 0x38000;
            *(u8 *)(c + 0x6de) = 1;
            *(u8 *)(c + 0x6df) = 0;
        }
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        v[1] = ((int *)c)[0x18] + 0x3c000;
        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(volatile int *)(c + 0x628), 0x27, v[0], v[1], *(int *)(c + 0x64), 0, 0);
    } else if (_ZN6Player12FinishedAnimEv(c) != 0) {
        if (*(u8 *)(c + 0x70a) == 0x10) {
            *(u8 *)(c + 0x6e3) = 0x19;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x99, 0x40000000, 0x1000, 0);
            *(int *)(c + 0x368) = 0;
            if (*(unsigned int *)(c + 8) <= 1) {
                *(int *)(c + 0x368) = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d, (*(unsigned int *)(c + 8) << 8) | 0xf, c + 0x5c, 0,
                    *(s8 *)(c + 0xcc), -1);
            }
            switch (*(unsigned int *)(c + 8)) {
            case 0:
                _ZN5Sound9PlayBank0EjRK7Vector3(0xd9, c + 0x74);
                break;
            case 1:
                _ZN5Sound9PlayBank0EjRK7Vector3(0xda, c + 0x74);
                break;
            case 2:
                _ZN5Sound9PlayBank0EjRK7Vector3(0xdb, c + 0x74);
                break;
            }
        } else {
            int st;
            *(int *)(c + 0xa8) = 0;
            *(int *)(c + 0x688) = 0x187;
            func_ov002_020c7ff8(c);
            st = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, 0x6f, c + 0x5c, (int)(c + 0x8c), *(s8 *)(c + 0xcc), -1);
            if (st != 0)
                func_ov002_020e7090(st, c);
        }
        *(u8 *)(c + 0x70c) = 0;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 171 -- func_ov002_020c897c, 0x020c897c, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c897c
extern "C" {
int func_ov002_020c897c(char* c)
{
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, u32 b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int frame);
    extern void func_02012694(u32 a, void*);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void func_ov002_020ca108(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 a, int b, Fix12i c, u32 d);
    extern void func_ov002_020c897c(void*);
    int id;

    id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
    if (_ZNK9Animation12WillHitFrameEi(*(char**)(c + id * 4 + 0xdc) + 0x50, 0x5f)) {
        func_02012694(0x3b, c + 0x74);
    } else {
        id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
        if (_ZNK9Animation12WillHitFrameEi(*(char**)(c + id * 4 + 0xdc) + 0x50, 0x3c)) {
            func_02012694(0x42, c + 0x74);
        } else if (_ZN6Player12FinishedAnimEv(c)) {
            func_ov002_020ca108(c);
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
            *(u8*)(c + 0x743) = 0;
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 172 -- func_ov002_020c8a4c, 0x020c8a4c, size 0x108 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c8a4c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* The ROM's Player::SetAnim takes Fix12<int> third, so its symbol is
   _ZN6Player7SetAnimEji5Fix12IiEj. A local `struct Player` declaring
   `SetAnim(unsigned, int, int, unsigned)` mangles to _ZN6Player7SetAnimEjiij,
   which exists in no module -- invisible to the byte gate, because relocated
   words compare as wildcards. Same extern "C" idiom the rest of the tree uses
   for this symbol (src/_ZN6Player12St_Hurt_InitEv.cpp). */
/* extern "C": this is the ROM symbol's own name. A bare `extern` in a //cpp file
   makes the compiler mangle it AGAIN, into _Z15_ZN7Vector3D1Evv, which exists
   nowhere -- and the byte gate cannot see it, because relocated words are
   wildcarded. Only check_references does. */
extern "C" {
int func_ov002_020c8a4c(char* self) {
    extern unsigned int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int anim, int a, int fix, unsigned int b);
    extern short Vec3_HorzAngle(const Vector3*, const Vector3*);
    extern void func_020731dc(void*, void*, void*);
    extern void Vec3_RotateYAndTranslate(void*, const Vector3*, int, const Vector3*);
    extern void _ZN7Vector3D1Ev(void);
    extern int func_ov002_020c8a4c(void*);
    extern int func_02053274(const Vector3*, const Vector3*);
    extern int data_ov002_0210e150;
    extern int data_ov002_0210f8cc[3];
    extern int data_ov002_0210f89c;
    extern void func_ov002_020ca108(void*);
    int spd = *(int*)(self + 0x98) >> 3;
    if (spd < 0x400) spd = 0x400;
    _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x48, 0, spd, 0);
    *(short*)(self + 0x94) = Vec3_HorzAngle((Vector3*)(self + 0x5c), (Vector3*)(self + 0x744));
    *(short*)(self + 0x8e) = *(short*)(self + 0x94);
    if (func_02053274((Vector3*)(self + 0x5c), (Vector3*)(self + 0x744)) < 0xa000) {
        if (*(unsigned char*)(self + 0x70c) == 0) {
            *(unsigned char*)(self + 0x70c) = 1;
            if (!(data_ov002_0210e150 & 1)) {
                data_ov002_0210f8cc[0] = 0;
                data_ov002_0210f8cc[1] = 0;
                data_ov002_0210f8cc[2] = 0x12c000;
                func_020731dc(data_ov002_0210f8cc, (void*)_ZN7Vector3D1Ev, &data_ov002_0210f89c);
                data_ov002_0210e150 |= 1;
            }
            Vec3_RotateYAndTranslate((Vector3*)(self + 0x744), (Vector3*)(self + 0x750), *(short*)(self + 0x69e), (Vector3*)data_ov002_0210f8cc);
        } else {
            func_ov002_020ca108(self);
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 173 -- func_ov002_020c8b54, 0x020c8b54, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c8b54
extern "C" {
int func_ov002_020c8b54(char* c)
{
    extern void func_ov002_020c8b54(void*);
    if (*(unsigned short*)(c + 0x6a4) == 0) {
        *(int*)(c + 0x98) = 0xa000;
        *(unsigned char*)(c + 0x6e3) = 0xf;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 174 -- func_ov002_020c8b78, 0x020c8b78, size 0x138 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c8b78
/* recovered: shared common types */
extern "C" {
int func_ov002_020c8b78(char *self) {
    struct fBase_c {};
    struct Player : fBase_c {
        int dummy;
    };
    extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int, int);
    extern int _ZNK6Player14GetBodyModelIDEjb(Player const *, unsigned int, bool);
    extern int _ZNK9Animation12WillHitFrameEi(Animation const *, int);
    extern void func_02012790(int);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void func_020731dc(void*, void*, void*);
    extern void _ZN7Vector3D1Ev(void);
    extern void Vec3_RotateYAndTranslate(void*, void*, int, void*);
    extern void _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void*, fBase_c &, unsigned int, Vector3 const *, unsigned int, unsigned int);
    extern int data_ov002_0210e154;
    extern int data_ov002_0210f824[];
    extern char data_ov002_0210f7f4[];
    extern void func_ov002_020c8b78(void*);
    Player *p = (Player *)self;
    Vector3 v;
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
    {
        int id = _ZNK6Player14GetBodyModelIDEjb(p, *(int *)(self + 8) & 0xff, false);
        Animation *a = (Animation *)(*(int *)(self + id * 4 + 0xdc) + 0x50);
        if (_ZNK9Animation12WillHitFrameEi(a, 0x40))
            func_02012790(0x3a);
    }
    if (_ZN6Player12FinishedAnimEv(p) != 0) {
        if (!(data_ov002_0210e154 & 1)) {
            data_ov002_0210f824[0] = 0;
            data_ov002_0210f824[1] = 0x96000;
            data_ov002_0210f824[2] = 0x64000;
            func_020731dc(data_ov002_0210f824, (void *)_ZN7Vector3D1Ev, data_ov002_0210f7f4);
            data_ov002_0210e154 |= 1;
        }
        Vec3_RotateYAndTranslate(&v, self + 0x5c, *(s16 *)(self + 0x8e), data_ov002_0210f824);
        *(u8 *)(self + 0x722) = 1;
        {
            u16 *fl = (u16 *)(((int)self + 0x6ce));
            *fl |= 0x400;
        }
        {
            u8 saved = *(u8 *)(self + 0x70b);
            unsigned int msg = 0x188;
            if (*(u8 *)(self + 0x70c))
                msg = 0x29;
            _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(p, *(fBase_c *)p, msg, &v, 0, 2);
            *(u8 *)(self + 0x70b) = saved;
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 175 -- func_ov002_020c8cb0, 0x020c8cb0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c8cb0
extern "C" {
int func_ov002_020c8cb0(char* c){
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void func_ov002_020ca108(void*);
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern signed char data_02092110;
    extern void func_ov002_020c8cb0(void*);
  if (_ZN6Player12FinishedAnimEv(c)) {
    if (data_02092110 >= 0)
      *(unsigned char*)(c+0x70b) = 1;
    func_ov002_020ca108(c);
    *(unsigned char*)(c+0x743) = 0;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 176 -- func_ov002_020c8d14, 0x020c8d14, size 0x1f8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c8d14
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020c8d14(char *self)
{
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
    extern void func_ov002_020c9e18(void*);
    extern int func_ov002_020d22ec(void*, int arg);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int data_ov002_021106ac;
    extern int data_ov002_0211013c;
    extern void func_ov002_020c8d14(void*);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int);
    struct Vector3 v;
    int val;
    int id;
    char *anim;

    val = *(int *)(self + 0x84) + 0xf0;
    if (val >= 0x3000) val = 0x3000;
    *(int *)(self + 0x80) = val;
    *(int *)(self + 0x84) = val;
    *(int *)(self + 0x88) = val;

    v.x = *(int *)(self + 0x5c);
    v.y = *(int *)(self + 0x60);
    v.z = *(int *)(self + 0x64);
    v.y = *(int *)(self + 0x60) + (int)(((long long)*(int *)(self + 0x84) * 0x50000 + 0x800) >> 12);

    id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 4)) {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5d, v.x, v.y, v.z);
    } else {
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 2)) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5e, v.x, v.y, *(volatile int *)&v.z);
        }
    }

    id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 0x2f)) {
        func_ov002_020c9e18(self);
        *(unsigned short *)(self + 0x6c2) = 0x258;
    }

    id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    anim = (char *)(((long long)(int)((char *)((int *)(self + 0xdc))[id] + 0x50)));
    if ((unsigned int)(*(int *)(anim + 8) << 4) >> 0x10 >= 0x2f) {
        if (*(u8 *)(self + 0x706) == 0) {
            if (func_ov002_020d22ec(self, 0))
                return 0;
        }
    }

    if (_ZN6Player12FinishedAnimEv(self)) {
        if (*(u8 *)(self + 0x706) != 0)
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021106ac);
        else
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211013c);
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 177 -- func_ov002_020c8f0c, 0x020c8f0c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c8f0c
extern "C" {
int func_ov002_020c8f0c(char* c){
    extern void EnterBigBoosHaunt(void);
    extern void func_ov002_020c8f0c(void*);
  int *p0 = (int*)(((int)c + 0x80));
  int *p1 = (int*)(((int)c + 0x84));
  int *p2 = (int*)(((int)c + 0x88));
  *p0 -= 0x80;
  *p1 -= 0x80;
  *p2 -= 0x80;
  if (*(int*)(c+0x80) < 0) {
    *(int*)(c+0x80) = 0;
    *(int*)(c+0x84) = 0;
    *(int*)(c+0x88) = 0;
  }
  if (*(unsigned char*)(c+0x6de) == 0) {
    EnterBigBoosHaunt();
    *(unsigned char*)(c+0x6e3) = 6;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 178 -- func_ov002_020c8f80, 0x020c8f80, size 0xcc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c8f80
/* recovered: shared common types */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12t<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" {
int func_ov002_020c8f80(Player *thiz)
{
    typedef int Fix12t;
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12t, unsigned int);
    extern void func_ov002_020c8f80(void*);
    char *p = (char *)thiz;

    *(int *)(p + 0x80) -= 0x80;
    *(int *)(p + 0x84) -= 0x80;
    *(int *)(p + 0x88) -= 0x80;

    unsigned int id = thiz->GetBodyModelID(*(int *)(p + 8) & 0xff, false);
    Animation *anim = (Animation *)(*(char **)(p + id * 4 + 0xdc) + 0x50);
    if (anim->WillHitFrame(1)) {
        if (*(unsigned char *)(p + 0x70c) == 0) {
            *(unsigned char *)(p + 0x70c) = 1;
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(thiz, 0x43, 0x40000000, 0x1000, 0);
            *(unsigned char *)(p + 0x6e3) = 0xd;
            *(int *)(p + 0x9c) = -0x4000;
            *(unsigned short *)(p + 0x8c) = 0x4000;
            Sound::PlayBank0(0x1a, *(Vector3 *)(p + 0x74));
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 179 -- func_ov002_020c904c, 0x020c904c, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c904c
extern "C" {
int func_ov002_020c904c(char *c)
{
    typedef unsigned int u32;
    typedef int s32;
    typedef unsigned char u8;
    struct Vector3
    {
      s32 x;
      s32 y;
      s32 z;
    };
    extern void _Z14ApproachLinearRiii(void*, int target, int step);
    extern int Math_Function_0203b14c(void*, int a, int b, int c, int d);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 anim, int a, int fix, u32 b);
    extern int _ZN6Player6IsAnimEj(void*, u32 anim);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, int b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int frame);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const struct Vector3 *pos);
    extern void func_ov002_020c904c(void*);
  int mid;
  char *anim;
  _Z14ApproachLinearRiii((int *) (c + 0x5c), *((int *) (c + 0x744)), 0x8000);
  _Z14ApproachLinearRiii((int *) (c + 0x64), *((int *) (c + 0x74c)), 0x8000);
  if (Math_Function_0203b14c(c + 0x60, (*((int *) (c + 0x644))) + 0x190000, 0x200, 0x64000, 0x10000) == 0)
  {
    *((u8 *) (c + 0x6e3)) = 0xc;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x56, 0, 0x1000, 0);
    *((u8 *) (c + 0x70c)) = 0;
  }
  if (_ZN6Player6IsAnimEj(c, 0x56) != 0)
  {
    mid = _ZNK6Player14GetBodyModelIDEjb(c, (*((int *) (c + 8))) & 0xff, 0);
    anim = (*((char **) ((c + (mid * 4)) + 0xdc))) + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 0) != 0)
    {
      _ZN5Sound9PlayBank0EjRK7Vector3(6, (struct Vector3 *) (c + 0x74));
    }
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 180 -- func_ov002_020c9128, 0x020c9128, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9128
extern "C" {
int func_ov002_020c9128(char* c){
    struct State;
    extern State data_ov002_0211013c;
    extern short _Z15ApproachLinear2Rsss(short& v, short t, short s);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020c9128(void*);
  if (*(unsigned char*)(c+0x6e3) == 4) {
    _Z15ApproachLinear2Rsss(*(short*)(c+0x69c), 0x1000, 0x400);
  } else {
    if (_Z15ApproachLinear2Rsss(*(short*)(c+0x69c), 0x4000, 0x100)) {
      _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
      return 1;
    }
  }
  *(short*)(c+0x762) = 0;
  *(short*)(c+0x764) = 0;
  *(short*)(c+0x766) = *(short*)(c+0x69c);
  *(unsigned char*)(c+0x742) = 2;
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 181 -- func_ov002_020c91bc, 0x020c91bc, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c91bc
extern "C" {
int func_ov002_020c91bc(char* c)
{
    extern unsigned char data_0209d660;
    extern int data_0209b454;
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int data_ov002_0211067c;
    extern int data_ov002_0211013c;
    extern int func_ov002_020c91bc(void*);
    if (data_0209d660 != 0) return 1;
    *(int *)((int)(c + 0xb0)) &= ~0x800000;
    data_0209b454 &= ~0x800000;
    if (_ZN6Player12FinishedAnimEv(c)) {
        if (*(unsigned char*)(c + 0x706) != 0)
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
        else
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 182 -- func_ov002_020c924c, 0x020c924c, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c924c
extern "C" {
int func_ov002_020c924c(char *c)
{
    extern void func_0201f32c(int x);
    extern void func_ov002_020c924c(void*);
    if (*(unsigned short *)(c + 0x6a4) == 0) {
        func_0201f32c((short)*(int *)(c + 0x688));
        *(unsigned char *)(c + 0x6e3) = 0x15;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 183 -- func_ov002_020c9288, 0x020c9288, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9288
extern "C" {
int func_ov002_020c9288(char* c){
    extern int func_ov002_020bea7c(void*);
    extern void func_ov002_020c43c4(void*, int a);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern char data_ov002_0211067c[];
    extern char data_ov002_0211013c[];
    extern void func_ov002_020c9288(void*);
  if (!func_ov002_020bea7c(c)) {
    if (*(unsigned char*)(c+0x6e6) == 0) {
      func_ov002_020c43c4(c, 6);
      *(unsigned char*)(c+0x6e6) = 1;
    } else if (*(unsigned char*)(c+0x706) != 0) {
      _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211067c);
    } else {
      _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
    }
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 184 -- func_ov002_020c92fc, 0x020c92fc, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c92fc
extern "C" {
int func_ov002_020c92fc(char *self)
{
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int f);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int a);
    extern void func_0200d3f8(void*, unsigned char playerID, void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int data_0209b454;
    extern unsigned char data_0209f2d8;
    extern int data_0209caa0[];
    extern void *data_0209f318;
    extern int data_ov002_0211067c;
    extern int data_ov002_0211013c;
    extern void func_ov002_020c92fc(void*);
    int flag = 0;
    *(int *)(self + 0xa8) = 0;

    if (*(int *)(self + 0x688) != -1) {
        if (*(u8 *)(self + 0x706) == 0) {
            int id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, flag);
            char *anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
            if (_ZNK9Animation12WillHitFrameEi(anim, 0x20)) flag = 1;
        } else {
            if (_ZN6Player12FinishedAnimEv(self)) flag = 1;
        }
    }

    if (flag != 0) {
        *(unsigned short *)(self + 0x6a4) = 0x14;
        *(u8 *)(self + 0x6e3) = 0x14;
        *(int *)(self + 0xa8) = 0;
        *(int *)((int)(self + 0xb0)) |= 0x800000;
        data_0209b454 |= 0x800000;
        return 0;
    }

    if (_ZN6Player12FinishedAnimEv(self)) {
        if (*(u8 *)(self + 0x6e3) == 9) {
            if ((int)(data_0209f2d8 == 1) == 0) {
                if ((data_0209caa0[2] & 0x10000000) == 0 && _ZN8SaveData19IsCharacterUnlockedEj(0) == 0 && *(u8 *)(self + 0x6dd) == 0 && *(u8 *)(self + 0x6dc) == 3) {
                    *(u8 *)(self + 0x6e3) = 0xa;
                    *(u8 *)(self + 0x6e6) = 0;
                    func_0200d3f8(data_0209f318, *(u8 *)(self + 0x6d8), 0);
                    return 0;
                }
            }
        }
        if (*(u8 *)(self + 0x706) != 0)
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211067c);
        else
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211013c);
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 185 -- func_ov002_020c94a4, 0x020c94a4, size 0x1b8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c94a4
extern "C" {
int func_ov002_020c94a4(char *self)
{
    extern signed char data_0209f2f8;
    extern u8 data_0209211c;
    extern u8 data_0209f200;
    extern void ExitLevel(void);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int a, int b);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int f);
    extern int func_ov002_020c94a4(void*);
    unsigned short st;

    *(int *)(self + 0xa8) = 0;

    st = *(unsigned short *)(self + 0x6a4);
    if (st != 0) {
        unsigned char sub;
        if (st != 1)
            goto ret1;

        sub = *(u8 *)(self + 0x70a);
        if (sub == 0) {
            if (data_0209f2f8 == 0x21) {
                data_0209211c = 4;
                data_0209f200 = 7;
            }
            ExitLevel();
            *(u8 *)(self + 0x6e3) = 6;
            goto ret1;
        }

        if (sub == 3) {
            switch (data_0209f2f8) {
            case 0x24:
                data_0209211c = 2;
                data_0209f200 = 0xa;
                break;
            case 0x26:
                data_0209211c = 4;
                data_0209f200 = 9;
                break;
            case 0x2d:
                data_0209211c = 0x32;
                data_0209f200 = 3;
                break;
            case 0x2f:
                data_0209211c = 3;
                data_0209f200 = 0;
                break;
            case 0x31:
                data_0209211c = 5;
                data_0209f200 = 0xb;
                break;
            }
            ExitLevel();
            *(u8 *)(self + 0x6e3) = 6;
        }
    ret1:
        return 1;
    }

    {
        int id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        void *anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x20))
            *(unsigned short *)(self + 0x6a4) = 0x3c;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 186 -- func_ov002_020c965c, 0x020c965c, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c965c
extern "C" {
int func_ov002_020c965c(char* c)
{
    extern void func_ov002_020c7ff8(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, int a, int b, int c, u32 d);
    extern void func_ov002_020c965c(void*);
    u8 st;

    if (*(u8*)(c + 0x6de) != 0) {
        if (*(u8*)(c + 0x706) == 0) goto done;
    }

    *(int*)(c + 0xa8) = 0;
    st = *(u8*)(c + 0x70a);
    if (st == 0 || st == 1 || st == 0x11 || (u8)(st + 0xfe) <= 1) {
        func_ov002_020c7ff8(c);
        goto done;
    }
    if (st == 8) {
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6b, 0x40000000, 0x1000, 0);
        *(u8*)(c + 0x6e3) = 9;
        goto done;
    }
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
    *(u8*)(c + 0x6e3) = 3;

done:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 187 -- func_ov002_020c9718, 0x020c9718, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9718
/* func_ov002_020c9718 at 0x020c9718
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */
extern "C" {
void func_ov002_020c9718(unsigned char *self)
{
    extern int func_ov002_020ceaf4(void*);
    extern int data_0209f32c;
    int r2;
    if (self[0x706] == 0)
        return;
    *(int *)(self + 0xa8) = func_ov002_020ceaf4(self);
    r2 = data_0209f32c - 0x50000;
    if (*(int *)(self + 0x60) < r2)
        return;
    if (*(int *)(self + 0xa8) >= 0)
        *(int *)(self + 0x60) = r2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 188 -- _ZN6Player17St_NoControl_InitEv, 0x020c976c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17St_NoControl_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_NoControl_Init()
{
    extern int Player_DisableInteraction(void*);
    extern int Player_ReleaseHeldActor(void*);
    extern struct PMF { int adj; int ptr; };
    extern struct PMF data_ov002_02110884[];
  mIsControlDisabled=1;
  mStatePhase=0;
  Player_DisableInteraction(((char*)this));
  Player_ReleaseHeldActor(((char*)this));
  mRidingShell=0;
  mHeldObj=0;
  mGrabbedByActor=0;
  mIsOpeningBigDoor=0;
  unsigned idx = mNoCtrlKind;
  struct PMF* m=&data_ov002_02110884[idx];
  char* obj=((char*)this) + (m->ptr >> 1);
  int fn=m->ptr;
  void (*f)(void*);
  if(fn & 1){
    f=*(void(**)(void*))(*(int*)obj + m->adj);
  } else {
    f=(void(*)(void*))m->adj;
  }
  f(obj);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 189 -- func_ov002_020c97e0, 0x020c97e0, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c97e0
extern "C" {
void func_ov002_020c97e0(char *p)
{
    *(char *)(p + 0x6e3) = 19;
    *(int *)(p + 0x98) = 0;
    *(int *)(p + 0xa8) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 190 -- func_ov002_020c97f8, 0x020c97f8, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c97f8
extern "C" {
void func_ov002_020c97f8(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
  *(unsigned char*)(c+0x6e3) = 0x1a;
  *(short*)(c+0x69c) = 0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
  *(int*)(c+0x98) = 0;
  *(int*)(c+0xa8) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 191 -- func_ov002_020c9840, 0x020c9840, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9840
extern "C" {
void func_ov002_020c9840(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, void*);
  *(unsigned char*)(c+0x71a) = 0;
  *(unsigned char*)(c+0x6e3) = 0x18;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
  *(unsigned short*)(c+0x600+0xa4) = 2;
  *(int*)(c+0x98) = 0;
  *(int*)(c+0xa8) = 0;
  *(unsigned char*)(c+0x70c) = 0;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 4, (void*)(c+0x74));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 192 -- func_ov002_020c98a4, 0x020c98a4, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c98a4
extern "C" {
void func_ov002_020c98a4(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, int, int, int, unsigned int);
    extern void func_0200d148(void*, unsigned char);
    extern void* data_0209f318[];
  *(unsigned char*)(c+0x71a)=0;
  *(unsigned char*)(c+0x6e3)=0x18;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
  *(short*)(c+0x6a4)=2;
  *(int*)(c+0x98)=0;
  *(int*)(c+0xa8)=0;
  *(unsigned char*)(c+0x70c)=0;
  func_0200d148(data_0209f318[0], *(unsigned char*)(c+0x6d8));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 193 -- func_ov002_020c990c, 0x020c990c, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c990c
extern "C" {
void func_ov002_020c990c(void* c) {
    typedef int Fix12;
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12, unsigned int);
    extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, void*);
  *(unsigned char*)((char*)c+0x71a)=1;
  if (*(unsigned char*)((char*)c+0x6de)) {
    *(unsigned char*)((char*)c+0x6e3)=0x16;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x54, 0x40000000, 0x1000, 0);
  } else {
    *(unsigned char*)((char*)c+0x6e3)=0x17;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x98, 0x40000000, 0x1000, 0);
  }
  *(int*)((char*)c+0x98)=0;
  *(int*)((char*)c+0xa8)=0;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)((char*)c+0x6d9), 0x29, (char*)c+0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 194 -- func_ov002_020c9998, 0x020c9998, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9998
/* recovered: shared common types */
extern "C" {
void func_ov002_020c9998(void* c) {
    typedef int Fix12i;
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
    extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, const struct Vector3*, const struct Vector3_16*, int, int);
  *(unsigned char*)((char*)c+0x6e3) = 0x12;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x92, 0x40000000, 0x1000, 0);
  _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as( 0xb2, 0xffff, (struct Vector3*)((char*)c+0x5c), (struct Vector3_16*)((char*)c+0x8c), *(signed char*)((char*)c+0xcc), -1);
  *(int*)((char*)c+0x98) = 0;
  *(int*)((char*)c+0xa8) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 195 -- func_ov002_020c9a04, 0x020c9a04, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9a04
/* recovered: shared common types */
extern "C" {
void func_ov002_020c9a04(char *c)
{
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
    extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
    struct Vector3 v;

    *(unsigned char *)(c + 0x6e3) = 0x11;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x9a, 0x40000000, 0x1000, 0);

    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa8) = 0;

    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    v.y = *(int *)(c + 0x60) + 0x28000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5f, v.x, v.y, v.z);

    v.y = *(int *)(c + 0x60) + (int)(((long long)*(int *)(c + 0x84) * 0x50000 + 0x800) >> 12);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x60, v.x, v.y, *(volatile int *)&v.z);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 196 -- func_ov002_020c9ac0, 0x020c9ac0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9ac0
extern "C" {
void func_ov002_020c9ac0(char* c){
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
  *(unsigned char*)(c+0x743)=1;
  *(unsigned char*)(c+0x716)=1;
  *(unsigned char*)(c+0x713)=0;
  *(int*)(c+0x9c)=0;
  *(int*)(c+0x98)=0;
  *(int*)(c+0xa8)=0;
  *(unsigned char*)(c+0x6e3)=0x10;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x93,0x40000000,0x1000,0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 197 -- func_ov002_020c9b10, 0x020c9b10, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9b10
extern "C" {
int func_ov002_020c9b10(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
    *(unsigned char*)(c+0x6e3)=0xe;
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x47,0,0x1000,0);
    *(short*)(c+0x6a4)=0x14;
    *(int*)(c+0x98)=0;
    *(int*)(c+0xa8)=0;
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 198 -- func_ov002_020c9b5c, 0x020c9b5c, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9b5c
extern "C" {
void func_ov002_020c9b5c(char *self) {
    extern void func_ov002_020c9b10(void*);
    *(unsigned char *)(self + 0x716) = 1;
    *(unsigned char *)(self + 0x713) = 0;
    *(int *)(self + 0x9c) = 0;
    func_ov002_020c9b10(self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 199 -- func_ov002_020c9b7c, 0x020c9b7c, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9b7c
extern "C" {
void func_ov002_020c9b7c(void* c) {
    typedef int Fix12i;
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
    extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
    extern int func_ov002_020e25f0(void*, int);
  *(char*)((char*)c+0x6e3)=0xb;
  *(char*)((char*)c+0x713)=0;
  if (*(unsigned char*)((char*)c+0x6de)) {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x56, 0, 0x1000, 0);
    _ZN5Sound9PlayBank0EjRK7Vector3(6, (char*)c+0x74);
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x4f, 0x40000000, 0x1000, 0);
    func_ov002_020e25f0(c, 0);
  }
  *(int*)((char*)c+0x9c)=0;
  *(int*)((char*)c+0x98)=0;
  *(int*)((char*)c+0xa8)=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 200 -- func_ov002_020c9c00, 0x020c9c00, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9c00
extern "C" {
int func_ov002_020c9c00(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
    if(*(unsigned char*)(c+0x6de)!=0){
        _ZN6Player7SetAnimEji5Fix12IiEj(c,0x54,0x40000000,0x1000,0);
    }
    *(unsigned char*)(c+0x6e3)=0;
    *(int*)(c+0x98)=0;
    *(int*)(c+0xa8)=0;
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 201 -- func_ov002_020c9c4c, 0x020c9c4c, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9c4c
extern "C" {
void func_ov002_020c9c4c(char* c) {
    typedef int Fix12i;
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
    extern unsigned int data_ov002_020ff118[];
    _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_020ff118[*(unsigned char*)(c+0x6e3)], 0x40000000, 0x1000, 0);
    *(unsigned char*)(((int)c + 0x6e3)) += 7;
    *(char*)(c+0x743) = 1;
    *(char*)(c+0x716) = 1;
    *(char*)(c+0x713) = 0;
    *(int*)(c+0x9c) = 0;
    *(int*)(c+0x98) = 0;
    *(int*)(c+0xa8) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 202 -- func_ov002_020c9cc0, 0x020c9cc0, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9cc0
extern "C" {
void func_ov002_020c9cc0(char* c){
    extern signed char data_02092110;
  *(short*)(c+0x6a6)=3;
  *(unsigned char*)(c+0x6e3)=6;
  *(unsigned char*)(c+0x716)=1;
  *(unsigned char*)(c+0x713)=0;
  if(*(int*)(c+0xa8)>=0x14000) *(int*)(c+0xa8)=0x14000;
  if(data_02092110==0x26 || data_02092110==0x12) *(unsigned char*)(c+0x6f5)=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 203 -- func_ov002_020c9d1c, 0x020c9d1c, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9d1c
extern "C" {
int func_ov002_020c9d1c(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
    *(unsigned char*)(c+0x6e3)=4;
    *(short*)(c+0x69c)=0x4000;
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x47,0,0x1000,0);
    *(int*)(c+0x98)=0;
    *(int*)(c+0xa8)=0;
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 204 -- func_ov002_020c9d68, 0x020c9d68, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9d68
extern "C" {
void func_ov002_020c9d68(char* c){
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int fix, unsigned int d);
    extern short GetAngleToCamera(int i);
  if(*(unsigned char*)(c+0x6de) == 0){
    int b = (int)(*(int*)(c+0x354) != 0);
    if(b == 0) goto tail;
  }
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x54, 0x40000000, 0x1000, 0);
tail:
  *(short*)(c+0x8e) = GetAngleToCamera(*(unsigned char*)(c+0x6d8));
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  *(unsigned char*)(c+0x6e3) = 0;
  *(unsigned char*)(c+0x70c) = 0;
  *(int*)(c+0x98) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 205 -- func_ov002_020c9de4, 0x020c9de4, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9de4
extern "C" {
void func_ov002_020c9de4(char* c){
    extern void func_ov002_020bdd9c(void*);
    extern void func_ov002_020bdef0(void*);
    extern void func_ov002_020bdd2c(void*);
    extern void func_ov002_020d80d0(void*);
    extern void func_ov002_020c9d68(void*);
  func_ov002_020bdd9c(c);
  func_ov002_020bdef0(c);
  func_ov002_020bdd2c(c);
  func_ov002_020d80d0(c);
  func_ov002_020c9d68(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 206 -- func_ov002_020c9e18, 0x020c9e18, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020c9e18
extern "C" {
void func_ov002_020c9e18(char *c)
{
    if (*(unsigned char *)(c + 0x709) != 1) return;
    *(unsigned char *)(c + 0x709) = 0;
    *(unsigned int *)(c + 0x2ec) &= ~4;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 207 -- Player_DisableInteraction, 0x020c9e40, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol Player_DisableInteraction
extern "C" {
void Player_DisableInteraction(char *self)
{
    unsigned int tmp;
    char *slot;

    *(unsigned char *)(self + 0x709) = 1;
    slot = (char *)(self + 0x2ec);
    tmp = *(unsigned int *)slot;
    tmp |= 4u;
    *(unsigned int *)slot = tmp;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 208 -- _ZN6Player12Unk_020c9e5cEh, 0x020c9e5c, size 0x298 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12Unk_020c9e5cEh
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020c9e5c(unsigned char h)
{
    extern Player::State data_ov002_0211022c;
  if(!IsState(data_ov002_0211022c)) return 0;
  if(*(unsigned char*)((char*)&mNoCtrlKind) != h) return 0;
  switch(h){
  case 0:
  case 3:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==1) return 1; }
    break;
  case 0x11:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==0x18) return 1; }
    /* fallthrough */
  case 1:
  case 2:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==2 || (unsigned char)(s+0xec)<=1) return 1; }
    break;
  case 4:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==3) return 1; }
    break;
  case 5:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xfc)<=1) return 1; }
    break;
  case 6:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==6) return 1; }
    break;
  case 7:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xf9)<=1) return 1; }
    break;
  case 8:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==9 || (unsigned char)(s+0xec)<=1) return 1; }
    break;
  case 9:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xf5)<=2) return 1; }
    break;
  case 0xa:
  case 0xc:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xf2)<=1) return 1; }
    break;
  case 0xb:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==0x10) return 1; }
    break;
  case 0xd:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==0x11) return 1; }
    break;
  case 0xe:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if(s==0x12) return 1; }
    break;
  case 0xf:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xea)<=1) return 1; }
    break;
  case 0x10:
    { unsigned char s = *(unsigned char*)((char*)&mStateStep);
      if((unsigned char)(s+0xe8)<=1) return 1; }
    break;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 209 -- func_ov002_020ca0f4, 0x020ca0f4, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ca0f4
extern "C" {
int func_ov002_020ca0f4(void* player) {
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern char data_ov002_0211022c[];
    return _ZN6Player7IsStateERNS_5StateE(player, data_ov002_0211022c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 210 -- func_ov002_020ca108, 0x020ca108, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ca108
extern "C" {
void func_ov002_020ca108(char* c){
    extern int data_ov002_0211013c[];
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void func_ov002_020ca108(void*);
  if(*(unsigned char*)(c+0x70b)){
    *(unsigned char*)(c+0x6e3)=0x13;
    return;
  }
  _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_0211013c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 211 -- _ZN6Player11OpenBigDoorEv, 0x020ca144, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player11OpenBigDoorEv
/* recovered: named members + shared header, real C++ method */
/* Player::OpenBigDoor() at 0x020ca144 (ov002).
 * Sets a u8 flag at offset 0x70b (1803) in the Player object to 1.
 * Player.h has only `u8 unk70b` here, so keep the typed offset access.
 */
extern "C" {
void Player::OpenBigDoor()
{
    mIsOpeningBigDoor = 1; /* +0x70b */
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 212 -- _ZN6Player12Unk_020ca150Eh, 0x020ca150, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12Unk_020ca150Eh
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020ca150(unsigned char a)
{
    extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern Player::State data_ov002_0211022c;
    extern Player::State data_ov002_0211013c;
  if(IsState(data_ov002_0211022c)){
    if(a==4){
      _ZN6Player11ChangeStateERNS_5StateE(((void *)this), &data_ov002_0211013c);
      return 1;
    }
    if(a==5) *(unsigned char*)((char*)&mStateStep)=5;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 213 -- _ZN6Player17SetNoControlStateEhih, 0x020ca1b8, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17SetNoControlStateEhih
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::SetNoControlState(unsigned char a_, int b, unsigned char c_)
{
    struct State;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern struct State data_ov002_0211010c;
    extern struct State data_ov002_02110124;
    extern struct State data_ov002_0211022c;
    int a = (int)a_;
    u8 c = (u8)c_;

    u8 flag;

    if (_ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_0211010c) || _ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_02110124)) return 0;

    flag = 0;
    if (a <= 3) {
        if (mIsNoControl != 0) return 0;
        flag = c;
    } else {
        if (mIsNoControl != 0) {
            if (c == 0) return 0;
        }
    }

    mNoCtrlKind = a;
    mAttachOffsetY = b;
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211022c);
    mStatePhase = flag;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 214 -- func_ov002_020ca270, 0x020ca270, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ca270
extern "C" {
int func_ov002_020ca270(char* c){
    struct State;
    extern State data_ov002_02110124;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
  if(_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110124)){
    if(*(unsigned char*)(c+0x6e3)==1) return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 215 -- _ZN6Player15JumpIntoBooCageER7Vector3, 0x020ca2ac, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15JumpIntoBooCageER7Vector3
/* recovered: named members + shared header, real C++ method
 *
 * Refuses while airborne and still rising, then takes control, records the
 * cage position in the 0x744 vector and turns the player to face away from it.
 */
extern "C" {
int Player::JumpIntoBooCage(Vector3 & v_)
{
    extern int _ZN6Player17SetNoControlStateEhih(void*, unsigned char, int, unsigned char);
    extern short Vec3_HorzAngle(const Vector3*, const Vector3*);
    Vector3 *v = &v_;
    if (mIsAirborne) {
        if (mVertSpeed < 0) return 0;
    }
    if (!_ZN6Player17SetNoControlStateEhih(this, 9, -1, 0)) return 0;
    unk_744 = v->x;
    unk_748 = v->y;
    unk_74c = v->z;
    mAngleY = Vec3_HorzAngle(v, (const Vector3 *)&mPosX) + 0x8000;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 216 -- _ZN6Player13TryTalkToDoorEh, 0x020ca344, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player13TryTalkToDoorEh
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::TryTalkToDoor(unsigned char a)
{
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int _ZN6Player17SetNoControlStateEhih(void*, unsigned char, int, unsigned char);
    extern int data_ov002_0211013c[];
    extern int data_ov002_0211043c[];
  if(_ZN6Player7IsStateERNS_5StateE(((void*)this),data_ov002_0211013c) || _ZN6Player7IsStateERNS_5StateE(((void*)this),&ST_WAIT) || _ZN6Player7IsStateERNS_5StateE(((void*)this),data_ov002_0211043c)){
    *(unsigned char*)((char*)&mStateArg)=a;
    _ZN6Player17SetNoControlStateEhih(((void*)this),0xe,-1,0);
    return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 217 -- _ZN6Player21IsOpeningDoorWithStarEv, 0x020ca3d0, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player21IsOpeningDoorWithStarEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsOpeningDoorWithStar()
{
    extern int _ZN6Player12Unk_020c9e5cEh(void*, unsigned char a);
    extern int _ZN6Player12GetTalkStateEv(void*);
  return _ZN6Player12Unk_020c9e5cEh(((void *)this),0xe) || _ZN6Player12GetTalkStateEv(((void *)this)) > 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 218 -- _ZN6Player16TryTalkToKeyDoorEv, 0x020ca410, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16TryTalkToKeyDoorEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::TryTalkToKeyDoor()
{
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int _ZN6Player17SetNoControlStateEhih(void*, unsigned char, int, unsigned char);
    extern int data_ov002_0211013c[];
    extern int data_ov002_0211043c[];
  if(_ZN6Player7IsStateERNS_5StateE(((void*)this),data_ov002_0211013c) || _ZN6Player7IsStateERNS_5StateE(((void*)this),&ST_WAIT) || _ZN6Player7IsStateERNS_5StateE(((void*)this),data_ov002_0211043c)){
    _ZN6Player17SetNoControlStateEhih(((void*)this),0xb,-1,0);
    return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 219 -- _ZN6Player12Unk_020ca488Ev, 0x020ca488, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12Unk_020ca488Ev
/* recovered: named members + shared header, real C++ method */
/* _ZN6Player12Unk_020ca488Ev @ 0x20ca488 (ov002) -- tail-call veneer to _ZN6Player12Unk_020c9e5cEh (0x20c9e5c) with r1=11.
 */
extern "C" {
void Player::Unk_020ca488()
{
    extern void _ZN6Player12Unk_020c9e5cEh(void*, int);
    _ZN6Player12Unk_020c9e5cEh(((void*)this), 11);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 220 -- _ZN6Player16TryEnterStarDoorER7Vector3s, 0x020ca498, size 0x134 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16TryEnterStarDoorER7Vector3s
/* recovered: named members + shared header, real C++ method
 *
 * Only from the walking, waiting or crouching states. Takes control, records
 * the door position in the 0x750 anchor and derives the approach point at
 * 0x744 from it, and turns the player to face the door. The one-shot guarded
 * by bit 0 of data_ov002_0210e164 registers the offset vector once.
 */
extern "C" {
int Player::TryEnterStarDoor(Vector3 & pos_, short kind)
{
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_02110154;
    extern Player::State data_ov002_0211043c;
    extern int data_ov002_0210e164;
    extern int data_ov002_0210f344[3];
    extern int data_ov002_0210f308;
    extern void _ZN7Vector3D1Ev(void);
    extern int _ZN6Player17SetNoControlStateEhih(void*, unsigned char a, int b, unsigned char d);
    extern void func_020731dc(int a, int b, void*);
    extern void Vec3_RotateYAndTranslate(void*, void*, short angle, void*);
    extern short Vec3_HorzAngle(const void *v0, const void *v1);
    int *pos = (int *)&pos_;
    if (IsState(data_ov002_0211013c) || IsState(data_ov002_02110154) || IsState(data_ov002_0211043c)) {
        if (_ZN6Player17SetNoControlStateEhih(this, 0xa, -1, 0)) {
            unk_750 = pos[0];
            unk_754 = pos[1];
            unk_758 = pos[2];
            unk_69e = kind;
            mStateArg = 0;
            if (!(data_ov002_0210e164 & 1)) {
                data_ov002_0210f344[0] = 0;
                data_ov002_0210f344[1] = 0;
                data_ov002_0210f344[2] = 0x64000;
                func_020731dc((int)data_ov002_0210f344, (int)_ZN7Vector3D1Ev, (void **)&data_ov002_0210f308);
                data_ov002_0210e164 |= 1;
            }
            Vec3_RotateYAndTranslate(&unk_744, &unk_750, unk_69e, data_ov002_0210f344);
            mAngleY = Vec3_HorzAngle(pos, &mPosX) + 0x8000;
            return 1;
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 221 -- _ZN6Player12CanEnterDoorEh, 0x020ca5cc, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12CanEnterDoorEh
/* recovered: named members + shared header, real C++ method */
// Player::CanEnterDoor(unsigned char door)
// The +0x48 vcall must be spelled as a real C++ virtual call (dummy-virtual
// struct, slot 18): the C function-pointer cast perturbs callee-saved homing
// rank to this=r4/door=r5, while virtual dispatch keeps the natural
// reverse-arg homing (door=r4, this=r5) the ROM has. See
// notes/mwccarm-codegen.md 6bc.
extern "C" {
int Player::CanEnterDoor(unsigned char door)
{
    struct StarDoor {
        virtual int v00(); virtual int v01(); virtual int v02(); virtual int v03();
        virtual int v04(); virtual int v05(); virtual int v06(); virtual int v07();
        virtual int v08(); virtual int v09(); virtual int v10(); virtual int v11();
        virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
        virtual int v16(); virtual int v17();
        virtual int GetType();
    };
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int _ZN6Player17SetNoControlStateEhih(void*, unsigned char, int, unsigned char);
    extern int data_ov002_0211022c[];
    extern int data_ov002_0211013c[];
    extern int data_ov002_02110154[];
    extern int data_ov002_0211043c[];
    extern int data_ov002_0211004c[];
  if (IsState(*(State *)data_ov002_0211022c)) {
    if (!(mStateStep == 0x13 && mIsOpeningBigDoor != 0))
      return 0;
  }
  if (IsState(*(State *)data_ov002_0211013c) || IsState(*(State *)data_ov002_02110154) || IsState(*(State *)data_ov002_0211022c) || IsState(*(State *)data_ov002_0211043c)) {
    if (*(struct StarDoor **)&mObjInMouth != 0) {
      if ((*(struct StarDoor **)&mObjInMouth)->GetType() == 6 || (*(struct StarDoor **)&mObjInMouth)->GetType() == 1) {
        _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_0211004c);
      }
      return 0;
    }
    mStateStep = door;
    if (_ZN6Player17SetNoControlStateEhih(this, 7, -1, 1) != 0)
      return 1;
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 222 -- _ZN6Player17PlayMammaMiaSoundEv, 0x020ca708, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17PlayMammaMiaSoundEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
void Player::PlayMammaMiaSound()
{
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3 *v);
    /* mCamSpacePosX/Y/Z are three consecutive words; dActor_c.h declares them
       individually rather than as one Vector3. */
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x27, (const Vector3 *)&mCamSpacePosX);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 223 -- _ZN6Player24TryExitWhiteDoorWithStarEv, 0x020ca724, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player24TryExitWhiteDoorWithStarEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::TryExitWhiteDoorWithStar()
{
    extern int _ZN6Player17SetNoControlStateEhih(void*, unsigned char a, int b, unsigned char d);
    extern Player::State data_ov002_0211022c;
  if(IsState(data_ov002_0211022c)){
    if(*(unsigned char*)((char*)&mStateStep)!=0x13 || *(unsigned char*)((char*)&mIsOpeningBigDoor)==0) return 0;
  }
  return _ZN6Player17SetNoControlStateEhih(((void *)this),0x11,-1,1)!=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 224 -- func_ov002_020ca78c, 0x020ca78c, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ca78c
extern "C" {
int func_ov002_020ca78c(void *c){
    struct State2i { int a; int b; };
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int _ZN6Player17SetNoControlStateEhih(void*, unsigned char a, int b, unsigned char d);
    extern struct State2i data_ov002_0211022c;
  if(_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211022c)){
    if(*(unsigned char*)((char*)c+0x6e3)!=0x13 || *(unsigned char*)((char*)c+0x70b)==0) return 0;
  }
  return _ZN6Player17SetNoControlStateEhih(c,0x10,-1,1)!=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 225 -- _ZN6Player16St_DebugFly_MainEv, 0x020ca7f4, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16St_DebugFly_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DebugFly_Main()
{
    extern int func_ov002_020bf224(int a, int b, int c);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_020a0e40;
    extern char data_0209f4a0[];
    extern char data_0209f49c[];
    extern char data_020a0e5a[];
    extern int data_ov002_0211013c;
    u32 idx;
    u16 flags;

    mHorzSpeed = 0;
    mVertSpeed = 0;
    mClsnFlags = 0;

    if (*(s16 *)(data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        mPrevAngleY = mDesiredAngleY;
        mHorzSpeed = func_ov002_020bf224((int)((char *)this), 0x50000, 0);
    }

    idx = data_020a0e40;
    flags = *(u16 *)(data_0209f49c + idx * 0x18);
    if (flags & 2) {
        *(int *)((char *)&mPosY) += 0x28000;
    } else if (flags & 1) {
        *(int *)((char *)&mPosY) -= 0x28000;
    }

    if (*(u16 *)(data_020a0e5a + idx * 4) & 0x400) {
        _ZN6Player11ChangeStateERNS_5StateE(((char *)this), &data_ov002_0211013c);
    }

    Player_AdvanceAnims(((char *)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 226 -- _ZN6Player16St_DebugFly_InitEv, 0x020ca8d0, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16St_DebugFly_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_DebugFly_Init()
{
    extern void Player_DisableInteraction(void*);
    Player_DisableInteraction(((char *)this));
    mHorzSpeed = 0;
    mVertSpeed = 0;
    mVertAccel = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 227 -- _ZN6Player12Unk_020ca8f8Ev, 0x020ca8f8, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12Unk_020ca8f8Ev
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::Unk_020ca8f8()
{
    extern Player::State data_ov002_0211064c;
    extern Player::State data_ov002_02110664;
  if(IsState(data_ov002_0211064c)) return 1;
  if(IsState(data_ov002_02110664)) return 2;
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 228 -- func_ov002_020ca940, 0x020ca940, size 0x2a0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ca940
extern "C" {
void func_ov002_020ca940(char* self)
{
    extern u8 data_0209f2d8;
    extern int data_0209caa0[];
    extern u8 data_020a0e40;
    extern u8 data_0209f49e[];
    extern int data_0209f318;
    extern char data_ov002_0211013c;
    extern char data_ov002_0211043c;
    extern char data_ov002_02110154;
    extern char data_ov002_0211064c;
    extern char data_ov002_02110664;
    extern int func_0200d0ac(int a, unsigned int b);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern unsigned int func_02012790(unsigned int a);
    extern void func_0200d064(int a, unsigned int b);
    extern void func_ov002_020ca940(void*);
    u8 old;
    int h;
    int b;

    if (*(u8*)(self + 0x709) != 0) return;

    b = data_0209f2d8;
    b = b == 1;
    if (b == false) {
        if (!(data_0209caa0[2] & 0x80)) return;
    }

    if (*(u16*)(data_0209f49e + (u8)data_020a0e40 * 0x18) & 0x8000) {
        old = *(u8*)(self + 0x715);
        h = data_0209f318;
        (*(u8*)((int)self + 0x715))++;

        if (*(u8*)(self + 0x715) > 2) {
            *(u8*)(self + 0x715) = 0;
        }

        switch (*(u8*)(self + 0x715)) {
        case 1:
            if (func_0200d0ac(h, *(u8*)(self + 0x6d8)) != 0) break;
            *(u8*)(self + 0x715) = 2;
        case 2:
            if (*(u8*)(self + 0x703) == 0) {
                b = data_0209f2d8;
                b = b == 1;
                if (b == false) {
                    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110154)) {
                        *(u16*)((int)self + 0x6ce) |= 4;
                        break;
                    }
                }
            }
            if (old == 0) {
                func_02012790(0xe);
                *(u8*)(self + 0x715) = old;
                break;
            }
            *(u8*)(self + 0x715) = 0;
        case 0:
            func_0200d064(h, *(u8*)(self + 0x6d8));
            if ((u16)(*(u16*)(self + 0x6ce) & 4) || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211064c)) {
                *(u8*)(self + 0x715) = 2;
            }
            break;
        default:
            *(u8*)(self + 0x715) = 0;
            func_0200d064(h, *(u8*)(self + 0x6d8));
            break;
        }
    }

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c)) return;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110154)) return;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c)) return;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211064c)) return;
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110664)) return;

    if ((u16)(*(u16*)(self + 0x6ce) & 4)) {
        *(u16*)((int)self + 0x6ce) &= ~4;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 229 -- func_ov002_020cabe0, 0x020cabe0, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cabe0
extern "C" {
void func_ov002_020cabe0(char* c){
    extern int data_ov002_0211064c[];
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
  unsigned short v = (unsigned short)(*(unsigned short*)(c+0x6ce) & 4);
  if (v == 0) return;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211064c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 230 -- _ZN6Player15St_Null_CleanupEv, 0x020cac20, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15St_Null_CleanupEv
/* recovered: named members + shared header, real C++ method */
/* Player::St_Null_Cleanup() at 0x020cac20 (ov002).
 * Cleanup hook of the ST_NULL do-nothing player state; returns VS_FAIL (1).
 * The implicit Player* this is unused.
 */
extern "C" {
s32 Player::St_Null_Cleanup()
{
    (void)this;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 231 -- _ZN6Player12St_Null_MainEv, 0x020cac28, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12St_Null_MainEv
/* recovered: named members + shared header, real C++ method */
/* Player::St_Null_Main() at 0x020cac28 (ov002).
 * Main hook of the ST_NULL do-nothing player state; returns VS_FAIL (1).
 * (All ST_NULL state functions are "return 1;" per Player.h.)
 */
extern "C" {
s32 Player::St_Null_Main()
{
    (void)this;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 232 -- _ZN6Player12St_Null_InitEv, 0x020cac30, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12St_Null_InitEv
/* recovered: shared header, real C++ method
 *
 * ov002, 0x020cac30, size 0x8 -- `mov r0, #1; bx lr` and nothing else.
 *
 * The whole body IS the evidence for the name. Player::State holds three
 * `int (Player::*)()` members, ChangeState calls mInit last on the state
 * being entered, and a handler returns 1 for success. A state called Null
 * whose Init does nothing but succeed is exactly eight bytes. ov002 reaches
 * this address by `kind:load ... module:overlay(2)` from 0x0210a17c, which
 * sits in the eight-byte-stride pointer-to-member run that holds those
 * State objects.
 *
 * This file carried the placeholder name func_ov002_020cac30 until the symbol
 * was moved here from ov006, where it had been sitting on an unrelated 0x6c
 * function at the same shared address. See the commit and include/Player.h.
 */
extern "C" {
int Player::St_Null_Init()
{
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 233 -- _ZN6Player21St_CameraZoom_CleanupEv, 0x020cac38, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player21St_CameraZoom_CleanupEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_CameraZoom_Cleanup()
{
    *(unsigned short *)((char *)&mStateFlags) &= ~4;
    mUseFarCamera = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 234 -- func_ov002_020cac60, 0x020cac60, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cac60
extern "C" {
int func_ov002_020cac60(char *self)
{
    extern unsigned char NumStars(void);
    extern short GetAngleToCamera(int i);
    extern void LoadLevel(int a, int b, int c, int d, int e);
    extern void _ZN8dScene_c20SetAndStopColorFaderEv(void);
    extern void func_02012790(int a);
    extern short data_0209f5e8;
    short ang;
    short angle;
    if (*(int *)(self + 0x664) == 0xd && NumStars() >= 0xe) {
        ang = *(short *)(self + 0x766);
        angle = GetAngleToCamera(*(unsigned char *)(self + 0x6d8));
        if (ang <= 0x1c00 && angle >= -0x1000 && angle <= 0x1000) {
            LoadLevel(0x1f, 0, 1, 0, 0xe);
            _ZN8dScene_c20SetAndStopColorFaderEv();
            *(short *)((char *)&data_0209f5e8 + 0xc) = 0x7fff;
            func_02012790(0x1e);
            return 1;
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 235 -- _ZN6Player18St_CameraZoom_MainEv, 0x020cad10, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player18St_CameraZoom_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * The zoomed-in camera pose. Runs only while data_02092110 is negative. Losing
 * the 0x4 state flag ends it: back to walking, the body model's animation
 * field at +0x50+8 is cleared, and the facing angle is latched. Landing, or
 * any of the 0x8f03 input bits, clears that flag.
 */
extern "C" {
int Player::St_CameraZoom_Main()
{
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, Player::State&);
    extern void func_ov002_020cae10(void*);
    extern int func_ov002_020cac60(void*);
    extern void Player_AdvanceAnims(void*);
    extern signed char data_02092110;
    extern Player::State data_ov002_0211013c;
    extern unsigned char data_020a0e40;
    extern unsigned short data_0209f49e;
    if (data_02092110 < 0) {
        if ((unsigned short)(mStateFlags & 4) == 0) {
            unsigned int id;
            _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_0211013c);
            id = GetBodyModelID(param1 & 0xff, 0);
            int q = (int)((*(volatile int*)((char*)&mBodyModels + id * 4) + 0x50));
            *(int*)(q + 8) = 0;
            mPrevAngleY = mAngleY;
            return 1;
        }
        if (mIsAirborne) {
            mStateFlags &= ~4;
        }
        func_ov002_020cae10(this);
        if (*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18) & 0x8f03) {
            mStateFlags &= ~4;
        }
        func_ov002_020cac60(this);
    }
    Player_AdvanceAnims(this);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 236 -- func_ov002_020cae10, 0x020cae10, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cae10
extern "C" {
void func_ov002_020cae10(char* c)
{
    typedef int Fix12i;
    extern unsigned char data_0209f250;
    extern void* data_0209f318;
    extern short GetAngleToCamera(int i);
    int r4 = 0x4000;
    int ang;
    int r8;
    if (*(unsigned char*)(c + 0x6d8) == data_0209f250) {
        r4 -= *(short*)((char*)*(void**)&data_0209f318 + 0x17e);
        r4 = (short)r4;
    }
    ang = (short)((*(short*)(c + 0x8e) - GetAngleToCamera(*(unsigned char*)(c + 0x6d8))) + 0x8000);
    r8 = ang;
    if (r8 < (int)0xffffd556) r8 = (int)0xffffd556;
    if (r8 > 0x2aaa) r8 = 0x2aaa;
    *(short*)(c + 0x764) = (short)(int)(((long long)r8 * 0xc00 + 0x800) >> 12);
    *(short*)(c + 0x766) = (short)(int)(((long long)r4 * 0xc00 + 0x800) >> 12);
    *(unsigned char*)(c + 0x742) = 2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 237 -- _ZN6Player18St_CameraZoom_InitEv, 0x020caef4, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player18St_CameraZoom_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_CameraZoom_Init()
{
    extern int func_0200d064(void*, int playerID);
    extern void func_0200d7e0(void*, int playerID);
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern struct Camera* data_0209f318;
    extern unsigned char data_0209f28c;
  *(int*)((char*)&mHorzSpeed)=0;
  *(int*)((char*)&mVertSpeed)=0;
  struct Camera* cam = data_0209f318;
  int pid = *(unsigned char*)((char*)&mPlayerNo);
  func_0200d064(cam, pid);
  func_0200d7e0(cam, *(unsigned char*)((char*)&mPlayerNo));
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this),0x47,0,0x1000,0);
  data_0209f28c=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 238 -- func_ov002_020caf68, 0x020caf68, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020caf68
extern "C" {
void func_ov002_020caf68(void *self) {
    extern void func_ov002_020e63a4(void*);
    *(unsigned int *)((char *)self + 0x37c) = 0;
    *(unsigned int *)((char *)self + 0x2ec) &= ~4u;
    *(unsigned int *)((char *)self + 0x2ec) |= 8u;
    func_ov002_020e63a4(self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 239 -- func_ov002_020caf98, 0x020caf98, size 0x1c4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020caf98
extern "C" {
int func_ov002_020caf98(Player *self, Arg *arg)
{
    extern Player::State data_ov002_0211013c;
    extern Player::State data_ov002_021105bc;
    extern Player::State data_ov002_0211031c;
    extern Player::State data_ov002_021101b4;
    extern Player::State data_ov002_021106dc;
    extern int func_ov002_020e0478(void*);
    extern int func_ov002_020e3078(void*, void*);
    char *p = (char *)self;
    int b354, b358;

    if (self->IsState(data_ov002_0211013c) == 0 && *(u16 *)(p + 0x6b6) == 0)
    {
        b354 = (*(int *)(p + 0x354) != 0);
        if (b354 == 0 && *(int *)(p + 0x37c) == 0)
        {
            b358 = (*(int *)(p + 0x358) != 0);
            if (b358 == 0 && *(u8 *)(p + 0x708) == 0 && *(u8 *)(p + 0x709) == 0 && *(u8 *)(p + 0x706) == 0 && *(u8 *)(p + 0x703) == 0)
            {
                goto cont;
            }
        }
    }
    return 0;

cont:
    if (self->IsInAir() != 0) goto state_check;
    if (self->IsBeingShotOutOfCannon() != 0) goto state_check;
    if (self->IsState(data_ov002_021105bc) != 0) goto state_check;
    if (self->IsState(data_ov002_0211031c) != 0) goto state_check;

    if (func_ov002_020e0478(self) == 0) goto ret0a;

state_check:
    if (self->IsState(data_ov002_021101b4) != 0) {
        if (func_ov002_020e3078(self, &data_ov002_021106dc) != 0) return 0;
    }

    if (*(int *)((char *)arg + 0x18) & 0x400000) return 0;

    {
        Ret *r = arg->m2();
        if (*(int *)(p + 0x60) - r->y < -0x64000)
            *(int *)(p + 0x60) = r->y - 0x64000;
    }

    *(void **)(p + 0x37c) = arg;
    self->ChangeState(data_ov002_021106dc);
    return 1;

ret0a:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 240 -- _ZN6Player17St_Headstand_MainEv, 0x020cb15c, size 0x194 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17St_Headstand_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Headstand_Main()
{
    struct ObjHS {
        virtual void v0();
        virtual void v1();
        virtual ObjHS* v2();
    };
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int d, unsigned int e);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_020a0e40;
    extern u16 data_0209f49c[];
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a4[];
    extern void func_ov002_020cc05c(void*, unsigned short);
    extern int data_ov002_02110724[];
    extern int data_ov002_021106dc[];
    ObjHS* obj = *(ObjHS**)((char*)&unk_37c);
    ObjHS* r = obj->v2();
    mPosX = *(int*)r;
    mPosZ = *(int*)((char*)r+8);
    mPosY = *(int*)((char*)r+4) + *(int*)((char*)(*(ObjHS**)((char*)&unk_37c))+8);

    u8 st = mStateStep;
    switch (st) {
    case 2:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x1d, 0, 0x1000, 0);
            mStateStep = 3;
        }
        break;
    case 3: {
        int idx = data_020a0e40 * 0x18;
        s16 val = *(s16*)((char*)data_0209f4a4 + idx);
        u16 mag = *(u16*)((char*)data_0209f49c + idx);
        if (val > 0x200) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x1f, 0x40000000, 0x1000, 0);
            mStateStep = 4;
        }
        func_ov002_020cc05c(((char*)this), mag);
        {
            s16* p = (s16*)((char*)&mAngleY);
            *p = *p + mAngleYSpeed;
        }
        if (*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2) {
            mPrevAngleY = mAngleY;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110724);
            return 1;
        }
        break;
    }
    case 4:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021106dc);
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 241 -- _ZN6Player17St_Headstand_InitEv, 0x020cb2f0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17St_Headstand_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Headstand_Init()
{
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int d);
    extern void func_0200d580(void*, int playerID);
    extern struct Camera *data_0209f318;
  _ZN6Player7SetAnimEji5Fix12IiEj(((void *)this), 0x1e, 0x40000000, 0x1000, 0);
  *(unsigned char*)((char*)&mStateStep)=2;
  *(int*)((char*)&mVertAccel)=0;
  *(int*)((char*)&mHorzSpeed)=0;
  *(int*)((char*)&mVertSpeed)=0;
  func_0200d580(data_0209f318, *(unsigned char*)((char*)&mPlayerNo));
  *(unsigned char*)((char*)&unk_717)=1;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 242 -- func_ov002_020cb354, 0x020cb354, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cb354
/* recovered: shared common types */
extern "C" {
int func_ov002_020cb354(void* thisptr)
{
    extern int _ZN6Player6IsAnimEj(void*, unsigned int);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int, bool);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, const Vector3&);
    unsigned char* p = (unsigned char*)thisptr;
    int a = _ZN6Player6IsAnimEj(thisptr, 0x24);
    if (a == 0) return a;
    int idx = _ZNK6Player14GetBodyModelIDEjb(thisptr, *(unsigned int*)(p + 8) & 0xff, 0);
    int* m = *(int**)(p + (idx << 2) + 0xdc);
    int* q = (int*)(((int)m + 0x50));
    if ((int)((unsigned int)((q[2]) << 4) >> 0x10) < 8) {
        unsigned char f = *(unsigned char*)(p + 0x70c);
        if (f != 0) return f;
        if (*(int*)(*(int*)(p + 0x37c) + 0x18) & 0x1000000)
            _ZN5Sound9PlayBank0EjRK7Vector3(0xd, *(Vector3*)(p + 0x74));
        else
            _ZN5Sound9PlayBank0EjRK7Vector3(0xb0, *(Vector3*)(p + 0x74));
        *(unsigned char*)(p + 0x70c) = 1;
        return 1;
    }
    *(unsigned char*)(p + 0x70c) = 0;
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 243 -- func_ov002_020cb400, 0x020cb400, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cb400
extern "C" {
int func_ov002_020cb400(char* c){
    extern int func_02012194(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
  int v = *(short*)(c+0x600+0x9c) / 16;
  if (v >= 0x100) v = 0x100;
  int code = 0x107;
  if (*(int*)(*(int*)(c+0x37c)+0x18) & 0x1000000) code = 0x105;
  int r = func_02012194(*(int*)(c+0x620), 0, code, 3, v, (int)(c+0x74), 0);
  *(int*)(c+0x620) = r;
  return r;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 244 -- func_ov002_020cb474, 0x020cb474, size 0xf4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cb474
extern "C" {
void func_ov002_020cb474(char* c){
    extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int c, int d, int e, const void* f, void*);
    extern signed char data_0209f2f8;
  int* r1 = *(int**)(c+0x37c);
  if ((*(int*)((char*)r1+0x18) & 0x1000000) == 0) return;
  if (((*(int*)((char*)r1+8)) >> 2) > *(int*)(c+0x688)) return;
  volatile int v[3];
  int z = *(int*)(c+0x64);
  int y = *(int*)(c+0x60) + 0x32000;
  int x = *(int*)(c+0x5c);
  v[0] = z ? x : x;
  v[1] = y;
  v[2] = z;
  unsigned int id;
  switch (data_0209f2f8) {
    case 0x13:
    case 0xa:
      id = 0xba;
      break;
    case 6:
      id = 0xb8;
      break;
    case 0x10:
      return;
    default:
      id = 0xb9;
      break;
  }
  *(void**)(c+0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    *(volatile unsigned int*)(c+0x628), id, v[0], v[1], v[2], 0, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 245 -- _ZN6Player16St_Climb_CleanupEv, 0x020cb568, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16St_Climb_CleanupEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Climb_Cleanup()
{
    extern int func_ov002_020caf68(void*);
    extern int data_ov002_021106f4[];
    extern int data_ov002_021106dc[];
  int v = *(int*)((char*)&mRequestedState);
  if(v != (int)data_ov002_021106dc && v != (int)data_ov002_021106f4){
    func_ov002_020caf68(((void *)this));
    *(unsigned short*)((char*)((void *)this)+0x600+0xb6)=8;
  }
  *(unsigned short*)((char*)((void *)this)+0x600+0xa8)=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 246 -- _ZN6Player13St_Climb_MainEv, 0x020cb5bc, size 0x778 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player13St_Climb_MainEv
/* Player::St_Climb_Main (ov002:0x020cb5bc, 0x778).
 *
 * func_ov002_020cc05c takes the pad-held halfword as a second argument it
 * never reads (decl_common.h declares (void*, unsigned short); the matched
 * St_Headstand_Main passes it too). Dropping it recolors the whole
 * case-1/tail scratch web down one register (r2 -> r1, 9 words); passing it
 * keeps the data_0209f49c load live in r1 through both call sites, which is
 * the ROM's coloring.
 *
 * Honest residue (plan-cpp-language-mode section 3, principle 2):
 *  - SetAnim stays a mangled extern: it takes Fix12<int> BY VALUE, the
 *    audit's excluded-by-value class, and Player.h does not declare it.
 *  - ChangeState stays a mangled extern: Player::State is not declared yet.
 *  - The object at this+0x37c (the ground/platform node) has no real header
 *    class; its slot-2 virtual (returns the position vector) is called
 *    through a raw function-pointer read, same spelling St_Climb_Init uses,
 *    rather than a local shadow struct.
 *  - Fields are raw offsets off (char*)this; Player.h names are still
 *    placeholders below 0x768 and renaming cannot change codegen.
 */
/* slot 2 of the ground node's vtable: s32* GetPos(void) thiscall */
extern "C" {
int Player::St_Climb_Main()
{
    typedef s32* (*GroundPosFn)(void*);
    extern void Vec3_RotateYAndTranslate(void*, void*, s16 angle, void*);
    extern void func_0200d5c0(void*, u8 playerID);
    extern void func_0200d5fc(void*, s32 playerID);
    extern void ApproachAngle(void*, s16 target, s32 divisor, s32 band, s32 maxStep);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 anim, s32 a, s32 b, u32 d);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020cb354(void*);
    extern s16 GetAngleToCamera(u8 i);
    extern void _Z14ApproachLinearRsss(void*, s16 target, s16 step);
    extern void func_ov002_020bf340(void*, void*, s32 delta, s32 limit);
    extern void func_ov002_020cb400(void*);
    extern void func_ov002_020cb474(void*);
    extern int func_ov002_020cbea8(void*);
    extern void Player_AdvanceAnims(void*);
    extern void* data_0209f318;
    extern u8 data_020a0e40;
    extern u16 data_0209f49e[];
    extern u16 data_0209f49c[];
    extern s16 data_02082214[];
    extern int data_ov002_021101b4[];
    extern int data_ov002_021106f4[];
    extern u8 data_0209f4ac[];
    extern int data_ov002_0211013c[];
    extern int data_ov002_0211070c[];
    extern s16 data_0209f4a4[];
    extern void func_ov002_020cc05c(void*, unsigned short);
    char* c = (char*)this;
    s32 vbuf[6];

    mPeakY = mPosY;

    {
        char* gr = *(char**)(c + 0x37c);
        s32* p = (*(GroundPosFn*)(*(char**)gr + 8))(gr);
        vbuf[0] = p[0];
        vbuf[1] = p[1];
        vbuf[2] = p[2];
    }
    if (*(s32*)(c + 8) == 2) {
        char* gr;
        vbuf[4] = 0;
        vbuf[3] = 0;
        vbuf[5] = -0x10000;
        gr = *(char**)(c + 0x37c);
        Vec3_RotateYAndTranslate(&vbuf[0], (*(GroundPosFn*)(*(char**)gr + 8))(gr), mAngleY, &vbuf[3]);
    }
    mPosX = vbuf[0];
    mPosZ = vbuf[2];

    {
        void* cam = data_0209f318;
        if (*(u32*)(*(char**)(c + 0x37c) + 0x18) & 0x1000000) {
            func_0200d5c0(cam, mPlayerNo);
        } else {
            s32 v = (s32)(((s64)(*(s32*)(*(char**)(c + 0x37c) + 8)) * 0xb33 + 0x800) >> 12);
            if (v <= mAttachOffsetY) {
                func_0200d5c0(cam, mPlayerNo);
            } else {
                func_0200d5fc(cam, mPlayerNo);
            }
        }
    }

    switch (mStateStep) {
    case 0:
        ApproachAngle((s16*)(c + 0x69c), 0, 8, 0x2000, 0x200);
        if (FinishedAnim()) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x26, 0, 0x1000, 0);
            mStateStep = 1;
        }
        goto tail;

    case 1: {
        s16 v;
        u32 idx = (u32)data_020a0e40 * 0x18;
        u16 flagsE = *(u16*)((char*)data_0209f49e + idx);
        u16 flagsC = *(u16*)((char*)data_0209f49c + idx);
        if (flagsE & 0x400) {
            s16 ang = (s16)(mAngleY + 0x8000);
            s32 t = (u16)ang;
            s32 ti = (t >> 4) * 2;
            s16 cosv = data_02082214[ti];
            mPosX += (s32)(((s64)cosv * 0x64000 + 0x800) >> 12);
            s16 sinv = data_02082214[ti + 1];
            mPosZ += (s32)(((s64)sinv * 0x64000 + 0x800) >> 12);
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
            return 1;
        }

        {
            v = *(s16*)((char*)data_0209f4a4 + idx);
            if (v < -0x400) {
                func_ov002_020cb354(c);
                mAngleYSpeed = 0;
                _Z14ApproachLinearRsss((s16*)(c + 0x8e), GetAngleToCamera(mPlayerNo), 0x200);
                if (*(s32*)(*(char**)(c + 0x37c) + 8) <= mAttachOffsetY) {
                    if ((*(u32*)(*(char**)(c + 0x37c) + 0x18) & 0x2000000) && mVertSpeed == 0 && (s32)(*(s16*)((char*)data_0209f4a4 + (u32)data_020a0e40 * 0x18)) < -0xc00 && mStateWork == 1) {
                        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021106f4);
                    }
                    mAttachOffsetY = *(s32*)(*(char**)(c + 0x37c) + 8);
                    mVertSpeed = 0;
                } else {
                    u32 idx2 = (u32)data_020a0e40 * 0x18;
                    s32 mag;
                    if (*((u8*)data_0209f4ac + idx2) == 0) {
                        mag = (*(u16*)((char*)data_0209f49c + idx2) & 0x800) ? 0xc000 : 0x6660;
                    } else {
                        mag = 0xc000;
                    }
                    s16 av = *(s16*)((char*)data_0209f4a4 + idx2);
                    if (av < 0) av = -av;
                    s32 fx = (s32)(((s64)mag * av + 0x800) >> 12);
                    func_ov002_020bf340(c, (s32*)(c + 0xa8), 0x800, fx);
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x24, 0, 0x1000, 0);
                    s32 fx2 = (s32)(((s64)fx * 0x500 + 0x800) >> 12);
                    if (fx2 >= 0x4000) fx2 = 0x4000;
                    {
                        s32 modelIdx = GetBodyModelID((u8)(*(s32*)(c + 8)), 0);
                        char* p2 = (char*)((long long)(int)(*(char**)(c + modelIdx * 4 + 0xdc) + 0x50));
                        *(s32*)(p2 + 0xc) = fx2;
                    }
                    mStateWork = 0;
                }
                goto end;
            } else if (v > 0x200) {
                func_ov002_020cb400(c);
                *(s16*)(c + 0x6a4) = 0x1e;
                {
                    u32 idx3 = (u32)data_020a0e40 * 0x18;
                    s32 mag;
                    if (*((u8*)data_0209f4ac + idx3) == 0) {
                        mag = (*(u16*)((char*)data_0209f49c + idx3) & 0x800) ? 0x1e000 : 0xfff0;
                    } else {
                        mag = 0x1e000;
                    }
                    s16 val = *(s16*)((char*)data_0209f4a4 + idx3);
                    s32 fx = (s32)(((s64)mag * val + 0x800) >> 12);
                    func_ov002_020bf340(c, (s32*)(c + 0xa8), -0x800, fx);
                }
                mAngleYSpeed -= 0x40;
                {
                    u32 idx4 = (u32)data_020a0e40 * 0x18;
                    s16 av = *(s16*)((char*)data_0209f4a4 + idx4);
                    if (av < 0) av = -av;
                    if (mAngleYSpeed <= av) av = mAngleYSpeed;
                    mAngleYSpeed = av;
                }
                if (!IsAnim(0x25)) {
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x25, 0, 0x1000, 0);
                }
                if (mAttachOffsetY < -0x64000) {
                    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
                    return 1;
                }
                goto end;
            } else {
                *(s16*)(c + 0x6a4) = 0x1e;
                mVertSpeed = 0;
                func_ov002_020cc05c(c, flagsC);
                if (!IsAnim(0x26)) {
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x26, 0, 0x1000, 0);
                }
                goto end;
            }
        }
    }

    default:
        goto tail;
    }

end:
    if (mIsAirborne == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        return 1;
    }
    if (*(u16*)((char*)data_0209f49e + (u32)data_020a0e40 * 0x18) & 2) {
        mPrevAngleY = mAngleY;
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211070c);
        return 1;
    }

tail:
    {
        char* gr = *(char**)(c + 0x37c);
        s32* p3 = (*(GroundPosFn*)(*(char**)gr + 8))(gr);
        s32 y = p3[1];
        if ((mPosY - y) < -0x64000) {
            mPosY = y - 0x64000;
        }
    }
    if (mAttachOffsetY < -0x64000) {
        mAttachOffsetY = -0x64000;
    }
    {
        s32 t2 = *(s32*)(*(char**)(c + 0x37c) + 8);
        if (t2 <= mAttachOffsetY) {
            mAttachOffsetY = t2;
        }
    }
    if (mJumpComboTimer != 0 || (mVertSpeed != 0 && mStateStep == 1)) {
        func_ov002_020cb474(c);
    }
    {
        s16* p8e = (s16*)(c + 0x8e);
        *p8e = (s16)(*p8e + mAngleYSpeed);
    }
    {
        s32* p688 = (s32*)(c + 0x688);
        *p688 += mVertSpeed;
    }
    if (func_ov002_020cbea8(c)) {
        return 1;
    }
    {
        char* gr = *(char**)(c + 0x37c);
        s32* p4 = (*(GroundPosFn*)(*(char**)gr + 8))(gr);
        mPosY = mAttachOffsetY + p4[1];
    }
    if (*(s16*)((char*)data_0209f4a4 + (u32)data_020a0e40 * 0x18) == 0) {
        mStateWork = 1;
    }
    Player_AdvanceAnims(c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 247 -- _ZN6Player13St_Climb_InitEv, 0x020cbd34, size 0x174 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player13St_Climb_InitEv
extern "C" {
int Player::St_Climb_Init() {
    extern int Player_ReleaseHeldActor(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, const Vector3&);
    extern int func_ov002_020e3078(void*, void*);
    extern char data_ov002_021106f4[];
    char* self = (char*)this;
    Player_ReleaseHeldActor(self);

    int* bitfield = (int*)(((int)self + 0x2ec));
    *bitfield |= 4;
    *bitfield &= ~8;

    if (mHorzSpeed <= 0xa000) {
        mAngleYSpeed = 0;
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x26, 0, (Fix12i)0x1000, 0);
        mStateStep = 1;
    } else {
        int val = mHorzSpeed / 16;
        mAngleYSpeed = (short)val;
        if (mAngleYSpeed >= 0x3000)
            mAngleYSpeed = 0x3000;
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x27, 0x40000000, (Fix12i)0x1000, 0);
        mStateStep = 0;
    }

    void* base = *(void**)(self + 0x37c);
    void* vtable = *(void**)base;
    void* (*func)(void*) = *(void* (**)(void*))((char*)vtable + 8);
    void* res = func(base);

    mPosX = *(int*)res;
    mPosZ = *(int*)((char*)res + 8);
    mAttachOffsetY = mPosY - *(int*)((char*)res + 4);
    mVertAccel = 0;
    mHorzSpeed = 0;
    mVertSpeed = 0;

    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xd, *(Vector3*)(self + 0x74));

    mStateWork = 0;
    *(short*)(self + 0x6b8) = 3;
    *(int*)(self + 0x2dc) = 0x5a000;
    unk_717 = 1;

    {
        char* b = *(char**)(self + 0x37c);
        if (*(int*)(b + 0x18) & 0x1000000) {
            if ((*(int*)(b + 8) >> 2) <= *(int*)(self + 0x688)) {
                if (func_ov002_020e3078(self, data_ov002_021106f4) == 0) {
                    *(short*)(self + 0x6a8) = 0xf;
                }
            }
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 248 -- func_ov002_020cbea8, 0x020cbea8, size 0x174 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cbea8
#define LA(p) ((int)(p))
extern "C" {
int func_ov002_020cbea8(char *c)
{
    extern char *_ZN8dActor_c10FindWithIDEj(u32 id);
    extern int func_ov002_020cc01c(void*);
    char *o;
    int amt;
    int b;
    int ca8;
    int *p;
    int t;

    if (*(u32 *)(c + 0x2f8) == 0) return 0;
    if ((*(int *)(c + 0x2f4) & 0x400000) == 0) return 0;
    o = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x2f8));
    if (o == 0 || o == c) return 0;
    b = (int)(*(unsigned short *)(o + 0xc) == 0xbf);
    if (b == 0) return 0;
    if (*(int *)(c + 0x37c) == 0 || *(int *)(o + 0x37c) == 0) return 0;
    if (func_ov002_020cc01c(c) != 0 || func_ov002_020cc01c(o) != 0) return 1;
    ca8 = *(int *)(c + 0xa8);
    amt = *(int *)(c + 0x2e4);
    if (ca8 == 0 || *(int *)(o + 0xa8) == 0) amt = amt * 2;
    if (ca8 != 0) {
        p = (int *)LA(c + 0x688);
        *p += amt * 2;
        *(short *)(c + 0x69c) = 0;
        *(int *)(c + 0xa8) = 0;
    }
    if (*(int *)(o + 0xa8) != 0) {
        p = (int *)LA(o + 0x688);
        *p -= amt * 2;
        *(short *)(o + 0x69c) = 0;
        t = *(int *)(o + 0xac);
        *(int *)(o + 0xa8) = 0;
        *(int *)(o + 0xac) = t;
    }
    return 0;
}
}
#undef LA

/* -------------------------------------------------------------------------- */
/* ROM ordinal 249 -- func_ov002_020cc01c, 0x020cc01c, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cc01c
extern "C" {
int func_ov002_020cc01c(char* c){
    extern int data_ov002_021101b4[];
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
  if (*(unsigned short*)(c+0x6b8) == 0) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 250 -- func_ov002_020cc05c, 0x020cc05c, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cc05c
extern "C" {
void func_ov002_020cc05c(char *self)
{
    extern u8 data_020a0e40;
    extern s16 data_0209f4a2[];
    extern void func_ov002_020cc05c(void*, unsigned short);
    int idx = data_020a0e40 * 0x18;
    s16 curval = *(s16*)((int)self + 0x69c);
    *(s16*)((int)self + 0x69c) = curval + (s16)(int)(((s64)*(s16*)((char*)data_0209f4a2 + idx) * 0x80 + 0x800) >> 12);
    {
        s16 tv2 = *(s16*)((char*)data_0209f4a2 + idx);
    {
        s16 av = tv2;
        if (av < 0) av = (s16)(-av);
        {
            int mag32 = (int)(((s64)av * 0x480 + 0x800) >> 12);
            s16 mag = (s16)mag32;
            s16 nmag = (s16)(-mag);
            s16 cur2 = *(s16*)(self + 0x600 + 0x9c);
            s16 result;
            if (cur2 < nmag) result = nmag;
            else {
                if (cur2 <= mag) mag = cur2;
                result = mag;
            }
            *(s16*)(self + 0x600 + 0x9c) = result;
        }
    }
}
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 251 -- _ZN6Player9IsOnShellEv, 0x020cc140, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player9IsOnShellEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::IsOnShell()
{
    extern Player::State data_ov002_02110304;
  if(IsState(data_ov002_02110304)) return 1;
  *(int*)((char*)&mRidingShell)=0;
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 252 -- func_ov002_020cc16c, 0x020cc16c, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cc16c
extern "C" {
int func_ov002_020cc16c(void* c, void* state) {
    extern void func_ov002_020bdef0(void*);
    extern void func_ov002_020bdd9c(void*);
    extern void func_ov002_020d80d0(void*);
    extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void* data_ov002_02110304;
  if (*(int*)((char*)c+0x354) != 0) return 0;
  if (*(unsigned char*)((char*)c+0x6f9) != 0 || *(unsigned char*)((char*)c+0x6fb) != 0) {
    func_ov002_020bdef0(c);
    func_ov002_020bdd9c(c);
  }
  if (*(unsigned char*)((char*)c+0x6f8) != 0) func_ov002_020d80d0(c);
  *(void**)((char*)c+0x354) = state;
  _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110304);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 253 -- func_ov002_020cc1f4, 0x020cc1f4, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cc1f4
extern "C" {
void func_ov002_020cc1f4(char *c) {
    extern void _ZN6Player16St_Shell_CleanupEv(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern char data_ov002_0211013c[];
    if (*(int*)(c + 0x354) == 0) return;
    _ZN6Player16St_Shell_CleanupEv(c);
    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 254 -- _ZN6Player16St_Shell_CleanupEv, 0x020cc228, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player16St_Shell_CleanupEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Shell_Cleanup()
{
    extern void func_ov002_020bd8c0(void*, unsigned int r1);
  char* p = *(char**)((char*)&mRidingShell);
  if (p != 0) {
    *(int*)(p + 0x3c0) = 0;
    mRidingShell = 0;
    mPosY += 0x46000;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    func_ov002_020bd8c0(((char*)this), 0x33);
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 255 -- _ZN6Player13St_Shell_MainEv, 0x020cc27c, size 0x3e4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player13St_Shell_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Shell_Main()
{
    extern void* _ZN8dActor_c10FindWithIDEj(u32 id);
    extern void func_ov002_020eeca8(void*, int arg);
    extern int _ZNK10dBgCh_Actr8IsOnWallEv(void*);
    extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
    extern int func_02037e58(void*);
    extern void ApproachAngle(void*, short target, int divisor, int band, int maxStep);
    extern void _Z14ApproachLinearRiii(void*, int b, int c);
    extern void func_ov002_020e25f0(void*, int a);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void func_ov002_020e28d4(void*, int a, int b);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern int _ZN6Player6IsAnimEj(void*, u32 anim);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 anim, int a, Fix12i b, u32 d);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_020a0e40;
    extern s16 data_0209f4a0[];
    extern u16 data_0209f49e[];
    extern u16 data_0209f49c[];
    extern int data_ov002_0211004c[];
    extern int data_ov002_021104e4[];
    extern int func_02035638(void*);
    extern int func_0203564c(int);
    extern int _ZNK5dBgPi9GetClsnIDEv(void*);
    extern int AngleDiff(int, int);
    extern void*_ZNK10dBgCh_Actr13GetWallResultEv(void*);
    extern void func_ov002_020c1eb4(void*, int);
    extern void func_ov002_020cc660(void*, int);
    extern int func_ov002_020c04e0(void*);
    extern int func_ov002_020d674c(void*);
    int flag = 0;
    void* res;

    if (func_02035638((u8*)((char*)&mMeshClsn))) {
        flag = 1;
        res = (void*)func_0203564c((int)((char*)&mMeshClsn));
        if (_ZNK5dBgPi9GetClsnIDEv(res) != -1) {
            void* a = _ZN8dActor_c10FindWithIDEj((u32)_ZNK5dBgPi9GetClsnIDEv(res));
            if (a) {
                u16 type = *(u16*)((char*)a + 0xc);
                if ((type == 0x14 ? flag : 0) || (type == 0x15 ? 1 : 0)) {
                    func_ov002_020eeca8((void*)((char*)&mMeshClsn), (int)((char*)this));
                    flag = 0;
                }
            }
        }
    }

    if (_ZNK10dBgCh_Actr8IsOnWallEv((void*)((char*)&mMeshClsn))) {
        int ang = _ZN4cstd5atan2E5Fix12IiES1_(mWallNormalX, mWallNormalZ);
        if (AngleDiff(ang, mPrevAngleY) > 0x6000) {
            if (func_02037e58((u32*)((char*)_ZNK10dBgCh_Actr13GetWallResultEv((void*)((char*)&mMeshClsn)) + 4)) != 1) {
                flag = 1;
            }
        }
    }

    if (flag) {
        func_ov002_020c1eb4(((char*)this), (s16)(mAngleY + 0x8000));
        return 1;
    }

    if (mStateStep == 0) {
        s16 target;
        int accel;
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            target = mDesiredAngleY;
            accel = 0x40000;
        } else {
            target = mAngleY;
            accel = 0x18000;
        }
        s16 angleSave = mPrevAngleY;
        ApproachAngle((short*)((char*)&mPrevAngleY), target, 8, 0x800, 0x100);
        mAngleY = mPrevAngleY;
        func_ov002_020cc660(((char*)this), angleSave);
        {
            int spd = mHorzSpeed;
            int step = 0x1100;
            if (spd > 0) step = 0x600;
            _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), accel, step);
        }
        func_ov002_020c04e0(((char*)this));
        if (mIsAirborne != 0) {
            (*(u8*)((char*)&mStateStep))++;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        {
            int off = data_020a0e40 * 0x18;
            if (*(u16*)((char*)data_0209f49e + off) & 2) {
                (*(u8*)((char*)&mStateStep))++;
                mVertSpeed = (mHorzSpeed >> 2) + 0x2a000;
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                func_ov002_020e25f0(((char*)this), 0);
                return 1;
            } else if (*(u16*)((char*)data_0209f49c + off) & 0x400) {
                if (func_ov002_020d674c(((char*)this))) {
                    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211004c);
                } else {
                    mStateStep = 0;
                    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021104e4);
                }
                return 1;
            }
        }
    } else {
        if (mIsAirborne == 0) {
            mStateStep = 0;
            unk_766 = 0x1000;
            unk_760 = unk_766;
        } else {
            unk_766 = 0;
            unk_760 = unk_766;
            func_ov002_020e28d4(((char*)this), 0x1800, 0x800);
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x4b000;
            if (mVertSpeed >= 0) {
                mVertAccel = -0x8000;
                if (*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 2) {
                    mVertAccel = -0x3400;
                }
            } else {
                if ((*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 2) && mHasWings != 0) {
                    mVertAccel = -0x2000;
                    mTerminalVelocity = -0x25800;
                }
            }
        }
    }

    if (_ZN6Player12FinishedAnimEv(((char*)this)) && _ZN6Player6IsAnimEj(((char*)this), 0x35)) {
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x34, 0, 0x1000, 0);
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 256 -- func_ov002_020cc660, 0x020cc660, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cc660
extern "C" {
void func_ov002_020cc660(char *self, int angle){
    extern s32 ApproachAngle(void*, s16 from, s16 start, s16 speed, s16 max);
    extern void func_ov002_020cc660(void*, int);
 s16 v,w; int delta; int third;
 delta=angle-*(s16*)(self+0x94);
 third=*(int*)(self+0x98)/12;
 v=(s16)(int)(((long long)delta*third+0x800)>>12);
 if(v<-0x1800)v=-0x1800; else if(v>0x1800)v=0x1800;
 ApproachAngle((s16*)(self+0x75e),v,8,0x200,0x20);
 *(s16*)(self+0x762)=-*(s16*)(self+0x75e);
 *(s16*)(self+0x90)=*(s16*)(self+0x75e);
 w=(s16)(int)(((long long)*(int*)(self+0x98)*0xaa+0x800)>>12);
 if(w<0)w=0; else if(w>0x1000)w=0x1000;
 ApproachAngle((s16*)(self+0x760),w,8,0x200,0x20);
 *(u8*)(self+0x742)=1;
 *(int*)(self+0x690)=0x46000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 257 -- _ZN6Player13St_Shell_InitEv, 0x020cc780, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player13St_Shell_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Shell_Init()
{
    extern int Player_ReleaseHeldActor(void*);
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern int func_ov002_020bd928(void*, int);
  Player_ReleaseHeldActor(((char*)this));
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x35,0x40000000,0x1000,0);
  mStateStep=0;
  func_ov002_020bd928(((char*)this),0x33);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 258 -- _ZN6Player17St_HurtWater_MainEv, 0x020cc7d0, size 0x140 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17St_HurtWater_MainEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_HurtWater_Main()
{
    extern int func_ov002_020cec2c(void*);
    extern void _Z14ApproachLinearRiii(void*, int, int);
    extern int func_ov002_020ceaf4(void*);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern int _ZN6Player9GetHealthEv(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void func_ov002_020c5dec(void*, int);
    extern int func_ov002_020ceb7c(void*);
    extern int Player_AdvanceAnims(void*);
    extern int data_ov002_0211067c[];
    extern int data_ov002_021106ac[];
    extern int data_0209f32c[];
  if(func_ov002_020cec2c(((char*)this))) return 1;
  _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed),0,0x800);
  int t=func_ov002_020ceaf4(((char*)this));
  _Z14ApproachLinearRiii((int*)((char*)&mVertSpeed),t,0x800);
  if(_ZN6Player12FinishedAnimEv(((char*)this))){
    if((mStateStep&1)==0){
      mPrevAngleY=mPrevAngleY+0x8000;
      mAngleY=mPrevAngleY;
    }
    if(_ZN6Player9GetHealthEv(((char*)this))){
      if(mIsMetal==0)
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_0211067c);
      else
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_021106ac);
      mIsTakingDamage=0;
      if(mHurtDamage)
        mInvincibleTimer=0x24;
    }else{
      func_ov002_020c5dec(((char*)this),8);
      return 1;
    }
  }
  int lim=*(int*)data_0209f32c-0x50000;
  if(mPosY>=lim && (mClsnFlags&1)==0){
    if(mVertSpeed>=0){
      mPosY=lim;
      mVertSpeed=0;
    }
  }
  func_ov002_020ceb7c(((char*)this));
  Player_AdvanceAnims(((char*)this));
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 259 -- _ZN6Player17St_HurtWater_InitEv, 0x020cc910, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player17St_HurtWater_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_HurtWater_Init()
{
    extern int data_0209f318[];
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned, int, int, unsigned);
    extern void func_ov002_020d93ac(void*);
    extern void func_ov002_020d94cc(void*);
    extern int data_ov002_02109fe4[];
    extern void func_0200d89c(void*);
  unsigned a=data_ov002_02109fe4[mStateStep&1];
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),a,0x40000000,0x1000,0);
  mStateWork=0;
  mStateArg=0;
  mVertAccel=0;
  mTerminalVelocity=-0xc000;
  mVertSpeed=0x10000;
  mIsTakingDamage=1;
  if(mHurtDamage){
    int b=mStateStep&0xf0;
    if(b==0 || b==0x10){
      func_ov002_020d93ac(((char*)this));
      func_ov002_020d94cc(((char*)this));
    }
  }
  func_0200d89c(*(char**)data_0209f318);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 260 -- _ZN6Player23St_MetalWaterWater_MainEv, 0x020cc9c0, size 0x27c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player23St_MetalWaterWater_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Metal Mario walking along the bottom underwater. Losing both the metal and
 * mega caps drops back to the swim state. While mStateWork is 0 the player
 * walks and sinks -- gravity and terminal velocity depend on whether the water
 * surface (data_0209f32c) is more than 0x46000 above (0xd2000 when mega).
 * Landing plays a thud, spawns a splash particle, and either returns to the
 * ground state or starts the landing animation.
 */
extern "C" {
int Player::St_MetalWaterWater_Main()
{
    struct S18b2 { short a; short b; int c[5]; };
    extern int func_ov002_020cec2c(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020eeca8(void*, void*);
    extern int func_ov002_020bf224(void*, int a, int b);
    extern void ApproachAngle(void*, int target, int a, int b, int d);
    extern void _Z14ApproachLinearRiii(void*, int a, int b);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void*);
    extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 a, int b, int d, u32 e);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_020a0e40;
    extern struct S18b2 data_0209f4a0[];
    extern int data_0209f32c;
    extern int data_ov002_0211067c;
    extern int data_ov002_021106ac;
    /* volatile: the original pins these stores to memory in this order. */
    volatile Vector3 pos;
    u32 f;
    int v;
    int lim;
    int py;
    int px, py2, pz;

    if (func_ov002_020cec2c(this) != 0) return 1;
    f = mIsMetal;
    if (f == 0 && mIsMega == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211067c);
        return 1;
    }
    if (f != 0) func_ov002_020eeca8(&mMeshClsn, this);

    if (mStateWork == 0) {
        v = 0;
        if (data_0209f4a0[data_020a0e40].a != 0) {
            v = func_ov002_020bf224(this, 0x14000, 0x1000);
            ApproachAngle(&mPrevAngleY, mDesiredAngleY, 0x20, 0x1000, 0x10);
        }
        mAngleY = mPrevAngleY;
        _Z14ApproachLinearRiii(&mHorzSpeed, v, 0x400);
        py = mPosY;
    lim = py + 0x46000;
    if (mIsMega != 0) lim = py + 0xd2000;
        if (data_0209f32c > lim) {
            mVertAccel = -0x1800;
            mTerminalVelocity = -0x14000;
        } else {
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x4b000;
        }
        if (mIsAirborne == 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0xa9, &mCamSpacePosX);
            px = mPosX;
            pos.x = px;
            py2 = mPosY;
            pos.y = py2;
            pz = mPosZ;
            pos.z = pz;
            if (mIsMega != 0) {
                py2 += 0x19000;
                pos.y = py2;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5c, px, py2, pz);
            } else {
                py2 += 0x32000;
                pos.y = py2;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd7, px, py2, pz);
            }
            if (data_0209f4a0[data_020a0e40].a != 0) {
                _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_021106ac);
            } else {
                mHorzSpeed = 0;
                if (mStateStep != 0)
                    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x52, 0x40000000, 0x1000, 0);
                else
                    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x55, 0x40000000, 0x1000, 0);
                mStateWork = 1;
            }
        }
    } else {
        if (FinishedAnim() != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_021106ac);
            return 1;
        }
    }
    Player_AdvanceAnims(this);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 261 -- _ZN6Player23St_MetalWaterWater_InitEv, 0x020ccc3c, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player23St_MetalWaterWater_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_MetalWaterWater_Init()
{
    extern void func_ov002_020bf2d8(void*, int);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned, int, int, unsigned);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned, void*);
  mIsInAirState=1;
  mIsUnderwater=1;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  if(mStateStep==0){
    func_ov002_020bf2d8(((char*)this),0x20000);
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x53,0x40000000,0x1000,0);
  }else{
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x54,0x40000000,0x1000,0);
  }
  _ZN5Sound9PlayBank0EjRK7Vector3(0xa8,((char*)this)+0x74);
  mStateWork=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 262 -- _ZN6Player24St_MetalWaterGround_MainEv, 0x020ccccc, size 0x404 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player24St_MetalWaterGround_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_MetalWaterGround_Main()
{
    extern int func_ov002_020cec2c(void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void ApproachAngle(void*, short, int, int, int);
    extern int func_ov002_020bf224(void*, int, int);
    extern void _Z14ApproachLinearRiii(void*, int, int);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern int _ZN6Player6IsAnimEj(void*, unsigned int);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int, int);
    extern void func_0201251c(int, int, void*, int);
    extern void Player_AdvanceAnims(void*);
    extern int data_0209f32c;
    extern unsigned char data_0209f49e;
    extern unsigned char data_020a0e40;
    extern short data_0209f4a0;
    extern int data_ov002_0211013c[];
    extern int data_ov002_0211067c[];
    extern int data_ov002_021106c4[];
    extern void func_ov002_020cd190(void*);
    extern void func_ov002_020bf5e0(void*);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int);
    if(func_ov002_020cec2c(((char*)this))) return 1;
    int u703 = mIsMega;
    if((int)(data_0209f32c - 0x50000) < (int)(mPosY - 0xa000)){
        mIsUnderwater = 0;
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        return 1;
    }
    if(*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18) & 2){
        mStateStep = 0;
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021106c4);
        return 1;
    }
    if(mIsAirborne != 0 && (mPosY - mGroundY) >= 0x32000){
        mStateStep = 1;
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021106c4);
        return 1;
    }
    if(mIsMetal == 0 && u703 == 0){
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211067c);
        return 1;
    }
    func_ov002_020cd190(((char*)this));
    int approach = 0;
    if(*(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18) != 0){
        ApproachAngle((short*)((char*)&mPrevAngleY), mDesiredAngleY, 8, 0x1000, 0x80);
        approach = func_ov002_020bf224(((char*)this), 0x14000, 0x1000);
    }
    _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), approach, 0x1000);
    mAngleY = mPrevAngleY;
    if(mIsMega != 0){
        if(mHorzSpeed == 0){
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0xa0, 0, 0x1000, 0);
        }else if(_ZN6Player6IsAnimEj(((char*)this), 0xa0) != 0){
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x9f, 0x40000000, 0x1000, 0);
        }else if(_ZN6Player6IsAnimEj(((char*)this), 0x9f) != 0){
            if(_ZN6Player12FinishedAnimEv(((char*)this)) != 0){
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x9e, 0, *(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18) * 2, 0);
            }
        }else if(_ZN6Player6IsAnimEj(((char*)this), 0x9f) == 0){
            int v = *(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18);
            if(v != 0){
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x9e, 0, v * 2, 0);
                func_ov002_020bf5e0(((char*)this));
            }else{
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x9d, 0x40000000, 0x1000, 0);
            }
        }
    }else if(mHorzSpeed == 0){
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x47, 0, 0x1000, 0);
    }else{
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x48, 0, 0x1000, 0);
        int r5 = (int)((((long long)mHorzSpeed << 9) + 0x800) >> 12);
        if(r5 < 0x400) r5 = 0x400;
        char* anim = *(char**)(((char*)this) + _ZNK6Player14GetBodyModelIDEjb(((char*)this), (unsigned char)param1, 0) * 4 + 0xdc) + 0x50;
        *(int*)(anim + 0xc) = r5;
        if(_ZNK9Animation12WillHitFrameEi((void*)(*(char**)(((char*)this) + _ZNK6Player14GetBodyModelIDEjb(((char*)this), (unsigned char)param1, 0) * 4 + 0xdc) + 0x50), 4) != 0 || _ZNK9Animation12WillHitFrameEi((void*)(*(char**)(((char*)this) + _ZNK6Player14GetBodyModelIDEjb(((char*)this), (unsigned char)param1, 0) * 4 + 0xdc) + 0x50), 0x13) != 0){
            func_0201251c(0, 0xaa, (void*)((char*)&mCamSpacePosX), mHorzSpeed);
        }
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 263 -- _ZN6Player24St_MetalWaterGround_InitEv, 0x020cd0d0, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player24St_MetalWaterGround_InitEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_MetalWaterGround_Init()
{
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned, unsigned, void*);
    int* p = *(int**)((char*)&mHeldObj);
    int b0 = (p != 0);
    if (b0) {
        int h = *(unsigned short*)((char*)p+0xc);
        int b1 = (h == 0x10c);
        if (!b1) {
            int b2 = (h == 0x10b);
            if (!b2) goto tail;
        }
        *(int*)(int)((char*)p+0xb0) &= ~0x4000;
        *(int*)(int)((char*)*(int**)((char*)&mHeldObj)+0xb0) &= ~0x100;
        *(int**)((char*)&mHeldObj) = 0;
    }
tail:
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x2e, ((char*)this)+0x74);
    mVertAccel = -0x1800;
    mTerminalVelocity = -0x14000;
    if (mHorzSpeed >= 0x14000) mHorzSpeed = 0x14000;
    mIsUnderwater = 1;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 264 -- func_ov002_020cd190, 0x020cd190, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd190
extern "C" {
void func_ov002_020cd190(char* c){
    extern void func_ov002_020ce8bc(void*, int x);
    extern int data_0209f32c;
    extern void func_ov002_020cd190(void*);
  if(*(unsigned char*)(c+0x703)==0) return;
  if(*(int*)(c+0x60) + 0x180000 <= data_0209f32c) return;
  func_ov002_020ce8bc(c, *(int*)(c+0x98));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 265 -- _ZN6Player15St_Swim_CleanupEv, 0x020cd1e4, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player15St_Swim_CleanupEv
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Swim_Cleanup()
{
    extern int func_ov002_020bd8c0(void*, int);
  if(mSwimMusicPushed!=0){
    mSwimMusicPushed=0;
    func_ov002_020bd8c0(((char*)this),0x33);
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 266 -- func_ov002_020cd218, 0x020cd218, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd218
extern "C" {
void func_ov002_020cd218(char* c, int v, int* out1, int* out2){
    extern short data_02082214[];
    extern void func_ov002_020cd218(void*, int, void*, void*);
  *out1 = (int)(((long long)v * data_02082214[(*(unsigned short*)(c+0x92) >> 4)*2+1] + 0x800) >> 12);
  *out2 = -(int)(((long long)v * data_02082214[(*(unsigned short*)(c+0x92) >> 4)*2] + 0x800) >> 12);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 267 -- func_ov002_020cd28c, 0x020cd28c, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd28c
extern "C" {
int func_ov002_020cd28c(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0xaf,0x40000000,0x1000,0);
  *(unsigned char*)(c+0x6e3)=7;
  return 7;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 268 -- func_ov002_020cd2c4, 0x020cd2c4, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd2c4
extern "C" {
void func_ov002_020cd2c4(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
    extern void func_ov002_020cd2c4(void*);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0xab,0x40000000,0x1000,0);
  *(unsigned char*)(c+0x6e3)=1;
  *(unsigned short*)(c+0x6a4)=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 269 -- func_ov002_020cd308, 0x020cd308, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd308
extern "C" {
void func_ov002_020cd308(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
    extern void func_ov002_020cd308(void*);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0xae,0x40000000,0x1000,0);
  *(unsigned char*)(c+0x6e3)=6;
  *(unsigned short*)(c+0x6a4)=0x10;
  *(unsigned char*)(((int)c+0x6e5))&=0xa;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 270 -- func_ov002_020cd364, 0x020cd364, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd364
extern "C" {
void func_ov002_020cd364(char *c) {
    extern void func_ov002_020dc020(void*);
    extern void _ZN5dCc_c5ClearEv(void*);
    extern void _ZN5dCc_c6UpdateEv(void*);
    extern void func_ov002_020cd364(void*);
    func_ov002_020dc020(c);
    _ZN5dCc_c5ClearEv(c + 0x314);
    _ZN5dCc_c6UpdateEv(c + 0x314);
    *(short *)(c + 0x600 + 0xa4) = 2;
    *(unsigned char *)(c + 0x6e3) = 5;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 271 -- func_ov002_020cd39c, 0x020cd39c, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd39c
extern "C" {
void func_ov002_020cd39c(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int a, int b, int f, unsigned int g);
    extern void func_ov002_020cd39c(void*);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0xaa,0x40000000,0x1000,0);
  *(unsigned char*)(c+0x6e3)=4;
  *(unsigned short*)(c+0x6a4)=7;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 272 -- func_ov002_020cd3e0, 0x020cd3e0, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd3e0
extern "C" {
void func_ov002_020cd3e0(char* c){
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, int, int, int, unsigned int);
    extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
    extern void func_ov002_020cd3e0(void*);
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa9, 0x40000000, 0x1000, 0);
  *(unsigned char*)(c+0x6e3)=2;
  *(short*)(c+0x6a4)=0xf;
  *(unsigned char *)(((int)c + 0x6e5)) &= 0xfe;
  _ZN5Sound9PlayBank0EjRK7Vector3(0x14, c+0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 273 -- func_ov002_020cd448, 0x020cd448, size 0x108 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd448
extern "C" {
void func_ov002_020cd448(char* self) {
    extern int func_ov002_020c18b0(void*, int x);
    extern void _Z15ApproachLinear2Rsss(short&, short, short);
    extern unsigned char data_020a0e40;
    extern short data_0209f4a4;
    extern void func_ov002_020cd448(void*);
    unsigned char fl;
    if (*(unsigned char*)(self + 0x6e3) < 2) goto tail;
    fl = *(unsigned char*)(self + 0x6e9);
    if (fl & 1) {
        short before;
        *(short*)(self + 0x8c) = *(short*)(self + 0x92);
        before = *(short*)(self + 0x8c);
        func_ov002_020c18b0(self, 0);
        if (before < *(short*)(self + 0x8c)) goto tail;
        _Z15ApproachLinear2Rsss(*(short*)(self + 0x92), *(short*)(self + 0x8c), 0x200);
        goto tail;
    }
    if (fl & 2) {
        if (*(short*)((char*)&data_0209f4a4 + data_020a0e40 * 0x18) != 0) goto tail;
        if (*(short*)(self + 0x92) > 0) {
            short* q = (short*)(((int)self + 0x92));
            *q += 0x200;
            if (*(short*)(self + 0x92) > 0x3f00) *(short*)(self + 0x92) = 0x3f00;
        } else {
            short* q = (short*)(((int)self + 0x92));
            *q -= 0x200;
            if (*(short*)(self + 0x92) < -0x3f00) *(short*)(self + 0x92) = -0x3f00;
        }
    }
tail:
    _Z15ApproachLinear2Rsss(*(short*)(self + 0x69e), *(short*)(self + 0x90), 0x200);
    *(short*)(self + 0x90) = *(short*)(self + 0x69e);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 274 -- func_ov002_020cd550, 0x020cd550, size 0x1cc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd550
/* the += / -= on 0x92 go through a materialized base (add r1,c,#0x92;
   ldrsh/strh [r1]); the (long long)-mask launder stops mwcc folding it. */
#define LS16(p) (*(short*)(int)(p))
extern "C" {
void func_ov002_020cd550(char* c)
{
    struct S18a { short f0; char pad[0x16]; };
    extern u8 data_020a0e40;
    extern struct S18a data_0209f4a4[];
    extern short data_02082214[];
    extern void func_ov002_020cd448(void*);
    extern void func_ov002_020cd550(void*);
    short lim, v, w, a;
    int step, idx;

    lim = (short)(int)((((long long)data_0209f4a4[data_020a0e40].f0 * -0x3f00LL) + 0x800) >> 12);
    lim &= 0xfff0;

    step = *(short*)(c + 0x92) > 0 ? 0x100 : 0x200;
    if (*(short*)(c + 0x92) < lim) {
        LS16(c + 0x92) += step;
        if (*(short*)(c + 0x92) > lim) *(short*)(c + 0x92) = lim;
    } else if (*(short*)(c + 0x92) > lim) {
        LS16(c + 0x92) -= step;
        if (*(short*)(c + 0x92) < lim) *(short*)(c + 0x92) = lim;
    }

    func_ov002_020cd448(c);

    v = *(short*)(c + 0x92);
    if (v < 0) *(short*)(c + 0x8c) = v * 6 / 10;
    else *(short*)(c + 0x8c) = v * 10 / 8;

    *(int*)(c + 0x690) = 0;
    w = *(short*)(c + 0x8c);
    if (w >= 0)
        *(int*)(c + 0x690) = (int)((((long long)w * 0xc000LL) + 0x800) >> 12);

    a = (short)(*(short*)(c + 0x8c) - 0x2000);
    if (a < -0x2000) a = -0x2000;
    if (a > 0) a = 0;
    idx = (u16)(short)(a * 2) >> 4;
    *(int*)(c + 0x694) = data_02082214[idx * 2 + 1] * -0x30;
}
}
#undef LS16

/* -------------------------------------------------------------------------- */
/* ROM ordinal 275 -- func_ov002_020cd71c, 0x020cd71c, size 0x230 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cd71c
extern "C" {
void func_ov002_020cd71c(char *self)
{
    typedef short s16;
    extern int data_ov002_020ff1f0[];
    extern unsigned char data_020a0e40[];
    extern short data_0209f4a2[];
    extern void _Z15ApproachLinear2Rsss(void*, short t, short s);
    extern void func_ov002_020cd71c(void*);
    int ip = data_ov002_020ff1f0[*(int *)(self + 8)];
    int a = (int)(((long long)ip * 0x320 + 0x800) >> 12);
    int c = (int)((((long long)ip << 7) + 0x800) >> 12);
    s16 step = (s16)((((long long)ip << 5) + 0x800) >> 12);
    int b = (int)(((long long)ip * 0x240 + 0x800) >> 12);
    int e = *(short *)((char *)data_0209f4a2 + data_020a0e40[0] * 0x18);
    s16 target = (s16)(((long long)(-b) * e + 0x800) >> 12);

    if (*(int *)(self + 0x640) == 0) {
        step = (s16)((((long long)ip << 6) + 0x800) >> 12);
        target = (s16)(((long long)(-a) * e + 0x800) >> 12);
    }

    if (target > 0) {
        if (*(s16 *)(self + 0x69c) < 0) {
            s16 *q = (s16 *)(((int)self + 0x69c));
            *q = *q + 0x40;
            if (*(s16 *)(self + 0x69c) > c)
                *(s16 *)(self + 0x69c) = c;
        } else {
            _Z15ApproachLinear2Rsss((short *)(self + 0x69c), target, step);
        }
    } else if (target < 0) {
        if (*(s16 *)(self + 0x69c) > 0) {
            s16 *q = (s16 *)(((int)self + 0x69c));
            *q = *q - 0x40;
            if (*(s16 *)(self + 0x69c) < -c)
                *(s16 *)(self + 0x69c) = -c;
        } else {
            _Z15ApproachLinear2Rsss((short *)(self + 0x69c), target, step);
        }
    } else {
        _Z15ApproachLinear2Rsss((short *)(self + 0x69c), 0, 0x40);
    }

    {
        s16 *q = (s16 *)(((int)self + 0x8e));
        *q = *q + *(s16 *)(self + 0x69c);
    }
    *(s16 *)(self + 0x90) = -(*(s16 *)(self + 0x69c) << 3);
    *(s16 *)(self + 0x94) = *(s16 *)(self + 0x8e);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 276 -- _ZN6Player12St_Swim_MainEv, 0x020cd94c, size 0x9d8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12St_Swim_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Swimming, per frame. Metal cap diverts to the metal-underwater state and
 * surfacing to the walking one. mStateStep selects the stroke phase; each
 * phase drives the stroke animation, the body model's animation block at
 * +0x50, and the forward impulse, with a kick sound on the frame the
 * animation crosses its trigger.
 */
extern "C" {
int Player::St_Swim_Main()
{
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int _ZN6Player9GetHealthEv(void);
    extern void func_ov002_020c5dec(void*, int);
    extern int func_ov002_020cede0(void*);
    extern int func_ov002_020cec2c(void*);
    extern void func_ov002_020cd71c(void*);
    extern void func_ov002_020cd550(void*);
    extern unsigned int func_02022d44(unsigned int, unsigned int, int, int, int, void*);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
    extern void* _ZNK10dBgCh_Actr14GetFloorResultEv(void*);
    extern void* func_02037dc4(void*);
    extern int _ZN4cstd5atan2E5Fix12IiES1_(int, int);
    extern void Vec3_RotateYAndTranslate(void*, void*, short, void*);
    extern void _Z14ApproachLinearRiii(void*, int, int);
    extern int _ZN6Player6IsAnimEj(void*, unsigned int);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned char, int);
    extern int func_ov002_020ce324(void*);
    extern void func_ov002_020cd364(void*);
    extern void func_ov002_020cd3e0(void*);
    extern int func_ov002_020cedb0(void*, int);
    extern void func_ov002_020cd39c(void*);
    extern int _ZNK9Animation12WillHitFrameEi(void*, int);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
    extern void func_ov002_020cd2c4(void*);
    extern void func_ov002_020cd308(void*);
    extern void func_ov002_020dbf4c(void*);
    extern void _ZN5dCc_c5ClearEv(void*);
    extern void _ZN5dCc_c6UpdateEv(void*);
    extern void func_ov002_020bd928(void*, unsigned int);
    extern void func_ov002_020bd8c0(void*, unsigned int);
    extern void Player_ReleaseHeldActor(void*);
    extern void func_ov002_020daa74(void*);
    extern void func_ov002_020cd218(void*, int, void*, void*);
    extern int func_ov002_020ceaf4(void*);
    extern int func_ov002_020ce5f8(void*);
    extern void Player_AdvanceAnims(void*);
    extern char data_ov002_021106ac[];
    extern char data_ov002_021101b4[];
    extern unsigned char data_020a0e40;
    extern unsigned short data_0209f49c;
    extern unsigned short data_0209f49e;
    extern char data_0209ee90[];
    int t4;
    volatile int pos[3];
    int* fr;
    int ang;
    int out[3];
    int vec[3];
    short yaw;
    int var_r2;
    unsigned short a4;
    int var_r6;
    int var_r1;
    int has;
    int* p;
    int* sub;
    unsigned int t5;


    if (mIsMetal != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_021106ac);
        return 1;
    }
    if (_ZN6Player9GetHealthEv() == 0) {
        func_ov002_020c5dec(this, 8);
        return 1;
    }
    if (func_ov002_020cede0(this) == 0) {
        mIsUnderwater = 0;
        _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_021101b4);
        return 1;
    }
    mPeakY = mPosY;
    if (func_ov002_020cec2c(this) != 0) return 1;

    t4 = (*(unsigned short*)((char*)&data_0209f49c + data_020a0e40 * 0x18)) & 0x802;
    unk_69e = mAngleZ;
    func_ov002_020cd71c(this);
    func_ov002_020cd550(this);

    if (mJumpComboTimer != 0) {
        int eid = 0xcd;
        int y, z;
        pos[0] = mPosX;
        y = mPosY;
        pos[1] = y;
        z = mPosZ;
        pos[2] = z;
        y = y + 0x32000;
        pos[1] = y;
        *(unsigned int*)&mParticle3 = func_02022d44(*(volatile unsigned int*)(&mParticle3), eid, pos[0], pos[1], z, 0);
    }

    if (mPrevVertSpeed == 0 && _ZNK10dBgCh_Actr10IsOnGroundEv(&mMeshClsn) != 0) {
        fr = (int*)func_02037dc4((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(&mMeshClsn) + 4);
        ang = fr[1];
        if (ang < 0x720 && ang > 0x400 && mPosY >= unk_64c - 0xc8000) {
            int at;
            mPosY = mPreClsnPosY - 0x8000;
            at = _ZN4cstd5atan2E5Fix12IiES1_(fr[0], fr[2]);
            vec[0] = 0;
            vec[1] = 0;
            at = at + 0x8000;
            yaw = (short)at;
            vec[2] = (*(int*)((char*)data_0209ee90 + 0x244)) << 0xc;
            Vec3_RotateYAndTranslate(out, &mPosX, yaw, vec);
            mPosX = out[0];
            mPosY = out[1];
            mPosZ = out[2];
        }
    }

    switch (mStateStep) {
    case 0:
        mPrevAngleX = -0x4000;
        mPrevAngleZ = 0;
        mAngleX = 0;
        mAngleZ = 0;
        mAngleYSpeed = 0;
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), 0, 0x5000);
        if (_ZN6Player6IsAnimEj(this, 0xa8) == 0 && _ZN6Player12FinishedAnimEv(this) != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xa8, 0, 0x1000, 0);
        }
        if (mPrevVertSpeed == 0) {
            mStateStep = 1;
            mPrevAngleX = 0;
            mStateTimer = 0x1e;
        }
        break;
    case 1:
        var_r2 = 0x800;
        if (mStateTimer != 0) var_r2 = 0x200;
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), 0, var_r2);
        if (_ZN6Player6IsAnimEj(this, 0xa8) == 0) {
            if (_ZN6Player12FinishedAnimEv(this) != 0) {
                _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xa8, 0, 0x1000, 0);
            }
        } else if (mAngleX >= 0x1000) {
            sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
            if (*(int*)((char*)sub + 0xc) != 0x3000) {
                sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
                *(int*)((char*)sub + 0xc) = 0x3000;
            }
        } else if (mAngleYSpeed != 0) {
            sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
            if (*(int*)((char*)sub + 0xc) != 0x2000) {
                sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
                *(int*)((char*)sub + 0xc) = 0x2000;
            }
        } else {
            sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
            if (*(int*)((char*)sub + 0xc) != 0x1000) {
                sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
                *(int*)((char*)sub + 0xc) = 0x1000;
            }
        }
        if (func_ov002_020ce324(this) != 0) return 1;
        if ((*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18)) & 1) {
            func_ov002_020cd364(this);
        } else if (t4 != 0) {
            func_ov002_020cd3e0(this);
        }
        break;
    case 2:
        a4 = mStateTimer;
        if (a4 >= 0xa) var_r6 = 0x800;
        else if (a4 <= 4) var_r6 = 0x1800;
        else var_r6 = 0;
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), func_ov002_020cedb0(this, 0x10000), var_r6);
        if (mStateTimer < 0xa && t4 == 0) {
            mStateWork |= 1;
        }
        if (mStateTimer == 0 && _ZN6Player6IsAnimEj(this, 0xa9) != 0 && _ZN6Player12FinishedAnimEv(this) != 0) {
            if (!(mStateWork & 1)) {
                mStateStep = 3;
            } else {
                func_ov002_020cd39c(this);
            }
        }
        break;
    case 3:
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), func_ov002_020cedb0(this, 0xc000), 0x200);
        if (func_ov002_020ce324(this) != 0) return 1;
        if (t4 != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xa6, 0, 0x1000, 0);
            if (_ZNK9Animation12WillHitFrameEi((void*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc) + 0x50), 0) != 0) {
                _ZN5Sound9PlayBank0EjRK7Vector3(0x15, &mCamSpacePosX);
            }
        } else {
            func_ov002_020cd39c(this);
        }
        break;
    case 4:
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), 0, 0x400);
        if (func_ov002_020ce324(this) != 0) return 1;
        if (mStateTimer == 0 && t4 != 0) {
            func_ov002_020cd3e0(this);
        }
        if (_ZN6Player12FinishedAnimEv(this) != 0 && _ZN6Player6IsAnimEj(this, 0xaa) != 0) {
            func_ov002_020cd2c4(this);
        }
        break;
    case 5:
        if (mStateTimer == 0) {
            func_ov002_020cd308(this);
        }
        break;
    case 6:
        func_ov002_020dbf4c(this);
        var_r1 = func_ov002_020cedb0(this, 0x10000);
        a4 = mStateTimer;
        if (a4 >= 5) var_r2 = 0x1000;
        else if (a4 == 0) { var_r2 = 0x800; var_r1 = 0; }
        else var_r2 = 0;
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), var_r1, var_r2);
        if (_ZN6Player12FinishedAnimEv(this) != 0) {
            has = (mHeldObj != 0) ? 1 : 0;
            if (has != 0) {
                mStateStep = 8;
                mStateTimer = 0xf0;
                p = (int*)(mHeldObj + 0xb0);
                *p |= 0x4000;
                _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xb0, 0, 0x1000, 0);
            } else {
                func_ov002_020cd2c4(this);
            }
        }
        _ZN5dCc_c5ClearEv(&mAttackClsn);
        sub = (int*)((char*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc)) + 0x50);
        t5 = ((unsigned int)(*(int*)((char*)sub + 8)) << 4) >> 0x10;
        if ((int)t5 >= 5 && (int)t5 <= 7) {
            _ZN5dCc_c6UpdateEv(&mAttackClsn);
        }
        break;
    case 7:
        if (_ZN6Player12FinishedAnimEv(this) != 0) {
            mStateStep = 8;
            mStateTimer = 0xf0;
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xb0, 0, 0x1000, 0);
            p = (int*)(mHeldObj + 0xb0);
            *p |= 0x4000;
            func_ov002_020bd928(this, 0x33);
            mSwimMusicPushed = 1;
        }
        break;
    case 8:
        _Z14ApproachLinearRiii((int*)(&mPrevVertSpeed), func_ov002_020cedb0(this, 0x1e000), 0x400);
        if ((*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18)) & 1) {
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0xb1, 0x40000000, 0x1000, 0);
            mStateStep = 9;
            func_ov002_020bd8c0(this, 0x33);
        }
        if (mStateTimer != 0) {
            has = (mHeldObj != 0) ? 1 : 0;
            if (has == 0) goto case8_block107;
        } else {
        case8_block107:
            Player_ReleaseHeldActor(this);
            func_ov002_020cd2c4(this);
            func_ov002_020bd8c0(this, 0x33);
            mSwimMusicPushed = 0;
        }
        if (_ZNK9Animation12WillHitFrameEi((void*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc) + 0x50), 0) != 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0x15, &mCamSpacePosX);
        }
        break;
    case 9:
        if (_ZNK9Animation12WillHitFrameEi((void*)(*(int*)((char*)this + _ZNK6Player14GetBodyModelIDEjb(this, (unsigned char)*(int*)&param1, 0)*4 + 0xdc) + 0x50), 0xd) != 0) {
            func_ov002_020daa74(this);
        }
        if (_ZN6Player12FinishedAnimEv(this) != 0) {
            func_ov002_020cd2c4(this);
        }
        break;
    }

    func_ov002_020cd218(this, mPrevVertSpeed, &mHorzSpeed, &mVertSpeed);
    {
        int* pa8 = (int*)((unsigned int)(&mVertSpeed));
        *pa8 = *pa8 + func_ov002_020ceaf4(this);
    }
    if (func_ov002_020ce5f8(this) != 0) return 1;
    Player_AdvanceAnims(this);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 277 -- func_ov002_020ce324, 0x020ce324, size 0x22c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ce324
extern "C" {
int func_ov002_020ce324(char* c)
{
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef struct Vec3i { int x, y, z; } Vec3i;
    typedef struct dActor_c dActor_c;
    struct S18a  { short f0; char pad[0x16]; };
    struct S18b { u16 f0; char pad[0x16]; };
    extern u8 data_020a0e40;
    extern struct S18b data_0209f49e[];
    extern struct S18a data_0209f4a4[];
    extern short data_02082214[];
    extern char data_ov002_02110694;
    extern int func_ov002_020ceb54(void*);
    extern void _ZN9dBgCh_LinC1Ev(void*);
    extern void _ZN9dBgCh_LinD1Ev(void*);
    extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void*, const Vec3i* a, const Vec3i* b, void*);
    extern int _ZN9dBgCh_Lin10DetectClsnEv(void*);
    extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(const void* self, void*);
    extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
    extern int AngleDiff(int a, int b);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020ce324(void*);
    Vec3i a1, a2, b1, b2, nrm;
    char rl1[0x78];
    int ok;

    if (!(data_0209f49e[data_020a0e40].f0 & 2)) return 0;
    if (func_ov002_020ceb54(c)) return 0;

    _ZN9dBgCh_LinC1Ev(rl1);
    a1.x = *(int*)(c + 0x5c);
    a1.y = *(int*)(c + 0x60);
    a1.z = *(int*)(c + 0x64);
    a2.x = *(int*)(c + 0x5c);
    a2.y = *(int*)(c + 0x60);
    a2.z = *(int*)(c + 0x64);
    a1.y += 0x50000;
    a2.y += 0x96000;
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl1, &a1, &a2, (dActor_c*)c);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl1)) {
        _ZN9dBgCh_LinD1Ev(rl1);
        return 0;
    }

    ok = 0;
    if (data_0209f4a4[data_020a0e40].f0 >= 0xa00) {
        ok = 1;
    } else {
        char rl2[0x78];
        int ax, bz, bx, az, by, ay;
        u16 ang = *(u16*)(c + 0x8e);

        _ZN9dBgCh_LinC1Ev(rl2);
        ax = *(int*)(c + 0x5c);
        az = *(int*)(c + 0x64);
        ay = *(int*)(c + 0x60) + 0x32000;
        b1.x = ax;
        b1.y = ay;
        b1.z = az;
        bz = *(int*)(c + 0x64) + data_02082214[(ang >> 4) * 2 + 1] * 0x64;
        bx = *(int*)(c + 0x5c) + data_02082214[(ang >> 4) * 2] * 0x64;
        by = *(int*)(c + 0x60) + 0x32000;
        b2.x = bx;
        b2.y = by;
        b2.z = bz;
        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl2, &b1, &b2, (dActor_c*)c);
        if (_ZN9dBgCh_Lin10DetectClsnEv(rl2)) {
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rl2 + 0x14, &nrm);
            if (nrm.y == 0) {
                if (AngleDiff(_ZN4cstd5atan2E5Fix12IiES1_(nrm.x, nrm.z), *(short*)(c + 0x8e)) >= 0x5000)
                    ok = 1;
            }
        }
        _ZN9dBgCh_LinD1Ev(rl2);
    }

    if (ok == 1) {
        *(u8*)(c + 0x706) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110694);
        _ZN9dBgCh_LinD1Ev(rl1);
        return 1;
    }
    _ZN9dBgCh_LinD1Ev(rl1);
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 278 -- _ZN6Player12St_Swim_InitEv, 0x020ce550, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player12St_Swim_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_Swim_Init()
{
    extern void Player_ReleaseHeldActor(void*);
    extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned, unsigned, void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned, int, int, unsigned);
    extern int data_0209f318[];
    extern void func_0200d768(void*, unsigned char);
  Player_ReleaseHeldActor(((char*)this));
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter,0x2e,((char*)this)+0x74);
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0xa7,0x40000000,0x1000,0);
  mStateStep=0;
  mVertAccel=0;
  mTerminalVelocity=-0xc000;
  mIsUnderwater=1;
  func_0200d768(*(void**)data_0209f318,mPlayerNo);
  mPrevVertSpeed=mVertSpeed;
  mHorzSpeed=0;
  mVertSpeed=0;
  mAngleYSpeed=0;
  mStateWork=0;
  mStateArg=0;
  mJumpComboTimer=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 279 -- func_ov002_020ce5f8, 0x020ce5f8, size 0x1a0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ce5f8
/* recovered: shared common types */
extern "C" {
int func_ov002_020ce5f8(char* c) {
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int func_ov002_020ceb54(void*);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const struct Vector3* pos);
    extern void func_ov002_020ce8bc(void*, int arg);
    extern void func_ov002_020ceb7c(void*);
    extern u32 _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, int b);
    extern void MulVec3Mat4x3(void*, void*, void*);
    extern void* Vec3_LslInPlace(void*, int sh);
    extern u32 func_020229f0(int x, int y, int z);
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, const struct Vector3* pos);
    extern int data_0209f32c;
    extern char data_ov002_0211013c;
    extern int func_ov002_020ce5f8(void*);
    int y, threshold;
    struct Vector3 pos;

    threshold = data_0209f32c - 0x50000;
    y = *(int*)(c + 0x60);
    if (y >= threshold) {
        if ((*(u8*)(c + 0x6e9) & 1) == 0) {
            if (*(int*)(c + 0xa8) >= 0) {
                *(int*)(c + 0x60) = threshold;
                *(int*)(c + 0xa8) = 0;
            }
        } else {
            if (threshold < y - 0xa000) {
                *(u8*)(c + 0x706) = 0;
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
                return 1;
            }
        }
    }

    if (func_ov002_020ceb54(c) == 0) {
        if (*(u8*)(c + 0x70c) == 0) {
            *(u8*)(c + 0x70c) |= 1;
            _ZN5Sound9PlayBank0EjRK7Vector3(0x17, (struct Vector3*)(c + 0x74));
            *(int*)(c + 0x628) = 0;
        }
        func_ov002_020ce8bc(c, *(int*)(c + 0x640));
    } else {
        *(u8*)(c + 0x70c) = 0;
    }
    func_ov002_020ceb7c(c);

    {
        u32 idx = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
        char* m1 = *(char**)(c + idx * 4 + 0xdc);
        char* base;
        u32 idx2;
        char* m2;
        base = *(char**)(m1 + 0x14) + 0x2d0;
        idx2 = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
        m2 = *(char**)(c + idx2 * 4 + 0xdc);
        MulVec3Mat4x3(base + 0x24, m2 + 0x1c, &pos);
    }
    Vec3_LslInPlace(&pos, 3);

    if (pos.y < data_0209f32c - 0x1e000) {
        func_020229f0(pos.x, *(volatile int*)&pos.y, pos.z);
        if (*(u16*)(c + 0x6a6) == 0) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0xb, (struct Vector3*)(c + 0x74));
            *(u16*)(c + 0x6a6) = 0xa;
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 280 -- func_ov002_020ce798, 0x020ce798, size 0x124 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ce798
/* the -= 0x4000 goes through a materialized base (add r1,c,#0x60; ldr/str [r1]);
   the (long long)-mask launder is what stops mwcc folding it to [c,#0x60]. */
#define LDR_I(p) (*(int*)(int)(p))
extern "C" {
void func_ov002_020ce798(char* c)
{
    typedef struct Vec3i { int x, y, z; } Vec3i;
    typedef struct dActor_c dActor_c;
    extern int data_0209f32c;
    extern char data_ov002_0211067c;
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(const void* self);
    extern void _ZN9dBgCh_LinC1Ev(void*);
    extern void _ZN9dBgCh_LinD1Ev(void*);
    extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void*, const Vec3i* a, const Vec3i* b, void*);
    extern int _ZN9dBgCh_Lin10DetectClsnEv(void*);
    extern void _ZN9dBgCh_Lin10GetClsnPosEv(void*, void*);
    char rl[0x78];
    Vec3i v1, v2;
    Vec3i cp;

    if (!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211067c)) return;

    if (*(int*)(c + 0x60) >= data_0209f32c - 0x50000) {
        if (!_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x380))
            *(int*)(c + 0x60) = data_0209f32c - 0x50000;
    }

    _ZN9dBgCh_LinC1Ev(rl);
    v1.x = *(int*)(c + 0x5c);
    v1.y = *(int*)(c + 0x60);
    v1.z = *(int*)(c + 0x64);
    v2.x = *(int*)(c + 0x5c);
    v2.y = *(int*)(c + 0x60);
    v2.z = *(int*)(c + 0x64);
    v1.y += 0x50000;
    v2.y += 0x96000;
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &v1, &v2, (dActor_c*)c);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl)) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&cp, rl);
        if (*(int*)(c + 0x60) >= cp.y - 0x82000) {
            LDR_I(c + 0x60) -= 0x4000;
            if (*(int*)(c + 0x60) < cp.y - 0x82000)
                *(int*)(c + 0x60) = cp.y - 0x82000;
        }
    }
    _ZN9dBgCh_LinD1Ev(rl);
}
}
#undef LDR_I

/* -------------------------------------------------------------------------- */
/* ROM ordinal 281 -- func_ov002_020ce8bc, 0x020ce8bc, size 0x10c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ce8bc
extern "C" {
void func_ov002_020ce8bc(char* self, int arg1)
{
    extern u32 func_02022cbc(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
    extern char* data_0209f32c;
    int z = *(int*)(self + 0x64);
    volatile int x = *(int*)(self + 0x5c);
    volatile int y = (int)(data_0209f32c + 0x3000);
    volatile int zshadow = z;

    if (*(unsigned char*)(self + 0x703) != 0) {
        if (arg1 != 0) {
            *(int*)(self + 0x634) = func_02022cbc(*(volatile int*)(self + 0x634), 0x65, x, y, z, 0);
        } else {
            *(int*)(self + 0x638) = func_02022cbc(*(volatile int*)(self + 0x638), 0x66, x, y, z, 0);
        }
    } else {
        if (arg1 != 0) {
            *(int*)(self + 0x634) = func_02022cbc(*(volatile int*)(self + 0x634), 0xe8, x, y, z, 0);
        } else {
            *(int*)(self + 0x638) = func_02022cbc(*(volatile int*)(self + 0x638), 0xe9, x, y, z, 0);
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 282 -- func_ov002_020ce9c8, 0x020ce9c8, size 0x12c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ce9c8
extern "C" {
void func_ov002_020ce9c8(char* self)
{
    extern u32 func_02022c80(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
    extern u32 func_02022cbc(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
    extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void*);
    extern void _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const void* v);
    extern char* data_0209f32c;
    int v[3];
    int effectID;

    *(volatile int*)&v[0] = *(int*)(self + 0x5c);
    *(volatile int*)&v[1] = *(int*)(self + 0x60);
    *(volatile int*)&v[2] = *(int*)(self + 0x64);
    v[1] = (int)(data_0209f32c + 0x4b000);
    effectID = 0xce;
    if (*(unsigned char*)(self + 0x703))
        effectID = 0x54;
    func_02022c80(0, effectID, v[0], v[1], v[2], 0);

    v[1] = (int)(data_0209f32c + 0x3000);
    effectID = 0xcf;
    if (*(unsigned char*)(self + 0x703))
        effectID = 0x55;
    func_02022cbc(0, effectID, v[0], v[1], v[2], 0);

    if (*(unsigned char*)(self + 0x703))
        *(int*)(self + 0x628) = func_02022d00(*(int*)(self + 0x628), 0x56, *(int*)(self + 0x5c), (int)data_0209f32c, *(int*)(self + 0x64), 0);
    else
        _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(*(int*)(self + 0x5c), (int)data_0209f32c, *(int*)(self + 0x64));

    if (*(unsigned char*)(self + 0x703))
        _ZN5Sound9PlayBank0EjRK7Vector3(0xd4, self + 0x74);
    else
        _ZN5Sound9PlayBank0EjRK7Vector3(0x16, self + 0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 283 -- func_ov002_020ceaf4, 0x020ceaf4, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ceaf4
extern "C" {
int func_ov002_020ceaf4(char* c){
    extern int data_0209f32c;
    extern int data_ov002_020ff1e0[];
  int v;
  if(data_0209f32c - 0x50000 - *(int*)(c+0x60) < 0x1f4000) return 0x1800;
  v = data_ov002_020ff1e0[*(int*)(c+8)];
  return -(int)((((long long)v << 13) + 0x800) >> 12);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 284 -- func_ov002_020ceb54, 0x020ceb54, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ceb54
extern "C" {
int func_ov002_020ceb54(char *p)
{
    extern int data_0209f32c;
    extern int func_ov002_020ceb54(void*);
    return data_0209f32c - 0x50000 >= *(int *)(p + 0x60) + 0x3c000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 285 -- func_ov002_020ceb7c, 0x020ceb7c, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020ceb7c
/* The 0x6e5 read-modify-write goes through a materialized base
   (ldr rN,[pc]; add rN,c,rN) because 0x6e5 is not an ARM rotated immediate;
   the (long long)-mask launder is what stops mwcc folding it to [c,#0x6e5]. */
#define LAUNDER_U8(p)      ((unsigned char *)(int)(p))
extern "C" {
void func_ov002_020ceb7c(struct CameraRaw *c)
{
    extern struct CameraRaw *data_0209f318;
    extern int data_0209f32c;
    extern void func_0200d768(void*, unsigned char playerID);
    extern void func_0200d72c(void*, unsigned char playerID);
    struct CameraRaw *g = data_0209f318;

    if (c->f6e5 & 2) {
        if (data_0209f32c - 0x50000 - c->f60 >= 0x64000)
            return;
        func_0200d768(g, c->f6d8);
        *LAUNDER_U8((char *)c + 0x6e5) ^= 2;
    } else {
        if (data_0209f32c - 0x50000 - c->f60 <= 0x12c000)
            return;
        func_0200d72c(g, c->f6d8);
        *LAUNDER_U8((char *)c + 0x6e5) |= 2;
    }
}
}
#undef LAUNDER_U8

/* -------------------------------------------------------------------------- */
/* ROM ordinal 286 -- func_ov002_020cec2c, 0x020cec2c, size 0x184 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cec2c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* func_ov002_020cec2c at 0x020cec2c (ov002), size 0x184
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
int func_ov002_020cec2c(char *self)
{
    extern void _ZN6Player4HealEi(void*, int a);
    extern int _ZN6Player9GetHealthEv(void*);
    extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, const struct Vector3 *v, unsigned int d);
    extern int func_ov002_020d91e0(void*, int damage, int doPre);
    extern void func_ov002_020da9d4(void*);
    extern int func_ov002_020c5dec(void*, int r1);
    extern s8 data_0209f2f8;
    extern int func_ov002_020ceb54(void*);
    int flag;
    int thresh;

    if (*(u8 *)(self + 0x709) != 0 || *(u8 *)(self + 0x6f9) != 0 || *(u8 *)(self + 0x703) != 0 || *(u8 *)(self + 0x708) != 0)
        return 0;

    flag = 0;
    if (data_0209f2f8 == 0x13 || data_0209f2f8 == 0x14 || data_0209f2f8 == 0x30)
        flag = 1;

    if (func_ov002_020ceb54(self) == 0 && flag == 0) {
        *(u16 *)(self + 0x6ca) = 0;
        if (*(u16 *)(self + 0x6b8) == 0) {
            _ZN6Player4HealEi(self, 0x100);
            *(u16 *)(self + 0x6b8) = 0xf;
        }
        return 0;
    }

    thresh = 0xf0;
    if (flag != 0) thresh = 0x50;

    if (_ZN6Player9GetHealthEv(self) <= 2) {
        *(unsigned int *)(self + 0x624) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            *(unsigned int *)(self + 0x624), 2, 0x27, (struct Vector3 *)(self + 0x74), 0);
    }

    {
        u16 *p = (u16 *)(self + 0x6ca);
        *p = *p + 1;
    }

    if (*(u16 *)(self + 0x6ca) < thresh) goto ret0;

    *(u16 *)(self + 0x6ca) = 0;
    if (func_ov002_020d91e0(self, 0x100, 0) == 0) goto ret0;

    func_ov002_020da9d4(self);
    func_ov002_020c5dec(self, 7);
    return 1;

ret0:
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 287 -- func_ov002_020cedb0, 0x020cedb0, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cedb0
extern "C" {
Fix12i func_ov002_020cedb0(void *c, Fix12i a)
{
    extern unsigned short data_ov002_020ff158[];
    extern int func_ov002_020cedb0(void*, int);
    unsigned int t = data_ov002_020ff158[*(int*)((char*)c + 8)];
    return (Fix12i)(((long long)a * (int)t + 0x800) >> 12);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 288 -- func_ov002_020cede0, 0x020cede0, size 0x1a4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cede0
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020cede0(void* thiz_) {
    extern int SurfaceInfo_TestFlag0x20(void*);
    extern int data_0209f32c;
    extern signed char data_0209f2f8;
    extern int func_ov002_020cede0(void*);
    extern int data_0209212c;
    char* thiz = (char*)thiz_;
  Vector3 v;
  int y = *(int*)(thiz + 0x60);
  int g = data_0209f32c;
  if (g < y - 0x64000)
    return 0;
  {
    int d = g - y;
    if (d < 0) d = -d;
    if (d >= 0xc8000)
      return 1;
  }
  dBgCh_Gnd rg;
  {
    int z = *(int*)(thiz + 0x64);
    int yy = data_0209f32c + 0xc8000;
    int x = *(int*)(thiz + 0x5c);
    v.x = x;
    v.y = yy;
    v.z = z;
  }
  rg.StartDetectingWater();
  rg.StopDetectingOrdinary();
  rg.SetObjAndPos(v, (dActor_c*)thiz);
  if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int*)&rg.surface) != 0) {
    return 1;
  }
  v.y = data_0209212c;
  rg.StartDetectingWater();
  rg.StopDetectingOrdinary();
  rg.SetObjAndPos(v, (dActor_c*)thiz);
  if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int*)&rg.surface) != 0) {
    *(int*)(thiz + 0x64c) = rg.clsnY;
    data_0209f32c = *(int*)(thiz + 0x64c);
    return 1;
  }
  if (data_0209f2f8 == 0x21) {
    *(int*)(thiz + 0x64c) = 0x80000000;
    data_0209f32c = *(int*)(thiz + 0x64c);
    return 0;
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 289 -- func_ov002_020cef84, 0x020cef84, size 0x288 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cef84
extern "C" {
int func_ov002_020cef84(char *self)
{
    typedef struct {
        void *tag;
        int f04, f08, f0c, f10, f14;
        unsigned short f18, f1a;
        int f1c, f20, f24;
    } ClsnResultTmp;
    extern int _ZN6Player7IsInAirEv(void*);
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern void _ZN9dBgCh_Lin10GetClsnPosEv(void*, void*);
    extern void func_02035414(void*);
    extern void func_02035428(void*);
    extern int func_02037e38(void*);
    extern unsigned _ZNK5dBgPi9GetClsnIDEv(void*);
    extern void *_ZN8dActor_c10FindWithIDEj(unsigned id);
    extern void _ZN5dBgPiD1Ev(void*);
    extern char data_ov002_0211001c;
    extern int data_02099368;
    Vector3 v1, v2, cp;
    ClsnResultTmp tmp;
    int lim;

    dBgCh_Lin line;
    if (!_ZN6Player7IsInAirEv(self) || *(unsigned char *)(self + 0x706) != 0 || *(int *)(self + 0xa8) <= 0 || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211001c) || *(unsigned char *)(self + 0x703) != 0 || *(unsigned char *)(self + 0x708) != 0 || *(unsigned char *)(self + 0x709) != 0) {
        return 0;
    }

    v1.x = *(int *)(self + 0x5c);
    int y1 = *(int *)(self + 0x60);
    v1.y = y1;
    v1.z = *(int *)(self + 0x64);
    v2.x = *(int *)(self + 0x5c);
    int y2 = *(int *)(self + 0x60);
    v2.y = y2;
    v2.z = *(int *)(self + 0x64);
    v1.y = y1 + 0x64000;
    v2.y = y2 + 0x96000;
    lim = 0x8c000;
    {
        int t = *(int *)(self + 0x358) != 0;
        if (t != false) {
            if (*(int *)(self + 8) == 2) {
                v2.y += 0x32000;
                lim = 0xb4000;
            }
        }
    }
    func_02035414(&line);
    if (*(unsigned char *)(self + 0x6fb) != 0)
        func_02035428(&line);
    line.SetObjAndLine(v1, v2, (dActor_c*)self);
    if (line.DetectClsn()) {
        _ZN9dBgCh_Lin10GetClsnPosEv(&cp, &line);
        if (func_02037e38((unsigned int *)&line.surface) == 3) {
            int t2 = *(int *)(self + 0x358) != 0;
            if (t2 == false) {
                _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211001c);
                *(int *)(self + 0x60) = cp.y - 0x78000;
                return 1;
            }
        }
        {
            int d = cp.y - *(int *)(self + 0x60);
            if (d < 0) d = -d;
            if (d < lim) {
                void *a;
                int *dst = &tmp.f04;
                int w0, w1;
                *(int *)(self + 0xa8) = -0x1000;
                *(unsigned char *)(self + 0x6e9) |= 8;
                w0 = *(int *)((char *)&line + 0x14);
                w1 = *(int *)((char *)&line + 0x18);
                dst[0] = w1 ? w0 : w0;
                dst[1] = w1;
                dst[2] = *(int *)((char *)&line + 0x1c);
                dst[3] = *(int *)((char *)&line + 0x20);
                dst[4] = *(int *)((char *)&line + 0x24);
                tmp.tag = &data_02099368;
                tmp.f18 = *(unsigned short *)((char *)&line + 0x28);
                tmp.f1a = *(unsigned short *)((char *)&line + 0x2a);
                tmp.f1c = *(int *)((char *)&line + 0x2c);
                tmp.f20 = *(int *)((char *)&line + 0x30);
                tmp.f24 = *(int *)((char *)&line + 0x34);
                a = _ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(&tmp));
                if (a)
                    (*(void (**)(void *, char *))(*(int *)a + 0x70))(a, self);
                _ZN5dBgPiD1Ev(&tmp);
            }
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 290 -- func_ov002_020cf20c, 0x020cf20c, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cf20c
/* recovered: shared common types */
// func_ov002_020cf20c at 0x020cf20c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020cf20c(char* c) {
    extern void _ZN9dBgCh_Lin10GetClsnPosEv(void*, void*);
    extern int func_02037e38(void*);
    extern int func_02037e48(void*);
    extern void func_0200ca14(void*, unsigned char b, int c);
    extern void* data_0209f318[];
    extern int func_ov002_020cf20c(void*);
    Vector3 v1;
    Vector3 v2;
    Vector3 clsnPos;
    dBgCh_Lin line;
    v1.x = *(int*)(c + 0x5c);
    int y1 = *(int*)(c + 0x60);
    v1.y = y1;
    v1.z = *(int*)(c + 0x64);
    v2.x = *(int*)(c + 0x5c);
    int y2 = *(int*)(c + 0x60);
    v2.y = y2;
    v2.z = *(int*)(c + 0x64);
    v1.y = y1 + 0x64000;
    v2.y = y2 + 0xb4000;
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        if (func_02037e38((unsigned int*)&line.surface) == 3) {
            _ZN9dBgCh_Lin10GetClsnPosEv(&clsnPos, &line);
            void* cam = data_0209f318[0];
            int t = func_02037e48((unsigned int*)&line.surface);
            func_0200ca14(cam, *(unsigned char*)(c + 0x6d8), t);
            int ret = clsnPos.y;
            return ret;
        }
    }
    return (int)0x80000000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 291 -- func_ov002_020cf2f8, 0x020cf2f8, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cf2f8
extern "C" {
void func_ov002_020cf2f8(void* c) {
    extern int _Z14ApproachLinearRiii(void*, int, int);
    extern int ApproachAngle(void*, int, int, int, int);
    extern int func_ov002_020bf224(void*, int, int);
    extern unsigned char data_020a0e40[];
    extern short data_0209f4a0[];
    extern void func_ov002_020cf2f8(void*);
  if (*(short*)((char*)data_0209f4a0 + data_020a0e40[0]*0x18) == 0) return;
  _Z14ApproachLinearRiii((char*)c+0x98, func_ov002_020bf224(c, 0x8000, 0x2000), 0x1000);
  ApproachAngle((char*)c+0x94, *(short*)((char*)c+0x6d2), 0x10, 0x1000, 0x200);
  *(short*)((char*)c+0x8e) = *(short*)((char*)c+0x94);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 292 -- func_ov002_020cf384, 0x020cf384, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cf384
extern "C" {
void func_ov002_020cf384(char *p)
{
    extern void func_ov002_020cf384(void*);
    *(int *)(p + 0x9c) = 0;
    *(int *)(p + 0xa8) = 0;
    *(int *)(p + 0x98) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 293 -- _ZN6Player20St_CeilingGrate_MainEv, 0x020cf398, size 0x324 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player20St_CeilingGrate_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_CeilingGrate_Main()
{
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int _ZN6Player12FinishedAnimEv(void*);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 anim, int a, int b, u32 d);
    extern int _ZNK6Player14GetBodyModelIDEjb(void*, u32 a, int b);
    extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void*);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_020a0e40;
    extern u16 data_0209f49c[];
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern int data_ov002_0211013c;
    extern int data_ov002_0211004c;
    extern int data_ov002_021105a4;
    extern int func_ov002_020cf20c(void*);
    extern int func_ov002_020d674c(void*);
    extern u8 data_0209f4ab[];
    extern void func_ov002_020cf384(void*);
    extern u32 data_ov002_0210a60c[];
    extern void func_ov002_020cf2f8(void*);
    int r4;
    int spd;

    r4 = func_ov002_020cf20c(((char*)this));
    if (r4 == 0x80000000) {
        mPosX = mPreClsnPosX;
        mPosY = mPreClsnPosY;
        mPosZ = mPreClsnPosZ;
        r4 = mPosY + 0x90000;
    }

    {
        int idx = data_020a0e40 * 0x18;
        if ((*(u16*)((char*)data_0209f49c + idx) & 2) == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211013c);
            return 1;
        }
        if (*(u16*)((char*)data_0209f49e + idx) & 0x400) {
            if (func_ov002_020d674c(((char*)this)) != 0)
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211004c);
            else
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_021105a4);
            return 1;
        }
        {
            u8* p = data_0209f4ab + idx;
            if (*p != 0)
                *p = 5;
        }
    }

    switch (mStateStep) {
    case 0:
        func_ov002_020cf384(((char*)this));
        if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
            mStateStep = 1;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a60c[mStateWork & 1], 0, 0x1000, 0);
        }
        {
            char* anim = (char*)(*(char**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0) << 2) + 0xdc) + 0x50);
            if ((u32)(*(int*)(anim + 8) << 4) >> 0x10 < 0x21)
                break;
        }
    case 1:
        func_ov002_020cf384(((char*)this));
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            mStateWork ^= 1;
            mStateStep = 2;
            spd = mHorzSpeed >> 3;
            if (spd < 0x800)
                spd = 0x800;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a60c[(mStateWork & 1) + 2], 0x40000000, spd, 0);
        }
        break;
    case 2:
        func_ov002_020cf2f8(((char*)this));
        spd = mHorzSpeed >> 3;
        if (spd < 0x800)
            spd = 0x800;
        {
            char* anim = (char*)(*(char**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0) << 2) + 0xdc) + 0x50);
            *(int*)(anim + 0xc) = spd;
        }
        if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0xb2, ((char*)this) + 0x74);
            if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
                mStateWork ^= 1;
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a60c[(mStateWork & 1) + 2], 0x40000000, 0x1000, 0);
            } else {
                mStateStep = 1;
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_0210a60c[mStateWork & 1], 0, 0x1000, 0);
            }
        }
        break;
    }

    mPosY = r4 - 0x90000;
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 294 -- _ZN6Player20St_CeilingGrate_InitEv, 0x020cf6bc, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player20St_CeilingGrate_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_CeilingGrate_Init()
{
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern void func_ov002_020cf384(void*);
  func_ov002_020cf384(((char*)this));
  mStateStep=0;
  mStateWork=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x5b,0x40000000,0x1000,0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 295 -- func_ov002_020cf700, 0x020cf700, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cf700
extern "C" {
int func_ov002_020cf700(void* player) {
    extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
    extern char data_ov002_0211001c[];
    extern int func_ov002_020cf700(void*);
    return _ZN6Player7IsStateERNS_5StateE(player, data_ov002_0211001c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 296 -- _ZN6Player14St_OnWall_MainEv, 0x020cf714, size 0x37c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player14St_OnWall_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_OnWall_Main()
{
    extern void func_ov002_020c0364(void*, u32 a);
    extern void func_ov002_020cabe0(void*);
    extern int func_ov002_020c5244();
    extern int func_ov002_020d36d8(void*, int a);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12i a, int b);
    extern void _Z14ApproachLinearRsss(void*, s16 t, s16 s);
    extern int func_ov002_020bf224(void*, int a, int b);
    extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, u32 anim, int a, Fix12i b, u32 d);
    extern int func_ov002_020d4d88(void*, int a, int b);
    extern void Player_AdvanceAnims(void*);
    extern u8 data_020a0e40;
    extern s16 data_0209f4a0[];
    extern int data_ov002_0211013c[];
    extern int AngleDiff(int, int);
    extern void _ZN6Player14St_OnWall_InitEv(void*);
    extern void func_ov002_020eee3c(void*, void*);
    u16 f = mStateFlags;
    u16 b0 = f & 1;
    if (b0 != 0) {
        func_ov002_020c0364(((char*)this), 3);
        return 1;
    }
    u16 b1 = f & 4;
    if (b1 != 0) {
        func_ov002_020cabe0(((char*)this));
        return 1;
    }
    if (func_ov002_020c5244() != 0)
        return 1;
    if (func_ov002_020d36d8(((char*)this), 0) != 0) {
        mPrevAngleY = mAngleY;
        return 1;
    }
    if ((mClsnFlags & 2) == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        return 1;
    }
    if (mStateStep == 2) {
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            return 1;
        }
        int a = _ZN4cstd5atan2E5Fix12IiES1_(mWallNormalX, mWallNormalZ) + 0x8000;
        int d = AngleDiff((s16)a, mDesiredAngleY);
        if (d >= 0x4000) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            return 1;
        }
        if (d >= 0x1555) {
            mStateStep = 0;
            if ((s16)a == (s16)(mDesiredAngleY + d))
                mStateStep = 1;
            _ZN6Player14St_OnWall_InitEv(((char*)this));
            return 1;
        }
        _Z14ApproachLinearRsss((s16*)((char*)&mAngleY), (s16)a, 0x800);
        mPrevAngleY = a;
        mHorzSpeed = func_ov002_020bf224(((char*)this), 0xa000, 0x2000);
        func_ov002_020eee3c(((char*)this) + 0x380, ((char*)this));
    } else {
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            int ang = _ZN4cstd5atan2E5Fix12IiES1_(mWallNormalX, mWallNormalZ);
            int d = AngleDiff((s16)(ang + 0x8000), mDesiredAngleY);
            if (d >= 0x4000) {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
                return 1;
            }
            if (d < 0x1555) {
                mStateStep = 2;
                _ZN6Player14St_OnWall_InitEv(((char*)this));
                return 1;
            }
            int a2 = ang + 0x8000;
            _Z14ApproachLinearRsss((s16*)((char*)&mAngleY), (s16)a2, 0x800);
            u32 anim;
            if ((s16)a2 == (s16)(mDesiredAngleY + d)) {
                mPrevAngleY = ang + 0x4000;
                anim = 0x5d;
            } else {
                mPrevAngleY = ang - 0x4000;
                anim = 0x5c;
            }
            Fix12i spd = (Fix12i)(((s64)mHorzSpeed * 0x600 + 0x800) >> 12);
            if (spd < 0x400)
                spd = 0x400;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), anim, 0, spd, 0);
            func_ov002_020d4d88(((char*)this), func_ov002_020bf224(((char*)this), 0x6000, 0x1000), 0x1000);
        } else {
            mHorzSpeed = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x5e, 0, 0x1000, 0);
        }
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 297 -- _ZN6Player14St_OnWall_InitEv, 0x020cfa90, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Player14St_OnWall_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {
int Player::St_OnWall_Init()
{
    extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
    extern int data_ov002_020ff164[];
  int r3=0x1000;
  if(param1==2){
    if(mStateStep==2) r3=0x2000;
  }
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_020ff164[mStateStep], 0, r3, 0);
  mHorzSpeed=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 298 -- func_ov002_020cfaf0, 0x020cfaf0, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cfaf0
extern "C" {
int func_ov002_020cfaf0(char* c) {
    extern short data_02082214[];
    extern int func_ov002_020cfaf0(void*);
    dBgCh_Lin line;
    Vector3 v1;
    Vector3 v2;
    {
        int ang = *(short*)(c + 0x8e);
        int j = 2 * (((unsigned short)(short)(ang + 0x8000)) >> 4);
        int s = data_02082214[j] * 32;
        int t = data_02082214[j + 1] * 32;
        int ax = *(int*)(c + 0x5c) + s;
        int az = *(int*)(c + 0x64) + t;
        int ay = *(int*)(c + 0x60) - 0x20000;
        v1.x = ax;
        v1.y = ay;
        v1.z = az;
        int bx = *(int*)(c + 0x5c) + s;
        int bz = *(int*)(c + 0x64) + t;
        int by = *(int*)(c + 0x60) + 0x20000;
        v2.x = bx;
        v2.y = by;
        v2.z = bz;
    }
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        return 1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 299 -- func_ov002_020cfbdc, 0x020cfbdc, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cfbdc
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {
int func_ov002_020cfbdc(char *self)
{
    extern void func_ov002_020d0948(void*);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
    extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void*);
    extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
    extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
    extern short data_02082214[];
    extern int data_ov002_021101b4;
    extern int func_ov002_020cfbdc(void*);
    extern int _ZNK5dBgPi9GetClsnIDEv(void*);
    Vector3 pts[3];
    int idx = (unsigned short)(short)(*(short *)(self + 0x8e) + 0x8000) >> 4;
    int r6 = data_02082214[idx * 2];
    int r5 = data_02082214[idx * 2 + 1];

    if (*(unsigned short *)(self + 0x6a6) == 0) {
        if ((*(unsigned char *)(self + 0x6e9) & 2) != 0) {
            if (_ZNK10dBgCh_Actr10IsOnGroundEv(self + 0x380) != 0) {
                void *fr = _ZNK10dBgCh_Actr14GetFloorResultEv(self + 0x380);
                if (_ZNK5dBgPi9GetClsnIDEv(fr) != -1) {
                    if (_ZN8dActor_c10FindWithIDEj((unsigned int)_ZNK5dBgPi9GetClsnIDEv(fr)) != 0) {
                        func_ov002_020d0948(self);
                        return 1;
                    }
                }
            }
        }
    }

    {
        int az = *(int *)(self + 0x64) + (r5 << 5);
        int ay = *(int *)(self + 0x60) - 0xa000;
        int ax = *(int *)(self + 0x5c) + (r6 << 5);
        pts[1].x = ax;
        pts[1].y = ay;
        pts[1].z = az;
    }
    {
        int bx = *(int *)(self + 0x5c) + r6 * 0x60;
        int bz = *(int *)(self + 0x64) + r5 * 0x60;
        int by = *(int *)(self + 0x60) - 0xa000;
        pts[2].x = bx;
        pts[2].y = by;
        pts[2].z = bz;
    }

    dBgCh_Lin line;
    line.SetObjAndLine(pts[1], pts[2], (dActor_c*)self);
    if (line.DetectClsn()) {
        *(int *)((int)(self + 0x5c)) += r5 * 0x60;
        *(int *)((int)(self + 0x60)) -= 0x80000;
        *(int *)((int)(self + 0x64)) += r5 * 0x60;
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021101b4);
        return 1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 300 -- func_ov002_020cfd84, 0x020cfd84, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020cfd84
// func_ov002_020cfd84 at 0x020cfd84
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020cfd84(void* actor, void* a, void* b) {
    extern int _ZNK5dBgPi9GetClsnIDEv(void*);
    extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
    dBgCh_Lin line;
    dBgPi result;
    line.SetObjAndLine(*(Vector3*)a, *(Vector3*)b, (dActor_c*)actor);
    if (line.DetectClsn()) {
        result.surface = line.surface;
        result.triangleID = line.triangleID;
        result.flags = line.flags;
        result.clsnID = line.clsnID;
        result.unk_020 = line.unk_020;
        result.unk_024 = line.unk_024;
        if (_ZNK5dBgPi9GetClsnIDEv(&result) != -1) {
            void* act = _ZN8dActor_c10FindWithIDEj((unsigned int)_ZNK5dBgPi9GetClsnIDEv(&result));
            if (act != 0) {
                int t = (*(unsigned short*)((char*)act + 0xc) == 0x12a);
                if (t == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
}

/* Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020bd828  src/_ZN6Player8CanPauseEv.cpp
 *   [1] 0x020bd8ac  src/func_ov002_020bd8ac.c
 *   [2] 0x020bd8c0  src/func_ov002_020bd8c0.cpp
 *   [3] 0x020bd928  src/func_ov002_020bd928.cpp
 *   [4] 0x020bd984  src/func_ov002_020bd984.cpp
 *   [5] 0x020bd9ec  src/func_ov002_020bd9ec.cpp
 *   [6] 0x020bda48  src/func_ov002_020bda48.c
 *   [7] 0x020bda98  src/_ZN6Player8HasNoCapEv.cpp
 *   [8] 0x020bdb50  src/func_ov002_020bdb50.cpp
 *   [9] 0x020bdc18  src/func_ov002_020bdc18.c
 *   [10] 0x020bdc58  src/_ZN6Player16IncMegaKillCountEv.cpp
 *   [11] 0x020bdd2c  src/func_ov002_020bdd2c.c
 *   [12] 0x020bdd9c  src/func_ov002_020bdd9c.cpp
 *   [13] 0x020bde14  src/_ZN6Player14InitMetalWarioEv.cpp
 *   [14] 0x020bdef0  src/func_ov002_020bdef0.cpp
 *   [15] 0x020bdf4c  src/_ZN6Player15InitVanishLuigiEv.cpp
 *   [16] 0x020bdf8c  src/func_ov002_020bdf8c.c
 *   [17] 0x020be008  src/func_ov002_020be008.cpp
 *   [18] 0x020be0f8  src/_ZN6Player18SetNewHatCharacterEjjb.cpp
 *   [19] 0x020be1e0  src/_ZN6Player16SetRealCharacterEj.cpp
 *   [20] 0x020be324  src/_ZN6Player18TurnOffToonShadingEj.cpp
 *   [21] 0x020be3b0  src/func_ov002_020be3b0.c
 *   [22] 0x020bea7c  src/func_ov002_020bea7c.c
 *   [23] 0x020bea94  src/_ZN6Player15IsCollectingCapEv.cpp
 *   [24] 0x020beabc  src/func_ov002_020beabc.cpp
 *   [25] 0x020beb38  src/func_ov002_020beb38.c
 *   [26] 0x020bebd4  src/func_ov002_020bebd4.cpp
 *   [27] 0x020bec2c  src/func_ov002_020bec2c.cpp
 *   [28] 0x020bec84  src/func_ov002_020bec84.c
 *   [29] 0x020bec9c  src/func_ov002_020bec9c.cpp
 *   [30] 0x020becc8  src/_ZNK6Player14GetBodyModelIDEjb.cpp
 *   [31] 0x020becf4  src/func_ov002_020becf4.cpp
 *   [32] 0x020bed98  src/_ZN6Player12FinishedAnimEv.cpp
 *   [33] 0x020bedd4  src/Player_AdvanceAnims.cpp
 *   [34] 0x020beecc  src/_ZN6Player6IsAnimEj.cpp
 *   [35] 0x020bef2c  src/_ZN6Player7SetAnimEji5Fix12IiEj.cpp
 *   [36] 0x020bf13c  src/func_ov002_020bf13c.c
 *   [37] 0x020bf224  src/func_ov002_020bf224.c
 *   [38] 0x020bf27c  src/func_ov002_020bf27c.c
 *   [39] 0x020bf2d8  src/func_ov002_020bf2d8.c
 *   [40] 0x020bf30c  src/Player_ScaleByCharFactor.c
 *   [41] 0x020bf340  src/func_ov002_020bf340.c
 *   [42] 0x020bf36c  src/func_ov002_020bf36c.cpp
 *   [43] 0x020bf40c  src/func_ov002_020bf40c.c
 *   [44] 0x020bf4e4  src/_ZN6Player4HealEi.cpp
 *   [45] 0x020bf548  src/_ZN6Player9GetHealthEv.cpp
 *   [46] 0x020bf56c  src/func_ov002_020bf56c.cpp
 *   [47] 0x020bf5e0  src/func_ov002_020bf5e0.c
 *   [48] 0x020bf800  src/func_ov002_020bf800.cpp
 *   [49] 0x020bf88c  src/func_ov002_020bf88c.c
 *   [50] 0x020bf90c  src/func_ov002_020bf90c.c
 *   [51] 0x020bf9d4  src/func_ov002_020bf9d4.cpp
 *   [52] 0x020bfa74  src/func_ov002_020bfa74.cpp
 *   [53] 0x020bfec0  src/func_ov002_020bfec0.c
 *   [54] 0x020c0108  src/func_ov002_020c0108.c
 *   [55] 0x020c031c  src/func_ov002_020c031c.cpp
 *   [56] 0x020c0364  src/func_ov002_020c0364.c
 *   [57] 0x020c0434  src/func_ov002_020c0434.c
 *   [58] 0x020c04ac  src/func_ov002_020c04ac.c
 *   [59] 0x020c04e0  src/func_ov002_020c04e0.c
 *   [60] 0x020c0688  src/func_ov002_020c0688.cpp
 *   [61] 0x020c06fc  src/func_ov002_020c06fc.c
 *   [62] 0x020c0cbc  src/func_ov002_020c0cbc.c
 *   [63] 0x020c0d90  src/func_ov002_020c0d90.c
 *   [64] 0x020c0fb4  src/func_ov002_020c0fb4.c
 *   [65] 0x020c1234  src/func_ov002_020c1234.cpp
 *   [66] 0x020c133c  src/func_ov002_020c133c.c
 *   [67] 0x020c14b8  src/func_ov002_020c14b8.c
 *   [68] 0x020c16ec  src/func_ov002_020c16ec.c
 *   [69] 0x020c179c  src/func_ov002_020c179c.cpp
 *   [70] 0x020c18b0  src/func_ov002_020c18b0.c
 *   [71] 0x020c19d0  src/func_ov002_020c19d0.cpp
 *   [72] 0x020c1ad8  src/func_ov002_020c1ad8.c
 *   [73] 0x020c1c84  src/func_ov002_020c1c84.c
 *   [74] 0x020c1e44  src/func_ov002_020c1e44.c
 *   [75] 0x020c1eb4  src/func_ov002_020c1eb4.cpp
 *   [76] 0x020c200c  src/func_ov002_020c200c.c
 *   [77] 0x020c2138  src/func_ov002_020c2138.cpp
 *   [78] 0x020c2270  src/func_ov002_020c2270.cpp
 *   [79] 0x020c231c  src/func_ov002_020c231c.c
 *   [80] 0x020c25a8  src/func_ov002_020c25a8.c
 *   [81] 0x020c29d4  src/func_ov002_020c29d4.cpp
 *   [82] 0x020c2b08  src/func_ov002_020c2b08.c
 *   [83] 0x020c2db8  src/func_ov002_020c2db8.c
 *   [84] 0x020c2e78  src/func_ov002_020c2e78.cpp
 *   [85] 0x020c2ef0  src/_ZN6Player7CanWarpEv.cpp
 *   [86] 0x020c2f3c  src/_ZN6Player7IsInAirEv.cpp
 *   [87] 0x020c2f64  src/func_ov002_020c2f64.c
 *   [88] 0x020c2fec  src/func_ov002_020c2fec.c
 *   [89] 0x020c3148  src/func_ov002_020c3148.c
 *   [90] 0x020c3160  src/func_ov002_020c3160.c
 *   [91] 0x020c37a4  src/func_ov002_020c37a4.cpp
 *   [92] 0x020c38a0  src/func_ov002_020c38a0.c
 *   [93] 0x020c3a48  src/func_ov002_020c3a48.c
 *   [94] 0x020c3bdc  src/func_ov002_020c3bdc.c
 *   [95] 0x020c3cf0  src/func_ov002_020c3cf0.c
 *   [96] 0x020c3d1c  src/_ZN6Player17St_EndingFly_MainEv.cpp
 *   [97] 0x020c3d6c  src/_ZN6Player17St_EndingFly_InitEv.cpp
 *   [98] 0x020c3dbc  src/func_ov002_020c3dbc.c
 *   [99] 0x020c3dd0  src/_ZN6Player21St_OpeningWakeUp_MainEv.cpp
 *   [100] 0x020c3e8c  src/func_ov002_020c3e8c.c
 *   [101] 0x020c3ea0  src/func_ov002_020c3ea0.c
 *   [102] 0x020c3ed8  src/_ZN6Player21St_OpeningWakeUp_InitEv.cpp
 *   [103] 0x020c3f18  src/func_ov002_020c3f18.c
 *   [104] 0x020c3f2c  src/func_ov002_020c3f2c.c
 *   [105] 0x020c3f40  src/_ZN6Player15St_Respawn_MainEv.cpp
 *   [106] 0x020c4088  src/_ZN6Player15St_Respawn_InitEv.cpp
 *   [107] 0x020c4188  src/func_ov002_020c4188.c
 *   [108] 0x020c43c4  src/func_ov002_020c43c4.c
 *   [109] 0x020c44c4  src/func_ov002_020c44c4.c
 *   [110] 0x020c4760  src/_ZN6Player15St_Talk_CleanupEv.cpp
 *   [111] 0x020c47f4  src/func_ov002_020c47f4.cpp
 *   [112] 0x020c48e0  src/_ZN6Player12St_Talk_MainEv.cpp
 *   [113] 0x020c4bd8  src/_ZN6Player12St_Talk_InitEv.cpp
 *   [114] 0x020c4c60  src/_ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh.cpp
 *   [115] 0x020c4ec0  src/_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh.cpp
 *   [116] 0x020c4efc  src/_ZN6Player18HasFinishedTalkingEv.cpp
 *   [117] 0x020c4f40  src/_ZN6Player12Unk_020c4f40Et.cpp
 *   [118] 0x020c4fa0  src/_ZN6Player9StartTalkER7fBase_cb.cpp
 *   [119] 0x020c51d0  src/func_ov002_020c51d0.cpp
 *   [120] 0x020c5244  src/func_ov002_020c5244.c
 *   [121] 0x020c524c  src/_ZN6Player12GetTalkStateEv.cpp
 *   [122] 0x020c52bc  src/_ZN6Player21St_StuckInGround_MainEv.cpp
 *   [123] 0x020c5444  src/func_ov002_020c5444.cpp
 *   [124] 0x020c54e8  src/_ZN6Player21St_StuckInGround_InitEv.cpp
 *   [125] 0x020c5560  src/_ZN6Player24St_BowserEarthquake_MainEv.cpp
 *   [126] 0x020c568c  src/_ZN6Player24St_BowserEarthquake_InitEv.cpp
 *   [127] 0x020c56f0  src/func_ov002_020c56f0.c
 *   [128] 0x020c5780  src/_ZN6Player15St_DeadPit_MainEv.cpp
 *   [129] 0x020c59c0  src/_ZN6Player15St_DeadPit_InitEv.cpp
 *   [130] 0x020c5cc8  src/_ZN6Player14EnterWhirlpoolEv.cpp
 *   [131] 0x020c5cd8  src/func_ov002_020c5cd8.c
 *   [132] 0x020c5d0c  src/func_ov002_020c5d0c.c
 *   [133] 0x020c5d60  src/func_ov002_020c5d60.c
 *   [134] 0x020c5dec  src/func_ov002_020c5dec.cpp
 *   [135] 0x020c5e34  src/_ZN6Player15St_DeadHit_MainEv.cpp
 *   [136] 0x020c6010  src/_ZN6Player15St_DeadHit_InitEv.cpp
 *   [137] 0x020c607c  src/func_ov002_020c607c.cpp
 *   [138] 0x020c61ac  src/func_ov002_020c61ac.c
 *   [139] 0x020c647c  src/func_ov002_020c647c.cpp
 *   [140] 0x020c6538  src/func_ov002_020c6538.cpp
 *   [141] 0x020c65f8  src/_ZN6Player17St_Squish_CleanupEv.cpp
 *   [142] 0x020c662c  src/_ZN6Player14St_Squish_MainEv.cpp
 *   [143] 0x020c6908  src/func_ov002_020c6908.cpp
 *   [144] 0x020c69a0  src/_ZN6Player14St_Squish_InitEv.cpp
 *   [145] 0x020c6a10  src/_ZN6Player12Unk_020c6a10Ej.cpp
 *   [146] 0x020c6adc  src/func_ov002_020c6adc.cpp
 *   [147] 0x020c6b3c  src/_ZN6Player16St_Teleport_MainEv.cpp
 *   [148] 0x020c6dc4  src/_ZN6Player16St_Teleport_InitEv.cpp
 *   [149] 0x020c6e14  src/func_ov002_020c6e14.c
 *   [150] 0x020c6f34  src/_ZN6Player21St_LevelEnter_CleanupEv.cpp
 *   [151] 0x020c6f3c  src/_ZN6Player18St_LevelEnter_MainEv.cpp
 *   [152] 0x020c6fe4  src/func_ov002_020c6fe4.c
 *   [153] 0x020c70ac  src/func_ov002_020c70ac.c
 *   [154] 0x020c7194  src/func_ov002_020c7194.cpp
 *   [155] 0x020c71e0  src/func_ov002_020c71e0.cpp
 *   [156] 0x020c72a4  src/func_ov002_020c72a4.cpp
 *   [157] 0x020c7350  src/func_ov002_020c7350.cpp
 *   [158] 0x020c75f0  src/func_ov002_020c75f0.c
 *   [159] 0x020c7838  src/_ZN6Player18St_LevelEnter_InitEv.cpp
 *   [160] 0x020c7cbc  src/func_ov002_020c7cbc.c
 *   [161] 0x020c7dd0  src/func_ov002_020c7dd0.cpp
 *   [162] 0x020c7e84  src/_ZN6Player15IsEnteringLevelEv.cpp
 *   [163] 0x020c7ed4  src/_ZN6Player20IsStateEnteringLevelEv.cpp
 *   [164] 0x020c7f10  src/func_ov002_020c7f10.cpp
 *   [165] 0x020c7ff8  src/func_ov002_020c7ff8.c
 *   [166] 0x020c8128  src/_ZN6Player20St_NoControl_CleanupEv.cpp
 *   [167] 0x020c816c  src/_ZN6Player17St_NoControl_MainEv.cpp
 *   [168] 0x020c84b0  src/func_ov002_020c84b0.c
 *   [169] 0x020c8540  src/func_ov002_020c8540.c
 *   [170] 0x020c8714  src/func_ov002_020c8714.c
 *   [171] 0x020c897c  src/func_ov002_020c897c.c
 *   [172] 0x020c8a4c  src/func_ov002_020c8a4c.cpp
 *   [173] 0x020c8b54  src/func_ov002_020c8b54.c
 *   [174] 0x020c8b78  src/func_ov002_020c8b78.cpp
 *   [175] 0x020c8cb0  src/func_ov002_020c8cb0.cpp
 *   [176] 0x020c8d14  src/func_ov002_020c8d14.c
 *   [177] 0x020c8f0c  src/func_ov002_020c8f0c.cpp
 *   [178] 0x020c8f80  src/func_ov002_020c8f80.cpp
 *   [179] 0x020c904c  src/func_ov002_020c904c.c
 *   [180] 0x020c9128  src/func_ov002_020c9128.cpp
 *   [181] 0x020c91bc  src/func_ov002_020c91bc.c
 *   [182] 0x020c924c  src/func_ov002_020c924c.c
 *   [183] 0x020c9288  src/func_ov002_020c9288.cpp
 *   [184] 0x020c92fc  src/func_ov002_020c92fc.c
 *   [185] 0x020c94a4  src/func_ov002_020c94a4.c
 *   [186] 0x020c965c  src/func_ov002_020c965c.c
 *   [187] 0x020c9718  src/func_ov002_020c9718.c
 *   [188] 0x020c976c  src/_ZN6Player17St_NoControl_InitEv.cpp
 *   [189] 0x020c97e0  src/func_ov002_020c97e0.c
 *   [190] 0x020c97f8  src/func_ov002_020c97f8.cpp
 *   [191] 0x020c9840  src/func_ov002_020c9840.cpp
 *   [192] 0x020c98a4  src/func_ov002_020c98a4.cpp
 *   [193] 0x020c990c  src/func_ov002_020c990c.c
 *   [194] 0x020c9998  src/func_ov002_020c9998.c
 *   [195] 0x020c9a04  src/func_ov002_020c9a04.c
 *   [196] 0x020c9ac0  src/func_ov002_020c9ac0.cpp
 *   [197] 0x020c9b10  src/func_ov002_020c9b10.c
 *   [198] 0x020c9b5c  src/func_ov002_020c9b5c.c
 *   [199] 0x020c9b7c  src/func_ov002_020c9b7c.c
 *   [200] 0x020c9c00  src/func_ov002_020c9c00.c
 *   [201] 0x020c9c4c  src/func_ov002_020c9c4c.c
 *   [202] 0x020c9cc0  src/func_ov002_020c9cc0.c
 *   [203] 0x020c9d1c  src/func_ov002_020c9d1c.c
 *   [204] 0x020c9d68  src/func_ov002_020c9d68.cpp
 *   [205] 0x020c9de4  src/func_ov002_020c9de4.c
 *   [206] 0x020c9e18  src/func_ov002_020c9e18.c
 *   [207] 0x020c9e40  src/Player_DisableInteraction.c
 *   [208] 0x020c9e5c  src/_ZN6Player12Unk_020c9e5cEh.cpp
 *   [209] 0x020ca0f4  src/func_ov002_020ca0f4.c
 *   [210] 0x020ca108  src/func_ov002_020ca108.cpp
 *   [211] 0x020ca144  src/_ZN6Player11OpenBigDoorEv.cpp
 *   [212] 0x020ca150  src/_ZN6Player12Unk_020ca150Eh.cpp
 *   [213] 0x020ca1b8  src/_ZN6Player17SetNoControlStateEhih.cpp
 *   [214] 0x020ca270  src/func_ov002_020ca270.cpp
 *   [215] 0x020ca2ac  src/_ZN6Player15JumpIntoBooCageER7Vector3.cpp
 *   [216] 0x020ca344  src/_ZN6Player13TryTalkToDoorEh.cpp
 *   [217] 0x020ca3d0  src/_ZN6Player21IsOpeningDoorWithStarEv.cpp
 *   [218] 0x020ca410  src/_ZN6Player16TryTalkToKeyDoorEv.cpp
 *   [219] 0x020ca488  src/_ZN6Player12Unk_020ca488Ev.cpp
 *   [220] 0x020ca498  src/_ZN6Player16TryEnterStarDoorER7Vector3s.cpp
 *   [221] 0x020ca5cc  src/_ZN6Player12CanEnterDoorEh.cpp
 *   [222] 0x020ca708  src/_ZN6Player17PlayMammaMiaSoundEv.cpp
 *   [223] 0x020ca724  src/_ZN6Player24TryExitWhiteDoorWithStarEv.cpp
 *   [224] 0x020ca78c  src/func_ov002_020ca78c.c
 *   [225] 0x020ca7f4  src/_ZN6Player16St_DebugFly_MainEv.cpp
 *   [226] 0x020ca8d0  src/_ZN6Player16St_DebugFly_InitEv.cpp
 *   [227] 0x020ca8f8  src/_ZN6Player12Unk_020ca8f8Ev.cpp
 *   [228] 0x020ca940  src/func_ov002_020ca940.c
 *   [229] 0x020cabe0  src/func_ov002_020cabe0.cpp
 *   [230] 0x020cac20  src/_ZN6Player15St_Null_CleanupEv.cpp
 *   [231] 0x020cac28  src/_ZN6Player12St_Null_MainEv.cpp
 *   [232] 0x020cac30  src/_ZN6Player12St_Null_InitEv.cpp
 *   [233] 0x020cac38  src/_ZN6Player21St_CameraZoom_CleanupEv.cpp
 *   [234] 0x020cac60  src/func_ov002_020cac60.c
 *   [235] 0x020cad10  src/_ZN6Player18St_CameraZoom_MainEv.cpp
 *   [236] 0x020cae10  src/func_ov002_020cae10.c
 *   [237] 0x020caef4  src/_ZN6Player18St_CameraZoom_InitEv.cpp
 *   [238] 0x020caf68  src/func_ov002_020caf68.c
 *   [239] 0x020caf98  src/func_ov002_020caf98.cpp
 *   [240] 0x020cb15c  src/_ZN6Player17St_Headstand_MainEv.cpp
 *   [241] 0x020cb2f0  src/_ZN6Player17St_Headstand_InitEv.cpp
 *   [242] 0x020cb354  src/func_ov002_020cb354.cpp
 *   [243] 0x020cb400  src/func_ov002_020cb400.cpp
 *   [244] 0x020cb474  src/func_ov002_020cb474.cpp
 *   [245] 0x020cb568  src/_ZN6Player16St_Climb_CleanupEv.cpp
 *   [246] 0x020cb5bc  src/_ZN6Player13St_Climb_MainEv.cpp
 *   [247] 0x020cbd34  src/_ZN6Player13St_Climb_InitEv.cpp
 *   [248] 0x020cbea8  src/func_ov002_020cbea8.c
 *   [249] 0x020cc01c  src/func_ov002_020cc01c.cpp
 *   [250] 0x020cc05c  src/func_ov002_020cc05c.c
 *   [251] 0x020cc140  src/_ZN6Player9IsOnShellEv.cpp
 *   [252] 0x020cc16c  src/func_ov002_020cc16c.c
 *   [253] 0x020cc1f4  src/func_ov002_020cc1f4.c
 *   [254] 0x020cc228  src/_ZN6Player16St_Shell_CleanupEv.cpp
 *   [255] 0x020cc27c  src/_ZN6Player13St_Shell_MainEv.cpp
 *   [256] 0x020cc660  src/func_ov002_020cc660.c
 *   [257] 0x020cc780  src/_ZN6Player13St_Shell_InitEv.cpp
 *   [258] 0x020cc7d0  src/_ZN6Player17St_HurtWater_MainEv.cpp
 *   [259] 0x020cc910  src/_ZN6Player17St_HurtWater_InitEv.cpp
 *   [260] 0x020cc9c0  src/_ZN6Player23St_MetalWaterWater_MainEv.cpp
 *   [261] 0x020ccc3c  src/_ZN6Player23St_MetalWaterWater_InitEv.cpp
 *   [262] 0x020ccccc  src/_ZN6Player24St_MetalWaterGround_MainEv.cpp
 *   [263] 0x020cd0d0  src/_ZN6Player24St_MetalWaterGround_InitEv.cpp
 *   [264] 0x020cd190  src/func_ov002_020cd190.c
 *   [265] 0x020cd1e4  src/_ZN6Player15St_Swim_CleanupEv.cpp
 *   [266] 0x020cd218  src/func_ov002_020cd218.cpp
 *   [267] 0x020cd28c  src/func_ov002_020cd28c.c
 *   [268] 0x020cd2c4  src/func_ov002_020cd2c4.c
 *   [269] 0x020cd308  src/func_ov002_020cd308.c
 *   [270] 0x020cd364  src/func_ov002_020cd364.c
 *   [271] 0x020cd39c  src/func_ov002_020cd39c.c
 *   [272] 0x020cd3e0  src/func_ov002_020cd3e0.cpp
 *   [273] 0x020cd448  src/func_ov002_020cd448.cpp
 *   [274] 0x020cd550  src/func_ov002_020cd550.c
 *   [275] 0x020cd71c  src/func_ov002_020cd71c.c
 *   [276] 0x020cd94c  src/_ZN6Player12St_Swim_MainEv.cpp
 *   [277] 0x020ce324  src/func_ov002_020ce324.c
 *   [278] 0x020ce550  src/_ZN6Player12St_Swim_InitEv.cpp
 *   [279] 0x020ce5f8  src/func_ov002_020ce5f8.c
 *   [280] 0x020ce798  src/func_ov002_020ce798.c
 *   [281] 0x020ce8bc  src/func_ov002_020ce8bc.c
 *   [282] 0x020ce9c8  src/func_ov002_020ce9c8.c
 *   [283] 0x020ceaf4  src/func_ov002_020ceaf4.c
 *   [284] 0x020ceb54  src/func_ov002_020ceb54.c
 *   [285] 0x020ceb7c  src/func_ov002_020ceb7c.c
 *   [286] 0x020cec2c  src/func_ov002_020cec2c.c
 *   [287] 0x020cedb0  src/func_ov002_020cedb0.c
 *   [288] 0x020cede0  src/func_ov002_020cede0.cpp
 *   [289] 0x020cef84  src/func_ov002_020cef84.cpp
 *   [290] 0x020cf20c  src/func_ov002_020cf20c.cpp
 *   [291] 0x020cf2f8  src/func_ov002_020cf2f8.c
 *   [292] 0x020cf384  src/func_ov002_020cf384.c
 *   [293] 0x020cf398  src/_ZN6Player20St_CeilingGrate_MainEv.cpp
 *   [294] 0x020cf6bc  src/_ZN6Player20St_CeilingGrate_InitEv.cpp
 *   [295] 0x020cf700  src/func_ov002_020cf700.c
 *   [296] 0x020cf714  src/_ZN6Player14St_OnWall_MainEv.cpp
 *   [297] 0x020cfa90  src/_ZN6Player14St_OnWall_InitEv.cpp
 *   [298] 0x020cfaf0  src/func_ov002_020cfaf0.cpp
 *   [299] 0x020cfbdc  src/func_ov002_020cfbdc.cpp
 *   [300] 0x020cfd84  src/func_ov002_020cfd84.cpp
 */
