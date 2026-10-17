//cpp
/* dEntObj_c and UnknownVsPlayer. ov075 0x02113ee0..0x02115ab8, 43 functions.
 *
 * The versus-mode entry scene's stage object: four player figures on the
 * entry stage. Each figure runs two pointer-to-member state tables, one for
 * movement (UnknownVsPlayer::mMoveState) and one for animation (mAnimState).
 * The state handlers and the scene's own bookkeeping are members of the two
 * classes; the ROM keeps no names for them, so they keep address spellings.
 *
 * The class name is the ROM's own: _ZTS9dEntObj_c at 0x0211c648, with
 * _ZTI9dEntObj_c at 0x0211c66c and the vtable _ZTV9dEntObj_c at 0x0211c6a0.
 * UnknownVsPlayer has no vtable and no RTTI, so the cartridge gives no name
 * for it and the project's name stays.
 *
 * #pragma defer_codegen off lays .text down in source order, so the file
 * reads in ROM order: the structors, the helpers, the four vtable methods,
 * the classInit factory, then UnknownVsPlayer's constructor.
 *
 * leftovers:
 *  - The Fix12-by-value member calls (BlendModelAnim::SetAnim,
 *    ModelAnim::SetAnim, TextureSequence::SetFile,
 *    dExtShadowModel_c::InitModel, Particle::System::New,
 *    G3i::PerspectiveW_) keep measured scalar-extern spellings: spelled as
 *    the real member types, mwccarm homes the register arguments and the
 *    callers grow.
 *  - LAUNDER((char*)) stays on the mModel.data / mAnimation.data /
 *    mModel.mat4x3 alias reads in func_ov075_02114be4 -- the plain member
 *    read changes which value-numbering class mwcc folds them into.
 *  - dEntObj_c_classInit keeps its hand-written ABI construction
 *    (fBase_c C2, explicit _ZTV9dEntObj_c + 2 store, __cxa_vec_ctor over
 *    the four UnknownVsPlayer elements): the natural `new dEntObj_c`
 *    emits the base steps inline instead of the C2 branch and no vec
 *    ctor call.
 *  - VsBoneFile/VsBoneComponents/S48 stay file-layout shadows: the bone
 *    record offsets are measured, not the BMD/ModelComponents layouts.
 *  - Behavior's unk_155 probe keeps the second `walk` induction variable
 *    alongside `player`: folding it into `player->unk_155` drops a
 *    register and rewrites the whole function.
 *  - func_ov075_0211524c's row*16/playerNo*4 byte form stays: the
 *    int[]-indexed spelling emits a different addressing pair.
 */

#include "dEntObj_c.h"
#include "decl_common.h"
#include "common.h"
#include "dClipper.h"
#include "SharedFilePtr.h"
#include "nitro/fx/fx.h"
#include "SpawnInfo.h"

struct BMD_File;
struct BTP_File;

namespace cstd { int fdiv(int a, int b); }
namespace Particle { void RenderAll(); }
namespace G3i {
    void LookAt_(const Vector3 *eye, const Vector3 *at, const Vector3 *up, bool b, Matrix4x3 *m);
}
bool ApproachLinear(short &value, short target, short step);
void CopyToViewMat(const Matrix4x3 *m);

/* The resource handles as the ROM lays them out: {id, loaded file}. The
   model handles construct through func_02017acc and destroy through
   func_02017ab4; the animation handles construct through
   SharedFilePtr::Construct and destroy through SharedFilePtr_Destruct_Anim;
   the texture-sequence handles construct through
   SharedFilePtr_Construct_TexSeq and destroy through
   SharedFilePtr_Destruct_TexSeq. The manifest aliases each undefined member
   onto its ROM symbol. */
struct EntObjModelFilePtr : SharedFilePtr {
    u32 id; void *ptr;

    EntObjModelFilePtr(u32 fileID);
    ~EntObjModelFilePtr();
};

struct EntObjAnimationFileHandle : SharedFilePtr {
    u32 id; void *ptr;

    EntObjAnimationFileHandle(u32 fileID);
    ~EntObjAnimationFileHandle();
};

struct EntObjTextureSequenceFileHandle : SharedFilePtr {
    u32 id; void *ptr;

    EntObjTextureSequenceFileHandle(u32 fileID);
    ~EntObjTextureSequenceFileHandle();
};

/* Fix12<int> is passed by value at these call boundaries. Spelled as the
 * class type, mwccarm homes the register arguments and the callers grow, so
 * the measured scalar views stay. */
extern "C" {
void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    void *anim, void *file, int numBlendFrames, int flags, int speed, unsigned short startFrame);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *anim, void *file, int flags, int speed, unsigned short startFrame);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    void *seq, void *file, int flags, int speed, unsigned short startFrame);
void _ZN17dExtShadowModel_c9InitModelEP9Matrix4x35Fix12IiES3_S3_j(
    void *shadow, Matrix4x3 *mat, int radius, int height, int depth, unsigned char flags);
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned handle, unsigned id, int x, int y, int z, const Vector3_16 *dir, void *callback);
void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
    int sinFov, int cosFov, int aspect, int nearZ, int farZ, int scaleW, bool load, Matrix4x3 *m);


int RandomIntInternal(int *seed);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
void Vec3_Asr(Vector3 *dst, Vector3 *src, int shift);
void Vec3_MulScalarInPlace(Vector3 *v, int scale);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angY);
int Math_Function_0203b14c(int *p, int target, int a, int b, int c);
unsigned char DecIfAbove0_Byte(unsigned char *p);
int func_0201251c(int a, int b, void *pos, int c);
unsigned int func_02012174(unsigned int a, unsigned int b);
unsigned int func_02012790(unsigned int sound);
void func_020167a4(void *model);
int func_ov075_0211b3d8(void *p);
long long __aeabi_uidiv(unsigned int n, int d);

