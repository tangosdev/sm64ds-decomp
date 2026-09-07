//cpp
/* ov091/daDsnBase_c -- the abstract base of the crushers (Thwomp, Grindel).
 *
 * Gathered from the run's one-function shards; see include/daDsnBase_c.h for the
 * class's shape, its vtable/RTTI split (vtable ov091 0x021351fc, record ov025
 * 0x021137f8) and why the destructor is inline in the class body.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * CleanupResources is this class's ABI key function (first declared virtual
 * that is neither inline nor pure), so this TU emits _ZTV11daDsnBase_c and,
 * with it, both destructor variants. The forcing scaffolds the two destructor
 * shards carried (`p->~daDsnBase_c();` and `delete p;`) are therefore not
 * needed here and have been dropped.
 *
 * LICENSED FOR 9 OF THE RUN'S 11 FUNCTIONS: .text 0x02132dc0..0x02133254.
 * The destructor pair below that range stays owned by its own two enrolled
 * shards, because the cartridge orders D0 (0x02132d04) BELOW D1 (0x02132d6c)
 * and no admissible source form emits that order -- a destructor defined
 * inline in the class body always emits D1 then D0, and an out-of-line one
 * emits D2, D0, D1 plus a homeless D2 the ROM does not carry. This file still
 * emits both variants byte-identically; the manifest licenses those two copies
 * deadstrip-duplicate. See the manifest's boundary_evidence.
 *
 * Absorbed these 9 legacy one-function sources (ROM address order):
 *   0x02132dc0  func_ov091_02132dc0
 *   0x02132e64  func_ov091_02132e64
 *   0x02132e98  func_ov091_02132e98
 *   0x02132f04  func_ov091_02132f04
 *   0x02132ff4  func_ov091_02132ff4
 *   0x02133020  func_ov091_02133020
 *   0x02133098  func_ov091_02133098
 *   0x021331b8  daDsnBase_c::CleanupResources
 *   0x02133210  daDsnBase_c::Render
 *
 * Still enrolled separately, NOT absorbed:
 *   0x02132d04  _ZN11daDsnBase_cD0Ev
 *   0x02132d6c  _ZN11daDsnBase_cD1Ev
 */

/* daDsnBase_c.h reaches dBgActor_c.h, which includes common.h BEFORE Model.h.
 * That fixes Matrix4x3 to common.h's flat `s32 m[12]` spelling -- the same one
 * every shard here compiled against -- so the merge does not change the type.
 * Do not hoist math/Matrix.h above this include. */
#include "daDsnBase_c.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
/* Two shards spelled this `int(char*)` and `int(void*)`; every call site here
 * passes `char* + 0x39e`, so the char* spelling serves both. */
extern int DecIfAbove0_Byte(char*);
/* Two shards spelled this `int(void*)` and `int(int*)`, and spelled its
 * argument `data_0209e650` both as an array and as a scalar. Declared as an
 * array taking void*, both call sites below stand verbatim and both resolve to
 * the same address. */
