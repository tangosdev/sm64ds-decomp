//cpp
/* Production translation unit for ov016/daObjKi_Hasira_c, hand-curated.
 * 8 function(s), .text 0x02112a00..0x02112ef4: the seven class members plus
 * the daObjKi_Hasira_c_classInit registry factory folded in from
 * fold-lane-c-0929. The TU also owns the static initializer
 * __sinit_d_a_obj_ki_hasira.cpp (.init 0x02113978..0x021139e4, folded from
 * the __sinit_ov016_02113978 shard) with its .ctor word and the two file
 * handles' .bss.
 *
 * Jolly Roger Bay's stone pillar (KI_HASIRA; `hasira` = pillar): a
 * dBgActor_c that waits for a player to come within 1300 units, turns to
 * face them, then topples -- its pitch accelerating until it passes 0x3900 --
 * and crashes in a burst of dust, a camera shake and a sound, and is
 * destroyed. InitResources also spawns the actor 0x3b at its feet.
 *
 * NAME: daObjKi_Hasira_c is the cartridge's RTTI spelling -- _ZTS at ov016
 * 0x02114ac8 is the byte string "16daObjKi_Hasira_c", and _ZTI at 0x02114abc
 * reads [__si_class_type_info+8, that string, _ZTI10dBgActor_c].
 *
 * THE DESTRUCTOR IS THIS CLASS'S KEY FUNCTION, so this TU also emits
 * _ZTV16daObjKi_Hasira_c, _ZTI16daObjKi_Hasira_c and _ZTS16daObjKi_Hasira_c
 * as vague linkage, alongside the inherited bases' RTTI records. Every one of
 * them has a configured ROM home, so all of them license as deadstrip-data.
 * The destructor is declared and defined inline and empty in the class
 * header -- see that file for why -- so there is deliberately no destructor
 * text in this .cpp.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. The folded-in
 * daObjKi_Hasira_c_classInit factory (0x02112ec4) is now the highest address
 * in the run, so it leads the file, ahead of InitResources. Do not reorder.
 *
 * daObjKi_Hasira_c_classInit (historical aliases RockPillar_Spawn and
 * RockPillar for the class itself) hand-called fBase_c::operator new(808),
 * dBgActor_c::dBgActor_c() and stored _ZTV16daObjKi_Hasira_c. This class
 * declares no constructor of its own, so the compiler-synthesized default
 * constructor emits exactly that sequence; the factory is now
 * `return new daObjKi_Hasira_c();`.
 *
 * UpdateModelMatrix is the three-axis model-matrix update (dBgActor_c::
 *   UpdateModelPosAndRotY's pitch-and-roll sibling); the name is coined,
 *   the real one is unrecovered.
 * What stays: the model and collision SharedFilePtrs are ov016 .bss rows still
 *   named RockPillar_ModelFile / RockPillar_ClsnFile in symbols.txt; they are
 *   data names, not the class name, and renaming them is a separate change.
 * What stays: data_ov016_02113cac, the CLPS block handed to SetFile, is an
 *   unnamed ov016 .data row this TU does not own.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjKi_Hasira_c_AssetId { ASSET_KI_HASIRA_BMD = 0x063d, ASSET_KI_HASIRA_KCL = 0x063e };

#include "daObjKi_Hasira_c.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "Particle__System.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct HasiraModelFile : SharedFilePtr {
    HasiraModelFile(u32 fileID);
    ~HasiraModelFile();
};

struct HasiraCollisionFile : SharedFilePtr {
    HasiraCollisionFile(u32 fileID);
    ~HasiraCollisionFile();
};

/* This class's model and collision files (ov016 .bss) and the CLPS block
 * dBgW_KcMbg::SetFile is handed (ov016 .data). */
extern HasiraModelFile RockPillar_ModelFile;
extern HasiraCollisionFile RockPillar_ClsnFile;
extern CLPS_Block data_ov016_02113cac;

/* local extern: no shared header declares these. */
extern "C" {

/* The camera the shake request goes to (arm9). */
extern dCamera_c *gCamera;
}


