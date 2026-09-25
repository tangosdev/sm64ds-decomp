//cpp
/**
 * Shared base for enemies and other actors that use the enemy collision,
 * death and Yoshi-eat handling.
 *
 * IsGoingOffCliff probes straight down from the actor and stores the
 * result in mCliffState. AngleAwayFromWallOrCliff turns the actor when
 * that probe says the floor is gone or too steep.
 *
 * The constructor and destructor bodies are empty. The compiler supplies
 * the base call and the vptr store (see the constructor note). Source
 * order is the reverse of the ROM because codegen is deferred.
 * decl_common.h is left out: its helper spellings disagree with this file.
 *
 * The out-of-line destructor is the class's key function. It emits D2, D0
 * and D1 in cartridge order (0x020aed18, 0x020aed3c, 0x020aed74), so this
 * object carries _ZTV/_ZTI/_ZTS12dEnemyBase_c and the inherited RTTI. The
 * manifest's compiler_only_output licenses them, and the promotion claims
 * no data.
 *
 * The name is the cartridge's: ov002 holds _ZTI12dEnemyBase_c/
 * _ZTS12dEnemyBase_c at 0x021081c0/0x021081cc, and da1up_c's
 * __si_class_type_info reaches them through its +8 word.
 */
#include "dEnemyBase_c.h"
#include "decl_dBgPi.h"
#include "dBgCh_Lin.h"

/* Written as the real constructor, which is the form the legacy shard recovered
   and byte-matched. Two steps, and neither is spelled below: the dActor_c
   base-subobject call, then this class's own vptr store. `dEnemyBase_c : dActor_c`
   supplies the first and `virtual ~dEnemyBase_c()` the second, in the order mwcc
   always uses (notes/ctor-migration.md sec 6: base calls -> derived vptr store ->
   members -> body).

   The base call is a real `bl` only because include/dActor_c.h DECLARES dActor_c()
   and never defines it; left undeclared the compiler would synthesise and inline
   the base construction here and the candidate would land short. That is
   notes/ctor-migration.md sec 2, and it is why dEnemyBase_c.h declares
   dEnemyBase_c() the same way for the classes below it.

   One definition emits C1 AND C2 (sec 1). There is no C1 file and no C1 in the ROM
   -- nothing constructs a standalone dEnemyBase_c, so the linker never kept one;
   the manifest licenses the C1 this TU emits as compiler-only output. */
// @symbol _ZN12dEnemyBase_cC2Ev
dEnemyBase_c::dEnemyBase_c()
{
}

/* One definition, three emitted sections: D2 0x020aed18, D0 0x020aed3c, D1 0x020aed74. */
// @symbol _ZN12dEnemyBase_cD2Ev
// @symbol _ZN12dEnemyBase_cD0Ev
// @symbol _ZN12dEnemyBase_cD1Ev
/* Store this class's vtable over the one dActor_c's constructor left, then run the
   dActor_c subobject destructor. D0 additionally returns the object to the actor
   heap through dEnemyBase_c's own inline operator delete (see include/dEnemyBase_c.h). The
   three legacy files each spelled one variant; the compiler emits all three from
   this. */
dEnemyBase_c::~dEnemyBase_c()
{
}

/* Runs the mesh collision (sel picks which of four update flavours), then caches
   whichever surface normals came back: the floor result's into mFloorNormal*,
   the wall result's into mWallNormal*. Those two CopyNormalTo calls are what
   evidence both triples as Vector3s.

   On a floor that does not limit movement, the vertical velocity is re-derived
   so the motion stays in the floor plane. */
// @symbol _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj
extern "C" {
extern void func_020383f0(dBgCh_Actr *);
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *);
extern void func_02038414(dBgCh_Actr *);
extern void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *);
extern struct SurfaceInfo *_ZNK10dBgCh_Actr14GetFloorResultEv(dBgCh_Actr *);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *, Vector3 *);
extern int _ZNK10dBgCh_Actr13GetLimMovFlagEv(dBgCh_Actr *);
extern int _ZN4cstd4fdivEii(int, int);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *);
extern struct SurfaceInfo *_ZNK10dBgCh_Actr13GetWallResultEv(dBgCh_Actr *);
}

void dEnemyBase_c::UpdateWMClsn(dBgCh_Actr & clsn_, unsigned int sel)
{
    dBgCh_Actr *clsn = &clsn_;

    switch (sel) {
    case 1: func_020383f0(clsn); break;
    case 2: dBgCh_Actr_UpdateDiscreteNoLava_veneer(clsn); break;
    case 3: func_02038414(clsn); break;
    default: dBgCh_Actr_UpdateContinuous_Veneer(clsn); break;
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {

        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn)+4, (Vector3*)&mFloorNormalX);
        if (_ZNK10dBgCh_Actr13GetLimMovFlagEv(clsn) == 0) {
            int dz = mFloorNormalY;
            if (dz != 0) {
                int nx = mFloorNormalX;
                int vx = unk_0a4;
                int nz = mFloorNormalZ;
                int vz = unk_0ac;
                long long a = (long long)nx * vx + 0x800;
                long long b = (long long)nz * vz + 0x800;
                int num = (int)(a >> 12) + (int)(b >> 12);
                int q = _ZN4cstd4fdivEii(num, dz);
                mVertSpeed = -(q + 0x8000);
            }
        }
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4, (Vector3*)&mWallNormalX);
    }

}

