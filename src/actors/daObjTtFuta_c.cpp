//cpp
/**
 * Tiny-Huge Island's pounded lid (TT_FUTA).
 *
 * Ground-pounding it bursts particles, plays the bank sound, sets the
 * drain event bit and destroys it. InitResources loads the model and
 * collision files; the lid stays shut until event bit 0xe opens it.
 *
 * Leftover: Particle::System::NewSimple keeps its mangled spelling
 *   (Fix12<int> by value in the mangled name, wall 6az).
 * Leftover: Event::GetBit / Event::SetBit stay mangled (no Event
 *   header in this tree).
 * Leftover: dBgW_KcMbg::SetFile keeps its mangled spelling (by-value
 *   Fix12<int> parameters, wall 6az).
 * Leftover: dBgActor_c::IsClsnInRange keeps its mangled spelling
 *   (Fix12<int> in the mangled name, wall 6az per the ukiyuka base).
 * Leftover: data_ov033_02111bfc is the unnamed CLPS home this TU
 *   feeds SetFile.
 */

#include "daObjTtFuta_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"

/* BSS file homes; Init loads them, Cleanup releases them. */
extern SharedFilePtr data_ov033_021124c8;
extern SharedFilePtr data_ov033_021124c0;

extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern int data_ov033_02111bfc[];
extern int _ZN5Event6GetBitEj(unsigned int);
extern void _ZN5Event6SetBitEj(unsigned int b);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
}

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN13daObjTtFuta_cD1Ev
// @symbol _ZN13daObjTtFuta_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * Model and dBgW_KcMbg, then dActor_c. */
daObjTtFuta_c::~daObjTtFuta_c()
{
}

// @symbol _ZN13daObjTtFuta_c15OnGroundPoundedER8dActor_c
/* Real member function: `other`'s own position stays a raw pointer read
 * (`(char *)&other + 0x5c`, then `v[0..2]`) rather than three named-field
 * reads: the ROM computes one base address and offsets from it, not three
 * independent field loads -- member-access syntax compiled differently. */
void daObjTtFuta_c::OnGroundPounded(dActor_c &other)
{
    int *v = (int *)(((char *)&other) + 0x5c);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x28, v[0], v[1], v[2]);
    Sound::PlayBank3(0xf, *(const Vector3 *)&mCamSpacePosX);
    _ZN5Event6SetBitEj(0xe);
    MarkForDestruction();
}

// @symbol _ZN13daObjTtFuta_c16CleanupResourcesEv
s32 daObjTtFuta_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov033_021124c8.Release();
    data_ov033_021124c0.Release();
    return 1;
}

// @symbol _ZN13daObjTtFuta_c6RenderEv
s32 daObjTtFuta_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN13daObjTtFuta_c8BehaviorEv
s32 daObjTtFuta_c::Behavior() {
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    return 1;
}

// @symbol _ZN13daObjTtFuta_c13InitResourcesEv
s32 daObjTtFuta_c::InitResources() {
  void *f = Model::LoadFile(data_ov033_021124c8);
  mModel.SetFile((BMD_File *)f, 1, -1);
  UpdateModelPosAndRotY();
  UpdateClsnPosAndRot();
  void *k = dBgW_Kc::LoadFile(data_ov033_021124c0);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, k, &mClsnMat, 0x199, mAngleY, data_ov033_02111bfc);
  return _ZN5Event6GetBitEj(0xe) == 0;
}
