//cpp
/* PROMOTED translation unit -- ov029/daObjWc_Mizu_c (9 function(s)).
 *
 * This one file is the production source for the whole class: the ROM build
 * takes every one of these functions from a single object, the way the
 * cartridge's own build did. It licenses the contiguous .text run
 * 0x021121a4..0x02112630 in ov029 (config/tu_manifest.d/ov029/daObjWc_Mizu_c.json,
 * config/arm9/overlays/ov029/delinks.txt).
 *
 * WRITTEN IN REVERSE ROM ORDER. mwccarm emits one .text section per function
 * in the reverse of source order, so the highest-address ROM function is
 * written FIRST and the lowest last. Do not reorder these blocks.
 *
 * WHY D1 AND D0 ARE MANGLED BODIES RATHER THAN A REAL DESTRUCTOR.
 * The cartridge keeps D1 at 0x021121a4, BELOW D0 at 0x021121f0. Defining
 * `daObjWc_Mizu_c::~daObjWc_Mizu_c()` as a real C++ member makes mwccarm emit the
 * D2/D1/D0 triple as ONE group in the order D0-then-D1, which objisolate
 * refuses with `requested functions are not emitted in ROM order`; the group
 * also carries an unhomed `_ZN14daObjWc_Mizu_cD2Ev`. Writing the two variants as
 * separate `// @symbol` marked bodies under their mangled names is the only
 * placement that reaches both ROM addresses. It costs the CONVERTED tier's
 * no-raw-offset and no-mangled-refs criteria for those two functions, banked
 * in config/converted-backslide-exceptions.jsonl. Byte-match over readability.
 * The manifest flagged this before the promotion was attempted, as
 * `functions_occur_in_expected_order: PARTIAL -- [(0, 1)]`.
 *
 * THE VPTR STORES CARRY NO BIAS. symbols.txt records _ZTV14daObjWc_Mizu_c at
 * 0x021140dc, which IS the vtable's address point -- the cartridge's literal
 * pool at 0x021121e8 holds exactly that word. So the stores below name the
 * symbol with addend 0; `&_ZTV14daObjWc_Mizu_c[2]` would ask production
 * isolation to rewrite an undefined RTTI reference by 8 and is refused.
 *
 * THIS TU CARRIES THE CLASS'S VTABLE AND RTTI AS VAGUE LINKAGE. With no key
 * function defined anywhere, mwccarm emits _ZTV/_ZTI/_ZTS14daObjWc_Mizu_c plus the
 * base chain here -- but only because include/daObjWc_Mizu_c.h declares
 * ~daObjWc_Mizu_c() AFTER the non-virtual methods. Declared first, this file emits
 * no data at all and those three ROM records go unverified by any source. The 11
 * symbols are licensed as `deadstrip-data` in the manifest; production isolation
 * discards them after tools/romdata_check.py compares them to the cartridge.
 *
 * Absorbed these legacy one-function sources (ROM address order), all deleted
 * by tools/tu_promote.py:
 *   [0] 0x021121a4  src/_ZN14daObjWc_Mizu_cD1Ev.cpp
 *   [1] 0x021121f0  src/_ZN14daObjWc_Mizu_cD0Ev.cpp
 *   [2] 0x02112250  src/func_ov029_02112250.cpp
 *   [3] 0x021122b4  src/func_ov029_021122b4.c
 *   [4] 0x021122dc  src/_ZN14daObjWc_Mizu_c16CleanupResourcesEv.cpp
 *   [5] 0x02112320  src/_ZN14daObjWc_Mizu_c6RenderEv.cpp
 *   [6] 0x02112354  src/_ZN14daObjWc_Mizu_c8BehaviorEv.cpp
 *   [7] 0x021124d0  src/_ZN14daObjWc_Mizu_c13InitResourcesEv.cpp
 *   [8] 0x021125f8  src/daObjWc_Mizu_c_classInit.c
 *
 * Proved at 106/106 exact, 100.000000% of compared bytes, mismatching 0.
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daObjWc_Mizu_c_classInit, 0x021125f8, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol daObjWc_Mizu_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV14daObjWc_Mizu_c */
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: WDW_Water_Spawn. */
int *daObjWc_Mizu_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(840);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjWc_Mizu_c; /* the vtable's address point; the cartridge stores 0x021140dc */
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN14daObjWc_Mizu_c13InitResourcesEv, 0x021124d0, size 0x128 */
/* -------------------------------------------------------------------------- */
#include "types.h"
// @symbol _ZN14daObjWc_Mizu_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Mizu_c.h"
#include "dBgW.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *bmd, void *bta);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *tt, void *bta, int a, int fix, unsigned int b);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *c);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *c);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *sfp);
}
extern "C" {
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mc, void *kcl, void *mtx, int fix, short s, void *clps);
}

