//cpp
/* daKirai_c: the spike bomb (ov060). Every live bomb registers with the
 * spike-bomb slot table (AddSpikeBomb) and remembers the uniqueIDs of its
 * siblings (actor 0x11c) so that, once they have all gone off, the one
 * nearest the player can re-arm.
 *
 * Behavior dispatches mStateIndex through a table of member pointers at
 * data_ov060_0211b1d8, filled at static-init time from {fn, 0} records in
 * .data: StateArmed, StateSwelling, StateExploding, StateSpent.
 * The registry factory daKirai_c_classInit closes the file.
 */
/* Notes:
 * - data_ov060_0211b1c4 is the model handle (LoadFile / Release).
 * - &mHomePosX stays punned: daKirai_c stores it as three scalars.
 * - The armed state tests IsPlayerActor, an int-returning predicate: a bool
 *   local there misses.
 * - The spent state keeps opt_strength_reduction and opt_common_subs off.
 * - func_02012694 plays sound 0x2f at mCamSpacePos.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daKirai_c_AssetId { ASSET_KOOPA_BOMB_BMD = 0x0382 };

#include "daKirai_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Particle__System.h"
#include "math/MtxStack.h"

/* Per-member opt brackets bind only when codegen is not deferred, and that
 * also lays .text down in source order. The file is therefore ROM-ascending. */
#pragma defer_codegen off

typedef void (daKirai_c::*KiraiState)();

/* The model file handle constructs through func_02017acc and destroys through
 * func_02017ab4; the manifest aliases the wrapper's undefined members onto
 * those ROM symbols. */
struct KiraiModelFilePtr : SharedFilePtr {
    KiraiModelFilePtr(u32 fileID);
    ~KiraiModelFilePtr();
};

/* arm9 C helpers with no shared header. */
extern "C" {
extern void ClearSpikeBomb(int idx);
extern int AddSpikeBomb(void *p);
extern int Vec3_HorzLen(const Vector3 *v);
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern void func_02012694(int a, void *b);
}

extern KiraiModelFilePtr data_ov060_0211b1c4;
extern KiraiState data_ov060_0211b1d8[4];

/* mFlags 0x08 is the framework's off-screen bit (dActor_c.h bit table). */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

/* Actor 0xbf is the player. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

// @symbol _ZN9daKirai_cD1Ev
// @symbol _ZN9daKirai_cD0Ev
daKirai_c::~daKirai_c() {}

/* Disarm: give up the spike-bomb slot, stop colliding, and remember every
   other bomb still in the level. */
// @symbol _ZN9daKirai_c6DisarmEv
void daKirai_c::Disarm() {
    int i;
    int j;
    unsigned int id;
    dActor_c *a;

    ClearSpikeBomb(mSlotIndex);
    mdCcAcPos_c.flags |= 1;
    mStateIndex = 3;
    a = 0;
    for (i = 0; i < 8; i++)
        mOtherBombIDs[i] = 0;
    j = 0;
    id = 0x11c;
    while (1) {
        a = dActor_c::FindWithActorID(id, a);
        if (a == 0)
            break;
        if (a != this) {
            mOtherBombIDs[j] = a->uniqueID;
            j++;
            if (j == 8)
                break;
        }
    }
}

/* Is `pos` close enough to set this bomb off? */
// @symbol _ZN9daKirai_c16IsInTriggerRangeEP7Vector3
int daKirai_c::IsInTriggerRange(Vector3 *pos) {
    int horz, homeHorz;
    if (mStateIndex != 0)
        return 0;
    horz = Vec3_HorzLen(pos);
    homeHorz = mHomeHorzDist;
    if (horz >= homeHorz - 0x12c000 && horz <= homeHorz + 0x12c000) {
        if (Vec3_Dist((Vector3 *)&mHomePosX, pos) < mHomeYOffset)
            return 1;
    }
    return 0;
}

/* Explode. */
// @symbol _ZN9daKirai_c7ExplodeEv
void daKirai_c::Explode() {
    Vector3 v;
    mStateIndex = 1;
    Particle::System::NewSimple(0xa8, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0xa9, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0xaa, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0xab, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0xac, mPosX, mPosY, mPosZ);
    func_02012694(0x2f, &mCamSpacePos);
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    Earthquake(v, 0x7d0000);
    mTimer = 0;
    Disarm();
}

