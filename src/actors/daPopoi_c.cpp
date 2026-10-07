//cpp
/* POPOI foe (ov077/daPopoi_c, aliased HeaveHo_Spawn), 22 functions, all
 * daPopoi_c members except the classInit factory. REVERSE ROM order (highest
 * address first); do not reorder.
 *
 * What the actor does, as far as these functions show: it wanders around the
 * spot it spawned at; when the Player comes within 1000 units of that spot (and
 * is not Metal or Mega and GetHurtState() is negative) it chases; when its
 * sensor touches the Player it enters the Grab state, which launches them
 * (func_ov002_020db674 sets the Player's heading, horizontal speed 0x28000 and
 * vertical speed 0x70000, then calls Player::ChangeState with
 * data_ov002_02110094, which symbols/verified.tsv names _ZN6Player7ST_HURTE).
 * It is driven by a five-record state table of {enter, update} PMF pairs on
 * this class, described with the fields in include/daPopoi_c.h; the state names
 * there and in the comments below say what the handlers do, they are not
 * recovered labels. The records are data_ov077_02127ce8 Wander, 02127cf8 Pause,
 * 02127d08 Chase, 02127d18 TurnAway, 02127cd8 Grab -- bss copies of the .data
 * templates at data_ov077_02127a00..02127a48, filled by __sinit_ov077_021275fc.
 *
 * The Wander, Chase and Pause handlers read mStateTimer as an UNSIGNED short
 * through a cast of its address (dEnemyBase_c declares it s16); "heading" below means mPrevAngleY, which
 * Behavior copies into mAngleY every frame.
 *
 * deslop leftovers:
 *   - The state records and the four file homes keep decl_common.h's char
 *     spelling, so each install site casts to daPopoi_StateRecord *.
 *   - ModelAnim::SetAnim and the three collision Init calls stay spelled-out
 *     _ZN identifiers (Fix12<int> by value; the real headers can't express the
 *     call-site mangling). Sound::PlayLong and dBgCh_Actr::GetFloorResult are
 *     real calls; PlayLong takes a const Vector3& so mCamSpacePosX is punned.
 *   - func_ov077_021269a8 calls AngleDiff through an (int(*)(short,short))
 *     pointer cast: decl_common declares (int,int), and the short call-site
 *     typing is byte-load-bearing.
 *   - Render reaches ModelAnim::Render through the ModelAnimDraw slot window:
 *     the real class is multiply derived, so declaring it changes codegen.
 *   - unk_41c is only ever zeroed here, and nothing reads it in this file.
 *   - the Grab enter handler passes flags 0x40000000 to ModelAnim::SetAnim
 *     where every other state passes 0; what that bit does is not recovered.
 *   - the flag words passed to dCcAc_c::Init / dCcAcPos_c::Init
 *     (0x800004 and 0x200004) are not decoded here.
 *   - pad_3cc (0x30 bytes at 0x3cc) is untouched by any function in this file.
 */

#include "common.h"
#include "types.h"
#include "daPopoi_c.h"
#include "Model.h"
#include "Player.h"
#include "dExtFrameCtrl_c.h"
#include "SurfaceInfo.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "decl_common.h"

/* Member bodies have C++ linkage and mwccarm does not inherit C linkage into
 * their block-scope externs, so every external FUNCTION declaration lives in
 * this file-scope region (extern data names are unmangled either way; they are
 * kept here too so each call site reads identically). The _ZN spellings are
 * the Fix12-by-value signatures the real headers cannot express. */
extern "C" {
extern int data_0209f32c;               /* WATER_HEIGHT */
extern signed char data_0209f2f8;       /* LEVEL_ID */
extern int data_0209e650[];             /* RandomIntInternal state */
extern char data_020a0e68[];            /* scratch Matrix4x3 */
extern char data_ov077_02127cf8[];      /* Pause record; decl_common has no row */
extern Vector3 data_ov077_02127a5c;     /* sensor offset (0, 0, 44 units) */

unsigned short DecIfAbove0_Short(unsigned short *p);
extern unsigned int RandomIntInternal(void *s);
extern int Vec3_Dist(void *a, void *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern int func_02010844(void *unused, Vector3 *v, s16 angle);
extern int func_ov002_020db674(void *c, int a1, int a2, int a3);
void func_02012694(int a, void *b, int c);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angle);
extern void MulVec3Mat4x3(void *in, void *m, void *out);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int j);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void*, void*, void*, int, int, unsigned int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void*, void*, int, int, void*, int);
}

