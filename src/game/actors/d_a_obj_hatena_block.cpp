//cpp
/**
 * daObjHatenaBlock_c -- the ? block and its relatives, ov102: HATENA_BLOCK 20,
 * ITEM_BLOCK 21, VS_ITEM_BLOCK 22, CAP_BLOCK_M / W / L 23-25.
 *
 * One class, six registry factories. Hit from below, kicked, ground-pounded,
 * hit by a mega player or attacked, the block bounces (squash and stretch),
 * then pops and spawns its prize. HATENA_BLOCK / ITEM_BLOCK take the prize
 * from a content-type table (mContentType, the low byte of param1): a spray
 * of COINs, a STAR, a ONEUPKINOKO, a SHELL, a SCALEUP_KINOKO, or a
 * FEATHER / POWER_UP_ITEM / BOMBHEI chosen by the hitter's param1
 * (daObjHatenaBlock_c.h, Content); VS_ITEM_BLOCK uses a one-row table of
 * FEATHERs; each CAP_BLOCK spawns an OBJ_MARIO_CAP for one character
 * (M / L / W = 0 / 1 / 2), or a BOMBHEI when SaveData::HasPlayerLostCap()
 * is non-zero. A popped block that survives its prize (coins, the star
 * and the 1-up kill it) is not drawn and returns to IDLE after 300 frames
 * once the player is more than 100 units away.
 *
 * STATES. mState (daObjHatenaBlock_c::State) indexes the {enter, update}
 * table at data_ov102_0214e890: ChangeState enters a state,
 * CallStateUpdate (top of Behavior) runs its update:
 *   IDLE      enter 02149d80  update 02149ccc  (mega-player check)
 *   BOUNCING  enter 02149c78  update 021498e0  (squash, then the prize)
 *   POPPED    enter 021498c4  update 02149878  (300-frame wait)
 * The names are descriptive; the ROM stores none. Prize rows are
 * data_ov102_0214e8c0 (rows 0..7 by content, 4 columns by hitter) and
 * data_ov102_0214e870 (VS); the spawners are the func_ov102_02149xxx
 * routines below, each commented with what it spawns.
 *
 * daObjHatenaBlock_c_classInit_* are reconstructed names (RTTI
 * daObjHatenaBlock_c, the six registry IDs above); retail does not store
 * them. Historical aliases: QuestionBlock_Spawn, ExclamationBlock_Spawn,
 * ExclamationBlockVs_Spawn, CapBlockMario/Wario/Luigi_Spawn.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *
 *   common.h must be included first. Otherwise dExtSimpleModel_c.h's nested Matrix4x3
 *   wins and UpdateMeshCollider / 02149ff0 change size (0x18 / 0x1c).
 *
 *   The destructor stays inline; out of line, mwccarm emits D0 before D1.
 *
 *   `return new` emits a homeless _ZN10dBgActor_cD2Ev. That is a licensed
 *   deadstrip, the same helper ov045/daObjKm2_Ami_Bou_c records.
 *
 *   HbSpawnFrame stays at file scope so @class$ does not drift (Vector3 has
 *   an inline dtor; the manifest marks it compiler_only).
 *
 *   The (int *) / (Vector3 *) casts around the local position buffers in
 *   the spawn helpers stay: the buffers are plain ints, and a real Vector3
 *   local would emit an extra Vector3 D1 destructor. The copies here are
 *   licensed deadstrip-duplicates.
 *
 *   OnAttacked1 / OnHitFromUnderneath keep their valueless nested-if exits;
 *   C++ rejects a valueless return in a non-void function.
 *
 * WHY SOME CALLS ARE SPELLED AS MANGLED SYMBOLS:
 *
 *   (a) Fix12<int> passed by value (notes/mwccarm-codegen.md 6az). A
 *       method call homes the argument and the bytes move: dExtAnmModel_c::SetAnim and
 *       dBgW_KcMbg::SetFile (InitResources), dBgActor_c::IsClsnInRange
 *       (Behavior), dActor_c::DropShadowScaleXYZ (UpdateDropShadow),
 *       Particle::System::NewSimple (also absent from Particle.h).
 *
 *   (b) No usable declaration: dActor_c::Earthquake is not on dActor_c.h
 *       (EnterBouncing).
 *
 * Known limits:
 *   The helpers carry role names (symbols/actor_renames.tsv records the evidence for each);
 *   func_ov102_02149684 keeps its address. All take the block as arg0 and are declared as members on
 *   daObjHatenaBlock_c except func_ov102_02149684, which takes it as arg1
 *   (the out-param is arg0) and stays free. The state and prize tables use
 *   native daObjHatenaBlock_c member pointers. All observed callers discard
 *   the three corrected handlers' return values.
 *   None is coined.
 *   Unrecovered meanings: SaveData::flags1 bit 31 (SAVE_FLAGS1_BIT31); the
 *   LEVEL_ID values 0x1c / 0x1f / 0x21 this class special-cases (0x15 is
 *   Wet-Dry World per dActor_c::GetWaterHeightWDW); the
 *   particle IDs 9 / 0xb / 0xc / 0xd / 0x10 and sound 0 used at the pop; the
 *   spawn parameters handed to the prizes (STAR |0x40, COIN 2, BOMBHEI 4);
 *   what dBgActor_c::IsClsnInRange's two arguments and
 *   the two mesh-collider words poked in Behavior are; the files loaded at
 *   data_ov002_0210da58 / 0210d9a0 / 0210d9c0 / 0210da40; why
 *   the hitter's param1 picks the prize column (it is clamped to 0..3 and
 *   the table has no column names); and why a lost cap swaps the prize for a
 *   BOMBHEI.
 *
 * Native file-handle and dispatch-table storage is owned in .bss and
 * initialized by this TU. Its complete RTTI, six SpawnInfo profiles and
 * vtable are owned in .data, together with all 42 native member-pointer
 * descriptors. Their address labels remain configuration identities.
 *
 * NOT OWNED BY THIS TU (named where they live, not here):
 *   func_ov102_0214ad14 / 0214b384 -- daBmb_c helpers, called by
 *     SpawnPrizeBobOmb on the Bob-omb it spawns.
 *   daSCoin_c::Collect -- ov002; the bounce end calls it on a held
 *     SECRET_COIN (actorID 0x149).
 *   data_ov002_0210d9* / da40 / d954 -- ov002 BMD/CLPS handles loaded for
 *     the caps and contents.
 *   FX_SinCosTable_ -- NitroSDK's sin/cos table; the bounce squash indexes it
 *     by mBounceAng.
 *   data_0209caa0, gGameMode, gCurrentSublevel, data_0209f32c, gCamera,
 *     data_0209e650, data_020a0edc -- arm9 globals.
 *
 * deslop leftovers:
 * - func_ov102_02149684 stays free: it writes the prize point into a
 *   caller's buffer in arg0 and takes the block in arg1, so member form
 *   would swap the register operands.
 * - The (Vector3 *) prize-point casts around func_ov102_02149684's int
 *   buffers stay: they are local ints, and a real Vector3 local emits a
 *   Vector3 D1 deadstrip.
 */

