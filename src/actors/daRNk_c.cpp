//cpp
/* Koopa the Quick (ROM RTTI: daRNk_c), overlay 62.
 * This production TU owns the 18 functions at 0x0211975c..0x0211af38.
 * Class identity, lifecycle and four actor overrides are reconstructed; the
 * remaining address-named helpers still retain some raw field and ABI views.
 * Their original source lineage is recorded in the TU manifest.
 *
 * What the class does. Koopa waits until the player walks up
 * (STATE_WAIT_FOR_PLAYER), turns to face them and puts up a two-way message
 * (STATE_OFFER_RACE); if the player picks answer 1 he runs the level's path
 * toward the RACE_FLAG actor (STATE_RACE), jumping at the nodes param1 names
 * and to avoid a rolling iron ball (IRONBALL, daIbl_c) that gets in his way.
 * The course timer runs from the start. When he reaches the end of the path
 * he slows to a stop (STATE_PULL_UP, STATE_STOP) and then talks about who got
 * to the flag first (STATE_POST_RACE_TALK), spawning the star if the player
 * did. The states are listed with mState in include/daRNk_c.h.
 *
 * param1: the low nibble is the path ID; bits 4..9 and bits 10..15 each give
 * a jump node (see mPathPtToJumpAt1). mAngleX's low nibble is the star number.
 *
 * The pinned compiler emits these functions in reverse source order. The
 * inline destructor and factory supply the retail D1/D0 order and class data;
 * the text-only manifest verifies and externalizes the emitted RTTI/vtable.
 * Measured source-form constraints: notes/experiments/pr2859-source-repair-0920.json.
 *
 * Leftover:
 *  - the helpers keep their addresses as method names; real naming belongs
 *    at their definitions (the State enum gives each state handler its role).
 *  - the SetAnim, PlaySub and shadow calls still go through mangled C names
 *    carrying 5Fix12IiE; their scalar signatures are what the bytes need.
 *  - Player::Unk_020c4f40(0x5a), the 0x1f PlaySub pairs, sounds 0x4d and 0xec
 *    and the 0xf last argument of DropShadowRadHeight have no recovered
 *    meaning; the sounds are named by when they play (0xec as kSfxTurnToTalk),
 *    the rest are annotated with where they happen, nothing more.
 *  - unk_3aa is written and never read in this TU.
 *  - the ANIM_* macros name each BCA by when it plays, not from its contents;
 *    the animation files themselves have not been inspected.
 *  - only the radius and height of the dCcAc_c::Init and dBgCh_Actr::Init
 *    calls are understood; the trailing arguments are passed as the bytes
 *    have them.
 */
#include "daRNk_c.h"
#include "daRFlag_c.h"
#include "daIbl_c.h"
#include "Player.h"
#include "types.h"
#include "common.h"
#include "Timer.h"
#include "SharedFilePtr.h"

/* Actor IDs (symbols/actor_debug_names.tsv). */
enum {
    kRaceFlagActorId = 0xcd,    /* RACE_FLAG: daRFlag_c, the finish flag */
    kIronBallActorId = 0xdc     /* IRONBALL: daIbl_c, the rolling iron ball */
};

/* Message IDs handed to Player::ShowMessage, named by when each is shown
   rather than by their text. LEVEL_ID 0x18 (kAltLevelID) uses different
   message IDs for the offer and the result, and a 36-unit cruise speed. */
enum {
    MSG_OFFER_NOT_MARIO = 0x9e, /* offer, player is not Mario */
    MSG_OFFER = 0x91,           /* offer to Mario */
    MSG_OFFER_ALT_LEVEL = 0xc4, /* offer to Mario when LEVEL_ID is kAltLevelID */
    MSG_RESULT_KOOPA_FIRST = 0x14d, /* Koopa reached the flag before the player */
    MSG_RESULT_CANNON = 0x14c,  /* the player got there first but used a cannon */
    MSG_RESULT_PLAYER_FIRST = 0x92, /* the player got there first */
    MSG_RESULT_ALT_LEVEL = 0xc5,    /* ... when LEVEL_ID is kAltLevelID */
    MSG_RESULT_NOT_MARIO = 0x9f,    /* result talk with a player who is not Mario */
    MSG_CHAT_MARIO = 0xa0,      /* later chatter, Mario */
    MSG_CHAT_NOT_MARIO = 0xa1   /* later chatter, anyone else */
};

enum { kAltLevelID = 0x18 };    /* LEVEL_ID with its own offer/result message IDs and cruise speed */

/* Ids by when they play. */
enum {
    kSfxFootstep = 0xe4,        /* a foot comes down in the run cycle */
    kSfxTurnToTalk = 0xec,      /* Koopa has finished turning to face the player */
    kSfx4D = 0x4d,              /* played at the race start (two positional calls) and, in 2D, when the post-race talk begins; the sound itself is unidentified */
    kMusicRace = 0x41,          /* loaded on layer 2 when the race starts, stopped when it ends */
    kPtclFootstepDust = 0xf9,   /* puff beside a foot */
    kPtclLandingDust = 0xb2     /* puff where he lands from a jump (dActor_c::HugeLandingDustAt's id too) */
};

/* Distances, in fix12 units (0x1000 = 1.0; the numbers here are the whole-unit
   values). */
enum {
    kStartTalkDist = 0xc8000,   /* 200: the player must be this close before Koopa speaks first */
    kChatDist = 0x190000,       /* 400: post-race talks start inside this */
    kBallAheadRange = 0x190000, /* 400: ball ahead of Koopa matters inside this */
    kBallBehindRange = 0x12c000,/* 300: ball in the rear half matters inside this */
    kCatchUpDist = 0x7d0000,    /* 2000: once the flag is touched, farther from the player than this and he runs at his fastest */
    kMsgAnchorHeight = 0xc8000, /* 200: ShowMessage's anchor is this far above Koopa */
    kDustHeight = 0x28000,      /* 40: landing dust is spawned this far above his feet */
    kStarSpawnHeight = 0x64000  /* 100: the star appears this far above him */
};

