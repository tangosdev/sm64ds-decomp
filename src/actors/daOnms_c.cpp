//cpp
/* daOnms_c -- the rolling crush box. ov092 0x02130f00..0x02132018,
 * twenty-two functions.
 *
 * The cartridge spells the class 8daOnms_c: _ZTS8daOnms_c at 0x02132288.
 * _ZTI8daOnms_c at 0x021322a0 reads [__si_class_type_info, 0x02132288,
 * _ZTI10dBgActor_c], so the single base is dBgActor_c, and the word before
 * _ZTV8daOnms_c (0x021322d0) is that _ZTI. The class was coined ToxBox
 * before the ROM name was read (factory alias ToxBox_Spawn).
 * daOnms_c_classInit at 0x02132018 abuts this run and stays in
 * src/d_a_onms.cpp.
 *
 * #pragma defer_codegen off emits .text in source order, so this file is
 * ROM-ascending. One out-of-line destructor is the key function: it emits
 * D1 (0x02130f00) then D0 (0x02130f5c) and anchors _ZTV8daOnms_c. The D2
 * it also emits has no home in ov092 (manifest: deadstrip). The factory is
 * not instantiated here.
 *
 * daOnms_c.h is included first so common.h's flat Matrix4x3 stands.
 * These bodies copy that matrix as twelve words and do not use .t.
 */

#pragma defer_codegen off

#include "daOnms_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

typedef void (daOnms_c::*ToxBoxState)();
struct ToxBoxStateEntry { ToxBoxState state; };
typedef struct { int x, y, z; } Vec3;
typedef Matrix4x3 Mtx43;

#define LA(p) (p)

extern ToxBoxStateEntry data_ov092_02132568[];
extern SharedFilePtr data_ov092_02132540;
extern SharedFilePtr data_ov092_02132548;

extern "C" {
void _ZN7fBase_c18MarkForDestructionEv(void *);
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
int func_ov002_020de328(void *player);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *c);
void dBgCh_Actr_UpdateContinuous_Veneer(void);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
int _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *v, int f);
void func_02012694(unsigned int id, const Vector3 *v);
void _ZN8dActor_c13LandingDustAtER7Vector3b(void *self, Vector3 *v, int b);
void _ZN8dActor_c14TriplePoofDustEv(void *self);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3 &pos);
int func_02037e38(void *p);
int func_02037e84(void *p);
void func_0203568c(void *p, int v);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int b, int cc, int d);
s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
s16 data_02082214[];
s8 data_ov092_0213208c[];
int data_ov092_02132074[];
int data_ov092_02132080[];
dActor_c *_ZN8dActor_c10FindWithIDEj(unsigned int id);
int _ZN8dActor_c24BumpedUnderneathByPlayerER6Player(void *thiz, void *p);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int range, int offset);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, int a1, int a2, int a3, int sp0, int sp1);
void _ZN7PathPtr6FromIDEj(PathPtr *path, unsigned int id);
void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angX);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    int scale, short angY, void *clps);
void func_020393d4(int *p, int v);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset,
    int radius, int height, unsigned int flags, unsigned int vulnFlags);
extern char data_ov092_02132220;
extern char data_ov092_02132294;
extern struct Matrix4x3 data_020a0e68;
void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(
    dBgW &clsn, dActor_c *clsnActor, dBgPi &res, Vector3 &pos,
    Vector3_16 *motionAng, Vector3_16 *ang);

void func_ov092_021316d8(char *c, int a1, int a2, int a3, s16 a4);
void func_ov092_021319b0(char *c);
}


// @symbol _ZN8daOnms_cD1Ev
// @symbol _ZN8daOnms_cD0Ev
/* recovered: native complete destructor -- the compiler emits the typed
 * dCcAcPos_c/dBgCh_Actr teardown and the inline dBgActor_c base teardown.
 * D0, the deleting destructor, is the same typed teardown followed by the
 * inherited actor-heap deallocation. */
daOnms_c::~daOnms_c()
{
}