/* common.h first: see the header. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjHatenaBlock_c_AssetId {
    ASSET_HATENA_BOX_BMD = 0x0463,
    ASSET_HATENA_KARABOX_BCA = 0x0464,
    ASSET_HATENA_KARABOX_BMD = 0x0465,
    ASSET_OBJ_CAP_BOX_L_BMD = 0x0466,
    ASSET_OBJ_CAP_BOX_M_BMD = 0x0467,
    ASSET_OBJ_CAP_BOX_W_BMD = 0x0468
};

#include "common.h"
#include "SpawnInfo.h"
#include "types.h"
#include "daObjHatenaBlock_c.h"
#include "dBgCh_Gnd.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "SaveData.h"
#include "daSCoin_c.h"
#include "dCamera_c.h"
#include "daShl_c.h"
#include "daBmb_c.h"
#include "nitro/fx/fx.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "LevelRng.h"
#include "nitro/fx/fx_trig.h"

struct CLPS_Block;
struct KCL_File;
struct BCA_File;

struct V3 {
  int x, y, z;
  V3(int a, int b, int d) { x = a; y = b; z = d; }
  V3() {}
};

/* State entries pair enter/update handlers; prize rows contain four handlers.
 * All dispatches discard results. These native member pointers are eight
 * bytes under the pinned compiler, including the this-adjustment word. */
typedef void (daObjHatenaBlock_c::*PMF)();
struct Entry { PMF pmf[2]; };

/* Actor IDs, as symbols/actor_debug_names.tsv lists them. */
enum {
    ACTOR_HATENA_BLOCK = 20,
    ACTOR_ITEM_BLOCK = 21,
    ACTOR_VS_ITEM_BLOCK = 22,
    ACTOR_CAP_BLOCK_M = 23,
    ACTOR_CAP_BLOCK_W = 24,
    ACTOR_CAP_BLOCK_L = 25,
    ACTOR_STAR = 178,         /* 0xb2 */
    ACTOR_BOMBHEI = 206,      /* 0xce */
    ACTOR_OBJ_MARIO_CAP = 269, /* 0x10d */
    ACTOR_ONEUPKINOKO = 276,  /* 0x114 */
    ACTOR_SCALEUP_KINOKO = 277, /* 0x115 */
    ACTOR_SHELL = 285,        /* 0x11d */
    ACTOR_COIN = 288,         /* 0x120 */
    ACTOR_POWER_UP_ITEM = 306, /* 0x132 */
    ACTOR_SECRET_COIN = 329,  /* 0x149 */
    ACTOR_FEATHER = 345       /* 0x159 */
};

/* SaveData::flags1 (data_0209caa0.file.flags1) bit 31. Its meaning is not named in
   the tree; this class only tests it. While it is CLEAR, an actor 20
   (HATENA_BLOCK) is drawn as the animated model (mAnmModel), Behavior only
   advances that animation (while IDLE) and keeps the collider off, and the mega-player
   trigger (UpdateIdle) does not run for it. */
enum { SAVE_FLAGS1_BIT31 = 0x80000000 };


extern signed char gCurrentSublevel;
extern int data_0209f32c;
extern unsigned char gGameMode;
extern dCamera_c *gCamera;
extern char data_020a0edc[];

extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov002_0210d9e0;
/* The ov002 model-file handles (src/ov002_handles.cpp defines all of them). */
extern SharedFilePtr data_ov002_0210da18;
extern SharedFilePtr data_ov002_0210da30;
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210da58;
extern SharedFilePtr gPFlowerOpenModelFile;
extern SharedFilePtr gPFlowerCloseModelFile;
extern CLPS_Block data_ov002_0210d954;

extern "C" int RandomIntInternal(int *seed);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern "C" u16 DecIfAbove0_Short(u16 *p);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void d_a_obj_hatena_blockEarthquakeHold(void *self, void *pos, s32 radius);
extern "C" void hold_d_a_obj_hatena_block(void *thiz, void *kcl, void *mtx, int fix, short s,
                                          void *clps);

/* Free: the block is its second argument (dst, src), so member form would
   swap the register operands. */
void func_ov102_02149684(int *dst, daObjHatenaBlock_c *src);

/* The block's file-scope SharedFilePtr objects, spelled through the same
   wrapper idiom the landed folds use: the handles' code is the veneer pair
   the cartridge already carries, so the wrappers get no bodies of their own.
   The constructors alias func_02017acc (model), SharedFilePtr::Construct
   (animation) and func_02017b4c (collision) in this unit's manifest, and the
   destructors func_02017ab4, SharedFilePtr_Destruct_Anim and
   SharedFilePtr_Destruct_Clsn. */
struct HatenaBlockModelFilePtr : SharedFilePtr {
    HatenaBlockModelFilePtr(unsigned int id);
    ~HatenaBlockModelFilePtr();
};

struct HatenaBlockAnimFileHandle : SharedFilePtr {
    HatenaBlockAnimFileHandle(unsigned int id);
    ~HatenaBlockAnimFileHandle();
};

struct HatenaBlockClsnFileHandle : SharedFilePtr {
    HatenaBlockClsnFileHandle(unsigned int id);
    ~HatenaBlockClsnFileHandle();
};

/* -------------------------------------------------------------------------- */
/* The six model handles, the animation handle and the collision handle, in
   static-initializer order: the initializer constructs each and registers
   its destructor. InitResources feeds the models to dExtSimpleModel_c::LoadFile, the
   animation to dExtFrameCtrl_c::LoadFile and the collision one to
   dBgW_Kc::LoadFile. */
/* -------------------------------------------------------------------------- */
HatenaBlockModelFilePtr   data_ov102_0214e800(0x800e); /* ITEM/VS BMD */
HatenaBlockModelFilePtr   data_ov102_0214e7e8(ASSET_HATENA_BOX_BMD);  /* HATENA_BLOCK BMD */
HatenaBlockModelFilePtr   data_ov102_0214e808(ASSET_HATENA_KARABOX_BMD);  /* HATENA anim BMD */
HatenaBlockModelFilePtr   data_ov102_0214e7f0(ASSET_OBJ_CAP_BOX_M_BMD);  /* CAP_BLOCK_M BMD */
HatenaBlockModelFilePtr   data_ov102_0214e7d8(ASSET_OBJ_CAP_BOX_L_BMD);  /* CAP_BLOCK_L BMD */
HatenaBlockModelFilePtr   data_ov102_0214e7e0(ASSET_OBJ_CAP_BOX_W_BMD);  /* CAP_BLOCK_W BMD */
HatenaBlockAnimFileHandle data_ov102_0214e7f8(ASSET_HATENA_KARABOX_BCA);  /* HATENA BCA */
HatenaBlockClsnFileHandle data_ov102_0214e7d0(0x800d); /* KCL */

