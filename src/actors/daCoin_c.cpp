//cpp
/* daCoin_c -- the coin actor (yellow, red, and blue).
 *
 * RTTI names it: _ZTS8daCoin_c at ov002:0x02108778 and _ZTI8daCoin_c at
 * 0x02108784. The coined vtable _ZTV4Coin was the same object as
 * _ZTV8daCoin_c at 0x021087ec; this TU keeps the ROM name. The run is
 * 0x020b0f54..0x020b2a98, 28 functions. The three factories at 0x020b2a98
 * (daCoin_c_classInit_BLUE_COIN, _RED_COIN, _COIN) stay in their own files.
 *
 * One out-of-line destructor is the key function. Under
 * `#pragma defer_codegen off` it emits D1, D0, then a D2 the cartridge does
 * not keep, and .text follows source order, so this file is ROM-ascending.
 * `#pragma opt_common_subs off` is bracketed around InitResources only; that
 * shard needs it and the others do not.
 */

#pragma defer_codegen off

#include "common.h"
#include "daCoin_c.h"
#include "SharedFilePtr.h"
#include "Model.h"

/* The .c shards spell false/true as enumerators. In this C++ TU those words
 * are keywords, and mwccarm still honors the object-like macros the
 * InitResources shard already used. */
#define false 0
#define true 1