// @symbol func_ov092_02130fcc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_02130fcc(char *c)
{
    int limit;
    int cur;

    cur = *(int *)(((long long)(int)((char *)c + 0x60)));
    cur -= 0x5000;
    *(int *)(((long long)(int)((char *)c + 0x60))) = cur;
    limit = *(int *)(c + 0x55c) - 0x3e8000;
    cur = *(int *)(c + 0x60);
    if (cur >= limit)
        return;
    _ZN7fBase_c18MarkForDestructionEv(c);
}
}

// @symbol func_ov092_02131010
// Matched byte-identical (mwccarm 1.2/sp2p3). The former div-2 "mov r2,#1 vs
// str ip,[sp,#8]" ordering residual was fixed by dropping volatile on tmp and
// making tmp/eq/dust members of ONE escaped stack struct: the escape keeps the
// dead tmp stores in memory while leaving the scheduler free to hoist the
// LandingDustAt bool-arg mov above the stores, exactly as the ROM has it.
extern "C" int func_ov092_02131010(unsigned char *thiz)
{
    struct Frame { Vector3 tmp; Vector3 eq; Vector3 dust; } f;

    if (_ZN8dActor_c13ClosestPlayerEv(thiz) != 0) {
        if (((int (*)(void))func_ov002_020de328)() != 0) {
            int *p504 = (int *)LA(thiz + 0x504);
            int *p500 = (int *)LA(thiz + 0x500);
            *p504 |= 0x400000;
            *p500 |= 4;
        } else {
            int *p504 = (int *)LA(thiz + 0x504);
            int *p500 = (int *)LA(thiz + 0x500);
            *p504 &= ~0x400000;
            *p500 &= ~4;
        }
    }

    {
        s16 *p8c = (s16 *)LA(thiz + 0x8c);
        s16 *p8e = (s16 *)LA(thiz + 0x8e);
        s16 *p90 = (s16 *)LA(thiz + 0x90);
        *p8c += ((s16 *)(thiz + 0x400))[0xe0 / 2];
        *p8e += ((s16 *)(thiz + 0x400))[0xe2 / 2];
        *p90 += ((s16 *)(thiz + 0x400))[0xe4 / 2];
    }

    {
        int v = *(int *)(thiz + 0x9c);
        if (v == 0) return v;
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(thiz, 0);
    ((void (*)(void *))dBgCh_Actr_UpdateContinuous_Veneer)(thiz + 0x324);
    {
        int g = _ZNK10dBgCh_Actr10IsOnGroundEv(thiz + 0x324);
        if (g == 0) return g;
    }

    {
        short lr = 0;
        ((short *)(thiz + 0x400))[0xe4 / 2] = lr;
        ((short *)(thiz + 0x400))[0xe2 / 2] = ((short *)(thiz + 0x400))[0xe4 / 2];
        ((short *)(thiz + 0x400))[0xe0 / 2] = ((short *)(thiz + 0x400))[0xe2 / 2];
        *(int *)(thiz + 0x98) = lr;
        *(int *)(thiz + 0xa8) = lr;
    }

    f.eq.x = *(int *)(thiz + 0x5c);
    f.eq.y = *(int *)(thiz + 0x60);
    f.eq.z = *(int *)(thiz + 0x64);
    ((void (*)(void *, const Vector3 *, int))_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE)(
        thiz, &f.eq, 0x5dc000);
    func_02012694(0x46, (const Vector3 *)(thiz + 0x74));

    {
        int x, y, z;
        x = *(int *)(thiz + 0x5c);
        f.tmp.x = x;
        y = *(int *)(thiz + 0x60);
        f.tmp.y = y;
        z = *(int *)(thiz + 0x64);
        f.tmp.z = z;
        y = y - 0x78000;
        f.tmp.y = y;
        f.dust.x = x;
        f.dust.y = y;
        f.dust.z = z;
    }
    _ZN8dActor_c13LandingDustAtER7Vector3b(thiz, &f.dust, 1);

    *(int *)(thiz + 0x9c) = 0;
    return 0;
}

// @symbol func_ov092_021311b0
/* recovered: shared common types */
extern "C" void func_ov092_021311b0(void *thiz)
{
    char *c = (char *)thiz;
    volatile int saved[3];
    Vector3 v1;
    Vector3 v2;
    char *fr;

    {
        s16 *p8c = (s16 *)LA(c + 0x8c);
        s16 *p8e = (s16 *)LA(c + 0x8e);
        s16 *p90 = (s16 *)LA(c + 0x90);
        *p8c += ((s16 *)(c + 0x400))[0xe0 / 2];
        *p8e += ((s16 *)(c + 0x400))[0xe2 / 2];
        *p90 += ((s16 *)(c + 0x400))[0xe4 / 2];
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);

    saved[0] = *(int *)(c + 0x5c);
    saved[1] = *(int *)(c + 0x60);
    saved[2] = *(int *)(c + 0x64);
    ((void (*)(void *))dBgCh_Actr_UpdateContinuous_Veneer)(c + 0x324);

    if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x324) != 0) {
        _ZN8dActor_c14TriplePoofDustEv(c);
        _ZN7fBase_c18MarkForDestructionEv(c);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(const Vector3 *)(c + 0x74));
        return;
    }

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x324) == 0)
        return;

    *(int *)(c + 0x55c) = *(int *)(c + 0x60);
    fr = (char *)_ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x324);

    if (func_02037e38(fr + 4) == 9) {
        int z;
        *(int *)(c + 0x568) = 8;
        z = 0;
        *(int *)(c + 0x98) = z;
        *(int *)(c + 0xa8) = z;
        func_02012694(0x178, (const Vector3 *)(c + 0x74));
    } else if (func_02037e84(fr + 4) == 8) {
        char *o;
        int *p;
        *(int *)(c + 0x568) = 7;
        func_0203568c(c + 0x324, 0x78000);
        o = *(char **)(c + 0x320);
        p = (int *)LA(o + 0x5c);
        v1.x = p[0];
        v1.y = p[1];
        v1.z = p[2];
        ((void (*)(void *, const Vector3 *, int))_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE)(
            c, &v1, 0xff8000);
        *(int *)(c + 0x5c) = saved[0];
        *(int *)(c + 0x60) = saved[1];
        *(int *)(c + 0x64) = saved[2];
        func_02012694(0x178, (const Vector3 *)(c + 0x74));
    } else {
        char *o;
        int *p;
        _ZN8dActor_c14TriplePoofDustEv(c);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xfa, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
        _ZN7fBase_c18MarkForDestructionEv(c);
        o = *(char **)(c + 0x320);
        p = (int *)LA(o + 0x5c);
        v2.x = p[0];
        v2.y = p[1];
        v2.z = p[2];
        ((void (*)(void *, const Vector3 *, int))_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE)(
            c, &v2, 0xff8000);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(const Vector3 *)(c + 0x74));
    }
    *(int *)(c + 0x320) = 0;
}

