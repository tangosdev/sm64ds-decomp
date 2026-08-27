//cpp
/* HAND-ASSEMBLED translation unit -- ov029/WaterDiamond (10 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111728  src/game/actors/WaterDiamond/_ZN12WaterDiamondD1Ev.cpp
 *   [1] 0x02111760  src/_ZN12WaterDiamondD0Ev.c
 *   [2] 0x021117ac  src/func_ov029_021117ac.cpp
 *   [3] 0x02111850  src/func_ov029_02111850.cpp
 *   [4] 0x021118c8  src/func_ov029_021118c8.c
 *   [5] 0x02111908  src/_ZN12WaterDiamond16CleanupResourcesEv.c
 *   [6] 0x0211192c  src/game/actors/WaterDiamond/_ZN12WaterDiamond6RenderEv.cpp
 *   [7] 0x02111954  src/game/actors/WaterDiamond/_ZN12WaterDiamond8BehaviorEv.cpp
 *   [8] 0x02111a04  src/game/actors/WaterDiamond/_ZN12WaterDiamond13InitResourcesEv.cpp
 *   [9] 0x02111a84  src/game/actors/WaterDiamond/WaterDiamond_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- WaterDiamond_Spawn, 0x02111a84, size 0x40 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol WaterDiamond_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#define WATERDIAMOND_H  /* suppress include/WaterDiamond.h: its WaterDiamond is the flat C view without the destructor; the TU-local class below is the C++ view, same evidenced offsets */
#include "Model.h"
#include "dCcAc_c.h"

extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

struct dActor_c {   /* shadow base: vptr at 0, evidenced dActor_c fields, size 0xd4 */
    char  pad0[0x4];
    s32   mParam;                /* 0x008 */
    char  pad0c[0x54];
    s32   mPosY;                 /* 0x060 */
    char  pad64[0x2a];
    s16   mAngleY;               /* 0x08e */
    char  pad90[0x44];
    virtual ~dActor_c();
    static dActor_c *FindWithID(unsigned int id);
    /* dActor_c's own inline operator delete (include/dActor_c.h): what lets the
       single ~WaterDiamond() reproduce the ROM's deleting destructor D0 */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }
};
struct WaterDiamond : dActor_c {
    Model mModel;                            /* 0x0d4 */
    dCcAc_c mdCcAc_c;  /* 0x124 */
    s32 mWaterID;                             /* 0x158 */
    s8  mWaterParam;
    u8  mActive;
    virtual ~WaterDiamond();
    int Behavior();
    int InitResources();
    int Render();
};
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12WaterDiamond */
int *WaterDiamond_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(352);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV12WaterDiamond[2]; /* +8: this TU defines the vtable */
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN7dCcAc_cC1Ev((char *)p + 0x124);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN12WaterDiamond13InitResourcesEv, 0x02111a04, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12WaterDiamond13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterDiamond.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* bmd, int a, int b);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, int fix, int t, unsigned int u, unsigned int v);
}

int WaterDiamond::InitResources()
{
  void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_02114270);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, m, 1, -1);
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this)+0x124, ((char*)this), 0x32000, 0x64000, 0x800002, 0);
  mWaterID = 0;
  mWaterParam = mParam & 1;
  mActive = 0;
  mAngleY = 0;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN12WaterDiamond8BehaviorEv, 0x02111954, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12WaterDiamond8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterDiamond.h"
typedef short s16;
extern "C" {
extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN5dCc_c5ClearEv(void* self);
extern void _ZN5dCc_c6UpdateEv(void* self);
}

int WaterDiamond::Behavior()
{
    func_ov029_02111850(((char*)this));
    if (mWaterID == 0) return 1;
    func_ov029_021117ac(((char*)this));
    if (mActive != 0) {
        char* p;
        s16* a = (s16*)(((int)((char*)this) + 0x8e));
        *a = *a + 0x400;
        p = _ZN8dActor_c10FindWithIDEj(mWaterID);
        if (p != 0) {
            if (mPosY == *(int*)(p+0x60)) {
                if (mAngleY == 0) mActive = 0;
            }
            if (mPosY != *(int*)(p+0x334)) {
                mActive = 0;
                mAngleY = 0;
            }
        }
    }
    func_ov029_021118c8(((char*)this));
    _ZN5dCc_c5ClearEv((void*)((char*)&mdCcAc_c));
    _ZN5dCc_c6UpdateEv((void*)((char*)&mdCcAc_c));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN12WaterDiamond6RenderEv, 0x0211192c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12WaterDiamond6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "WaterDiamond.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };

int WaterDiamond::Render()
{
 Base *b = &((Derived *)this)->base; b->m(0); return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN12WaterDiamond16CleanupResourcesEv, 0x02111908, size 0x24 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov029_02114270[];
int _ZN12WaterDiamond16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov029_02114270);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov029_021118c8, 0x021118c8, size 0x40 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void Matrix4x3_FromRotationY(void *, int);
void func_ov029_021118c8(char *t)
{
    Matrix4x3_FromRotationY(t + 0xf0, *(short *)(t + 0x8e));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov029_02111850, 0x02111850, size 0x78 */
/* -------------------------------------------------------------------------- */
extern "C" {
char* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
void _ZN7fBase_c18MarkForDestructionEv(void*);
void func_ov029_02111850(char* c){
  char* a;
  if(*(int*)(c+0x158) != 0) return;
  a = 0;
  for(;;){
    a = _ZN8dActor_c15FindWithActorIDEjPS_(0x65, a);
    if(a == 0) goto done;
    if(*(unsigned char*)(c+0x15c) == *(unsigned char*)(a+0x338)) break;
  }
  *(int*)(c+0x158) = *(int*)(a+4);
done:
  if(*(int*)(c+0x158) != 0) return;
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov029_021117ac, 0x021117ac, size 0xa4 */
/* -------------------------------------------------------------------------- */
struct Vector3;
/* (dActor_c: TU-local class above carries FindWithID) */
struct Sound { static void PlayBank3(unsigned int id, const Vector3 &v); };

extern "C" void func_ov029_021117ac(char *c)
{
    if (*(unsigned char *)(c + 0x15d) != 0)
        return;
    unsigned int id = *(unsigned int *)(c + 0x148);
    if (id == 0)
        return;
    char *a = (char *)dActor_c::FindWithID(id);
    if (a == 0)
        return;
    int b = (int)(*(unsigned short *)(a + 0xc) == 0xbf);
    if (b == 0)
        return;
    char *o = (char *)dActor_c::FindWithID(*(unsigned int *)(c + 0x158));
    int v = *(int *)(o + 0x334);
    if (v != *(int *)(o + 0x60))
        return;
    if (v == *(int *)(c + 0x60))
        return;
    *(unsigned char *)(c + 0x15d) = 1;
    *(int *)(o + 0x334) = *(int *)(c + 0x60);
    Sound::PlayBank3(0x63, *(Vector3 *)(c + 0x74));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN12WaterDiamondD0Ev, 0x02111760, size 0x4c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol _ZN12WaterDiamondD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV15daObjWc_Obj03_c */
extern void *data_020a0eac;
/* (no separate definition: the single ~WaterDiamond() below emits the D0 and D1
 * variants together -- a hand-mangled D0 next to a real destructor is the
 * known mwccarm ICE, ELFgen.c:483.) */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN12WaterDiamondD1Ev, 0x02111728, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12WaterDiamondD1Ev

/* (the TU-local WaterDiamond at the top of this file replaces the legacy
 * shadow world; real Model/dCcAc_c members, evidenced offsets) */

WaterDiamond::~WaterDiamond()
{
}