/* -------------------------------------------------------------------------- */
// @symbol _ZN8daCoin_cD1Ev
// @symbol _ZN8daCoin_cD0Ev
daCoin_c::~daCoin_c()
{
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b1008
extern "C" {
void func_ov002_020b1008(char *c){
    extern void *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void *start);
    extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);

  char* actor;
  if (*(unsigned char*)(c+0x3af)) return;
  if (*(int*)(c+0x3a0) != 1) return;
  actor = (char*)_ZN8dActor_c15FindWithActorIDEjPS_(0xf, 0);
  while (actor) {
    if (Vec3_Dist((Vector3*)(c+0x5c), (Vector3*)(actor+0x5c)) < 0x96000) {
      *(unsigned char*)(c+0x3b0) = 1;
      *(void**)(actor+0x328) = c;
      break;
    }
    actor = (char*)_ZN8dActor_c15FindWithActorIDEjPS_(0xf, actor);
  }
  *(unsigned char*)(c+0x3af) = 1;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b10a0
extern "C" {
int func_ov002_020b10a0(char* c){
    extern int _ZN8dActor_c18GetBitInDeathTableEv(void*);
    extern void* func_ov002_020b1328(void*);
    extern void _ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv(void* self);
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);

  void* marker;
  if(_ZN8dActor_c18GetBitInDeathTableEv(c)==0) return 0;
  marker = func_ov002_020b1328(c);
  if(marker) _ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv(marker);
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b10e4
extern "C" {
void func_ov002_020b10e4(char* c)
{
    struct BF3ae {
        u8 b0 : 1;
    u8 : 4;
    u8 sel : 3;
    };
    extern signed char data_0209f2f8;
    typedef struct dBgCh_Lin { char pad[0x78]; } dBgCh_Lin;
    extern dActor_c* _ZN8dActor_c4NextEPKS_(const dActor_c* prev);
    extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
    extern void *_ZN9dBgCh_LinC1Ev(dBgCh_Lin* self);
    extern void _ZN9dBgCh_LinD1Ev(void* self);
    extern void *_ZN5dBgPiC1Ev(void* self);
    extern void _ZN5dBgPiD1Ev(void* self);
    extern int _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, const Vector3* a, const Vector3* b, dActor_c* obj);
    extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
    extern void _ZNK5dBgPi6CopyToERS_(const void* self, void* dst);
    extern u32 _ZNK5dBgPi9GetClsnIDEv(const void* self);
    extern dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
    extern void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3* res, void* self);

    int b;
    dActor_c* a;

    b = (int)((*(int*)(c + 0xb0) & 8) != 0);
    if (b) return;

    if (((struct BF3ae*)(c + 0x3ae))->sel != 7) return;

    if (data_0209f2f8 == 0x1c || data_0209f2f8 == 0x27) {
        a = _ZN8dActor_c4NextEPKS_(0);
        if (a) {
            do {
                char* ac = (char*)a;
                u16 type = *(u16*)(ac + 0xc);
                if (type == 0x7e || type == 0x81 || type == 0x9c) {
                    int dy = *(int*)(c + 0x60) - *(int*)(ac + 0x60);
                    int radius = *(int*)(ac + 0xb8);
                    int dist = Vec3_HorzDist((Vector3*)(c + 0x5c), (Vector3*)(ac + 0x5c));
                    if (dist < (radius << 3) && dy <= 0x1f4000 && dy >= 0) {
                        *(int*)(c + 0x398) = *(int*)(ac + 0x60) + 0x32000;
                        ((struct BF3ae*)((long long)(c + 0x3ae)))->sel = 1;
                        return;
                    }
                }
                a = _ZN8dActor_c4NextEPKS_(a);
            } while (a);
        }
    }

    {
        char rl[0x78];
        char cr[0x28];
        Vector3 va, vb;
        _ZN9dBgCh_LinC1Ev((dBgCh_Lin*)rl);
        _ZN5dBgPiC1Ev(cr);
        vb.x = *(int*)(c + 0x5c);
        vb.y = *(int*)(c + 0x60);
        vb.z = *(int*)(c + 0x64);
        va.x = vb.x;
        va.y = vb.y;
        va.z = vb.z;
        va.y += 0x14000;
        vb.y -= 0x1f4000;
        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &va, &vb, (dActor_c*)c);
        if (_ZN9dBgCh_Lin10DetectClsnEv(rl)) {
            _ZNK5dBgPi6CopyToERS_(rl + 0x10, cr);
            if (_ZNK5dBgPi9GetClsnIDEv(cr) != (u32)-1 &&
                _ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(cr)) != 0) {
                ((struct BF3ae*)((long long)(c + 0x3ae)))->sel = 0;
            } else {
                Vector3 pos;
                _ZN9dBgCh_Lin10GetClsnPosEv(&pos, rl);
                *(int*)(c + 0x398) = pos.y;
                ((struct BF3ae*)((long long)(c + 0x3ae)))->sel = 1;
            }
        }
        _ZN5dBgPiD1Ev(cr);
        _ZN9dBgCh_LinD1Ev(rl);
    }
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b12ec
extern "C" {
int func_ov002_020b12ec(char *self)
{
    unsigned int v;
    unsigned int flag;
    unsigned int r3;
    char *slot;
    unsigned int r1;

    v = *(unsigned int *)(self + 0xb0);
    if ((v & 0xe0000) != 0) {
        flag = 1;
    } else {
        flag = 0;
    }
    if (flag != 0) {
        return 1;
    }
    r3 = 0;
    *(unsigned int *)(self + 0xd0) = r3;
    slot = (char *)(self + 0xb0);
    r1 = *(unsigned int *)slot;
    r1 &= ~0xe0000u;
    *(unsigned int *)slot = r1;
    return r3;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b1328
extern "C" {
void* func_ov002_020b1328(void* r5) {
    extern int _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);

    void* r1 = 0;
    while (1) {
        r1 = (void*)_ZN8dActor_c15FindWithActorIDEjPS_(0xb4, r1);
        if (!r1) break;
        if (*(unsigned char*)((char*)r5 + 0x3ab) == *(unsigned char*)((char*)r1 + 0x1d9))
            if (*(unsigned char*)((char*)r1 + 0x1d8) == 0)
                return r1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b1384
extern "C" {
void func_ov002_020b1384(char* c){
    extern int _ZNK10dBgCh_Actr8IsOnWallEv(void*);
    extern int _ZNK10dBgCh_Actr13GetWallResultEv(void*);
    extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void*, void*);
    extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void*, int, int, short);

  if(_ZNK10dBgCh_Actr8IsOnWallEv(c+0x1ac)==0) return;
  int v[3];
  void* w=(void*)_ZNK10dBgCh_Actr13GetWallResultEv(c+0x1ac);
  _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)w+4, v);
  *(short*)(c+0x94)=_ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(c, v[0], v[2], *(short*)(c+0x94));
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b13e0
extern "C" {
void func_ov002_020b13e0(char* self){
    extern unsigned char DecIfAbove0_Byte(unsigned char* p);
    extern unsigned short DecIfAbove0_Short(unsigned short* p);
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(char* c);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* self, void* cyl);
    extern int LenVec3(void* v);
    extern void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
    extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* p);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(char* w);
    extern char* _ZNK10dBgCh_Actr14GetFloorResultEv(char* w);
    extern int SurfaceInfo_TestFlag0x20(void* p);
    extern void func_02012694(int a, void* p);
    extern void _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(int a, int b, int c);
    extern void _ZN10dBgCh_Actr18StopDetectingWaterEv(char* w);

  DecIfAbove0_Byte((unsigned char*)(self+0x3aa));
  if (DecIfAbove0_Short((unsigned short*)(self+0x3a8)) == 1) {
    _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
    return;
  }
  _ZN8dActor_c9UpdatePosEP5dCc_c(self, self+0x178);
  if (LenVec3(self+0xa4) > *(int*)(self+0x1c4))
    dBgCh_Actr_UpdateContinuous_Veneer(self+0x1ac);
  else
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(self+0x1ac);
  if (!_ZNK10dBgCh_Actr10IsOnGroundEv(self+0x1ac)) return;
  if (SurfaceInfo_TestFlag0x20(_ZNK10dBgCh_Actr14GetFloorResultEv(self+0x1ac) + 4) == 0) return;
  if (*(int*)(self+0xa8) > 0) return;
  func_02012694(0xe2, self+0x74);
  _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(*(int*)(self+0x5c), *(int*)(self+0x60), *(int*)(self+0x64));
  *(int*)(self+0x98) = 0;
  *(int*)(self+0x9c) = -0x800;
  *(int*)(self+0xa0) = -0x5000;
  _ZN10dBgCh_Actr18StopDetectingWaterEv(self+0x1ac);
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b14d8
extern "C" {
void func_ov002_020b14d8(char *c)
{
    struct BF3ae {
        unsigned char b0 : 1;
    unsigned char : 4;
    unsigned char sel : 3;
    };
    extern int IDENTITY_MATRIX4X3[];
    extern void Matrix4x3_FromRotationY(void *m, int angle);
    extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        void *c, void *sm, void *mtx, int a, int b, unsigned int u);

    int drop;
    int sel;

    Matrix4x3_FromRotationY(c + 0xe4, *(s16*)(c + 0x8e));
    *(int*)(c + 0x108) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x10c) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x110) = *(int*)(c + 0x64) >> 3;
    *(struct Matrix4x3*)(c + 0x120) = *(struct Matrix4x3*)(c + 0xe4);
    *(struct Matrix4x3*)(c + 0x368) = *(struct Matrix4x3*)IDENTITY_MATRIX4X3;
    *(int*)(c + 0x38c) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x390) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x394) = *(int*)(c + 0x64) >> 3;

    drop = *(int*)(c + 0xb0) & 0x40000;
    drop = drop != 0;
    if (drop) return;

    if (((struct BF3ae*)(c + 0x3ae))->b0 == 0) return;

    if (*(int*)(c + 0x3a0) == 1) {
        if (*(unsigned char*)(c + 0x3b0) != 0) return;
    }

    sel = ((struct BF3ae*)(c + 0x3ae))->sel;
    if (sel == 0) {
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x150, c + 0x368, 0x50000, 0x1f4000, 0xf);
        *(int*)(c + 0xb8) = 0x3e800;
        return;
    }
    if (sel != 1) return;

    {
        int t = *(int*)(c + 0x60) - *(int*)(c + 0x398);
        *(int*)(c + 0xb8) = (t + 0x50000) >> 3;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x150, c + 0x368, 0x50000, t + 0x28000, 0xf);
    }
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b1674
extern "C" {
void func_ov002_020b1674(char* c, char* p){
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
    extern int func_02012694(int, void*);
    extern int GiveCoins(int, int);
    extern int _ZN6Player4HealEi(void*, int);

  *(short*)(c+0x3a8)=0;
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
  func_02012694(0x1c, c+0x74);
  GiveCoins(*(unsigned char*)(p+0x6d8), 5);
  _ZN6Player4HealEi(p, 0x500);
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b16c4
#define LAUNDER(p) ((int)(p))
extern "C" {
void func_ov002_020b16c4(void *cc, void *pp)
{
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *o);
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3 *pos);
    extern int GiveRedCoins(int i, int amt);
    extern s8 NumRedCoins(void);
    extern void _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(void *o, const Vector3 *v,
                                                        unsigned int n, int b,
                                                        unsigned short t, void *a);
    extern unsigned int func_02012790(unsigned int a);
    extern void GiveCoins(int idx, int amount);
    extern void _ZN6Player4HealEi(void *p, int amt);
    extern void *func_ov002_020b1328(void *r5);
    extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int b,
                                                              const Vector3 *pos, const void *r,
                                                              int e, int f);
    extern void _ZN9PowerStar13AddStarMarkerEv(void *o);

    char *c = (char *)cc;
    char *pl = (char *)pp;
    volatile Vector3 a;
    Vector3 w;
    Vector3 b;
    char *ps;
    char *st;
    Vector3 *q;
    int x, y, z;
    u8 v;

    *(short *)(c + 0x3a8) = 0;
    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
    if (*(u8 *)(pl + 0x706) != 0)
        _ZN5Sound9PlayBank3EjRK7Vector3(0x12, (const Vector3 *)(c + 0x74));
    else
        _ZN5Sound9PlayBank3EjRK7Vector3(0x11, (const Vector3 *)(c + 0x74));

    v = *(u8 *)(c + 0x3ab);
    if (v != 0 && v <= 7) {
        GiveRedCoins(*(u8 *)(pl + 0x6d8), 1);
        x = *(int *)(c + 0x5c);
        a.x = x;
        y = *(int *)(c + 0x60);
        a.y = y;
        z = *(int *)(c + 0x64);
        a.z = z;
        y += 0x64000;
        a.y = y;
        b.x = x;
        b.y = y;
        b.z = z;
        _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(c, &b, NumRedCoins(), 0, 0, 0);
        func_02012790(NumRedCoins() + 0x2f);
    }

    GiveCoins(*(u8 *)(pl + 0x6d8), 2);
    _ZN6Player4HealEi(pl, 0x200);
    if (NumRedCoins() != 8) return;
    ps = (char *)func_ov002_020b1328(c);
    if (ps == 0) return;
    q = (Vector3 *)LAUNDER(ps + 0x5c);
    x = q->x;
    *(volatile int *)&w.x = x;
    y = q->y;
    *(volatile int *)&w.y = y;
    z = q->z;
    *(volatile int *)&w.z = z;
    y += 0x78000;
    *(volatile int *)&w.y = y;
    st = (char *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0xb2, *(u8 *)(c + 0x3ab) | 0x40, &w, 0, *(s8 *)(ps + 0xcc), -1);
    if (st == 0) return;
    if (*(s8 *)(c + 0xcc) != *(s8 *)(st + 0xcc))
        *(s8 *)(st + 0xcc) = -1;
    _ZN9PowerStar13AddStarMarkerEv(st);
    *(unsigned short *)LAUNDER(st + 0x4a2) |= 0x1000;
    *(int *)(st + 0x434) = *(int *)(ps + 4);
    *(u8 *)LAUNDER(ps + 0x1db) |= 4;
}
}
#undef LAUNDER

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b1884
extern "C" {
void func_ov002_020b1884(char* c, char* r4){
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(char*);
    extern int _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, void*);
    extern void GiveCoins(int idx, int amount);
    extern int _ZN6Player4HealEi(char*, int);

  *(short*)(c+0x3a8)=0;
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
  if(*(unsigned char*)(r4+0x706))
    _ZN5Sound9PlayBank3EjRK7Vector3(0x12, c+0x74);
  else
    _ZN5Sound9PlayBank3EjRK7Vector3(0x11, c+0x74);
  GiveCoins(*(unsigned char*)(r4+0x6d8), 1);
  _ZN6Player4HealEi(r4, 0x100);
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b18f0
extern "C" {
void func_ov002_020b18f0(char* c)
{
    struct Vector3_16;
    extern signed char data_0209f2f8;
    extern short data_0209f358[];
    extern int SublevelToLevel(int);
    extern int _ZN5Event6GetBitEj(unsigned int b);
    extern void _ZN5Event6SetBitEj(unsigned int b);
    extern void _ZN9PowerStar13AddStarMarkerEv(char* c);
    extern char* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, struct Vector3* v, struct Vector3_16* rot, int e, int f);

    char* r;
    struct Vector3 vec;
    struct Vector3* p;
    int y;
    if (_ZN5Event6GetBitEj(0x1f)) return;
    if (SublevelToLevel(data_0209f2f8) >= 0xf) return;
    if (c == 0) return;
    if (data_0209f358[*(unsigned char*)(c + 0x6d8)] < 0x64) return;
    p = (struct Vector3*)(c + 0x5c);
    vec.x = p->x;
    y = p->y;
    vec.y = y;
    vec.z = p->z;
    vec.y = y + 0x12c000;
    r = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, 0x20, &vec, 0, *(signed char*)(c + 0xcc), -1);
    if (r == 0) return;
    _ZN5Event6SetBitEj(0x1f);
    _ZN9PowerStar13AddStarMarkerEv(r);
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b19dc
extern "C" {
int func_ov002_020b19dc(char *self)
{
    /* func_ov002_020b19dc at 0x020b19dc
     *
     * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
     */

    extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
    extern void func_ov002_020b16c4(char *self, char *p);
    extern void func_ov002_020b1674(char *self, char *p);
    extern void func_ov002_020b1884(char *self, char *p);

    unsigned int id = *(unsigned int *)(self + 0x19c);
    if (id != 0) {
        char *p = (char *)_ZN8dActor_c10FindWithIDEj(id);
        if (p != 0) {
            if (*(int *)(self + 0x198) & 0x400000) {
                *(unsigned short *)(self + 0x3a8) = 0;
                if (*(int *)(self + 0x3a0) == 1)
                    func_ov002_020b16c4(self, p);
                else if (*(int *)(self + 0x3a0) == 2)
                    func_ov002_020b1674(self, p);
                else
                    func_ov002_020b1884(self, p);
                return 1;
            }
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b1a60
extern "C" {
void func_ov002_020b1a60(void* c) {
    extern void _ZN7fBase_c18MarkForDestructionEv(void*);

  {
    unsigned short* n = (unsigned short*)(((int)c + 0x3a8));
    *n += 1;
  }
  {
    int* s = (int*)(((int)*(void**)((char*)c+0x39c) + 0x5c));
    *(int*)((char*)c+0x5c) = s[0];
    *(int*)((char*)c+0x60) = s[1];
    *(int*)((char*)c+0x64) = s[2];
  }
  {
    int* g = (int*)(((int)c + 0x60));
    *g += 0xc8000;
  }
  if (*(unsigned short*)((char*)c+0x300+0xa8) < 0x41) return;
  _ZN7fBase_c18MarkForDestructionEv(c);
  *(unsigned short*)((char*)c+0x300+0xa8) = 0;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b1ad4
extern "C" {
void func_ov002_020b1ad4(char *self)
{
    struct Bits3ae { u8 lo : 2; u8 field : 3; u8 hi : 3; };
    extern signed char data_0209f2f8;
    extern char *_ZN8dActor_c13ClosestPlayerEv(char *self);
    extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
    extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(char *self);
    extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char *self);
    extern int *_ZNK10dBgCh_Actr14GetFloorResultEv(char *self);
    extern int SurfaceInfo_TestFlag0x20(int *p);
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3 *pos);
    extern void func_ov002_020b13e0(char *self);
    extern void func_ov002_020b1384(char *c);

    char *player;
    int r6;
    int v;

    if (((struct Bits3ae *)(self + 0x3ae))->field == 0) {
        player = _ZN8dActor_c13ClosestPlayerEv(self);
        if (player == 0) return;
        r6 = (data_0209f2f8 == 0xb) ? 0x258000 : 0x4b0000;
        if (Vec3_Dist((Vector3 *)(self + 0x5c), (Vector3 *)(player + 0x5c)) > r6) return;
        v = *(int *)(player + 0x98);
        if (v < 0x14000) v = 0x14000;
        else if (v > 0x28000) v = 0x28000;
        if (data_0209f2f8 == 0xb) v <<= 1;
        *(int *)(self + 0x98) = v;
        *(int *)(self + 0xa8) = 0x14000;
        ((struct Bits3ae *)(int)(self + 0x3ae))->field++;
        _ZN10dBgCh_Actr13SetLimMovFlagEv(self + 0x1ac);
        *(short *)(self + 0x3a8) = 0x1c2;
    }
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(self + 0x1ac)) {
        if (SurfaceInfo_TestFlag0x20(_ZNK10dBgCh_Actr14GetFloorResultEv(self + 0x1ac) + 1) == 0) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x52, (const Vector3 *)(self + 0x74));
            *(int *)(self + 0xa8) = 0x19000;
        }
    }
    func_ov002_020b13e0(self);
    func_ov002_020b1384(self);
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b1bfc
extern "C" {
void func_ov002_020b1bfc(char* c) {
    extern void func_ov002_020b13e0(char* c);
    extern void func_ov002_020b1384(char* c);
    extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char* c);
    extern void* _ZNK10dBgCh_Actr14GetFloorResultEv(char* c);
    extern int SurfaceInfo_TestFlag0x20(void* p);
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned a, void* v);
    extern int _ZN8dActor_c13DistToCPlayerEv(char* c);
    extern int _ZN8dActor_c18HorzAngleToCPlayerEv(char* c);
    extern int _ZN8dActor_c18HorzAngleToFPlayerEv(char* c);

    func_ov002_020b13e0(c);
    func_ov002_020b1384(c);
    if (!_ZNK10dBgCh_Actr13JustHitGroundEv(c+0x1ac)) return;
    if (SurfaceInfo_TestFlag0x20((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(c+0x1ac)+4) != 0) return;
    _ZN5Sound9PlayBank3EjRK7Vector3(0x52, c+0x74);
    if (*(int*)(c+0x98) == 0) {
        *(int*)(c+0x98) = 0x13000;
        *(short*)(c+0x3a8) = 0x12c;
    } else {
        if (*(int*)(c+0x98) > 0x13000) *(int*)(c+0x98) = 0x13000;
    }
    *(int*)(c+0xa8) = 0x23000;
    if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x4b0000) {
        *(short*)(c+0x94) = _ZN8dActor_c18HorzAngleToCPlayerEv(c) + 0x8000;
    } else {
        *(short*)(c+0x94) = _ZN8dActor_c18HorzAngleToFPlayerEv(c);
    }
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b1cc0
extern "C" {
void func_ov002_020b1cc0(char *c)
{
    typedef struct BF { u8 f0 : 2; u8 f1 : 3; u8 f2 : 3; } BF;
    extern int data_ov002_020ff078[];
    void func_ov002_020b13e0(char *c);
    void func_ov002_020b1384(char *c);
    char *_ZN8dActor_c10FindWithIDEj(int id);
    void _ZN5Sound9PlayBank3EjRK7Vector3(int a, void *v);
    int _ZNK10dBgCh_Actr10IsOnGroundEv(char *w);
    char *_ZNK10dBgCh_Actr14GetFloorResultEv(char *w);
    int func_02037e58(char *s);
    void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(char *s, Vector3 *v);
    int _ZNK10dBgCh_Actr13JustHitGroundEv(char *w);
    int SurfaceInfo_TestFlag0x20(char *s);
    int Vec3_HorzLen(void *v);
    void _ZN8dActor_c28UpdatePosWithHorzSpeedAndAngEv(char *c);
    int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
    int _ZN4cstd4fdivEii(int a, int b);

    Vector3 n;
    char *fl;
    int attr;
    int t;
    int v;
    int r3v;
    BF *bf;
    char *p;

    func_ov002_020b13e0(c);
    func_ov002_020b1384(c);
    p = _ZN8dActor_c10FindWithIDEj(*(int *)(c + 0xd4));
    if (p != 0) {
        t = *(u16 *)(p + 0xc);
        t = t == 0x4f;
        if (t != false) {
            if (*(int *)(c + 0x60) <= *(int *)(p + 0x60)) {
                *(int *)(c + 0x60) = *(int *)(p + 0x60);
                _ZN5Sound9PlayBank3EjRK7Vector3(0x52, c + 0x74);
                bf = (BF *)(((int)c + 0x3ae));
                bf->f1++;
                v = ((BF *)(c + 0x3ae))->f1;
                if (v < 3u)
                    r3v = ((3 - v) << 12) / 3;
                else
                    r3v = 0x555;
                *(int *)(c + 0xa8) = -*(int *)(c + 0xa8) * r3v / 0x1000;
                *(int *)(((int)c + 0x98)) >>= 1;
                if (*(int *)(c + 0xa8) >= -*(int *)(c + 0x9c))
                    return;
                *(int *)(c + 0xa8) = 0;
                *(int *)(c + 0x98) = 0;
                *(int *)(c + 0x9c) = 0;
                *(int *)(c + 0x3a4) = 4;
                return;
            }
        }
    }

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1ac) == 0)
        return;

    fl = _ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x1ac);
    attr = func_02037e58(fl + 4);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fl + 4, &n);

    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1ac) != 0) {
        if (SurfaceInfo_TestFlag0x20(_ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x1ac) + 4) == 0) {
            if (*(u16 *)(c + 0x3a8) > 0xf000)
                *(u16 *)(c + 0x3a8) = 0xf;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x52, c + 0x74);
            bf = (BF *)(((int)c + 0x3ae));
            bf->f1++;
            v = ((BF *)(c + 0x3ae))->f1;
            if (v < 3u)
                r3v = ((3 - v) << 12) / 3;
            else
                r3v = 0x555;
            *(int *)(c + 0xa8) = -*(int *)(c + 0xa8) * r3v / 0x1000;
            *(int *)(((int)c + 0x98)) >>= 1;
        }
    }

    if (n.x != 0 || n.z != 0) {
        int m6 = data_ov002_020ff078[attr];
        int *pa = (int *)(((int)c + 0xa4));
        *pa += n.x * m6;
        *(int *)(((int)c + 0xac)) += n.z * m6;
        *(int *)(c + 0x98) = Vec3_HorzLen(pa);
        if (*(int *)(c + 0x98) > 0x1c000) {
            *(int *)(c + 0x98) = 0x1c000;
            _ZN8dActor_c28UpdatePosWithHorzSpeedAndAngEv(c);
        }
        *(u16 *)(c + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(c + 0xa4), *(int *)(c + 0xac));
    }

    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1ac) != 0)
        return;
    if (SurfaceInfo_TestFlag0x20(_ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x1ac) + 4) != 0)
        return;

    if (n.y != 0) {
        int v1 = (int)(((long long)n.x * *(int *)(c + 0xa4) + 0x800) >> 12);
        int v2 = (int)(((long long)n.z * *(int *)(c + 0xac) + 0x800) >> 12);
        *(int *)(c + 0xa8) = -(_ZN4cstd4fdivEii(v1 + v2, n.y) + 0x8000);
    }

    if ((unsigned)(attr - 1) <= 1) {
        *(int *)(c + 0x98) = 0;
    } else {
        *(int *)(c + 0x98) = *(int *)(c + 0x98) * 0xc00 / 0x1000;
    }

    if (*(int *)(c + 0x98) >= 0x1000)
        return;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0x3a4) = 4;
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b2070
extern "C" {
void func_ov002_020b2070(char* c){
    extern void DecIfAbove0_Short(void*);
    extern void DecIfAbove0_Byte(void*);
    extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);

  DecIfAbove0_Short(c+0x3a8);
  DecIfAbove0_Byte(c+0x3aa);
  if(*(unsigned short*)(c+0x3a8)==1)
    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b20b4
extern "C" {
void func_ov002_020b20b4(char* c){
    struct BF3ae {
        unsigned char b0 : 1;
    unsigned char : 4;
    unsigned char sel : 3;
    };
    extern void func_ov002_020b1008(void* c);
    extern int _ZN5Event6GetBitEj(unsigned int b);
    extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void* start);

  func_ov002_020b1008(c);
  if (((struct BF3ae*)(c+0x3ae))->b0) return;
  if (_ZN5Event6GetBitEj(*(unsigned char*)(c+0x3ab)) == 0) return;
  ((struct BF3ae*)(int)((int)c + 0x3ae))->b0 = 1;
  *(int*)(c+0x3a4) = 4;
  *(int*)(int)((int)c + 0x190) &= ~1;
  char* found = (char*)_ZN8dActor_c15FindWithActorIDEjPS_(0xa, 0);
  if (found) {
    *(short*)(c+0x3a8) = *(unsigned short*)(found+0x32a);
  } else {
    *(short*)(c+0x3a8) = 0xfa;
  }
}
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b2150
extern "C" {
void func_ov002_020b2150(char* c){
    extern int _ZNK10dBgCh_Actr8IsOnWallEv(void*);
    extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);

  func_ov002_020b13e0(c);
  if(_ZNK10dBgCh_Actr8IsOnWallEv(c+0x1ac)) *(int*)(c+0x98)=0;
  if(_ZNK10dBgCh_Actr10IsOnGroundEv(c+0x1ac)){
    *(int*)(c+0xa8)=0;
    *(int*)(c+0x9c)=0;
    *(int*)(c+0x3a4)=4;
  }
}
}

