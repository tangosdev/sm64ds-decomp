//cpp
/* HAND-ASSEMBLED translation unit -- ov029/SwitchActivatedPlank (9 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02112630  src/_ZN20SwitchActivatedPlankD1Ev.c
 *   [1] 0x0211267c  src/_ZN20SwitchActivatedPlankD0Ev.c
 *   [2] 0x021126dc  src/unnamed/ov029/0211/func_ov029_021126dc.c
 *   [3] 0x02112710  src/unnamed/ov029/0211/func_ov029_02112710.c
 *   [4] 0x02112750  src/game/actors/SwitchActivatedPlank/_ZN20SwitchActivatedPlank16CleanupResourcesEv.cpp
 *   [5] 0x02112794  src/game/actors/SwitchActivatedPlank/_ZN20SwitchActivatedPlank6RenderEv.cpp
 *   [6] 0x021127cc  src/game/actors/SwitchActivatedPlank/_ZN20SwitchActivatedPlank8BehaviorEv.cpp
 *   [7] 0x021128b0  src/game/actors/SwitchActivatedPlank/_ZN20SwitchActivatedPlank13InitResourcesEv.cpp
 *   [8] 0x02112964  src/game/actors/SwitchActivatedPlank/SwitchActivatedPlank_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- SwitchActivatedPlank_Spawn, 0x02112964, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol SwitchActivatedPlank_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV20SwitchActivatedPlank */
int *SwitchActivatedPlank_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(936);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* This TU emits the vtable object, so its symbol includes the two-word
         * Itanium preamble.  The ROM/config symbol names the slot array. */
        p[0] = (int)(_ZTV20SwitchActivatedPlank + 2);
        _ZN5ModelC1Ev((char *)p + 0x320);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN20SwitchActivatedPlank13InitResourcesEv, 0x021128b0, size 0xb4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20SwitchActivatedPlank13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SwitchActivatedPlank.h"
typedef int Fix12i;
struct SharedFilePtr; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
/* Model and ModelBase are the real classes now, through this actor's header --
 * and since #1643 so are dBgW_Kc and dBgW_KcMbg, which arrive via
 * SwitchActivatedPlank.h -> include/dBgW_Kc.h / include/dBgW_KcMbg.h. The two
 * one-int placeholder definitions that used to stand in for them here made the
 * whole TU fail to compile ("class 'dBgW_Kc' redefined", then an internal
 * compiler error at the D1 body), and nothing noticed: an unbuildable source
 * file is an ABSENT one, so every byte gate stayed green over a TU that produced
 * no object at all. Only the references were repaired in #1667; this is the
 * type-level half the reference gate cannot see. All 9 functions byte-match the
 * ROM again with the placeholders gone.
 */

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase*, BMD_File*, int, int);
extern "C" KCL_File* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg*, KCL_File*, const Matrix4x3&, Fix12i, short, CLPS_Block&);
extern "C" void func_020393d4(int* p, int v);

extern SharedFilePtr data_ov029_0211432c;
extern SharedFilePtr data_ov029_02114324;
extern CLPS_Block data_ov029_0211304c;
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

int SwitchActivatedPlank::InitResources()
{
    char* c = ((char*)this);
    BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_0211432c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0x320), bmd, 1, -1);
    func_ov029_02112710(c);
    func_ov029_021126dc(c);
    {
        KCL_File* kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov029_02114324);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (dBgW_KcMbg*)(c + 0x124), kcl, *(const Matrix4x3*)(c + 0x370),
            0x199, *(short*)(c + 0x8e), data_ov029_0211304c);
    }
    func_020393d4((int*)(c + 0x124), (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    *(unsigned char*)(c + 0x3a2) = 0;
    *(short*)(c + 0x3a0) = 0;
    *(unsigned char*)(c + 0x3a3) = 0;
    *(unsigned char*)(c + 0x3a4) = (unsigned char)(*(int*)(c + 8) & 0x1f);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN20SwitchActivatedPlank8BehaviorEv, 0x021127cc, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20SwitchActivatedPlank8BehaviorEv
/* recovered: named members + shared header */
#include "SwitchActivatedPlank.h"
#include "dBgW.h"
extern "C" {
void func_020393a4(void* p, int v);
int _ZN5Event6GetBitEj(unsigned int);
void func_ov029_021126dc(char* c);
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void*, void*, int);

#pragma optimize_for_size on

int _ZN20SwitchActivatedPlank8BehaviorEv(struct SwitchActivatedPlank *self) {
    func_020393a4(((char*)self)+0x124, 0x100000);

    switch(self->mState){
    case 0:
        if(_ZN5Event6GetBitEj(self->mEventID) == 0) break;

        {
            unsigned char* st_ptr = (unsigned char*)(((int)((char*)self) + 0x3a2));
            *st_ptr = *st_ptr + 1;
        }

        *(short*)(((char*)self) + 0x300 + 0xa0) = 0;
        self->mVisible = 1;

        ((dBgW *)(((char*)self)+0x124))->Enable((dActor_c *)(((char*)self)));
        func_ov029_021126dc(((char*)self));
        _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(((char*)self)+0x124, ((char*)self)+0x370, self->mAngleY);
        break;

    case 1: {
        unsigned short* p = (unsigned short*)((char*)&self->mStateTimer);
        if (*(unsigned short*)(((char*)self) + 0x300 + 0xa0) > 0x168) {
            self->mVisible = *(unsigned short*)(((char*)self) + 0x300 + 0xa0) & 1;
        }
        *p = *p + 1;
        if (_ZN5Event6GetBitEj(self->mEventID) != 0) break;
        ((dBgW *)((char*)&self->mMeshCollider))->Disable();
        self->mState = 0;
        self->mVisible = 0;
        break;
    }
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN20SwitchActivatedPlank6RenderEv, 0x02112794, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20SwitchActivatedPlank6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "SwitchActivatedPlank.h"
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(int); };

int SwitchActivatedPlank::Render()
{
  if (mVisible != 0) {
    Sub *s = (Sub*)((char *)&mModel2);
    s->v5(0);
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN20SwitchActivatedPlank16CleanupResourcesEv, 0x02112750, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20SwitchActivatedPlank16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SwitchActivatedPlank.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

int SwitchActivatedPlank::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled()) {
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    }
    ((SharedFilePtr *)&data_ov029_0211432c)->Release();
    ((SharedFilePtr *)&data_ov029_02114324)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov029_02112710, 0x02112710, size 0x40 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void Matrix4x3_FromRotationY(void *, int);
void func_ov029_02112710(char *t)
{
    Matrix4x3_FromRotationY(t + 0x33c, *(short *)(t + 0x8e));
    *(int *)(t + 0x360) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x364) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x368) = *(int *)(t + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov029_021126dc, 0x021126dc, size 0x34 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void Matrix4x3_FromRotationY(void *, int);

void func_ov029_021126dc(char *c) {
    Matrix4x3_FromRotationY(c + 0x370, *(short *)(c + 0x8e));
    *(int *)(c + 0x394) = *(int *)(c + 0x5c);
    *(int *)(c + 0x398) = *(int *)(c + 0x60);
    *(int *)(c + 0x39c) = *(int *)(c + 0x64);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0/1 -- mwcc emits D1 and D0 from this one C++ definition. */
/* -------------------------------------------------------------------------- */
SwitchActivatedPlank::~SwitchActivatedPlank()
{
}
