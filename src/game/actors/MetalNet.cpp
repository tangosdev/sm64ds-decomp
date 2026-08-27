//cpp
/* Canonical production translation unit -- reconstructed with tools/tubuild.py.
 * ov009/MetalNet  (8 function(s))
 *
 * The default stock build compiles this file once, derives its 8
 * exact text contributions, and substitutes them for retained one-function
 * comparison objects. The manifest partial-link proof and ordinary ROM build
 * both require the linked module and final ROM to remain byte-identical.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111dc4  src/game/actors/MetalNet/_ZN8MetalNetD1Ev.cpp
 *   [1] 0x02111e08  src/game/actors/MetalNet/_ZN8MetalNetD0Ev.cpp
 *   [2] 0x02111e60  src/game/actors/MetalNet/_ZN8MetalNet16CleanupResourcesEv.cpp
 *   [3] 0x02111ea4  src/_ZN8MetalNet16OnPendingDestroyEv.c
 *   [4] 0x02111ea8  src/game/actors/MetalNet/_ZN8MetalNet6RenderEv.cpp
 *   [5] 0x02111ed0  src/game/actors/MetalNet/_ZN8MetalNet8BehaviorEv.cpp
 *   [6] 0x02111f40  src/game/actors/MetalNet/_ZN8MetalNet13InitResourcesEv.cpp
 *   [7] 0x02112048  src/game/actors/MetalNet/MetalNet_Spawn.c
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "MetalNet.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'Base' */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

/* shadow struct 'Derived' */
struct Derived { char pad[0xd4]; Base base; };

/* shadow typedef 'short' */
typedef short s16;

/* shadow struct 'SharedFilePtr' */
struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;

extern "C" {
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern unsigned char data_0209f2d8;
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
struct KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
unsigned char NumStars(void);
extern struct SharedFilePtr data_ov009_02113e90;
extern struct SharedFilePtr data_ov009_02113e88;
extern struct CLPS_Block data_ov009_02112bf8;
extern int data_0209caa0;
/* TUBUILD CONFLICT -- alternate declaration of _ZN10dBgActor_c21UpdateModelPosAndRotYEv, from the legacy file for _ZN8MetalNet13InitResourcesEv, NOT applied: void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN10dBgActor_c19UpdateClsnPosAndRotEv, from the legacy file for _ZN8MetalNet13InitResourcesEv, NOT applied: void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self); */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- MetalNet_Spawn, 0x02112048, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol MetalNet_Spawn
/* recovered: globals resolved, declarations from a shared header */
/* recovered: globals resolved */
/* resolved: VT = _ZTV8MetalNet */
extern "C" int *MetalNet_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)&_ZTV8MetalNet[2]; /* +8: this TU defines the vtable; mwcc's symbol addresses the object start, the ROM stores the slot array */ }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN8MetalNet13InitResourcesEv, 0x02111f40, size 0x108 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8MetalNet13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int MetalNet::InitResources()
{
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov009_02113e90);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((unsigned char *)this) + 0xd4, bmd, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((unsigned char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((unsigned char *)this));
    {
        struct KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov009_02113e88);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            ((unsigned char *)this) + 0x124, kcl, *(struct Matrix4x3 *)((unsigned char *)&(*(u8 *)&mClsnMat)), 0x1000,
            mAngleY, data_ov009_02112bf8);
    }
    if (((*(s32 *)&param1) & 0xff) == 0xff) {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) goto ret1;
        if ((*(int*)((char*)&data_0209caa0 + 8) & 0x80000) == 0) goto ret1;
        return 0;
    } else {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) goto ret1;
        if (NumStars() >= 0x96) return 0;
    }
ret1:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN8MetalNet8BehaviorEv, 0x02111ed0, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8MetalNet8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int MetalNet::Behavior()
{
    int b;
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((void*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((void*)this));
    b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        if (((dBgW *)((char*)&mMeshCollider))->IsEnabled() == 0)
            ((dBgW *)((char*)((void*)this)+0x124))->Enable((dActor_c *)(((void*)this)));
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((void*)this), 0, 0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN8MetalNet6RenderEv, 0x02111ea8, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8MetalNet6RenderEv
/* recovered: named members + shared header, real C++ method */
int MetalNet::Render()
{
 Base *b = &((Derived *)this)->base; b->m(0); return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN8MetalNet16OnPendingDestroyEv, 0x02111ea4, size 0x4 */
/* -------------------------------------------------------------------------- */
extern "C" void _ZN8MetalNet16OnPendingDestroyEv(void)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN8MetalNet16CleanupResourcesEv, 0x02111e60, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8MetalNet16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int MetalNet::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)(&data_ov009_02113e90))->Release();
    ((SharedFilePtr *)(&data_ov009_02113e88))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN8MetalNetD0Ev, 0x02111e08, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8MetalNetD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~MetalNet()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
/* (no separate definition: the single ~MetalNet() below emits the D0 and D1
 * variants together; mwccarm orders the variant group itself.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN8MetalNetD1Ev, 0x02111dc4, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8MetalNetD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct MetalNet : dBgActor_c`: its own vptr, then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds no
 * member with a destructor of its own.
 */
MetalNet::~MetalNet()
{
}