/* resolved: VT0 = _ZTV9daPopoi_c */
// @symbol daPopoi_c_classInit
extern "C" int *daPopoi_c_classInit(void)
{
    return (int *)new daPopoi_c;
}

// @symbol _ZN9daPopoi_c13InitResourcesEv

/* Loads the four files (data_ov077_02127c88 = file 0x40d, the model; 02127ca0 =
 * 0x40e, 02127c90 = 0x40f and 02127c98 = 0x410, the three animations, the way
 * __sinit_ov077_021275fc constructs them), sets gravity (-1.0 per frame squared)
 * and terminal fall speed (-30 units per frame), and initialises the three
 * collision objects:
 *   mdCcAc_c      radius 82, height 82 (0x52000), flags 0x800004
 *   mdCcAcPos_c   radius 84, height 50 (0x54000, 0x32000), flags 0x200004, offset
 *                 data_ov077_02127a5c = (0, 0, 44 units)
 *   mWithMeshClsn both size arguments 100 units (0x64000)
 * then copies the spawn position into mHomePos and mSavedPos, starts the
 * animation at normal speed, and installs the Wander state. */
int daPopoi_c::InitResources()
{
  Vector3 v;
  mModelAnim.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)&data_ov077_02127c88), 1, -1);
  dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov077_02127ca0);
  dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov077_02127c90);
  dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov077_02127c98);
  mVertAccel = -0x1000;
  mTerminalVelocity = -0x1e000;
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x52000, 0x52000, 0x800004, 0);
  v.x = data_ov077_02127a5c.x;
  v.y = data_ov077_02127a5c.y;
  v.z = data_ov077_02127a5c.z;
  _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0x54000, 0x32000, 0x200004, 0);
  mAngleY = mPrevAngleY;
  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
  mCaughtPlayer = 0;
  mHomePosX = mPosX;
  mHomePosY = mPosY;
  mHomePosZ = mPosZ;
  mModelAnim.speed = 0x1000;
  mSavedPosX = mPosX;
  mSavedPosY = mPosY;
  mSavedPosZ = mPosZ;
  func_ov077_02126d5c((daPopoi_StateRecord *)&data_ov077_02127ce8);   /* start in Wander */
  return 1;
}

// @symbol _ZN9daPopoi_c8BehaviorEv
/* The state machine, as this member sees it. mState points at a record of two
 * pointer-to-member functions on this class: the ENTER handler at offset 0
 * (run once by func_ov077_02126d5c as the state is installed) and the UPDATE
 * handler at offset 8 (run each frame below). */
struct daPopoi_StateRecord { int (daPopoi_c::*enter)(); int (daPopoi_c::*update)(); };

/* The per-frame update (vtable slot 6).
 *
 * Below the water surface (data_0209f32c, WATER_HEIGHT) the actor is put back at its
 * spawn position and the frame ends. Otherwise: count down mStateTimer and
 * mCooldown, run the current state's update handler, UpdatePos with the sensor
 * (mdCcAcPos_c), measure the slope under the feet, undo the step if it would go
 * off a ledge or onto steep ground, refresh the model matrix, look for a Player to
 * grab (unless already in Grab), then refresh both collision objects and step the
 * animation. */
