//cpp
/* Production translation unit for ov022/daObjFl_Ring_c, hand-curated.
 * 9 function(s), .text 0x021111a0..0x021115a8.
 *
 * FL_RING, the fire ring of Lethal Lava Land (ov022): a dBgActor_c that turns
 * about Y at a rate kept in its own Z channel (mPrevAngleZ). It starts at
 * -0x100 per frame (or the placement's Z angle); a trigger
 * slows it to 0 and holds it there while a 0x96 frame cooldown runs, during which it spawns actor 0xf3 (OBJ_VOLCANO_CANNON,
 * daObj_volcanoCannon_c) whenever it is on screen and its spawn guard has run
 * down. Then the rate returns to -0x100.
 *
 * NAME: daObjFl_Ring_c is the cartridge's RTTI spelling -- _ZTS at ov022
 * 0x02113ce0 is the byte string "14daObjFl_Ring_c", and _ZTI at 0x02113cd4
 * reads [__si_class_type_info+8, that string, _ZTI10dBgActor_c].
 *
 * The destructor is the key function, so this TU also emits _ZTV14daObjFl_Ring_c,
 * _ZTI14daObjFl_Ring_c and _ZTS14daObjFl_Ring_c as vague linkage, alongside the
 * inherited bases' RTTI records. Every one of them has a configured ROM home,
 * so all of them license as deadstrip-data and the object isolates cleanly. The destructor is declared and defined inline and empty in the class
 * header (see that file for why), so there is no destructor text here for an
 * @symbol marker to sit above.
 *
 * Function order is the reverse of the ROM's: mwccarm 2004/b56 emits one .text
 * section per function in the reverse of source order, so the highest-address
 * ROM function is written first. Do not reorder. The compiler places the D1/D0
 * pair on its own.
 *
 * Notes:
 * - func_020393d4 / func_020393c4 are 4-byte stores into dBgW's callback
 *   slots, and func_020393a4 / func_02039394 write its collision extents.
 *   d_bg_w.cpp defines them and no header declares them.
 * - The three pointers this class's file table holds -- the ov022 .bss
 *   SharedFilePtrs at 0x02114500 and 0x02114508, and the ov064 CLPS block at
 *   0x0211bbac -- are unnamed rows in their modules' data. The two handles
 *   are this TU's own .bss, defined at the bottom; the CLPS block is
 *   ov064's.
 * - The run ends with the trigger helper OnStoodOn, the collision callback
 *   StoodOnCallback that InitResources installs, and the registry factory
 *   daObjFl_Ring_c_classInit.
 * - g_profile_FL_RING, the registry row that names that factory, is ov022
 *   .data at 0x02113cf4 and lives outside this TU.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjFl_Ring_c_AssetId { ASSET_FL_RING_BMD = 0x060e, ASSET_FL_RING_KCL = 0x060f };

#include "daObjFl_Ring_c.h"
#include "SharedFilePtr.h"
#include "daObj_volcanoCannon_c.h"
#include "Sound.h"
#include "math/CalcLib.h"

/* This class's own file table, ov022 .data at 0x02113cc8: the model and
 * collision SharedFilePtrs (ov022 .bss 0x02114500 / 0x02114508, defined at
 * the bottom of this file) and the CLPS block SetFile is handed (ov064
 * 0x0211bbac). All three targets are unnamed rows in their modules' symbol
 * tables, so the table keeps its address-derived name. */
struct daObjFl_Ring_c_Files {
    SharedFilePtr *mModelFile;
    SharedFilePtr *mClsnFile;
    void *mClps;
};

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct RingModelFilePtr : SharedFilePtr {
    RingModelFilePtr(u32 fileID);
    ~RingModelFilePtr();
};

struct RingCollisionFilePtr : SharedFilePtr {
    RingCollisionFilePtr(u32 fileID);
    ~RingCollisionFilePtr();
};

extern daObjFl_Ring_c_Files data_ov022_02113cc8;
extern RingModelFilePtr gFlRingModelFile;
extern RingCollisionFilePtr gFlRingCollisionFile;

extern "C" {
/* local extern: d_bg_w.cpp defines these and no header declares them. */
void func_020393a4(void *p, int v);
void func_02039394(void *p, int v);
void func_020393d4(void *p, void *fn);
void func_020393c4(void *p, void *fn);
}

/* dActor_c::mFlags bit 0x08 is written by the framework while the actor is
 * off screen. This int-returning predicate is what makes the ROM
 * materialize 0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

// @symbol daObjFl_Ring_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjFl_Ring_c through RTTI,
 * allocation size, vtable identity, and the FL_RING registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: VolcanoRing_Spawn.
 *
 * `new daObjFl_Ring_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x328), dBgActor_c's base constructor, then the
 * vptr store. */