/* Its legacy file built this from a local `struct dEnemyBase_c : dActor_c` shadow with a
   local POD `struct Vector3`; both are the real ones now. Measured: using
   types.h's Vector3 -- which declares ~Vector3(){} and is therefore NOT a POD --
   for the local `v` costs nothing here, all 0x13c bytes reproduce, which is what
   lets dActor_c::Spawn take it by const reference as its real signature says. */
extern "C" int RandomIntInternal(int *seed);
extern u16 data_ov002_020ff014;
extern int data_0209e650;

// @symbol _ZN12dEnemyBase_c9SpawnCoinEv
void dEnemyBase_c::SpawnCoin()
{
    char *t = (char *)this;
    int i;
    Vector3 v;
    if (*(u8 *)(t + 0x108) != 0) {
        int tz = *(int *)(t + 0x64);
        int ty = *(int *)(t + 0x60) + 0x78000;
        int tx = *(int *)(t + 0x5c);
        v.x = tx;
        v.y = ty;
        v.z = tz;
        if (*(u8 *)(t + 0x108) >= 4)
            *(u8 *)(t + 0x108) = 1;
        for (i = 0; i < *(u8 *)(t + 0x10a) + 1; i++) {
            dActor_c *coin = dActor_c::Spawn(
                (&data_ov002_020ff014)[*(u8 *)(t + 0x108) - 1],
                0xf2, v, 0, *(s8 *)(t + 0xcc), -1);
            if (coin != 0) {
                int rnd1 = RandomIntInternal(&data_0209e650);
                int rnd2 = RandomIntInternal(&data_0209e650);
                int a = (int)((u32)rnd1 >> 16 << 27) >> 16;
                u32 b = (u32)rnd2 >> 16;
                *(s16 *)((char *)coin + 0x92) = 0;
                *(s16 *)((char *)coin + 0x94) = (s16)(a * i);
                *(s16 *)((char *)coin + 0x96) = 0;
                *(int *)((char *)coin + 0x98) = (((b % 50) + 100) << 15) / 100;
            }
        }
    }
    this->PoofDust();
}

// @symbol func_ov002_020aea30
/* Start the selected enemy death sequence. This reconstructed interface uses
   receiver, attacker and nullable collision pointers, matching all callers.
   The handler table forwards the latter two registers; current handlers use
   the attacker and ignore collision. The exact original prototype is unknown.
   Incoming r3 is overwritten before use. Prior fourth-argument claims confused
   death-state stores with argument setup; three-argument compiler probes match.
   ov004 has a different function at this address and remains separately named.

   If the state index at +0x10c is set: clears bit 0x10000000 at +0xb0, zeroes
   the halfword at +0x102, invokes the pointer-to-member-function from
   data_ov002_0210db80[index-1] (forwarding both pointer args), then writes
   -0x2000 to +0x9c and clears the +0xb0 bit again.

   The receiver stays a file-local shadow (`Aea30C`) rather than dEnemyBase_c: the
   pointer-to-member type is what selects the call sequence, and dEnemyBase_c has a
   polymorphic base, so retyping it is a codegen change, not a rename. */
struct Aea30C;
typedef void (Aea30C::*Aea30PMF)(dActor_c*, dBgCh_Actr*);
extern Aea30PMF data_ov002_0210db80[];
struct Aea30C {
  char pad0[0x9c];
  int f9c;
  char pad1[0x10];
  int fb0;
  char pad2[0x4e];
  short f102;
  char pad3[8];
  int f10c;
};
extern "C" void func_ov002_020aea30(Aea30C* c, dActor_c* a, dBgCh_Actr* b) {
  if (c->f10c == 0) return;
  (*(unsigned int*)((char*)c + 0xb0)) &= ~0x10000000;
  c->f102 = 0;
  (c->*data_ov002_0210db80[c->f10c - 1])(a, b);
  c->f9c = -0x2000;
  (*(unsigned int*)((char*)c + 0xb0)) &= ~0x10000000;
}

// @symbol func_ov002_020aea2c
extern "C" void func_ov002_020aea2c(void)
{
}

// @symbol func_ov002_020aea24
extern "C" int func_ov002_020aea24(void)
{
    return 0;
}

// @symbol func_ov002_020ae9f8
/* Zero two u32 fields, set a u16 field to 0xf, then clear bit 0x1 in the u32 at
   self+0xb0. */
extern "C" void func_ov002_020ae9f8(char *self)
{
    *(unsigned int *)(self + 0x98) = 0;
    *(unsigned int *)(self + 0xa8) = 0;
    *(unsigned short *)(self + 0x102) = 0xf;
    *(unsigned int *)(self + 0xb0) &= ~0x1;
}

