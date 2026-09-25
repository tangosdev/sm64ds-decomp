//cpp
/* Production translation unit for ov016/daObjKi_Hasira_c, hand-curated.
 * 7 function(s), .text 0x02112a00..0x02112ec4.
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
 * text in this .cpp for an @symbol marker to sit above.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled in
 *   this TU -- both take Fix12<int> by value (wall 6az); a member call homes
 *   the argument and size-DIFFs InitResources / Behavior.
 * Leftover: func_ov016_02112a9c is still the linker name of this class's
 *   three-axis model-matrix update (dBgActor_c::UpdateModelPosAndRotY's
 *   pitch-and-roll sibling). It stays a C-linkage helper under that name;
 *   giving it a member spelling would coin a mangled name.
 * Leftover: the model and collision SharedFilePtrs are ov016 .bss rows still
 *   named RockPillar_ModelFile / RockPillar_ClsnFile in symbols.txt; they are
 *   data names, not the class name, and renaming them is a separate change.
 * Leftover: data_ov016_02113cac, the CLPS block handed to SetFile, is an
 *   unnamed ov016 .data row this TU does not own.
 * Leftover: the factory daObjKi_Hasira_c_classInit (0x02112ec4) sits just
 *   past this run's right edge and stays a one-function source; tu_map.py
 *   ends this candidate at 0x02112ec4.
 */

#include "daObjKi_Hasira_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"

typedef long long s64;

struct Camera;

extern "C" {
/* This class's model and collision files (ov016 .bss) and the CLPS block
 * dBgW_KcMbg::SetFile is handed (ov016 .data). */
extern SharedFilePtr RockPillar_ModelFile;
extern SharedFilePtr RockPillar_ClsnFile;
extern char data_ov016_02113cac[];

void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, s16 angleY, void *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);

void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
u8 DecIfAbove0_Byte(u8 *p);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
/* The camera-shake request and the camera it goes to (arm9). */
void func_0200d8c8(Camera *cam, const Vector3 *pos, int strength);
extern void *data_0209f318;
/* The sin/cos table: (sin, cos) pairs of s16 indexed by angle >> 4. */
extern s16 data_02082214[];

void func_ov016_02112a9c(daObjKi_Hasira_c *self);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjKi_Hasira_c13InitResourcesEv
s32 daObjKi_Hasira_c::InitResources()
{
    void *f = Model::LoadFile(RockPillar_ModelFile);
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov016_02112a9c(this);
    UpdateClsnPosAndRot();

    char *k = dBgW_Kc::LoadFile(RockPillar_ClsnFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x199, mAngleY, data_ov016_02113cac);

    Spawn(0x3b, 0, *(Vector3 *)&mPosX, 0, mAreaId, -1);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjKi_Hasira_c8BehaviorEv
s32 daObjKi_Hasira_c::Behavior()
{
    Vector3 targetPos;
    /* volatile on purpose: the cartridge stores each crash coordinate as soon
       as it is known, then stores x, y and z again once the offsets are in,
       and reads them back for dustPos. Without it the early stores fold away. */
    volatile Vector3 crashPos;
    Vector3 dustPos;

    /* Nothing happens while the pillar is off screen (mFlags 0x08). Written
       as a flag first: tested inline, the branch comes out a different shape. */
    int offScreen = (mFlags & 8) != 0;
    if (offScreen)
        return 1;

    Player *target = mTarget;
    if (target != 0) {
        s32 *p = &target->mPosX;
        targetPos.x = p[0];
        targetPos.y = p[1];
        targetPos.z = p[2];
    }

    switch (mState) {
    case 0: /* wait for a player to come within 1300 units */
        mTarget = ClosestPlayer();
        if (mTarget != 0) {
            if (Vec3_Dist((Vector3 *)&mPosX, &targetPos) < 0x514000) {
                mState++;
                mTurnTimer = 10;
                Sound::PlayBank3(0x56, *(Vector3 *)&mCamSpacePosX);
            }
        }
        break;

    case 1: /* turn to face them for ten frames */
        if (mTarget != 0) {
            s16 angle = Vec3_HorzAngle((Vector3 *)&mPosX, &targetPos);
            _Z14ApproachLinearRsss(&mAngleY, angle, 0x400);
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
            int x = mPosX;
            s16 *sinCos = data_02082214;
            crashPos.x = x;
            int y = mPosY;
            crashPos.y = y;
            int z = mPosZ;
            int reach = 0x384000;
            crashPos.z = z;
            u16 yaw = mAngleY;
            int round = 0x800;
            y = y + 0x96000;
            s16 sin = sinCos[(yaw >> 4) << 1];
            unsigned particleID = 0x107;
            x = x + (int)(((s64)sin * (s64)reach + round) >> 12);
            crashPos.x = x;
            yaw = mAngleY;
            s16 cos = sinCos[((yaw >> 4) << 1) + 1];
            z = z + (int)(((s64)cos * (s64)reach + round) >> 12);
            crashPos.y = y;
            crashPos.z = z;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(particleID, x, y, z);
            dustPos.x = crashPos.x; dustPos.y = crashPos.y; dustPos.z = crashPos.z;
            LandingDustAt(dustPos, true);
            func_0200d8c8((Camera *)data_0209f318, (Vector3 *)&mPosX, 0x5dc000);
            Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
            MarkForDestruction();
        }
        break;
    }
    }

    func_ov016_02112a9c(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjKi_Hasira_c6RenderEv
s32 daObjKi_Hasira_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjKi_Hasira_c16CleanupResourcesEv
s32 daObjKi_Hasira_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    RockPillar_ModelFile.Release();
    RockPillar_ClsnFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov016_02112a9c
/* dBgActor_c::UpdateModelPosAndRotY with pitch and roll as well: rebuild the
 * model matrix from all three angles, then drop the position into its
 * translation row at 1/8 scale, the model-space unit. */
extern "C" void func_ov016_02112a9c(daObjKi_Hasira_c *self)
{
    Matrix4x3_FromRotationXYZExt(&self->mModel.mat4x3, self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModel.mat4x3.m[9]  = self->mPosX >> 3;
    self->mModel.mat4x3.m[10] = self->mPosY >> 3;
    self->mModel.mat4x3.m[11] = self->mPosZ >> 3;
}
