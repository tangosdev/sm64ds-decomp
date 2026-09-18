//cpp
/* SHADOW translation unit -- ov016 / ShipUp (RTTI name daObjKi_Fune_c).
 *
 * The Jolly Roger Bay sunken ship. One class drives both halves of the pair:
 * actor 0x39 takes model index 0, the bobbing wreck that rides a sine table and
 * loops a long sound while the player is near; anything else takes index 1, the
 * still hull, whose Behavior only keeps its collider live.
 *
 * NOT ENROLLED, AND NOT CANONICAL. This file contributes nothing to the ROM
 * build. The seven one-function sources under src/ remain the enrolled owners of
 * 0x0211260c..0x021129a0. See notes/translation-unit-reconstruction-plan.md (the
 * design) and notes/tu-reconstruction-pilot-report.md (what this measured).
 *
 * WHY IT IS STILL A SHADOW. Compiling the class here makes this the key-function
 * TU, so mwccarm emits the vtable and the whole inherited RTTI chain as a side
 * effect. Nine of those eleven records resolve to a cartridge address and are
 * licensed in the manifest as compiler-only deadstrip-data. The remaining two,
 * _ZTI6ShipUp and _ZTS6ShipUp, cannot be: an _ZTI/_ZTS record is a
 * length-prefixed mangled string, and the cartridge spells this class
 * daObjKi_Fune_c -- _ZTI14daObjKi_Fune_c at ov016 0x021149dc and
 * _ZTS14daObjKi_Fune_c at ov016 0x021149e8. Under the coined spelling the two
 * records match nothing in the ROM, so tubuild refuses to license them and
 * refuses the promotion. Giving the class its cartridge RTTI name is the
 * unblocker, and it is a separate change from this one.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order. The reading order below is
 * therefore InitResources, Behavior, Render, CleanupResources,
 * func_ov016_021126a8. The destructor is defined in the class body, in
 * include/ShipUp.h: that form -- and not an out-of-line definition -- is what
 * makes the compiler emit the complete-object D1 ahead of the deleting D0, with
 * no D2 at all, which is the order and the set the cartridge has. Measured
 * against this exact compiler, not assumed. Do not reorder.
 *
 * THIS TU ALSO OWNS, and this round neither declares nor verifies any of it:
 *   _ZTV6ShipUp          .data  0x02114a3c (slot array; the C++ object starts
 *                               8 bytes earlier, at 0x02114a34)
 *   _ZTI14daObjKi_Fune_c .data  0x021149dc
 *   _ZTS14daObjKi_Fune_c .data  0x021149e8
 *   g_profile_KI_FUNE    .data  0x021149fc
 *   g_profile_KI_FUNE_UP .data  0x02114a18
 * The two factories the profiles point at, daObjKi_Fune_c_classInit_KI_FUNE_UP
 * at 0x021129a0 and daObjKi_Fune_c_classInit_KI_FUNE at 0x021129d0, sit directly
 * above this run and are left outside it; build/tu_map.json does not join them,
 * and the sibling that did join its factory (src/game/actors/d_a_obj_ki_ita.cpp)
 * had one, not two, to argue for.
 *
 * deslop
 * Leftover: Render still reaches dBgActor_c::mModel through a local `Base`
 *   shape instead of Model's own declaration, so slot 5 of that vtable is still
 *   unnamed. Both spellings reproduce the bytes; naming the slot is a Model.h
 *   question and not this TU's to answer.
 * Leftover: func_020393a4 / func_020393d4 are still the linker names of two
 *   arm9 dBgW helpers, and data_02082214 of the arm9 sine table Behavior indexes
 *   with mBobAngle. Naming those belongs in arm9.
 * Leftover: data_ov016_021136dc, data_ov016_021136e4 and data_ov016_021149d4 are
 *   still the linker names of this overlay's per-model file and CLPS tables.
 *   They are indexed by mModelIndex and this TU does not own them.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x0211260c  src/_ZN6ShipUpD1Ev.cpp
 *   [1] 0x02112650  src/_ZN6ShipUpD0Ev.cpp
 *   [2] 0x021126a8  src/func_ov016_021126a8.c
 *   [3] 0x021126f0  src/_ZN6ShipUp16CleanupResourcesEv.cpp
 *   [4] 0x02112744  src/_ZN6ShipUp6RenderEv.cpp
 *   [5] 0x0211276c  src/_ZN6ShipUp8BehaviorEv.cpp
 *   [6] 0x0211283c  src/_ZN6ShipUp13InitResourcesEv.cpp
 */