// @symbol func_ov002_020ae968
extern "C" {
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void *v);
extern u16 data_ov002_020ff01c[];
}

extern "C" void func_ov002_020ae968(char* c, char* arg)
{
    int b;
    *(int*)(c + 0xa8) = 0x14000;
    b = *(u16*)(arg + 0xc);
    b = b == 0xbf;
    if (b != 0) {
        u16 t = data_ov002_020ff01c[(u16)*(int*)(arg + 8)];
        *(int*)(c + 0x98) = ((int)t << 0xf) / 50;
    } else {
        *(int*)(c + 0x98) = 0x14000;
    }
    *(int *)(((int)c + 0xb0)) &= ~1;
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (void*)(c + 0x74));
}

// @symbol func_ov002_020ae954
struct Unk954A
{
    char pad[0x94];
    short unk94;
};

struct Unk954B
{
    char pad[0x8e];
    short unk8e;
};

extern "C" void func_ov002_020ae954(Unk954A *a, Unk954B *b)
{
    a->unk94 = b->unk8e;
    func_ov002_020ae968((char *)a, (char *)b);
}

// @symbol func_ov002_020ae8b8
extern "C" void func_ov002_020ae8b8(char* c, char* arg)
{
    int b;
    b = *(u16*)(arg + 0xc);
    b = b == 0xbf;
    if (b != 0) {
        u16 t = data_ov002_020ff01c[(u16)*(int*)(arg + 8)];
        int scaled = (int)t << 3;
        int ip = scaled << 12;
        int d1 = ip / 100;
        int d2 = scaled / 100;
        *(int*)(c + 0x98) = d1;
        *(int*)(c + 0xa8) = (d2 + 0x20) << 12;
    } else {
        *(int*)(c + 0x98) = 0xa000;
        *(int*)(c + 0xa8) = 0x28000;
    }
    *(int *)(((int)c + 0xb0)) &= ~1;
}

// @symbol func_ov002_020ae890
extern "C" int func_ov002_020ae890(void* c, void* a)
{
    *(short*)((char*)c+0x94) = *(short*)((char*)a+0x8e);
    func_ov002_020ae8b8((char*)c, (char*)a);
    return _ZN5Sound9PlayBank0EjRK7Vector3(0xa, (char*)c+0x74);
}

// @symbol func_ov002_020ae87c
struct Unk87cA
{
    char pad[0x94];
    short unk94;
};

struct Unk87cB
{
    char pad[0x8e];
    short unk8e;
};

extern "C" void func_ov002_020ae87c(Unk87cA *a, Unk87cB *b)
{
    a->unk94 = b->unk8e;
    func_ov002_020ae8b8((char *)a, (char *)b);
}

// @symbol func_ov002_020ae844
extern "C" s16 Vec3_HorzAngle(const void *v0, const void *v1);

extern "C" void func_ov002_020ae844(void *c, void *a)
{
    s16 angle = Vec3_HorzAngle((char *)a + 0x5c, (char *)c + 0x5c);
    *(short *)((char *)c + 0x94) = angle;
    func_ov002_020ae968((char *)c, (char *)a);
}

// @symbol func_ov002_020ae80c
extern "C" void func_ov002_020ae80c(void* c, void* a)
{
    *(short*)((char*)c+0x94) = Vec3_HorzAngle((char*)a+0x5c, (char*)c+0x5c);
    func_ov002_020ae8b8((char*)c, (char*)a);
}

// @symbol func_ov002_020ae73c
extern "C" void func_ov002_020ae73c(char* c, char* arg)
{
    *(s16*)(c + 0x94) = Vec3_HorzAngle(arg + 0x5c, c + 0x5c);
    *(int*)(c + 0xa8) = 0xa000;

    if ((int)(*(unsigned short*)(arg + 0xc) == 0xbf) != 0)
        *(int*)(c + 0x98) = (data_ov002_020ff01c[(unsigned short)*(u32*)(arg + 8)] << 15) / 50;
    else
        *(int*)(c + 0x98) = 0x14000;

    {
        int *p = (int *)(((int)(c) + 0x98));
        *p = *p / 6;
    }
    *(s16*)(c + 0x102) = 8;
    *(int *)(((int)(c) + 0xb0)) &= ~1;
    _ZN5Sound9PlayBank0EjRK7Vector3(0xa, c + 0x74);
}

/* unk_10c selects which death handler from a table of POINTERS TO MEMBER
   FUNCTION, then the position and mesh collision are updated regardless. */
extern int (dEnemyBase_c::*data_ov002_0210dbc0[])(dBgCh_Actr &);

extern "C" {
extern void DecIfAbove0_Short(unsigned short *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *thiz, void *clsn);
}

// @symbol _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr
int dEnemyBase_c::UpdateDeath(dBgCh_Actr & clsn_)
{
    dBgCh_Actr *clsn = &clsn_;
    int ret;
    if (mDeathState == 0)
        return 0;
    DecIfAbove0_Short(&mDeathTimer);
    ret = (this->*data_ov002_0210dbc0[mDeathState - 1])(*clsn);
    this->UpdatePos(0);
    UpdateWMClsn(*clsn, 0);
    return ret;
}