// @symbol func_ov092_021313b0
/* recovered: shared common types */
extern "C" void func_ov092_021313b0(void *t) {
    char *c = (char*)t;
    Vector3 old;
    old.x = *(int*)(c+0x580);
    old.y = *(int*)(c+0x584);
    old.z = *(int*)(c+0x588);
    (*(int*)LA(c+0x57c))++;
    if (*(int*)(c+0x57c) >= *(int*)(c+0x578))
        *(int*)(c+0x57c) = 0;
    ((PathPtr*)(c+0x58c))->GetNode(*(Vector3*)(c+0x580), (unsigned int)*(int*)(c+0x57c));
    if (*(int*)(c+0x580) == old.x && *(int*)(c+0x588) == old.z) {
        *(int*)(c+0x568) = 1;
        return;
    }
    u16 a = (u16)Vec3_HorzAngle(&old, (Vector3*)(c+0x580));
    if (a >= 0x2000 && a < 0x6000) *(int*)(c+0x568) = 5;
    else if (a >= 0x6000 && a < 0xa000) *(int*)(c+0x568) = 3;
    else if (a >= 0xa000 && a < 0xe000) *(int*)(c+0x568) = 4;
    else *(int*)(c+0x568) = 2;
}

// @symbol func_ov092_021314d0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_021314d0(char* c) {
  if (*(unsigned char*)(c+0x574) == 3) {
    func_ov092_021313b0(c);
  } else {
    *(int*)(((int)c + 0x570)) = *(int*)(((int)c + 0x570)) + 1;
    int* arr = *(int**)(c+0x56c);
    int v = arr[*(int*)(c+0x570)];
    if (v != -1) {
      *(int*)(c+0x568) = v;
    } else {
      *(int*)(c+0x568) = arr[0];
      *(int*)(c+0x570) = 0;
    }
  }
  *(unsigned char*)(c+0x575) = (*(s16*)(c+0x92) >> 0xe) & 3;
  {
    unsigned char* f = (unsigned char*)(((int)c + 0x575));
    *f = *f | ((*(s16*)(c+0x94) >> 0xc) & 0xc);
    *f = *f | ((*(s16*)(c+0x96) >> 0xa) & 0x30);
  }
}
}