int daPopoi_c::Behavior()
{
    int goingOffCliff;
    Vector3 floorNormal;
    int slope;
    daPopoi_StateRecord *state;

    /* Below the water surface (data_0209f32c): snap back to the spawn position
     * and skip the frame. */
    if (mPosY < data_0209f32c) {
        mPosX = mHomePosX;
        mPosY = mHomePosY;
        mPosZ = mHomePosZ;
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)((char *)&mStateTimer));
    DecIfAbove0_Short((unsigned short *)((char *)&mCooldown));

    state = mState;
    if (state->update != 0)
        (this->*state->update)();

    UpdatePos(&mdCcAcPos_c);

    /* Slope under the feet, as an angle relative to the way we are facing. */
    slope = 0;
    if (mWithMeshClsn.IsOnGround()) {
        void *floorResult = mWithMeshClsn.GetFloorResult();
        ((SurfaceInfo *)((char *)floorResult + 4))->CopyNormalTo(floorNormal);
        slope = func_02010844(((char *)this), &floorNormal, mAngleY);
    }

    /* Roll back to last frame's position if this step would walk off a ledge,
     * or onto ground tilted more than 0x100 either way (0x100 = 1.4 degrees).
     * IsGoingOffCliff gets a downward probe of 60 units (0x3c000), a steepest
     * acceptable slope of 0x2888 (57 degrees), detectWater 0, skipPipeCheck 1 and an
     * upward probe of 50 units (0x32000). */
    goingOffCliff = IsGoingOffCliff(mWithMeshClsn, 0x3c000, (s16)0x2888, 0, 1, 0x32000);
    if (goingOffCliff == 0) {
        if (slope < 0)
            slope = (s16)-slope;
        if (slope <= 0x100)
            goto writeback;
    }
    mPosX = mSavedPosX;
    mPosY = mSavedPosY;
    mPosZ = mSavedPosZ;
writeback:
    mSavedPosX = mPosX;
    mSavedPosY = mPosY;
    mSavedPosZ = mPosZ;
    UpdateWMClsn(mWithMeshClsn, 2);

    mAngleY = mPrevAngleY;
    func_ov077_02126dac();

    /* On the ground and not already in Grab (data_ov077_02127cd8): see whether the
     * sensor is touching a Player to grab. */
    if (mWithMeshClsn.IsOnGround() && mState != (daPopoi_StateRecord *)data_ov077_02127cd8) {
        func_ov077_02126528();
    }
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();

    mModelAnim.Advance();
    return 1;
}

// @symbol _ZN9daPopoi_c6RenderEv

/* Just enough of ModelAnim's vtable to reach ModelAnim::Render at slot 5
 * (offset 0x14). The five leading virtuals exist only to place that slot;
 * declaring the real class here would drag in its bases, and ModelAnim is
 * multiply derived. */
struct ModelAnimDraw {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void Render(int);  /* vtable offset 0x14 */
};

int daPopoi_c::Render()
{
    /* Nothing below the water surface is drawn. */
    if (mPosY < data_0209f32c) return 1;
    ModelAnimDraw *model = (ModelAnimDraw *)((char *)&mModelAnim);
    model->Render(0);
    return 1;
}

// @symbol _ZN9daPopoi_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */

void daPopoi_c::OnPendingDestroy()
{
}

// @symbol _ZN9daPopoi_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 4 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
int daPopoi_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov077_02127c88)->Release();
    ((SharedFilePtr *)&data_ov077_02127ca0)->Release();
    ((SharedFilePtr *)&data_ov077_02127c90)->Release();
    ((SharedFilePtr *)&data_ov077_02127c98)->Release();
    return 1;
}

// @symbol _ZN9daPopoi_c19func_ov077_02126dacEv
/* Rebuilds the model's matrix (mModelAnim.mat4x3): a rotation about Y by mAngleY,
 * with the translation (words 9..11 of the flat 12-word spelling this TU sees) set
 * to the actor's position >> 3. Called from Behavior after the position writeback. */
