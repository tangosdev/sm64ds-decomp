//cpp
/* Production translation unit for ov081/daSnowball_c.
 * 13 function(s), .text 0x02125f14..0x02126504. The SNOWBALL actor.
 *
 * NAME: _ZTS12daSnowball_c is "12daSnowball_c" at ov081 0x02128a88; _ZTI at
 * 0x02128a7c reads [__si_class_type_info, that string, _ZTI12dEnemyBase_c],
 * and the word before the _ZTV12daSnowball_c address point (0x02128abc)
 * points at that _ZTI. The tree previously called the class Snowball
 * (coined; that spelling is not in the cartridge).
 *
 * This is the whole unit: the destructor pair, the five state and collision
 * helpers that sit between it and the virtuals, the five virtuals, then the
 * registry factory daSnowball_c_classInit (0x021264b4). The
 * out-of-line destructor is the key function, so this TU emits
 * _ZTV/_ZTI/_ZTS12daSnowball_c; the manifest's compiler_only_output rows
 * route those (and the homeless D2) to the cartridge's own copies.
 *
 * decl_common.h is first so common.h's Matrix4x3 wins over math/Matrix.h.
 * Under `#pragma defer_codegen off` .text is laid down in source order, so
 * this file is ROM-ascending.
 *
 * What the object does: a timed hazard. InitResources places it 50 units above
 * where it was spawned and it drops under gravity (clamped at the terminal
 * velocity). On the first frame it is below the spawn height the update helper
 * zeroes its vertical speed and gravity, so it hovers there. It bursts (a
 * particle plus MarkForDestruction, and on two of the four paths the bank-3
 * sound) when its 200-frame mStateTimer expires, when its mesh collision
 * reports ground, or on a contact. Contacts are only handled on frames where
 * one of the x/z velocity words at 0x0a4/0x0ac is non-zero. A contact with the
 * player calls Player::Hurt, plays the sound and bursts; a metal player makes
 * it burst without Hurt; a vanished player is ignored (no burst).
 *
 * What stays:
 * - The file-scope record data_ov081_02128eb4 is what the compiler's
 *   __sinit_d_a_snowball.cpp copies the two pointer-to-member descriptors
 *   into.
 * - func_02012694's sound id 0x3c and the particle id 0x11c are bare numbers;
 *   the SND3_/PTCL_ enum names only say where they are used.
 * - The hit-flag bits (0x10, 0x40000) follow dCc_c's best-effort table and are
 *   named by value here for that reason.
 * - mdCcAc_c's `flags`/`vulnFlags` words (0x200004 / 0x40010) are likewise
 *   given as numbers.
 * - mSpeedX / mSpeedZ are dActor_c's x/z velocity words (as da1up_c's banner
 *   records); they stay unnamed because naming them is a shared header change.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daSnowball_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Particle__System.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

extern SharedFilePtr data_ov081_02128d90;
extern daSnowball_StateRec data_ov081_02128eb4;

/* local extern: no shared header declares these. */
extern "C" {
void func_02012694(int a0, void *a1);
void Vec3_Asr(void *dst, void *src, int n);
}

// @symbol _ZN12daSnowball_cD1Ev
// @symbol _ZN12daSnowball_cD0Ev
/* The key function. D1 stores the vtable, destroys the members in reverse
 * declaration order, then runs dEnemyBase_c::~dEnemyBase_c; D0 does the
 * same and hands the object to dEnemyBase_c's inline operator delete. The
 * base-object D2 mwcc also emits has no home in the cartridge. */
daSnowball_c::~daSnowball_c() {}

enum SnowballBool { SNOWBALL_FALSE, SNOWBALL_TRUE };

/* The one actor id this file compares against: SNOWBALL's own contact handler
 * tests whether the thing that touched it is the player (symbols/actor_debug_names.tsv
 * row 191, 0xbf). */
enum { ACTOR_ID_PLAYER = 191 };

/* Particle::System::NewSimple id started at the snowball's position by every
 * burst below. Only the number is known here. */
enum { PTCL_SNOWBALL_BURST = 0x11c };

/* Sound id handed to func_02012694, the bank-3 wrapper (it calls
 * Sound::Play(3, id, pos)). Played at the camera-space position (mCamSpacePosX)
 * on two of the four burst paths: timer expiry or mesh-collision ground in the
 * update helper, and after Player::Hurt in the contact handler. The two
 * hit-flag / metal-player bursts are silent. Only the number is known here. */
enum { SND3_SNOWBALL_BURST = 0x3c };

/* dCcAc_c::Init flag arguments from InitResources. Only the vulnFlags word
 * (0x40010 = 0x10 | 0x40000) is in the dCc_c bit table's terms, and it is
 * exactly the two bits the contact handler tests; the table's own caveat
 * applies (best-effort names, not pinned by the ROM). The flags word (0x200004)
 * is not described by that table. */