// @symbol func_ov092_02131578
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_02131578(char* c){
  *(unsigned char*)(c+0x576) = 0;
  if(*(unsigned short*)(c+0x564) == 0x14){
    func_ov092_021314d0(c);
  }
  func_ov092_021319b0(c);
}
}

// @symbol func_ov092_021315ac
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_021315ac(char *c){
  *(int*)(c+0x98)=0;
  if(!*(unsigned short*)(c+0x500+0x64)){
    struct Vector3 v;
    v.x=*(int*)(c+0x5c); v.y=*(int*)(c+0x60); v.z=*(int*)(c+0x64);
    ((void (*)(void *, const Vector3 *, int))_ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE)(
        c, &v, 0x860000);
    func_02012694(0x46, (const Vector3 *)(c + 0x74));
  }
  *(int*)(c+0x60) = *(int*)(c+0x55c) + 0x3000;
  func_ov092_02131578(c);
}
}

// @symbol func_ov092_02131620
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_02131620(void *c) {
    func_ov092_021316d8((char *)c, 0, 0x3e800, 0, (s16)-0x800);
}
}

// @symbol func_ov092_02131650
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_02131650(void *c) {
    func_ov092_021316d8((char *)c, 0, -0x3e800, 0, (s16)0x800);
}
}

// @symbol func_ov092_02131680
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_02131680(void *c) {
    func_ov092_021316d8((char *)c, -0x3e800, 0, -0x800, (s16)0);
}
}

// @symbol func_ov092_021316b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_021316b0(void *r0) {
    func_ov092_021316d8((char *)r0, 0x3e800, 0, 0x800, (s16)0);
}
}

// @symbol func_ov092_021316d8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_021316d8(char *c, int a1, int a2, int a3, s16 a4)
{
    int idx;
    int r5;
    s16 mv;
    s16 *ang;

    {
        s16 t = (s16)((*(u16 *)(c + 0x564) + 1) << 12);
        idx = ((u16)t >> 4) * 2;
    }
    r5 = *(int *)(c + 0x55c);
    r5 = r5 + 0x3000;
    mv = data_02082214[idx];
    *(int *)(c + 0x60) = r5 + ((mv * 0x6793a) >> 12);

    {
        int *px = (int *)LA(c + 0x5c);
        *px = *px + a2;
    }
    ang = (s16 *)(c + 0x8c);
    {
        int *pz = (int *)LA(c + 0x64);
        *pz = *pz + a1;
    }

    if (a3) {
        u8 f = *(u8 *)(c + 0x575);
        if ((f & 0x10) != 0) {
            if ((f & 0x20) != 0) {
                s16 *p = (s16 *)LA(c + 0x8e);
                *p = (s16)(*p + a3);
            } else {
                s16 *p = (s16 *)LA(c + 0x8e);
                *p = (s16)(*p - a3);
            }
        } else {
            if ((f & 0x20) != 0)
                *ang = (s16)(*ang - a3);
            else
                *ang = (s16)(*ang + a3);
        }
    } else {
        u8 f = *(u8 *)(c + 0x575);
        s8 k = data_ov092_0213208c[f];
        if (k < 0) {
            s16 *p = ang - ((int)k + 1);
            *p = (s16)(*p - a4);
        } else {
            s16 t = ang[k];
            ang[k] = (s16)(t + a4);
        }
    }

    if (*(u16 *)(c + 0x564) != 7)
        return;

    {
        s16 *ip = (s16 *)LA(c + 0x92);
        s16 *r3 = (s16 *)LA(c + 0x94);
        s16 *r2 = (s16 *)LA(c + 0x96);
        *ip = (s16)(*ip + *(s16 *)(c + 0x8c));
        *r3 = (s16)(*r3 + *(s16 *)(c + 0x8e));
        *r2 = (s16)(*r2 + *(s16 *)(c + 0x90));
    }

    if (*(u8 *)(c + 0x574) == 3)
        *(int *)(c + 0x568) = 0;
    else
        func_ov092_021314d0(c);
    *(u8 *)(c + 0x577) = 1;
}
}

