//cpp
/* Dire, Dire Docks whirlpool (WATER_TATUMAKI). A player who comes within
 * 300 of the centre is caught and swung down the funnel; once they pass the
 * centre, KillPlayer runs once. Anyone merely inside 1100 is pulled inward,
 * unless they are metal. The model is water_tatumaki.bmd (handle 0x4a9) and
 * the joint animation is water_tatumaki.bca (handle 0x4a8). The texture
 * animation is the overlay BTA at data_ov026_02112f40, not a NitroFS file.
 * The factory stays in src/d_a_water_tatumaki.c. No g_profile in this TU.
 *
 * Distances below are 20.12 (1.0 == 0x1000). Angles are s16, full circle
 * 0x10000.
 *
 * deslop leftovers:
 * - func_ov026_02111b24: ApproachLinear on mPullPos with a Fix12<int>
 *   temporary is 0x198 against ROM 0x190 and adds a 4-byte .rodata word.
 *   The int spelling of the mangled symbol keeps the step in a register.
 * - func_ov026_02111b24: without volatile, the spilled player position is
 *   0x178 against ROM 0x190. The ROM stores the three words and compares
 *   the y still in the register.
 * - func_ov026_02111b24, func_ov026_02111cb4, func_ov026_02111d4c,
 *   func_ov026_02111f30: a TU-local reference to mPosX grows those
 *   functions by 4 bytes (0x190, 0x98, 0x18c, 0x74 become 0x194, 0x9c,
 *   0x190, 0x78). dActor_c has three position scalars and no Pos().
 * - func_ov026_02111cb4: deleting the stores of 0 to mPullHeight and
 *   mPullRadius is 0x90 against ROM 0x98. Deleting unk_190 = 0 is 0x94
 *   against ROM 0x98. The ROM zeroes both heights and then overwrites
 *   them; unk_190 is stored 0 and never read in this TU.
 * - func_ov026_02111f30: mModelAnim.mat4x3 = data_020a0e68 is 0x90 against
 *   ROM 0x74. This TU sees Matrix4x3 as {Matrix3x3 r; Vector3 t}. The ROM
 *   copies twelve words with one three-pass ldm/stm.
 * - InitResources: TextureTransformer::SetFile with a Fix12<int> temporary
 *   is 0xd8 against ROM 0xd0 and adds a 4-byte .rodata word.
 *   ModelAnim::SetAnim the same way is 0xdc against ROM 0xd0, also with
 *   a 4-byte .rodata word. The int spellings of the mangled symbols match.
 * - Behavior: Particle::System::New(mParticleID, 0x139, mPosX,
 *   mPosY + 0x384000, mPosZ, 0, 0) is 0xc8 against ROM 0xe0. The ROM
 *   spills the position, adds the lift into the spill, and reloads x and
 *   the raised y. include/Particle__System.h does not declare New.
 * - func_ov026_02111ed8: daWater_Tatumaki_c::SpinEnter leaves
 *   func_ov026_02111ed8 missing from the object. The five state bodies
 *   are the ROM labels the state records point at, so they stay extern "C".
 * - Without #pragma defer_codegen off the sections come out in reverse
 *   source order (InitResources first, D1 last). The pragma is what puts
 *   this TU in ROM order. D2 still has no cartridge home.
 */

#pragma defer_codegen off

#include "daWater_Tatumaki_c.h"
#include "dActor_c.h"
#include "common.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "TextureTransformer.h"
#include "private/mtx43.h"

/* 20.12 distances and s16 angles used by the funnel. Same numeric value can
   be a fix12 step and an angle; the names stay with the call that uses them. */
enum {
    kAnimSpeed = 0x1000,
    kSpinTarget = 0x1000,
    kSpinStep = 0x200,
    kTightRadius = 0x100,
    kRadiusStep = 0x1000,
    kFunnelDepth = 0x64000,   /* 100.0 */
    kSinkStep = 0x4000,       /* 4.0 */
    kChaseStep = 0x1e000,     /* 30.0 */
    kHalfTurn = 0x8000,
    kLookDown = -0x2000,
    kPullTilt = 0x2000,
    kCaptureReach = 0x12c000, /* 300.0 */
    kDrawReach = 0x44c000,    /* 1100.0 */
    kDrawDivisor = 35,
    kSprayLift = 0x384000,    /* 900.0 above the actor */
    kSprayEffect = 0x139
};