void daPopoi_c::func_ov077_02126dac()
{
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = mPosY >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN9daPopoi_c19func_ov077_02126d5cEP19daPopoi_StateRecord
/* Install a state and run its entry action on the same frame.
 *
 * `next` is a state record (data_ov077_02127cd8..02127d18, see
 * include/daPopoi_c.h); it lands in mState (+0x3fc). The record's first
 * pointer-to-member word is the ENTER handler: a null one returns 1 without
 * calling anything, otherwise its result is returned.
 *
 * The record pointer is read back out of the field after the store rather than
 * reused from the argument -- mwccarm emits the str and then an ldr of the same
 * slot, and sourcing it from `next` instead collapses that pair. */
int daPopoi_c::func_ov077_02126d5c(daPopoi_StateRecord *next)
{
    mState = next;

    if (mState->enter == 0) return 1;
    return (this->*mState->enter)();
}

// @symbol _ZN9daPopoi_c19func_ov077_02126cd4Ev
/* Wander, ENTER handler (record data_ov077_02127ce8). Picks a random heading (one of
 * 16 multiples of 0x1000 = 22.5 degrees) as the target angle, a random duration of
 * 170..233 frames (0xaa + 0..63) in mStateTimer, sets the animation to normal
 * speed and plays file 0x40f's animation (data_ov077_02127c90), and starts walking
 * at 8 units/frame (0x8000). Always returns 1. */
int daPopoi_c::func_ov077_02126cd4(){
  mTargetAngleY = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0xf) << 0xc);
  mStateTimer = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0xaa);
  mModelAnim.speed = 0x1000;
  mHorzSpeed = 0x8000;
  unk_41c = 0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void*)((int*)&data_ov077_02127c90)[1], 0, 0x1000, 0);
  return 1;
}

bool ApproachLinear(short &value, short target, short step);

// @symbol _ZN9daPopoi_c19func_ov077_02126ad0Ev

/* Wander, UPDATE handler (record data_ov077_02127ce8). Each frame:
 *   - distance from the actor to its home position, and the movement sound
 *     (daPopoi_SND_MOVE_LOOP) kept alive through mSoundHandle;
 *   - the ahead probe (func_ov077_02126300) firing -> TurnAway (data_ov077_02127d18),
 *     return;
 *   - against a wall -> position restored from mSavedPos;
 *   - more than 500 units (0x1f4000) from home -> the target heading becomes the
 *     direction to home, mStateTimer is raised to at least 20 frames, and the
 *     heading is stepped toward it by 0x400 (5.6 degrees);
 *   - the heading is stepped toward mTargetAngleY by 0x100 (1.4 degrees);
 *   - mStateTimer below 100: the animation speed becomes 0x1000 / (100 - timer)
 *     (raw fixed point, so it tapers from 1.0 over the last 99 frames);
 *   - mStateTimer == 0 -> Pause (data_ov077_02127cf8), return;
 *   - mCooldown nonzero -> return;
 *   - otherwise take the nearest Player; if that Player is within 1000 units
 *     (0x3e8000) of the HOME position (not of the actor), is not Metal or Mega,
 *     and GetHurtState() is negative -> Chase (data_ov077_02127d08).
 * Always returns 1. */
int daPopoi_c::func_ov077_02126ad0()
{
    int dist;
    Player* player;
    struct Vector3 pp;

    dist = Vec3_Dist(&mPosX, &mHomePosX);
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, daPopoi_SND_MOVE_LOOP, *(const Vector3 *)&mCamSpacePosX, 0);

    if (func_ov077_02126300() != 0) {
        func_ov077_02126d5c((daPopoi_StateRecord *)data_ov077_02127d18);   /* TurnAway */
        return 1;
    }

    if (mWithMeshClsn.IsOnWall() != 0) {
        mPosX = mSavedPosX;
        mPosY = mSavedPosY;
        mPosZ = mSavedPosZ;
    }

    if (dist > 0x1f4000) {
        mTargetAngleY = Vec3_HorzAngle(&mPosX, &mHomePosX);
        if (*(unsigned short*)&mStateTimer < 0x14)
            *(unsigned short*)&mStateTimer = 0x14;
        ApproachLinear(mPrevAngleY, mTargetAngleY, 0x400);
    }
    ApproachLinear(mPrevAngleY, mTargetAngleY, 0x100);

    if (*(unsigned short*)&mStateTimer < 0x64)
        mModelAnim.speed = 0x1000 / (0x64 - *(unsigned short*)&mStateTimer);

    if (*(unsigned short*)&mStateTimer == 0) {
        func_ov077_02126d5c((daPopoi_StateRecord *)data_ov077_02127cf8);   /* Pause */
        return 1;
    }

    if (mCooldown != 0)
        return 1;

    player = ClosestPlayer();
    if (player != 0) {
        struct Vector3* src = (struct Vector3*)(((long)&player->mPosX));
        pp.x = src->x;
        pp.y = src->y;
        pp.z = src->z;
        if (Vec3_Dist(&mHomePosX, &pp) < 0x3e8000
            && player->mIsMetal == 0
            && player->mIsMega == 0
            && player->GetHurtState() < 0) {
            func_ov077_02126d5c((daPopoi_StateRecord *)data_ov077_02127d08);   /* Chase */
            return 1;
        }
    }
    return 1;
}

