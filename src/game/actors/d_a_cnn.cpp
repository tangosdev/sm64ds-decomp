//cpp
/* The cannon (CANNON profile), ov098 .text 0x0213a900..0x0213b47c,
 * thirteen functions.
 *
 * Variant 2 starts sunk into the ground. When a player touches it, it climbs
 * them in, rises, turns, tilts up, follows the camera until the player has
 * been shot out, then sinks again. The other variants aim at a random heading
 * near their home angle and, while a player is within 2048.0, fire a water
 * bomb (WATERBOMB, spawn parameter 3) and recoil; variant 1 never runs its
 * states and only holds its barrel up.
 * Behavior runs one of four states out of a pointer-to-member table that the
 * module's static initializer fills:
 *
 *   0 StateAim           ease the barrel to its target, then fire
 *   1 StateFire          count down, launch a water bomb, recoil
 *   2 StateWaitForRider  wait for a player to climb in
 *   3 StateLoad          rise, turn, shoot the player out
 *
 * NAME: daCnn_c is the cartridge's RTTI spelling. _ZTS at ov098 0x0213c664 is
 * the string "7daCnn_c", and the _ZTI at 0x0213c670 names the vtable at
 * 0x0213c6a0 as this class's. The tree called the class Cannon until then.
 * daCnn_c_classInit is reconstructed (RTTI daCnn_c, CANNON registry);
 * retail does not store that spelling. Historical alias: Cannon_Spawn.
 *
 * THE DESTRUCTOR IS INLINE AND EMPTY in the class header, so this TU emits
 * _ZTV7daCnn_c, _ZTI7daCnn_c and _ZTS7daCnn_c with the inherited bases' RTTI
 * records, and D1 then D0 in the cartridge's order.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so
 * the highest-address ROM function, the factory, is written FIRST here. Do
 * not reorder.
 *
 * The state, aim and pose names are descriptive; the cartridge keeps no
 * names for them. The state table's pointer-to-member records are still the
 * sinit's .data words, resolved to the mangled spellings through symbols.txt.
 * Leftover: UpdateModel and InitResources reach the model's bone
 *   records through raw offsets; BMD_Bone does not name those fields.
 * Leftover: the fire state's launch block keeps the legacy match's shape (a
 *   volatile velocity copy read through an opaque pointer, hand-hoisted
 *   sine/cosine operands); written plainly it schedules differently and
 *   grows 4 bytes. The load state keeps an unused Vector3 that holds the
 *   ROM's 0x1c frame. The smoke rotation is read as three u16 words: copied
 *   as a Vector3_16 the compiler loads it with ldrsh, the ROM with ldrh.
 * Leftover: the model file handles (data_ov098_0213c8e8, data_ov098_0213c91c,
 *   data_ov002_0210da38), the state table (data_ov098_0213c8fc, .bss) and the
 *   particle rotation (data_ov098_0213c63c) are unnamed rows this TU does not
 *   own; func_ov002_020df300 / func_ov002_020df34c and func_ov102_0214ae1c
 *   are unnamed Player / Bob-omb helpers.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_cnn_AssetId { ASSET_HOUDAI_BMD = 0x045e };

#include "common.h"
#include "daCnn_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "Particle__System.h"

typedef void (daCnn_c::*State)();

/* One 0x34-byte runtime bone of mModel.data.bones (ModelComponents types the
   array as BMD_Bone, but the pose pass fills these). Extends
   d_a_moray.cpp's MorayBone view (rotX/rotY/rotZ at 0x1a) with the
   translation at 0x20: InitResources keeps the barrel bone's transY as its
   rest height, and the barrel's recoil slides transY/transZ. */