/* The initializer copies native member-pointer constants into the tables
   below. Their complete .data contribution is 0x0214e258..0x0214e3a8. */

/* The three PMF tables, in static-initializer order: the mState {enter,
   update} rows, the per-content spawner grid (eight rows of four columns,
   each column pair copied from one record), then the VS rows. */
Entry data_ov102_0214e890[3] = {
    {&daObjHatenaBlock_c::EnterIdle, &daObjHatenaBlock_c::UpdateIdle},
    {&daObjHatenaBlock_c::EnterBouncing, &daObjHatenaBlock_c::UpdateBouncing},
    {&daObjHatenaBlock_c::EnterPopped, &daObjHatenaBlock_c::UpdatePopped},
};
PMF data_ov102_0214e8c0[8][4] = {
    {&daObjHatenaBlock_c::SpawnPrizeCoins, &daObjHatenaBlock_c::SpawnPrizeCoins,
     &daObjHatenaBlock_c::SpawnPrizeCoins, &daObjHatenaBlock_c::SpawnPrizeCoins},
    {&daObjHatenaBlock_c::SpawnPrizeStar, &daObjHatenaBlock_c::SpawnPrizeStar,
     &daObjHatenaBlock_c::SpawnPrizeStar, &daObjHatenaBlock_c::SpawnPrizeStar},
    {&daObjHatenaBlock_c::SpawnPrize1Up, &daObjHatenaBlock_c::SpawnPrize1Up,
     &daObjHatenaBlock_c::SpawnPrize1Up, &daObjHatenaBlock_c::SpawnPrize1Up},
    {&daObjHatenaBlock_c::SpawnPrizeShell, &daObjHatenaBlock_c::SpawnPrizeShell,
     &daObjHatenaBlock_c::SpawnPrizeShell, &daObjHatenaBlock_c::SpawnPrizeShell},
    {&daObjHatenaBlock_c::SpawnPrizeScaleUpKinoko, &daObjHatenaBlock_c::SpawnPrizeScaleUpKinoko,
     &daObjHatenaBlock_c::SpawnPrizeScaleUpKinoko, &daObjHatenaBlock_c::SpawnPrizeScaleUpKinoko},
    {&daObjHatenaBlock_c::SpawnPrizeFeather, &daObjHatenaBlock_c::SpawnPrizePowerUpItem,
     &daObjHatenaBlock_c::SpawnPrizePowerUpItem, &daObjHatenaBlock_c::SpawnPrizePowerUpItem},
    {&daObjHatenaBlock_c::SpawnPrizePowerUpItem, &daObjHatenaBlock_c::SpawnPrizePowerUpItem,
     &daObjHatenaBlock_c::SpawnPrizePowerUpItem, &daObjHatenaBlock_c::SpawnPrizePowerUpItem},
    {&daObjHatenaBlock_c::SpawnPrizeBobOmb, &daObjHatenaBlock_c::SpawnPrizePowerUpItem,
     &daObjHatenaBlock_c::SpawnPrizePowerUpItem, &daObjHatenaBlock_c::SpawnPrizePowerUpItem},
};
PMF data_ov102_0214e870[1][4] = {
    {&daObjHatenaBlock_c::SpawnPrizeFeather, &daObjHatenaBlock_c::SpawnPrizeFeather,
     &daObjHatenaBlock_c::SpawnPrizeFeather, &daObjHatenaBlock_c::SpawnPrizeFeather},
};

daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK() {
    return new daObjHatenaBlock_c();
}

daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_HATENA_BLOCK() {
    return new daObjHatenaBlock_c();
}

daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_ITEM_BLOCK() {
    return new daObjHatenaBlock_c();
}

daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_CAP_BLOCK_M() {
    return new daObjHatenaBlock_c();
}

daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_CAP_BLOCK_L() {
    return new daObjHatenaBlock_c();
}

daObjHatenaBlock_c *daObjHatenaBlock_c_classInit_CAP_BLOCK_W() {
    return new daObjHatenaBlock_c();
}

/* Loads the shared files for this block type, then sets the block up.
 *
 *  1. Per actor ID (the switch on actorID - 20): the block's model, plus the
 *     extra shared files that type needs (HATENA_BLOCK also its animated
 *     model and animation; the three CAP_BLOCKs the matching cap file; both
 *     also load data_ov002_0210d9e0, which daBmb_c loads as the Bob-omb
 *     model).
 *  2. Sets the model, a cuboid shadow, state IDLE, terminal velocity
 *     -0x3c000 (-60 units), unit scale, the model and collider matrices, and
 *     the mesh collider (scale 0x199, about 0.1; heading mAngleY).
 *  3. mHomePosY = mPosY. mContentType = low byte of param1 (0xff reads as 0);
 *     for CONTENT_STAR, mStarId = param1 bits 8..15 (0xff reads as 0) and the
 *     star is tracked.
 *  4. The VS block (22) forces mContentType to 0 and loads the file at
 *     data_ov002_0210da58.
 *  5. For HATENA_BLOCK / ITEM_BLOCK only: load the shared files the chosen
 *     content type needs (1-up, shell, mushroom, the Power Flower models, and the Bob-omb model for CONTENT_POWER_UP_7).
 *     CleanupResources releases the same set. Always returns 1. */
