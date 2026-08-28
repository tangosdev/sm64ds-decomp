//cpp
/* HAND-ASSEMBLED translation unit -- ov029/WDW_Water (9 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021121a4  src/_ZN9WDW_WaterD1Ev.cpp
 *   [1] 0x021121f0  src/_ZN9WDW_WaterD0Ev.cpp
 *   [2] 0x02112250  src/func_ov029_02112250.cpp
 *   [3] 0x021122b4  src/func_ov029_021122b4.c
 *   [4] 0x021122dc  src/_ZN9WDW_Water16CleanupResourcesEv.cpp
 *   [5] 0x02112320  src/_ZN9WDW_Water6RenderEv.cpp
 *   [6] 0x02112354  src/_ZN9WDW_Water8BehaviorEv.cpp
 *   [7] 0x021124d0  src/_ZN9WDW_Water13InitResourcesEv.cpp
 *   [8] 0x021125f8  src/WDW_Water_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- WDW_Water_Spawn, 0x021125f8, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol WDW_Water_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9WDW_Water */
int *WDW_Water_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(840);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)&_ZTV9WDW_Water[2]; /* +8: this TU defines the vtable */
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN9WDW_Water13InitResourcesEv, 0x021124d0, size 0x128 */
/* -------------------------------------------------------------------------- */
#include "types.h"
// @symbol _ZN9WDW_Water13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WDW_Water.h"
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

int WDW_Water::InitResources()
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
/* ROM ordinal 6 -- _ZN9WDW_Water8BehaviorEv, 0x02112354, size 0x17c */
/* -------------------------------------------------------------------------- */
#include "types.h"
// @symbol _ZN9WDW_Water8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WDW_Water.h"
#include "dBgW.h"
extern "C" {
extern int IsAreaShowing(int idx);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned a, unsigned b, unsigned c, void *pos, unsigned e);
extern void _ZN9Animation7AdvanceEv(void *a);
}
extern s16 data_02082214[];
extern int data_0209f32c;

int WDW_Water::Behavior()
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
/* ROM ordinal 5 -- _ZN9WDW_Water6RenderEv, 0x02112320, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9WDW_Water6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "WDW_Water.h"
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base { char pad[0xd4]; Sub sub; };
extern "C" void _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);

int WDW_Water::Render()
{
 _ZN18TextureTransformer6UpdateER15ModelComponents((char *)((Base *)this) + 0x320, (char *)((Base *)this) + 0xdc); Sub *b = &((Base *)this)->sub; b->m(0); return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN9WDW_Water16CleanupResourcesEv, 0x021122dc, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9WDW_Water16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WDW_Water.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
extern int RotatingPlatformWdw_ClsnFile[];
extern int RotatingPlatformWdw_ModelFile[];

int WDW_Water::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(RotatingPlatformWdw_ModelFile))->Release();
    ((SharedFilePtr *)(RotatingPlatformWdw_ClsnFile))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov029_021122b4, 0x021122b4, size 0x28 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
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
/* ROM ordinal 1 -- _ZN9WDW_WaterD0Ev, 0x021121f0, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9WDW_WaterD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~WDW_Water()` is enough, because mwcc
 * emits D1 and D0 together from the inline body in the header, and objisolate
 * keeps the one this file is bound to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
#include "WDW_Water.h"

/* (no separate definition: the single ~WDW_Water() below emits the D0 and D1
 * variants together.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN9WDW_WaterD1Ev, 0x021121a4, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9WDW_WaterD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct WDW_Water : dBgActor_c`: its own vptr, then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds no
 * member with a destructor of its own.
 */
#include "WDW_Water.h"