// @symbol func_ov002_020ae64c
/* Forward declaration, because func_ov002_020ae5c8 is written LOWER in this file (the whole
   TU runs highest ROM address first).

   THE SECOND PARAMETER IS REAL AND IS LOAD-BEARING, and merging is what proved
   it. func_ov002_020ae5c8's own file defines it as `(void *c)` and never reads a
   second argument; this file's declared it `(char *, int)` and passed one. Built
   the one-argument way, this function comes out 0x50 against the ROM's 0x5c --
   twelve bytes and three instructions short, because `x` no longer has to
   survive the body in a callee-saved register. So both observations are true at
   once: the caller passes it, the callee ignores it, and the definition below
   carries the parameter unused. */
extern "C" int func_ov002_020ae5c8(void *c, int x);

extern "C" int func_ov002_020ae64c(char* c, int x){
  if(*(unsigned short*)(c+0x102) >= 5){
    int v=*(int*)(c+0x84) - 0x23d;
    if(v < 0x4cc) v=0x4cc;
    *(int*)(c+0x84)=v;
    *(int*)(c+0x88)=0x2000 - *(int*)(c+0x84);
    *(int*)(c+0x80)=*(int*)(c+0x88);
  }
  return func_ov002_020ae5c8(c, x);
}

// @symbol func_ov002_020ae608
extern "C" int func_ov002_020ae608(void* c, void* a){
  if(_ZNK10dBgCh_Actr10IsOnGroundEv(a)==0) return 0;
  ((dEnemyBase_c *)c)->SpawnCoin();
  ((dEnemyBase_c *)c)->KillAndTrackInDeathTable();
  *(int*)((char*)c+0x10c)=0;
  return 1;
}

// @symbol func_ov002_020ae5c8
/* `x` is unused on purpose -- see the note on func_ov002_020ae64c, which passes it. */
extern "C" int func_ov002_020ae5c8(void* c, int x){
  unsigned short* p=(unsigned short*)((char*)c+0x100);
  if(p[1]!=0) return 0;
  ((dEnemyBase_c *)c)->SpawnCoin();
  ((dEnemyBase_c *)c)->KillAndTrackInDeathTable();
  *(int*)((char*)c+0x10c)=0;
  return 1;
}

// @symbol func_ov002_020ae4cc
struct Vector3_16f;
extern "C" {
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
  unsigned int a, unsigned int b, int c, int d, int e, const void* f, void* g);
extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
  u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const Vector3_16f* dir);
}

extern "C" int func_ov002_020ae4cc(char* self, char* clsn){
  int v[3];
  int n = *(int*)(self+0x84);
  if (n == 0) {
    int z = *(int*)(self+0x64);
    int y = *(int*)(self+0x60) + 0x50000;
    int x = *(int*)(self+0x5c);
    v[0] = x;
    v[1] = y;
    v[2] = z;
  } else {
    int z = *(int*)(self+0x64);
    int y = *(int*)(self+0x60) + n * 0x50;
    int x = *(int*)(self+0x5c);
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {
    ((dEnemyBase_c *)self)->SpawnCoin();
    ((dEnemyBase_c *)self)->KillAndTrackInDeathTable();
    *(int*)(self+0x10c) = 0;
    return 1;
  }
  *(void**)(self+0xf8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    *(unsigned int*)(self+0xf8), 0x13a, *(int*)((char*)v + 0), *(int*)((char*)v + 4), *(int*)((char*)v + 8), 0, 0);
  *(u32*)(self+0xfc) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    *(unsigned int*)(self+0xfc), 0x13b, *(int*)((char*)v + 0), *(int*)((char*)v + 4), *(int*)((char*)v + 8), 0);
  return 0;
}

// @symbol func_ov002_020ae454
extern "C" int func_ov002_020ae454(char* c, void* a){
  if(*(unsigned short*)(c+0x102)==0 || _ZNK10dBgCh_Actr10IsOnGroundEv(a)!=0 || _ZNK10dBgCh_Actr8IsOnWallEv(a)!=0){
    ((dEnemyBase_c *)c)->SpawnCoin();
    ((dEnemyBase_c *)c)->KillAndTrackInDeathTable();
    *(int*)(c+0x10c)=0;
    return 1;
  }
  return 0;
}

/* Written as the real method over the real dBgCh_Lin/dBgPi classes, which is the
   form the legacy shard recovered and byte-matched. The two by-value Fix12i
   parameters the mangled name claims are spelled as themselves. */
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern short data_02082214[];
}