/* Pause, ENTER handler (record data_ov077_02127cf8): stop (horizontal speed 0),
 * stand for 70 frames (mStateTimer = 0x46), normal animation speed, and play file
 * 0x410's animation (data_ov077_02127c98). Returns 1. */
int daPopoi_c::func_ov077_02126a84() {
    mHorzSpeed = 0;
    mStateTimer = 0x46;
    mModelAnim.speed = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void*)((void**)&data_ov077_02127c98)[1], 0, 0x1000, 0);
    return 1;
}

/* Pause, UPDATE handler (record data_ov077_02127cf8): when mStateTimer reaches 0,
 * go back to Wander (data_ov077_02127ce8). Returns 1. */
int daPopoi_c::func_ov077_02126a50() {
    unsigned short h = *(unsigned short*)&mStateTimer;
    if (h == 0) {
        func_ov077_02126d5c((daPopoi_StateRecord *)&data_ov077_02127ce8);   /* Wander */
    }
    return 1;
}

/* TurnAway, ENTER handler (record data_ov077_02127d18): the target heading becomes
 * mAngleY + 0x4000 (a quarter turn), animation at normal speed, file 0x40f's
 * animation (data_ov077_02127c90). Returns 1. */
int daPopoi_c::func_ov077_02126a04() {
    mTargetAngleY = mAngleY + 0x4000;
    mModelAnim.speed = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, ((void**)&data_ov077_02127c90)[1], 0, 0x1000, 0);
    return 1;
}

/* TurnAway, UPDATE handler (record data_ov077_02127d18): step the heading toward
 * mTargetAngleY by 0x500 (7.0 degrees) per frame; once AngleDiff of the two is below
 * 0x100 (1.4 degrees), set mCooldown to 30 frames and go back to Wander
 * (data_ov077_02127ce8). Returns 1. */
int daPopoi_c::func_ov077_021269a8() {
    /* (short,short) view of AngleDiff (decl_common says int,int); the cast keeps
       C linkage and the short call-site typing, which is byte-load-bearing. */
    short tgt = mTargetAngleY;
    ApproachLinear(mPrevAngleY, tgt, 0x500);
    int diff = ((int (*)(short, short))AngleDiff)(mPrevAngleY, mTargetAngleY);
    if (diff < 0x100) {
        mCooldown = 0x1e;
        func_ov077_02126d5c((daPopoi_StateRecord *)&data_ov077_02127ce8);   /* Wander */
    }
    return 1;
}

/* Chase, ENTER handler (record data_ov077_02127d08): random duration of 170..233
 * frames in mStateTimer, horizontal speed 6 units/frame (0x6000), turn rate zeroed,
 * and file 0x40f's animation (data_ov077_02127c90). The store of 0x2000 (2.0) to
 * mModelAnim.speed is overwritten by SetAnim's speed argument 0x1000, so the
 * animation ends up at speed 1.0. Returns 1. */
int daPopoi_c::func_ov077_02126930(){
  mStateTimer = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0xaa);
  mHorzSpeed = 0x6000;
  mTurnRate = 0;
  unk_41c = 0;
  mModelAnim.speed = 0x2000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void*)((int*)&data_ov077_02127c90)[1], 0, 0x1000, 0);
  return 1;
}

