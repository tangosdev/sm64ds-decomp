//cpp
/* Tiny-Huge Island's pounded lid (TT_FUTA). Ground-pounding it bursts
 * particles, plays the bank sound, sets the drain event bit and destroys
 * it. The lid stays shut until event bit 0xe opens it.
 */
// leftovers:
// - Particle::System::NewSimple stays mangled. This TU's pound passes
//   Fix12<int> by value.
// - Event::GetBit and Event::SetBit stay mangled. There is no Event header.
//   InitResources and the pound call them for bit 0xe.
// - dBgW_KcMbg::SetFile stays mangled. InitResources passes Fix12<int> by
//   value, and data_ov033_02111bfc is the CLPS block it takes.
// - dBgActor_c::IsClsnInRange stays mangled. Behavior passes Fix12<int>.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjTtFuta_c_AssetId {
    ASSET_TT_OBJ_FUTA_BMD = 0x06c5,
    ASSET_TT_OBJ_FUTA_KCL = 0x06c6
};

#include "daObjTtFuta_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct TtFutaModelFilePtr : SharedFilePtr {
    u32 words[2];

    TtFutaModelFilePtr(u32 fileID);
    ~TtFutaModelFilePtr();
};

struct TtFutaCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    TtFutaCollisionFilePtr(u32 fileID);
    ~TtFutaCollisionFilePtr();
};

/* BSS file homes; Init loads them, Cleanup releases them. Defined at the
   end of this file so the constructors do not enter .text. */
extern TtFutaModelFilePtr data_ov033_021124c8;
extern TtFutaCollisionFilePtr data_ov033_021124c0;

extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern int data_ov033_02111bfc[];
extern int _ZN5Event6GetBitEj(unsigned int);
extern void _ZN5Event6SetBitEj(unsigned int b);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
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
    Sound::PlayBank3(0xf, mCamSpacePos);
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

/* TT_FUTA factory. */
// @symbol daObjTtFuta_c_classInit
extern "C" daObjTtFuta_c *daObjTtFuta_c_classInit()
{
    return new daObjTtFuta_c();
}

/* Source order is construction order: model file 1733, collision file 1734.
 * __sinit_d_a_obj_tt_futa.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
// @symbol __sinit_d_a_obj_tt_futa.cpp
TtFutaModelFilePtr data_ov033_021124c8(ASSET_TT_OBJ_FUTA_BMD);
TtFutaCollisionFilePtr data_ov033_021124c0(ASSET_TT_OBJ_FUTA_KCL);