/* Turning speed: 0x800 a frame (0x10000 is a full turn). */
enum { kTurnStep = 0x800 };

/* dExtFrameCtrl_c playback flags for SetAnim, inferred rather than read from the
   animation files (dExtFrameCtrl_c.h only says "loop flags"; 0x40000000 = play once
   is a reading of how they are used). Most animations played with kAnimPlayOnce
   are later waited on with Finished/WillHitFrame (RUN_START, LAND, BRAKE);
   ANIM_JUMP is played once and never waited on. The idle, talk and run cycles
   are played with 0. */
enum {
    kAnimLoop = 0,
    kAnimPlayOnce = 0x40000000
};

/* Each SharedFilePtr below is {?, loaded file} with the file at +4. Names are
   by when SetAnim plays them. */
#define ANIM_IDLE      (*(void **)(data_ov062_0211e034 + 4))   /* after talking, after stopping, at spawn */
#define ANIM_TALK      (*(void **)(data_ov062_0211e03c + 4))   /* turned to face the player and talking */
#define ANIM_RUN_START ((void *)data_ov062_0211e014[1])        /* at the start of the race; then ANIM_RUN */
#define ANIM_RUN       ((void *)data_ov062_0211e024[1])        /* the looping run; speed follows mHorzSpeed */
#define ANIM_JUMP      ((void *)data_ov062_0211e02c[1])        /* in the air */
#define ANIM_LAND      ((void *)data_ov062_0211e004[1])        /* on landing, or when the path ends; then ANIM_RUN */
#define ANIM_BRAKE     (*(void **)(data_ov062_0211e01c + 4))   /* STATE_PULL_UP's last frame to a stop */

/* Play a BCA on Koopa's ModelAnim at normal speed (0x1000 = 1.0), from frame 0. */
#define SET_ANIM(this, bca, flags) \
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&(this)->mModelAnim, (bca), (flags), 0x1000, 0)

bool ApproachLinear(short &value, short target, short step);
int ApproachLinear(int &value, int target, int step);

/* func_ov062_02119af0's result. */
enum {
    PATH_DONE = -1,             /* reached the last node: the path is finished */
    PATH_TRAVELLING = 0,
    PATH_NODE_REACHED = 1       /* reached a node and turned to the next */
};

/* func_ov062_021199ac's result: what the rolling ball asks of Koopa. */
enum {
    BALL_BEHIND_STOP_AND_JUMP = -1, /* ball in the rear half and faster: stop and jump */
    BALL_NONE = 0,
    BALL_AHEAD_JUMP = 1         /* ball ahead and slow enough to jump over */
};

extern "C" {
/* Table of six pointers-to-member, one per State, built by
   __sinit_ov062_0211d4a0. */
extern int data_ov062_0211e0a4[];
void _ZN5Sound22LoadAndSetMusic_Layer2Ej(u32 id);
}

/* Scalar fixed-point ABI entry; the class-valued form still needs
   caller reconstruction under the pinned compiler. */
extern "C" s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);

extern "C" {
/* Positional sound at the actor's camera-space position (Sound::Play(3, id, pos)). */
extern void func_0201267c(unsigned int id, void *p);
extern void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern short data_02082214[];   /* SINE_TABLE: {sin, cos} pairs in fix12, indexed by angle >> 4 */
/* This scalar entry follows its C definition, including u16 startFrame.
   ModelAnim.h instead declares u32; that shared contract is still unresolved. */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, u16);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern void _ZN5Sound22StopLoadedMusic_Layer2Ev(void);
/* Play2D(3, id): a sound with no position. */
extern unsigned int func_0201277c(unsigned int id);
extern signed char data_0209f2f8;   /* LEVEL_ID */
extern s8 data_ov002_02111184;      /* DISPLAY_TIMER: nonzero shows the course timer */
extern bool _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, int d, int e);
extern void func_02012694(u32 a, void *b);
unsigned int func_02012790(unsigned int id);
extern u8 data_0209d684;            /* MESSAGE_RESULT: which answer the player picked (1 or 2) */
extern Timer data_0209d4c8;         /* TIME_TIMER: the course timer */
extern void Vec3_Asr(void *destination, void *source, int shift);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int a, int b, unsigned char g);
extern struct Matrix4x3 data_020a0e68;  /* MATRIX_SCRATCH_PAPER */
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int r, int h, unsigned int d, unsigned int e);
/* The existing C initializer erases pointer slots to integers. Keep the
   evidenced actor view here; repairing the shared initializer is separate. */
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int r, int h, void *p, int q);
/* The files InitResources loads and CleanupResources releases; see ANIM_*. */
extern char data_ov062_0211e00c[];  /* the model (BMD) */
extern int data_ov062_0211e014[];   /* ANIM_RUN_START */
extern int data_ov062_0211e024[];   /* ANIM_RUN */
extern char data_ov062_0211e01c[];  /* ANIM_BRAKE */
extern char data_ov062_0211e034[];  /* ANIM_IDLE */
extern char data_ov062_0211e03c[];  /* ANIM_TALK */
extern int data_ov062_0211e02c[];   /* ANIM_JUMP */
extern int data_ov062_0211e004[];   /* ANIM_LAND */
}

// @symbol daRNk_c_classInit
extern "C" {
int *daRNk_c_classInit(void)
{
    return (int *)new daRNk_c;
}
}

// @symbol _ZN7daRNk_c13InitResourcesEv
/* Load the model and animations, set up the collision, read the actor's
   parameters (see the banner) and start at the first path node. Returns 0 if
   the model or shadow cannot be set up. */