/* Chase, UPDATE handler (record data_ov077_02127d08). Each frame:
 *   - distance from the actor to its home position;
 *   - the ahead probe firing -> TurnAway, return;
 *   - against a wall -> position restored from mSavedPos;
 *   - nearest Player (if any): the target heading becomes the direction to them;
 *   - the heading is stepped toward mTargetAngleY by mTurnRate, and mTurnRate is
 *     itself stepped toward 0x600 (8.4 degrees) by 0x100 -- so the turn tightens
 *     over the first few frames of a chase;
 *   - the movement sound (daPopoi_SND_MOVE_LOOP) through mSoundHandle;
 *   - mStateTimer below 100: the animation speed becomes 0x1000 / (5 - timer / 20),
 *     which falls from 1.0 to 0.2 in five 20-frame bands (1/1, 1/2, 1/3, 1/4, 1/5);
 *   - more than 1500 units (0x5dc000) from home, or the timer at 0, or the ahead
 *     probe firing -> Pause (data_ov077_02127cf8).
 * Always returns 1. */
// @symbol _ZN9daPopoi_c19func_ov077_0212679cEv
int daPopoi_c::func_ov077_0212679c()
{
    struct Vector3 pv;
    struct Vector3 *pp;
    int dist;
    Player *p;
    unsigned short spd;

    dist = Vec3_Dist((void *)&mPosX, (void *)&mHomePosX);

    if (func_ov077_02126300() != 0) {
        func_ov077_02126d5c((daPopoi_StateRecord *)data_ov077_02127d18);   /* TurnAway */
        return 1;
    }

    if (mWithMeshClsn.IsOnWall() != 0) {
        mPosX = mSavedPosX;
        mPosY = mSavedPosY;
        mPosZ = mSavedPosZ;
    }

    p = ClosestPlayer();
    if (p != 0) {
        /* u64 launder forces base materialization after the null cmp */
        pp = (struct Vector3 *)(void *)(unsigned long long)(unsigned long)&p->mPosX;
        pv.x = pp->x;
        pv.y = pp->y;
        pv.z = pp->z;
        mTargetAngleY = Vec3_HorzAngle((void *)&mPosX, &pv);
    }

    ApproachLinear(mPrevAngleY, mTargetAngleY, mTurnRate);
    ApproachLinear(mTurnRate, 0x600, 0x100);

    mSoundHandle = Sound::PlayLong(
        mSoundHandle, 3, daPopoi_SND_MOVE_LOOP, *(const Vector3 *)&mCamSpacePosX, 0);

    spd = *(unsigned short *)&mStateTimer;
    if (spd < 0x64) {
        mModelAnim.speed = __aeabi_idiv(0x1000, 5 - spd / 20);
    }

    if (dist > 0x5dc000 || *(unsigned short *)&mStateTimer == 0 || func_ov077_02126300() != 0)
        func_ov077_02126d5c((daPopoi_StateRecord *)data_ov077_02127cf8);   /* Pause */

    return 1;
}

/* Grab, ENTER handler (record data_ov077_02127cd8): stop (horizontal speed 0),
 * normal animation speed, and play file 0x40e's animation (data_ov077_02127ca0) with
 * SetAnim flags 0x40000000 (every other state passes 0). Returns 1. */
int daPopoi_c::func_ov077_02126758(){
  mHorzSpeed=0;
  mModelAnim.speed=0x1000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, ((void**)&data_ov077_02127ca0)[1], 0x40000000, 0x1000, 0);
  return 1;
}

