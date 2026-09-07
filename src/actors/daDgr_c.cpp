//cpp
/* daDgr_c -- a swinging platform. ov025, 9 functions, 0x021111a0..0x02111898.
 *
 * ENROLLED AND CANONICAL. This is the whole translation unit: one complete
 * entry in config/arm9/overlays/ov025/delinks.txt covers the entire range, and
 * the nine one-function files it was assembled from are gone. Started from
 * tools/tubuild.py create; the shadow declarations below have since been
 * reconciled against the real project headers, so read them as the TU's own and
 * not as unreviewed leftovers. tubuild verify: 9/9 MATCH, objisolate clean,
 * reloc-destinations clean. tubuild linkcheck, run before promotion:
 * SCRATCH-LINK-VERIFIED -- the whole range reproduces from one object, all 106
 * modules byte-exact, and the full ROM builds to the same sha256 as the
 * source-independent stock control.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * THE DESTRUCTOR PAIR IS WHY THIS PROMOTED WHOLE. The cartridge puts D1 at
 * 0x021111a0 below D0 at 0x021111e4 and has no D2 at all, which is exactly what
 * an inline in-class virtual destructor emits -- so include/daDgr_c.h declares
 * `virtual ~daDgr_c() {}` first and nothing here defines it. Had the ROM
 * ordered D0 below D1, no admissible source form would reproduce it and the
 * pair would have had to stay outside the licensed range.
 *
 * Assembled from these legacy one-function sources (ROM address order), all
 * nine now deleted and superseded by this file:
 *   [0] 0x021111a0  src/_ZN7daDgr_cD1Ev.cpp
 *   [1] 0x021111e4  src/_ZN7daDgr_cD0Ev.cpp
 *   [2] 0x0211123c  src/func_ov025_0211123c.cpp
 *   [3] 0x021112e0  src/func_ov025_021112e0.cpp
 *   [4] 0x02111344  src/func_ov025_02111344.c
 *   [5] 0x02111384  src/_ZN7daDgr_c16CleanupResourcesEv.cpp
 *   [6] 0x021113c8  src/_ZN7daDgr_c6RenderEv.cpp
 *   [7] 0x021113f0  src/_ZN7daDgr_c8BehaviorEv.cpp
 *   [8] 0x021117dc  src/_ZN7daDgr_c13InitResourcesEv.cpp
 */

/* Includes: the union of the nine legacy files', first-seen in ROM-ascending
 * processing order. The ordering IS load-bearing here -- daDgr_c.h reaches
 * math/Matrix.h, which wins the guarded Matrix4x3 race against common.h and
 * changed the codegen of func_ov025_021112e0 below; see that function's own
 * comment. Do not reorder these without re-running tubuild verify. */
#include "daDgr_c.h"
#include "SharedFilePtr.h"
#include "decl_common.h"

/* Declarations for the ROM symbols this TU calls that no project header owns.
 * These have been checked against include/decl_common.h and the decl_*.h family
 * -- where a real header declares one of these, the header wins and the local
 * spelling is gone. What remains is what no header declares. */
/* shadow typedef 'Vec3' */
typedef struct { s32 x, y, z; } Vec3;

/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* shadow struct 'SharedFilePtr' */
struct SharedFilePtr;

/* shadow struct 'BMD_File' */
struct BMD_File;

/* shadow struct 'KCL_File' */
struct KCL_File;

/* shadow struct 'CLPS_Block' */
struct CLPS_Block;

/* shadow struct 'ModelBase' */
struct ModelBase;

#define LAUNDER(p) (p)