int daObjHatenaBlock_c::InitResources()
{
    void *modelFile = 0;
    switch (actorID - ACTOR_HATENA_BLOCK) {
    case 0:   /* HATENA_BLOCK */
        modelFile = dExtSimpleModel_c::LoadFile(data_ov102_0214e7e8);
        mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov102_0214e808), 1, 0x19);
        mAnmModel.SetAnim(dExtFrameCtrl_c::LoadFile(data_ov102_0214e7f8), 0, 0x1000, 0);
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9e0);
        break;
    case 1:   /* ITEM_BLOCK */
    case 2:   /* VS_ITEM_BLOCK */
        modelFile = dExtSimpleModel_c::LoadFile(data_ov102_0214e800);
        break;
    case 3:   /* CAP_BLOCK_M */
        modelFile = dExtSimpleModel_c::LoadFile(data_ov102_0214e7f0);
        dExtSimpleModel_c::LoadFile(data_ov002_0210da40);
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9e0);
        break;
    case 5:   /* CAP_BLOCK_L */
        modelFile = dExtSimpleModel_c::LoadFile(data_ov102_0214e7d8);
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9a0);
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9e0);
        break;
    case 4:   /* CAP_BLOCK_W */
        modelFile = dExtSimpleModel_c::LoadFile(data_ov102_0214e7e0);
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9c0);
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9e0);
        break;
    }

    mModel.SetFile((BMD_File *)modelFile, 1, -1);
    mShadowModel.InitCuboid();
    ChangeState(STATE_IDLE);
    mTerminalVelocity = -0x3c000;   /* -60 units */
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    UpdateModelMatrix();
    UpdateMeshCollider();
    mShadowMat = mModel.mat4x3;
    {
        KCL_File *kcl = dBgW_Kc::LoadFile(data_ov102_0214e7d0);
        mMeshCollider.SetFile(kcl, mClsnMat, 0x199, mAngleY, *(CLPS_Block *)(&data_ov002_0210d954));
    }
    mHomePosY = mPosY;
    mContentType = (unsigned char)param1;
    if (mContentType == 0xff)
        mContentType = 0;
    if (mContentType == CONTENT_STAR) {
        mStarId = (unsigned char)(param1 >> 8);
        if (mStarId == 0xff)
            mStarId = 0;
        mStarTracked = TrackStar(mStarId, 2);
    }

    {
        int b16 = !(actorID != ACTOR_VS_ITEM_BLOCK);
        if (b16 != 0) {
            mContentType = 0;
            dExtSimpleModel_c::LoadFile(data_ov002_0210da58);
        }
    }

    do {
        int b14 = !(actorID != ACTOR_HATENA_BLOCK);
        if (b14 == 0) {
            int b15 = !(actorID != ACTOR_ITEM_BLOCK);
            if (b15 == 0)
                break;
        }

        switch (mContentType) {
        case CONTENT_COINS:
        case CONTENT_STAR:
            break;
        case CONTENT_SHELL:
            dExtSimpleModel_c::LoadFile(data_ov002_0210da18);
            break;
        case CONTENT_ONEUP:
            dExtSimpleModel_c::LoadFile(data_ov002_0210d9d8);
            break;
        case CONTENT_SCALEUP_KINOKO:
            dExtSimpleModel_c::LoadFile(data_ov002_0210da30);
            break;
        case CONTENT_POWER_UP_7:
            dExtSimpleModel_c::LoadFile(gPFlowerOpenModelFile);
            dExtSimpleModel_c::LoadFile(gPFlowerCloseModelFile);
            dExtSimpleModel_c::LoadFile(data_ov002_0210d9e0);
            break;
        case CONTENT_POWER_UP_5:
            dExtSimpleModel_c::LoadFile(data_ov002_0210da58);
            dExtSimpleModel_c::LoadFile(gPFlowerOpenModelFile);
            dExtSimpleModel_c::LoadFile(gPFlowerCloseModelFile);
            break;
        case CONTENT_POWER_UP_6:
            dExtSimpleModel_c::LoadFile(gPFlowerOpenModelFile);
            dExtSimpleModel_c::LoadFile(gPFlowerCloseModelFile);
            break;
        }
    } while (0);
    return 1;
}

/* Per frame:
 *  1. Run the state's update routine (CallStateUpdate).
 *  2. Unless POPPED: move (UpdatePos), clamp mPosY up to mHomePosY, rebuild
 *     the model matrix and the shadow / clip volume.
 *  3. Poke two words of the mesh collider (dBgW::SetClipRadius / dBgW::SetClipOffsetY)
 *     with 0x8c000 (140 units) and 0x46000 (70 units).
 *  4. In any state but IDLE keep the collider disabled and return.
 *  5. In IDLE: a HATENA_BLOCK with SAVE_FLAGS1_BIT31 clear just advances its
 *     animation with the collider off. Otherwise, when gGameMode
 *     (CURRENT_GAMEMODE in symbols/verified.tsv) is 1, the collider is
 *     enabled and that is all. Otherwise dBgActor_c::IsClsnInRange is asked
 *     with (0x460000, 0x46000) in level 0x1c, else (0x118000, 0x46000); a
 *     non-zero answer refreshes the collider (UpdateMeshCollider). The
 *     meaning of its two arguments is not recovered here. Always returns 1. */
int daObjHatenaBlock_c::Behavior()
{
    CallStateUpdate();
    if (mState != STATE_POPPED) {
        UpdatePos(0);
        if (mPosY <= mHomePosY) mPosY = mHomePosY;
        UpdateModelMatrix();
        UpdateDropShadow();
    }
    mMeshCollider.SetClipRadius(0x8c000);
    mMeshCollider.SetClipOffsetY(0x46000);
    if (mState != STATE_IDLE) {
        if (mMeshCollider.IsEnabled() != 0) {
            mMeshCollider.Disable();
        }
    } else if ((data_0209caa0.file.flags1 & SAVE_FLAGS1_BIT31) == 0 &&
               (int)(actorID == ACTOR_HATENA_BLOCK) != 0) {
        mAnmModel.Advance();
        if (mMeshCollider.IsEnabled() != 0) {
            mMeshCollider.Disable();
        }
    } else if ((int)(gGameMode == 1) != 0) {
        if (mMeshCollider.IsEnabled() == 0) {
            mMeshCollider.Enable(this);
        }
    } else if (gCurrentSublevel == 0x1c) { /* LEVEL_ID 0x1c */
        if (IsClsnInRange(0x460000, 0x46000) != 0) {
            UpdateMeshCollider();
        }
    } else if (IsClsnInRange(0x118000, 0x46000) != 0) {
        UpdateMeshCollider();
    }
    return 1;
}

/* Draws nothing while POPPED. A HATENA_BLOCK with SAVE_FLAGS1_BIT31 clear
   draws mAnmModel; every other case draws mModel, passing the Vector3 at
   mScaleX. Always returns 1. */
int daObjHatenaBlock_c::Render()
{
    do {
        if (mState == STATE_POPPED)
            break;
        if ((data_0209caa0.file.flags1 & SAVE_FLAGS1_BIT31) == 0) {
            int b = (actorID == ACTOR_HATENA_BLOCK);
            if (b != 0) {
                mAnmModel.Render(0);
                break;
            }
        }
        {
            mModel.Render(&mScale);
        }
    } while (0);
    return 1;
}

/* Switches the collider off, then releases what InitResources loaded, in
   the same three groups: the VS block's data_ov002_0210da58 file; for
   HATENA_BLOCK / ITEM_BLOCK the per-content files (a CONTENT_STAR block also
   untracks its star); and the per-actor-ID files plus the shared KCL.
   Always returns 1. */