// @symbol func_ov092_02131878
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov092_02131878(char* c, char* a1, unsigned int a2){
  unsigned char f = *(unsigned char*)(c+0x575);
  int t = (f & 0xf) + ((f >> 4) & 3);
  if ((t & 3) != 0) return;
  if ((*(int*)(a1+8) & 3) != 2) return;
  if (Vec3_HorzDist((Vector3*)(c+0x5c), (Vector3*)(a1+0x5c)) >= 0xd2000) return;
  func_02012694(0x177, (const Vector3 *)(c + 0x74));
  *(int*)(c+0x568) = 6;
  *(int*)(c+0x9c) = -0xc000;
  *(int*)(c+0xa0) = -0x48000;
  *(int*)(c+0xa8) = data_ov092_02132074[a2];
  *(int*)(c+0x98) = data_ov092_02132080[a2];
  if (a2 < 2) *(short*)(c+0x94) = *(short*)(a1+0x8e);
  else *(short*)(c+0x94) = Vec3_HorzAngle((Vector3*)(a1+0x5c), (Vector3*)(c+0x5c));
  {
    unsigned n = (*(unsigned short*)(c+0x94)) >> 4;
    short *base = (short*)(c + 0x400);
    short *tbl = data_02082214;
    int *pb0 = (int*)LA(c+0xb0);
    base[0xe0/2] = (short)(tbl[(n<<1)+1] >> 2);
    base[0xe2/2] = 0;
    base[0xe4/2] = (short)((-(int)tbl[n<<1]) >> 2);
    *pb0 = *pb0 & ~0x2000000;
    *(int*)(c+0x4ec) = 0x1a9000;
  }
}
}

// @symbol func_ov092_021319b0
extern "C" {
void func_ov092_021319b0(char* c)
{
    unsigned int id = *(unsigned int*)(c + 0x50c);
    if (id == 0) return;
    char* o = (char*)_ZN8dActor_c10FindWithIDEj(id);
    if (o == 0) return;
    int b = (*(unsigned short*)(o + 0xc) == 0xbf);
    if (b == 0) return;
    int f = *(int*)(c + 0x508);
    if (f & 0x40) {
        func_ov092_02131878(c, o, 0);
    } else if (f & 0x380) {
        func_ov092_02131878(c, o, 1);
    } else if (_ZN8dActor_c24BumpedUnderneathByPlayerER6Player(c, o) != 0) {
        if (*(int*)(o + 0x60) > *(int*)(c + 0x60) - 0x64000)
            func_ov092_02131878(c, o, 2);
    }
    *(void**)(c + 0x320) = o;
}
}

// @symbol func_ov092_02131a88
/* recovered: shared common types */
extern "C" {

struct MMC { char p[0x124]; };
struct Obj { char p[0x2ec]; Matrix4x3 m; };
void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void *, void *, short);
void func_ov092_02131a88(char* self){
    Obj* o = (Obj*)self;
    o->m = *(Matrix4x3*)(self + 0xf0);
    *(int*)(self+0x310) = *(int*)(self+0x5c);
    *(int*)(self+0x314) = *(int*)(self+0x60);
    *(int*)(self+0x318) = *(int*)(self+0x64);
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s((void *)(self + 0x124), (void *)&o->m, *(short *)(self + 0x8e));
}
}

