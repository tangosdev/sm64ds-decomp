//cpp
/* daKirai_c: the spike bomb (ov060). Every live bomb registers with the
 * spike-bomb slot table (AddSpikeBomb) and remembers the uniqueIDs of its
 * siblings (actor 0x11c) so that, once they have all gone off, the one
 * nearest the player can re-arm.
 *
 * Behavior dispatches mStateIndex through a table of member pointers at
 * data_ov060_0211b1d8, filled at static-init time from {fn, 0} records in
 * .data: armed (func_ov060_02118970), swelling (func_ov060_021188e8),
 * exploding (func_ov060_02118834), spent (func_ov060_02118728).
 * The registry factory daKirai_c_classInit closes the file.
 */

#include "daKirai_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* Per-member opt brackets bind only when codegen is not deferred, and that
 * also lays .text down in source order. The file is therefore ROM-ascending. */
#pragma defer_codegen off

typedef void (daKirai_c::*KiraiState)();

/* The model file handle constructs through func_02017acc and destroys through
 * func_02017ab4; the manifest aliases the wrapper's undefined members onto
 * those ROM symbols. */
struct KiraiModelFilePtr : SharedFilePtr {
    u32 words[2];

    KiraiModelFilePtr(u32 fileID);
    ~KiraiModelFilePtr();
};

extern "C" {
extern void ClearSpikeBomb(int idx);
extern int AddSpikeBomb(void *p);
extern int Vec3_HorzLen(const Vector3 *v);
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void func_02012694(int a, void *b);
/* local extern: Particle::System::NewSimple, dActor_c::Earthquake and
   Player::Hurt take Fix12<int> by value and none of them is declared in
   include/; the sibling promoted TUs (daDkk_c, daDgr_c) keep the same
   mangled spelling for the same reason. */
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, Vector3 *v, int f);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, Vector3 *v, unsigned int b, int c, unsigned int d, unsigned int e, unsigned int f);
/* local extern: dCcAcPos_c::Init also takes its radius and height as
   Fix12<int> by value; spelt through the header, the two aggregate
   temporaries grow the frame by 8 and InitResources by 0x10. */
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 &v, int radius, int height, unsigned flags, unsigned vulnFlags);
extern KiraiModelFilePtr data_ov060_0211b1c4;
extern KiraiState data_ov060_0211b1d8[4];
}

// @symbol _ZN9daKirai_cD1Ev
// @symbol _ZN9daKirai_cD0Ev
daKirai_c::~daKirai_c()
{
}

/* Disarm: give up the spike-bomb slot, stop colliding, and remember every
   other bomb still in the level. */
// @symbol _ZN9daKirai_c19func_ov060_021184bcEv
void daKirai_c::func_ov060_021184bc()
{
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
// @symbol _ZN9daKirai_c19func_ov060_02118544EP7Vector3
int daKirai_c::func_ov060_02118544(Vector3 *pos)
{
    int horz, homeHorz;
    if (mStateIndex != 0) return 0;
    horz = Vec3_HorzLen(pos);
    homeHorz = mHomeHorzDist;
    if (horz >= homeHorz - 0x12c000 && horz <= homeHorz + 0x12c000) {
        if (Vec3_Dist((Vector3 *)&mHomePosX, pos) < mHomeYOffset) return 1;
    }
    return 0;
}

/* Explode. */
// @symbol _ZN9daKirai_c19func_ov060_021185c4Ev
void daKirai_c::func_ov060_021185c4()
{
    Vector3 v;
    mStateIndex = 1;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa8, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa9, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xaa, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xab, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xac, mPosX, mPosY, mPosZ);
    func_02012694(0x2f, &mCamSpacePosX);
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &v, 0x7d0000);
    mTimer = 0;
    func_ov060_021184bc();
}