// @symbol _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi
int dEnemyBase_c::IsGoingOffCliff(dBgCh_Actr &clsn_, Fix12i fix2, s16 a3,
                                  bool a4, bool a5, Fix12i fix6)
{
  void *clsn = &clsn_;
  Vector3 v1;
  Vector3 v2;
  mCliffState = 0;
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn) != 0) {
    dBgCh_Lin line;
    v1.x = this->mPosX;
    v1.y = this->mPosY;
    v1.z = this->mPosZ;
    v1.y += fix6;
    v2.x = this->mPosX;
    v2.y = this->mPosY;
    v2.z = this->mPosZ;
    v2.y -= fix2;
    line.SetObjAndLine(v1, v2, this);
    if (a4 != 0)
      line.StartDetectingWater();
    if (line.DetectClsn()) {
      if (line.clsnDist - fix6 >= fix2)
        mCliffState = 1;
      if (a5 == 0) {
        dBgPi result;
        line.CopyTo(result);
        if (result.GetClsnID() != -1) {
          mCliffState = 1;
          return 1;
        }
      }
      Vector3 normal;
      line.surface.CopyNormalTo(normal);
      int idx = ((unsigned short)a3 >> 4) * 2;
      short s = data_02082214[idx + 1];
      if (normal.y < s)
        mCliffState = 2;
    } else {
      mCliffState = 1;
    }
  }
  return this->mCliffState != 0;
}

/* On a wall, reflect the heading off it; on a cliff edge (mCliffState), turn
   around; otherwise report that nothing was done. */
extern "C" {
/* ReflectAngle takes Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with scalars in those slots. */
extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *actor, int a, int b, short c);
}

// @symbol _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs
int dEnemyBase_c::AngleAwayFromWallOrCliff(dBgCh_Actr & clsn_, short & outAngle_)
{
    void *clsn = &clsn_;
    short *outAngle = &outAngle_;
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        *outAngle = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(this,
            mWallNormalX, mWallNormalZ, *outAngle);
    } else if (mCliffState) {
        *outAngle = (short)(mPrevAngleY + 0x8000);
    } else {
        return 0;
    }
    return 1;
}

/* One frame of "this enemy is in Yoshi's mouth / has just been spat out". The
   three flag bits at 0x0b0 pick the phase: 0x40000 = held in the mouth (track the
   eater's position and return 2), 0x20000 = swallowed (return 1), 0x80000 = just
   spat out, in which case the launch velocity is built from the eater's facing
   angle and then the body flies under mesh collision until it lands.

   The launch block was the last residue: the cartridge fills the load-use slot
   after `ldr r0,[r6,#0xd0]` with the rounding constant (mov r3,#0x800) while every
   spelling tried here filled it with the position base (add r1,r6,#0x5c). Writing
   the eater test in its POSITIVE sense -- `if (eater != 0) { launch } else v =
   0x14000;` rather than `if (eater == 0) v = 0x14000; else { launch }` -- puts the
   launch block first in the linearisation and the slot falls the cartridge's way.
   Same branch, same semantics; only the order the two arms are visited in.

   The raw `self + offset` spellings are kept because dEnemyBase_c's header does
   not name every field this function touches; `this` and the reference argument
   arrive in r0/r1 exactly as the free-function form did, so the body is unchanged.

   UpdateWMClsn above already declared GetLimMovFlag, UpdateContinuous, IsOnGround,
   GetFloorResult and CopyNormalTo with typed receivers in a file-scope extern "C"
   region. A member function cannot sit inside such a region, so those cannot be
   re-declared here with this member's narrower `char *` view -- extern "C" has no
   overloading. The typed declarations are reused and the two places the width
   mattered (GetFloorResult's return feeding `fr + 4`, CopyNormalTo's arguments)
   carry an explicit cast, which costs nothing. */
extern "C" {
extern s32 Vec3_HorzLen(const Vector3 *v);
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *c);
extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *c);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *c);
extern short data_02082214[];
}


