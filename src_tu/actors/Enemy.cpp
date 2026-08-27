//cpp
/* Recovered translation unit -- ov002/dEnemyBase_c  (Nintendo's dEnemyBase_c)
 *
 * .text span 0x020ad838..0x020aedbc, 31 functions in build/tu_map.json.
 * THIRTY of them are assembled here; ordinal 4,
 * _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr (0x020ade78, 0x3cc), has no source
 * anywhere under src/ and is the module's one unmatched member, so it is a hole
 * in the middle of this run. That is why the manifest declares 30 functions and
 * why this TU can never be link-verified as it stands.
 *
 * NOT ENROLLED. config/**\/delinks.txt still names the thirty one-function
 * files under src/ as the sole owners of these bytes.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. mwccarm 2004/b56 emits one .text
 * section per function in the reverse of source order, so the HIGHEST-address
 * ROM function (_ZN12dEnemyBase_cC2Ev, 0x020aed98) is written FIRST and the lowest
 * (dEnemyBase_c::UpdateKillByInvincibleChar, 0x020ad838) LAST. Do not reorder. The one
 * exception is the destructor: a single `dEnemyBase_c::~dEnemyBase_c()` emits D2, D0 and D1 as
 * three sections in a compiler-chosen order, which here happens to be exactly
 * the ROM's (0x020aed18 D2, 0x020aed3c D0, 0x020aed74 D1).
 *
 * Assembled by hand rather than by `tubuild.py create`: three of the legacy
 * files wrap their whole body in `extern "C" { ... }`, which that generator's
 * splitter refuses, and one member has no legacy file at all. The manifest entry
 * is still tubuild's own (tubuild.build_manifest_entry).
 *
 * WHAT THE MERGE FORCED, and nothing else was changed:
 *   1. Local shadow `struct dEnemyBase_c` / `struct dActor_c` (SpawnMegaCharParticles,
 *      SpawnCoin) replaced by include/dEnemyBase_c.h's real classes -- two globally
 *      visible types named `dEnemyBase_c` and `dActor_c` cannot survive in one TU.
 *   2. Duplicate local helper types renamed apart per block (UnkA/UnkB appeared
 *      twice with identical bodies; two files declared their own POD `Vector3`
 *      that collides with types.h's non-POD one). Renaming a file-local type
 *      cannot change codegen; keeping the POD spelling deliberately does not.
 *   3. `enum { false, true };` deleted -- both are keywords in C++.
 *   4. Three members are real dEnemyBase_c methods here, so the four call sites that
 *      used to reach them through `extern "C"` mangled-name declarations call
 *      them as methods instead. Non-virtual, so still a direct `bl`.
 *   5. Same extern declared with different signatures in different files ->
 *      one canonical spelling, keeping the most complete observation.
 *   6. include/decl_common.h dropped: it declares this TU's OWN
 *      func_ov002_020ae968 as `int (void*, void*)` while the definition below
 *      is `void (char*, char*)`.
 *
 * KEY-FUNCTION SIDE EFFECT: dEnemyBase_c's first non-inline virtual is ~dEnemyBase_c, and this
 * TU defines it, so the object also carries _ZTV12dEnemyBase_c/_ZTI12dEnemyBase_c/_ZTS12dEnemyBase_c and
 * the inherited RTTI records. Expected, unlicensed, and inventoried by
 * `tubuild.py verify` -- their correctness is NOT claimed here.
 */
#include "dEnemyBase_c.h"
#include "decl_dBgPi.h"

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN12dEnemyBase_cC2Ev, 0x020aed98, size 0x24                      */
/* -------------------------------------------------------------------------- */
/* Left as an extern "C" free function under its mangled name, NOT written as
   `dEnemyBase_c::dEnemyBase_c() {}`. dEnemyBase_c.h declares `dEnemyBase_c();` precisely so the compiler
   never synthesises and inlines one into the 51 subclasses; defining it as a
   real method here would undo that. The vptr is stored through
   data_ov002_021081e4 -- the alias ov002/symbols.txt gives _ZTV12dEnemyBase_c's ROM
   address -- rather than through the locally emitted _ZTV12dEnemyBase_c, so the addend
   stays 0 and the pilot report's sec 5.1 vtable-preamble trap does not apply. */