/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjKi_Hasira_c through
 * RTTI, allocation size, vtable identity, and the KI_HASIRA registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical aliases: RockPillar_Spawn, and RockPillar for
 * the class itself. */
extern "C" daObjKi_Hasira_c *daObjKi_Hasira_c_classInit() {
    return new daObjKi_Hasira_c();
}

/* -------------------------------------------------------------------------- */
s32 daObjKi_Hasira_c::InitResources() {
    BMD_File *f = dExtSimpleModel_c::LoadFile(RockPillar_ModelFile);
    mModel.SetFile(f, 1, -1);
    UpdateModelMatrix();
    UpdateClsnPosAndRot();

    KCL_File *k = dBgW_Kc::LoadFile(RockPillar_ClsnFile);
    mMeshCollider.SetFile(k, mClsnMat, 0x199, mAngleY, data_ov016_02113cac);

    Spawn(0x3b, 0, mPos, 0, mAreaId, -1);
    return 1;
}

/* -------------------------------------------------------------------------- */
s32 daObjKi_Hasira_c::Behavior() {
    Vector3 targetPos;
    Vector3 crashPos;
    Vector3 dustPos;

    /* Nothing happens while the pillar is off screen (mFlags 0x08). Written
       as a flag first: tested inline, the branch comes out a different shape. */
    int offScreen = (mFlags & 8) != 0;
    if (offScreen)
        return 1;

    daPly_c *target = mTarget;
    if (target != 0) {
        const Vector3 &p = target->mPos;
        targetPos = p;
    }

    switch (mState) {
    case 0: /* wait for a player to come within 1300 units */
        mTarget = ClosestPlayer();
        if (mTarget != 0) {
            if (Vec3_Dist(&mPos, &targetPos) < 0x514000) {
                mState++;
                mTurnTimer = 10;
                Sound::PlayBank3(0x56, mCamSpacePos);
            }
        }
        break;

    case 1: /* turn to face them for ten frames */
        if (mTarget != 0) {
            s16 angle = Vec3_HorzAngle(&mPos, &targetPos);
            ApproachLinear(mAngleY, angle, 0x400);
            if (DecIfAbove0_Byte(&mTurnTimer) == 0)
                mState++;
        }
        break;

    case 2: { /* topple, faster every frame, and crash */
        mFallAccel += 4;
        mFallSpeed += mFallAccel;
        mAngleX += mFallSpeed;

        if (mAngleX > 0x3900) {
            /* The crash point: 900 units out along the facing, 150 up. */
            crashPos = mPos;
            crashPos.x += FX_MUL(FX_SinIdx((u16)mAngleY >> 4), 0x384000);
            crashPos.z += FX_MUL(FX_CosIdx((u16)mAngleY >> 4), 0x384000);
            crashPos.y += 0x96000;
            Particle::System::NewSimple(0x107, crashPos.x, crashPos.y, crashPos.z);
            dustPos = crashPos;
            LandingDustAt(dustPos, true);
            gCamera->func_0200d8c8(&mPos, 0x5dc000);
            Sound::PlayBank3(0x41, mCamSpacePos);
            MarkForDestruction();
        }
        break;
    }
    }

    UpdateModelMatrix();
    if (IsClsnInRange(0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
s32 daObjKi_Hasira_c::Render() {
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
s32 daObjKi_Hasira_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    RockPillar_ModelFile.Release();
    RockPillar_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* dBgActor_c::UpdateModelPosAndRotY with pitch and roll as well: rebuild the
 * model matrix from all three angles, then drop the position into its
 * translation row at 1/8 scale, the model-space unit. */
void daObjKi_Hasira_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* Static-init globals (was the handwritten __sinit_ov016_02113978 shard).
 * Definition order is the retail initializer's construction order: the
 * 1597 model handle, then the 1598 collision handle. */
HasiraModelFile RockPillar_ModelFile(ASSET_KI_HASIRA_BMD);
HasiraCollisionFile RockPillar_ClsnFile(ASSET_KI_HASIRA_KCL);