extern "C" {
extern void Matrix4x3_FromRotationX(void *, int);
/* The two file handles carry ONE spelling here, not the two the merge produced.
 * The CleanupResources shard declared them `int[]` and cast the decayed pointer;
 * the InitResources shard declared them `SharedFilePtr` and passed a reference.
 * Both are the same object, so the merged file has to choose, and the object
 * spelling is the one that lets both call sites be ordinary C++ (`.Release()`
 * and a plain reference argument) with no cast at all. `tubuild create`'s
 * conflict detector does not compare types, so it emitted both and reported
 * no conflict; this would not have compiled. */
extern SharedFilePtr data_ov025_02113a68;
extern SharedFilePtr data_ov025_02113a60;
extern s16 data_02082214[];
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(daDgr_c *self, s32 a, s32 b);
void func_02012694(s32 a, void *b);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(daDgr_c *self, Vec3 *pos, s32 fix);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
u32 slot, u32 effect, s32 x, s32 y, s32 z, const void *rot, void *cb);
extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase*, BMD_File*, int, int);
extern "C" KCL_File* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block( dBgW_KcMbg*, KCL_File*, const Matrix4x3&, Fix12i, short, CLPS_Block&);
extern "C" void func_020393d4(int* p, int v);
extern CLPS_Block data_ov025_02112c28;
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN7daDgr_c13InitResourcesEv, 0x021117dc, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_c13InitResourcesEv
/* daDgr_c::InitResources -- vtable slot 0. Real C++ method over the shared
 * header: loads the swinging platform's model and its KCL, points the mesh
 * collider's update hook at dBgW::UpdatePosAndAngs, zeroes the facing angle the
 * KCL was baked against, and parks the swing state -- resting height, angular
 * speed, phase, stage, direction and both particle handles.
 *
 * (This used to be an extern "C" free function over a raw `char *` with every
 * field reached by literal offset. Converting it to a real method and naming
 * the fields is byte-exact under the pinned 2004/b56 -- checked with
 * build_pin.verify in tools/build_pin.py.)
 *
 * The two func_ov025_* helpers still take a `char *`: decl_common.h spells them
 * that way and this file does not own that header. */
