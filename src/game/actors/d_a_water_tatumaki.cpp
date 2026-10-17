//cpp
/* Dire, Dire Docks whirlpool (WATER_TATUMAKI). A player who comes within
 * 300 of the centre is caught and swung down the funnel; once they pass the
 * centre, KillPlayer runs once. Anyone merely inside 1100 is pulled inward,
 * unless they are metal. The model is water_tatumaki.bmd (handle 0x4a9) and
 * the joint animation is water_tatumaki.bca (handle 0x4a8). The texture
 * animation is the overlay BTA at data_ov026_02112f40, not a NitroFS file.
 * Fourteen functions, ov026 .text 0x02111aa0..0x021121fc; the registry
 * factory daWater_Tatumaki_c_classInit is the last. No g_profile in this TU.
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
 *   0x190, 0x78).
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
 * - The four state bodies stay address-named methods. A coined spelling
 *   would not be the ROM symbol.
 * - Without #pragma defer_codegen off the sections come out in reverse
 *   source order (InitResources first, D1 last). The pragma is what puts
 *   this TU in ROM order. D2 still has no cartridge home.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daWater_Tatumaki_c_AssetId {
    ASSET_WATER_TATUMAKI_BCA = 0x04a8,
    ASSET_WATER_TATUMAKI_BMD = 0x04a9
};

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

/* File handles this TU's static initializer constructs, and the two state
   records it copies pointer-to-member descriptors into. Definitions are at
   the end of the file. SharedFilePtr.h has no fields, so the 8-byte handle
   is read through SharedFileBytes: fileID at +0, refCount at +2, file at +4. */
struct SharedFileBytes {
    u16 fileID;
    u8 refCount;
    u8 pad;
    void *file;
};

struct TatumakiModelFilePtr : SharedFilePtr {
    u32 words[2];

    TatumakiModelFilePtr(u32 fileID);
    ~TatumakiModelFilePtr();
};

struct TatumakiAnimFileHandle : SharedFilePtr {
    u32 words[2];

    TatumakiAnimFileHandle(u32 fileID);
    ~TatumakiAnimFileHandle();
};

extern "C" TatumakiModelFilePtr gWaterTatumakiModelFile; /* water_tatumaki.bmd, 0x4a9 */
extern "C" TatumakiAnimFileHandle gWaterTatumakiAnimationFile;  /* water_tatumaki.bca, 0x4a8 */
extern daWater_Tatumaki_c::State data_ov026_02113f2c; /* spin, waiting to catch */
extern daWater_Tatumaki_c::State data_ov026_02113f3c; /* drag the player down */
extern BTA_File data_ov026_02112f40;

#define MODEL_FILE (*(SharedFileBytes *)&gWaterTatumakiModelFile)
#define ANIM_FILE  (*(SharedFileBytes *)&gWaterTatumakiAnimationFile)
#define AS_SHARED(file) (*(SharedFilePtr *)&(file))
#define WHIRLPOOL_BTA data_ov026_02112f40
#define STATE_SPIN (&data_ov026_02113f2c)
#define STATE_DRAG (&data_ov026_02113f3c)

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

// @symbol _ZN18daWater_Tatumaki_c19func_ov026_02111b24Ev
/* Drag, each frame: orbit the player around the funnel and sink them. Below
   the centre, kill them once. */