extern u8 data_0209f2c0[];
extern int data_0209f32c;

int daObjWc_Mizu_c::InitResources()
{
    void *m;
    void *k;

    m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_021142fc);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, m, 1, -1);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(data_ov029_021142fc[1], &data_ov029_02112fb8);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(((char *)this) + 0x320, &data_ov029_02112fb8, 0, 0x1000, 0);

    mUseSpawnPosY = (u8)((*(s32 *)&param1) & 1);
    if (mUseSpawnPosY == 0) {
        unsigned int idx = data_0209f2c0[0];
        if (idx >= 3) idx = 2;
        mPosY = data_ov029_02112b2c[idx];
    }
    mTargetPosY = mPosY;
    data_0209f32c = mPosY;

    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));

    k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov029_02114304);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov029_02112fec);
    ((dBgW *)(&mMeshCollider))->Enable((dActor_c *)(((char *)this)));

    mTrueAreaID = (u8)mAreaId;
    mAreaId = -1;
    *(s16*)(((char *)this) + 0x300 + 0x42) = 0;

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN14daObjWc_Mizu_c8BehaviorEv, 0x02112354, size 0x17c */
/* -------------------------------------------------------------------------- */
#include "types.h"
// @symbol _ZN14daObjWc_Mizu_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Mizu_c.h"
#include "dBgW.h"
extern "C" {
extern int IsAreaShowing(int idx);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned a, unsigned b, unsigned c, void *pos, unsigned e);
extern void _ZN9Animation7AdvanceEv(void *a);
}
extern s16 data_02082214[];
extern int data_0209f32c;

int daObjWc_Mizu_c::Behavior()
{
    int t;
    s16 *q;
    int i;

    mAreaId = -1;

    /* area id at 0x340: ROM does add r0,r4,#0x300; ldrsb r0,[r0,#0x40] */
    if (IsAreaShowing(*(s8 *)((u8 *)(((int)((u8 *)this) + 0x300)) + 0x40)) == 0) {
        mAreaId = *(s8 *)((u8 *)(((unsigned)((u8 *)this) + 0x300)) + 0x40);
        if (((dBgW *)((u8 *)&(*(u8 *)&mMeshCollider)))->IsEnabled() != 0) {
            ((dBgW *)((u8 *)&(*(u8 *)&mMeshCollider)))->Disable();
        }
    } else {
        if (((dBgW *)((u8 *)&(*(u8 *)&mMeshCollider)))->IsEnabled() == 0) {
            ((dBgW *)(((u8 *)this) + 0x124))->Enable((dActor_c *)(((u8 *)this)));
        }
    }

    t = mTargetPosY;
    if (mPosY != t) {
        if (mPosY < t) {
            int *p = (int *)(((int)((u8 *)this) + 0x60));
            *p = *p + 0xa000;
            if (mPosY > mTargetPosY)
                mPosY = mTargetPosY;
        } else {
            int *p = (int *)(((int)((u8 *)this) + 0x60));
            *p = *p - 0xa000;
            if (mPosY < mTargetPosY)
                mPosY = mTargetPosY;
        }
    }

    if (mPosY != mPrevPosY) {
        mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            *(unsigned *)((u8 *)&mSoundID), 3, 0x96, ((u8 *)this) + 0x74, 0);
    }

    {
        /* angle at 0x342: pool offset 0x342 */
        s16 *q = (s16 *)(((int)((u8 *)this) + 0x342));
        *q = (s16)(*q + 0x200);
        /* index from (u16)angle at 0x342 via 0x300+0x42 */
        i = *(u16 *)((u8 *)(((unsigned)((u8 *)this) + 0x300)) + 0x42) >> 4;
        mWaterHeight = (int)data_02082214[i * 2] * 0xf + mPosY;
    }
    data_0209f32c = mWaterHeight;
    func_ov029_021122b4(((u8 *)this));
    func_ov029_02112250(((u8 *)this));
    (*(s32 *)((char *)&mTextureTransformer + 0xc)) = 0x1000;
    _ZN9Animation7AdvanceEv((u8 *)&(*(u8 *)&mTextureTransformer));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN14daObjWc_Mizu_c6RenderEv, 0x02112320, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjWc_Mizu_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Mizu_c.h"
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base { char pad[0xd4]; Sub sub; };
extern "C" void _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);