enum {
    SNOWBALL_CC_FLAGS = 0x200004, /* dCc_c::flags word */
    SNOWBALL_CC_VULN = 0x40010    /* dCc_c::vulnFlags word */
};

/* Hit-flag bits tested on mdCcAc_c.hitFlags. The values are the 0x10 and 0x40000
 * rows of the dCc_c bit table ("mega character" and "fire"; best-effort names). */
enum { SNOWBALL_HIT_BIT_0x10 = 0x10, SNOWBALL_HIT_BIT_0x40000 = 0x40000 };

// @symbol _ZN12daSnowball_c13HandleContactEv
/* Contact handler, called from the update helper whenever one of the x/z
 * velocity words at 0x0a4/0x0ac is non-zero. mdCcAc_c.otherOwner holds the
 * unique id of the actor that owns the other cylinder; hitFlags says which hit
 * bits fired. Does nothing when there is no such id or no live actor with it.
 * Otherwise:
 *   - hitFlags has the 0x10 or the 0x40000 bit: burst.
 *   - the other actor is the player: ignore it entirely while the player is
 *     vanished (mIsVanish), burst without hurting while the player is metal
 *     (mIsMetal == 1), else call Player::Hurt, play the burst sound and burst.
 *   - anything else: nothing.
 * "Burst" is a particle at the snowball's position plus MarkForDestruction; only
 * the Player::Hurt path also plays the sound. */
void daSnowball_c::HandleContact() {
    dActor_c *other;
    int flags;
    unsigned int id = mdCcAc_c.otherOwner;
    if (id == 0)
        return;
    other = dActor_c::FindWithID(id);
    if (other == 0)
        return;
    flags = mdCcAc_c.hitFlags;
    if ((flags & SNOWBALL_HIT_BIT_0x10) || (flags & SNOWBALL_HIT_BIT_0x40000)) {
        Particle::System::NewSimple(PTCL_SNOWBALL_BURST, mPosX, mPosY, mPosZ);
        MarkForDestruction();
        return;
    }
    {
        SnowballBool eq = (SnowballBool)(other->actorID == ACTOR_ID_PLAYER);
        if (eq != SNOWBALL_FALSE) {
            Player *player = static_cast<Player *>(other);
            if (player->mIsVanish != 0)
                return;
            if (player->mIsMetal == 1) {
                Particle::System::NewSimple(PTCL_SNOWBALL_BURST, mPosX, mPosY, mPosZ);
                MarkForDestruction();
                return;
            }
            {
                /* Hurt's arguments after the source position are the same tuple
                   (1, 0xc000, 1, 0, 1) daBasabasa_c and Scuttlebug pass; 0xc000
                   is Fix12 12.0. */
                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                player->Hurt(pos, 1, 0xc000, 1, 0, 1);
                func_02012694(SND3_SNOWBALL_BURST, &mCamSpacePosX);
                Particle::System::NewSimple(PTCL_SNOWBALL_BURST, mPosX, mPosY, mPosZ);
                MarkForDestruction();
                return;
            }
        }
    }
}

// @symbol _ZN12daSnowball_c6State0Ev
/* The state record's update function (offset +8 of data_ov081_02128eb4).
 * Behavior calls it once per frame.
 *
 * 1. While mReachedSpawnY is still 0 and mTerminalVelocity still holds the
 *    -0x3c000 (-60 units/frame) InitResources set: the first frame the snowball
 *    is below its spawn height (mSpawnPosY > mPosY) zero mVertSpeed and
 *    mVertAccel and set mReachedSpawnY. The snowball then hovers at (just
 *    below) the height it was placed at instead of falling further.
 * 2. Burst, with the bank-3 sound, when mStateTimer has run out to 0 or the
 *    collision reports it is on the ground. The burst only marks the actor for
 *    destruction; this function carries on to step 3 afterwards.
 * 3. If either x/z velocity word at 0x0a4/0x0ac is non-zero, run the contact handler.
 * Always returns 1. */
int daSnowball_c::State0() {
    if (this->mReachedSpawnY == 0 && this->mTerminalVelocity == -0x3c000) {
        if (this->mSpawnPosY > this->mPosY) {
            this->mVertSpeed = 0;
            this->mVertAccel = 0;
            this->mReachedSpawnY = 1;
        }
    }
    if ((unsigned short)this->mStateTimer == 0 || mWithMeshClsn.IsOnGround() != 0) {
        func_02012694(SND3_SNOWBALL_BURST, &this->mCamSpacePosX);
        Particle::System::NewSimple(PTCL_SNOWBALL_BURST, this->mPosX, this->mPosY, this->mPosZ);
        MarkForDestruction();
    }
    if (this->mSpeedX != 0 || this->mSpeedZ != 0) {
        HandleContact();
    }
    return 1;
}