int daWater_Tatumaki_c::func_ov026_02111b24()
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
    target.z = this->mPullRadius;
    player = this->ClosestPlayer();
    if (player) {
        Matrix4x3_FromRotationY(&data_020a0e68, this->mPullAngle);
        MulVec3Mat4x3(&target, &data_020a0e68, &offset);
        target.x = this->mCenter.x;
        target.y = this->mCenter.y;
        target.z = this->mCenter.z;
        ApproachLinear2(this->mPullSpin, kSpinTarget, kSpinStep);
        ApproachLinear(this->mPullRadius, kTightRadius, kRadiusStep);
        ApproachLinear(this->mPullHeight, this->mCenter.y - kFunnelDepth, kSinkStep);
        this->mPullAngle += this->mPullSpin;
        target.y = this->mPullHeight;
        target.x += offset.x;
        target.z += offset.z;
        ApproachLinearVec(this->mPullPos, target, kChaseStep);
        player->mPosX = this->mPullPos.x;
        player->mPosY = this->mPullPos.y;
        player->mPosZ = this->mPullPos.z;
        {
            int ang = this->HorzAngleToCPlayer() + kHalfTurn;
            player->mAngleX = kLookDown;
            player->mAngleY = ang;
            player->mAngleZ = 0;
        }
        {
            const Vector3 *playerPos = &player->mPos;
            spilled[0] = playerPos->x;
            spilled[1] = playerPos->y;
            spilled[2] = playerPos->z;
            if (this->mCenter.y > playerPos->y) {
                if (this->mPlayerKilled == 0) {
                    KillPlayer();
                    this->mPlayerKilled = 1;
                }
            }
        }
    }
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c19func_ov026_02111cb4Ev
/* Drag, on entry: start the orbit where the player is standing. */
int daWater_Tatumaki_c::func_ov026_02111cb4()
{
    Player *player = this->ClosestPlayer();
    if (player) {
        const Vector3 *playerPos = &player->mPos;
        Vector3 pos;
        pos.x = playerPos->x;
        pos.y = playerPos->y;
        pos.z = playerPos->z;
        this->mPullAngle = Vec3_HorzAngle(&this->mPos, &pos);
        this->unk_190 = 0;
        this->mPullHeight = 0;
        this->mPullRadius = 0;
        this->mPullPos.x = pos.x;
        this->mPullPos.y = pos.y;
        this->mPullPos.z = pos.z;
        this->mPullHeight = pos.y;
        this->mPullRadius = Vec3_HorzDist(&this->mCenter, &pos);
    }
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c19func_ov026_02111d4cEv
/* Spin, each frame: catch a player who has reached the centre, otherwise
   draw a non-metal player inward. The pull is stronger when they are closer. */
int daWater_Tatumaki_c::func_ov026_02111d4c()
{
    Player *player = this->ClosestPlayer();
    if (player != 0) {
        Vector3 pos;
        {
            const Vector3 *playerPos = &player->mPos;
            pos = *playerPos;
        }

        if (Vec3_HorzDist(&this->mCenter, &pos) <= kCaptureReach) {
            int dy = this->mCenter.y - pos.y;
            if (dy < 0) dy = -dy;
            if (dy <= kCaptureReach) {
                player->EnterWhirlpool();
                func_ov026_02111ee0(this, STATE_DRAG);
                return 1;
            }
        }

        if (player->mIsMetal == 0) {
            int dist = Vec3_Dist(&this->mCenter, &pos);
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

                Matrix4x3_FromRotationY(&data_020a0e68, (short)(this->HorzAngleToCPlayer() + kHalfTurn));
                Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, kPullTilt);
                MulVec3Mat4x3(&pull, &data_020a0e68, &out);

                {
                    const Vector3 *playerPos = &player->mPos;
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

// @symbol _ZN18daWater_Tatumaki_c19func_ov026_02111ed8Ev
/* Spin, on entry: nothing to set up. */
int daWater_Tatumaki_c::func_ov026_02111ed8()
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
    Vec3_Asr(&pos, &self->mPos, 3);
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

    dExtFrameCtrl_c::LoadFile(AS_SHARED(ANIM_FILE));

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

/* Reconstructed source-style name: SM64DS proves daWater_Tatumaki_c through
 * RTTI, allocation size, vtable identity, and the WATER_TATUMAKI registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: Whirlpool_Spawn. */
// @symbol daWater_Tatumaki_c_classInit
extern "C" daWater_Tatumaki_c *daWater_Tatumaki_c_classInit()
{
    return new daWater_Tatumaki_c();
}

/* Static initializer: model handle, anim handle, then the two state records.
   The pointer-to-member descriptors stay anonymous compiler objects. */
TatumakiModelFilePtr gWaterTatumakiModelFile(ASSET_WATER_TATUMAKI_BMD);
TatumakiAnimFileHandle gWaterTatumakiAnimationFile(ASSET_WATER_TATUMAKI_BCA);

daWater_Tatumaki_c::State data_ov026_02113f2c = {
    &daWater_Tatumaki_c::func_ov026_02111ed8,
    &daWater_Tatumaki_c::func_ov026_02111d4c,
};
daWater_Tatumaki_c::State data_ov026_02113f3c = {
    &daWater_Tatumaki_c::func_ov026_02111cb4,
    &daWater_Tatumaki_c::func_ov026_02111b24,
};