int daObjWc_Mizu_c::Render()
{
 _ZN18TextureTransformer6UpdateER15ModelComponents((char *)((Base *)this) + 0x320, (char *)((Base *)this) + 0xdc); Sub *b = &((Base *)this)->sub; b->m(0); return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN14daObjWc_Mizu_c16CleanupResourcesEv, 0x021122dc, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjWc_Mizu_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Mizu_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int daObjWc_Obj07_c_ClsnFile[];
extern int daObjWc_Obj07_c_ModelFile[];

int daObjWc_Mizu_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(daObjWc_Obj07_c_ModelFile))->Release();
    ((SharedFilePtr *)(daObjWc_Obj07_c_ClsnFile))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov029_021122b4, 0x021122b4, size 0x28 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov029_021122b4
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);

void func_ov029_021122b4(void *vself)
{
    char *self = (char *)vself;
    Matrix4x3_FromTranslation(self + 0xf0, *(int *)(self + 0x5c) >> 3, *(int *)(self + 0x344) >> 3, *(int *)(self + 0x64) >> 3);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov029_02112250, 0x02112250, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov029_02112250
/* recovered: shared common types */
#include "common.h"
extern "C" {

struct MMC { char p[0x124]; };
struct Obj { char p[0x2ec]; Matrix4x3 m; };
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(MMC*, Matrix4x3&, short);
void func_ov029_02112250(void* vself){
    char* self = (char*)vself;
    Obj* o = (Obj*)self;
    o->m = *(Matrix4x3*)(self + 0xf0);
    *(int*)(self+0x310) = *(int*)(self+0x5c);
    *(int*)(self+0x314) = *(int*)(self+0x344);
    *(int*)(self+0x318) = *(int*)(self+0x64);
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s((MMC*)(self+0x124), o->m, *(short*)(self+0x8e));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14daObjWc_Mizu_cD0Ev, 0x021121f0, size 0x60 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* mangled body: C linkage so the name is emitted verbatim */
// @symbol _ZN14daObjWc_Mizu_cD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why two vptr stores appear -- then return the object to its heap.
 *
 * Written as a mangled body rather than as a real `~daObjWc_Mizu_c()` member.
 * A real member definition makes mwccarm emit the D2/D1/D0 triple as ONE
 * group, in the order D0-then-D1, while the cartridge keeps D1 (0x021121a4)
 * BELOW D0 (0x021121f0); objisolate then refuses with `requested functions are
 * not emitted in ROM order`, and the group also carries an unhomed D2. The
 * manifest flagged this in advance as
 * `functions_occur_in_expected_order: PARTIAL -- [(0, 1)]`.
 */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_TextureTransformer.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
extern int _ZTV10dBgActor_c[];
/* vtable identified: VT0 = _ZTV14daObjWc_Mizu_c; VT1 = _ZTV10dBgActor_c */
extern void *data_020a0eac;
int *_ZN14daObjWc_Mizu_cD0Ev(int *t)
{
    t[0] = (int)_ZTV14daObjWc_Mizu_c;
    _ZN18TextureTransformerD1Ev((char *)t + 0x320);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN10dBgW_KcMbgD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN14daObjWc_Mizu_cD1Ev, 0x021121a4, size 0x4c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* mangled body: C linkage so the name is emitted verbatim */
// @symbol _ZN14daObjWc_Mizu_cD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header
 *
 * Two vtable stores and four destructor calls, every one a consequence of
 * `struct daObjWc_Mizu_c : dBgActor_c`: its own vptr, then its own
 * TextureTransformer member at +0x320, then dBgActor_c's vptr -- stored
 * inline, because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's dBgW_KcMbg at +0x124 and Model at +0xd4, then dActor_c.
 *
 * A mangled body for the same emission-order reason given above D0.
 */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_TextureTransformer.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
extern int _ZTV10dBgActor_c[];
/* vtable identified: VT0 = _ZTV14daObjWc_Mizu_c; VT1 = _ZTV10dBgActor_c */
int *_ZN14daObjWc_Mizu_cD1Ev(int *t)
{
    t[0] = (int)_ZTV14daObjWc_Mizu_c;
    _ZN18TextureTransformerD1Ev((char *)t + 0x320);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN10dBgW_KcMbgD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
}