/* -------------------------------------------------------------------------- */
/* Particle::System::NewSimple takes its coordinates as Fix12<int>, which this
 * tree still spells as a plain s32, so the call is reached through its mangled
 * name. The declaration must carry C linkage: a C++-linkage prototype of that
 * identifier mangles a second time and names a symbol nothing defines. */
extern "C" void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12i x, Fix12i y, Fix12i z);

// @symbol _ZN8daCoin_c16CleanupResourcesEv
s32 daCoin_c::CleanupResources()
{
    /* daCoin_c::CleanupResources -- vtable slot 3. Releases the files this coin's own
     * type loaded, gives up its star-tracking slot, decrements the live-coin count
     * on the puzzle manager that spawned it, and -- unless it is disappearing on a
     * timer -- puts up the collection sparkle a little above itself. */
    extern char data_ov002_0210d9a8;
    extern char *data_ov002_020ff06c[];
    extern char *data_ov002_020ff060[];

    dActor_c *o;
    int b = (int)(actorID == 0x121);
    if (b != 0) ((SharedFilePtr *)&data_ov002_0210d9a8)->Release();
    if (mCoinType == 2) {
        ((SharedFilePtr *)data_ov002_020ff06c[mCoinType])->Release();
        ((SharedFilePtr *)data_ov002_020ff060[mCoinType])->Release();
    }
    UntrackStar(mTrackStarID);
    o = dActor_c::FindWithID(mPuzzleManagerID);
    if (o != 0) {
        int b2 = (int)(o->actorID == 0x4f);
        if (b2 != 0) {
            /* +0xd6 is the puzzle manager's own live-coin count; no header
               describes that class yet, so the offset stays raw. */
            if (*(unsigned char *)((char *)o + 0xd6) != 0) {
                unsigned char *p = (unsigned char *)((int)o + 0xd6);
                *p = *p - 1;
            }
        }
    }
    /* THE ROM READS THIS FIELD UNSIGNED (`ldrh`, not `ldrsh`), and the legacy C
       form spelled it `*(unsigned short*)(c + 0x3a8)`. include/daCoin_c.h types it
       s16, so the cast here is what keeps the load faithful; the header may
       simply have the sign wrong, but changing it is a separate question from
       this migration. Dropping the cast is the one word this function misses. */
    if ((u16)mDisappearTimer != 0) return 1;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY + 0x28000, mPosZ);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8daCoin_c6RenderEv