int daRNk_c::InitResources()
{
    unsigned char b;
    int zero;

    Model::LoadFile(*(SharedFilePtr *)data_ov062_0211e00c);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov062_0211e014);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov062_0211e024);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov062_0211e01c);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov062_0211e034);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov062_0211e03c);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov062_0211e02c);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov062_0211e004);
    if (mModelAnim.SetFile(*(BMD_File **)(data_ov062_0211e00c + 4), 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    SET_ANIM(this, ANIM_IDLE, kAnimLoop);
    mScaleX = 0x14cc;   /* 0x14cc / 0x1000 = ~1.3 (1.2998) */
    mScaleY = 0x14cc;
    mScaleZ = 0x14cc;
    /* Body cylinder: radius 120 units, height 300 units. */
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x78000, 0x12c000, 0x800004, 0);
    zero = 0;
    mState = zero;
    unk_3aa = (s16)zero;
    mHasFinished = (unsigned char)zero;
    mSpawnPos.x = mPosX;
    mSpawnPos.y = mPosY;
    mSpawnPos.z = mPosZ;
    mVertAccel = -0x2000;           /* gravity: 2 units a frame squared */
    mTerminalVelocity = -0x3c000;   /* 60 units a frame */
    /* Mesh collision: 120 units by 120 units. */
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x78000, 0x78000, 0, 0);
    mPathPtr.FromID(param1 & 0xf);
    mNumPathPts = mPathPtr.NumNodes();
    mCurPathPt = zero;
    mPrevPathPt.x = mPosX;
    mPrevPathPt.y = mPosY;
    mPrevPathPt.z = mPosZ;
    mPathPtr.GetNode(mPathTarget, mCurPathPt);
    mHasPlayerUsedCannon = (unsigned char)zero;
    b = (unsigned char)((((unsigned int)param1 >> 4) + 1) & 0x3f);
    mPathPtToJumpAt1 = b;
    b = (unsigned char)((((unsigned int)param1 >> 10) + 1) & 0x3f);
    mPathPtToJumpAt2 = b;
    if (mPathPtToJumpAt1 <= 1)
        mPathPtToJumpAt1 = 0xff;
    if (mPathPtToJumpAt2 <= 1)
        mPathPtToJumpAt2 = 0xff;
    mStarID = (unsigned char)(mAngleX & 0xf);
    b = mStarID;
    mTrackedStar = TrackStar(b, 2);
    mFlagID = zero;
    mPlayer = 0;
    mIsRacing = (unsigned char)zero;
    mIsTalkingToMario = (unsigned char)zero;
    return 1;
}

// @symbol _ZN7daRNk_c16CleanupResourcesEv
/* Release the eight shared files, and stop the race music if it is playing. */
int daRNk_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov062_0211e00c)->Release();
    ((SharedFilePtr *)&data_ov062_0211e014)->Release();
    ((SharedFilePtr *)&data_ov062_0211e024)->Release();
    ((SharedFilePtr *)&data_ov062_0211e01c)->Release();
    ((SharedFilePtr *)&data_ov062_0211e034)->Release();
    ((SharedFilePtr *)&data_ov062_0211e03c)->Release();
    ((SharedFilePtr *)&data_ov062_0211e02c)->Release();
    ((SharedFilePtr *)&data_ov062_0211e004)->Release();
    if (mIsRacing) _ZN5Sound22StopLoadedMusic_Layer2Ev();
    return 1;
}

// @symbol _ZN7daRNk_c8BehaviorEv
/* Per frame: run the handler for mState, advance the animation, put the
   steered heading (mPrevAngleY) into mAngleY, move and collide, refresh the
   body cylinder and draw the drop shadow. */
int daRNk_c::Behavior()
{
  typedef void (daRNk_c::*StateFunc)();
  StateFunc *states = (StateFunc *)data_ov062_0211e0a4;
  (this->*states[mState])();
  mModelAnim.Advance();
  mAngleY = mPrevAngleY;
  UpdatePos(&mdCcAc_c);
  UpdateWMClsn(mWithMeshClsn, 0);
  mdCcAc_c.Clear();
  mdCcAc_c.Update();
  func_ov062_0211aac0();
  return 1;
}

// @symbol _ZN7daRNk_c6RenderEv
/* Model is the real class now, through daRNk_c.h: HideMaterial is its own
   non-virtual and the slot-5 virtual is Render, which ModelAnim overrides. */
int daRNk_c::Render()
{
  mModelAnim.HideMaterial(0, 1);
  mModelAnim.Render((const Vector3 *)&mScaleX);
  return 1;
}

// @symbol _ZN7daRNk_c19func_ov062_0211aac0Ev
/* Drop shadow: build a matrix from the position (divided by 8, as the shadow
   matrix expects) and heading in the scratch matrix, copy it into the model's matrix,
   and give DropShadowRadHeight a radius and height of 160 units. */
void daRNk_c::func_ov062_0211aac0(){
  struct Vector3 v;
  Vec3_Asr(&v, &this->mPosX, 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, this->mAngleY);
  { struct M43w { int w[12]; };  /* array-wrapper copy: keeps C's block copy under -lang c++ */
    *(M43w*)&this->mModelAnim.mat4x3 = *(M43w*)&data_020a0e68; }
  _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(this, &this->mShadowModel, &this->mModelAnim.mat4x3, 0xa0000, 0xa0000, 0xf);
}

// @symbol _ZN7daRNk_c19func_ov062_0211a9c4Ev
/* STATE_WAIT_FOR_PLAYER. Count mStateTimer down first (a declined offer sets
   it to 60 frames). Then look for the closest player; if there is one within
   200 units and Player::StartTalk accepts, find
   the race flag, remember its ID, arm it (mHasTouchedFlag = 0; the flag
   starts at 0xff, disarmed), turn
   toward the player and go to STATE_OFFER_RACE. */
