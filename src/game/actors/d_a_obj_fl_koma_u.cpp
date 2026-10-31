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
// - Behavior's collider slot writes keep their raw w offsets (see the note
//   at the stores).
// - This TU emits its own __sinit for the file handles below, which the
//   manifest does not license. The handles stay: nothing else defines
//   their BSS or .init range.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjFl_KomaU_c_AssetId { ASSET_FL_KOMA_U_BMD = 0x05f7, ASSET_FL_KOMA_U_KCL = 0x05f8 };

#include "daObjFl_KomaU_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Player.h"
#include "Particle__System.h"
#include "math/MtxStack.h"

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

/* local extern: arm9 vector helpers and the dBgW callback store, with no
 * shared header yet. */
extern "C" {
void MulVec3Mat4x3(const Vector3 *v, const Matrix4x3 *m, Vector3 *dst);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *dst);
void func_020393d4(dBgW *bgw, void *fn);
}

// @symbol _ZN15daObjFl_KomaU_cD1Ev
// @symbol _ZN15daObjFl_KomaU_cD0Ev
daObjFl_KomaU_c::~daObjFl_KomaU_c() {}

// @symbol _ZN15daObjFl_KomaU_c16CleanupResourcesEv
s32 daObjFl_KomaU_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov064_0211adbc.model->Release();
    data_ov064_0211adbc.collision->Release();
    return 1;
}

// @symbol _ZN15daObjFl_KomaU_c6RenderEv
s32 daObjFl_KomaU_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjFl_KomaU_c8BehaviorEv
s32 daObjFl_KomaU_c::Behavior() {
    int i;
    char *w;
    dCcPos_c *cyl;
    int b;
    s16 *angle;

    angle = &mAngleY;
    *angle = *angle - 0x100;

    b = ((mFlags & 8) != 0) ? 1 : 0;
    if (b) {
        if (mMeshCollider.IsEnabled() != 0)
            mMeshCollider.Disable();
        return 1;
    }

    UpdateModelPosAndRotY();
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);

    w = (char *)this;
    cyl = mColliders;
    i = 0;

    do {
        Vector3 in;
        Vector3 out;
        dActor_c *actor;
        u32 id;
        int is_burn;

        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        in.x = 0;
        in.y = 0;
        in.z = (i < 4) ? ((int)(long long)i - 4) * 0x96000 - 0x64000
                       : ((int)(long long)i - 3) * 0x96000 + 0x64000;

        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        AddVec3(&out, &mPos, &out);

        /* Raw offsets from w, not cyl->pos / cyl->otherOwner. The member
         * spelling makes mwccarm recompute the element address and Behavior
         * stops matching; cyl++ does the same. The (int)(long long)i round
         * trip and the pointer-local mAngleY decrement are instruction
         * selection: the plain forms stop matching. */
        *(int *)(w + 0x390) = out.x;
        *(int *)(w + 0x394) = out.y;
        *(int *)(w + 0x398) = out.z;

        id = *(u32 *)(w + 0x384);
        if (id != 0) {
            actor = dActor_c::FindWithID(id);
            if (actor != 0) {
                is_burn = (actor->actorID == 0xbf) ? 1 : 0;
                if (is_burn != 0)
                    ((Player *)actor)->Burn();
            }
        }

        out.y = out.y + 0x64000;
        mParticleID[0][i] =
            Particle::System::New(mParticleID[0][i], 0x4b, out.x, out.y, out.z, 0, 0);
        mParticleID[1][i] =
            Particle::System::New(mParticleID[1][i], 0x4c, out.x, out.y, out.z, 0, 0);

        cyl->Clear();
        cyl->Update();

        w += 0x3c;
        cyl = (dCcPos_c *)((char *)cyl + 0x3c);
        i += 1;
    } while (i < 8);

    if (IsClsnInRange(0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN15daObjFl_KomaU_c13InitResourcesEv
s32 daObjFl_KomaU_c::InitResources() {
    int i;

    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov064_0211adbc.model), 1, -1);
    for (i = 0; i < 8; i++)
        mColliders[i].Init(mPos, 0x4b000, 0x96000, 0x200002, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile((KCL_File *)dBgW_Kc::LoadFile(*data_ov064_0211adbc.collision), mClsnMat,
                          0x199, mAngleY, *data_ov064_0211adbc.clps);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daObjFl_KomaU_c through
 * RTTI, allocation size, vtable identity, and the FL_KOMA_U registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: RotatingFirebar_Spawn. */
// @symbol daObjFl_KomaU_c_classInit
extern "C" daObjFl_KomaU_c *daObjFl_KomaU_c_classInit() {
    return new daObjFl_KomaU_c();
}

/* Retail construction order: model 1527, then collision 1528. */
KomaUModelFilePtr gFlKomaUModelFile(ASSET_FL_KOMA_U_BMD);
KomaUClsnFileHandle gFlKomaUCollisionFile(ASSET_FL_KOMA_U_KCL);
