//cpp
/* daTrs_c's teardown-and-draw run -- ov063/daTrs_c.
 *
 * Three ROM-adjacent members with no gap and no other class between them,
 * bounded below by the ov063 state helpers and above by daTBasket_c::Render:
 * CleanupResources at 0x0211ae40, OnPendingDestroy at 0x0211af6c (slot 12, an
 * empty override -- a single bx lr), Render at 0x0211af70. They are written
 * here highest-address-first: mwccarm lays .text down in reverse source
 * order.
 *
 * Render draws the Boo's own body model at unk_510 while flags bit 3 is up
 * (and the cap model through the dCapEnemy_c base), then the animated model
 * at the actor position; CleanupResources releases the file handles the
 * variant loaded and unloads the cap model through the base.
 *
 * deslop leftovers:
 * - CleanupResources' spawner bump reaches the parent's mChildDeaths as
 *   (int)ptr + 0x5a0: the found actor is a dActor_c, not a daTrs_c, so the
 *   word has no member spelling here.
 * - Render hands &mScaleX / &mBodyScaleX to the Vector3 draw-scale parameter
 *   as a pun: no shared overlay accessor exists, and growing one is a
 *   follow-up, not this TU.
 * - The flags halfword at 0x5d4 is read through the typed mFlags_5d4 member:
 *   a local shadow compiles to a literal-pool address load where the ROM has
 *   add+ldrh, one instruction off a byte match.
 * - UnloadBlueCoinModel / UnloadKeyModels have no header declaration; the
 *   TU-local externs stay.
 */

#include "decl_common.h"
#include "daTrs_c.h"
#include "SharedFilePtr.h"

extern "C" void UnloadBlueCoinModel(void *o);

extern SharedFilePtr data_ov063_0211edec;
extern SharedFilePtr data_ov063_0211edc4;
extern SharedFilePtr data_ov063_0211eddc;
extern SharedFilePtr data_ov063_0211edf4;
extern SharedFilePtr data_ov063_0211ede4;
extern SharedFilePtr data_ov063_0211edd4;
extern SharedFilePtr data_ov063_0211edcc;

// @symbol _ZN7daTrs_c6RenderEv
int daTrs_c::Render()
{
    /* 0x40000 is the yoshi-mouth-adjacent talk bit: Behavior runs the
       ShowMessage flow while it is set. Not in dActor_c.h's bit table. */
    int b = (int)(((mFlags & 0x40000) != 0));
    if (b != 0)
        return 1;

    {
        if (!mFlags_5d4.b3)
            return 1;
        if (mFlags_5d4.b1) {
            mBodyModel.Render((const Vector3 *)&mBodyScaleX);
        }
        RenderCapModel(0);
    }

    if (mOpacity < 8)
        return 1;

    {
        unsigned char st = unk_5cf;
        if (st >= 0xc && st != 0xf)
            mModelAnim.HideMaterial(0, 2);
    }

    /* The four-way == 3 chain is the ROM's own boolean shape. The arms draw
       through different implementations: state 3 calls Model::Render directly,
       anything else dispatches the ModelAnim override through the vtable. */
    if (mDeathState != 8 &&
        (unk_5cc == 3 ||
         unk_5cc == 3 ||
         unk_5cc == 3 ||
         unk_5cc == 3)) {
        mModelAnim.Model::Render((const Vector3 *)&mScaleX);
    } else {
        mModelAnim.Render((const Vector3 *)&mScaleX);
    }

    return 1;
}

// @symbol _ZN7daTrs_c16OnPendingDestroyEv
void daTrs_c::OnPendingDestroy()
{
}

// @symbol _ZN7daTrs_c16CleanupResourcesEv
int daTrs_c::CleanupResources()
{
    int b;
    int *cnt;

    if (mSpawnedActorID != 0) {
        mFoundActor = dActor_c::FindWithID(mSpawnedActorID);
        if (mFoundActor != 0)
            mFoundActor->MarkForDestruction();
        mFoundActor = 0;
    }
    if (mSpawnerID != 0) {
        mFoundActor = dActor_c::FindWithID(mSpawnerID);
        if (mFoundActor != 0) {
            /* The spawner's child-death counter (a daTrs_c word); this death
               is what variant 3 counts toward its 5-spawn cap. */
            cnt = (int *)(((int)mFoundActor + 0x5a0));
            (*cnt)++;
        }
        mFoundActor = 0;
    }
    if (mCarriedID == 0x122)
        UnloadBlueCoinModel(this);
    else if (mCarriedID == 0xd4)
        data_ov063_0211edec.Release();

    b = (actorID == 0xd1);
    if (b != 0) {
        data_ov063_0211edc4.Release();
        data_ov063_0211eddc.Release();
    } else {
        data_ov063_0211edf4.Release();
        data_ov063_0211ede4.Release();
        if (unk_5cf == 0xf) {
            UnloadKeyModels(3);
            data_ov063_0211edd4.Release();
            data_ov063_0211edcc.Release();
        }
    }
    UnloadCapModel();
    return 1;
}