// @symbol _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr
int dEnemyBase_c::UpdateYoshiEat(dBgCh_Actr & clsn_)
{
    char *self = (char *)this;
    char *clsn = (char *)&clsn_;
    int flags;
    int t;
    int v;
    int lim;

    flags = *(int *)(self + 0xb0);
    t = flags & 0x40000;
    t = t != 0;
    if (t != 0) {
        if (*(char **)(self + 0xd0) == 0) {
            *(int *)(self + 0xb0) &= ~0x40000;
        } else {
            char *o = *(char **)(self + 0xd0) + 0x5c;
            *(int *)(self + 0x5c) = *(int *)o;
            *(int *)(self + 0x60) = *(int *)(o + 4);
            *(int *)(self + 0x64) = *(int *)(o + 8);
            *(int *)(self + 0x60) += 0x30000;
        }
        *(unsigned char *)(self + 0x107) = 0;
        return 2;
    }
    t = flags & 0x20000;
    t = t != 0;
    if (t != 0) {
        *(unsigned char *)(self + 0x107) = 0;
        return 1;
    }
    t = flags & 0x80000;
    t = t != 0;
    if (t != 0 || *(unsigned char *)(self + 0x107) != 0) {
        if (*(u16 *)(self + 0x104) != 0)
            *(u16 *)(self + 0x104) -= 1;
        t = *(int *)(self + 0xb0) & 0x80000;
        t = t != 0;
        if (t != 0) {
            *(unsigned char *)(self + 0x107) = 1;
            *(u16 *)(self + 0x104) = 5;
            *(int *)(self + 0xb0) &= ~0x80000;
            if (*(char **)(self + 0xd0) != 0) {
                char *o2 = *(char **)(self + 0xd0) + 0x5c;
                *(int *)(self + 0x5c) = *(int *)o2;
                *(int *)(self + 0x60) = *(int *)(o2 + 4);
                *(int *)(self + 0x64) = *(int *)(o2 + 8);
                *(int *)(self + 0x60) += 0x32000;
                {
                    long long rnd = 0x800;
                    char *o3 = *(char **)(self + 0xd0);
                    u16 ang = *(u16 *)(o3 + 0x8e);
                    int idx = (ang >> 4) << 1;
                    *(int *)(self + 0x5c) =
                        (int)(((long long)data_02082214[idx] * 0x32000 + rnd) >> 12) +
                        *(int *)(self + 0x5c);
                    *(int *)(self + 0x64) +=
                        (int)(((long long)data_02082214[idx + 1] * 0x32000 + rnd) >> 12);
                }
                {
                    s16 *p = (s16 *)(*(char **)(self + 0xd0) + 0x8c);
                    *(s16 *)(self + 0x92) = *p;
                    *(s16 *)(self + 0x94) = p[1];
                    *(s16 *)(self + 0x96) = p[2];
                }
                v = *(int *)(*(char **)(self + 0xd0) + 0x98);
            } else {
                v = 0x14000;
            }
            if (v >= 0x14000)
                *(int *)(self + 0x98) = v + 0xa000;
            else
                *(int *)(self + 0x98) = (int)(((long long)v * 0x800 + 0x800) >> 12) + 0x14000;
            *(int *)(self + 0x98) = (int)(((long long)*(int *)(self + 0x98) * 0x14cc + 0x800) >> 12);
            *(int *)(self + 0xa8) = 0xc000;
            *(int *)(self + 0xd0) = 0;
        }
        lim = _ZNK10dBgCh_Actr13GetLimMovFlagEv((dBgCh_Actr *)clsn);
        _ZN10dBgCh_Actr13SetLimMovFlagEv(clsn);
        _ZN8dActor_c9UpdatePosEP5dCc_c(self, 0);
        dBgCh_Actr_UpdateContinuous_Veneer((dBgCh_Actr *)clsn);
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn) != 0) {
            char *fr = (char *)_ZNK10dBgCh_Actr14GetFloorResultEv((dBgCh_Actr *)clsn);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, (Vector3 *)(self + 0xd4));
            if (_ZNK10dBgCh_Actr13JustHitGroundEv(clsn) == 0) {
                *(int *)(self + 0xa8) = 0;
                *(unsigned char *)(self + 0x107) = 0;
            } else {
                _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, (Vector3 *)(self + 0xd4));
                *(int *)(self + 0xa8) =
                    _ZN4cstd4fdivEii((*(int *)(self + 0xa8) * -50) / 100, *(int *)(self + 0xd8));
            }
            {
                int *pa = (int *)(self + 0xa4);
                *pa += *(int *)(self + 0xd4) << 3;
                *(int *)(self + 0xac) += *(int *)(self + 0xdc) << 3;
                *(int *)(self + 0x98) = Vec3_HorzLen((const Vector3 *)pa);
            }
        }
        *(s16 *)(self + 0x8e) = *(s16 *)(self + 0x94);
        if (lim == 0)
            _ZN10dBgCh_Actr15ClearLimMovFlagEv(clsn);
        return 3;
    }
    *(int *)(self + 0xd0) = 0;
    *(int *)(self + 0xb0) &= ~0xe0000;
    *(unsigned char *)(self + 0x107) = 0;
    return 0;
}

/* While mEatenByYoshi is set, a cylinder collision against anything other than actor
   IDs 0x120/0x121 spawns the mega-character particles; otherwise bit 0x20000 on
   the collision is raised. Clearing mEatenByYoshi clears that bit instead. */
extern "C" {
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int);
}

// @symbol _ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c
int dEnemyBase_c::SpawnParticlesIfHitOtherObj(dCc_c & clsn_)
{
    char* clsn = (char*)&clsn_;
    int* f;
    if (mEatenByYoshi != 0) {
        unsigned int id = *(unsigned int*)(clsn+0x24);
        if (id != 0) {
            void* a = _ZN8dActor_c10FindWithIDEj(id);
            if (a != 0) {
                unsigned short t = *(unsigned short*)((char*)a+0xc);
                int e1 = (t == 0x120);
                if (e1 == 0) {
                    int e2 = (t == 0x121);
                    if (e2 == 0) {
                        SpawnMegaCharParticles(*(dActor_c *)a, clsn);
                        return 1;
                    }
                }
            }
        }
        f = (int*)(((int)clsn + 0x18));
        *f = *f | 0x20000;
        goto done;
    }
    f = (int*)(((int)clsn + 0x18));
    *f = *f & ~0x20000;
done:
    return 0;
}