// @symbol _ZN12daSnowball_c10InitState0Ev
/* The state record's enter function (offset +0 of data_ov081_02128eb4).
 * SetState runs it: clear mReachedSpawnY and arm mStateTimer with 200
 * frames. */
int daSnowball_c::InitState0() {
    this->mReachedSpawnY = 0;
    this->mStateTimer = 200;
    return 1;
}

// @symbol _ZN12daSnowball_c8SetStateEP19daSnowball_StateRec
/* Installs a state record in mStateRec and runs its enter function, if it has one. */
int daSnowball_c::SetState(daSnowball_StateRec *p) {
    mStateRec = p;
    daSnowball_StateRec *q = mStateRec;
    if (q->enter == 0)
        return 1;
    return (this->*(q->enter))();
}

// @symbol _ZN12daSnowball_c17UpdateModelMatrixEv
/* Builds the model's world matrix from position (>> 3 per axis, via Vec3_Asr)
 * and the three angles, through the scratch matrix data_020a0e68, and copies it
 * to mModel.mat4x3. */
void daSnowball_c::UpdateModelMatrix() {
    int v[3];
    Vec3_Asr(v, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN12daSnowball_c16CleanupResourcesEv
/* Releases the one shared file InitResources claimed. Touches no field: the
 * ROM body never reads `this`, and as a method it receives one and ignores
 * it, which measured byte-free. */
int daSnowball_c::CleanupResources() {
    data_ov081_02128d90.Release();
    return 1;
}

// @symbol _ZN12daSnowball_c16OnPendingDestroyEv
/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. */
void daSnowball_c::OnPendingDestroy() {}

// @symbol _ZN12daSnowball_c6RenderEv
/* Draws mModel (the matrix Behavior built into mat4x3). Always returns 1. */
int daSnowball_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN12daSnowball_c8BehaviorEv
/* Per-frame update, always returns 1:
 *   - count mStateTimer down towards 0 (DecIfAbove0_Short stops at 0);
 *   - run the installed state record's update function (State0 for the
 *     record InitResources installs), if it has one;
 *   - mVertSpeed = max(mVertSpeed + mVertAccel, mTerminalVelocity), i.e. fall
 *     under gravity but not faster than the terminal velocity (both negative);
 *   - move by speed, update the mesh collision, copy mPrevAngleY into mAngleY,
 *     rebuild the model matrix;
 *   - clear the cylinder and, if there is a closest player and it is not
 *     vanished (mIsVanish), re-register it with dCcAc_c::Update.
 * What stays: the read and write-back of mSpeedZ around the mVertSpeed store is
 * a no-op in source terms; it stays as written, the shape these bytes were
 * matched with. */
int daSnowball_c::Behavior() {
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    daSnowball_StateRec *rec = mStateRec;
    if (rec->update != 0)
        (this->*(rec->update))();
    int v = mVertSpeed + mVertAccel;
    int hi = mTerminalVelocity;
    if (v >= hi)
        hi = v;
    int tmp = mSpeedZ;
    mVertSpeed = hi;
    mSpeedZ = tmp;
    UpdatePosWithOnlySpeed((dCc_c *)&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngleY = mPrevAngleY;
    UpdateModelMatrix();
    mdCcAc_c.Clear();
    Player *player = ClosestPlayer();
    if (player != 0 && player->mIsVanish == 0)
        mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN12daSnowball_c13InitResourcesEv
/* Loads and binds the model file, then sets up the snowball: gravity of
 * -0x2000 (-2 units/frame^2) with a terminal velocity of -0x3c000 (-60
 * units/frame); a cylinder of radius and height 0x1e000 (30 units); the spawn
 * position saved and the snowball lifted 0x32000 (50 units) above it; a mesh
 * collision of 0x14000 (20 units); and the two-entry state record installed.
 * Returns 0 if the model file could not be bound, else 1. */
int daSnowball_c::InitResources() {
    void *file = dExtSimpleModel_c::LoadFile(data_ov081_02128d90);
    if (mModel.SetFile((BMD_File *)file, 1, -1) == 0)
        return 0;

    mShadowModel.InitCylinder();

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mdCcAc_c.Init(this, 0x1e000, 0x1e000, SNOWBALL_CC_FLAGS, SNOWBALL_CC_VULN);

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mPosY += 0x32000;
    mAngleY = mPrevAngleY;
    mWithMeshClsn.Init(this, 0x14000, 0x14000, 0, 0);

    SetState(&data_ov081_02128eb4);
    return 1;
}

// @symbol daSnowball_c_classInit
extern "C" daSnowball_c *daSnowball_c_classInit() {
    return new daSnowball_c();
}

/* The record InitResources installs. Its two pointer-to-member descriptors
 * are anonymous compiler objects; this definition is what makes mwcc emit
 * __sinit_d_a_snowball.cpp. */
daSnowball_StateRec data_ov081_02128eb4 = {
    &daSnowball_c::InitState0,
    &daSnowball_c::State0,
};