// @symbol _ZN9daPopoi_c19func_ov077_02126640Ev
/* Grab, UPDATE handler (record data_ov077_02127cd8). Older note, kept as found:
 * "daTgz_c::Kill - recovered from vtable slot identity". That label does not fit:
 * this body is at 0x02126640, past the 0x0212624c end of daTgz_c's range (see
 * daTgz_c.cpp), so it is daPopoi_c's.
 * While mCaughtPlayer is set: re-seat the
 * sensor (mdCcAcPos_c) at the (0, 0, 44 units) offset data_ov077_02127a5c; then,
 * if the sensor's otherOwner names an actor, and that actor is the caught Player, and
 * the Player is not Metal, not Mega, not mIsNoControl and has a negative
 * GetHurtState(), launch them with func_ov002_020db674 (horizontal speed 0x28000 = 40
 * units/frame, vertical speed 0x70000 = 112 units/frame, heading argument mAngleY +
 * 0x8000; the callee stores that argument as the Player's mPrevAngleY and the argument
 * + 0x8000 as the Player's mAngleY, so the Player ends up facing this actor's heading,
 * and puts the Player in ST_HURT). If that call returns nonzero, mCaughtPlayer = 0 and
 * daPopoi_SND_THROW_PLAYER plays at the actor's position.
 * Whether or not anything was caught, once the animation has Finished the state goes
 * to Pause (data_ov077_02127cf8). Returns 1. */