extern Matrix4x3 data_020a0e68;
extern dClipper data_0209f43c;
extern Matrix4x3 data_0209b3ec;
extern int data_0209e650;
extern short data_02082214[];     /* sine/cosine table, interleaved */
extern short data_02082614[];
extern u8 data_0209fc5c[];
extern signed char data_0209fc64[];

/* SharedFilePtr handles; the loaded file sits at .ptr. */
extern EntObjAnimationFileHandle data_ov075_0211d384;
extern EntObjAnimationFileHandle data_ov075_0211d38c;
extern EntObjAnimationFileHandle data_ov075_0211d394;
extern EntObjAnimationFileHandle data_ov075_0211d39c;
extern EntObjAnimationFileHandle data_ov075_0211d3a4;
extern EntObjAnimationFileHandle data_ov075_0211d3ac;
extern EntObjAnimationFileHandle data_ov075_0211d3b4;
extern EntObjModelFilePtr data_ov075_0211d3bc;
extern EntObjModelFilePtr data_ov075_0211d3c4;
extern EntObjAnimationFileHandle data_ov075_0211d3cc;
extern EntObjAnimationFileHandle data_ov075_0211d3d4;
extern EntObjTextureSequenceFileHandle data_ov075_0211d3dc;
extern EntObjAnimationFileHandle data_ov075_0211d3e4;
extern EntObjAnimationFileHandle data_ov075_0211d3ec;
extern EntObjAnimationFileHandle data_ov075_0211d3f4;
extern EntObjModelFilePtr data_ov075_0211d3fc;
extern EntObjModelFilePtr data_ov075_0211d404;
extern EntObjTextureSequenceFileHandle data_ov075_0211d40c;
extern EntObjAnimationFileHandle data_ov075_0211d414;
extern EntObjAnimationFileHandle data_ov075_0211d41c;
extern EntObjAnimationFileHandle data_ov075_0211d424;
extern EntObjAnimationFileHandle data_ov075_0211d42c;
extern int *data_ov075_0211c678[];
extern int *data_ov075_0211c688[];

extern unsigned char data_ov075_0211b524[];
extern int data_ov075_0211b534[];
extern int data_ov075_0211b544[];
extern int data_ov075_0211b554[][4];
extern int data_ov075_0211b594[];

extern void *_ZN7fBase_cnwEj(unsigned int size);
extern void *_ZN7fBase_cC2Ev(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *p);
extern void *_ZN5ModelC1Ev(void *p);
extern void *_ZN9ModelAnimC1Ev(void *p);
extern void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
                           void (*ctor)(void *), void (*dtor)(void *));
extern void *data_0208e4b8;
extern UnknownVsPlayer *_ZN15UnknownVsPlayerD1Ev(UnknownVsPlayer *object);
extern UnknownVsPlayer *_ZN15UnknownVsPlayerC1Ev(UnknownVsPlayer *object);

}

/* A pointer-to-member row: the two per-figure state tables are arrays of
 * these. __sinit_ov075_0211b5e0 fills them, which fixes each index:
 *
 *   data_ov075_0211d56c, movement (mMoveState):
 *     0 none, 1 func_ov075_021147d4, 2 func_ov075_0211478c,
 *     3 func_ov075_0211473c, 4 none, 5 func_ov075_02114560, 6 none,
 *     7 func_ov075_021143e4, 8 func_ov075_02114390, 9 func_ov075_02114300
 *   data_ov075_0211d53c, animation (mAnimState):
 *     0 none, 1 func_ov075_0211427c, 2 func_ov075_02114218,
 *     3 func_ov075_021141b8, 4 func_ov075_021140e4, 5 func_ov075_02114010
 *
 * "none" rows point at the empty func_ov075_02114890 / func_ov075_021142fc. */
typedef void (UnknownVsPlayer::*UnknownVsPlayerState)();
struct UnknownVsPlayerStateRow { UnknownVsPlayerState state; };
extern "C" UnknownVsPlayerStateRow data_ov075_0211d56c[];
extern "C" UnknownVsPlayerStateRow data_ov075_0211d53c[];

typedef struct S48 { int w[12]; } S48;

#define LAUNDER(x) ((char*)(x))

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* 0x02113ee0 _ZN9dEntObj_cD1Ev, 0x02113f54 _ZN9dEntObj_cD0Ev                 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dEntObj_cD1Ev
dEntObj_c::~dEntObj_c()
{
}

// @symbol _ZN9dEntObj_cD0Ev
/* The deleting destructor (D0) has no source of its own: the compiler emits
   it from the definition above. */

