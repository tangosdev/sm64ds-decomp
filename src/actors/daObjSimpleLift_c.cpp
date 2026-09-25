//cpp
/**
 * daObjSimpleLift_c -- the plain sliding slab (ov091, 0x02132404..0x021327e8).
 *
 * Seven registry profiles share this one class: BK_TRANSBAR, KM2_RIFUT01,
 * KM2_RIFUT02, KM3_DERU01, KM3_DERU02, KM1_DERU and RC_RIFT01. Each classInit
 * allocates 0x330 and installs the same vtable, so the slab tells itself apart
 * by its spawn ID, which InitResources folds into a variant index 0..6 used to
 * pick the model, the collision mesh, the starting heading and the travel time.
 *
 * Behavior is a two-timer walk: while the pause timer is running the slab sits
 * still; once it expires the move timer counts the outbound leg down, and when
 * that reaches zero the slab turns a half circle (+0x8000 in the DS's 16-bit
 * angle) and waits 15 frames before setting off again. The mesh collider is
 * only re-placed when the slab is near enough to matter.
 */

#include "daObjSimpleLift_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "types.h"

/* Render reaches the inherited Model at 0xd4 through its own vtable. The
   model's class is not migrated yet, so the slot is spelled positionally
   here rather than by name. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };

/* ABI seam: everything below still answers to its linker name. Declared above
   the first @symbol marker so no member is charged with the spellings. */
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(char* c, Fix12i a, Fix12i b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* mc, void* kcl, void* mtx, int f, s16 s, void* clps);
void func_020393d4(void* p, void* v);
extern char data_ov091_02135028[]; /* per-variant collision file, stride 0xc */
extern char data_ov091_0213502c[]; /* per-variant CLPS block, stride 0xc */
extern u16 data_ov091_02134514[]; /* per-variant heading offset */
extern u16 data_ov091_02134504[]; /* per-variant travel time */
}

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN17daObjSimpleLift_cD1Ev
// @symbol _ZN17daObjSimpleLift_cD0Ev
/* One out-of-line definition; mwccarm emits D1 then D0 from it. Both bodies
 * are entirely consequences of `daObjSimpleLift_c : dBgActor_c`: this class's
 * vptr store, then dBgActor_c's -- inlined, because dBgActor_c defines its
 * destructor in its class body -- then dBgActor_c's Model and dBgW_KcMbg, then
 * dActor_c. This class adds no member that needs destroying. */
daObjSimpleLift_c::~daObjSimpleLift_c()
{
}

// @symbol _ZN17daObjSimpleLift_c16CleanupResourcesEv
/* Park the collider and give back the two files this variant borrowed. */
int daObjSimpleLift_c::CleanupResources()
{
  if(((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
    ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
  ((SharedFilePtr *)(*(void**)(data_ov091_02135024 + (unsigned char)((char *)this)[0x322]*0xc)))->Release();
  ((SharedFilePtr *)(*(void**)(data_ov091_02135028 + (unsigned char)((char *)this)[0x322]*0xc)))->Release();
  return 1;
}

// @symbol _ZN17daObjSimpleLift_c6RenderEv
int daObjSimpleLift_c::Render()
{
 Base *model = &((Derived *)this)->base; model->m(0); return 1;
}

// @symbol _ZN17daObjSimpleLift_c8BehaviorEv
/* Pause timer, then move timer; when the outbound leg runs out, turn a half
   circle and pause again. */
int daObjSimpleLift_c::Behavior()
{
  if (DecIfAbove0_Byte((unsigned char*)((char*)&mPauseTimer)) == 0) {
    if (DecIfAbove0_Short((unsigned short*)((char*)&mMoveTimer)) == 0) {
      s16* heading = (s16*)(((int)((char*)this) + 0x94));
      s16 travelTime = data_ov091_02134504[mVariant];
      mMoveTimer = travelTime;
      *heading += 0x8000;
      mPauseTimer = 0xf;
    } else {
      UpdatePos(0);
    }
  }
  UpdateModelPosAndRotY();
  if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0) != 0) {
    UpdateClsnPosAndRot();
  }
  return 1;
}

// @symbol _ZN17daObjSimpleLift_c13InitResourcesEv
/* Which slab this is comes from the actor ID; everything else follows from
   the variant index. */
int daObjSimpleLift_c::InitResources()
{
    u8* c = (u8*)((void*)this);
    u16 actorID = *(u16*)(c+0xc);
    switch (actorID) {
        case 0x37: mVariant = 6; break;
        case 0x7c: mVariant = 3; break;
        case 0x93: mVariant = 4; break;
        case 0x9b: mVariant = 2; break;
        case 0x8a: mVariant = 0; break;
        case 0x9a: mVariant = 1; break;
        case 0x92: mVariant = 5; break;
    }

    void* bmd = Model::LoadFile(**(SharedFilePtr**)(data_ov091_02135024 + mVariant*0xc));
    ((ModelBase*)(c+0xd4))->SetFile((BMD_File*)bmd, 1, -1);

    /* Heading to slide along: the spawn angle plus this slab's own offset,
       unless the spawn supplies one of its own. */
    mPrevAngleY = (u16)(mAngleY + data_ov091_02134514[mVariant]);

    if (mAngleX != 0) {
        mPrevAngleY = (u16)(mAngleY + mAngleX);
    }

    mMoveTimer = data_ov091_02134504[mVariant];
    mHorzSpeed = 0xa000;
    mBasePosX = mPosX;
    mBasePosY = mPosY;
    mBasePosZ = mPosZ;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    /* Slab 6 gets the full-size collider; the rest are scaled to 0x199. */
    if (mVariant == 6) {
        int tableOffset = mVariant * 0xc;
        void* kcl = dBgW_Kc::LoadFile(**(SharedFilePtr**)(data_ov091_02135028+tableOffset));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (void*)(c+0x124), kcl, (void*)(c+0x2ec), 0x1000, *(s16*)(c+0x8e), *(void**)(data_ov091_0213502c+tableOffset));
    } else {
        int tableOffset = mVariant * 0xc;
        void* kcl = dBgW_Kc::LoadFile(**(SharedFilePtr**)(data_ov091_02135028+tableOffset));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (void*)(c+0x124), kcl, (void*)(c+0x2ec), 0x199, *(s16*)(c+0x8e), *(void**)(data_ov091_0213502c+tableOffset));
    }
    func_020393d4((void*)(c+0x124), (void*)dBgW::UpdatePosWithTransform);
    return 1;
}
