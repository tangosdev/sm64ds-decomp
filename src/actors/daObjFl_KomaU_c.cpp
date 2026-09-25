//cpp
/* Production translation unit for ov064/daObjFl_KomaU_c.
 * 6 function(s), .text 0x02118138..0x02118564. The rotating fire bar
 * (registry profile FL_KOMA_U).
 *
 * NAME: _ZTS15daObjFl_KomaU_c is "15daObjFl_KomaU_c" at ov064 0x0211bdd8;
 * _ZTI at 0x0211bdcc reads [__si_class_type_info, that string,
 * _ZTI10dBgActor_c]. The tree previously called the class RotatingFirebar
 * (coined).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02118138), D0
 * (0x02118194), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is
 * ROM-ascending. The factory daObjFl_KomaU_c_classInit (0x02118564) stays in
 * its own source, src/d_a_obj_fl_koma_u.cpp.
 */

#include "decl_common.h"
#include "daObjFl_KomaU_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Player.h"

#pragma defer_codegen off

struct CLPS_Block;

struct FileTable {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

/* Leftover: dBgActor_c::IsClsnInRange, dBgW_KcMbg::SetFile, dCcPos_c::Init
 * and Particle::System::New take Fix12<int> by value, so they stay mangled. */
extern "C" {
extern FileTable data_ov064_0211adbc;
extern Matrix4x3 data_020a0e68;

void Matrix4x3_FromRotationY(Matrix4x3 *mat, int angle);
void MulVec3Mat4x3(const Vector3 *v, const Matrix4x3 *m, Vector3 *dst);
void AddVec3(const Vector3 *a, const Vector3 *b, Vector3 *dst);
void func_020393d4(dBgW *bgw, void *fn);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
    dCcPos_c *self, const Vector3 *pos, int radius, int height, u32 flags, u32 vuln);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 handle, u32 effect, int x, int y, int z, const void *direction, void *callback);
}

// @symbol _ZN15daObjFl_KomaU_cD1Ev
// @symbol _ZN15daObjFl_KomaU_cD0Ev
daObjFl_KomaU_c::~daObjFl_KomaU_c()
{
}

// @symbol _ZN15daObjFl_KomaU_c16CleanupResourcesEv
s32 daObjFl_KomaU_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov064_0211adbc.model->Release();
    data_ov064_0211adbc.collision->Release();
    return 1;
}

// @symbol _ZN15daObjFl_KomaU_c6RenderEv
s32 daObjFl_KomaU_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjFl_KomaU_c8BehaviorEv
s32 daObjFl_KomaU_c::Behavior()
{
    int i;
    char *w;
    dCcPos_c *cyl;
    int b;
    s16 *angle;

    angle = &mAngleY;
    *angle = *angle - 0x100;

    b = ((mFlags & 8) != 0) ? 1 : 0;
    if (b) {
        if (((dBgW *)&mMeshCollider)->IsEnabled() != 0)
            ((dBgW *)&mMeshCollider)->Disable();
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
        AddVec3(&out, (Vector3 *)&mPosX, &out);

        /* Leftover: raw offsets from w, not cyl->pos / cyl->otherOwner. The
         * member spelling makes mwccarm recompute the element address and
         * Behavior stops matching. */
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
        mParticleID[0][i] = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticleID[0][i], 0x4b, out.x, out.y, out.z, 0, 0);
        mParticleID[1][i] = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticleID[1][i], 0x4c, out.x, out.y, out.z, 0, 0);

        cyl->Clear();
        cyl->Update();

        w += 0x3c;
        cyl = (dCcPos_c *)((char *)cyl + 0x3c);
        i += 1;
    } while (i < 8);

    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN15daObjFl_KomaU_c13InitResourcesEv
s32 daObjFl_KomaU_c::InitResources()
{
    int i;

    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov064_0211adbc.model), 1, -1);
    for (i = 0; i < 8; i++)
        _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
            &mColliders[i], (Vector3 *)&mPosX, 0x4b000, 0x96000, 0x200002, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*data_ov064_0211adbc.collision),
        &mClsnMat, 0x199, mAngleY, data_ov064_0211adbc.clps);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);
    return 1;
}