/* The two SharedFilePtr globals, the BTA, and the two State records.
   Spelt the way __sinit_ov026_02112c94.c and include/decl_common.h declare
   them, so every declaration of each symbol agrees. The handles are read
   through SharedFileBytes: the constructor stores fileID at +0, refCount at
   +2 and the loaded file at +4 (func_02017e0c). SharedFilePtr.h has no
   fields, on purpose, so this view stays in the TU. */
struct SharedFileBytes {
    u16 fileID;
    u8 refCount;
    u8 pad;
    void *file;
};

extern int data_ov026_02113f0c[];   /* water_tatumaki.bmd, handle 0x4a9 */
extern int data_ov026_02113f04[];   /* water_tatumaki.bca, handle 0x4a8 */
extern BTA_File data_ov026_02112f40;
extern int data_ov026_02113f2c;     /* State: spin, waiting to catch */
extern void *data_ov026_02113f3c;   /* State: drag the player down */

#define MODEL_FILE (*(SharedFileBytes *)data_ov026_02113f0c)
#define ANIM_FILE  (*(SharedFileBytes *)data_ov026_02113f04)
#define AS_SHARED(file) (*(SharedFilePtr *)&(file))
#define WHIRLPOOL_BTA data_ov026_02112f40

/* sinit copies these two PMF pairs out of .data. Spin enter is
   func_ov026_02111ed8 and its execute is func_ov026_02111d4c. Drag enter is
   func_ov026_02111cb4 and its execute is func_ov026_02111b24. The PMF's
   second word is 0, so each one is an ordinary non-virtual function. */
#define STATE_SPIN     ((const daWater_Tatumaki_c::State *)&data_ov026_02113f2c)
#define STATE_DRAG     ((const daWater_Tatumaki_c::State *)&data_ov026_02113f3c)

extern "C" {
extern void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, short angX);
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, short rx, short ry, short rz);
extern void MulVec3Mat4x3(Vector3 *in, Matrix4x3 *m, Vector3 *out);
extern void Vec3_Asr(void *dst, void *src, int n);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void KillPlayer(void);
/* Scratch matrix. The helpers below write it; the model copy reads it back. */
extern Matrix4x3 data_020a0e68;
/* Fix12<int> by value. The int parameter is the matching call. */
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int uniqueID, unsigned int effectID, int x, int y, int z, void *dir, void *callback);
extern int _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 &value, const Vector3 &target, int step);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(TextureTransformer *self, BTA_File &file, int flags, int speed, u32 startFrame);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *self, BCA_File *file, int flags, int speed, u32 startFrame);
}

#define Particle_System_New _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE
#define ApproachLinearVec _Z14ApproachLinearR7Vector3RKS_5Fix12IiE
#define TextureTransformer_SetFile _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj
#define ModelAnim_SetAnim _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj

void ApproachLinear2(s16 &value, s16 target, s16 step);
void ApproachLinear(int &value, int target, int step);

extern "C" int func_ov026_02111ee0(daWater_Tatumaki_c *self, const daWater_Tatumaki_c::State *state);

// @symbol _ZN18daWater_Tatumaki_cD1Ev
daWater_Tatumaki_c::~daWater_Tatumaki_c()
{
}

// @symbol _ZN18daWater_Tatumaki_cD0Ev
/* D0 is emitted from the destructor above. #pragma defer_codegen off puts
   D1 then D0, which is the cartridge order. The trailing D2 has no ROM home. */

// @symbol func_ov026_02111b24
/* Drag, each frame: orbit the player around the funnel and sink them. Below
   the centre, kill them once. */