/* Place the model and fade it. */
// @symbol _ZN9daKirai_c19func_ov060_02118690Ev
void daKirai_c::func_ov060_02118690()
{
    Matrix4x3_FromTranslation(&mModel.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    mModel.ApplyOpacity((u8)(mOpacity >> 3), 1);
}

/* Re-arm. */
// @symbol _ZN9daKirai_c19func_ov060_021186d8Ev
void daKirai_c::func_ov060_021186d8()
{
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
// @symbol _ZN9daKirai_c19func_ov060_02118728Ev
void daKirai_c::func_ov060_02118728()
{
    Vector3 v;
    Player *player;
    daKirai_c *bestActor;
    daKirai_c *actor;
    int best;
    int i;

    player = ClosestPlayer();
    if (((mFlags & 8) ? 1 : 0) == 0) return;
    if (player == 0) return;

    {
        Vector3 *pp = (Vector3 *)&player->mPosX;
        v.x = pp->x;
        v.y = pp->y;
        v.z = pp->z;
    }
    best = Vec3_Dist((Vector3 *)&mPosX, &v);
    bestActor = this;

    for (i = 0; i < 8; i++) {
        int id = mOtherBombIDs[i];
        if (id == 0) continue;
        actor = (daKirai_c *)dActor_c::FindWithID(id);
        if (actor != 0) {
            if (actor->mStateIndex != 3) return;
            if (((actor->mFlags & 8) ? 1 : 0) == 0) continue;
            {
                int d = Vec3_Dist(&v, (Vector3 *)&actor->mPosX);
                if (d < best) {
                    best = d;
                    bestActor = actor;
                }
            }
        } else {
            mOtherBombIDs[i] = 0;
        }
    }
    bestActor->func_ov060_021186d8();
}
#pragma opt_common_subs on
#pragma opt_strength_reduction on

/* Exploding: swell, fade and rise for 0x1c frames, then disarm. */
// @symbol _ZN9daKirai_c19func_ov060_02118834Ev
void daKirai_c::func_ov060_02118834()
{
    int scale = (mTimer * 9 << 12) / 14 + 0x1000;
    mScaleX = scale;
    mScaleY = scale;
    mScaleZ = scale;
    mOpacity -= 0xa;
    if (mOpacity < 0xa) mOpacity = 0;
    mPosY += mVertSpeed;
    if (mTimer == 0x1c) func_ov060_021184bc();
    mTimer++;
}

/* Swell for 0x1c frames without colliding, then disarm. */
// @symbol _ZN9daKirai_c19func_ov060_021188e8Ev
void daKirai_c::func_ov060_021188e8()
{
    int scale;
    mdCcAcPos_c.flags |= 1;
    scale = (mTimer * 9 << 12) / 14 + 0x1000;
    mScaleX = scale;
    mScaleY = scale;
    mScaleZ = scale;
    if (mTimer == 0x1c)
        func_ov060_021184bc();
    mTimer++;
}

/* Armed: if the thing that touched us is a player (actor 0xbf), blow up in
   its face. */
// @symbol _ZN9daKirai_c19func_ov060_02118970Ev
void daKirai_c::func_ov060_02118970()
{
    dActor_c *a;
    Vector3 quakePos, hurtPos;
    int isPlayer; /* int, not bool: measured, a bool local misses */
    unsigned int id;
    id = mdCcAcPos_c.otherOwner;
    if (id == 0) return;
    a = dActor_c::FindWithID(id);
    if (a == 0) return;
    isPlayer = (a->actorID == 0xbf);
    if (!isPlayer) return;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa8, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa9, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xaa, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xab, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xac, mPosX, mPosY, mPosZ);
    func_02012694(0x2f, &mCamSpacePosX);
    quakePos.x = mPosX;
    quakePos.y = mPosY;
    quakePos.z = mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &quakePos, 0x7d0000);
    hurtPos.x = mPosX;
    hurtPos.y = mPosY;
    hurtPos.z = mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &hurtPos, 2, 0xc000, 1, 0, 1);
    func_ov060_021184bc();
}

// @symbol _ZN9daKirai_c16CleanupResourcesEv
int daKirai_c::CleanupResources()
{
    data_ov060_0211b1c4.Release();
    return 1;
}

// @symbol _ZN9daKirai_c6RenderEv
int daKirai_c::Render()
{
    if (mStateIndex != 0) return 1;
    if (mOpacity < 8) return 1;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9daKirai_c8BehaviorEv
int daKirai_c::Behavior()
{
    (this->*data_ov060_0211b1d8[mStateIndex])();
    func_ov060_02118690();
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
int daKirai_c::InitResources()
{
    Vector3 v;
    Vector3 z;

    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov060_0211b1c4), 1, -1);
    v.x = 0;
    v.y = -0x96000;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, v, 0x96000, 0x12c000, 0x204004, 0);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mOpacity = 0xff;
    z.x = 0;
    z.y = 0;
    z.z = 0;
    Vec3_HorzAngle(&z, (const Vector3 *)&mPosX);
    mHomeYOffset = 0x2ee000;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomePosY += mHomeYOffset >> 3;
    mHomeHorzDist = Vec3_HorzLen((const Vector3 *)&mPosX);
    mStateIndex = 0;
    mSlotIndex = AddSpikeBomb(this);
    return 1;
}

/* The model handle and the state table are this TU's static-init globals:
 * mwcc emits the initializer that constructs the handle (file 0x382),
 * registers its destructor, and copies the four pointer-to-member records. */
KiraiModelFilePtr data_ov060_0211b1c4(0x382);

KiraiState data_ov060_0211b1d8[4] = {
    &daKirai_c::func_ov060_02118970,
    &daKirai_c::func_ov060_021188e8,
    &daKirai_c::func_ov060_02118834,
    &daKirai_c::func_ov060_02118728,
};

/* recovered: vtable identified, globals resolved */
/* Reconstructed source-style name: SM64DS proves daKirai_c through RTTI,
 * allocation size, vtable identity, and the KIRAI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: SpikeBomb_Spawn.
 *
 * `new daKirai_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x1b0), dActor_c's base constructor, the vptr store,
 * then the Model and dCcAcPos_c member constructors. */
// @symbol daKirai_c_classInit
extern "C" daKirai_c *daKirai_c_classInit(void)
{
    return new daKirai_c;
}