void daRNk_c::func_ov062_0211a9c4()
{
    unsigned short val;
    struct Vector3 v;
    daRFlag_c *flag;
    struct Vector3 *sp;

    val = *(unsigned short *)&this->mStateTimer;
    if (val != 0) {
        *(unsigned short *)&this->mStateTimer = val - 1;
        return;
    }

    this->mPlayer = this->ClosestPlayer();
    if (this->mPlayer == 0)
        return;

    sp = (struct Vector3 *)&this->mPlayer->mPosX;
    v.x = sp->x;
    v.y = sp->y;
    v.z = sp->z;

    if (Vec3_Dist((struct Vector3 *)&this->mPosX, &v) >= kStartTalkDist)
        return;

    if (this->mPlayer->StartTalk(*this, true) == 0)
        return;

    flag = (daRFlag_c *)dActor_c::FindWithActorID(kRaceFlagActorId, 0);
    if (flag == 0)
        return;

    this->mFlagID = flag->uniqueID;
    flag->mHasTouchedFlag = 0;
    this->mState = daRNk_c::STATE_OFFER_RACE;
    this->mTargetAngleY = Vec3_HorzAngle((struct Vector3 *)&this->mPosX, &v);
    this->mStep = 0;
}

// @symbol _ZN7daRNk_c19func_ov062_0211a740Ev
/* STATE_OFFER_RACE. Offer the race after the player finishes talking.
 *   step 0  turn toward the player (0x800 a frame, mAngleY following); once
 *           facing, switch to the talk animation, play the cue, and for Mario
 *           note it in mIsTalkingToMario (func_02012790(0xa) is called too;
 *           its meaning is not recovered)
 *   step 1  when the player's talk state reads 0, show the offer (a different
 *           message for a player who is not Mario, and for LEVEL_ID 0x18)
 *   step 2  when the talk state reads 2 (apparently: the player has answered), read
 *           MESSAGE_RESULT: 1 starts the race (STATE_RACE, mStateTimer = 50);
 *           2, or a player who is not Mario, goes back to
 *           STATE_WAIT_FOR_PLAYER with a 60-frame pause; both of those set
 *           the idle animation. An answer other than 1 or 2 from Mario
 *           changes neither state nor animation. In every case step returns
 *           to 0, mHorzSpeed to 0, and the course timer is reset. */
void daRNk_c::func_ov062_0211a740()
{
    switch (this->mStep) {
    case 0:
        if (ApproachLinear(this->mPrevAngleY, this->mTargetAngleY, kTurnStep) != 0) {
            SET_ANIM(this, ANIM_TALK, kAnimLoop);
            this->mStep += 1;
            func_0201267c(kSfxTurnToTalk, &this->mCamSpacePosX);
            if (this->mPlayer->param1 == 0) {
                func_02012790(0xa);
                this->mIsTalkingToMario = 1;
            }
        }
        this->mAngleY = this->mPrevAngleY;
        return;
    case 1:
        if (this->mPlayer->GetTalkState() != 0)
            return;
        {
            unsigned int msg;
            Vector3 v;
            if (this->mIsTalkingToMario == 0) {
                msg = MSG_OFFER_NOT_MARIO;
            } else if (data_0209f2f8 == kAltLevelID) {
                msg = MSG_OFFER_ALT_LEVEL;
            } else {
                msg = MSG_OFFER;
            }

            {
                int z = this->mPosZ;
                int y = this->mPosY + kMsgAnchorHeight;
                int x = this->mPosX;
                v.x = x;
                v.y = y;
                v.z = z;
            }
            if (this->mPlayer->ShowMessage(*this, msg, &v, 1, 0) != 0)
                this->mStep += 1;
        }
        return;
    case 2:
        if (this->mPlayer->GetTalkState() != 2)
            return;
        if (this->mIsTalkingToMario != 0) {
            if (data_0209d684 == 1) {
                this->mState = daRNk_c::STATE_RACE;
                this->mStateTimer = 0x32;   /* 50 frames; nothing in STATE_RACE reads it */
                SET_ANIM(this, ANIM_IDLE, kAnimLoop);
                this->unk_3aa = 0;
                this->mIsTalkingToMario = 0;
            } else if (data_0209d684 == 2) {
                this->mState = daRNk_c::STATE_WAIT_FOR_PLAYER;
                this->mStateTimer = 0x3c;   /* 60 frames before he speaks again */
                this->mPlayer->HasFinishedTalking();
                SET_ANIM(this, ANIM_IDLE, kAnimLoop);
                this->mIsTalkingToMario = 0;
            }
        } else {
            this->mState = daRNk_c::STATE_WAIT_FOR_PLAYER;
            this->mStateTimer = 0x3c;
            this->mPlayer->HasFinishedTalking();
            SET_ANIM(this, ANIM_IDLE, kAnimLoop);
        }
        this->mStep = 0;
        this->mHorzSpeed = 0;
        data_0209d4c8.ResetTimer();
        return;
    default:
        return;
    }
}

// @symbol _ZN7daRNk_c19func_ov062_0211a1f4Ev
/* STATE_RACE.
 *   step 0  wait for Player::Unk_020c4f40(0x5a) to report non-zero
 *   step 1  two PlaySub calls (arguments unidentified) bracket a wait for the
 *           talk to finish (talk state -1); then the race starts: sound,
 *           race music, mIsRacing, the run-start animation, the course timer
 *           shown and started
 *   step 2  run the path (func_ov062_02119af0). When it reports the end, find
 *           the flag, stop the timer and go to STATE_PULL_UP, recording in
 *           mPlayerWon whether the flag was already touched, then touch it
 *           himself. Otherwise: pick a target speed, accelerate toward it,
 *           turn toward the path, set the run animation's speed, and jump if
 *           he is at a jump node or the ball asks him to; if he walks off an
 *           edge he goes to step 3
 *   step 3  in the air: keep turning toward the path; land (back to step 2,
 *           landing dust) when the mesh collision says he is on the ground,
 *           or finish the race if the path ends mid-air */
