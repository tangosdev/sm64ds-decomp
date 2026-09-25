//cpp
/* daObjKi_Fune_c -- the Jolly Roger Bay ship (KI_FUNE / KI_FUNE_UP), ov016.
 *
 * One class drives both halves of the pair: actor 0x39 takes model index 0,
 * the ship that rocks on a sine of mBobAngle and loops a long sound while
 * the player is within 3000.0; anything else takes index 1, the still hull,
 * whose Behavior only keeps its collider live. InitResources succeeds for
 * index 0 only once star 1 of SublevelToLevel(8) is collected (with
 * data_0209f220 > 1), and for index 1 only until then, so the two never
 * coexist.
 *
 * Every member of this class is defined here. This TU owns the whole seven
 * function linker run 0x0211260c..0x021129a0 as one `complete` span.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order. The reading order below is
 * therefore InitResources, Behavior, Render, CleanupResources,
 * func_ov016_021126a8. The destructor is defined in the class body, in
 * include/daObjKi_Fune_c.h: that form -- and not an out-of-line definition --
 * is what makes the compiler emit the complete-object D1 ahead of the deleting
 * D0, with no D2 at all, which is the order and the set the cartridge has,
 * D1 at 0x0211260c and D0 at 0x02112650. Measured against this exact compiler,
 * not assumed. Do not reorder.
 *
 * KEY-FUNCTION SIDE EFFECTS. The inline destructor moves the key function to
 * InitResources, the first non-inline virtual, so this TU also emits the class
 * vtable and the whole inherited RTTI chain. Every one of those records
 * resolves to a cartridge address and is licensed in the manifest as
 * compiler-only deadstrip-data; the retail data bands stay under legacy
 * ownership and no symbols.txt row was added or moved for them:
 *   _ZTV14daObjKi_Fune_c  .data  ov016 0x02114a3c (public address point; the
 *                                emitted C++ object starts eight bytes earlier,
 *                                at 0x02114a34)
 *   _ZTI14daObjKi_Fune_c  .data  ov016 0x021149dc
 *   _ZTS14daObjKi_Fune_c  .data  ov016 0x021149e8
 * plus the inherited fBase_c / dBase_c / dActor_c / dBgActor_c _ZTI and _ZTS
 * pairs, each verified against its own module's canonical address.
 *
 * NOT OWNED HERE, and this TU neither declares nor verifies any of it:
 *   g_profile_KI_FUNE     .data  ov016 0x021149fc
 *   g_profile_KI_FUNE_UP  .data  ov016 0x02114a18
 * The two factories those profiles point at,
 * daObjKi_Fune_c_classInit_KI_FUNE_UP at 0x021129a0 and
 * daObjKi_Fune_c_classInit_KI_FUNE at 0x021129d0, sit directly above this run
 * and are left outside it; build/tu_map.json does not join them, and the
 * sibling that did join its factory (src/game/actors/d_a_obj_ki_ita.cpp) had
 * one, not two, to argue for.
 *
 * Known limits:
 *   Render still reaches dBgActor_c::mModel through a local `Base`
 *   shape instead of Model's own declaration, so slot 5 of that vtable is still
 *   unnamed. Both spellings reproduce the bytes; naming the slot is a Model.h
 *   question and not this TU's to answer.
 *   func_020393a4 / func_020393d4 are still the linker names of two
 *   arm9 dBgW helpers, and data_02082214 of the arm9 sine table Behavior indexes
 *   with mBobAngle. Naming those belongs in arm9.
 *   data_ov016_021136dc, data_ov016_021136e4 and data_ov016_021149d4 are
 *   still the linker names of this overlay's per-model file and CLPS tables.
 *   They are indexed by mModelIndex and this TU does not own them.
 *   func_ov016_021126a8 keeps its address-derived linker name and its
 *   `char *` parameter. The ROM spells neither, so nothing is coined for it
 *   here and include/decl_common.h's declaration of it is deliberately kept.
 */

/* Union of the seven legacy files' includes, first-seen in ROM-ascending
 * processing order. daObjKi_Fune_c.h leads because it pulls dBgActor_c.h,
 * which must see common.h's Matrix4x3 before Model.h offers the other
 * spelling. */
#include "daObjKi_Fune_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* Render's local view of the object at +0xd4 -- dBgActor_c::mModel -- reaching
 * its slot 5. See Known limits in the file banner. */
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

// @symbol _ZN14daObjKi_Fune_c13InitResourcesEv
int daObjKi_Fune_c::InitResources()
{
    void* clpsBlocks[2];
    unsigned int idx;
    void* file;
    int isModel0;
    clpsBlocks[0] = data_ov016_021149d4[0];
    clpsBlocks[1] = data_ov016_021149d4[1];
    isModel0 = (int)(actorID == 0x39);
    if (isModel0 != 0) mModelIndex = 0;
    else mModelIndex = 1;
    idx = mModelIndex;
    file = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_021136e4[idx]);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, file, 1, -1);
    func_ov016_021126a8(((char*)this));
    UpdateClsnPosAndRot();
    idx = mModelIndex;
    file = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov016_021136dc[idx]);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char*)this)+0x124, file, ((char*)this)+0x2ec, 0x1000, mAngleY, clpsBlocks[idx]);
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

// @symbol _ZN14daObjKi_Fune_c8BehaviorEv
int daObjKi_Fune_c::Behavior()
{
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled() == 0){
    ((dBgW *)(((char*)this)+0x124))->Enable((dActor_c *)(((char*)this)));
  }
  func_020393a4((int*)((char*)&mMeshCollider), 0x2000000);
  if(mModelIndex == 0){
    /* mBobAngle, through a short: the u16 member spelling fails ov016. */
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

// @symbol _ZN14daObjKi_Fune_c6RenderEv
int daObjKi_Fune_c::Render()
{
 Base *model = &((Derived *)this)->base; model->m(0); return 1;
}

// @symbol _ZN14daObjKi_Fune_c16CleanupResourcesEv
int daObjKi_Fune_c::CleanupResources()
{
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled())
    ((dBgW *)((char*)&mMeshCollider))->Disable();
  ((SharedFilePtr *)(data_ov016_021136e4[mModelIndex]))->Release();
  ((SharedFilePtr *)(data_ov016_021136dc[mModelIndex]))->Release();
  return 1;
}

// @symbol func_ov016_021126a8
extern "C" {
void func_ov016_021126a8(char *t)
{
    Matrix4x3_FromRotationXYZExt(t + 0xf0, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
}