/* -------------------------------------------------------------------------- */
/* 0x02113fdc _ZN15UnknownVsPlayerD1Ev                                        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15UnknownVsPlayerD1Ev
UnknownVsPlayer::~UnknownVsPlayer()
{
}

/* dExtFrameCtrl_c state 5, the fast gait. Below half speed it drops back to the
 * state-4 gait; the playback rate follows the speed, and frames 4 and 0x22
 * (the footfalls) play a step sound at the figure's screen position. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114010Ev
void UnknownVsPlayer::func_ov075_02114010(){
    if (mSpeed < mMaxSpeed / 2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &mModel, (void *)data_ov075_0211d424.ptr, 4, 0, 0x1000, 0);
        mAnimState = 4;
    }
    mModel.speed = cstd::fdiv(mSpeed, mMaxSpeed) + 0x1000;
    if (!mModel.WillHitFrame(4)) {
        if (mModel.WillHitFrame(0x22) == 0) return;
    }
    Vector3 screenPos;
    data_0209f43c.TransformAndClip(data_0209b3ec, mPosition, 0, screenPos);
    func_0201251c(0, 0x20, &screenPos, mSpeed);
}

/* dExtFrameCtrl_c state 4, the slow gait: the mirror of state 5, switching up to
 * it at half speed. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_021140e4Ev
void UnknownVsPlayer::func_ov075_021140e4(){
    if (mSpeed >= mMaxSpeed / 2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &mModel, (void *)data_ov075_0211d42c.ptr, 4, 0, 0x1000, 0);
        mAnimState = 5;
    }
    mModel.speed = cstd::fdiv(mSpeed, mMaxSpeed) + 0x1000;
    if (!mModel.WillHitFrame(4)) {
        if (mModel.WillHitFrame(0x22) == 0) return;
    }
    Vector3 screenPos;
    data_0209f43c.TransformAndClip(data_0209b3ec, mPosition, 0, screenPos);
    func_0201251c(0, 0x20, &screenPos, mSpeed);
}

/* dExtFrameCtrl_c state 3: once the current animation finishes, switch to the
 * one in data_ov075_0211d3ec and drop to state 0. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_021141b8Ev
int UnknownVsPlayer::func_ov075_021141b8(){
    int finished = mModel.Finished();
    if (!finished) return finished;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        &mModel, (void *)data_ov075_0211d3ec.ptr, 4, 0, 0x1000, 0);
    mAnimState = 0;
    return 0;
}

/* dExtFrameCtrl_c state 2: once the current one-shot finishes, start the next one
 * and hand over to state 3. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114218Ev
int UnknownVsPlayer::func_ov075_02114218(){
    int finished = mModel.Finished();
    if (!finished) return finished;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        &mModel, (void *)data_ov075_0211d3a4.ptr, 4, 0x40000000, 0x1000, 0);
    mAnimState = 3;
    return 3;
}

/* dExtFrameCtrl_c state 1: step sounds on frames 4 and 0x22 while walking. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_0211427cEv
int UnknownVsPlayer::func_ov075_0211427c(){
    Vector3 screenPos;
    if (mModel.WillHitFrame(4) == 0) {
        int hit = mModel.WillHitFrame(0x22);
        if (hit == 0) return hit;
    }
    data_0209f43c.TransformAndClip(data_0209b3ec, mPosition, 0, screenPos);
    return func_0201251c(0, 0x20, &screenPos, mSpeed);
}

// @symbol _ZN15UnknownVsPlayer19func_ov075_021142fcEv
void UnknownVsPlayer::func_ov075_021142fc(){
}

/* Movement state 9: until the figure is past x = 0x1c2000, keep its
 * particle effect on bone 15. The bone's translation is carried into the
 * scene by the model matrix, then scaled back up by 8 (the render matrices
 * hold positions >> 3). */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114300Ev
void UnknownVsPlayer::func_ov075_02114300(){
    if (mPosition.x >= 0x1c2000) return;
    Vector3 pos;
    Matrix4x3 *bone = &mModel.data.transforms[15];
    MulVec3Mat4x3(&bone->t, &mModel.mat4x3, &pos);
    Vec3_MulScalarInPlace(&pos, 0x8000);
    pos.y += 0x28000;
    mParticleHandle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle, 0x140, pos.x, pos.y, pos.z, (Vector3_16 *)0, (void *)0);
}