extern "C" {
extern "C" int _ZN11CommonModel6RenderEPK7Vector3(void*, void*);
}
int daCoin_c::Render()
{
    struct Flags { unsigned char b0:1; };

  int f;
  int b;
  if (!((struct Flags*)((char*)&mCoinFlags))->b0) return 1;
  f = mFlags;
  b = (f & 0x40000) != 0;
  if (b) return 1;
  {
    unsigned short x = mDisappearTimer;
    if (x < 0x2d && (x & 1)) return 1;
  }
  if (!(f & 0x10))
    _ZN11CommonModel6RenderEPK7Vector3(((char*)this)+0xd8, 0);
  else
    _ZN11CommonModel6RenderEPK7Vector3(((char*)this)+0x114, 0);
  return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8daCoin_c8BehaviorEv
extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, char *pos);
extern int LenVec3(char *v);
}
int daCoin_c::Behavior()
{
    /* _ZN8daCoin_c8BehaviorEv @ 0x020b2324 (ov002, size 0x1e4)
     * daCoin_c main behavior: pickup-flag sound, magnet flag, physics helpers,
     * state PMF-table dispatch, collision clear/update paths.
     */
    struct C {};
    typedef void (C::*PMF)();
    extern PMF data_ov002_0210dc70[];
    extern unsigned char data_0209f2d8;

    if ((unsigned int)(mCoinFlags << 0x1e) >> 0x1f) {
        *(unsigned char *)(((int)((char *)this) + 0x3ae)) &= ~2;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x30, ((char *)this) + 0x74);
    }
    if ((int)(actorID == 0x122) != 0) {
        if ((unsigned int)(mCoinFlags << 0x1f) >> 0x1f)
            mAreaId = -1;
    }
    if (func_ov002_020b10a0(((char *)this)) != 0) return 1;
    *(short *)(((int)((char *)this) + 0x8e)) += 0xc00;
    if (func_ov002_020b12ec(((char *)this)) != 0) {
        func_ov002_020b14d8(((char *)this));
        mdCc_c.Clear();
        return 1;
    }
    func_ov002_020b10e4(((char *)this));
    mEatingPlayer = 0;
    if (func_ov002_020b19dc(((char *)this)) != 0) return 1;
    (((C *)((char *)this))->*data_ov002_0210dc70[mBehaviorType])();
    if ((int)(data_0209f2d8 == 1) == 0 && (int)((mFlags & 8) != 0) != 0) {
        mdCc_c.Clear();
        if (mNoClsnTimer == 0 && LenVec3((char *)&mCamSpacePosX) < 0x64000) {
            if (mCoinType != 1 || mInBrickBlock == 0)
                mdCc_c.Update();
        }
    } else {
        func_ov002_020b14d8(((char *)this));
        mdCc_c.Clear();
        if (mNoClsnTimer == 0) {
            if (mCoinType != 1 || mInBrickBlock == 0)
                mdCc_c.Update();
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8daCoin_c13InitResourcesEv
extern "C" {
extern int SublevelToLevel(int i);
extern void SetStarMarker(int i, void* actor, int v2);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, void* actor, s32 f1, s32 f2, u32 a, u32 b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, s32 f1, s32 f2, void* v, s32 f3);
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void* thiz);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void* thiz);
}
s32 daCoin_c::InitResources()
{
    /* daCoin_c::InitResources -- vtable slot 0. Sets up one coin from its spawn
     * parameter: mBehaviorType is the low nibble of param1 and selects the physics
     * (bounce height, gravity, terminal velocity) and the flag bits in mCoinFlags; the
    * actor ID then selects which of the three coin types it is (0x121 red, 0x122
     * blue, else yellow), which decides the models loaded and, for a red coin,
     * whether it claims a star-tracking slot. The last third builds the two models,
     * the shadow cylinder, the actor collider and the mesh collider, then sets the
     * disappear and no-collision timers.
     *
     * FOUR SITES KEEP RAW OFFSETS, and each one is measured, not left over:
     *
     *   mCoinFlags through `(int)c`  -- every read-modify-write of the flag byte is
     *       spelled `*(u8*)(((int)c + 0x3ae))` in the ROM's codegen. Spelled as the
     *       member, the function changes size. The plain `*(u8*)(c + 0x3ae)` sites
     *       DO convert, and have; the launder is per-site, not per-field.
    *   *(u16*)(c + 0x3a8) = 0xffffu  -- the same field the s16 reads reach as
     *       mDisappearTimer, but this one store only reproduces through the raw
     *       unsigned spelling.
     *   *(Matrix4x3*)(c + 0x368) = IDENTITY_MATRIX4X3  -- this is mShadowMat, a Matrix4x3.
     *       As a struct assignment C++ scalarizes the copy and the function changes
     *       size; as a 48-byte blob copy it is the memcpy the ROM has.
    *   *(s32*)(((int)c + 0x190))  -- inside the dCcAc_c sub-object at 0x178, whose
     *       own header does not name that word yet.
     *
     * `#pragma opt_common_subs off` is inherited from the C form and still load-
     * bearing: the ROM re-issues loads this compiler would otherwise CSE. */
    #pragma opt_common_subs off

    #define false 0
    #define true 1


    typedef struct { void* sfp; void* bmd; } FileEntry;
    extern Matrix4x3 IDENTITY_MATRIX4X3;
    extern s8 data_0209f2f8;
    extern u8 data_0209f220;
    extern s32 data_0209f40c[];
    extern u8 data_0209f2d8;
    extern FileEntry* data_ov002_020ff06c[];
    extern FileEntry* data_ov002_020ff060[];
    extern void* data_ov002_0210d9a8;

    char* c = (char*)this;
    s32 r5;
    s32 r4;
    s32 t;
    s8 i;
    s32 j;

    mCoinFlags = 0;
    r5 = 0x64000;
    t = (s32)param1 & 0xf;
    mBehaviorType = t;
    r4 = 0x40000;
    t = mBehaviorType;
    if (t >= 9) {
        t = 1;
        mBehaviorType = t;
    }
    t = mBehaviorType;
    if (t == 8) {
        mVertSpeed = 0x14000;
        mVertAccel = -0x4000;
        mTerminalVelocity = -0x37000;
        *(u8*)(((int)c + 0x3ae)) =
            (*(u8*)(((int)c + 0x3ae)) & ~0xe0) | 0x40;
        goto shared140;
    }
    if (t == 1 || t == 7) {
        goto case17;
    }
    if (t == 5) {
        goto case5;
    }
    /* default */
    mVertSpeed = 0x24000;
    *(u8*)(((int)c + 0x3ae)) |= 2;
    goto block_11;
case5:
    {
        u32 flags = param1;
        if (flags & 8) {
            mPrevAngleY = (s16)(((flags & 0x70) << 8) + 0x8000);
        } else {
            mPrevAngleY = (s16)((flags & 0x70) << 8);
        }
    }
block_11:
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x37000;
    *(u8*)(((int)c + 0x3ae)) &= ~0xe0;
    goto shared140;
case17:
    *(u8*)(((int)c + 0x3ae)) =
        (*(u8*)(((int)c + 0x3ae)) & ~0xe0) | 0xe0;
    mFloorPosY = mPosY - 0x1f4000;
    if (mBehaviorType == 7) {
        r5 = 0x32000;
        r4 = 0x28000;
    }
shared140:;

    *(Matrix4x3*)(c + 0x368) = IDENTITY_MATRIX4X3;

    mTrackStarID = -1;
    mSpawnFilter = 0xff;

    {
        u16 h;
        int b;
        h = actorID;
        b = h;
        b = (b == 0x121);
        if (b) {
            mSpawnFilter = (u8)((param1 >> 4) & 7);
            Model::LoadFile(*(SharedFilePtr *)&data_ov002_0210d9a8);
            mCoinType = 1;
            if (SublevelToLevel(data_0209f2f8) == 0x13 ||
                mSpawnFilter == data_0209f220) {
                if (GetBitInDeathTable() == 0) {
                    for (i = 0; i < 0xc; i = (s8)(i + 1)) {
                        if (data_0209f40c[i] == 0) {
                            SetStarMarker(i, this, 4);
                            mTrackStarID = i;
                            break;
                        }
                    }
                }
            }
        } else {
            int b2;
            b2 = h;
            b2 = (b2 == 0x122);
            if (b2) {
                mSpawnFilter = (u8)((param1 >> 4) & 7);
                mCoinType = 2;
            } else {
                mCoinType = 0;
            }
        }
    }

    j = mCoinType;
    if (j < 2) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel1, data_ov002_020ff06c[j]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel2, data_ov002_020ff060[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
    } else {
        Model::LoadFile(*(SharedFilePtr *)data_ov002_020ff06c[j]);
        Model::LoadFile(*(SharedFilePtr *)data_ov002_020ff060[mCoinType]);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel1, data_ov002_020ff06c[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mCommonModel2, data_ov002_020ff060[mCoinType]->bmd, 1, 1) == 0) {
            return 0;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv(&mShadowModel) == 0) {
        return 0;
    }

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCc_c, this, r5, r4, 0x100002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x3c000, 0x3c000, 0, 0);
    _ZN10dBgCh_Actr13SetLimMovFlagEv(&mWithMeshClsn);
    _ZN10dBgCh_Actr19StartDetectingWaterEv(&mWithMeshClsn);

    t = mBehaviorType;
    if (t == 8) {
        mDisappearTimer = 0x2d;
        *(s32*)(((int)c + 0x190)) |= 1;
    } else {
        if (t == 6) {
            int b3;
            b3 = data_0209f2d8;
            b3 = (b3 == 1);
            if (b3 == false) {
                *(u16*)(c + 0x3a8) = 0xffffu;
                *(s32*)(((int)c + 0x190)) |= 1;
                goto after438;
            }
        }
        mDisappearTimer = 0xd2;
    after438:;
    }

    *(u8*)(((int)c + 0x3ae)) =
        (*(u8*)(((int)c + 0x3ae)) & ~1) | 1;

    t = mBehaviorType;
    if (t == 1 || t == 7) {
        goto case17b;
    }
    if (t == 5) {
        mNoClsnTimer = 0;
    } else {
        mNoClsnTimer = 0xf;
    }
    goto switch2end;
case17b:
    mNoClsnTimer = 0;
    if (mCoinType == 2 && (u32)mSpawnFilter < 8) {
        *(u8*)(((int)c + 0x3ae)) &= ~1;
        *(s32*)(((int)c + 0x190)) |= 1;
    }
switch2end:;

    *(u8*)(((int)c + 0x3ae)) &= ~0x1c;
    mPuzzleManagerID = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8daCoin_c13OnTurnIntoEggER6Player
void daCoin_c::OnTurnIntoEgg(Player &player)
{
    /* Reconstructed C++ method, vtable slot 19. Each path delegates the coin
     * payout to a helper and returns no value, matching the shared actor hook's
     * void contract. Calls and epilogues alone do not identify an original type. */

    char *c = (char *)this;
    char *p = (char *)&player;
    int state = mCoinType;

    if (state == 1) {
        mPosY += 0x50000;
        func_ov002_020b16c4(c, p);
    } else if (state == 2) {
        func_ov002_020b1674(c, p);
    } else {
        func_ov002_020b1884(c, p);
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN8daCoin_c13OnYoshiTryEatEv
s32 daCoin_c::OnYoshiTryEat()
{
    return 4;
}