extern "C" int func_ov026_02111b24(daWater_Tatumaki_c *self)
{
    Vector3 target;
    Vector3 offset;
    volatile s32 spilled[3]; /* stored, never read; the compare uses y in r1 */
    Player *player;

    target.x = 0;
    target.y = 0;
    target.z = 0;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    target.z = self->mPullRadius;
    player = self->ClosestPlayer();
    if (player) {
        Matrix4x3_FromRotationY(&data_020a0e68, self->mPullAngle);
        MulVec3Mat4x3(&target, &data_020a0e68, &offset);
        target.x = self->mCenter.x;
        target.y = self->mCenter.y;
        target.z = self->mCenter.z;
        ApproachLinear2(self->mPullSpin, kSpinTarget, kSpinStep);
        ApproachLinear(self->mPullRadius, kTightRadius, kRadiusStep);
        ApproachLinear(self->mPullHeight, self->mCenter.y - kFunnelDepth, kSinkStep);
        self->mPullAngle += self->mPullSpin;
        target.y = self->mPullHeight;
        target.x += offset.x;
        target.z += offset.z;
        ApproachLinearVec(self->mPullPos, target, kChaseStep);
        player->mPosX = self->mPullPos.x;
        player->mPosY = self->mPullPos.y;
        player->mPosZ = self->mPullPos.z;
        {
            int ang = self->HorzAngleToCPlayer() + kHalfTurn;
            player->mAngleX = kLookDown;
            player->mAngleY = ang;
            player->mAngleZ = 0;
        }
        {
            const Vector3 *playerPos = (const Vector3 *)&player->mPosX;
            spilled[0] = playerPos->x;
            spilled[1] = playerPos->y;
            spilled[2] = playerPos->z;
            if (self->mCenter.y > playerPos->y) {
                if (self->mPlayerKilled == 0) {
                    KillPlayer();
                    self->mPlayerKilled = 1;
                }
            }
        }
    }
    return 1;
}

// @symbol func_ov026_02111cb4
/* Drag, on entry: start the orbit where the player is standing. */
extern "C" int func_ov026_02111cb4(daWater_Tatumaki_c *self)
{
    Player *player = self->ClosestPlayer();
    if (player) {
        const Vector3 *playerPos = (const Vector3 *)&player->mPosX;
        Vector3 pos;
        pos.x = playerPos->x;
        pos.y = playerPos->y;
        pos.z = playerPos->z;
        self->mPullAngle = Vec3_HorzAngle((Vector3 *)&self->mPosX, &pos);
        self->unk_190 = 0;
        self->mPullHeight = 0;
        self->mPullRadius = 0;
        self->mPullPos.x = pos.x;
        self->mPullPos.y = pos.y;
        self->mPullPos.z = pos.z;
        self->mPullHeight = pos.y;
        self->mPullRadius = Vec3_HorzDist(&self->mCenter, &pos);
    }
    return 1;
}

// @symbol func_ov026_02111d4c
/* Spin, each frame: catch a player who has reached the centre, otherwise
   draw a non-metal player inward. The pull is stronger when they are closer. */
extern "C" int func_ov026_02111d4c(daWater_Tatumaki_c *self)
{
    Player *player = self->ClosestPlayer();
    if (player != 0) {
        Vector3 pos;
        {
            const Vector3 *playerPos = (const Vector3 *)&player->mPosX;
            pos = *playerPos;
        }

        if (Vec3_HorzDist(&self->mCenter, &pos) <= kCaptureReach) {
            int dy = self->mCenter.y - pos.y;
            if (dy < 0) dy = -dy;
            if (dy <= kCaptureReach) {
                player->EnterWhirlpool();
                func_ov026_02111ee0(self, STATE_DRAG);
                return 1;
            }
        }

        if (player->mIsMetal == 0) {
            int dist = Vec3_Dist(&self->mCenter, &pos);
            if (dist < kDrawReach) {
                Vector3 pull;
                Vector3 out;
                int speed;
                speed = kDrawReach;
                speed -= dist;
                speed = speed / kDrawDivisor;
                pull.x = 0;
                pull.y = 0;
                pull.z = speed;
                out.x = 0;
                out.y = 0;
                out.z = 0;

                Matrix4x3_FromRotationY(&data_020a0e68, (short)(self->HorzAngleToCPlayer() + kHalfTurn));
                Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, kPullTilt);
                MulVec3Mat4x3(&pull, &data_020a0e68, &out);

                {
                    const Vector3 *playerPos = (const Vector3 *)&player->mPosX;
                    pull = *playerPos;
                }
                pull.x += out.x;
                pull.y += out.y;
                pull.z += out.z;
                player->mPosX = pull.x;
                player->mPosY = pull.y;
                player->mPosZ = pull.z;
            }
        }
    }
    return 1;
}