/* Movement state 8: frame 0x14 of the animation comes round twice; the
 * first pass only arms the flag, the second plays the player's sound
 * 0x1c and stops. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114390Ev
void UnknownVsPlayer::func_ov075_02114390(){
    if (!mModel.WillHitFrame(0x14)) return;
    if (mInAir == 0) {
        mInAir = 1;
        return;
    }
    func_02012174(mPlayerNo, 0x1c);
    mMoveState = 0;
}

/* Movement state 7: pick the pose. Characters flagged in data_0209b2f0 take
 * the one fixed animation and go to state 8; the rest draw two bits at a
 * time from a shared random pattern (refilled from data_ov075_0211b524 when
 * it runs out) and go to state 9. The texture sequence starts at a
 * random frame. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_021143e4Ev
void UnknownVsPlayer::func_ov075_021143e4(){
    void *btp;
    if (data_ov075_0211d380 < 0) {
        data_ov075_0211d380 = data_ov075_0211b524[
            ((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 6];
    }
    if (data_0209b2f0[mPlayerNo] != 0) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &mModel, (void *)data_ov075_0211d3ac.ptr, 0, 0, 0x1000, 0);
        btp = (void *)data_ov075_0211d40c.ptr;
        mMoveState = 8;
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &mModel, (void *)(data_ov075_0211c688[data_ov075_0211d380 & 3])[1], 0, 0, 0x1000, 0);
        btp = (void *)data_ov075_0211d3dc.ptr;
        mMoveState = 9;
        data_ov075_0211d380 >>= 2;
    }
    TextureSequence::Prepare(*(BMD_File *)data_ov075_0211d3c4.ptr, *(BTP_File *)btp);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, btp, 0, 0x1000, 0);
    {
        unsigned int rv = (unsigned int)RandomIntInternal(&data_0209e650);
        int frames = mModel.GetFrameCount();
        long long v = __aeabi_uidiv(rv >> 0x10, frames);
        mTextureSequence.currFrame = ((unsigned)((int)(v >> 32) << 0x10)) >> 4;
    }
}

/* Movement state 5: run off the front of the stage. Past the player's
 * take-off line the figure jumps; it walks along its heading, falls under
 * gravity once in the air, and on dropping below the stage sets the
 * fell-off flags and plays the fall sound. Far enough down it parks in
 * state 6. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114560Ev
void UnknownVsPlayer::func_ov075_02114560(){
    if (mPosition.z < data_ov075_0211b534[mPlayerNo] && mInAir == 0) {
        mVertSpeed = 0x2a000;
        mInAir = 1;
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &mModel, (void *)data_ov075_0211d41c.ptr, 4, 0x40000000, 0x1000, 0);
        mAnimState = 0;
    }

    mPosition.x += FX_MUL(mSpeed, data_02082214[((unsigned short)mAngleY >> 4) * 2]);
    mPosition.y += mVertSpeed;
    mPosition.z += FX_MUL(mSpeed, data_02082214[((unsigned short)mAngleY >> 4) * 2 + 1]);

    if (mPosition.y < 0) {
        mPosition.y = 0;
        mVertSpeed = 0;
        mInAir = 0;
    }

    if (mInAir == 0) {
        mSpeed += 0x2000;
        if (mSpeed > mMaxSpeed)
            mSpeed = mMaxSpeed;
    }

    mVertSpeed -= 0x4000;
    if (mVertSpeed < -0x1e000)
        mVertSpeed = -0x1e000;

    if (mPosition.z < -0x30c000) {
        if (mFellOff == 0) {
            mFellOff = 1;
            unk_155 = 1;
            func_02012790(0x121);
        }
    }

    if (mPosition.z < -0x3e8000) {
        mMoveState = 6;
        mAnimState = 0;
    }
}

/* Movement state 3: turn toward the exit at the player's turn rate, then
 * hand over to state 4. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_0211473cEv
void UnknownVsPlayer::func_ov075_0211473c(){
    s16 angle = Vec3_HorzAngle(&mPosition, &mExitPos);
    unsigned char playerNo = mPlayerNo;
    short step = data_ov075_0211b52c[playerNo];
    if (ApproachLinear(mAngleY, angle, step)) {
        mMoveState = 4;
        mAnimState = 0;
    }
}

/* Movement state 2: wait out mWaitTimer, then face forward and turn toward
 * the exit (state 3) in the slow gait. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_0211478cEv
void UnknownVsPlayer::func_ov075_0211478c(){
    int waiting = DecIfAbove0_Byte(&mWaitTimer);
    if (waiting) return;
    mAngleY = 0;
    mMoveState = 3;
    mAnimState = 4;
}

/* Movement state 1: turn toward the target slot, walk there along x without
 * overshooting, turn back to face the camera, and go idle. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_021147d4Ev
void UnknownVsPlayer::func_ov075_021147d4(){
    if (mPosition.x != mTargetPos.x) {
        short angle = Vec3_HorzAngle(&mPosition, &mTargetPos);
        if (ApproachLinear(mAngleY, angle, 0x800) == 0) return;
        mPosition.x += mSpeed;
        if (mSpeed >= 0) {
            if (mPosition.x > mTargetPos.x) mPosition.x = mTargetPos.x;
            return;
        }
        if (mPosition.x < mTargetPos.x) mPosition.x = mTargetPos.x;
        return;
    }
    if (ApproachLinear(mAngleY, 0, 0x800) == 0) return;
    func_ov075_02114a6c();
}

// @symbol _ZN15UnknownVsPlayer19func_ov075_02114890Ev
void UnknownVsPlayer::func_ov075_02114890(){
}

/* Start the run off the stage (movement state 5) from a standstill. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114894Ev
void UnknownVsPlayer::func_ov075_02114894(){
    mSpeed = 0;
    mVertSpeed = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        &mModel, (void *)data_ov075_0211d424.ptr, 4, 0, 0x1000, 0);
    mMoveState = 5;
    mAnimState = 4;
    mInAir = 0;
}

/* Waiting at the exit (movement state 4)? */
// @symbol _ZN15UnknownVsPlayer19func_ov075_021148f0Ev
int UnknownVsPlayer::func_ov075_021148f0(){
    return mMoveState == 4;
}

/* Send the figure toward the exit at x = exitX after a short random wait
 * (movement state 2). */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114904Ei
void UnknownVsPlayer::func_ov075_02114904(int exitX){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        &mModel, (void *)data_ov075_0211d384.ptr, 8, 0x40000000, 0x1000, 0);
    mWaitTimer = (((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) + 0x1e) & 0xf;
    mExitPos.x = exitX;
    mExitPos.y = 0;
    mExitPos.z = -0x3e8000;
    mMoveState = 2;
    mAnimState = 0;
}

/* The selected figure's one-shot, followed by animation state 3. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114988Ev
int UnknownVsPlayer::func_ov075_02114988(){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        &mModel, (void *)data_ov075_0211d3a4.ptr, 8, 0x40000000, 0x1000, 0);
    mAnimState = 3;
    return 3;
}

/* Walk to a new slot at x = targetX (movement state 1). */
// @symbol _ZN15UnknownVsPlayer19func_ov075_021149d0Ei
void UnknownVsPlayer::func_ov075_021149d0(int targetX){
    if (mTargetPos.x == targetX)
        return;
    mTargetPos.x = targetX;
    if (targetX >= mPosition.x)
        mSpeed = 0x8000;
    else
        mSpeed = -0x8000;
    mMoveState = 1;
    mAnimState = 1;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        &mModel, (void *)data_ov075_0211d42c.ptr, 4, 0, 0x1000, 0);
    mModel.speed = 0x1000;
}

/* Idle (movement state 0)? */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114a58Ev
int UnknownVsPlayer::func_ov075_02114a58(){
    return mMoveState == 0;
}

/* Go idle in the player's own standing animation. */
// @symbol _ZN15UnknownVsPlayer19func_ov075_02114a6cEv
int UnknownVsPlayer::func_ov075_02114a6c(){
    unsigned char playerNo = mPlayerNo;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        &mModel, (void *)data_ov075_0211c678[playerNo][1], 4, 0, 0x1000, 0);
    mModel.speed = 0x1000;
    mMoveState = 0;
    mAnimState = 0;
    return 0;
}

// @symbol _ZN15UnknownVsPlayer19func_ov075_02114ac4EP7Vector3S1_
/* dCamera_c follow for the focused figure. While it runs off (movement state 5)
 * the eye pulls back with it; once it has dropped away (state 6) the eye
 * rises and eases toward the target's depth, and the target follows the
 * eye's height. Returns nonzero when the view needs rebuilding. */
int UnknownVsPlayer::func_ov075_02114ac4(Vector3 *at, Vector3 *pos){
    int state = mMoveState;
    if (state == 5) {
        int back = cstd::fdiv(mPosition.z, 0x19000);
        pos->z = back + 0x50000;
        return 1;
    }
    if (state != 6)
        return 0;
    Math_Function_0203b0fc((int *)&pos->y, 0x2bc00, 0x66, 0x1c00);
    Math_Function_0203b14c((int *)&pos->z, at->z, 1, 0x4000, 0x100);
    at->y = pos->y;
    return 1;
}