extern int RandomIntInternal(void*);
extern int data_0209e650[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN11daDsnBase_c6RenderEv, 0x02133210, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daDsnBase_c6RenderEv
int daDsnBase_c::Render()
{
  int check = *(int*)(mFileTable+0xc);
  if (check != 0)
    mTextureSequence.Update(mModel.data);
  ((Model*)((char*)&mModel))->Render(0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN11daDsnBase_c16CleanupResourcesEv, 0x021331b8, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daDsnBase_c16CleanupResourcesEv
int daDsnBase_c::CleanupResources()
{
    void **fp;
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    fp = *(void***)((char *)&mFileTable);
    ((SharedFilePtr *)(fp[0]))->Release();
    fp = *(void***)((char *)&mFileTable);
    ((SharedFilePtr *)(fp[1]))->Release();
    fp = *(void***)((char *)&mFileTable);
    if (fp[3] != 0) {
        ((SharedFilePtr *)(fp[3]))->Release();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov091_02133098, 0x02133098, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02133098
extern "C" {
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void* a, void* sm, void* mtx, int f, int t1, int t2, unsigned int x);

/* Parameter is `char*`, not the shard's `void*`: decl_common.h:2858 already
 * declares this symbol and the real header wins. */
void func_ov091_02133098(char* c){
  char* r = c;
  int off = 0x20000;
  int d = *(int*)(r+0x60) - *(int*)(r+0x394);
  if(d <= 0x14000){ d = 0x14000; off = 0; }
  int rad = (int)(((long long)d * 0x60 + 0x800) >> 12);
  int* pb8 = (int*)(((int)r + 0xb8));
  int v1 = *(int*)(*(char**)(r+0x320)+0x10) - rad;
  if(v1 < 0xa000) v1 = 0xa000;
  int v2 = *(int*)(*(char**)(r+0x320)+0x14) - rad;
  if(v2 < 0xa000) v2 = 0xa000;
  *(int*)(r+0xb8) = d + 0x8c000;
  {
    int t = *(int*)(r+0xb8);
    if(t < 0x200000) t = 0x200000;
    *(int*)(r+0xb8) = t;
  }
  *pb8 = *pb8 >> 3;
  *(Matrix4x3*)(r+0x360) = *(Matrix4x3*)(r+0xf0);
  *(int*)(r+0x384) = *(int*)(r+0x5c) >> 3;
  *(int*)(r+0x388) = (*(int*)(r+0x60) - off) >> 3;
  *(int*)(r+0x38c) = *(int*)(r+0x64) >> 3;
  _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
      r, r+0x338, r+0x360, v1, d + 0x28000, v2, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov091_02133020, 0x02133020, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02133020
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov091_02133020(char *c)
{
    (*(int *)(((int)c + 0x60))) += 0xa000;
    if (*(int *)(c + 0x60) < *(int *)(c + 0x390))
        return;
    *(int *)(c + 0x60) = *(int *)(c + 0x390);
    *(int *)(c + 0x398) = 1;
    *(unsigned char *)(c + 0x39e) =
        (unsigned char)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 0x1e + 0xa);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov091_02132ff4, 0x02132ff4, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02132ff4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov091_02132ff4(char* c) {
    int r = DecIfAbove0_Byte(c+0x39e);
    if (r == 0) {
        *(int*)(c+0x398) = 2;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov091_02132f04, 0x02132f04, size 0xf0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02132f04
extern "C" {

int _ZN8dActor_c15HugeLandingDustEb(void*, int);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned, int, int, int);
int _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void*, struct Vector3*, int);
int func_0201267c(int, void*);
void func_ov091_02132f04(char* c){
  int *pa8 = (int*)(((int)c + 0xa8));
  int a = *pa8;
  int *p60 = (int*)(((int)c + 0x60));
  a = a - 0x4000;
  *pa8 = a;
  a = *p60 + *(int*)(c+0xa8);
  *p60 = a;
  if(*(int*)(c+0x60) > *(int*)(c+0x394)) return;
  *(int*)(c+0x60) = *(int*)(c+0x394);
  int zero = 0;
  *(int*)(c+0xa8) = zero;
  *(int*)(c+0x398) = 3;
  *(unsigned char*)(c+0x39e) = 0xa;
  if(*(unsigned short*)(c+0xc) == 0xa1) zero = 1;

  struct { struct Vector3 part; struct Vector3 quake; } sp;
  if(zero != 0){
    _ZN8dActor_c15HugeLandingDustEb(c, 1);
  } else {
    sp.part.x = *(int*)(c+0x5c);
    sp.part.y = *(int*)(c+0x60);
    sp.part.z = *(int*)(c+0x64);
    sp.part.y = sp.part.y + 0x3c000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2e, sp.part.x, sp.part.y, sp.part.z);
  }
  sp.quake.x = *(int*)(c+0x5c);
  sp.quake.y = *(int*)(c+0x60);
  sp.quake.z = *(int*)(c+0x64);
  _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(c, &sp.quake, 0x7d0000);
  func_0201267c(0xc7, c+0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov091_02132e98, 0x02132e98, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02132e98
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov091_02132e98(char *c){
  if(DecIfAbove0_Byte(c+0x39e)) return;
  *(int*)(c+0x398)=4;
  unsigned int r=RandomIntInternal(data_0209e650);
  unsigned int v=r>>16;
  c[0x39e]=(char)(v%10+0x14);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov091_02132e64, 0x02132e64, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02132e64
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov091_02132e64(char* c){
  int r = DecIfAbove0_Byte(c+0x39e);
  if(r==0){
    *(int*)(c+0x398) = 0;
    *(unsigned char*)(c+0x39e) = 0x28;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov091_02132dc0, 0x02132dc0, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov091_02132dc0
extern "C" {
extern void* _ZN8dActor_c18ClosestWithActorIDEj(void*, unsigned int);
extern int Vec3_Dist(void*, void*);

int func_ov091_02132dc0(char* c) {
    void* p = _ZN8dActor_c18ClosestWithActorIDEj(c, 9);
    if (p != 0) {
        volatile struct Vector3 v;
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        v.y = v.y + ((dActor_c*)c)->OnAimedAtWithEgg();
        if (Vec3_Dist((char*)c + 0x5c, (char*)p + 0x5c) < (*(int*)(c + 0xb8) << 3)) {
            if (!((dBgW *)(c + 0x124))->IsEnabled()) {
                ((dBgW *)(c + 0x124))->Enable((dActor_c *)(c));
                return 1;
            }
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- _ZN11daDsnBase_cD1Ev 0x02132d6c,
 *                         _ZN11daDsnBase_cD0Ev 0x02132d04.
 *
 * No source appears here. The destructor is defined inline in the class body
 * (include/daDsnBase_c.h) so that every descendant inlines it, and owning the
 * key function above is what makes this TU emit the vtable and, with it, both
 * out-of-line variants. See the manifest's boundary_evidence for how the pair
 * is dispositioned.
 */
// @symbol _ZN11daDsnBase_cD1Ev
// @symbol _ZN11daDsnBase_cD0Ev