// @symbol func_ov026_02111ed8
/* Spin, on entry: nothing to set up. */
extern "C" int func_ov026_02111ed8(void)
{
    return 1;
}

// @symbol func_ov026_02111ee0
/* Install a state and run its enter half, if it has one. */
extern "C" int func_ov026_02111ee0(daWater_Tatumaki_c *self, const daWater_Tatumaki_c::State *state)
{
    self->mState = state;
    const daWater_Tatumaki_c::State *s = self->mState;
    if (s->enter == 0) return 1;
    return (self->*s->enter)();
}

// @symbol func_ov026_02111f30
/* Rebuild the model matrix from the position and angles. */
extern "C" void func_ov026_02111f30(daWater_Tatumaki_c *self)
{
    Vector3 pos;
    Vec3_Asr(&pos, (Vector3 *)&self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    /* Twelve flat words. A Matrix4x3 assignment copies r and t separately. */
    *(Mtx43 *)&self->mModelAnim.mat4x3 = *(Mtx43 *)&data_020a0e68;
}

// @symbol _ZN18daWater_Tatumaki_c16CleanupResourcesEv
int daWater_Tatumaki_c::CleanupResources()
{
    AS_SHARED(MODEL_FILE).Release();
    AS_SHARED(ANIM_FILE).Release();
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c16OnPendingDestroyEv
void daWater_Tatumaki_c::OnPendingDestroy()
{
}

// @symbol _ZN18daWater_Tatumaki_c6RenderEv
int daWater_Tatumaki_c::Render()
{
    mTextureTransformer.Update(mModelAnim.data);
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c8BehaviorEv
int daWater_Tatumaki_c::Behavior()
{
    volatile int spilled[3]; /* x and the raised y are reloaded as arguments */
    int x, y, z;

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    if (mState->execute != 0) {
        (this->*mState->execute)();
    }
    UpdatePos(0);

    x = mPosX;
    spilled[0] = x;
    y = mPosY;
    spilled[1] = y;
    z = mPosZ;
    spilled[2] = z;
    y += kSprayLift;
    spilled[1] = y;

    mParticleID = Particle_System_New(
        *(volatile u32 *)&mParticleID, kSprayEffect, spilled[0], spilled[1], z, 0, 0);

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;

    func_ov026_02111f30(this);
    mTextureTransformer.Advance();
    mModelAnim.Advance();
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c13InitResourcesEv
int daWater_Tatumaki_c::InitResources()
{
    BMD_File *model = (BMD_File *)Model::LoadFile(AS_SHARED(MODEL_FILE));
    mModelAnim.SetFile(model, 1, -1);

    Animation::LoadFile(AS_SHARED(ANIM_FILE));

    TextureTransformer::Prepare(*(BMD_File *)MODEL_FILE.file, WHIRLPOOL_BTA);
    TextureTransformer_SetFile(&mTextureTransformer, WHIRLPOOL_BTA, 0, kAnimSpeed, 0);

    mTextureTransformer.speed = kAnimSpeed;
    mModelAnim.speed = kAnimSpeed;

    mCenter.x = mPosX;
    mCenter.y = mPosY;
    mCenter.z = mPosZ;
    mCenter.y -= kFunnelDepth;

    ModelAnim_SetAnim(&mModelAnim, (BCA_File *)ANIM_FILE.file, 0, kAnimSpeed, 0);

    func_ov026_02111ee0(this, STATE_SPIN);
    return 1;
}