void daRNk_c::func_ov062_0211a1f4()
{
        int progress;
    int speedMul;
    int status;
    int speedScale;
    volatile int tmp[3];

    switch (this->mStep) {
    case 0:
        if (this->mPlayer->Unk_020c4f40(0x5a) != 0)
            this->mStep++;
        return;
    case 1:
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x14, 0x7f, 0x6b000, 0);
        if (this->mPlayer->GetTalkState() != -1)
            return;
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x7f, 0, 0x7f000, 0);
        func_02012694(kSfx4D, &this->mCamSpacePosX);
        _ZN5Sound22LoadAndSetMusic_Layer2Ej(kMusicRace);
        this->mIsRacing = 1;
        this->mStep++;
        SET_ANIM(this, ANIM_RUN_START, kAnimPlayOnce);
        data_ov002_02111184 = 1;
        data_0209d4c8.StartTimer();
        func_0201267c(kSfx4D, &this->mCamSpacePosX);
        return;
    case 2:
        progress = func_ov062_02119af0();
        if (this->mModelAnim.file == (BCA_File *)ANIM_RUN)
            func_ov062_02119800();
        if (progress == PATH_DONE) {
            daRFlag_c *flag;
            if (this->mFlagID == 0)
                return;
            flag = (daRFlag_c *)dActor_c::FindWithID(this->mFlagID);
            if (flag == 0)
                return;
            this->mState = daRNk_c::STATE_PULL_UP;
            this->mStep = 0;
            data_0209d4c8.StopTimer();
            this->mPlayerWon = (flag->mHasTouchedFlag != 0) ? 1 : 0;
            flag->mHasTouchedFlag = 1;
            return;
        }
        status = func_ov062_021199ac();
        /* Uphill slowdown: when the angle between his heading and the way the
           floor tilts (atan2 of the floor normal's x and z) is at least 0x6000
           (135 degrees), scale by normalY * 7 - 6 (1.0 on level ground, less
           on a slope); otherwise full speed (1.0). */
        if (this->GetSubtraction(this->mPrevAngleY,
                _ZN4cstd5atan2E5Fix12IiES1_(this->mFloorNormalX, this->mFloorNormalZ)) >= 0x6000)
            speedScale = this->mFloorNormalY * 7 - 0x6000;
        else
            speedScale = 0x1000;
        if (this->mHasPlayerUsedCannon == 0)
            this->mHasPlayerUsedCannon = this->mPlayer->IsBeingShotOutOfCannon();
        /* Target speed is speedMul * 6 units a frame: 24 normally, 36 in
           LEVEL_ID 0x18, 48 once the player has touched the flag and is
           more than 2000 units away. The 36 and 48 only apply while mFlagID
           is set and the flag is found; otherwise speedMul stays 4. */
        speedMul = 4;
        if (this->mFlagID != 0) {
            daRFlag_c *flag = (daRFlag_c *)dActor_c::FindWithID(this->mFlagID);
            if (flag != 0) {
                if (flag->mHasTouchedFlag != 0 &&
                    Vec3_Dist((const Vector3 *)&this->mPosX,
                              (const Vector3 *)&this->mPlayer->mPosX) > kCatchUpDist)
                    speedMul = 8;
                else if (data_0209f2f8 == kAltLevelID)
                    speedMul = 6;
            }
        }
        {
            /* target = speedMul * 6 units * speedScale (fix12 multiply, rounded);
               the step is speedMul * 0.1 units a frame. */
            int acc = speedMul * 0x6000;
            ApproachLinear(this->mHorzSpeed,
                (int)(((long long)acc * speedScale + 0x800) >> 12), speedMul * 0x19a);
        }
        ApproachLinear(this->mPrevAngleY, this->mTargetAngleY, kTurnStep);
        if (this->mModelAnim.file == (BCA_File *)ANIM_RUN) {
            this->mModelAnim.speed = this->mHorzSpeed >> 3;
        } else {
            this->mModelAnim.speed = 0x1000;
            if (this->mModelAnim.Finished() != 0 &&
                (this->mModelAnim.file == (BCA_File *)ANIM_RUN_START || this->mModelAnim.file == (BCA_File *)ANIM_LAND))
                SET_ANIM(this, ANIM_RUN, kAnimLoop);
        }
        if (progress == PATH_NODE_REACHED && (this->mCurPathPt == this->mPathPtToJumpAt1 || this->mCurPathPt == this->mPathPtToJumpAt2)) {
            func_ov062_02119954();
            return;
        }
        if (status != 0) {
            if (status < 0)
                this->mHorzSpeed = 0;
            if (status == 0)
                return;
            func_ov062_02119954();
            return;
        }
        if (this->mWithMeshClsn.IsOnGround() != 0)
            return;
        /* Walked off an edge: kick up 10 units a frame, keep 0xd00/0x1000 (81%)
           of his speed, and go to the in-the-air step. */
        SET_ANIM(this, ANIM_JUMP, kAnimPlayOnce);
        this->mVertSpeed = 0xa000;
        this->mHorzSpeed = (int)(((long long)this->mHorzSpeed * 0xd00 + 0x800) >> 12);
        this->mStep = 3;
        return;
    case 3:
        if (func_ov062_02119af0() == PATH_DONE) {
            daRFlag_c *flag;
            this->mState = daRNk_c::STATE_PULL_UP;
            this->mStep = 0;
            data_0209d4c8.StopTimer();
            flag = (daRFlag_c *)dActor_c::FindWithID(this->mFlagID);
            this->mPlayerWon = (flag->mHasTouchedFlag != 0) ? 1 : 0;
            flag->mHasTouchedFlag = 1;
            SET_ANIM(this, ANIM_LAND, kAnimPlayOnce);
            return;
        }
        ApproachLinear(this->mPrevAngleY, this->mTargetAngleY, kTurnStep);
        if (this->mWithMeshClsn.IsOnGround() == 0)
            return;
        this->mStep = 2;
        this->mVertAccel = -0x2000;     /* InitResources' gravity again */
        SET_ANIM(this, ANIM_LAND, kAnimPlayOnce);
        this->mModelAnim.currFrame = 0;
        tmp[0] = this->mPosX;
        tmp[1] = this->mPosY;
        tmp[2] = this->mPosZ;
        tmp[1] = this->mPosY + kDustHeight;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kPtclLandingDust, this->mPosX,
            this->mPosY + kDustHeight, this->mPosZ);
        return;
    default:
        return;
    }
}