// @symbol _ZN15UnknownVsPlayer19func_ov075_02114b60Ev
/* Put the model's bone translations back to the file's rest pose: bone 0,
 * then bones 2 onward (bone 1 keeps whatever the animation gave it). The
 * file's bone records are 0x40 bytes with the translation at +0x24; the
 * model's are 0x34 bytes with it at +0x20. */
struct VsBoneFileRecord {
    char _pad0[0x24];
    int x, y, z;
    char _pad30[0x10];
};
struct VsBoneRecord {
    char _pad0[0x20];
    int x, y, z;
    char _pad2c[0x8];
};
struct VsBoneFile {
    char _pad0[4];
    u32 numBones;
    VsBoneFileRecord *bones;
};
struct VsBoneComponents {
    VsBoneFile *file;
    char _pad0[4];
    VsBoneRecord *bones;
};
void UnknownVsPlayer::func_ov075_02114b60(){
    VsBoneComponents *q = (VsBoneComponents *)&mModel.data;
    VsBoneRecord *dst;
    VsBoneFile *file = q->file;
    VsBoneRecord *d = q->bones;
    VsBoneFileRecord *src = file->bones;
    VsBoneFileRecord *from;
    s16 i;
    d->x = src->x;
    d->y = src->y;
    d->z = src->z;
    dst = d + 2;
    from = src + 2;
    for (i = 2; i < file->numBones; i++) {
        dst->x = from->x;
        dst->y = from->y;
        dst->z = from->z;
        dst++;
        from++;
    }
}

// @symbol _ZN15UnknownVsPlayer19func_ov075_02114be4Ev
/* Draw one figure: restore its bone translations, pose it, push the palette
 * value into every material and draw the body; then the second model takes
 * the body's matrix and is drawn with the texture sequence applied. */
void UnknownVsPlayer::func_ov075_02114be4(){
    BMD_File *file;
    char *mat;

    func_020167a4(&mModel);
    func_ov075_02114b60();
    func_0204531c((char *)&mModel.data, mModel.blendWeight);

    {
        ModelComponents *data = (ModelComponents *)LAUNDER(&mModel.data);
        unsigned int i;
        file = data->modelFile;
        mat = (char *)data->materials;
        for (i = 0; i < file->numMaterials; i++) {
            *(int *)(mat + 0x20) = mMaterialColor;
            mat += sizeof(BMD_Material);
        }
        mModel.Model::Render(0);
        ModelAnim *anim = &mAnimation;
        anim->Virtual10(data->transforms[15]);
    }

    *(S48 *)&mAnimation.mat4x3 = *(S48 *)LAUNDER(&mModel.mat4x3);

    {
        ModelComponents *data = (ModelComponents *)LAUNDER(&mAnimation.data);
        unsigned int i;
        file = data->modelFile;
        mat = (char *)data->materials;
        for (i = 0; i < file->numMaterials; i++) {
            *(int *)(mat + 0x20) = mMaterialColor;
            mat += sizeof(BMD_Material);
        }
    }

    mAnimation.Model::Render(0);
    mTextureSequence.Update(mAnimation.data);
}

// @symbol _ZN15UnknownVsPlayer19func_ov075_02114cd8Ev
/* One figure's frame: run its movement state, rebuild the model matrix from
 * position and heading, run its animation state, then advance both
 * animations and place the shadow. */
void UnknownVsPlayer::func_ov075_02114cd8(){
    unk_155 = 0;
    (this->*data_ov075_0211d56c[mMoveState].state)();
    Matrix4x3_FromTranslation(&data_020a0e68, mPosition.x >> 3,
                              mPosition.y >> 3, mPosition.z >> 3);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    *(S48 *)&mModel.mat4x3 = *(S48 *)&data_020a0e68;
    (this->*data_ov075_0211d53c[mAnimState].state)();
    mModel.Advance();
    mTextureSequence.Advance();
    _ZN17dExtShadowModel_c9InitModelEP9Matrix4x35Fix12IiES3_S3_j(
        &mShadow, &mModel.mat4x3, 0x50000, 0x1f4000, 0x50000, 0xf);
}

// @symbol _ZN15UnknownVsPlayer19func_ov075_02114ddcEhhi
/* Set up the figure for player slot playerNo at x: both models, the pose
 * animation for this kind of entry scene, the texture sequence at a random
 * frame, the shadow and the start position. Kinds 0 and 2 start idle; the
 * others start in movement state 7. */
int UnknownVsPlayer::func_ov075_02114ddc(unsigned char kind, unsigned char playerNo, int x){
    void *btp;

    mModel.SetFile((BMD_File *)data_ov075_0211d404.ptr, 1, 1);
    mMaterialColor = *(int *)((char *)mModel.data.materials + 0x20) + (playerNo << 1);
    mAnimation.SetFile((BMD_File *)data_ov075_0211d3c4.ptr, 1, 1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mAnimation, (void *)data_ov075_0211d414.ptr, 0, 0x1000, 0);

    if (kind == 0 || kind == 2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &mModel, (void *)data_ov075_0211c678[playerNo][1], 0, 0, 0x1000, 0);
        btp = (void *)data_ov075_0211d40c.ptr;
        mMoveState = 0;
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &mModel, (void *)data_ov075_0211d3ac.ptr, 0, 0, 0x1000, 0);
        btp = (void *)data_ov075_0211d40c.ptr;
        mMoveState = 7;
    }

    TextureSequence::Prepare(*(BMD_File *)data_ov075_0211d3c4.ptr, *(BTP_File *)btp);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, btp, 0, 0x1000, 0);

    {
        unsigned rv = (unsigned)RandomIntInternal(&data_0209e650);
        int frames = mModel.GetFrameCount();
        mTextureSequence.currFrame = (((rv >> 0x10) % (unsigned)frames) << 0x10) >> 4;
    }

    if (mShadow.InitCylinder() == 0)
        return 0;

    mPosition.x = x;
    mPosition.y = 0;
    mPosition.z = 0;
    mTargetPos.x = mPosition.x;
    mTargetPos.y = mPosition.y;
    mTargetPos.z = mPosition.z;
    mMaxSpeed = data_ov075_0211b544[playerNo];
    mAnimState = 0;
    mPlayerNo = playerNo;
    return 1;
}

