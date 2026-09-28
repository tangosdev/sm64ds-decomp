//cpp
/* daObjFl_Amilift_c. ov064 0x02117978..0x02117fb4, 11 functions.
 *
 * ROM name from _ZTS17daObjFl_Amilift_c. Vtable _ZTV17daObjFl_Amilift_c
 * at 0x0211bc68. daObjFl_Amilift_c_classInit at 0x02117fe8 and the two
 * functions after InitResources (func_ov064_02117fb4, func_ov064_02117fd4)
 * stay out.
 *
 * #pragma defer_codegen off lays .text down in source order. One out-of-line
 * destructor emits D1 then D0.
 */

#include "daObjFl_Amilift_c.h"
#include "common.h"
#include "decl_common.h"
#include "decl_PathPtr.h"

/* shadow struct 'Base' */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

/* shadow struct 'Derived' */
struct Derived { char pad[0xd4]; Base base; };

/* shadow struct 'C' */
struct C;

/* shadow typedef 'void' */
typedef void (C::*PMF)();

/* shadow struct 'Entry' */
struct Entry { PMF pmf; };

/* shadow struct 'BMD_File' */
struct BMD_File;

/* shadow struct 'KCL_File' */
struct KCL_File;

/* shadow struct 'CLPS_Block' */
struct CLPS_Block;

/* shadow struct 'V3' */
struct V3 { int x, y, z; };

extern "C" {
void _ZN8dActor_c9UpdatePosEP5dCc_c(char* self, void* c);
int Vec3_HorzDist(const void* a, const void* b);
int _Z14ApproachLinearRiii(int* v, int target, int step);
void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, void* v, unsigned int j);
void func_ov064_02117a14(char* c, Vector3* a, Vector3* b);
extern void _ZN13SharedFilePtr7ReleaseEv(SharedFilePtr &);
extern Entry data_ov064_0211c750[];
extern short data_02082214[];
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block( void *self, KCL_File *k, Matrix4x3 *m, int fix, short s, CLPS_Block *clps);
void func_020393d4(void *p, void *v);
void func_020393c4(void *p, void *v);
void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern SharedFilePtr data_ov064_0211c730;
extern SharedFilePtr data_ov064_0211c728;
extern CLPS_Block data_ov064_0211bb6c;
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
}