// @symbol _ZN7daRNk_c19func_ov062_0211a168Ev
/* STATE_PULL_UP: ease the speed toward 3 units a frame (1 unit a frame
   step); when the current animation is about to hit its last frame, fix the
   speed at 3 units, play ANIM_BRAKE once and go to STATE_STOP. */
void daRNk_c::func_ov062_0211a168(){
  ApproachLinear(this->mHorzSpeed, 0x3000, 0x1000);
  if (this->mModelAnim.WillHitFrame(
        (unsigned short)(this->mModelAnim.GetFrameCount() - 1)) == 0) return;
  this->mState = daRNk_c::STATE_STOP;
  this->mHorzSpeed = 0x3000;
  this->mStep = 0;
  SET_ANIM(this, ANIM_BRAKE, kAnimPlayOnce);
}

// @symbol _ZN7daRNk_c19func_ov062_0211a0f0Ev
/* STATE_STOP: brake to a standstill (4 units a frame step); when the brake
   animation finishes, go back to the idle animation and on to
   STATE_POST_RACE_TALK. */
void daRNk_c::func_ov062_0211a0f0()
{
    ApproachLinear(this->mHorzSpeed, 0, 0x4000);
    if (!this->mModelAnim.Finished()) return;
    SET_ANIM(this, ANIM_IDLE, kAnimLoop);
    this->mState = daRNk_c::STATE_POST_RACE_TALK;
    this->mStep = 0;
    this->mStateTimer = 0;
}

// @symbol _ZN7daRNk_c19func_ov062_02119be0Ev
/* STATE_POST_RACE_TALK: the result talk, and then chatter forever. mStateTimer
 * counts down every frame and holds back step 0.
 *   step 0  once the timer is 0 and the player is within 400 units and agrees
 *           to a talk, turn toward them; the first time through, also stop the
 *           race music, play sound 0x4d in 2D and mark the race finished
 *   step 1  turn toward the player (0x800 a frame)
 *   step 2  choose the result message (see the MSG_RESULT_* names: who got to
 *           the flag first, a cannon, a player who is not Mario) and show it
 *   step 3  when the talk ends, go to idle with a 60-frame timer. If the
 *           player won, spawn the star 100 units above Koopa. Otherwise a
 *           player who is not Mario restarts the talk (step 0); anyone else
 *           moves on to step 4
 *   step 4  later visits: when the player is within 400 units and agrees to
 *           another talk, go on
 *   steps 5-8  turn to face the player, show a chatter message, and return
 *           to step 4 when it ends */