// @symbol _ZN9dEntObj_c19func_ov075_02114fa8Ev
/* Send every present player's figure toward its exit, and pick the one the
 * camera follows: player 2, else 1, else 3, else 0. */
void dEntObj_c::func_ov075_02114fa8(){
    int i;
    unsigned char *present = data_0209fc5c;
    UnknownVsPlayer *player = mPlayers;
    for (i = 0; i < 4; i++) {
        if (*present != 0) {
            int x = func_ov075_0211524c(i);
            player->func_ov075_02114904(x);
        }
        present++;
        player++;
    }

    if (data_0209fc5c[2] != 0)
        mFocusedPlayer = 2;
    else if (data_0209fc5c[1] != 0)
        mFocusedPlayer = 1;
    else if (data_0209fc5c[3] != 0)
        mFocusedPlayer = 3;
    else
        mFocusedPlayer = 0;
    mAnimActive = 0;
    mState = 1;
}

// @symbol _ZN9dEntObj_c19func_ov075_0211505cEv
/* Stop the selection animation and return the chosen figure to idle. */
void dEntObj_c::func_ov075_0211505c(){
    int r;
    if (mAnimActive == 0) return;
    r = func_0203da9c();
    mPlayers[r].func_ov075_02114a6c();
    mAnimActive = 0;
}

// @symbol _ZN9dEntObj_c19func_ov075_02115098Ei
/* Once every figure is idle, play the selection one-shot on player
 * playerNo's figure (if it is this console's player) and put the selection
 * model over it. Returns 0 while any figure is still moving. */
int dEntObj_c::func_ov075_02115098(int playerNo){
    if (mAnimActive != 0)
        return 1;
    int i;
    UnknownVsPlayer *player = mPlayers;
    for (i = 0; i < 4; i++) {
        if (player->func_ov075_02114a58() == 0)
            return 0;
        player++;
    }
    int me = func_0203da9c();
    if (playerNo == me) {
        mPlayers[playerNo].func_ov075_02114988();
        func_ov075_021151b4(playerNo);
        mAnimActive = 1;
    }
    return 1;
}

// @symbol _ZN9dEntObj_c19func_ov075_02115134Ev
/* Walk every figure to its slot for the current player count, and play the
 * join or leave sound when the count changed. */
void dEntObj_c::func_ov075_02115134(){
    s32 i;
    UnknownVsPlayer *player = mPlayers;
    u8 count, last;
    for (i = 0; i < 4; i++) {
        player->func_ov075_021149d0(func_ov075_02115290(i));
        player++;
    }
    count = data_0209fc50;
    last = mPlayerCount;
    if (last > count)
        func_02012790(0x12a);
    else if (last < count)
        func_02012790(0x129);
    mPlayerCount = count;
}

// @symbol _ZN9dEntObj_c19func_ov075_021151b4Ei
/* Put the selection model over player playerNo's figure, 0x32 units up. */
void dEntObj_c::func_ov075_021151b4(int playerNo){
    UnknownVsPlayer *player = mPlayers + playerNo;
    Vector3 *src = &player->mPosition;
    Vector3 v, out;

    v.x = src->x;
    v.y = src->y;
    v.z = src->z;
    v.y += 0x32000;
    Vec3_Asr(&out, &v, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, out.x, out.y, out.z);
    *(S48 *)&mModelAnim.mat4x3 = *(S48 *)&data_020a0e68;
}

// @symbol _ZN9dEntObj_c19func_ov075_0211524cEi
/* The exit x for player slot playerNo, from the row for the current player
 * count (data_0209fc64 maps a slot to its column; -1 means the last). */
int dEntObj_c::func_ov075_0211524c(int playerNo){
    unsigned int count = data_0209fc50;
    int row;
    if (count <= 1) {
        row = 0;
    } else {
        row = count - 1;
        playerNo = (unsigned char)data_0209fc64[playerNo];
        if (playerNo < 0) playerNo = 3;
    }
    return *(int *)((char *)data_ov075_0211b594 + row * 16 + playerNo * 4);
}

// @symbol _ZN9dEntObj_c19func_ov075_02115290Ei
/* The standing x for player slot playerNo, laid out the same way as the
 * exit table above. */
int dEntObj_c::func_ov075_02115290(int playerNo){
    int count = data_0209fc50;
    int row;
    if ((unsigned int)count <= 1) {
        row = 0;
    } else {
        row = count - 1;
        playerNo = data_0209fc64[playerNo];
        if (playerNo < 0) playerNo = 3;
    }
    return data_ov075_0211b554[row][playerNo];
}

// @symbol _ZN9dEntObj_c19func_ov075_021152d4Ev
/* Rebuild the projection and the view from the camera eye and target. */
void dEntObj_c::func_ov075_021152d4(){
    Matrix4x3 view;
    _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
        data_02082614[0xa], data_02082614[0xb], 0x1555, 0x1000, 0x1388000, 0x1000, true, (Matrix4x3 *)0);
    G3i::LookAt_((Vector3 *)&mCamPosX, (Vector3 *)&data_ov075_0211c660,
                 (Vector3 *)&mCamTargetX, true, &view);
    CopyToViewMat(&view);
    data_0209f43c.Func_020156DC(0x1555, 0x105b, 0x1000, 0x1388000);
}

