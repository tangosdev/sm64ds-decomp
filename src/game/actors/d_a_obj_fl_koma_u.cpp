//cpp
/* The rotating fire bar (registry profile FL_KOMA_U),
 * .text 0x02118138..0x021185c0. The out-of-line destructor is the key
 * function, so this TU emits _ZTV/_ZTI/_ZTS15daObjFl_KomaU_c: _ZTS is
 * "15daObjFl_KomaU_c" at ov064 0x0211bdd8 and _ZTI at 0x0211bdcc reads
 * [__si_class_type_info, that string, _ZTI10dBgActor_c]. The tree
 * previously called the class RotatingFirebar (coined). #pragma
 * defer_codegen off lays .text down in source order. The last function is
 * the registry factory daObjFl_KomaU_c_classInit (0x02118564),
 * `new daObjFl_KomaU_c()`; the dCcPos_c array's user-declared constructor
 * and destructor are what make the implicit constructor emit the ROM's
 * __cxa_vec_ctor over mColliders.
 */

// What stays, and why:
// - This TU emits its own __sinit for the file handles below, which the
//   manifest does not license. The handles stay: nothing else defines
//   their BSS or .init range.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjFl_KomaU_c_AssetId { ASSET_FL_KOMA_U_BMD = 0x05f7, ASSET_FL_KOMA_U_KCL = 0x05f8 };

#include "daObjFl_KomaU_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "daPly_c.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"

/* The ctor/dtor pairs are the ROM's SharedFilePtr veneers (func_02017acc /
 * func_02017ab4 for the model, func_02017b4c / SharedFilePtr_Destruct_Clsn
 * for the collision); the subclasses exist to spell those entry points.
 * data_ov064_0211adbc stays the file table. */
struct KomaUModelFilePtr : SharedFilePtr {
    KomaUModelFilePtr(unsigned int fileId);
    ~KomaUModelFilePtr();
};
struct KomaUClsnFileHandle : SharedFilePtr {
    KomaUClsnFileHandle(unsigned int fileId);
    ~KomaUClsnFileHandle();
};
typedef char KomaUModelFilePtr_size_must_be_8[sizeof(KomaUModelFilePtr) == 8 ? 1 : -1];
typedef char KomaUClsnFileHandle_size_must_be_8[sizeof(KomaUClsnFileHandle) == 8 ? 1 : -1];

#pragma defer_codegen off

struct CLPS_Block;

struct FileTable {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

extern FileTable data_ov064_0211adbc;

/* local extern: MTX_MultVec43 has no shared header yet. */
extern "C" {

void MTX_MultVec43(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
}


daObjFl_KomaU_c::~daObjFl_KomaU_c() {}

s32 daObjFl_KomaU_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov064_0211adbc.model->Release();
    data_ov064_0211adbc.collision->Release();
    return 1;
}

s32 daObjFl_KomaU_c::Render() {
    mModel.Render(0);
    return 1;
}

/* dActor_c::mFlags bit 0x08 is set by the framework while the actor is off
   screen; actor 0xbf is PLAYER. Int-returning, as the ROM materializes each
   0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

s32 daObjFl_KomaU_c::Behavior() {
    mAngleY -= 0x100;

    if (IsOffScreen(this)) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        return 1;
    }

    UpdateModelPosAndRotY();
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);

    for (int i = 0; i < 8; i++) {
        Vector3 in;
        Vector3 out;
        /* The early in.z store is dead but load-bearing: without it the
           zeroing stores schedule differently. */
        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        in.x = 0;
        in.y = 0;
        /* Four flames on each side of the hub, 150 units apart, the
           innermost 250 units out. The long spacing is NitroSDK's fx32
           (signed long; the tree's fx.h says int): with an int, the
           multiply is strength-reduced away. */
        in.z = i < 4 ? (i - 4) * 0x96000L - 0x64000 : (i - 3) * 0x96000L + 0x64000;

        MTX_MultVec43(&in, &data_020a0e68, &out);
        VEC_Add(&out, &mPos, &out);

        mColliders[i].pos = out;

        u32 id = mColliders[i].otherOwner;
        if (id != 0) {
            dActor_c *actor = dActor_c::FindWithID(id);
            if (actor != 0 && IsPlayerActor(actor))
                ((daPly_c *)actor)->Burn();
        }

        out.y += 0x64000;
        mParticleID[0][i] =
            Particle::System::New(mParticleID[0][i], kPtclFireBarA, out.x, out.y, out.z, 0, 0);
        mParticleID[1][i] =
            Particle::System::New(mParticleID[1][i], kPtclFireBarB, out.x, out.y, out.z, 0, 0);

        mColliders[i].Clear();
        mColliders[i].Update();
    }

    if (IsClsnInRange(0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

s32 daObjFl_KomaU_c::InitResources() {
    int i;

    mModel.SetFile(dExtSimpleModel_c::LoadFile(*data_ov064_0211adbc.model), 1, -1);
    for (i = 0; i < 8; i++)
        mColliders[i].Init(mPos, 0x4b000, 0x96000, 0x200002, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(*data_ov064_0211adbc.collision), mClsnMat, 0x199,
                          mAngleY, *data_ov064_0211adbc.clps);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daObjFl_KomaU_c through
 * RTTI, allocation size, vtable identity, and the FL_KOMA_U registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: RotatingFirebar_Spawn. */
extern "C" daObjFl_KomaU_c *daObjFl_KomaU_c_classInit() {
    return new daObjFl_KomaU_c();
}

/* Retail construction order: model 1527, then collision 1528. */
KomaUModelFilePtr gFlKomaUModelFile(ASSET_FL_KOMA_U_BMD);
KomaUClsnFileHandle gFlKomaUCollisionFile(ASSET_FL_KOMA_U_KCL);