struct CnnBone {
    u8 pad_00[0x1a];
    s16 rotX;   /* 0x1a */
    s16 rotY;   /* 0x1c */
    s16 rotZ;   /* 0x1e */
    s32 transX; /* 0x20 */
    s32 transY; /* 0x24 */
    s32 transZ; /* 0x28 */
    u8 pad_2c[0x8];
};
#ifndef SM64DS_PLATFORM_PC
typedef char CnnBone_size_must_be_0x34[sizeof(CnnBone) == 0x34 ? 1 : -1];
#endif

/* The cannon's model, and the water bomb's two (bomb and fragment), which
   InitResources preloads for the bombs it fires. */
extern SharedFilePtr data_ov098_0213c91c;
extern SharedFilePtr data_ov002_0210da38;
/* The smoke particles' base rotation, x/y/z. */
extern u16 data_ov098_0213c63c[3];
/* The sine and cosine table. */
extern s16 data_02082214[];
extern int data_0209e650;

/* local extern: arm9/ov002/ov102 C helpers with no shared header yet. The
   two Player helpers are defined in Player.cpp, the Bob-omb one in
   d_a_bmb.cpp. */
extern "C" {
int AngleDiff(int a, int b);
short GetAngleToCamera(int playerNo);

int func_ov002_020df300(Player *player);
int func_ov002_020df34c(Player *player);
void func_ov102_0214ae1c(dActor_c *bomb);
}

/* The cannon's file-scope SharedFilePtr object, spelled through the same
   wrapper idiom the landed folds use: the handle's code is the veneer pair
   the cartridge already carries, so the wrapper gets no bodies of its own.
   The constructor aliases func_02017acc and the destructor func_02017ab4 in
   this unit's manifest. */
struct CnnModelFilePtr : SharedFilePtr {
    CnnModelFilePtr(unsigned int id);
    ~CnnModelFilePtr();
};

/* ------------------------------------------------------------------------
 * The model handle and the state table's four member pointers, in
 * static-initializer order: the initializer constructs the handle, registers
 * its destructor, then copies the pointers from .data. InitResources feeds
 * the model to dExtSimpleModel_c::LoadFile.
 * ------------------------------------------------------------------------ */
// @symbol __sinit_d_a_cnn.cpp
CnnModelFilePtr data_ov098_0213c8e8(ASSET_HOUDAI_BMD);

/* The member pointers the initializer copies into the table
   (0x0213c644..0x0213c664 in .data, an unclaimed run). Each record is one
   eight-byte {fn, adj} pair. */
extern State data_ov098_0213c64c;
extern State data_ov098_0213c65c;
extern State data_ov098_0213c654;
extern State data_ov098_0213c644;

State data_ov098_0213c8fc[4] = {
    data_ov098_0213c64c,
    data_ov098_0213c65c,
    data_ov098_0213c654,
    data_ov098_0213c644,
};

enum {
    kPlayerActorID = 0xbf,
    kBobOmbActorID = 0xce,    /* BOMBHEI */
    kWaterBombActorID = 0xd0, /* WATERBOMB */
};