int daObjHatenaBlock_c::CleanupResources()
{
    int b, b2, b3;
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    b = (int)(actorID == ACTOR_VS_ITEM_BLOCK);
    if (b)
        data_ov002_0210da58.Release();

    do {
        b2 = (int)(actorID == ACTOR_HATENA_BLOCK);
        if (b2 == 0) {
            b3 = (int)(actorID == ACTOR_ITEM_BLOCK);
            if (b3 == 0)
                break;
        }
        switch (mContentType) {
        case CONTENT_STAR:
            UntrackStar(*(s8 *)&mStarTracked);
            break;
        case CONTENT_SHELL:
            data_ov002_0210da18.Release();
            break;
        case CONTENT_ONEUP:
            data_ov002_0210d9d8.Release();
            break;
        case CONTENT_SCALEUP_KINOKO:
            data_ov002_0210da30.Release();
            break;
        case CONTENT_POWER_UP_7:
            gPFlowerOpenModelFile.Release();
            gPFlowerCloseModelFile.Release();
            data_ov002_0210d9e0.Release();
            break;
        case CONTENT_POWER_UP_5:
            data_ov002_0210da58.Release();
            gPFlowerOpenModelFile.Release();
            gPFlowerCloseModelFile.Release();
            break;
        case CONTENT_POWER_UP_6:
            gPFlowerOpenModelFile.Release();
            gPFlowerCloseModelFile.Release();
            break;
        }
    } while (0);

    switch (actorID - ACTOR_HATENA_BLOCK) {
    case 0:   /* HATENA_BLOCK */
        data_ov102_0214e7e8.Release();
        data_ov102_0214e808.Release();
        data_ov102_0214e7f8.Release();
        data_ov002_0210d9e0.Release();
        break;
    case 1:   /* ITEM_BLOCK */
    case 2:   /* VS_ITEM_BLOCK */
        data_ov102_0214e800.Release();
        break;
    case 3:   /* CAP_BLOCK_M */
        data_ov102_0214e7f0.Release();
        data_ov002_0210da40.Release();
        data_ov002_0210d9e0.Release();
        break;
    case 5:   /* CAP_BLOCK_L */
        data_ov102_0214e7d8.Release();
        data_ov002_0210d9a0.Release();
        data_ov002_0210d9e0.Release();
        break;
    case 4:   /* CAP_BLOCK_W */
        data_ov102_0214e7e0.Release();
        data_ov002_0210d9c0.Release();
        data_ov002_0210d9e0.Release();
    }
    data_ov102_0214e7d0.Release();
    return 1;
}

/* Rebuilds the model matrix: a Y rotation by mAngleY, with the translation
   set to (mPosX, mPosY + mBounceYOffs, mPosZ) >> 3. A HATENA_BLOCK with
   SAVE_FLAGS1_BIT31 clear then copies the matrix into mAnmModel as well. */
void daObjHatenaBlock_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = (mPosY + mBounceYOffs) >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
    if (data_0209caa0.file.flags1 & SAVE_FLAGS1_BIT31)
        return;
    int b = (int)(actorID == ACTOR_HATENA_BLOCK);
    if (b == 0)
        return;
    mAnmModel.mat4x3 = mModel.mat4x3;
}

/* The drop shadow and the clip volume, both sized by the drop to the floor.
 *
 * mFloorY is refreshed (GetFloorY) unless mFlags bit 8 (dActor_c's
 * off-screen bit) is set and gGameMode is not 1. height = mPosY - mFloorY,
 * at least 0x1000 (1 unit).
 *   horizontal shadow scale = 0xb4000 (180 units) - height * 0x180 / 0x1000
 *                   (about 0.094 * height), at least 0xa000 (10 units), then
 *                   times mScaleX (a Fix12 multiply)
 *   shadow Y scale = height + 0x14000 (20 units)
 *   mClipOffsetY  = -(height + 0x14000) / 2 (signed divide by 2)
 *   mClipRadius   = max(height + 0x214000, 0x200000) / 16 (signed divide)
 * The shadow matrix is a Y rotation by mAngleY with the translation set to
 * (mPosX, mPosY - 0x20000 (32 units), mPosZ) >> 3. The drop-shadow call is
 * (shadow, matrix, scale X = horizontal, scale Y, scale Z = horizontal) with
 * opacity 0xf. */
void daObjHatenaBlock_c::UpdateDropShadow() {
    int shadowScale, height, paddedHeight;
    int b0, b1;

    b0 = (mFlags & 8) ? 1 : 0;
    if (b0 == 0 || ((b1 = (*(volatile unsigned char *)&gGameMode == 1) ? 1 : 0) != 0))
        mFloorY = GetFloorY();
    height = mPosY - mFloorY;
    if (height <= 0x1000) height = 0x1000;
    shadowScale = FX_MUL(height, 0x180);
    shadowScale = 0xb4000 - shadowScale;
    paddedHeight = height + 0x214000;
    if (shadowScale < 0xa000) shadowScale = 0xa000;
    if (paddedHeight < 0x200000) paddedHeight = 0x200000;
    mClipOffsetY = -((int)((height + 0x14000) + ((unsigned)(height + 0x14000) >> 31)) >> 1);
    mClipRadius = (int)(paddedHeight + ((unsigned)paddedHeight >> 31)) >> 4;
    shadowScale = FX_MUL(shadowScale, mScaleX);
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = (mPosY - 0x20000) >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    DropShadowScaleXYZ(mShadowModel, mShadowMat, shadowScale, height + 0x14000, shadowScale, 0xf);
}

/* Puts the mesh collider where the model is: the model matrix with the
   translation overwritten by (mPosX, mPosY + mBounceYOffs, mPosZ) (not
   shifted), and heading mAngleY. */
void daObjHatenaBlock_c::UpdateMeshCollider() {
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY + mBounceYOffs;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}

/* Runs the update routine (column 1) of mState's row in the state table. */
void daObjHatenaBlock_c::CallStateUpdate() {
    int j = mState;
    (this->*data_ov102_0214e890[j].pmf[1])();
}

/* Enters state i: stores it in mState, then runs that row's enter routine
   (column 0). */
void daObjHatenaBlock_c::ChangeState(int i) {
    mState = i;
    int j = mState;
    (this->*data_ov102_0214e890[j].pmf[0])();
}

/* STATE_IDLE enter routine: no bounce offset, unit scale, vertical
   acceleration -0x8000 (-8 units). */
void daObjHatenaBlock_c::EnterIdle() {
    mBounceYOffs = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mVertAccel = -0x8000;
}

/* STATE_IDLE update routine. Returns without effect for a HATENA_BLOCK while
   SAVE_FLAGS1_BIT31 is clear. Otherwise, when the closest player is mega
   (daPly_c::mIsMega), less than 0xc8000 (200 units) away horizontally and
   below the block (mPosY is greater than the player's), treats it as a hit
   by a mega character: virtual OnHitByMegaChar. */
void daObjHatenaBlock_c::UpdateIdle() {
    daPly_c *player;
    if (!(data_0209caa0.file.flags1 & SAVE_FLAGS1_BIT31)) {
        int b = (int)(actorID == ACTOR_HATENA_BLOCK);
        if (b)
            return;
    }
    player = ClosestPlayer();
    if (player->mIsMega == 0)
        return;
    if (Vec3_HorzDist(&mPos, &player->mPos) >= 0xc8000)
        return;
    if (mPosY <= player->mPosY)
        return;
    OnHitByMegaChar(*player);
}

/* STATE_BOUNCING enter routine: an Earthquake at the block's position
   (argument 0x5dc000, 1500 units), then the bounce begins: mBounceAng =
   0x4000 (a quarter turn) and mBounceTimer = 7 frames. */
void daObjHatenaBlock_c::EnterBouncing() {
    s32 vec[3];
    vec[0] = mPosX;
    vec[1] = mPosY;
    vec[2] = mPosZ;
    Earthquake(*(const Vector3 *)vec, 0x5dc000);
    mBounceAng = 0x4000;
    mBounceTimer = 7;
}

