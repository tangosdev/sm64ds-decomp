//cpp
/* Canonical production translation unit -- reconstructed with tools/tubuild.py.
 * ov009/Flag  (7 function(s))
 *
 * The default stock build compiles this file once, derives its 7
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
 *   [0] 0x02112078  src/game/actors/Flag/_ZN4FlagD1Ev.cpp
 *   [1] 0x021120a8  src/_ZN4FlagD0Ev.c
 *   [2] 0x021120ec  src/_ZN4Flag16CleanupResourcesEv.c
 *   [3] 0x0211211c  src/game/actors/Flag/_ZN4Flag6RenderEv.cpp
 *   [4] 0x02112144  src/game/actors/Flag/_ZN4Flag8BehaviorEv.cpp
 *   [5] 0x02112190  src/game/actors/Flag/_ZN4Flag13InitResourcesEv.cpp
 *   [6] 0x021121f0  src/game/actors/Flag/Flag_Spawn.c
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "math/Matrix.h"  /* first: Behavior needs the .r/.t spelling of Matrix4x3; whichever is seen first wins (see include/common.h) */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
#define FLAG_H  /* suppress include/Flag.h: its Flag is the flat C view without the destructor; the TU-local class below is the C++ view, same evidenced offsets */
#include "ModelAnim.h"
#include "decl_ActorBase.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

/* shadow base: vptr at 0, evidenced dActor_c fields, size 0xd4 with the vptr --
 * exactly the offsets include/Flag.h records (mPosX 0x5c, mAngleY 0x8e). The
 * inline operator delete is dActor_c's own (include/dActor_c.h), which is what
 * lets the single ~Flag() below reproduce the ROM's deleting destructor D0. */
struct dActor_c {
    char  pad0[0x58];
    int   mPosX, mPosY, mPosZ;   /* 0x5c 0x60 0x64 */
    char  pad68[0x26];
    short mAngleY;               /* 0x8e */
    char  pad90[0x44];
    virtual ~dActor_c();
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }
};

struct Flag : dActor_c {
    ModelAnim mModelAnim;        /* 0xd4 */
    virtual ~Flag();
    int Behavior();
    int InitResources();
    int Render();
};

/* shadow struct 'Base' */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

/* shadow struct 'Derived' */
struct Derived { char pad[0xd4]; Base base; };

extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov009_02113eb8[];
extern int data_ov009_02113eb0[];
extern void _ZN9Animation7AdvanceEv(void* c);
extern void Matrix4x3_FromRotationY(void* m, short angle);
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- Flag_Spawn, 0x021121f0, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol Flag_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV4Flag */
extern "C" int *Flag_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(312);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV4Flag[2]; /* +8: this TU defines the vtable */
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN4Flag13InitResourcesEv, 0x02112190, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN4Flag13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int Flag::InitResources()
{
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)((char*)this)+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov009_02113eb8), 1, -1);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)((char*)this)+0xd4, _ZN9Animation8LoadFileER13SharedFilePtr(data_ov009_02113eb0), 0, 0x1000, 0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN4Flag8BehaviorEv, 0x02112144, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN4Flag8BehaviorEv
/* recovered: named members + shared header, real C++ method */
int Flag::Behavior()
{
  _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim);
  Matrix4x3_FromRotationY(((char*)this)+0xf0, mAngleY);
  mModelAnim.mat4x3.t.x=mPosX>>3;
  mModelAnim.mat4x3.t.y=mPosY>>3;
  mModelAnim.mat4x3.t.z=mPosZ>>3;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN4Flag6RenderEv, 0x0211211c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN4Flag6RenderEv
/* recovered: named members + shared header, real C++ method */
int Flag::Render()
{
 Base *b = &((Derived *)this)->base; b->m(0); return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN4Flag16CleanupResourcesEv, 0x021120ec, size 0x30 */
/* -------------------------------------------------------------------------- */
extern "C" int _ZN4Flag16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov009_02113eb8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov009_02113eb0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN4FlagD0Ev, 0x021120a8, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN4FlagD0Ev
/* (no separate definition: the single ~Flag() below emits the D0 and D1
 * variants together; the deallocation is the shadow base's inline operator
 * delete. The legacy file stored _ZTV10daMcFlag_c -- the RTTI spelling of the
 * same vtable address _ZTV4Flag names.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN4FlagD1Ev, 0x02112078, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN4FlagD1Ev
Flag::~Flag()
{
}