/* -------------------------------------------------------------------------- */
/* 0x02115388 _ZN9dEntObj_c16CleanupResourcesEv                               */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dEntObj_c16CleanupResourcesEv
int dEntObj_c::CleanupResources()
{
    CleanCommonModelDataArr();
    data_ov075_0211d404.Release();
    data_ov075_0211d3c4.Release();
    data_ov075_0211d414.Release();
    if (param1 != 1) {
        data_ov075_0211d394.Release();
        data_ov075_0211d3cc.Release();
        data_ov075_0211d39c.Release();
        data_ov075_0211d3d4.Release();
        data_ov075_0211d3a4.Release();
        data_ov075_0211d3ec.Release();
        data_ov075_0211d384.Release();
        data_ov075_0211d424.Release();
        data_ov075_0211d42c.Release();
        data_ov075_0211d41c.Release();
    } else {
        data_ov075_0211d3ac.Release();
        data_ov075_0211d3b4.Release();
        data_ov075_0211d3f4.Release();
        data_ov075_0211d38c.Release();
        data_ov075_0211d3dc.Release();
    }
    data_ov075_0211d40c.Release();
    data_ov075_0211d3fc.Release();
    if (param1 != 1) {
        data_ov075_0211d3bc.Release();
        data_ov075_0211d3e4.Release();
    }
    func_ov075_0211b3b8(&unk_e80);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* 0x021154cc _ZN9dEntObj_c6RenderEv                                          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dEntObj_c6RenderEv
int dEntObj_c::Render()
{
    mModel.Render(0);
    dExtShadowModel_c::RenderAll();
    mParticles.Update();
    int i = 0;
    UnknownVsPlayer *player = mPlayers;
    do {
        player->func_ov075_02114be4();
        i++;
        player++;
    } while (i < 4);
    if (mAnimActive) {
        mModelAnim.Render(0);
    }
    func_ov075_0211b3d8(&unk_e80);
    Particle::RenderAll();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* 0x0211555c _ZN9dEntObj_c8BehaviorEv                                        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dEntObj_c8BehaviorEv
int dEntObj_c::Behavior()
{
    if (mSuspended == 0) {
        int i;
        UnknownVsPlayer *player;
        char *walk;
        u8 *present;
        int allAtExit;
        present = data_0209fc5c;
        walk = (char *)this;
        player = mPlayers;
        allAtExit = 1;
        i = 0;
        for (; i < 4; i++, player++, walk += sizeof(UnknownVsPlayer), present += 1) {
            player->func_ov075_02114cd8();
            if (((dEntObj_c *)walk)->mPlayers[0].unk_155) {
                Vector3 *src = &player->mPosition;
                int pos[3];
                pos[0] = src->x;
                pos[1] = src->y;
                pos[2] = src->z;
                func_ov075_0211ab38(&unk_e80, pos);
            }
            if (*present) {
                if (player->func_ov075_021148f0() == 0)
                    allAtExit = 0;
            }
        }

        if (mState == 1 && allAtExit != 0) {
            int j = 0;
            u8 *present2 = data_0209fc5c;
            UnknownVsPlayer *player2 = mPlayers;
            for (; j < 4; j++) {
                if (*present2) player2->func_ov075_02114894();
                present2 += 1;
                player2++;
            }
            mState = 2;
        }
        if (mState != 0) {
            if (mPlayers[mFocusedPlayer].func_ov075_02114ac4((Vector3 *)&mCamTargetX,
                                    (Vector3 *)&mCamPosX) != 0)
                func_ov075_021152d4();
        }
        if (mAnimActive) {
            int me = func_0203da9c();
            func_ov075_021151b4(me);
            mModelAnim.Advance();
        }
        func_ov075_0211b418(&unk_e80);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* 0x021156e0 _ZN9dEntObj_c13InitResourcesEv                                  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dEntObj_c13InitResourcesEv
int dEntObj_c::InitResources()
{
    int i; int kind; UnknownVsPlayer* player;

    InitialiseVramGlobals();
    Model::LoadFile(data_ov075_0211d3fc);
    if (param1 != 1) {
        Model::LoadFile(data_ov075_0211d3bc);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d3e4);
    }
    Model::LoadFile(data_ov075_0211d404);
    Model::LoadFile(data_ov075_0211d3c4);
    dExtFrameCtrl_c::LoadFile(data_ov075_0211d414);

    if (param1 != 1) {
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d394);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d3cc);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d39c);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d3d4);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d3a4);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d3ec);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d384);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d424);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d42c);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d41c);
    } else {
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d3ac);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d3b4);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d3f4);
        dExtFrameCtrl_c::LoadFile(data_ov075_0211d38c);
        TextureSequence::LoadFile(data_ov075_0211d3dc);
    }

    TextureSequence::LoadFile(data_ov075_0211d40c);

    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    dExtShadowModel_c::CleanAll();

    mModel.SetFile((BMD_File*)data_ov075_0211d3fc.ptr, 1, -1);

    func_0203c178(&data_020a0e68, 0x7d000, 0x7d000, 0x7d000);
    /* 0x888 is +0x1c inside the Model at 0x86c -- its mat4x3. The cartridge's own
       ~dEntObj_c proves the extent; see tools/dtor_members.py. */
    *(S48*)((char*)&mModel.mat4x3) = *(S48*)&data_020a0e68;

    if (param1 != 1) {
        mModelAnim.SetFile((BMD_File*)data_ov075_0211d3bc.ptr, 1, -1);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void*)data_ov075_0211d3e4.ptr, 0, 0x1000, 0);
    }

    func_ov075_0211b458((char*)&unk_e80, (int*)&data_ov075_0211c654, 0);
    mParticles.Initialise();

    player = mPlayers;
    i = 0;
    do {
        kind = param1;
        int r = func_ov075_02115290(i);
        if (!player->func_ov075_02114ddc(kind, i, r))
            return 0;
        i++;
        player++;
    } while (i < 4);

    data_ov075_0211d380 = -1;
    mAnimActive = 0;

    if (param1 == 2) {
        int v = func_0203da9c();
        func_ov075_02115098(v);
    }

    mCamTargetX = 0;
    mCamPosX = mCamTargetX;
    mCamTargetY = 0x14000;
    mCamPosY = mCamTargetY;
    mCamPosZ = 0x50000;
    mCamTargetZ = -0x8000;

    func_ov075_021152d4();

    mSuspended = 0;
    mState = 0;
    mFocusedPlayer = 0;
    mPlayerCount = data_0209fc50;
    if (mPlayerCount < 1)
        mPlayerCount = 1;

    return 1;
}