/* Place the model and fade it. */
// @symbol _ZN9daKirai_c20UpdateModelTransformEv
void daKirai_c::UpdateModelTransform() {
    Matrix4x3_FromTranslation(&mModel.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    mModel.ApplyOpacity((u8)(mOpacity >> 3), 1);
}

/* Re-arm. */
// @symbol _ZN9daKirai_c5RearmEv
void daKirai_c::Rearm() {
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mOpacity = 0xff;
    mStateIndex = 0;
    mdCcAcPos_c.flags &= ~1;
    mTimer = 0;
    mSlotIndex = AddSpikeBomb(this);
}

/* Spent: wait until every sibling has gone off, then re-arm whichever bomb
   (this one or a sibling) is nearest the player. */
#pragma opt_strength_reduction off
#pragma opt_common_subs off
// @symbol _ZN9daKirai_c10StateSpentEv
void daKirai_c::StateSpent() {
    Vector3 v;
    Player *player;
    daKirai_c *bestActor;
    daKirai_c *actor;
    int best;
    int i;

    player = ClosestPlayer();
    if (!IsOffScreen(this))
        return;
    if (player == 0)
        return;

    {
        Vector3 *pp = &player->mPos;
        v.x = pp->x;
        v.y = pp->y;
        v.z = pp->z;
    }
    best = Vec3_Dist(&mPos, &v);
    bestActor = this;

    for (i = 0; i < 8; i++) {
        int id = mOtherBombIDs[i];
        if (id == 0)
            continue;
        actor = (daKirai_c *)dActor_c::FindWithID(id);
        if (actor != 0) {
            if (actor->mStateIndex != 3)
                return;
            if (!IsOffScreen(actor))
                continue;
            {
                int d = Vec3_Dist(&v, &actor->mPos);
                if (d < best) {
                    best = d;
                    bestActor = actor;
                }
            }
        } else {
            mOtherBombIDs[i] = 0;
        }
    }
    bestActor->Rearm();
}
#pragma opt_common_subs on
#pragma opt_strength_reduction on

/* Exploding: swell, fade and rise for 0x1c frames, then disarm. */
// @symbol _ZN9daKirai_c14StateExplodingEv
void daKirai_c::StateExploding() {
    int scale = (mTimer * 9 << 12) / 14 + 0x1000;
    mScaleX = scale;
    mScaleY = scale;
    mScaleZ = scale;
    mOpacity -= 0xa;
    if (mOpacity < 0xa)
        mOpacity = 0;
    mPosY += mVertSpeed;
    if (mTimer == 0x1c)
        Disarm();
    mTimer++;
}

/* Swell for 0x1c frames without colliding, then disarm. */
// @symbol _ZN9daKirai_c13StateSwellingEv
void daKirai_c::StateSwelling() {
    int scale;
    mdCcAcPos_c.flags |= 1;
    scale = (mTimer * 9 << 12) / 14 + 0x1000;
    mScaleX = scale;
    mScaleY = scale;
    mScaleZ = scale;
    if (mTimer == 0x1c)
        Disarm();
    mTimer++;
}

/* Armed: if the thing that touched us is a player (actor 0xbf), blow up in
   its face. */
// @symbol _ZN9daKirai_c10StateArmedEv
void daKirai_c::StateArmed() {
    dActor_c *a;
    Vector3 quakePos, hurtPos;
    unsigned int id;
    id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return;
    a = dActor_c::FindWithID(id);
    if (a == 0)
        return;
    if (!IsPlayerActor(a))
        return;
    Particle::System::NewSimple(0xa8, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0xa9, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0xaa, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0xab, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0xac, mPosX, mPosY, mPosZ);
    func_02012694(0x2f, &mCamSpacePos);
    quakePos.x = mPosX;
    quakePos.y = mPosY;
    quakePos.z = mPosZ;
    Earthquake(quakePos, 0x7d0000);
    hurtPos.x = mPosX;
    hurtPos.y = mPosY;
    hurtPos.z = mPosZ;
    ((Player *)a)->Hurt(hurtPos, 2, 0xc000, 1, 0, 1);
    Disarm();
}

// @symbol _ZN9daKirai_c16CleanupResourcesEv
int daKirai_c::CleanupResources() {
    data_ov060_0211b1c4.Release();
    return 1;
}

// @symbol _ZN9daKirai_c6RenderEv
int daKirai_c::Render() {
    if (mStateIndex != 0)
        return 1;
    if (mOpacity < 8)
        return 1;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9daKirai_c8BehaviorEv
int daKirai_c::Behavior() {
    (this->*data_ov060_0211b1d8[mStateIndex])();
    UpdateModelTransform();
    mdCcAcPos_c.Clear();
    Vector3 v;
    v.x = 0;
    v.y = -0x96000;
    v.z = 0;
    mdCcAcPos_c.SetPosRelativeToActor(v);
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN9daKirai_c13InitResourcesEv
int daKirai_c::InitResources() {
    Vector3 v;
    Vector3 z;

    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov060_0211b1c4), 1, -1);
    v.x = 0;
    v.y = -0x96000;
    v.z = 0;
    mdCcAcPos_c.Init(this, v, 0x96000, 0x12c000, 0x204004, 0);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mOpacity = 0xff;
    z.x = 0;
    z.y = 0;
    z.z = 0;
    Vec3_HorzAngle(&z, &mPos);
    mHomeYOffset = 0x2ee000;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomePosY += mHomeYOffset >> 3;
    mHomeHorzDist = Vec3_HorzLen(&mPos);
    mStateIndex = 0;
    mSlotIndex = AddSpikeBomb(this);
    return 1;
}

/* The model handle and the state table are this TU's static-init globals:
 * mwcc emits the initializer that constructs the handle (file 0x382),
 * registers its destructor, and copies the four pointer-to-member records. */
KiraiModelFilePtr data_ov060_0211b1c4(ASSET_KOOPA_BOMB_BMD);

KiraiState data_ov060_0211b1d8[4] = {
    &daKirai_c::StateArmed,
    &daKirai_c::StateSwelling,
    &daKirai_c::StateExploding,
    &daKirai_c::StateSpent,
};

/* `new daKirai_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x1b0), dActor_c's base constructor, the vptr store,
 * then the dExtSimpleModel_c and dCcAcPos_c member constructors. */
// @symbol daKirai_c_classInit
extern "C" daKirai_c *daKirai_c_classInit(void) {
    return new daKirai_c;
}