// @symbol _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc
extern "C" {
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern s32 Vec3_HorzLen(const Vector3 *v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern s32 *Vec3_AsrInPlace(s32 *v, s32 sh);
}

void dEnemyBase_c::SpawnMegaCharParticles(dActor_c &a, char *p)
{
    char *self = (char *)this;
    char *ap = (char *)&a;
    s16 dirvec[3];
    Vector3 v;
    Vector3 dst, src;

    dst.x = *(s32 *)(self + 0x5c);
    dst.y = *(s32 *)(self + 0x60);
    {
        Vector3 *pv = (Vector3 *)(((long long)(int)(ap + 0x5c)));
        dst.z = *(s32 *)(self + 0x64);
        src.x = pv->x;
        src.y = pv->y;
        src.z = pv->z;
    }

    {
        int t = ((int) * (u16 *)(self + 0x8e)) >> 4;
        dirvec[0] = data_02082214[t * 2];
        dirvec[1] = 0;
        t = ((int) * (u16 *)(self + 0x8e)) >> 4;
        dirvec[2] = data_02082214[t * 2 + 1];
    }

    if (p != 0) {
        Vector3 delta;
        s32 w = *(s32 *)(p + 4);
        s32 aX, aY;
        s32 iX, iY;
        s32 s;

        Vec3_Sub(&delta, &src, &dst);

        v.x = delta.x;
        v.y = delta.y;
        v.z = delta.z;

        aX = _ZN4cstd5atan2E5Fix12IiES1_(v.x, v.z);
        aY = _ZN4cstd5atan2E5Fix12IiES1_(v.y, Vec3_HorzLen(&v));

        iX = (u16)aX >> 4;
        iY = (u16)aY >> 4;

        s = (s32)(((long long)w * data_02082214[iY * 2 + 1] + 0x800) >> 12);
        dst.x += (s32)(((long long)s * data_02082214[iX * 2] + 0x800) >> 12);
        dst.y += (s32)(((long long)w * data_02082214[iY * 2] + 0x800) >> 12);
        dst.z += (s32)(((long long)s * data_02082214[iX * 2 + 1] + 0x800) >> 12);

        dst.y += (*(s32 *)(p + 8)) >> 1;
    } else {
        Vector3 delta;
        Vec3_Sub(&delta, &src, &dst);
        dst.x = delta.x;
        dst.y = delta.y;
        dst.z = delta.z;
        Vec3_AsrInPlace(&dst.x, 1);
        dst.y += 0x46000;
    }

    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0x43, dst.x, dst.y, dst.z, 0, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0x44, dst.x, dst.y, dst.z, 0, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0xbf, dst.x, dst.y, dst.z, dirvec, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0xc0, dst.x, dst.y, dst.z, 0, 0);
}

/* What happens when a mega/invincible Mario runs an enemy over. The enemy is
   turned to face the player, given the launch velocity the caller passed, put into
   death state 8 with a 30-frame timer, and two particle systems are spawned at its
   position raised by whatever OnAimedAtWithEgg (vtable slot 29) reports.

   The third parameter is genuinely unused -- every caller materialises it and no
   body reads it. include/dEnemyBase_c.h carries the census that proves both the
   arity and this address's overlay; this member spent a long time spelled
   func_ov002_020ada40 because the mangled name was sitting on ov004's unrelated
   function at the same address. */
extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern void _ZN6Player16IncMegaKillCountEv(void *p);
}

// @symbol _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE
void dEnemyBase_c::KillByInvincibleChar(const Vector3_16 & vel_, Player & player_, Fix12<int>)
{
    mFlags &= ~0x10000001;
    mPrevAngleY = Vec3_HorzAngle((const Vector3 *)((char *)&player_ + 0x5c),
                                 (const Vector3 *)&mPosX);
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mDeathTimer = 0x1e;
    mSpinRateX = vel_.x;
    mSpinRateY = vel_.y;
    mSpinRateZ = vel_.z;
    mDeathState = 8;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (void *)&mCamSpacePosX);
    volatile int v[3];
    v[0] = mPosX;
    v[1] = mPosY;
    v[2] = mPosZ;
    int ret = OnAimedAtWithEgg();
    int vy = v[1];
    int vx = v[0];
    vy = vy + ret;
    v[1] = vy;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, vx, vy, v[2]);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, v[0], v[1], v[2]);
    _ZN6Player16IncMegaKillCountEv(&player_);
}


/* One frame of the death an invincible (mega) character inflicts. `flags` is a
   two-bit selector, not a boolean: bit 0 drops a coin, bit 1 books the kill in
   the death table. Both reference parameters are null-checked -- the compiler
   emits the test rather than folding it away, so `&ref == 0` is how the ROM's
   check is spelled. */
extern "C" {
extern void Vec3_Asr(void *dst, const void *src, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *actor);
extern char data_020a0e68;
}