int daPopoi_c::func_ov077_02126640()
{
    Vector3 v;
    Player *a;
    int t;
    if (mCaughtPlayer != 0) {
        v.x = data_ov077_02127a5c.x;
        v.y = data_ov077_02127a5c.y;
        v.z = data_ov077_02127a5c.z;
        mdCcAcPos_c.SetPosRelativeToActor(v);
        if (mdCcAcPos_c.otherOwner != 0) {
            a = (Player *)dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
            if (a != 0) {
                if (a == mCaughtPlayer) {
                    if (a->mIsMetal == 0) {
                        if (a->mIsMega == 0) {
                            t = (a->mIsNoControl != 0);
                            if (t == 0) {
                                if (a->GetHurtState() < 0) {
                                    if (func_ov002_020db674(a, 0x28000, 0x70000,
                                            (int)(short)(mAngleY + 0x8000)) != 0) {
                                        mCaughtPlayer = 0;
                                        func_02012694(daPopoi_SND_THROW_PLAYER, &mCamSpacePosX, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (mModelAnim.Finished() != 0) {
        func_ov077_02126d5c((daPopoi_StateRecord *)&data_ov077_02127cf8);   /* Pause */
    }
    return 1;
}

/* Looks for a Player to grab; called from Behavior while on the ground and not
 * already in Grab. Re-seats the sensor (mdCcAcPos_c) at the (0, 0, 44 units) offset
 * data_ov077_02127a5c, then returns without doing anything unless: the sensor's
 * otherOwner is nonzero and names a live actor; that actor's actorID is PLAYER; it is
 * not Metal and not Mega; mIsNoControl is zero; GetHurtState() is negative; and
 * IsCollectingCap() is zero. If every test passes, mCaughtPlayer = that Player and
 * the Grab state (data_ov077_02127cd8) is installed. */
void daPopoi_c::func_ov077_02126528()
{
    Vector3 v;
    Player *a;
    int t;
    v.x = data_ov077_02127a5c.x;
    v.y = data_ov077_02127a5c.y;
    v.z = data_ov077_02127a5c.z;
    mdCcAcPos_c.SetPosRelativeToActor(v);
    if (mdCcAcPos_c.otherOwner == 0) return;
    a = (Player *)dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
    if (a == 0) return;
    t = (a->actorID == daPopoi_ACTOR_PLAYER);
    if (t == 0) return;
    if (a->mIsMetal == 1) return;
    if (a->mIsMega == 1) return;
    t = (a->mIsNoControl != 0);
    if (t == 1) return;
    if (a->GetHurtState() >= 0) return;
    if (a->IsCollectingCap() != 0) return;
    mCaughtPlayer = a;
    func_ov077_02126d5c((daPopoi_StateRecord *)&data_ov077_02127cd8);   /* Grab */
}

/* Probe ahead for a wall or a missing floor. Returns 1 if the way is blocked,
 * and in that case also rolls the actor back to last frame's position (mSavedPos)
 * and zeroes its horizontal speed (mHorzSpeed). Returns 0 when the path is clear.
 *
 * Two rays leave 40 units (0x28000) above the actor's position: a long level one
 * 200 units (0xc8000) ahead, and a short one 44 units (0x2c000) ahead pitched
 * down 0x3000 (67.5 degrees). "Ahead" is the +Z direction rotated by mAngleY.
 * Blocked means "the level ray hit something, OR the pitched ray found no ground".
 *
 * Guarded by data_0209f2f8 (LEVEL_ID) == 0x2a (42), so the probe only casts in
 * that level; in every other level it returns 0 without casting anything.
 *
 * NOTE on the two `end.x = sx; end.x = sx + ox;` pairs below: the dead first
 * store is deliberate. mwccarm writes the base and then the sum, and folding
 * them into one assignment changes the store sequence this function's ROM bytes
 * record. Leave them. */
int daPopoi_c::func_ov077_02126300()
{
    int y;

    if (data_0209f2f8 == 0x2a) {
        dBgCh_Lin ray1;
        dBgCh_Lin ray2;
        Vector3 start;
        Vector3 end;
        Vector3 dir;
        Vector3 out;

        start.x = 0;
        start.y = 0;
        start.z = 0;
        end.x = 0;
        end.y = 0;
        end.z = 0;
        dir.x = 0;
        dir.y = 0;
        dir.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;

        start.x = mPosX;
        y = mPosY;
        start.y = y;
        start.z = mPosZ;
        start.y = y + 0x28000;
        dir.z = 0xc8000;
        Matrix4x3_FromRotationY(data_020a0e68, mAngleY);
        MulVec3Mat4x3(&dir, data_020a0e68, &out);
        {
            int sx = start.x;
            int ox = out.x;
            int sy = start.y;
            int sz = start.z;
            int oy;
            int oz;
            end.x = sx;
            end.x = sx + ox;
            oy = out.y;
            oz = out.z;
            end.y = sy;
            end.y = sy + oy;
            end.z = sz;
            end.z = sz + oz;
        }
        ray1.SetObjAndLine(start, end, this);

        start.x = mPosX;
        y = mPosY;
        start.y = y;
        start.z = mPosZ;
        start.y = y + 0x28000;
        dir.x = 0;
        dir.y = 0;
        dir.z = 0x2c000;
        Matrix4x3_FromRotationY(data_020a0e68, mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, 0x3000);
        MulVec3Mat4x3(&dir, data_020a0e68, &out);
        {
            int sx = start.x;
            int ox = out.x;
            int sy = start.y;
            int sz = start.z;
            int oy;
            int oz;
            end.x = sx;
            end.x = sx + ox;
            oy = out.y;
            oz = out.z;
            end.y = sy;
            end.y = sy + oy;
            end.z = sz;
            end.z = sz + oz;
        }
        ray2.SetObjAndLine(start, end, this);

        if (ray1.DetectClsn() != 0 || ray2.DetectClsn() == 0) {
            mPosX = mSavedPosX;
            mPosY = mSavedPosY;
            mPosZ = mSavedPosZ;
            mHorzSpeed = 0;
            return 1;
        }
    }
    return 0;
}

// @symbol _ZN9daPopoi_cD0Ev
/* D0 is the DELETING destructor: destroy through this class (dEnemyBase_c
 * chain) then return the object to its heap via an inline operator delete.
 * Both variants are emitted from the single inline destructor in
 * daPopoi_c.h (class-form skill): D1 then D0 in ROM order, no leaf D2. */

/* (no separate definition: the single ~daPopoi_c() below emits the D0 and
 * D1 variants together; mwccarm orders the variant group itself.) */

// @symbol _ZN9daPopoi_cD1Ev
/* D1 is emitted from the inline destructor in daPopoi_c.h alongside D0
 * (class-form skill); this marker at D1's ROM ordinal keeps the
 * accounting naming it. Members are destroyed in reverse declaration
 * order, then dEnemyBase_c::~dEnemyBase_c. */