void daRNk_c::func_ov062_02119be0()
{
    Vector3 playerPos;
    Vector3 msgPos;
    Vector3 starPos;
    unsigned int msg;
    unsigned short t;
    unsigned short* tp;
    int x;
    int y;
    int z;

    tp = (unsigned short*)&this->mStateTimer;
    t = *tp;
    if (t != 0) {
        t--;
        *tp = t;
    }

    switch (this->mStep) {
    case 0:
        if (*(unsigned short*)&this->mStateTimer != 0)
            return;
        {
            Vector3* pp = (Vector3 *)&this->mPlayer->mPosX;
            playerPos.x = pp->x;
            playerPos.y = pp->y;
            playerPos.z = pp->z;
        }
        if (Vec3_Dist((Vector3*)&this->mPosX, &playerPos) >= kChatDist)
            return;
        if (this->mPlayer->StartTalk(*this, 1) == 0)
            return;
        this->mTargetAngleY = Vec3_HorzAngle((Vector3*)&this->mPosX, &playerPos);
        this->mStep++;
        SET_ANIM(this, ANIM_TALK, kAnimLoop);
        if (this->mHasFinished == 0) {
            _ZN5Sound22StopLoadedMusic_Layer2Ev();
            func_0201277c(kSfx4D);
            this->mIsRacing = 0;
            this->mHasFinished = 1;
        }
        this->mRestartTalk = 0;
        return;
    case 1:
        if (ApproachLinear(this->mPrevAngleY, this->mTargetAngleY, kTurnStep) != 0)
            this->mStep++;
        this->mAngleY = this->mPrevAngleY;
        return;
    case 2:
        if (this->mPlayer->param1 == 0) {
            if (this->mPlayerWon == 0) {
                msg = MSG_RESULT_KOOPA_FIRST;
            } else if (this->mHasPlayerUsedCannon != 0) {
                this->mPlayerWon = 0;
                msg = MSG_RESULT_CANNON;
            } else if (data_0209f2f8 == kAltLevelID) {
                msg = MSG_RESULT_ALT_LEVEL;
            } else {
                msg = MSG_RESULT_PLAYER_FIRST;
            }
        } else {
            this->mPlayerWon = 0;
            this->mRestartTalk = 1;
            msg = MSG_RESULT_NOT_MARIO;
        }
        x = this->mPlayer->GetTalkState();
        if (x != 0)
            return;
        x = this->mPosX;
        z = this->mPosZ;
        y = this->mPosY + kMsgAnchorHeight;
        msgPos.x = x;
        msgPos.y = y;
        msgPos.z = z;
        if (this->mPlayer->ShowMessage(*this, msg, &msgPos, 0, 0) != 0)
            this->mStep++;
        return;
    case 3:
        if (this->mPlayer->GetTalkState() != 0xFFFFFFFF)
            return;
        this->mStep++;
        SET_ANIM(this, ANIM_IDLE, kAnimLoop);
        this->mStateTimer = 0x3c;
        if (this->mPlayerWon != 0) {
            starPos.x = this->mPosX;
            starPos.y = this->mPosY;
            starPos.z = this->mPosZ;
            starPos.y += kStarSpawnHeight;
            this->UntrackAndSpawnStar(this->mTrackedStar, this->mStarID, starPos, 4);
            return;
        }
        if (this->mRestartTalk == 1) {
            this->mRestartTalk = 0;
            this->mStep = 0;
        }
        return;
    case 4:
        {
            Vector3* pp = (Vector3 *)&this->mPlayer->mPosX;
            playerPos.x = pp->x;
            playerPos.y = pp->y;
            playerPos.z = pp->z;
        }
        if (Vec3_Dist((Vector3*)&this->mPosX, &playerPos) >= kChatDist)
            return;
        if (this->mPlayer->StartTalk(*this, 0) != 0)
            this->mStep++;
        return;
    case 5:
        if (this->mPlayer->GetTalkState() != 0)
            return;
        this->mTargetAngleY = Vec3_HorzAngle((Vector3*)&this->mPosX, (Vector3*)&this->mPlayer->mPosX);
        SET_ANIM(this, ANIM_TALK, kAnimLoop);
        this->mStep++;
        return;
    case 6:
        if (ApproachLinear(this->mPrevAngleY, this->mTargetAngleY, kTurnStep) != 0)
            this->mStep++;
        this->mAngleY = this->mPrevAngleY;
        return;
    case 7:
        {
            Player* p = this->mPlayer;
            if (p->param1 == 0)
                msg = MSG_CHAT_MARIO;
            else
                msg = MSG_CHAT_NOT_MARIO;
            x = p->GetTalkState();
            if (x != 0)
                return;
        }
        x = this->mPosX;
        z = this->mPosZ;
        y = this->mPosY + kMsgAnchorHeight;
        msgPos.x = x;
        msgPos.y = y;
        msgPos.z = z;
        if (this->mPlayer->ShowMessage(*this, msg, &msgPos, 0, 0) != 0)
            this->mStep++;
        return;
    case 8:
        if (this->mPlayer->GetTalkState() != 0xFFFFFFFF)
            return;
        this->mStep = 4;
        SET_ANIM(this, ANIM_IDLE, kAnimLoop);
        return;
    }
}

// @symbol _ZN7daRNk_c19func_ov062_02119af0Ev
/* The path follower. Aims mTargetAngleY at mPathTarget (x and z only). Koopa
   has reached the node when he has run past it (the dot product of the vector
   from the previous node to the target with the vector from him to the target
   is no longer positive, in whole units) or is within half a frame's run of
   it; he then makes the target the previous node, steps to the next index
   (wrapping to 0 at the end of the path) and returns PATH_NODE_REACHED, or
   PATH_DONE when the index wrapped. Returns PATH_TRAVELLING otherwise. */
int daRNk_c::func_ov062_02119af0() {
    daRNk_c *p = this;
    int dxc;
    int dzc;
    int dx;
    int dz;
    int targetX;
    int posX;
    int prevX;
    int targetZ;
    int posZ;
    int prevZ;

    targetX = p->mPathTarget.x;
    posX = p->mPosX;
    prevX = p->mPrevPathPt.x;
    dx = targetX - posX;
    dxc = targetX - prevX;  /* computed here, not after dz: first-use order drives the allocator under propagation (the standalone .c used #pragma opt_propagation off instead, which is file-final in a merged TU) */
    targetZ = p->mPathTarget.z;
    posZ = p->mPosZ;
    prevZ = p->mPrevPathPt.z;
    dz = targetZ - posZ;
    dzc = targetZ - prevZ;
    p->mTargetAngleY = (short)_ZN4cstd5atan2E5Fix12IiES1_(dx, dz);
    int dot = (dxc >> 0xc) * (dx >> 0xc) + (dzc >> 0xc) * (dz >> 0xc);
    if (dot <= 0 || Vec3_Dist((Vector3 *)&p->mPosX, (Vector3 *)&p->mPathTarget) < (p->mHorzSpeed >> 1)) {
        int v = p->mPathTarget.x;
        int *cnt = &p->mCurPathPt;
        p->mPrevPathPt.x = v;
        p->mPrevPathPt.y = p->mPathTarget.y;
        p->mPrevPathPt.z = p->mPathTarget.z;
        *cnt = *cnt + 1;
        if (p->mCurPathPt >= p->mNumPathPts) {
            p->mCurPathPt = 0;
        }
        int idx = p->mCurPathPt;
        if (idx == 0) return PATH_DONE;
        p->mPathPtr.GetNode(p->mPathTarget, (unsigned)idx);
        return PATH_NODE_REACHED;
    }
    return PATH_TRAVELLING;
}