s32 daDgr_c::InitResources()
{
    func_ov025_02111344((char*)this);
    func_ov025_021112e0((char*)this);
    {
        BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov025_02113a68);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)&mModel, bmd, 1, -1);
    }
    {
        KCL_File* kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov025_02113a60);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, mClsnMat, 0x1000, mAngleY, data_ov025_02112c28);
    }
    func_020393d4((int*)&mMeshCollider,
                  (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mAngleY = 0;
    mBasePosY = mPosY;
    mAngleXSpeed = 0;
    mPhaseTimer = 0;
    mSwingStage = 0;
    mSwingDir = 0;
    mDustParticle2 = 0;
    mDustParticle1 = mDustParticle2;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN7daDgr_c8BehaviorEv, 0x021113f0, size 0x3ec */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_c8BehaviorEv
/* daDgr_c::Behavior -- vtable slot 6, ov025 0x021113f0.
 *
 * A swinging platform. mPhaseTimer is the phase counter, mSwingStage the stage counter
 * (-1 is the "hold at the end" sentinel), mSwingDir the direction flag. The swing
 * speed comes from `10 - mSwingStage' folded to a magnitude and bucketed to 1/2/4,
 * so the platform eases in and out; mBasePosY holds the resting height that the
 * sine offset from data_02082214 is added to. Two dust particle systems
 * (mDustParticle1, mDustParticle2) trail either end, and the last frame of each swing shakes
 * the ground.
 *
 * IsClsnInRange, Earthquake and Particle::System::New all carry Fix12<int> by
 * value in their mangled names (wall 6az), so all three stay extern-C free
 * functions.
 *
 * LAUNDER() is a no-op macro the legacy file used to MARK its read-modify-write
 * sites. It is kept, with its name, so the marking survives -- it emits nothing
 * and it is not the reason those sites take an address. Every one of them now
 * names the member: `(s8 *)LAUNDER(&mSwingStage)', not `(s8 *)LAUNDER(self + 0x328)'.
 *
 * Swept greedily against build_pin.verify -- 33 substitutions, 33 kept once the
 * three real obstacles were fixed: `mAngleY' is read UNSIGNED at the table-index
 * sites (an s16 read costs four words, the same finding daObjC1_Trap_c::InitResources
 * produced), the three func_ov025_* helpers are declared `char *' in
 * decl_common.h so `this' needs a cast, and the two-step `b = self + 0x300;
 * b + 0x28' was just mSwingStage reached the long way round.
 */
s32 daDgr_c::Behavior()
{
    s32 loc[6];
    s32 n;
    s32 m;

    if (mSwingStage == -1) {
        if (mPhaseTimer == 0x20) {
            mSwingStage = 0;
            mPhaseTimer = 0;
        } else {
            u16 *cnt = (u16 *)LAUNDER(&mPhaseTimer);
            (*cnt)++;
            unk_0ac = 0;
            mAngleXSpeed = 0;
            func_ov025_02111344((char *)this);
            if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0)) {
                func_ov025_021112e0((char *)this);
            }
            return 1;
        }
    }

    n = 10 - mSwingStage;
    if (n < 0) {
        s32 neg = -1;
        n = n * neg;
    }
    n = n - 6;
    if (n < 0)
        n = 0;

    if (mPhaseTimer == n + 8) {
        s8 *p;
        mPhaseTimer = 0;
        p = (s8 *)LAUNDER(&mSwingStage);
        *p = (s8)(*p + 1);
        if (mSwingStage == 0x14) {
            u8 *pd = (u8 *)LAUNDER(&mSwingDir);
            *pd = (u8)(*pd ^ 1);
            mSwingStage = -1;
        }
    }

    if ((u32)(n - 3) <= 1u) {
        n = 4;
    } else if ((u32)(n - 1) <= 1u) {
        n = 2;
    } else if (n == 0) {
        n = 1;
    }

    m = n << 3;

    if (mPhaseTimer < m) {
        if (mSwingDir == 0) {
            unk_0ac = 0x14000 / n;
            mAngleXSpeed = (s16)(0x400 / n);
        } else {
            unk_0ac = (-0x14000) / n;
            mAngleXSpeed = (s16)((-0x400) / n);
        }

        {
            s32 *pz = (s32 *)LAUNDER(&mPosZ);
            s16 *pr = (s16 *)LAUNDER(&mAngleX);
            *pz = *pz + unk_0ac;
            *pr = (s16)(*pr + mAngleXSpeed);
            if ((mAngleX & 0x1fff) < 0x320) {
                if (mAngleXSpeed != 0)
                    func_02012694(0x65, (char *)&mCamSpacePosX);
            }
        }

        {
            s16 t = mAngleX;
            s32 idx = ((u16)(s16)(t << 2) >> 4) * 2;
            s32 prod = (s32)data_02082214[idx] * 23;
            if (prod < 0) {
                s32 neg = -1;
                prod = prod * neg;
            }
            mPosY = mBasePosY + prod;
        }

        if (mPhaseTimer == m - 1) {
            loc[3] = mPosX;
            loc[4] = mPosY;
            loc[5] = mPosZ;
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, (Vec3 *)&loc[3], 0x3e8000);
        }

        loc[0] = mPosX;
        loc[1] = mPosY;
        loc[2] = mPosZ;
        {
            s32 scale = 100;
            s32 zero = 0;
            s32 c;
            c = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            loc[0] = c * scale + mPosX;
            loc[1] = mBasePosY - 0xb9000;
            c = data_02082214[((u16)mAngleY >> 4) * 2];
            loc[2] = c * scale + mPosZ;
            mDustParticle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mDustParticle1, 0x2d, loc[0], loc[1], loc[2], (void *)zero, (void *)zero);
        }
        {
            s32 scale = 100;
            s32 zero = 0;
            s32 c;
            c = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            loc[0] = mPosX - c * scale;
            c = data_02082214[((u16)mAngleY >> 4) * 2];
            loc[2] = mPosZ - c * scale;
            mDustParticle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mDustParticle2, 0x2d, loc[0], loc[1], loc[2], (void *)zero, (void *)zero);
        }
    }

    func_ov025_02111344((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) || func_ov025_0211123c((char *)this)) {
        func_ov025_021112e0((char *)this);
    }

    {
        u16 *cnt = (u16 *)LAUNDER(&mPhaseTimer);
        (*cnt)++;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN7daDgr_c6RenderEv, 0x021113c8, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_c6RenderEv
/* daDgr_c::Render -- vtable slot 9. Real C++ method over the shared header;
   the Model sub-object at +0xd4 is rendered through its own vtable (mwccarm
   does not devirtualise an embedded member's virtual call). */
s32 daDgr_c::Render() { mModel.Render(0); return 1; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN7daDgr_c16CleanupResourcesEv, 0x02111384, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_c16CleanupResourcesEv
/* daDgr_c::CleanupResources -- vtable slot 3. Real C++ method over the shared
   header. */
s32 daDgr_c::CleanupResources() {
  data_ov025_02113a68.Release();
  data_ov025_02113a60.Release();
  if (mMeshCollider.IsEnabled())
    mMeshCollider.Disable();
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov025_02111344, 0x02111344, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov025_02111344
/* Added by hand: `tubuild create` writes @symbol markers only for mangled or
 * already-named members and skips auto-named func_ovNN_ADDR shards silently, so
 * without this line tiers scoring would miss this member. The other two
 * auto-named shards got theirs because they were carried in RAW. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov025_02111344(char *t)
{
    Matrix4x3_FromRotationX(t + 0xf0, *(short *)(t + 0x8c));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov025_021112e0, 0x021112e0, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov025_021112e0
/* recovered: shared common types */
#include "common.h"
extern "C" {

/* THE MATRIX SPELLING HERE IS LOAD-BEARING, AND THE MERGE IS WHAT MADE IT SO.
 *
 * `Matrix4x3` has two guarded 0x30-byte spellings and whichever a TU reaches
 * first stands: common.h's flat `s32 m[12]` and math/Matrix.h's structured
 * `Matrix3x3 r; Vector3 t`. As a one-function shard this file included
 * common.h with nothing ahead of it and got the flat one. Merged, daDgr_c.h ->
 * dBgW_KcMbg.h -> math/Matrix.h arrives first, common.h's guard stands down,
 * and the whole-matrix assignment below stopped matching -- the only DIFF in
 * the first verify of the merged TU, in a function nobody had touched.
 *
 * So the copy is spelled against a local flat type instead of against whichever
 * Matrix4x3 the include union happens to yield. Mat12 is the flat spelling by
 * construction, it cannot be reordered by a future include change, and it
 * restored the match (verify: 9/9). Transform's mangled extern-C declaration
 * takes it by reference; the parameter type is not part of an Itanium mangled
 * name, so nothing about the symbol or the call moves. */
struct Mat12 { s32 m[12]; };
struct MMC { char p[0x124]; };
struct Obj { char p[0x2ec]; Mat12 m; };
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(MMC*, Mat12&, short);
void func_ov025_021112e0(char* self){
    Obj* o = (Obj*)self;
    o->m = *(Mat12*)(self + 0xf0);
    *(int*)(self+0x310) = *(int*)(self+0x5c);
    *(int*)(self+0x314) = *(int*)(self+0x60);
    *(int*)(self+0x318) = *(int*)(self+0x64);
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s((MMC*)(self+0x124), o->m, *(short*)(self+0x8e));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov025_0211123c, 0x0211123c, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov025_0211123c
/* recovered: shared common types */
#include "common.h"
#include "dBgW.h"

/* The shard declared a local 30-slot `struct dActor_c` of f00()..f29() purely to
 * spell one virtual call, `((dActor_c*)c)->f29()`. That declaration cannot
 * survive the merge -- daDgr_c.h -> dBgActor_c.h -> dActor_c.h now brings in the
 * REAL dActor_c, and two definitions of one class name is a hard error the
 * conflict detector never looks for. The real header names slot 29
 * `OnAimedAtWithEgg()`, and `c` is this class's own `this`, so the call below
 * goes through daDgr_c and the compiler picks the slot from the real vtable. */
extern "C" {
extern void* _ZN8dActor_c18ClosestWithActorIDEj(void*, unsigned int);
extern int Vec3_Dist(void*, void*);

int func_ov025_0211123c(char* c) {
    void* p = _ZN8dActor_c18ClosestWithActorIDEj(c, 9);
    if (p != 0) {
        volatile struct Vector3 v;
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        v.y = v.y + ((daDgr_c*)c)->OnAimedAtWithEgg();
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
/* ROM ordinals 1 and 0 -- _ZN7daDgr_cD0Ev (0x021111e4, size 0x58) and
 *                         _ZN7daDgr_cD1Ev (0x021111a0, size 0x44)             */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_cD1Ev
// @symbol _ZN7daDgr_cD0Ev
/* NO DEFINITION APPEARS HERE, AND THAT IS THE POINT.
 *
 * Both variants come from `virtual ~daDgr_c() {}` in include/daDgr_c.h. Each of
 * the two one-function shards carried its own out-of-line `daDgr_c::~daDgr_c()
 * {}`, which is harmless in isolation -- objisolate keeps whichever variant
 * that file is bound to -- and wrong twice over once they are merged: it is a
 * duplicate definition, and mwccarm emits the out-of-line form as D2, D0, D1,
 * which is the wrong order for this cartridge and produces a D2 with no ROM
 * home.
 *
 * ov025 puts D1 at 0x021111a0 and D0 at 0x021111e4 -- D1 BELOW D0, the
 * reproducible direction -- and carries no D2 at all. The in-class body emits
 * exactly D1 then D0, so both land inside this TU's licensed .text run in ROM
 * address order and the class promotes whole.
 *
 * D0's deallocation is an inline `operator delete` found by ordinary lookup on
 * dActor_c (include/dActor_c.h), two levels up through dBgActor_c, which is why
 * nothing here mentions a heap. D1's body is the two vptr stores plus
 * dBgActor_c's Model and dBgW_KcMbg teardowns, every one of them a consequence
 * of `struct daDgr_c : dBgActor_c` alone; this class's own 0x14 tail bytes are
 * POD and destruct nothing.
 */