/* STATE_BOUNCING update routine.
 *
 * pos = the block's position, 50 units up (func_ov102_02149684).
 *
 * While mBounceTimer is still counting down (DecIfAbove0_Short returns the
 * new value; non-zero means it has not reached 0), squash and stretch, with
 * s = sin(mBounceAng) as Fix12 from FX_SinCosTable_ (0x1000 = 1.0):
 *   mScaleY       = (1 + s) * 0x999/0x1000 + 0x666/0x1000
 *                 = 0.4 + 0.6 * (1 + s)
 *   mScaleX, Z    = (1 - s) + 1.0
 *   mBounceYOffs  = (1 - s) * 13 (units)
 * then mBounceAng += 0x1000 (1/16 turn). The first frame starts at 0x4000
 * (s = 1: scale Y 1.6, X and Z 1.0, no offset).
 *
 * On the first update where the countdown returns 0 (the seventh after the
 * bounce begins, since mBounceTimer starts at 7) the block pops:
 *  - a SECRET_COIN that registered itself in mHeldActor gets
 *    daSCoin_c::Collect; mHeldActor is cleared;
 *  - sound 0 through Sound::PlayBank3 at the camera-space position, and
 *    particles 0xb and 0xd at pos, plus one that depends on the block (0xc
 *    for HATENA_BLOCK, 0x10 for ITEM / VS, 9 for the caps; IDs unnamed here);
 *  - the prize, per actor ID:
 *      ITEM_BLOCK     row mContentType of data_ov102_0214e8c0, column 0
 *      VS_ITEM_BLOCK  row mContentType of data_ov102_0214e870, column
 *                     mHitterParam clamped to 0..3
 *      HATENA_BLOCK   the same row/column of data_ov102_0214e8c0, unless
 *                     SaveData::HasPlayerLostCap() is non-zero and LEVEL_ID
 *                     is not 0x1f; then it spawns a BOMBHEI
 *                     (SpawnPrizeBobOmb) instead
 *      CAP_BLOCK_M/L/W  an OBJ_MARIO_CAP (SpawnPrizeMarioCap) for character
 *                     0 / 1 / 2 when HasPlayerLostCap() is 0; otherwise a
 *                     BOMBHEI
 *  - enters STATE_POPPED.
 * The `ch >= 4` tests below cannot fire: ch was clamped to 3 above. */