// @symbol _ZN7daRNk_c19func_ov062_021199acEv
/* React to the closest rolling iron ball, if it is kind 2 (a path follower;
   daIbl_c.h says kinds 2 and 4 follow paths, Koopa reacts to kind 2 only). With the angle from Koopa's heading to the ball (r) and
   the ball's speed along Koopa's heading (fixed1 = ball speed times the cosine
   of the heading difference, from SINE_TABLE):
     ball ahead (r < 0x4000, a quarter turn) and inside 400 units:
       BALL_AHEAD_JUMP if it is moving along his heading slower than 0.7 (0xb33
       / 0x1000) of his own speed; otherwise he slows by 2 units and
       carries on (BALL_NONE)
     ball more than a quarter turn off his heading (the rear half) and
     inside 300 units:
       BALL_BEHIND_STOP_AND_JUMP if it is moving along his heading faster than
       he is; else BALL_NONE */
int daRNk_c::func_ov062_021199ac()
{
    daIbl_c *other;
    s16 angle;
    s32 dist;
    s16 selfAngle;
    s16 otherAngle;
    u16 idx;
    s16 tableVal;
    s32 otherK;
    s32 fixed1;
    s32 r;

    other = (daIbl_c *)this->ClosestWithActorID(kIronBallActorId);
    if (other == 0)
        goto ret0;
    if (other->mVariant != 2)
        goto ret0;

    angle = Vec3_HorzAngle((const struct Vector3 *)&this->mPosX, (const struct Vector3 *)&other->mPosX);
    dist = Vec3_Dist((const struct Vector3 *)&this->mPosX, (const struct Vector3 *)&other->mPosX);
    selfAngle = this->mPrevAngleY;
    otherAngle = other->mPrevAngleY;
    idx = (u16)(s16)(otherAngle - selfAngle);
    tableVal = data_02082214[(idx >> 4) * 2 + 1];
    otherK = other->mHorzSpeed;
    fixed1 = (s32)(((long long)otherK * tableVal + 0x800) >> 12);

    r = this->GetSubtraction(selfAngle, angle);
    if (r < 0x4000) {
        s32 selfK;
        s32 fixed2;
        if (dist >= kBallAheadRange)
            goto ret0;
        selfK = this->mHorzSpeed;
        fixed2 = (s32)(((long long)selfK * 0xb33 + 0x800) >> 12);
        if (fixed1 < fixed2)
            return BALL_AHEAD_JUMP;
        this->mHorzSpeed -= 0x2000;
        goto ret0;
    } else {
        if (dist >= kBallBehindRange)
            goto ret0;
        if (fixed1 > this->mHorzSpeed)
            return BALL_BEHIND_STOP_AND_JUMP;
        goto ret0;
    }

ret0:
    return BALL_NONE;
}

// @symbol _ZN7daRNk_c19func_ov062_02119954Ev
/* Start a jump: ANIM_JUMP, 45 units a frame up, gravity 4 units a frame
   squared (InitResources' is 2), and the in-the-air step of STATE_RACE. */
void daRNk_c::func_ov062_02119954()
{
    SET_ANIM(this, ANIM_JUMP, kAnimPlayOnce);
    this->mVertSpeed = 0x2d000;
    this->mVertAccel = -0x4000;
    this->mStep = 3;
}

// @symbol _ZN7daRNk_c19func_ov062_02119800Ev
/* Footsteps. The run animation's whole-frame number (currFrame >> 12) says
   which foot is down: on frames 2..8 and 19..25 play the footstep sound and
   a dust puff 30 units to one side (opposite sides in the two ranges) and 30
   units above his position, once; mFootstepDone holds the latch until the
   animation leaves both ranges. */
void daRNk_c::func_ov062_02119800()
{
    volatile int stack[3];
    unsigned int kind = ((unsigned int)(this->mModelAnim.currFrame << 4)) >> 16;
    short ang;
    int x, y, z;

    if (kind < 2)
        goto check_hi;
    if (kind <= 8)
        goto body;
check_hi:
    if (kind < 0x13)
        goto reset;
    if (kind > 0x19)
        goto reset;

body:
    if (this->mFootstepDone != 0)
        return;
    func_0201267c(kSfxFootstep, &this->mCamSpacePosX);
    this->mFootstepDone = 1;

    ang = this->mAngleY;
    x = this->mPosX;
    /* Keep the comparison at this load boundary: the direct assignment
       moves six instructions under 2004/b56 (see the pinned experiment). */
    stack[0] = (kind > 8) ? x : x;
    y = this->mPosY;
    ang = (short)(ang + 0x4000);    /* a quarter turn: the sideways direction */
    stack[1] = y;
    z = this->mPosZ;
    stack[2] = z;
    stack[1] = y + 0x1e000;

    if (kind > 8)
        goto add_path;

    {
        unsigned short u = (unsigned short)ang;
        int n = (int)(u >> 4);
        short sinA = data_02082214[n * 2];
        short cosA = data_02082214[n * 2 + 1];
        short k = 0x1e;
        stack[0] = x - (int)(sinA * k);
        stack[2] = z - (int)(cosA * k);
    }
    goto do_new;

add_path:
    {
        unsigned short u = (unsigned short)ang;
        int n = (int)(u >> 4);
        short sinA = data_02082214[n * 2];
        short cosA = data_02082214[n * 2 + 1];
        short k = 0x1e;
        stack[0] = (int)(sinA * k) + x;
        stack[2] = (int)(cosA * k) + z;
    }
do_new:
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kPtclFootstepDust, (int)stack[0], (int)stack[1], (int)stack[2]);
    return;

reset:
    this->mFootstepDone = 0;
}

/* D0 is the DELETING destructor: destroy through this class (dEnemyBase_c
 * chain) then return the object to its heap via an inline operator delete.
 * Both variants are emitted from the single inline destructor in
 * daRNk_c.h (class-form skill): D1 then D0 in ROM order, no leaf D2. */

/* D1 is emitted from the inline destructor in daRNk_c.h alongside D0
 * alongside D0. Members are destroyed in reverse declaration
 * order, then dEnemyBase_c::~dEnemyBase_c. */