/* Emission order is ROM order. Do not reorder. */
#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN17daObjFl_Amilift_cD1Ev, 0x02117978, size 0x44 */
/* ROM ordinal 1 -- _ZN17daObjFl_Amilift_cD0Ev, 0x021179bc, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjFl_Amilift_cD1Ev
// @symbol _ZN17daObjFl_Amilift_cD0Ev
daObjFl_Amilift_c::~daObjFl_Amilift_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov064_02117a14, 0x02117a14, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_02117a14
extern "C" {
extern void Vec3_Sub(Vector3* dst, Vector3* a, Vector3* b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
void func_ov064_02117a14(char* c, Vector3* a, Vector3* b){
  Vector3 v;
  Vec3_Sub(&v, a, b);
  *(short*)(c+0x94)=(short)_ZN4cstd5atan2E5Fix12IiES1_(v.x, v.z);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov064_02117a44, 0x02117a44, size 0x148 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_02117a44
extern "C" int func_ov064_02117a44(char* c) {
  Vector3 a;
  Vector3 b;
  int r;
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
  r = (Vec3_HorzDist(c+0x5c, c+0x354) < 0x69000) ? 0 : 0xa000;
  if (_Z14ApproachLinearRiii((int*)(c+0x98), r, 0x800) != 0 && r == 0) {
    *(int*)(c+0x348) = *(int*)(c+0x354);
    *(int*)(c+0x34c) = *(int*)(c+0x358);
    *(int*)(c+0x350) = *(int*)(c+0x35c);
    *(int*)(c+0x5c) = *(int*)(c+0x348);
    *(int*)(c+0x60) = *(int*)(c+0x34c);
    *(int*)(c+0x64) = *(int*)(c+0x350);
    r = 1;
    if (*(unsigned char*)(c+0x33b) == 1) {
      ++*(int*)(((int)c + 0x344));
      if (*(int*)(c+0x344) >= *(int*)(c+0x340)) {
        *(int*)(c+0x344) = *(int*)(c+0x340) - 2;
        r = -1;
      }
    } else {
      --*(int*)(((int)c + 0x344));
      if (*(int*)(c+0x344) < 0) {
        *(int*)(c+0x344) = r;
        r = -1;
      }
    }
    _ZNK7PathPtr7GetNodeER7Vector3j(c+0x360, c+0x354, *(int*)(c+0x344));
    a.x = *(int*)(c+0x354);
    a.y = *(int*)(c+0x358);
    a.z = *(int*)(c+0x35c);
    b.x = *(int*)(c+0x348);
    b.y = *(int*)(c+0x34c);
    b.z = *(int*)(c+0x350);
    func_ov064_02117a14(c, &a, &b);
    return r;
  }
  return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov064_02117b8c, 0x02117b8c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_02117b8c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov064_02117b8c(void *c) {
    unsigned short v = *(unsigned short*)((char*)c + 0x338);
    if (v < 0x14) return;
    int r = func_ov064_02117a44((char *)c);
    if (r != -1) return;
    unsigned char b = *(unsigned char*)((char*)c + 0x33c);
    if (b == 0) {
        *(unsigned char*)((char*)c + 0x33b) = 0;
    } else {
        *(unsigned char*)((char*)c + 0x33b) = 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov064_02117bdc, 0x02117bdc, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_02117bdc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov064_02117bdc(void *c)
{
    unsigned char b = *(unsigned char*)((char*)c + 0x33c);
    if (b == 1) {
        unsigned short v = *(unsigned short*)((char*)c + 0x338);
        if (v < 0x14) return v;
    }
    int r = func_ov064_02117a44((char *)c);
    if (r == -1) {
        r = 2;
        *(unsigned char*)((char*)c + 0x33b) = r;
    }
    return r;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov064_02117c24, 0x02117c24, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov064_02117c24
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov064_02117c24(char *c)
{
    if (*(unsigned char *)(c + 0x33a) != 0) {
        if (*(unsigned char *)(c + 0x33c) != 1) {
            if (*(unsigned short *)(c + 0x338) <= 0x14)
                return;
        }
        *(unsigned char *)(c + 0x33b) = 1;
        struct Vector3 v0, v1;
        v0.x = *(int *)(c + 0x354);
        v0.y = *(int *)(c + 0x358);
        v0.z = *(int *)(c + 0x35c);
        v1.x = *(int *)(c + 0x348);
        v1.y = *(int *)(c + 0x34c);
        v1.z = *(int *)(c + 0x350);
        func_ov064_02117a14(c, &v0, &v1);
        return;
    }
    *(short *)(c + 0x338) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN17daObjFl_Amilift_c16CleanupResourcesEv, 0x02117cc4, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjFl_Amilift_c16CleanupResourcesEv
s32 daObjFl_Amilift_c::CleanupResources() {
    void * t = (void *)this;
    _ZN4dBgW7DisableEv((char *)t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c730);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c728);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN17daObjFl_Amilift_c6RenderEv, 0x02117cfc, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjFl_Amilift_c6RenderEv
s32 daObjFl_Amilift_c::Render() {
    Derived * d = (Derived *)this; Base *b = &d->base; b->m(0); return 1; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN17daObjFl_Amilift_c8BehaviorEv, 0x02117d24, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjFl_Amilift_c8BehaviorEv
s32 daObjFl_Amilift_c::Behavior() {
    char* c = (char*)this;
    int idx = *(unsigned char*)(c + 0x33b);
    (((C*)c)->*data_ov064_0211c750[idx].pmf)();
    unsigned short* p338 = (unsigned short*)(c + 0x338);
    *p338 = (unsigned short)(*p338 + 1);
    if (idx != *(unsigned char*)(c + 0x33b)) {
        *(short*)((char*)c + 0x300 + 0x38) = 0;
    }
    int target = *(unsigned char*)(c + 0x33a) ? -0x28000 : 0;
    if (_Z14ApproachLinearRiii((int*)(c + 0x320), target, 0x5000)) {
        short* pAng = (short*)(c + 0x328);
        *pAng = (short)(*pAng + 0xa00);
        unsigned short h = *(unsigned short*)((char*)c + 0x300 + 0x28);
        short s = data_02082214[(h >> 4) * 2];
        short ten = 10;
        *(int*)(c + 0x324) = s * ten;
    }
    int t330 = *(int*)(c + 0x330);
    int t320 = *(int*)(c + 0x320);
    int t324 = *(int*)(c + 0x324);
    int saved = *(int*)(c + 0x60);
    *(int*)(c + 0x60) = t324 + (t330 + t320);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    *(int*)(c + 0x60) = saved;
    *(unsigned char*)(c + 0x33a) = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN17daObjFl_Amilift_c13InitResourcesEv, 0x02117e44, size 0x170 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17daObjFl_Amilift_c13InitResourcesEv
s32 daObjFl_Amilift_c::InitResources() {
    char * self = (char *)this;
    BMD_File *bmd;
    KCL_File *kcl;
    V3 nodeA;
    V3 nodeB;
    unsigned char b;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov064_0211c730);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);

    b = (*(unsigned int *)(self + 8) >> 8) & 1;
    *(unsigned char *)(self + 0x33c) = b;
    b = *(unsigned char *)(self + 0x33c);
    if (b == 0)
        *(unsigned char *)(self + 0x33b) = 0;
    else
        *(unsigned char *)(self + 0x33b) = 1;

    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);

    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov064_0211c728);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, kcl, (Matrix4x3 *)(self + 0x2ec), 0x199, *(short *)(self + 0x8e), &data_ov064_0211bb6c);

    func_020393d4(self + 0x124, (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(self + 0x124, (void *)&func_ov064_02117fd4);

    _ZN7PathPtr6FromIDEj(self + 0x360, *(int *)(self + 8) & 0xf);
    *(int *)(self + 0x340) = _ZNK7PathPtr8NumNodesEv(self + 0x360);
    *(int *)(self + 0x344) = 1;

    *(int *)(self + 0x348) = *(int *)(self + 0x5c);
    *(int *)(self + 0x34c) = *(int *)(self + 0x60);
    *(int *)(self + 0x350) = *(int *)(self + 0x64);

    _ZNK7PathPtr7GetNodeER7Vector3j(self + 0x360, (Vector3 *)(self + 0x354), *(int *)(self + 0x344));

    nodeA.x = *(int *)(self + 0x354);
    nodeA.y = *(int *)(self + 0x358);
    nodeA.z = *(int *)(self + 0x35c);
    nodeB.x = *(int *)(self + 0x348);
    nodeB.y = *(int *)(self + 0x34c);
    nodeB.z = *(int *)(self + 0x350);

    func_ov064_02117a14(self, (Vector3 *)&nodeA, (Vector3 *)&nodeB);

    *(int *)(self + 0x32c) = *(int *)(self + 0x5c);
    *(int *)(self + 0x330) = *(int *)(self + 0x60);
    *(int *)(self + 0x334) = *(int *)(self + 0x64);

    return 1;
}