/* Reaches dActor_c vtable slot 29 -- vtable+0x74 -- whose return value, arithmetic
   shifted right by 3, is the height of the point the model spins about. Kept as
   a stand-in rather than dActor_c::OnAimedAtWithEgg() because that slot's NAME is
   imported from a different hierarchy (dScMgBase_c) and does not fit this use;
   flagged, not resolved. */
struct KbicVB {
    virtual void d00();
    virtual void d01();
    virtual void d02();
    virtual void d03();
    virtual void d04();
    virtual void d05();
    virtual void d06();
    virtual void d07();
    virtual void d08();
    virtual void d09();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void d18();
    virtual void d19();
    virtual void d20();
    virtual void d21();
    virtual void d22();
    virtual void d23();
    virtual void d24();
    virtual void d25();
    virtual void d26();
    virtual void d27();
    virtual void d28();
    virtual int m29();
};

struct KbicM48 { int w[12]; };

#define LAUNDER(p) ((int)(p))

// @symbol _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj
int dEnemyBase_c::UpdateKillByInvincibleChar(dBgCh_Actr & ww_, ModelAnim & mm_, unsigned int flags)
{
    dBgCh_Actr *clsn = &ww_;
    ModelAnim *anim = &mm_;
    int v[3];

    if (mDeathState != 8)
        return 0;

    if (mDeathTimer != 0)
        *(unsigned short *)LAUNDER(&mDeathTimer) -= 1;

    if (mDeathTimer == 0 ||
        (clsn != 0 && _ZNK10dBgCh_Actr10IsOnGroundEv(clsn) != 0 && mVertSpeed < 0)) {
        if (flags & 1)
            SpawnCoin();
        if (flags & 2)
            _ZN8dActor_c24KillAndTrackInDeathTableEv(this);
        mDeathState = 0;
        return 2;
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(this, 0);
    if (clsn != 0) {
        UpdateWMClsn(*clsn, 0);
        if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn) != 0)
            mPrevAngleY = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
                this, mWallNormalX, mWallNormalZ, mPrevAngleY);
    }

    if (anim != 0) {
        *(short *)LAUNDER(&mAngleX) += mSpinRateX;
        *(short *)LAUNDER(&mAngleY) += mSpinRateY;
        *(short *)LAUNDER(&mAngleZ) += mSpinRateZ;
        Vec3_Asr(v, &mPosX, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            ((KbicVB *)(void *)this)->m29() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
            mAngleX, mAngleY, mAngleZ);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            (-((KbicVB *)(void *)this)->m29()) >> 3, 0);
        *(KbicM48 *)((char *)anim + 0x1c) = *(KbicM48 *)&data_020a0e68;
    }
    return 1;
}

/* Ownership pilot for __sinit_ov002_02100938.  This is excluded from the normal
   shadow build: compile with -DSINIT_OWNERSHIP_PILOT to ask mwccarm to generate
   the candidate TU's static initializer from ordinary non-const PMF arrays.
   The current func_ov002_* placeholders do not expose the original member names,
   so the probe methods stand in for symbol spelling; the PMF representation,
   initializer bytes, section layout, relocation slots and addends are the claim
   being tested.  There is intentionally no hand-written __sinit body. */
#ifdef SINIT_OWNERSHIP_PILOT
struct EnemySinitOwnerProbe {
    void action0(int, int);
    void action1(int, int);
    void action2(int, int);
    void action3(int, int);
    void action4(int, int);
    void action5(int, int);
    void action6(int, int);
    void action7(int, int);

    int death0(dBgCh_Actr&);
    int death1(dBgCh_Actr&);
    int death2(dBgCh_Actr&);
    int death3(dBgCh_Actr&);
    int death4(dBgCh_Actr&);
    int death5(dBgCh_Actr&);
    int death6(dBgCh_Actr&);
    int death7(dBgCh_Actr&);
};

typedef void (EnemySinitOwnerProbe::*EnemyActionPMF)(int, int);
typedef int (EnemySinitOwnerProbe::*EnemyDeathPMF)(dBgCh_Actr&);

EnemyActionPMF data_ov002_0210db80__sinit_pilot[8] = {
    &EnemySinitOwnerProbe::action0,
    &EnemySinitOwnerProbe::action1,
    &EnemySinitOwnerProbe::action2,
    &EnemySinitOwnerProbe::action3,
    &EnemySinitOwnerProbe::action4,
    &EnemySinitOwnerProbe::action5,
    &EnemySinitOwnerProbe::action6,
    &EnemySinitOwnerProbe::action7,
};

EnemyDeathPMF data_ov002_0210dbc0__sinit_pilot[8] = {
    &EnemySinitOwnerProbe::death0,
    &EnemySinitOwnerProbe::death1,
    &EnemySinitOwnerProbe::death2,
    &EnemySinitOwnerProbe::death3,
    &EnemySinitOwnerProbe::death4,
    &EnemySinitOwnerProbe::death5,
    &EnemySinitOwnerProbe::death6,
    &EnemySinitOwnerProbe::death7,
};
#endif