/* -------------------------------------------------------------------------- */
/* 0x021159f4 dEntObj_c_classInit                                             */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves dEntObj_c through RTTI,
 * allocation size, vtable identity, and the ENTRY_OBJECT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The array runtime passes each element address and ignores
 * lifecycle results; explicit function-pointer casts mark that runtime ABI
 * boundary. */
// @symbol dEntObj_c_classInit
extern "C" dEntObj_c* dEntObj_c_classInit(void){
  dEntObj_c* p = (dEntObj_c*)_ZN7fBase_cnwEj(sizeof(dEntObj_c));
  if (p) {
    _ZN7fBase_cC2Ev(p);
    *(void**)p = &data_0208e4b8;
    *(void**)p = _ZTV9dEntObj_c + 2;
    _ZN8Particle10SysTrackerC1Ev(&p->mParticles);
    _ZN5ModelC1Ev(&p->mModel);
    _ZN9ModelAnimC1Ev(&p->mModelAnim);
    __cxa_vec_ctor(p->mPlayers, 4, sizeof(UnknownVsPlayer),
                  (void (*)(void *))_ZN15UnknownVsPlayerC1Ev,
                  (void (*)(void *))_ZN15UnknownVsPlayerD1Ev);
  }
  return p;
}

/* -------------------------------------------------------------------------- */
/* 0x02115a88 _ZN15UnknownVsPlayerC1Ev                                        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15UnknownVsPlayerC1Ev
UnknownVsPlayer::UnknownVsPlayer()
{
}

/* The twenty-two file-scope resource handles, in retail initializer order:
   four model files (func_02017acc / func_02017ab4), sixteen animation files
   (SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim), then two
   texture-sequence files (SharedFilePtr_Construct_TexSeq /
   SharedFilePtr_Destruct_TexSeq). */
EntObjModelFilePtr data_ov075_0211d3fc(0x9c05);
EntObjModelFilePtr data_ov075_0211d3bc(0x8417);
EntObjAnimationFileHandle data_ov075_0211d3e4(0x8416);
EntObjModelFilePtr data_ov075_0211d404(0x80c4);
EntObjModelFilePtr data_ov075_0211d3c4(0x80c2);
EntObjAnimationFileHandle data_ov075_0211d414(0x8043);
EntObjAnimationFileHandle data_ov075_0211d394(0x841e);
EntObjAnimationFileHandle data_ov075_0211d3cc(0x8420);
EntObjAnimationFileHandle data_ov075_0211d39c(0x8422);
EntObjAnimationFileHandle data_ov075_0211d3d4(0x8424);
EntObjAnimationFileHandle data_ov075_0211d3a4(0x8419);
EntObjAnimationFileHandle data_ov075_0211d3ec(0x841a);
EntObjAnimationFileHandle data_ov075_0211d3ac(0x8426);
EntObjAnimationFileHandle data_ov075_0211d3b4(0x841b);
EntObjAnimationFileHandle data_ov075_0211d3f4(0x841c);
EntObjAnimationFileHandle data_ov075_0211d38c(0x841d);
EntObjAnimationFileHandle data_ov075_0211d384(0x8050);
EntObjAnimationFileHandle data_ov075_0211d424(0x8051);
EntObjAnimationFileHandle data_ov075_0211d42c(0x8048);
EntObjAnimationFileHandle data_ov075_0211d41c(0x8078);
EntObjTextureSequenceFileHandle data_ov075_0211d40c(0x804f);
EntObjTextureSequenceFileHandle data_ov075_0211d3dc(0x804e);

/* The entry-stage object's base profile: the factory, then the same priority
   in both halfwords. */
extern "C" BaseSpawnInfo<dEntObj_c> g_profile_ENTRY_OBJECT = {
    dEntObj_c_classInit,
    0x015a,
    0x015a,
};

/* The two per-figure dispatch tables: ten movement handlers for mMoveState,
   six animation handlers for mAnimState; the unused indices point at the
   empty handlers. mwcc cannot link-time-initialize pointer-to-member
   records, so the initializer copies the descriptor records in. */
extern "C" UnknownVsPlayerStateRow data_ov075_0211d56c[10] = {
    {&UnknownVsPlayer::func_ov075_02114890},
    {&UnknownVsPlayer::func_ov075_021147d4},
    {&UnknownVsPlayer::func_ov075_0211478c},
    {&UnknownVsPlayer::func_ov075_0211473c},
    {&UnknownVsPlayer::func_ov075_02114890},
    {&UnknownVsPlayer::func_ov075_02114560},
    {&UnknownVsPlayer::func_ov075_02114890},
    {&UnknownVsPlayer::func_ov075_021143e4},
    {&UnknownVsPlayer::func_ov075_02114390},
    {&UnknownVsPlayer::func_ov075_02114300},
};
extern "C" UnknownVsPlayerStateRow data_ov075_0211d53c[6] = {
    {&UnknownVsPlayer::func_ov075_021142fc},
    {(UnknownVsPlayerState)&UnknownVsPlayer::func_ov075_0211427c},
    {(UnknownVsPlayerState)&UnknownVsPlayer::func_ov075_02114218},
    {(UnknownVsPlayerState)&UnknownVsPlayer::func_ov075_021141b8},
    {&UnknownVsPlayer::func_ov075_021140e4},
    {&UnknownVsPlayer::func_ov075_02114010},
};