void daObjHatenaBlock_c::UpdateBouncing() {
    Vector3 pos;
    int ch;
    u16 typ;
    dActor_c *held;
    func_ov102_02149684((int *)&pos, this);
    if (DecIfAbove0_Short(&mBounceTimer) != 0) {

        mScaleY = FX_MUL(FX_SinIdx((u16)mBounceAng >> 4) + 0x1000, 0x999) + 0x666;
        mScaleX = FX_MUL(0x1000 - FX_SinIdx((u16)mBounceAng >> 4), 0x1000) + 0x1000;
        mScaleZ = mScaleX;
        mBounceYOffs = (0x1000 - FX_SinIdx((u16)mBounceAng >> 4)) * 0xd;
        mBounceAng += 0x1000;

        return;
    }
    held = mHeldActor;
    if (held != 0) {
        if (held->actorID == ACTOR_SECRET_COIN)
            ((daSCoin_c *)held)->Collect();
        mHeldActor = 0;
    }
    Sound::PlayBank3(0, mCamSpacePos);
    Particle::System::NewSimple(kPtclBlockBreakA, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(kPtclBlockBreakB, pos.x, pos.y, pos.z);
    typ = actorID;
    switch (typ - ACTOR_HATENA_BLOCK) {
    case 1: case 2:   /* ITEM_BLOCK, VS_ITEM_BLOCK */
        Particle::System::NewSimple(kPtclItemBlockBreak, pos.x, pos.y, pos.z);
        break;
    case 0:           /* HATENA_BLOCK */
        Particle::System::NewSimple(kPtclHatenaBlockBreak, pos.x, pos.y, pos.z);
        break;
    case 3: case 4: case 5:   /* CAP_BLOCK_M, W, L */
        Particle::System::NewSimple(kPtclCapBlockBreak, pos.x, pos.y, pos.z);
        break;
    }
    ch = (int)mHitterParam;
    if (ch < 0) ch = 0; else if (ch > 3) ch = 3;
    typ = actorID;
    switch (typ - ACTOR_HATENA_BLOCK) {
    case 1: {   /* ITEM_BLOCK */
        PMF (*tbl)[4] = data_ov102_0214e8c0;
        u8 content = mContentType;
        (this->*tbl[content][0])();
        break;
    }
    case 2: {   /* VS_ITEM_BLOCK */
        PMF (*tbl)[4] = data_ov102_0214e870;
        u8 content = mContentType;
        if (ch >= 4) ch = 0;
        (this->*tbl[content][ch])();
        break;
    }
    case 0:     /* HATENA_BLOCK */
        if (SaveData::HasPlayerLostCap() == 0 || gCurrentSublevel == 0x1f) {
            PMF (*tbl)[4] = data_ov102_0214e8c0;
            u8 content = mContentType;
            if (ch >= 4) ch = 0;
            (this->*tbl[content][ch])();
        } else {
            SpawnPrizeBobOmb();
        }
        break;
    case 3:     /* CAP_BLOCK_M: character 0 */
        if (SaveData::HasPlayerLostCap() == 0)
            SpawnPrizeMarioCap(0, 0x12);
        else
            SpawnPrizeBobOmb();
        break;
    case 5:     /* CAP_BLOCK_L: character 1 */
        if (SaveData::HasPlayerLostCap() == 0)
            SpawnPrizeMarioCap(1, 0x12);
        else
            SpawnPrizeBobOmb();
        break;
    case 4:     /* CAP_BLOCK_W: character 2 */
        if (SaveData::HasPlayerLostCap() == 0)
            SpawnPrizeMarioCap(2, 0x12);
        else
            SpawnPrizeBobOmb();
        break;
    }
    ChangeState(STATE_POPPED);
}

/* STATE_POPPED enter routine: mBounceTimer = 0x12c (300 frames) and vertical
   acceleration -0x8000 (-8 units). */
void daObjHatenaBlock_c::EnterPopped() {
    mBounceTimer = 0x12c;
    mVertAccel = -0x8000;
}

/* STATE_POPPED update routine. Counts mBounceTimer down; once it reads 0 and
   the closest player is 0x64000 (100 units) away or less, returns
   without changing state; farther than that, the block re-enters STATE_IDLE.
   The state dispatcher does not consume a return value. */
void daObjHatenaBlock_c::UpdatePopped() {
    int r = DecIfAbove0_Short(&mBounceTimer);
    if (r != 0)
        return;
    r = DistToCPlayer();
    if (r <= 0x64000)
        return;
    ChangeState(STATE_IDLE);
}

/* Ground-pounded: ignored while BOUNCING or when IsHitBlocked says
   the block cannot be hit; otherwise records the pounder's param1 and starts
   the bounce. */
void daObjHatenaBlock_c::OnGroundPounded(dActor_c &other)
{
    if (mState == STATE_BOUNCING) return;
    int r = IsHitBlocked();
    if (r != 0) return;
    mHitterParam = other.param1;
    ChangeState(STATE_BOUNCING);
}

/* Attacked: the same accept test and bounce start as OnGroundPounded (declared
   int, but with no return statement on any path, like OnHitFromUnderneath). */
int daObjHatenaBlock_c::OnAttacked1(dActor_c &other)
{
    int v = mState;
    if (v != STATE_BOUNCING) {
        if (!IsHitBlocked()) {
            int val = other.param1;
            mHitterParam = val;
            ChangeState(STATE_BOUNCING);
        }
    }
}

/* Kicked: the same accept test and bounce start as OnGroundPounded. */
void daObjHatenaBlock_c::OnKicked(dActor_c &other)
{
    if (mState == STATE_BOUNCING) return;
    int r = IsHitBlocked();
    if (r != 0) return;
    mHitterParam = other.param1;
    ChangeState(STATE_BOUNCING);
}

/* Hit by a mega player: the same accept test as OnGroundPounded; when
   accepted, counts the hit (daPly_c::IncMegaKillCount), records the player's
   param1 and starts the bounce. */
void daObjHatenaBlock_c::OnHitByMegaChar(daPly_c &player) {
    if (mState == STATE_BOUNCING) return;
    if (IsHitBlocked() != 0)
        return;
    player.IncMegaKillCount();
    mHitterParam = player.param1;
    ChangeState(STATE_BOUNCING);
}

/* Hit from underneath: unless already BOUNCING, first sets mVertAccel to
   -0x8000 (-8 units) and mVertSpeed to 0x1e000 (30 units), whether or not
   the hit is then accepted; the accept test and bounce start are the same as
   OnGroundPounded's. */
int daObjHatenaBlock_c::OnHitFromUnderneath(dActor_c &other)
{
    if (mState != STATE_BOUNCING) {
        mVertAccel = -0x8000;
        mVertSpeed = 0x1e000;
        if (!IsHitBlocked()) {
            mHitterParam = other.param1;
            ChangeState(STATE_BOUNCING);
        }
    }
}

/* dst = the block's position with Y raised by 0x32000 (50 units): the point
   the prizes spawn at. */
void func_ov102_02149684(int* dst, daObjHatenaBlock_c* src){
  int z = src->mPosZ;
  int liftedY = src->mPosY + 0x32000;
  dst[0] = src->mPosX;
  dst[1] = liftedY;
  dst[2] = z;
}

/* Floor height under the block: a ground raycast (dBgCh_Gnd) from 0x28000 (40
   units) above the block, with probe height 0x3e8000 (1000 units). Returns
   the hit height, or mPosY when nothing is hit. */
int daObjHatenaBlock_c::GetFloorY() {
    V3 pos(mPosX, mPosY + 0x28000, mPosZ);
    dBgCh_Gnd rg;
    rg.SetObjAndPos(*(Vector3 *)&pos, 0);
    rg.mProbeHeight = 0x3e8000;
    int r = mPosY;
    if (rg.DetectClsn())
        r = rg.clsnY;
    return r;
}

struct HbSpawnFrame { Vector3 pos; int vel[3]; };

/* Cap prize: spawns an OBJ_MARIO_CAP at the prize point with param1 =
   p2 | (p1 << 8) (cap type 0x12 for character p1) and no rotation. If it
   spawned, the cap gets heading (RandomIntInternal() + camera mAngleY +
   0x8000, truncated to 16 bits) in its mPrevAngleY, speed 0x3320 (about 3.2
   units) in mHorzSpeed, and a velocity vector (0, 0x11000, 0) in
   mSpeedX / mVertSpeed / mSpeedZ (0x11000 = 17 units up). */
void daObjHatenaBlock_c::SpawnPrizeMarioCap(int p1, int p2) {
    HbSpawnFrame f;
    int rnd;
    dActor_c* o;
    dCamera_c* g;
    func_ov102_02149684((int*)&f.pos, this);
    o = dActor_c::Spawn(
        ACTOR_OBJ_MARIO_CAP, (unsigned int)(p2 | (p1 << 8)), f.pos, 0, mAreaId, -1);
    if (o == 0) return;
    g = gCamera;
    f.vel[0] = 0;
    f.vel[1] = 0x11000;
    f.vel[2] = 0;
    rnd = RandomIntInternal(&data_0209e650.value);
    {
        int vsum = rnd + (g->mAngleY + 0x8000);
        o->mPrevAngleX = 0;
        o->mPrevAngleY = (short)vsum;
        o->mPrevAngleZ = 0;
        o->mHorzSpeed = 0x3320;
        o->mSpeedX = f.vel[0];
        o->mVertSpeed = f.vel[1];
        o->mSpeedZ = f.vel[2];
    }
}

/* CONTENT_COINS prize: count = param1 bits 8..15 (0xff reads as 1); sprays
   that many coins from the prize point (SpawnCoinSpray, speed 0x1800 =
   1.5 units, base angle 0), then KillAndTrackInDeathTable. */
void daObjHatenaBlock_c::SpawnPrizeCoins() {
    int s[3];
    func_ov102_02149684(s, this);
    int count = (param1 >> 8) & 0xff;
    if (count == 0xff)
        count = 1;
    int w[3];
    w[0] = s[0];
    w[1] = s[1];
    w[2] = s[2];
    SpawnCoinSpray((Vector3 *)w, count, 0x1800, 0);
    KillAndTrackInDeathTable();
}

/* CONTENT_STAR prize: spawns a STAR at the prize point with param mStarId |
   0x40 (the 0x40 bit is not interpreted here), then MarkForDestruction. */
void daObjHatenaBlock_c::SpawnPrizeStar() {
    char local[12];
    func_ov102_02149684((int *)local, this);
    dActor_c::Spawn(ACTOR_STAR, mStarId | 0x40, *(Vector3 *)local, 0, mAreaId, -1);
    MarkForDestruction();
}

/* CONTENT_ONEUP prize: spawns a ONEUPKINOKO at the prize point, then
   KillAndTrackInDeathTable. */
void daObjHatenaBlock_c::SpawnPrize1Up() {
    struct Vector3 pos;
    func_ov102_02149684((int *)&pos, this);
    dActor_c::Spawn(ACTOR_ONEUPKINOKO, 0, pos, 0, mAreaId, -1);
    KillAndTrackInDeathTable();
}

/* CONTENT_SCALEUP_KINOKO prize: spawns a SCALEUP_KINOKO at the prize point.
   Does not kill the block. */
void daObjHatenaBlock_c::SpawnPrizeScaleUpKinoko() {
    Vector3 v;
    func_ov102_02149684((int *)&v, this);
    signed char cc = mAreaId;
    dActor_c::Spawn(ACTOR_SCALEUP_KINOKO, 0, v, (const Vector3_16*)0, cc, -1);
}

/* CONTENT_SHELL prize: spawns a SHELL at the prize point and, if it spawned,
   sets the new shell's mDespawnTimer to 0xb4 (180). */
void daObjHatenaBlock_c::SpawnPrizeShell() {
    Vector3 v;
    func_ov102_02149684((int *)&v, this);
    void *a = dActor_c::Spawn(ACTOR_SHELL, 0, v, 0, mAreaId, -1);
    if (a)
        ((daShl_c *)a)->mDespawnTimer = 0xb4;
}

/* FEATHER prize: spawns a FEATHER at the prize point with rotation taken
   from the three u16 words at data_020a0edc, except Y: the block's own
   mAngleY when gGameMode (CURRENT_GAMEMODE) is 1, else the camera's
   mAngleY + 0x4000. */
void daObjHatenaBlock_c::SpawnPrizeFeather() {
    struct Vector3 pos;
    struct Vector3_16 rot;
    func_ov102_02149684((int *)&pos, this);
    /* Raw: as Vector3_16 the three loads become ldrsh where the ROM has ldrh
       (copy and field-wise both, measured 2026-11-14). */
    rot.x = *(u16 *)(data_020a0edc);
    rot.y = *(u16 *)(data_020a0edc + 2);
    rot.z = *(u16 *)(data_020a0edc + 4);
    if ((int)(*(unsigned char *)(&gGameMode) == 1) != 0) {
        rot.y = mAngleY;
    } else {
        rot.y = gCamera->mAngleY + 0x4000;
    }
    dActor_c::Spawn(ACTOR_FEATHER, 0, pos, &rot, mAreaId, -1);
}

/* POWER_UP_ITEM prize: spawns a POWER_UP_ITEM (param 0) at the prize point. */
void daObjHatenaBlock_c::SpawnPrizePowerUpItem() {
    Vector3 v;
    func_ov102_02149684((int *)&v, this);
    dActor_c::Spawn(ACTOR_POWER_UP_ITEM, 0u, v, 0, mAreaId, -1);
}

/* Bob-omb prize: spawns a BOMBHEI (param 4) at the prize point; if it
   spawned, func_ov102_0214ad14 (daBmb_c: stores ClosestPlayer as its chase
   target) and func_ov102_0214b384(0x3c) (daBmb_c.h: only shortens the
   fuse). Callers discard the result of that helper. */
void daObjHatenaBlock_c::SpawnPrizeBobOmb() {
    Vector3 v;
    func_ov102_02149684((int *)&v, this);
    daBmb_c *a = (daBmb_c *)dActor_c::Spawn(ACTOR_BOMBHEI, 4, v, 0, mAreaId, -1);
    if (a == 0)
        return;
    a->func_ov102_0214ad14();
    a->func_ov102_0214b384(0x3c);
}

/* Spawns n COINs (param 2) at *pos. Each coin gets a heading baseAngle + dir
   in its mPrevAngleY, where dir is a 5-bit signed value times 0x800 (-0x8000
   .. 0x7800, a full turn is 0x10000) drawn from RandomIntInternal, re-drawn
   while it equals the previous coin's dir. The speed (initially the caller's
   `speed`) is scaled by (random % 50 + 100) / 100 for each coin, and the
   scaled value carries over to the next coin. */
void daObjHatenaBlock_c::SpawnCoinSpray(Vector3 *pos, int n, unsigned int speed, short baseAngle) {
    dActor_c *a;
    int dir;
    unsigned int rnd;
    unsigned int q;
    int prevDir = 0xff;
    int i = 0;

    if (n <= 0) return;

    do {
        a = dActor_c::Spawn(ACTOR_COIN, 2, *pos, 0, mAreaId, -1);
        if (a != 0) {
            do {
                rnd = (unsigned int)RandomIntInternal(&data_0209e650.value);
                dir = (int)((rnd >> 0x10) << 0x1b) >> 0x10;
            } while (dir == prevDir);
            rnd = (unsigned int)RandomIntInternal(&data_0209e650.value);
            a->mPrevAngleX = 0;
            q = rnd >> 0x10;
            speed = (speed * (q % 50 + 100)) / 100;
            prevDir = dir;
            a->mPrevAngleY = baseAngle + dir;
            a->mPrevAngleZ = 0;
            a->mHorzSpeed = speed;
        }
        i++;
    } while (i < n);
}

/* Whether the block cannot be hit right now: returns 1 in level 0x15 when
   mHomePosY - 0x32000 (50 units) is at or below data_0209f32c
   (WATER_HEIGHT in symbols/verified.tsv); in any other level except 0x21 (0
   there) when the closest player's mIsUnderwater is non-zero; otherwise 0.
   The levels are LEVEL_ID values (gCurrentSublevel); 0x15 is Wet-Dry World per
   dActor_c::GetWaterHeightWDW, the tree does not name 0x21. */
int daObjHatenaBlock_c::IsHitBlocked() {
    if (gCurrentSublevel == 0x15) {
        if ((int)(mHomePosY - 0x32000) <= data_0209f32c)
            return 1;
    } else {
        if (gCurrentSublevel == 0x21)
            return 0;
        if (ClosestPlayer()->mIsUnderwater)
            return 1;
    }
    return 0;
}

extern SpawnInfo<daObjHatenaBlock_c> g_profile_VS_ITEM_BLOCK = {
    daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK,
    0x16,
    0x28,
    0x2,
    0x60000,
    0x100000,
    0x1000000,
    0x1000000};

extern SpawnInfo<daObjHatenaBlock_c> g_profile_HATENA_BLOCK = {
    daObjHatenaBlock_c_classInit_HATENA_BLOCK,
    0x14,
    0x26,
    0x2,
    0x60000,
    0x100000,
    0x1000000,
    0x1000000};

extern SpawnInfo<daObjHatenaBlock_c> g_profile_ITEM_BLOCK = {
    daObjHatenaBlock_c_classInit_ITEM_BLOCK,
    0x15,
    0x27,
    0x2,
    0x60000,
    0x100000,
    0x1000000,
    0x1000000};

extern SpawnInfo<daObjHatenaBlock_c> g_profile_CAP_BLOCK_M = {
    daObjHatenaBlock_c_classInit_CAP_BLOCK_M,
    0x17,
    0x29,
    0x2,
    0x60000,
    0x100000,
    0x1000000,
    0x1000000};

extern SpawnInfo<daObjHatenaBlock_c> g_profile_CAP_BLOCK_L = {
    daObjHatenaBlock_c_classInit_CAP_BLOCK_L,
    0x19,
    0x2b,
    0x2,
    0x60000,
    0x100000,
    0x1000000,
    0x1000000};

extern SpawnInfo<daObjHatenaBlock_c> g_profile_CAP_BLOCK_W = {
    daObjHatenaBlock_c_classInit_CAP_BLOCK_W,
    0x18,
    0x2a,
    0x2,
    0x60000,
    0x100000,
    0x1000000,
    0x1000000};
