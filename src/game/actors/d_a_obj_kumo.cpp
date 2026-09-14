//cpp
/* Cloud platform (ov039 CLOUD / OBJ_KUMO, actor 314).
 *
 * A scenery model that ranks itself against the other clouds by camera-space
 * distance and writes that rank into the model's polygon ID, so nearer
 * clouds draw in front. Direct base is dActor_c; the only owned member is
 * Model at 0xd4, which closes the class on the factory's literal 0x124.
 *
 * daObjKumo_c_classInit is reconstructed (RTTI daObjKumo_c, OBJ_KUMO
 * registry). Retail does not store that spelling. Historical alias
 * Cloud_Spawn.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder.
 *
 * deslop
 * Leftover: data_ov039_021118e4 is this overlay's BMD SharedFilePtr handle
 *   and data_ov039_021118e0 is the load-count the sinit-owned BSS keeps.
 *   This TU is text-only, so they stay externs.
 * Leftover: g_profile_OBJ_KUMO lives outside this TU (S14).
 * Leftover: func_ov039_02111214 keeps its C-ABI cartridge name; it is not
 *   a vtable slot.
 * Leftover: Vec3_Dist has no shared header; every caller declares it
 *   locally.
 * Leftover: `#pragma opt_propagation off` is file-global last-wins;
 *   Behavior size-DIFFs without it.
 * Leftover: inline destructor (out-of-line emits D0 before D1).
 */

#include "daObjKumo_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern void Matrix4x3_FromRotationY(void *, int);
extern SharedFilePtr data_ov039_021118e4;
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
void func_ov039_02111214(daObjKumo_c *t);
extern int data_ov039_021118e0;
}

/* FILE-GLOBAL BY MEASUREMENT. `opt_propagation` is not positional in mwccarm
   2004/b56 -- it is file-global last-wins -- so the legacy Behavior source's
   `#pragma opt_propagation off` has to sit at the top of the merged TU. All
   members were re-verified with it here: without it Behavior DIFFs by 999
   words. */
#pragma opt_propagation off

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjKumo_c_classInit
/* Every instruction the cartridge has here falls out of the one `new`.
   0x02111380 loads 0x124 -- the class's own size -- into the header's inline
   operator new; the implicit ctor calls dActor_c's C2, stores this class's
   vptr, then Model's C1 at 0xd4. The null check is the one `new` itself
   emits. */
extern "C" daObjKumo_c *daObjKumo_c_classInit(void)
{
    return new daObjKumo_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_c13InitResourcesEv
int daObjKumo_c::InitResources()
{
    void *f = Model::LoadFile(data_ov039_021118e4);
    mModel.SetFile((BMD_File *)f, 1, 2);
    func_ov039_02111214(this);
    data_ov039_021118e0++;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_c8BehaviorEv
int daObjKumo_c::Behavior()
{
    int d = Vec3_Dist((const Vector3*)&mCamSpacePosX, (const Vector3*)&mPosX);
    int rank = 1;
    int base = 2;
    dActor_c *actor = FindWithActorID(0x13a, 0);
    while (actor) {
        if (actor != this) {
            int d2 = Vec3_Dist((const Vector3*)&mCamSpacePosX, (const Vector3*)&actor->mPosX);
            if (d > d2)
                rank++;
        }
        actor = FindWithActorID(0x13a, actor);
    }
    mModel.SetPolygonID((base + rank) & 0xff);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_c6RenderEv
int daObjKumo_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_c16CleanupResourcesEv
int daObjKumo_c::CleanupResources()
{
    data_ov039_021118e4.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov039_02111214(daObjKumo_c *t)
{
    Matrix4x3_FromRotationY(&t->mModel.mat4x3, t->mAngleY);
    t->mModel.mat4x3.t.x = t->mPosX >> 3;
    t->mModel.mat4x3.t.y = t->mPosY >> 3;
    t->mModel.mat4x3.t.z = t->mPosZ >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daObjKumo_cD1Ev
// @symbol _ZN11daObjKumo_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjKumo_c() {}` in the header is
   the whole source of both variants: from an inline body mwcc emits D1 and
   then D0 -- the cartridge's own order -- and no D2. Writing the body out of
   line here instead flips them to D0-before-D1 and the isolation step rejects
   the object.

   Their bodies are one vptr store, then mModel destroyed, then ~dActor_c --
   the direct-base chain the RTTI states. D0's trailing deallocation is the
   inline `operator delete` it inherits, which is why nothing here names a
   heap. */