/* Union of the seven legacy files' includes, first-seen in ROM-ascending
 * processing order. ShipUp.h leads because it pulls dBgActor_c.h, which must see
 * common.h's Matrix4x3 before Model.h offers the other spelling. */
#include "ShipUp.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* Render's local view of the object at +0xd4 -- dBgActor_c::mModel -- reaching
 * its slot 5. See the Leftover note in the file banner. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

struct Derived { char pad[0xd4]; Base base; };

extern "C" {
extern void Matrix4x3_FromRotationXYZExt(void *, int, int, int);
extern void func_020393a4(int* p, int v);
extern int _ZN8dActor_c13DistToCPlayerEv(void* a);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int e);
extern short data_02082214[];
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern void func_020393d4(int* p, int v);
extern int IsStarCollected(int a, int b);
extern void* _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern unsigned char data_0209f220;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN6ShipUp13InitResourcesEv, 0x0211283c, size 0x164 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6ShipUp13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
int ShipUp::InitResources()
{
    void* sp8[2];
    unsigned int idx;
    void* f;
    int b;
    sp8[0] = data_ov016_021149d4[0];
    sp8[1] = data_ov016_021149d4[1];
    b = (int)(actorID == 0x39);
    if (b != 0) mModelIndex = 0;
    else mModelIndex = 1;
    idx = mModelIndex;
    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_021136e4[idx]);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, f, 1, -1);
    func_ov016_021126a8(((char*)this));
    UpdateClsnPosAndRot();
    idx = mModelIndex;
    f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov016_021136dc[idx]);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char*)this)+0x124, f, ((char*)this)+0x2ec, 0x1000, mAngleY, sp8[idx]);
    if (mModelIndex == 0) {
        func_020393d4((int*)((char*)&mMeshCollider), (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    }
    ((dBgW *)(((char*)this)+0x124))->Enable((dActor_c *)(((char*)this)));
    mSoundHandle = 0;
    unk_328 = 0;
    if (data_0209f220 > 1) {
        if (IsStarCollected(SublevelToLevel(8), 1) != 0) {
            if (mModelIndex == 0) goto ret1;
            return 0;
        }
    }
    if (mModelIndex == 0) return 0;
ret1:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN6ShipUp8BehaviorEv, 0x0211276c, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6ShipUp8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
int ShipUp::Behavior()
{
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled() == 0){
    ((dBgW *)(((char*)this)+0x124))->Enable((dActor_c *)(((char*)this)));
  }
  func_020393a4((int*)((char*)&mMeshCollider), 0x2000000);
  if(mModelIndex == 0){
    *(short*)(((int)((char*)this) + 0x320)) += 0xda;
    mAngleX = (short)((*(short*)((char*)data_02082214 + ((mBobAngle>>4)<<2)) << 0xa) >> 0xc);
    if(_ZN8dActor_c13DistToCPlayerEv(((char*)this)) < 0xbb8000){
      mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 3, 0x8b, ((char*)this)+0x74, 0);
    }
    func_ov016_021126a8(((char*)this));
    UpdateClsnPosAndRot();
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN6ShipUp6RenderEv, 0x02112744, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6ShipUp6RenderEv
/* recovered: named members + shared header, real C++ method */
int ShipUp::Render()
{
 Base *b = &((Derived *)this)->base; b->m(0); return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN6ShipUp16CleanupResourcesEv, 0x021126f0, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6ShipUp16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
int ShipUp::CleanupResources()
{
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled())
    ((dBgW *)((char*)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(data_ov016_021136e4[mModelIndex]))->Release();
  ((SharedFilePtr *)(data_ov016_021136dc[mModelIndex]))->Release();
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov016_021126a8, 0x021126a8, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov016_021126a8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov016_021126a8(char *t)
{
    Matrix4x3_FromRotationXYZExt(t + 0xf0, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
}

