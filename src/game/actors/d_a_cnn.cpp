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
 *   0 func_ov098_0213b0a4  aim: ease the barrel to its target, then fire
 *   1 func_ov098_0213ade8  fire: count down, launch a water bomb, recoil
 *   2 func_ov098_0213ad08  wait for a player to climb in
 *   3 func_ov098_0213aa28  load sequence: rise, turn, shoot the player out
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
 * Leftover: the four state bodies and the two helpers keep their
 *   func_ov098_* linker names as C-linkage functions over a daCnn_c pointer.
 *   The state table reaches the state bodies only by address, through the
 *   .data words the static initializer copies into it, and
 *   include/decl_common.h declares func_ov098_0213b15c by that name.
 * Leftover: dCcAc_c::Init, dActor_c::Earthquake and Particle::System::New
 *   stay mangled; each takes Fix12<int> by value (notes/mwccarm-codegen.md
 *   6az).
 * Leftover: func_ov098_0213a984 and InitResources reach the model's bone
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

#include "common.h"
#include "daCnn_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"

typedef void (daCnn_c::*State)();

int ApproachLinear(short &value, short target, short step);

extern "C" {
/* The cannon's model, and the water bomb's two (bomb and fragment), which
   InitResources preloads for the bombs it fires. */
extern SharedFilePtr data_ov098_0213c8e8;
extern SharedFilePtr data_ov098_0213c91c;
extern SharedFilePtr data_ov002_0210da38;
/* The state table, indexed by mState. */
extern State data_ov098_0213c8fc[];
/* The smoke particles' base rotation, x/y/z. */
extern u16 data_ov098_0213c63c[3];
/* The sine and cosine table. */
extern s16 data_02082214[];
extern int data_0209e650;

void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
int RandomIntInternal(int *seed);
int AngleDiff(int a, int b);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
short GetAngleToCamera(int playerNo);

int func_ov002_020df300(Player *player);
int func_ov002_020df34c(Player *player);
void func_ov102_0214ae1c(dActor_c *bomb);

void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c *self, dActor_c *actor,
    int radius, int height, u32 flags, u32 vulnFlags);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(dActor_c *self,
    const Vector3 *pos, int magnitude);
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 id, int x, int y, int z, const Vector3_16 *rot, void *cb);

void func_ov098_0213a984(daCnn_c *self);
void func_ov098_0213aa28(daCnn_c *self);
void func_ov098_0213ad08(daCnn_c *self);
void func_ov098_0213ade8(daCnn_c *self);
void func_ov098_0213b0a4(daCnn_c *self);
void func_ov098_0213b15c(daCnn_c *self);
}

enum {
    kPlayerActorID = 0xbf,
    kBobOmbActorID = 0xce,       /* BOMBHEI */
    kWaterBombActorID = 0xd0,    /* WATERBOMB */
};

/* -------------------------------------------------------------------------- */
// @symbol daCnn_c_classInit
extern "C" daCnn_c *daCnn_c_classInit()
{
    return new daCnn_c();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c13InitResourcesEv
int daCnn_c::InitResources()
{
    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov098_0213c8e8);
    mModel.SetFile(file, 1, -1);
    mBarrelRestY = *(s32 *)((u8 *)mModel.data.bones + 0x58);
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
        func_ov098_0213b15c(this);
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0xa0000, 0x12c000, 0x800004, 0);
    } else {
        mPosY -= 0x190000;
        mTargetAngleY = mAngleZ;
        mTargetPitch = 0x2000;
        mState = 2;
        mFlags &= ~1;
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0x50000, 0x12c000, 0x800004, 0);
    }

    Model::LoadFile(data_ov098_0213c91c);
    Model::LoadFile(data_ov002_0210da38);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c8BehaviorEv
