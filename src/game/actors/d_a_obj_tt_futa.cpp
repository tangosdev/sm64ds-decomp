//cpp
/* Tiny-Huge Island's pounded lid (TT_FUTA). Ground-pounding it bursts
 * particles, plays the bank sound, sets the drain event bit and destroys
 * it. The lid stays shut until event bit 0xe opens it.
 */
/* data_ov033_02111bfc is the CLPS block InitResources hands SetFile. */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjTtFuta_c_AssetId { ASSET_TT_OBJ_FUTA_BMD = 0x06c5, ASSET_TT_OBJ_FUTA_KCL = 0x06c6 };

#include "daObjTtFuta_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "Event.h"
#include "Particle__System.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct TtFutaModelFilePtr : SharedFilePtr {
    TtFutaModelFilePtr(u32 fileID);
    ~TtFutaModelFilePtr();
};

struct TtFutaCollisionFilePtr : SharedFilePtr {
    TtFutaCollisionFilePtr(u32 fileID);
    ~TtFutaCollisionFilePtr();
};

/* BSS file homes; Init loads them, Cleanup releases them. Defined at the
   end of this file so the constructors do not enter .text. */
extern TtFutaModelFilePtr data_ov033_021124c8;
extern TtFutaCollisionFilePtr data_ov033_021124c0;

extern int data_ov033_02111bfc[];

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN13daObjTtFuta_cD1Ev
// @symbol _ZN13daObjTtFuta_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * dExtSimpleModel_c and dBgW_KcMbg, then dActor_c. */
daObjTtFuta_c::~daObjTtFuta_c() {}

// @symbol _ZN13daObjTtFuta_c15OnGroundPoundedER8dActor_c
/* Bursts particles at the pounder, plays the bank sound, sets the drain
 * event bit and destroys the lid. */
void daObjTtFuta_c::OnGroundPounded(dActor_c &other) {
    Vector3 *v = &other.mPos;
    Particle::System::NewSimple(0x28, v->x, v->y, v->z);
    Sound::PlayBank3(0xf, mCamSpacePos);
    Event::SetBit(0xe);
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
    IsClsnInRange(0, 0);
    return 1;
}

// @symbol _ZN13daObjTtFuta_c13InitResourcesEv
s32 daObjTtFuta_c::InitResources() {
    void *f = dExtSimpleModel_c::LoadFile(data_ov033_021124c8);
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *k = dBgW_Kc::LoadFile(data_ov033_021124c0);
    mMeshCollider.SetFile((KCL_File *)k, mClsnMat, 0x199, mAngleY,
                          *(CLPS_Block *)data_ov033_02111bfc);
    return Event::GetBit(0xe) == 0;
}

/* TT_FUTA factory. */
// @symbol daObjTtFuta_c_classInit
extern "C" daObjTtFuta_c *daObjTtFuta_c_classInit() {
    return new daObjTtFuta_c();
}

/* Source order is construction order: model file 1733, collision file 1734.
 * __sinit_d_a_obj_tt_futa.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
// @symbol __sinit_d_a_obj_tt_futa.cpp
TtFutaModelFilePtr data_ov033_021124c8(ASSET_TT_OBJ_FUTA_BMD);
TtFutaCollisionFilePtr data_ov033_021124c0(ASSET_TT_OBJ_FUTA_KCL);