// @symbol func_ov092_02131aec
/* recovered: shared common types */
extern "C" {

extern struct Matrix4x3 data_020a0e68;
void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
void Matrix4x3_ApplyInPlaceToRotationZ(void* m, short angZ);
void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
void func_ov092_02131aec(void* c){
  char* r4 = (char*)c;
  if(*(unsigned char*)(r4+0x576) != 0){
    data_020a0e68 = *(struct Matrix4x3*)(r4+0x528);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(r4+0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short*)(r4+0x90));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(r4+0x8e));
    *(struct Matrix4x3*)(r4+0xf0) = data_020a0e68;
    *(int*)(r4+0x114) = *(int*)(r4+0x5c) >> 3;
    *(int*)(r4+0x118) = *(int*)(r4+0x60) >> 3;
    *(int*)(r4+0x11c) = *(int*)(r4+0x64) >> 3;
  } else {
    *(struct Matrix4x3*)(r4+0xf0) = *(struct Matrix4x3*)(r4+0x528);
  }
}
}

// @symbol _ZN8daOnms_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
int daOnms_c::CleanupResources()
{
    data_ov092_02132540.Release();
    data_ov092_02132548.Release();
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    return 1;
}

// @symbol _ZN8daOnms_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daOnms_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN8daOnms_c8BehaviorEv
int daOnms_c::Behavior()
{
    unk_576 = 1;
    unk_577 = 0;

    s32 oldState = mMoveDir;
    (this->*data_ov092_02132568[oldState].state)();
    mStateTimer++;
    if (oldState != mMoveDir)
        mStateTimer = 0;

    func_ov092_02131aec((void *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        func_ov092_02131a88((char *)this);

    if (unk_577 == 1) {
        if (mAngleX != 0 || mAngleY != 0 || mAngleZ != 0) {
            mBaseMtx = mModel.mat4x3;
            mAngleX = 0;
            mAngleY = 0;
            mAngleZ = 0;
        }
    }

    mdCcAcPos_c.Clear();
    Vector3 offset;
    offset.x = 0;
    offset.y = -0xfa000;
    offset.z = 0;
    mdCcAcPos_c.SetPosRelativeToActor(offset);
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN8daOnms_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int daOnms_c::InitResources()
{
    void *f;
    Vec3 tmp;
    unsigned int idx;

    f = Model::LoadFile(data_ov092_02132540);
    mModel.SetFile((BMD_File *)f, 1, -1);

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, (int)this, 0xfa000, 0, 0, 0);
    mWithMeshClsn.SetLimMovFlag();

    idx = param1 & 3;
    mMoveKind = (unsigned char)idx;
    idx = mMoveKind;
    if (idx != 3) {
        mMoveSeq = ((s32 **)&data_ov092_02132294)[idx];
        mMoveSeqIndex = 0;
        mMoveDir = *mMoveSeq;
    } else {
        _ZN7PathPtr6FromIDEj(&mPathPtr, (param1 >> 8) & 0xf);
        mPathNodeCount = mPathPtr.NumNodes();
        mPathNodeIndex = 0;
        mPathPtr.GetNode(mPathNode, mPathNodeIndex);
        func_ov092_021313b0(this);
    }

    {
        mPosY += 0xfa000;
        mRestPos.x = mPosX;
        mRestPos.y = mPosY;
        mRestPos.z = mPosZ;
        Vec3_Asr(&tmp, (Vec3 *)&mPosX, 3);
    }

    Matrix4x3_FromTranslation(&data_020a0e68, tmp.x, tmp.y, tmp.z);
    *(Mtx43 *)((char *)this + 0xf0) = data_020a0e68;
    *(Mtx43 *)&mBaseMtx = *(Mtx43 *)((char *)this + 0xf0);

    func_ov092_02131a88((char *)this);

    f = dBgW_Kc::LoadFile(data_ov092_02132548);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)f, &mClsnMat,
        0x1000, mAngleY, &data_ov092_02132220);
    func_020393d4((int *)&mMeshCollider,
        (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    mOrientBits = (unsigned char)((mAngleX >> 0xe) & 3);
    {
        unsigned char *b = (unsigned char *)LA(&mOrientBits);
        *b = (unsigned char)(*b | ((mAngleY >> 0xc) & 0xc));
        *b = (unsigned char)(*b | ((mAngleZ >> 0xa) & 0x30));
    }

    {
        int stk[3];
        stk[0] = 0;
        stk[1] = -0xfa000;
        stk[2] = 0;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mdCcAcPos_c, this, (const Vector3 *)&stk[0],
            0xc8000, 0x190000, 2, 0x6003c0);
    }

    mPlayerActor = 0;
    return 1;
}