int daCnn_c::Behavior()
{
    if (mVariant != 1)
        (this->*data_ov098_0213c8fc[mState])();

    func_ov098_0213a984(this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c6RenderEv
int daCnn_c::Render()
{
    if (mState == 3 && mLoadStep >= 3)
        return 1;

    mModel.UpdateVerts();
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daCnn_c16CleanupResourcesEv
int daCnn_c::CleanupResources()
{
    data_ov098_0213c8e8.Release();
    data_ov098_0213c91c.Release();
    data_ov002_0210da38.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213b15c
/* Pick the next aim: a random pitch in 0x1000..0x1fff and a random heading
   within a quarter turn either side of home, at least 0x800 from the current
   heading. */
extern "C" void func_ov098_0213b15c(daCnn_c *self)
{
    int angleY;
    self->mTargetPitch = (short)(((unsigned int)RandomIntInternal(&data_0209e650) >> 16 & 0xfff) + 0x1000);
    angleY = self->mAngleY;
    do {
        self->mTargetAngleY = (short)(self->mHomeAngleY + 0xe000 + ((unsigned int)RandomIntInternal(&data_0209e650) >> 16 & 0x3fff));
    } while (AngleDiff(angleY, self->mTargetAngleY) < 0x800);
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213b0a4
/* State 0, aim. */
extern "C" void func_ov098_0213b0a4(daCnn_c *self)
{
    short prevAngleY = self->mPrevAngleY;
    ApproachLinear(self->mPrevAngleX, self->mTargetPitch, 0x100);
    ApproachLinear(self->mPrevAngleY, self->mTargetAngleY, 0x100);
    self->mAngleX = self->mPrevAngleX;
    self->mAngleY = self->mPrevAngleY;
    {
        short target = self->mTargetAngleY;
        if (prevAngleY != target && self->mPrevAngleY == target)
            Sound::PlayBank3(0x14d, *(const Vector3 *)&self->mCamSpacePosX);
    }
    if (self->mPrevAngleX != self->mTargetPitch) return;
    if (self->mPrevAngleY == self->mTargetAngleY) {
        self->mState = 1;
        self->mTimer = 120;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213ade8
/* State 1, fire: while a player is within 2048.0, count down; at 60 launch a
   water bomb out of the barrel with a puff of smoke and kick the barrel back,
   and at 0 pick a new aim. The barrel slides back under mRecoilSpeed and
   springs home. */
extern "C" void func_ov098_0213ade8(daCnn_c *self)
{
    Vector3_16 rot;
    Vector3 pos;
    /* The launch block keeps the legacy match's statement shape: the
       sine/cosine operands are hoisted by hand and the ball's velocity is
       read through an opaque pointer into a volatile copy. Written as plain
       member expressions the block schedules differently and grows 4 bytes. */
    volatile int vel[3];
    Player *closest;
    int timer;

    if (self->mTimer != 0)
        self->mTimer -= 1;

    closest = self->ClosestPlayer();
    if (Vec3_Dist((Vector3 *)&self->mPosX, (Vector3 *)&closest->mPosX) >= 0x800000)
        return;

    timer = self->mTimer;
    if (timer == 0) {
        self->mState = 0;
        func_ov098_0213b15c(self);
        Sound::PlayBank3(0x14c, *(const Vector3 *)&self->mCamSpacePosX);
        return;
    }

    if (timer == 60) {
        dActor_c *ball;
        s16 *sinCos;
        int speed;
        s16 elevation;
        int idx;

        pos.x = self->mPosX;
        pos.y = self->mPosY;
        pos.z = self->mPosZ;
        pos.y += 0x80000;

        {
            dActor_c *spawned = dActor_c::Spawn(kWaterBombActorID, 3, pos, 0,
                                                self->mAreaId, -1);
            s16 *table = data_02082214;
            int launchSpeed = 100;
            s16 pitch = self->mPrevAngleX;
            int up = (s16)(0x4000 - pitch);
            int i = (u16)up >> 4;
            int sine = table[i * 2];
            sinCos = table;
            speed = launchSpeed;
            ball = spawned;
            ball->unk_0a4 = 0;
            ball->mVertSpeed = sine * launchSpeed;
            ball->unk_0ac = 0;
        }

        elevation = (s16)(0x4000 - self->mPrevAngleX);
        idx = (u16)elevation >> 4;
        ball->mHorzSpeed = sinCos[idx * 2 + 1] * speed;

        {
            s16 yaw = self->mPrevAngleY;
            ball->mPrevAngleX = 0;
            ball->mPrevAngleY = yaw;
            ball->mPrevAngleZ = 0;
        }

        ball->UpdatePos(0);

        /* The muzzle is two frames of the ball's travel out from the spawn
           point. */
        {
            int *vp = (int *)(int)&ball->unk_0a4;
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
            self->mMuzzlePosX = x;
            self->mMuzzlePosY = y;
            self->mMuzzlePosZ = pz + (vz << 1);
        }

        rot.x = data_ov098_0213c63c[0];
        rot.y = data_ov098_0213c63c[1];
        rot.z = data_ov098_0213c63c[2];
        rot.x = (u16)data_02082214[((u16)self->mPrevAngleY >> 4) * 2];
        rot.z = (u16)data_02082214[((u16)self->mPrevAngleY >> 4) * 2 + 1];

        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 7, self->mMuzzlePosX, self->mMuzzlePosY, self->mMuzzlePosZ, &rot, 0);
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 8, self->mMuzzlePosX, self->mMuzzlePosY, self->mMuzzlePosZ, &rot, 0);

        Sound::PlayBank3(0xd4, *(const Vector3 *)&self->mCamSpacePosX);
        self->mRecoilSpeed = -0x1800;
        return;
    }

    self->mRecoil += self->mRecoilSpeed;
    self->mRecoilSpeed += 0xc00;
    if (self->mRecoilSpeed >= 0x800)
        self->mRecoilSpeed = 0x800;
    if (self->mRecoil >= 0) {
        self->mRecoilSpeed = 0;
        self->mRecoil = self->mRecoilSpeed;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213ad08
/* State 2, wait: when a player touches the collider, climb them in and start
   the load sequence. A player carrying a Bob-omb has it handed to
   func_ov102_0214ae1c instead. */
extern "C" void func_ov098_0213ad08(daCnn_c *self)
{
    u32 id = self->mdCcAc_c.otherOwner;
    dActor_c *other;
    if (id == 0) return;
    other = dActor_c::FindWithID(id);
    if (other == 0) return;
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
        if (func_ov002_020df34c(player) == 0) return;
        self->mState = 3;
        self->mLoadStep = 0;
        self->mPlayer = player;
        self->mdCcAc_c.flags |= 1;
        Sound::PlayBank3(0x14b, *(const Vector3 *)&self->mCamSpacePosX);
    }
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213aa28
/* State 3, the load sequence. The rider is pinned 208.0 above the cannon
   (and turned with it until step 3) while the cannon
     0  rises out of the ground to its spawn height and shakes the camera,
     1  turns to mTargetAngleY,
     2  tilts its barrel up and hands the rider over to func_ov002_020df300,
     3  follows the camera until the rider has been shot out,
     4  sinks back into the ground and returns to waiting. */
extern "C" void func_ov098_0213aa28(daCnn_c *self)
{
    /* Never used, but it holds the frame at the ROM's 0x1c with quakePos at
       sp+0xc; without it the frame is 0xc. */
    Vector3 pos;
    Vector3 quakePos;

    if (self->mLoadStep < 4) {
        Player *rider;
        int x, y, z;
        /* z first (statement order) colors y into r2 like the ROM */
        z = self->mPosZ;
        y = self->mPosY + 0xd0000;
        rider = self->mPlayer;
        x = self->mPosX;
        rider->mPosX = x;
        rider->mPosY = y;
        rider->mPosZ = z;
        if (self->mLoadStep < 3) {
            s16 angleY = self->mAngleY;
            s16 angleX = self->mAngleX;
            rider = self->mPlayer;
            s16 zero = 0;
            rider->mAngleX = angleX;
            rider->mAngleY = angleY;
            rider->mAngleZ = zero;
            angleY = self->mAngleY;
            angleX = self->mAngleX;
            rider = self->mPlayer;
            rider->mPrevAngleX = angleX;
            rider->mPrevAngleY = angleY;
            rider->mPrevAngleZ = zero;
        }
    }

    switch (self->mLoadStep) {
    case 0:
        self->mPosY += 0xa000;
        if (self->mPosY < self->mSpawnPosY)
            break;
        self->mPosY = self->mSpawnPosY;
        self->mLoadStep++;
        self->mTimer = 0;
        quakePos.x = self->mPosX;
        quakePos.y = self->mPosY;
        quakePos.z = self->mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(self, &quakePos, 0x1f4000);
        Sound::PlayBank3(0x14c, *(const Vector3 *)&self->mCamSpacePosX);
        break;

    case 1:
        if (self->mTimer <= 10)
            break;
        if (!ApproachLinear(self->mAngleY, self->mTargetAngleY, 0x200))
            break;
        self->mLoadStep++;
        self->mTimer = 0;
        Sound::PlayBank3(0x14d, *(const Vector3 *)&self->mCamSpacePosX);
        break;

    case 2:
        if (self->mTimer <= 10)
            break;
        if (!ApproachLinear(self->mAngleX, 0x2000, 0x200))
            break;
        if (!func_ov002_020df300(self->mPlayer))
            break;
        Sound::PlayBank3(0x14e, *(const Vector3 *)&self->mCamSpacePosX);
        self->mTimer = 0;
        self->mLoadStep++;
        break;

    case 3:
        self->mAngleY = GetAngleToCamera(self->mPlayer->mPlayerNo) + 0x4000;
        if (!self->mPlayer->IsBeingShotOutOfCannon())
            break;
        self->mLoadStep++;
        self->mTimer = 0;
        break;

    case 4:
    {
        u8 settled = 0;
        self->mPosY -= 0x32000;
        if (ApproachLinear(self->mAngleX, 0, 0x800))
            settled++;
        if (ApproachLinear(self->mAngleY, self->mHomeAngleY, 0x800))
            settled++;
        if (settled != 2)
            break;
        if (self->mPosY > self->mSpawnPosY - 0x190000)
            break;
        self->mState = 2;
        self->mdCcAc_c.flags &= ~1;
        self->mPosY = self->mSpawnPosY - 0x190000;
        self->mPlayer = 0;
        self->mAngleY = self->mHomeAngleY;
        self->mPrevAngleY = self->mAngleY;
        break;
    }
    }

    self->mTimer++;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213a984
/* Place the model and pose the barrel: yaw on the root bone, pitch and the
   recoil slide on the barrel bone. */
extern "C" void func_ov098_0213a984(daCnn_c *self)
{
    u16 pitch = self->mAngleX;
    s16 yaw = self->mAngleY;
    u8 *bones;
    int idx;

    Matrix4x3_FromTranslation(&self->mModel.mat4x3,
        self->mPosX >> 3, self->mPosY >> 3, self->mPosZ >> 3);

    bones = (u8 *)self->mModel.data.bones;
    idx = pitch >> 4;

    *(s16 *)(bones + 0x1c) = yaw;
    *(u16 *)(bones + 0x4e) = pitch;
    *(s32 *)(bones + 0x54) = 0;
    *(s32 *)(bones + 0x58) = self->mBarrelRestY + (self->mRecoil * data_02082214[idx * 2 + 1]) / 4096;
    *(s32 *)(bones + 0x5c) = (self->mRecoil * data_02082214[idx * 2]) / 4096;
}

// @symbol _ZN7daCnn_cD0Ev
// @symbol _ZN7daCnn_cD1Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