extern "C" daObjFl_Ring_c *daObjFl_Ring_c_classInit(void) {
    return new daObjFl_Ring_c;
}

/* The collision callback InitResources installs in mMeshCollider's slot. The
 * slot passes three arguments; the trigger helper wants the last two.
 * long_calls keeps the pooled absolute tail call. */
#pragma push
#pragma long_calls on
// @symbol _ZN14daObjFl_Ring_c15StoodOnCallbackEP4dBgWPS_P8dActor_c
void daObjFl_Ring_c::StoodOnCallback(dBgW *collider, daObjFl_Ring_c *self, dActor_c *other) {
    self->OnStoodOn(other);
}
#pragma pop

// @symbol _ZN14daObjFl_Ring_c9OnStoodOnEP8dActor_c
/* Something touched the ring: raise mTriggered, which the next Behavior
 * reads once and clears. */
void daObjFl_Ring_c::OnStoodOn(dActor_c *other) {
    mTriggered = 1;
}

// @symbol _ZN14daObjFl_Ring_c13InitResourcesEv
s32 daObjFl_Ring_c::InitResources() {
    void *f = dExtSimpleModel_c::LoadFile(*data_ov022_02113cc8.mModelFile);
    mModel.SetFile((BMD_File *)f, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    void *k = dBgW_Kc::LoadFile(*data_ov022_02113cc8.mClsnFile);
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile((KCL_File *)(k), mClsnMat, 0x199, mAngleY,
                              *(CLPS_Block *)(data_ov022_02113cc8.mClps));

    func_020393d4(&mMeshCollider, (void *)dBgW::UpdatePosAndAngs);
    func_020393c4(&mMeshCollider, (void *)StoodOnCallback);

    /* The ring's own Z channel carries its spin rate, not an angle: it rests
       at -0x100 per frame unless the map row gave it one. */
    mPrevAngleZ = -0x100;
    if (mAngleZ != 0)
        mPrevAngleZ = mAngleZ;
    return 1;
}

// @symbol _ZN14daObjFl_Ring_c8BehaviorEv
s32 daObjFl_Ring_c::Behavior() {
    switch (mState) {
    case 0:
        /* Turning: once the cooldown is spent and mTriggered is set, slow
           the spin to 0. */
        if (DecIfAbove0_Short(&mCooldown) != 0)
            break;
        if (mTriggered == 0)
            break;
        if (ApproachAngle(&mPrevAngleZ, 0, 0xf, 0x30, 2) != 0)
            break;
        mCooldown = 0x96;
        mState = 1;
        Sound::PlayBank3(0xc, mCamSpacePos);
        break;

    case 1:
        /* Stopped: spawn while the cooldown runs, then spin back up. */
        if (DecIfAbove0_Short(&mCooldown) == 0) {
            if (ApproachAngle(&mPrevAngleZ, -0x100, 0xf, 0x30, 2) != 0)
                break;
            mCooldown = 0x96;
            mState = 0;
            break;
        }
        if (IsOffScreen(this))
            break;
        if (DecIfAbove0_Byte(&mSpawnGuard) != 0)
            break;
        {
            int v[3];
            daObj_volcanoCannon_c *s;
            v[0] = mPosX;
            v[1] = mPosY;
            v[2] = mPosZ;
            v[1] = mPosY + 0x1f4000;
            s = (daObj_volcanoCannon_c *)dActor_c::Spawn(0xf3, 0, *(Vector3 *)v, 0, mAreaId, -1);
            s->mSpawner = this;
            s->mKillPosY = mPosY;
            {
                u16 *t = &mSpawnCount;
                *t = *t + 1;
            }
            mSpawnGuard = 2;
        }
        break;
    }

    {
        s16 *ip = &mPrevAngleY;
        *ip = (s16)(*ip + mPrevAngleZ);
    }
    mAngleY = mPrevAngleY;

    func_020393a4(&mMeshCollider, 0x780000);
    func_02039394(&mMeshCollider, 0x1000);
    UpdateModelPosAndRotY();
    if (IsClsnInRange(0x780000, 0x1000) != 0)
        UpdateClsnPosAndRot();

    mTriggered = 0;
    return 1;
}

// @symbol _ZN14daObjFl_Ring_c6RenderEv
s32 daObjFl_Ring_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjFl_Ring_c16CleanupResourcesEv
s32 daObjFl_Ring_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov022_02113cc8.mModelFile->Release();
    data_ov022_02113cc8.mClsnFile->Release();
    return 1;
}

/* Source order is construction order: model file 1550, collision file 1551.
 * __sinit_d_a_obj_fl_ring.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
RingModelFilePtr gFlRingModelFile(ASSET_FL_RING_BMD);
RingCollisionFilePtr gFlRingCollisionFile(ASSET_FL_RING_KCL);