extern "C" {
extern int _ZN8dActor_cC2Ev(void *);
extern int data_ov002_021081e4[];
int _ZN12dEnemyBase_cC2Ev(void *c)
{
    _ZN8dActor_cC2Ev(c);
    *(int *)c = (int)data_ov002_021081e4;
    return (int)c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 27/28/29 -- _ZN12dEnemyBase_cD2Ev 0x020aed18, _ZN12dEnemyBase_cD0Ev 0x020aed3c, */
/* _ZN12dEnemyBase_cD1Ev 0x020aed74.  ONE definition, three emitted sections.          */
/* -------------------------------------------------------------------------- */
/* Store this class's vtable over the one dActor_c's constructor left, then run the
   dActor_c subobject destructor. D0 additionally returns the object to the actor
   heap through dEnemyBase_c's own inline operator delete (see include/dEnemyBase_c.h). The
   three legacy files each spelled one variant; the compiler emits all three from
   this. */
dEnemyBase_c::~dEnemyBase_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj, 0x020aebf8, 0x120 */
/* -------------------------------------------------------------------------- */
/* Runs the mesh collision (sel picks which of four update flavours), then caches
   whichever surface normals came back: the floor result's into mFloorNormal*,
   the wall result's into mWallNormal*. Those two CopyNormalTo calls are what
   evidence both triples as Vector3s.

   On a floor that does not limit movement, the vertical velocity is re-derived
   so the motion stays in the floor plane. */
struct SurfaceInfo { s32 pad; };

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN12dEnemyBase_c9SpawnCoinEv, 0x020aeabc, size 0x13c             */
/* -------------------------------------------------------------------------- */
/* Its legacy file built this from a local `struct dEnemyBase_c : dActor_c` shadow with a
   local POD `struct Vector3`; both are the real ones now. Measured: using
   types.h's Vector3 -- which declares ~Vector3(){} and is therefore NOT a POD --
   for the local `v` costs nothing here, all 0x13c bytes reproduce, which is what
   lets dActor_c::Spawn take it by const reference as its real signature says. */
extern "C" int RandomIntInternal(int *seed);
extern u16 data_ov002_020ff014;
extern int data_0209e650;

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov002_020aea30, 0x020aea30, size 0x8c               */
/* -------------------------------------------------------------------------- */
/* If the state index at +0x10c is set: clears bit 0x10000000 at +0xb0, zeroes
   the halfword at +0x102, invokes the pointer-to-member-function from
   data_ov002_0210db80[index-1] (forwarding both int args), then writes -0x2000
   to +0x9c and clears the +0xb0 bit again.

   The receiver stays a file-local shadow (`Aea30C`) rather than dEnemyBase_c: the
   pointer-to-member type is what selects the call sequence, and dEnemyBase_c has a
   polymorphic base, so retyping it is a codegen change, not a rename. */
struct Aea30C;
typedef void (Aea30C::*Aea30PMF)(int, int);
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
extern "C" void func_ov002_020aea30(Aea30C* c, int a, int b) {
  if (c->f10c == 0) return;
  (*(unsigned int*)((char*)c + 0xb0)) &= ~0x10000000;
  c->f102 = 0;
  (c->*data_ov002_0210db80[c->f10c - 1])(a, b);
  c->f9c = -0x2000;
  (*(unsigned int*)((char*)c + 0xb0)) &= ~0x10000000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov002_020aea2c, 0x020aea2c, size 0x4                */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov002_020aea2c(void)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov002_020aea24, 0x020aea24, size 0x8                */
/* -------------------------------------------------------------------------- */
extern "C" int func_ov002_020aea24(void)
{
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov002_020ae9f8, 0x020ae9f8, size 0x2c               */
/* -------------------------------------------------------------------------- */
/* Zero two u32 fields, set a u16 field to 0xf, then clear bit 0x1 in the u32 at
   self+0xb0. */
extern "C" void func_ov002_020ae9f8(char *self)
{
    *(unsigned int *)(self + 0x98) = 0;
    *(unsigned int *)(self + 0xa8) = 0;
    *(unsigned short *)(self + 0x102) = 0xf;
    *(unsigned int *)(self + 0xb0) &= ~0x1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov002_020ae968, 0x020ae968, size 0x90               */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov002_020ae954, 0x020ae954, size 0x14               */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov002_020ae8b8, 0x020ae8b8, size 0x9c               */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov002_020ae8b8(char* c, char* arg)
{
    int b;
    b = *(u16*)(arg + 0xc);
    b = b == 0xbf;
    if (b != 0) {
        u16 t = data_ov002_020ff01c[(u16)*(int*)(arg + 8)];
        int r4 = (int)t << 3;
        int ip = r4 << 12;
        int d1 = ip / 100;
        int d2 = r4 / 100;
        *(int*)(c + 0x98) = d1;
        *(int*)(c + 0xa8) = (d2 + 0x20) << 12;
    } else {
        *(int*)(c + 0x98) = 0xa000;
        *(int*)(c + 0xa8) = 0x28000;
    }
    *(int *)(((int)c + 0xb0)) &= ~1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov002_020ae890, 0x020ae890, size 0x28               */
/* -------------------------------------------------------------------------- */
extern "C" int func_ov002_020ae890(void* c, void* a)
{
    *(short*)((char*)c+0x94) = *(short*)((char*)a+0x8e);
    func_ov002_020ae8b8((char*)c, (char*)a);
    return _ZN5Sound9PlayBank0EjRK7Vector3(0xa, (char*)c+0x74);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov002_020ae87c, 0x020ae87c, size 0x14               */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov002_020ae844, 0x020ae844, size 0x38               */
/* -------------------------------------------------------------------------- */
extern "C" s16 Vec3_HorzAngle(const void *v0, const void *v1);

extern "C" void func_ov002_020ae844(void *c, void *a)
{
    s16 angle = Vec3_HorzAngle((char *)a + 0x5c, (char *)c + 0x5c);
    *(short *)((char *)c + 0x94) = angle;
    func_ov002_020ae968((char *)c, (char *)a);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov002_020ae80c, 0x020ae80c, size 0x38               */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov002_020ae80c(void* c, void* a)
{
    *(short*)((char*)c+0x94) = Vec3_HorzAngle((char*)a+0x5c, (char*)c+0x5c);
    func_ov002_020ae8b8((char*)c, (char*)a);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov002_020ae73c, 0x020ae73c, size 0xd0               */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr, 0x020ae6a8, 0x94  */
/* -------------------------------------------------------------------------- */
/* unk_10c selects which death handler from a table of POINTERS TO MEMBER
   FUNCTION, then the position and mesh collision are updated regardless. */
extern int (dEnemyBase_c::*data_ov002_0210dbc0[])(dBgCh_Actr &);

extern "C" {
extern void DecIfAbove0_Short(unsigned short *p);
extern int _ZN8dActor_c9UpdatePosEP5dCc_c(void *thiz, void *clsn);
}

int dEnemyBase_c::UpdateDeath(dBgCh_Actr & clsn_)
{
    dBgCh_Actr *clsn = &clsn_;
    int ret;
    if (mDeathState == 0)
        return 0;
    DecIfAbove0_Short(&mDeathTimer);
    ret = (this->*data_ov002_0210dbc0[mDeathState - 1])(*clsn);
    _ZN8dActor_c9UpdatePosEP5dCc_c(this, 0);
    UpdateWMClsn(*clsn, 0);
    return ret;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov002_020ae64c, 0x020ae64c, size 0x5c               */
/* -------------------------------------------------------------------------- */
/* Forward declaration, because ordinal 9 is written LOWER in this file (the whole
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov002_020ae608, 0x020ae608, size 0x44               */
/* -------------------------------------------------------------------------- */
extern "C" int func_ov002_020ae608(void* c, void* a){
  if(_ZNK10dBgCh_Actr10IsOnGroundEv(a)==0) return 0;
  ((dEnemyBase_c *)c)->SpawnCoin();
  ((dEnemyBase_c *)c)->KillAndTrackInDeathTable();
  *(int*)((char*)c+0x10c)=0;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov002_020ae5c8, 0x020ae5c8, size 0x40                */
/* -------------------------------------------------------------------------- */
/* `x` is unused on purpose -- see the note on ordinal 11, which passes it. */
extern "C" int func_ov002_020ae5c8(void* c, int x){
  unsigned short* p=(unsigned short*)((char*)c+0x100);
  if(p[1]!=0) return 0;
  ((dEnemyBase_c *)c)->SpawnCoin();
  ((dEnemyBase_c *)c)->KillAndTrackInDeathTable();
  *(int*)((char*)c+0x10c)=0;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov002_020ae4cc, 0x020ae4cc, size 0xfc               */
/* -------------------------------------------------------------------------- */
extern "C" {
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
  unsigned int a, unsigned int b, int c, int d, int e, const void* f, void* g);
extern void* _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
  unsigned int a, unsigned int b, int c, int d, int e, const void* f);
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
  *(void**)(self+0xfc) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    *(unsigned int*)(self+0xfc), 0x13b, *(int*)((char*)v + 0), *(int*)((char*)v + 4), *(int*)((char*)v + 8), 0);
  return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov002_020ae454, 0x020ae454, size 0x78               */
/* -------------------------------------------------------------------------- */
extern "C" int func_ov002_020ae454(char* c, void* a){
  if(*(unsigned short*)(c+0x102)==0 || _ZNK10dBgCh_Actr10IsOnGroundEv(a)!=0 || _ZNK10dBgCh_Actr8IsOnWallEv(a)!=0){
    ((dEnemyBase_c *)c)->SpawnCoin();
    ((dEnemyBase_c *)c)->KillAndTrackInDeathTable();
    *(int*)(c+0x10c)=0;
    return 1;
  }
  return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi */
/* 0x020ae2b8, size 0x19c                                                     */
/* -------------------------------------------------------------------------- */
/* Stays an extern "C" free function under its mangled name: the symbol claims
   two by-value Fix12<int> parameters, which is the mwccarm 6az wall. */
extern "C" {
extern void _ZN9dBgCh_LinC1Ev(void* self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* act);
extern void _ZN5dBgCh19StartDetectingWaterEv(void* self);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern void _ZN5dBgPiC1Ev(void* self);
extern void _ZNK5dBgPi6CopyToERS_(void* self, void* other);
extern void _ZN5dBgPiD1Ev(void* self);
extern void _ZN9dBgCh_LinD1Ev(void* self);
extern short data_02082214[];
}

extern "C" int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(struct dEnemyBase_c *self, void* clsn, int fix2, short a3, unsigned char a4, unsigned char a5, int fix6) {
  Vector3 v1;
  Vector3 v2;
  char cr[0x28];
  Vector3 normal;
  char rl[0x7c];
  ((char*)self)[0x106] = 0;
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn) != 0) {
    _ZN9dBgCh_LinC1Ev(rl);
    v1.x = self->mPosX;
    v1.y = self->mPosY;
    v1.z = self->mPosZ;
    v1.y += fix6;
    v2.x = self->mPosX;
    v2.y = self->mPosY;
    v2.z = self->mPosZ;
    v2.y -= fix2;
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &v1, &v2, ((char*)self));
    if (a4 != 0)
      _ZN5dBgCh19StartDetectingWaterEv(rl);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl) != 0) {
      if (*(int*)(rl + 0x60) - fix6 >= fix2)
        ((char*)self)[0x106] = 1;
      if (a5 == 0) {
        _ZN5dBgPiC1Ev(cr);
        _ZNK5dBgPi6CopyToERS_(rl + 0x10, cr);
        if (_ZNK5dBgPi9GetClsnIDEv(cr) != -1) {
          ((char*)self)[0x106] = 1;
          _ZN5dBgPiD1Ev(cr);
          _ZN9dBgCh_LinD1Ev(rl);
          return 1;
        }
        _ZN5dBgPiD1Ev(cr);
      }
      _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rl + 0x14, &normal);
      int idx = ((unsigned short)a3 >> 4) * 2;
      short s = data_02082214[idx + 1];
      if (normal.y < s)
        ((char*)self)[0x106] = 2;
    } else {
      ((char*)self)[0x106] = 1;
    }
    _ZN9dBgCh_LinD1Ev(rl);
  }
  return self->unk_106 != 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs      */
/* 0x020ae244, size 0x74                                                      */
/* -------------------------------------------------------------------------- */
/* On a wall, reflect the heading off it; on a cliff edge (unk_106), turn
   around; otherwise report that nothing was done. */
extern "C" {
/* ReflectAngle takes Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with scalars in those slots. */
extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *actor, int a, int b, short c);
}

int dEnemyBase_c::AngleAwayFromWallOrCliff(dBgCh_Actr & clsn_, short & outAngle_)
{
    void *clsn = &clsn_;
    short *outAngle = &outAngle_;
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        *outAngle = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(this,
            mWallNormalX, mWallNormalZ, *outAngle);
    } else if (unk_106) {
        *outAngle = (short)(mPrevAngleY + 0x8000);
    } else {
        return 0;
    }
    return 1;
}

/* ========================================================================== */
/* HOLE: ROM ordinal 4 -- _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr,          */
/* 0x020ade78, size 0x3cc. No source exists anywhere under src/; this is      */
/* ov002's one unmatched member inside the span. Nothing is written for it,    */
/* which is why this TU cannot be link-verified.                              */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c     */
/* 0x020addc0, size 0xb8                                                      */
/* -------------------------------------------------------------------------- */
/* While mEatenByYoshi is set, a cylinder collision against anything other than actor
   IDs 0x120/0x121 spawns the mega-character particles; otherwise bit 0x20000 on
   the collision is raised. Clearing mEatenByYoshi clears that bit instead. */
extern "C" {
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int);
}

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc               */
/* 0x020adb40, size 0x280                                                     */
/* -------------------------------------------------------------------------- */
/* MegaV3 is deliberately a POD copy of Vector3: types.h's Vector3 declares
   ~Vector3(){} and is therefore non-POD, and every use here is a local
   scratch triple never handed to a Vector3-typed callee. */
struct MegaV3 { s32 x, y, z; };

extern "C" {
extern void Vec3_Sub(MegaV3 *out, const MegaV3 *a, const MegaV3 *b);
extern s32 Vec3_HorzLen(const MegaV3 *v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern s32 *Vec3_AsrInPlace(s32 *v, s32 sh);
}

void dEnemyBase_c::SpawnMegaCharParticles(dActor_c &a, char *p)
{
    char *self = (char *)this;
    char *ap = (char *)&a;
    s16 dirvec[3];
    MegaV3 v;
    MegaV3 dst, src;

    dst.x = *(s32 *)(self + 0x5c);
    dst.y = *(s32 *)(self + 0x60);
    {
        MegaV3 *pv = (MegaV3 *)(((long long)(int)(ap + 0x5c)));
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
        MegaV3 delta;
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
        MegaV3 delta;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- func_ov002_020ada40, 0x020ada40, size 0x100               */
/* -------------------------------------------------------------------------- */
/* include/dEnemyBase_c.h identifies this address as dEnemyBase_c::KillByInvincibleChar, but
   naming is out of scope for this TU, so the placeholder symbol stands and the
   receiver stays a file-local shadow. */
struct A40Obj {
    virtual int m00(); virtual int m01(); virtual int m02(); virtual int m03();
    virtual int m04(); virtual int m05(); virtual int m06(); virtual int m07();
    virtual int m08(); virtual int m09(); virtual int m0a(); virtual int m0b();
    virtual int m0c(); virtual int m0d(); virtual int m0e(); virtual int m0f();
    virtual int m10(); virtual int m11(); virtual int m12(); virtual int m13();
    virtual int m14(); virtual int m15(); virtual int m16(); virtual int m17();
    virtual int m18(); virtual int m19(); virtual int m1a(); virtual int m1b();
    virtual int m1c(); virtual int hit();
};

struct A40V3 { int x, y, z; };

struct A40Player {
    char pad5c[0x5c];
    A40V3 pos;
    char pad68[0x94-0x68];
    short ang;
    char pad96[0x98-0x96];
    int f98, f9c, fa0;
    char pada4[0xa8-0xa4];
    int fa8;
    char padac[0xb0-0xac];
    int fb0;
    char padb4[0xec-0xb4];
    short fec, fee, ff0;
    char padf2[0x102-0xf2];
    short f102;
    char pad104[0x10c-0x104];
    int f10c;
};

extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern void _ZN6Player16IncMegaKillCountEv(void* p);
}

extern "C" void func_ov002_020ada40(A40Player* r0, short* r1, void* r2) {
    *(int *)(((int)r0 + 0xb0)) &= ~0x10000001;
    r0->ang = Vec3_HorzAngle((char*)r2+0x5c, &r0->pos);
    r0->f98 = 0xa000;
    r0->fa8 = 0x28000;
    r0->f102 = 0x1e;
    r0->fec = r1[0];
    r0->fee = r1[1];
    r0->ff0 = r1[2];
    r0->f10c = 8;
    r0->f9c = -0x2000;
    r0->fa0 = -0x32000;
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (char*)r0+0x74);
    volatile int v[3];
    v[0] = r0->pos.x;
    v[1] = r0->pos.y;
    v[2] = r0->pos.z;
    int ret = ((A40Obj*)r0)->hit();
    int vy = v[1];
    int vx = v[0];
    vy = vy + ret;
    v[1] = vy;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, vx, vy, v[2]);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, v[0], v[1], v[2]);
    _ZN6Player16IncMegaKillCountEv(r2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_Actr      */
/*                  R9ModelAnimj, 0x020ad838, size 0x208                      */
/* -------------------------------------------------------------------------- */
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