/* -------------------------------------------------------------------------- */
// @symbol daCnn_c_classInit
extern "C" daCnn_c *daCnn_c_classInit() {
    return new daCnn_c();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c13InitResourcesEv
int daCnn_c::InitResources() {
    BMD_File *file = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov098_0213c8e8);
    mModel.SetFile(file, 1, -1);
    mBarrelRestY = ((CnnBone *)mModel.data.bones)[1].transY;
    mVariant = param1 & 3;
    mTimer = 0;
    mPosY -= 0x50000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mHomeAngleY = mPrevAngleY;
    mLoadStep = 0;

    if (mVariant != 2) {
        if (mVariant == 1)
            mAngleX = 0x2000;
        mTargetPitch = 0x2000;
        mTargetAngleY = mAngleY;
        mState = 0;
        PickNextAim();
        mdCcAc_c.Init(this, 0xa0000, 0x12c000, 0x800004, 0);
    } else {
        mPosY -= 0x190000;
        mTargetAngleY = mAngleZ;
        mTargetPitch = 0x2000;
        mState = 2;
        mFlags &= ~1;
        mdCcAc_c.Init(this, 0x50000, 0x12c000, 0x800004, 0);
    }

    dExtSimpleModel_c::LoadFile(data_ov098_0213c91c);
    dExtSimpleModel_c::LoadFile(data_ov002_0210da38);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c8BehaviorEv
int daCnn_c::Behavior() {
    if (mVariant != 1)
        (this->*data_ov098_0213c8fc[mState])();

    UpdateModel();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c6RenderEv
int daCnn_c::Render() {
    if (mState == 3 && mLoadStep >= 3)
        return 1;

    mModel.UpdateVerts();
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c16CleanupResourcesEv
int daCnn_c::CleanupResources() {
    data_ov098_0213c8e8.Release();
    data_ov098_0213c91c.Release();
    data_ov002_0210da38.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c11PickNextAimEv
/* Pick the next aim: a random pitch in 0x1000..0x1fff and a random heading
   within a quarter turn either side of home, at least 0x800 from the current
   heading. */
void daCnn_c::PickNextAim() {
    int angleY;
    mTargetPitch =
        (short)(((unsigned int)RandomIntInternal(&data_0209e650) >> 16 & 0xfff) + 0x1000);
    angleY = mAngleY;
    do {
        mTargetAngleY = (short)(mHomeAngleY + 0xe000 +
                                ((unsigned int)RandomIntInternal(&data_0209e650) >> 16 & 0x3fff));
    } while (AngleDiff(angleY, mTargetAngleY) < 0x800);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c8StateAimEv
/* State 0, aim. */
void daCnn_c::StateAim() {
    short prevAngleY = mPrevAngleY;
    ApproachLinear(mPrevAngleX, mTargetPitch, 0x100);
    ApproachLinear(mPrevAngleY, mTargetAngleY, 0x100);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    {
        short target = mTargetAngleY;
        if (prevAngleY != target && mPrevAngleY == target)
            Sound::PlayBank3(0x14d, mCamSpacePos);
    }
    if (mPrevAngleX != mTargetPitch)
        return;
    if (mPrevAngleY == mTargetAngleY) {
        mState = 1;
        mTimer = 120;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c9StateFireEv
/* State 1, fire: while a player is within 2048.0, count down; at 60 launch a
   water bomb out of the barrel with a puff of smoke and kick the barrel back,
   and at 0 pick a new aim. The barrel slides back under mRecoilSpeed and
   springs home. */
void daCnn_c::StateFire() {
    Vector3_16 rot;
    Vector3 pos;
    /* The launch block keeps the legacy match's statement shape: the
       sine/cosine operands are hoisted by hand and the ball's velocity is
       read through an opaque pointer into a volatile copy. Written as plain
       member expressions the block schedules differently and grows 4 bytes. */
    volatile int vel[3];
    Player *closest;
    int timer;

    if (mTimer != 0)
        mTimer -= 1;

    closest = ClosestPlayer();
    if (Vec3_Dist(&mPos, &closest->mPos) >= 0x800000)
        return;

    timer = mTimer;
    if (timer == 0) {
        mState = 0;
        PickNextAim();
        Sound::PlayBank3(0x14c, mCamSpacePos);
        return;
    }

    if (timer == 60) {
        dActor_c *ball;
        s16 *sinCos;
        int speed;
        s16 elevation;
        int idx;

        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        pos.y += 0x80000;

        {
            dActor_c *spawned = dActor_c::Spawn(kWaterBombActorID, 3, pos, 0, mAreaId, -1);
            s16 *table = data_02082214;
            int launchSpeed = 100;
            s16 pitch = mPrevAngleX;
            int up = (s16)(0x4000 - pitch);
            int i = (u16)up >> 4;
            int sine = table[i * 2];
            sinCos = table;
            speed = launchSpeed;
            ball = spawned;
            ball->mSpeedX = 0;
            ball->mVertSpeed = sine * launchSpeed;
            ball->mSpeedZ = 0;
        }

        elevation = (s16)(0x4000 - mPrevAngleX);
        idx = (u16)elevation >> 4;
        ball->mHorzSpeed = sinCos[idx * 2 + 1] * speed;

        {
            s16 yaw = mPrevAngleY;
            ball->mPrevAngleX = 0;
            ball->mPrevAngleY = yaw;
            ball->mPrevAngleZ = 0;
        }

        ball->UpdatePos(0);

        /* The muzzle is two frames of the ball's travel out from the spawn
           point. */
        {
            int *vp = (int *)(int)&ball->mSpeedX;
            int vx = *vp;
            int px = pos.x;
            vel[0] = vx;
            int vy = *(vp + 1);
            int py = pos.y;
            vel[1] = vy;
            int vz = *(vp + 2);
            int x = px + (vx << 1);
            int pz = pos.z;
            vel[2] = vz;
            int y = py + (vy << 1);
            mMuzzlePosX = x;
            mMuzzlePosY = y;
            mMuzzlePosZ = pz + (vz << 1);
        }

        rot.x = data_ov098_0213c63c[0];
        rot.y = data_ov098_0213c63c[1];
        rot.z = data_ov098_0213c63c[2];
        rot.x = (u16)data_02082214[((u16)mPrevAngleY >> 4) * 2];
        rot.z = (u16)data_02082214[((u16)mPrevAngleY >> 4) * 2 + 1];

        Particle::System::New(0, 7, mMuzzlePosX, mMuzzlePosY, mMuzzlePosZ,
                              (const Vector3_16f *)(&rot), 0);
        Particle::System::New(0, 8, mMuzzlePosX, mMuzzlePosY, mMuzzlePosZ,
                              (const Vector3_16f *)(&rot), 0);

        Sound::PlayBank3(0xd4, mCamSpacePos);
        mRecoilSpeed = -0x1800;
        return;
    }

    mRecoil += mRecoilSpeed;
    mRecoilSpeed += 0xc00;
    if (mRecoilSpeed >= 0x800)
        mRecoilSpeed = 0x800;
    if (mRecoil >= 0) {
        mRecoilSpeed = 0;
        mRecoil = mRecoilSpeed;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c17StateWaitForRiderEv
/* State 2, wait: when a player touches the collider, climb them in and start
   the load sequence. A player carrying a Bob-omb has it handed to
   func_ov102_0214ae1c instead. */
void daCnn_c::StateWaitForRider() {
    u32 id = mdCcAc_c.otherOwner;
    dActor_c *other;
    if (id == 0)
        return;
    other = dActor_c::FindWithID(id);
    if (other == 0)
        return;
    int isPlayer = other->actorID == kPlayerActorID;
    if (isPlayer) {
        Player *player = (Player *)other;
        dActor_c *held = (dActor_c *)player->mHeldObj;
        if (held != 0) {
            int isBobOmb = held->actorID == kBobOmbActorID;
            if (isBobOmb) {
                func_ov102_0214ae1c(held);
                return;
            }
        }
        if (func_ov002_020df34c(player) == 0)
            return;
        mState = 3;
        mLoadStep = 0;
        mPlayer = player;
        mdCcAc_c.flags |= 1;
        Sound::PlayBank3(0x14b, mCamSpacePos);
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c9StateLoadEv
/* State 3, the load sequence. The rider is pinned 208.0 above the cannon
   (and turned with it until step 3) while the cannon
     0  rises out of the ground to its spawn height and shakes the camera,
     1  turns to mTargetAngleY,
     2  tilts its barrel up and hands the rider over to func_ov002_020df300,
     3  follows the camera until the rider has been shot out,
     4  sinks back into the ground and returns to waiting. */
void daCnn_c::StateLoad() {
    /* Never used, but it holds the frame at the ROM's 0x1c with quakePos at
       sp+0xc; without it the frame is 0xc. */
    Vector3 pos;
    Vector3 quakePos;

    if (mLoadStep < 4) {
        Player *rider;
        int x, y, z;
        /* z first (statement order) colors y into r2 like the ROM */
        z = mPosZ;
        y = mPosY + 0xd0000;
        rider = mPlayer;
        x = mPosX;
        rider->mPosX = x;
        rider->mPosY = y;
        rider->mPosZ = z;
        if (mLoadStep < 3) {
            s16 angleY = mAngleY;
            s16 angleX = mAngleX;
            rider = mPlayer;
            s16 zero = 0;
            rider->mAngleX = angleX;
            rider->mAngleY = angleY;
            rider->mAngleZ = zero;
            angleY = mAngleY;
            angleX = mAngleX;
            rider = mPlayer;
            rider->mPrevAngleX = angleX;
            rider->mPrevAngleY = angleY;
            rider->mPrevAngleZ = zero;
        }
    }

    switch (mLoadStep) {
    case 0:
        mPosY += 0xa000;
        if (mPosY < mSpawnPosY)
            break;
        mPosY = mSpawnPosY;
        mLoadStep++;
        mTimer = 0;
        quakePos.x = mPosX;
        quakePos.y = mPosY;
        quakePos.z = mPosZ;
        Earthquake(quakePos, 0x1f4000);
        Sound::PlayBank3(0x14c, mCamSpacePos);
        break;

    case 1:
        if (mTimer <= 10)
            break;
        if (!ApproachLinear(mAngleY, mTargetAngleY, 0x200))
            break;
        mLoadStep++;
        mTimer = 0;
        Sound::PlayBank3(0x14d, mCamSpacePos);
        break;

    case 2:
        if (mTimer <= 10)
            break;
        if (!ApproachLinear(mAngleX, 0x2000, 0x200))
            break;
        if (!func_ov002_020df300(mPlayer))
            break;
        Sound::PlayBank3(0x14e, mCamSpacePos);
        mTimer = 0;
        mLoadStep++;
        break;

    case 3:
        mAngleY = GetAngleToCamera(mPlayer->mPlayerNo) + 0x4000;
        if (!mPlayer->IsBeingShotOutOfCannon())
            break;
        mLoadStep++;
        mTimer = 0;
        break;

    case 4: {
        u8 settled = 0;
        mPosY -= 0x32000;
        if (ApproachLinear(mAngleX, 0, 0x800))
            settled++;
        if (ApproachLinear(mAngleY, mHomeAngleY, 0x800))
            settled++;
        if (settled != 2)
            break;
        if (mPosY > mSpawnPosY - 0x190000)
            break;
        mState = 2;
        mdCcAc_c.flags &= ~1;
        mPosY = mSpawnPosY - 0x190000;
        mPlayer = 0;
        mAngleY = mHomeAngleY;
        mPrevAngleY = mAngleY;
        break;
    }
    }

    mTimer++;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c11UpdateModelEv
/* Place the model and pose the barrel: yaw on the root bone, pitch and the
   recoil slide on the barrel bone. */
void daCnn_c::UpdateModel() {
    u16 pitch = mAngleX;
    s16 yaw = mAngleY;
    CnnBone *bones;
    int idx;

    Matrix4x3_FromTranslation(&mModel.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);

    bones = (CnnBone *)mModel.data.bones;
    idx = pitch >> 4;

    bones[0].rotY = yaw;
    bones[1].rotX = pitch;
    bones[1].transX = 0;
    bones[1].transY = mBarrelRestY + (mRecoil * data_02082214[idx * 2 + 1]) / 0x1000;
    bones[1].transZ = (mRecoil * data_02082214[idx * 2]) / 0x1000;
}

// @symbol _ZN7daCnn_cD0Ev
// @symbol _ZN7daCnn_cD1Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
