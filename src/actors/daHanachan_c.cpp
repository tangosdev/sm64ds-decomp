//cpp
/* Production translation unit for ov034/daHanachan_c, the Wiggler.
 * 34 function(s), .text 0x021111a0..0x021136a4.
 *
 * NAME: _ZTS12daHanachan_c is "12daHanachan_c" at ov034 0x02114478; _ZTI at
 * 0x0211445c reads [__si_class_type_info, that string, _ZTI12dEnemyBase_c].
 * The vtable's address point is 0x021144bc; the word before it (0x021144b8)
 * is that _ZTI. The tree previously called the class Wiggler (coined).
 *
 * D1, D0 and four virtual methods, with 28 non-virtual members between
 * them: 22 state functions, SetState, ExecState and four helpers. The run is
 * gap-free and this is the only class in it. daHanachan_c_classInit at
 * 0x021136a4 is the next function and is not part of this translation unit.
 *
 * #pragma defer_codegen off emits .text in source order, so the file is
 * ROM-ascending. One out-of-line destructor is the key function, so this TU
 * emits _ZTV, _ZTI and _ZTS: D1 (0x021111a0), D0 (0x021112b0), then a D2 the
 * cartridge has no home for (manifest: deadstrip).
 */

#pragma defer_codegen off

#include "daHanachan_c.h"
#include "common.h"
#include "decl_Player.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "dBgCh_Gnd.h"
#include "Message.h"

void ApproachLinear(int &value, int target, int step);
void ApproachLinear(short &value, short target, short step);

/* The state table, filled in by __sinit_ov034_021138ec. */
extern daHanachan_c::State data_ov034_02114538[];

extern "C" {
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int flags, int speed, unsigned int startFrame);
extern void **data_ov034_02113888[];
extern "C" int DecIfAbove0_Byte(void*);
extern void **data_ov034_0211389c[];
extern void **data_ov034_02113860[];
extern "C" int Math_Function_0203b14c(void *base, int a, int b, int c, int d);
extern "C" int data_ov034_021138c4[];
extern void func_0201267c(int, void *);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern short data_ov034_02113820[];
extern void **data_ov034_021138b0[];
extern void Vec3_Sub(void *out, void *a, void *b);
extern int LenVec3(void *v);
extern int Vec3_HorzDist(void *a, void *b);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int data_ov034_02114488[];
void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned);
void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern char data_020a0e68[];
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void MulVec3Mat4x3(void *in, void *m, void *out);
extern "C" void Vec3_Add(void *out, void *a, void *b);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, struct Vector3 *v, unsigned int b, int c, unsigned int d, unsigned int e, unsigned int f);
extern int NormalizeVec3IfNonZero(void *v);
extern void Vec3_MulScalarInPlace(void *v, int s);
extern SharedFilePtr *data_ov034_02113838[];
extern SharedFilePtr *data_ov034_0211384c[];
extern void **data_ov034_02113874[];
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(s32 x, s32 y, s32 z);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
u32 a, u32 b, s32 x, s32 y, s32 z, const void *v, void *cb);
extern int func_ov002_020c51d0(void *, void *);
extern unsigned char data_ov034_0211433c[];
extern s32 data_ov034_021138d8[];
extern void func_020092c4(void *arg0, void *out, void *target);
extern u32 data_ov034_02113828[];
extern s16 data_02082214[];
extern void *data_0209f318;
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *ts, void *btp, int i, int fix, u32 j);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *pos, int fix, u32 a, u32 b, u32 cc);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int fix1, int fix2, void *v, int t);
}

// @symbol _ZN12daHanachan_cD1Ev
// @symbol _ZN12daHanachan_cD0Ev
/* Destroys the eight per-segment arrays in reverse declaration order, then
 * the dEnemyBase_c base. D0 frees through the inherited operator delete. */
daHanachan_c::~daHanachan_c()
{
}

/* DAMAGE_START: stagger the animation change down the body, then MOVE. */
// @symbol _ZN12daHanachan_c20StateDamageStartMainEv
void daHanachan_c::StateDamageStartMain()
{
    int i;
    int k;
    ModelAnim *anim;

    anim = &mModelAnims[0];
    anim = anim + 2;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim++) {
        if (mStateTimer == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_02113888[i][1], 0x40000000, 0x1000, 0);
    }
    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            if (mModelAnims[idx].Finished())
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[idx], data_ov034_02113874[idx][1], 0, 0x1000, 0);
        }
    }
    {
        int flag;
        int i;
        flag = 1;
        for (i = 0; i < 5; i++) {
            if (mModelAnims[i].file != data_ov034_02113874[i][1])
                flag = 0;
        }
        if (flag == 1)
            SetState(4);
    }
    mStateTimer++;
    if (mStateTimer > 12)
        mStateTimer = 13;
}

/* DAMAGE_START entry. */
// @symbol _ZN12daHanachan_c20StateDamageStartInitEv
void daHanachan_c::StateDamageStartInit()
{
    mStateTimer = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[0], data_ov034_02113888[0][1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[1], data_ov034_02113888[1][1], 0x40000000, 0x1000, 0);
}

/* DAMAGE_WAIT: wait out the timer, then DAMAGE_START. */
// @symbol _ZN12daHanachan_c19StateDamageWaitMainEv
void daHanachan_c::StateDamageWaitMain()
{
    if (DecIfAbove0_Byte(&mStateTimer))
        return;
    SetState(10);
}

/* DAMAGE_WAIT entry: 90 frames. */
// @symbol _ZN12daHanachan_c19StateDamageWaitInitEv
void daHanachan_c::StateDamageWaitInit()
{
    mStateTimer = 90;
}

/* DAMAGE_STOP: brake, stagger the animation change down the body, then DAMAGE_WAIT. */
// @symbol _ZN12daHanachan_c19StateDamageStopMainEv
void daHanachan_c::StateDamageStopMain()
{
    int i;
    int k;
    ModelAnim *anim;

    ApproachLinear(mHorzSpeed, 0, 0x1000);

    anim = &mModelAnims[0];
    anim = anim + 2;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim++) {
        if (mStateTimer == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_0211389c[i][1], 0x40000000, 0x1000, 0);
    }

    if (mModelAnims[4].Finished()) {
        int i;
        for (i = 0; i < 5; i++)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[i], data_ov034_02113860[i][1], 0, 0x1000, 0);
    }

    {
        int flag;
        int i;
        flag = 1;
        for (i = 0; i < 5; i++) {
            if (mModelAnims[i].file != data_ov034_02113860[i][1])
                flag = 0;
        }
        if (flag == 1)
            SetState(9);
    }

    mStateTimer++;
    if (mStateTimer > 12)
        mStateTimer = 13;
}

/* DAMAGE_STOP entry. */
// @symbol _ZN12daHanachan_c19StateDamageStopInitEv
void daHanachan_c::StateDamageStopInit()
{
    mStateTimer = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[0], data_ov034_0211389c[0][1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[1], data_ov034_0211389c[1][1], 0x40000000, 0x1000, 0);
}

/* DEAD: poof one segment every five frames while the scales settle, then spawn the star and go to MOVE_START. */
// @symbol _ZN12daHanachan_c13StateDeadMainEv
void daHanachan_c::StateDeadMain()
{
    /* starPos is written and never read; removing it changes StateDeadMain's bytes. */
    Vector3 starPos;
    Vector3 v;
    int a, b;
    int i;

    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        unsigned int idx = mDeathSegment;
        if (idx < 5) {
            Vector3 *vp = &mSegmentPos[idx];
            v.x = vp->x;
            v.y = vp->y;
            v.z = vp->z;
            PoofDustAt(v);
            mDeathSegment++;
            mStateTimer = 5;
        }
    }

    for (i = 0; i < 5; i++) {
        if (i > (int)mDeathSegment)
            break;
        mSegmentScale[i].z = mSegmentScale[i].x;
        mSegmentScale[i].y = mSegmentScale[i].z;
        a = Math_Function_0203b14c(&mSegmentSpacing[i], data_ov034_021138c4[i] >> 1, 0x800, 0x20000, 0x200) & 0xff;
        b = Math_Function_0203b14c(&mSegmentScale[i], 0x800, 0x800, 0x2000, 0x200) & 0xff;
    }

    if (a != 0)
        return;
    if (b != 0)
        return;

    if (mDeathDelay < 5) {
        mDeathDelay++;
        return;
    }

    {
        int px = mPosX;
        int pz = mPosZ;
        int py = mPosY + 0x64000;
        starPos.x = px;
        starPos.y = py;
        starPos.z = pz;
    }
    UntrackAndSpawnStar((s8 &)mStarTrackID, mStarID, *(const Vector3 *)&mPosX, 4);
    _ZN5Sound22StopLoadedMusic_Layer3Ev();
    SetState(3);
}

/* DEAD entry. */
// @symbol _ZN12daHanachan_c13StateDeadInitEv
void daHanachan_c::StateDeadInit()
{
    mDeathDelay = 0;
    mDeathSegment = 0;
    mDefeated = 1;
    mStateTimer = 5;
    func_0201267c(287, &mCamSpacePosX);
}

/* DAMAGETALK: once the message is done, MOVE_START while health remains, else DEAD. */
// @symbol _ZN12daHanachan_c19StateDamageTalkMainEv
void daHanachan_c::StateDamageTalkMain()
{
    ApproachLinear(mHorzSpeed, 0, 0x1000);
    if (mPlayer->GetTalkState() != 2)
        return;
    if (mHealth > 1) {
        SetState(3);
        return;
    }
    SetState(7);
}

/* DAMAGETALK entry. */
// @symbol _ZN12daHanachan_c19StateDamageTalkInitEv
void daHanachan_c::StateDamageTalkInit()
{
    int i;
    for (i = 0; i < 5; i++)
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[i], data_ov034_02113860[i][1], 0, 0x1000, 0);
}

/* DAMAGE: stop, face the player and start the hurt message, then DAMAGETALK. */
// @symbol _ZN12daHanachan_c15StateDamageMainEv
void daHanachan_c::StateDamageMain()
{
    Vector3 st;
    Player *talk;

    ApproachLinear(mHorzSpeed, 0, 0x1000);
    mTargetAngleY = HorzAngleToCPlayer();

    talk = mPlayer;
    st.x = mSegmentPos[0].x;
    st.y = mSegmentPos[0].y;
    st.z = mSegmentPos[0].z;
    st.y = st.y + 0x8c000;
    if (!talk->StartTalk(*this, true))
        return;

    _ZN6Camera9SetFlag_3Ev(data_0209f318);
    mDriveCamera = 1;

    ApproachLinear(mPlayer->mAngleY, Vec3_HorzAngle(&mSegmentPos[0], &mPlayer->mPosX), 0x100);

    func_ov002_020c51d0(talk, &st);

    if (!mModelAnims[0].Finished())
        return;

    {
        unsigned int idx = (4 - mHealth) & 0xff;
        if (idx >= 4)
            idx = 0;
        if (!talk->ShowMessage(*this, data_ov034_02113820[idx], &st, 1, 0))
            return;
    }

    func_0201267c(0x11b, &mCamSpacePosX);
    SetState(6);
}

#pragma opt_strength_reduction off
/* DAMAGE entry: hurt animation, flag 4 on the first cylinder set, one health lost. */
// @symbol _ZN12daHanachan_c15StateDamageInitEv
void daHanachan_c::StateDamageInit()
{
    int i;
    ModelAnim *anim = &mModelAnims[0];
    mParticle = 0;
    for (i = 0; i < 5; i++) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_021138b0[i][1], 0x40000000, 0x1000, 0);
        mdCc_cs1[i].flags |= 4;
        anim++;
    }
    if (mHealth > 1)
        mHealth--;
}

#pragma opt_strength_reduction on
/* MOVE: wander and keep near home, checking player contact. Once defeated, fall away when a player comes near. */
// @symbol _ZN12daHanachan_c13StateMoveMainEv
void daHanachan_c::StateMoveMain()
{
    int len;
    int r4;
    int v[3];

    Vec3_Sub(v, &mPosX, &mHomePos);
    len = LenVec3(v);

    if (mDefeated != 0 && mFalling == 0) {
        Player *cp = ClosestPlayer();
        if (Vec3_HorzDist(&mPosX, &cp->mPosX) < 0xc8000) {
            mFalling = 1;
            mVertAccel = -0x1000;
            mTerminalVelocity = -0x3c000;
            mVertSpeed = 0;
        }
    }

    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        r4 = RandomIntInternal(&data_0209e650);
        {
            unsigned char db = mHealth;
            unsigned int hi;
            if (db <= 3 || (((hi = (unsigned int)r4 >> 0x1b)) & 1) == 0) {
                mTargetAngleY = HorzAngleToCPlayer();
            } else {
                mTargetAngleY = (short)((hi & 0xf) << 0xc);
            }
        }
        mStateTimer = (unsigned char)((unsigned int)(r4 + 0x1e) >> 0x1b);
        mStateTimer = (unsigned char)(mStateTimer - (4 - mHealth) * 0x1e);
        if (mStateTimer >= 0x40)
            mStateTimer = 0;
    }

    if (mWithMeshClsn.IsOnWall() != 0 || len > 0x5dc000) {
        r4 = RandomIntInternal(&data_0209e650);
        mTargetAngleY = Vec3_HorzAngle(&mPosX, &mHomePos);
        mStateTimer = (unsigned char)((unsigned int)(r4 + 0x1e) >> 0x1b);
        mStateTimer = (unsigned char)(mStateTimer - (4 - mHealth) * 0x1e);
        if (mStateTimer >= 0x40)
            mStateTimer = 0;
    }

    {
        unsigned int idx = (unsigned int)((4 - mHealth) & 0xff);
        if (idx >= 4)
            idx = 0;
        ApproachLinear(mHorzSpeed, data_ov034_02114488[idx], 0x1000);
    }

    if (mDefeated != 0)
        return;
    HandlePlayerHits();
}

/* MOVE entry. */
// @symbol _ZN12daHanachan_c13StateMoveInitEv
void daHanachan_c::StateMoveInit()
{
    mPlayer = 0;
    if (mDefeated != 0)
        mStateTimer = 0x96;
}

/* MOVE_START: stagger the walk animation down the body; when the talk ends, hop by remaining health and enter MOVE. */
// @symbol _ZN12daHanachan_c18StateMoveStartMainEv
void daHanachan_c::StateMoveStartMain()
{
    int i;
    int k;
    ModelAnim *anim;
    unsigned health;
    Player *player;

    anim = &mModelAnims[0];
    anim = anim + 2;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim++) {
        if (mStateTimer == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_02113888[i][1], 0x40000000, 0x1000, 0);
    }

    {
        int idx;
        for (idx = 0; idx < 5; idx++) {
            if (mModelAnims[idx].Finished())
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[idx], data_ov034_02113874[idx][1], 0, 0x1000, 0);
        }
    }

    player = mPlayer;
    {
        int flag;
        int i;
        flag = 1;
        for (i = 0; i < 5; i++) {
            if (mModelAnims[i].file != data_ov034_02113874[i][1])
                flag = 0;
        }
        if (flag == 1) {
            if (player->HasFinishedTalking()) {
                health = mHealth;
                if (health < 4) {
                    if (health > 1) {
                        mVertSpeed = 0x41000 - (((int)(health - 2) * 10) << 12);
                        mVertAccel = -0x4000;
                    }
                }
                *(int *)((char *)data_0209f318 + 0x154) &= ~8;
                mDriveCamera = 0;
                SetState(4);
            }
        }
    }

    mStateTimer++;
    if (mStateTimer > 12)
        mStateTimer = 13;
}

#pragma opt_strength_reduction off
/* MOVE_START entry: clear flag 4 on the first cylinder set. */
// @symbol _ZN12daHanachan_c18StateMoveStartInitEv
void daHanachan_c::StateMoveStartInit()
{
    int i;
    for (i = 0; i < 5; i++)
        mdCc_cs1[i].flags &= ~4;
    mStateTimer = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[0], data_ov034_02113888[0][1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[1], data_ov034_02113888[1][1], 0x40000000, 0x1000, 0);
}

#pragma opt_strength_reduction on
/* MOVE_STOP: when the intro talk ends, start the music and enter MOVE_START. */
// @symbol _ZN12daHanachan_c17StateMoveStopMainEv
void daHanachan_c::StateMoveStopMain()
{
    int i, k;
    ModelAnim *anim;
    Player *player;

    ApproachLinear(mHorzSpeed, 0, 0x1000);

    if (mStateTimer == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[0], data_ov034_0211389c[0][1], 0x40000000, 0x1000, 0);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[1], data_ov034_0211389c[1][1], 0x40000000, 0x1000, 0);
    }

    anim = &mModelAnims[0];
    anim = anim + 2;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim++) {
        if (mStateTimer == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_0211389c[i][1], 0x40000000, 0x1000, 0);
    }

    if (mModelAnims[4].Finished()) {
        int i;
        for (i = 0; i < 5; i++)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[i], data_ov034_02113860[i][1], 0, 0x1000, 0);
    }

    player = mPlayer;

    {
        int flag;
        int i;
        flag = 1;
        for (i = 0; i < 5; i++) {
            if (mModelAnims[i].file != data_ov034_02113860[i][1])
                flag = 0;
        }
        if (flag == 1) {
            if (player->GetTalkState() == 2) {
                _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
                Message::EndTalk();
                SetState(3);
            }
        }
    }

    mStateTimer++;
    if (mStateTimer > 12)
        mStateTimer = 13;
}

/* MOVE_STOP entry. */
// @symbol _ZN12daHanachan_c17StateMoveStopInitEv
void daHanachan_c::StateMoveStopInit()
{
    mStateTimer = 0;
    mIntroDone = 1;
}

/* DEMOCALL: show the intro message, then MOVE_STOP. */
// @symbol _ZN12daHanachan_c17StateDemoCallMainEv
void daHanachan_c::StateDemoCallMain()
{
    Vector3 v;
    Player *player = mPlayer;
    v.x = mSegmentPos[0].x;
    v.y = mSegmentPos[0].y;
    v.z = mSegmentPos[0].z;
    v.y = v.y + 0x28000;
    if (player->ShowMessage(*this, data_ov034_02113820[0], &v, 1, 0) == 0)
        return;
    Message::PrepareTalk();
    _ZN6Camera9SetFlag_3Ev(data_0209f318);
    mDriveCamera = 1;
    func_0201267c(0x11b, &mCamSpacePosX);
    SetState(2);
}

/* DEMOCALL entry: remember the closest player. */
// @symbol _ZN12daHanachan_c17StateDemoCallInitEv
void daHanachan_c::StateDemoCallInit()
{
    mPlayer = ClosestPlayer();
}

/* DEMOWAIT: wander near home until a player is 0x12c000 above, then DEMOCALL. */
// @symbol _ZN12daHanachan_c17StateDemoWaitMainEv
void daHanachan_c::StateDemoWaitMain()
{
    Vector3 v;
    int len;
    Player *player;

    Vec3_Sub(&v, &mPosX, &mHomePos);
    len = LenVec3(&v);
    player = ClosestPlayer();
    if (player->mPosY > mPosY + 0x12c000) {
        SetState(1);
        return;
    }

    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        int r = (unsigned)RandomIntInternal(&data_0209e650) >> 0x1b;
        if (r & 1) {
            mTargetAngleY = (short)((r & 0xf) << 0xc);
        } else {
            mTargetAngleY = HorzAngleToCPlayer();
        }
        mStateTimer = data_ov034_0211433c[mHealth];
    }

    if (mWithMeshClsn.IsOnWall() || len > 0x5dc000) {
        mTargetAngleY = Vec3_HorzAngle(&mPosX, &mHomePos);
        mStateTimer = data_ov034_0211433c[mHealth];
    }
    ApproachLinear(mHorzSpeed, data_ov034_02114488[0], 0x1000);
}

#pragma opt_strength_reduction off
/* DEMOWAIT entry: lay the body out in a line behind the head. */
// @symbol _ZN12daHanachan_c17StateDemoWaitInitEv
void daHanachan_c::StateDemoWaitInit()
{
    /* The loop steps two row pointers through this object by hand (strength
       reduction is off for this function) and reaches the segment arrays off them. */
    char *self = (char *)this;
    int seg;
    char *posRow;
    char *rotRow;
    int zero;
    Vector3 in;
    Vector3 out;
    Vector3 sum;

    seg = 0;
    mPlayer = 0;
    mHorzSpeed = data_ov034_02114488[0];
    posRow = self;
    rotRow = self;
    zero = seg;
    for (; seg < 5; seg++, posRow += sizeof(Vector3), rotRow += sizeof(Vector3s)) {
        if (seg == 0) {
            *(s32 *)(posRow + 0x3cc) = mPosX;
            *(s32 *)(posRow + 0x3d0) = mPosY;
            *(s32 *)(posRow + 0x3d4) = mPosZ;
            {
                short ang = HorzAngleToCPlayer();
                *(s16 *)(rotRow + 0x446) = ang;
                mPrevAngleY = *(s16 *)(rotRow + 0x446);
            }
        } else {
            int prev;
            int zval;
            zval = -mSegmentSpacing[seg];
            in.x = zero;
            in.y = zero;
            out.x = zero;
            out.y = zero;
            out.z = zero;
            in.z = zval;
            Matrix4x3_FromRotationY(data_020a0e68, (short)(mSegmentRot[0].y + 0x200));
            MulVec3Mat4x3(&in, data_020a0e68, &out);
            prev = seg - 1;
            Vec3_Add(&sum, &mSegmentPos[prev], &out);
            *(s32 *)(posRow + 0x3cc) = sum.x;
            *(s32 *)(posRow + 0x3d0) = sum.y;
            *(s32 *)(posRow + 0x3d4) = sum.z;
            *(s16 *)(rotRow + 0x446) = mSegmentRot[prev].y;
        }
    }
}

#pragma opt_strength_reduction on
/* Enters a state and runs its Init. */
// @symbol _ZN12daHanachan_c8SetStateEi
void daHanachan_c::SetState(int state)
{
    mState = state;
    (this->*data_ov034_02114538[mState].mInit)();
}

/* Runs the current state's Main. */
// @symbol _ZN12daHanachan_c9ExecStateEv
void daHanachan_c::ExecState()
{
    (this->*data_ov034_02114538[mState].mMain)();
}

/* True when the actor is falling onto the cylinder from above. */
// @symbol _ZN12daHanachan_c11IsStompedByEP10dCcAcPos_cP8dActor_c
int daHanachan_c::IsStompedBy(dCcAcPos_c *cyl, dActor_c *actor)
{
    volatile int pad[4];    /* the ROM reserves 0x10 bytes of stack it never touches */
    s32 pushY = cyl->pushback.y;
    s32 vy = actor->mVertSpeed;
    (void)&pad;
    if (vy < 0 && pushY < 0 && pushY > vy)
        return 1;
    return 0;
}

#pragma opt_strength_reduction off
/* Players stomping a segment bounce off it; the head also hurts on contact. The second cylinder set only hurts. */
// @symbol _ZN12daHanachan_c16HandlePlayerHitsEv
void daHanachan_c::HandlePlayerHits()
{
    int i;
    dActor_c *a;
    dCcAcPos_c *cyl;
    volatile Vector3 playerPos;    /* copied and never read; the ROM keeps the copy */
    Vector3 v;
    Vector3 hv;

    cyl = &mdCc_cs1[0];
    for (i = 0; i < 5; i++, cyl++) {
        unsigned int id;
        int isPlayer;
        u16 type;
        Vector3 *src;
        id = mdCc_cs1[i].otherOwner;
        if (id == 0)
            continue;
        a = dActor_c::FindWithID(id);
        if (a == 0)
            continue;
        type = a->actorID;
        isPlayer = type == 0xbf;
        if (isPlayer == 0)
            continue;
        src = (Vector3 *)&a->mPosX;
        playerPos.x = src->x;
        playerPos.y = src->y;
        playerPos.z = src->z;
        if (IsStompedBy(cyl, a) != 0) {
            _ZN6Player6BounceE5Fix12IiE(a, 0x28000);
            func_0201267c(0x11c, &mCamSpacePosX);
            if ((unsigned int)i > 1) {
                mSegmentBounceTimer[i] = 0x1e;
                return;
            }
            mSegmentBounceTimer[0] = 0x1e;
            mPlayer = (Player *)a;
            SetState(5);
            return;
        }
        if (i == 0) {
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, 2, 0xc000, 1, 0, 1);
            SetState(8);
        }
    }
    {
        int j;
        for (j = 0; j < 5; j++) {
            unsigned int id;
            int isPlayer;
            u16 type;
            id = mdCc_cs2[j].otherOwner;
            if (id == 0)
                continue;
            a = dActor_c::FindWithID(id);
            if (a == 0)
                continue;
            type = a->actorID;
            isPlayer = type == 0xbf;
            if (isPlayer == 0)
                continue;
            hv.x = mPosX;
            hv.y = mPosY;
            hv.z = mPosZ;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &hv, 0, 0x5000, 1, 0, 1);
        }
    }
}

#pragma opt_strength_reduction off
#pragma opt_common_subs off
/* Pulls each segment to its spacing behind the one ahead, sets it on the ground and moves its cylinders. */
// @symbol _ZN12daHanachan_c14UpdateSegmentsEv
void daHanachan_c::UpdateSegments()
{
    /* Strength reduction and CSE are off here. rotRow and posRow are row
       pointers into this object that the ROM loop steps by hand and addresses
       mSegmentRot and mSegmentPos from; typed pointers change the loop. */
    char *self = (char *)this;
    int seg;
    Vector3 *cur;
    char *rotRow;
    char *posRow;
    Vector3 diff;
    Vector3 castPos;
    Vector3 sum;

    cur = &mSegmentPos[0];
    cur = cur + 1;
    rotRow = self + 6;
    posRow = self + 0xc;
    seg = 1;

    for (; seg < 5; seg++, cur++, rotRow += 6, posRow += 0xc) {
        int off = (seg - 1) * 0xc;
        Vec3_Sub(&diff, cur, (char *)&mSegmentPos[0] + off);
        if (NormalizeVec3IfNonZero(&diff) != 0) {
            Vec3_MulScalarInPlace(&diff, mSegmentSpacing[seg]);
        }
        {
            char *prev = (char *)&mSegmentPos[0] + off;
            short ang = Vec3_HorzAngle(cur, prev);
            *(short *)(rotRow + 0x446) = ang;
            Vec3_Add(&sum, prev, &diff);
        }

        *(int *)(posRow + 0x3cc) = sum.x;
        *(int *)(posRow + 0x3d0) = sum.y;
        *(int *)(posRow + 0x3d4) = sum.z;
        mdCc_cs1[seg].pos.x = *(int *)(posRow + 0x3cc);
        mdCc_cs1[seg].pos.y = *(int *)(posRow + 0x3d0);
        mdCc_cs1[seg].pos.z = *(int *)(posRow + 0x3d4);
        mdCc_cs2[seg].pos.x = *(int *)(posRow + 0x3cc);
        mdCc_cs2[seg].pos.y = *(int *)(posRow + 0x3d0);
        mdCc_cs2[seg].pos.z = *(int *)(posRow + 0x3d4);

        if (mFalling == 0) {
            int py;
            dBgCh_Gnd ray;
            castPos.x = *(int *)(posRow + 0x3cc);
            py = *(int *)(posRow + 0x3d0);
            castPos.y = py;
            castPos.z = *(int *)(posRow + 0x3d4);
            castPos.y = py + 0x3c000;
            ray.SetObjAndPos(castPos, 0);
            if (ray.DetectClsn() != 0) {
                int clY = ray.clsnY;
                if (*(int *)(posRow + 0x3d0) <= clY)
                    *(int *)(posRow + 0x3d0) = clY;
            }
        }
    }
}

#pragma opt_common_subs on
#pragma opt_strength_reduction on
/* Copies each segment's rotation and position into its model matrix. */
// @symbol _ZN12daHanachan_c19UpdateSegmentModelsEv
void daHanachan_c::UpdateSegmentModels()
{
    /* Four separate induction pointers, one per stride, as the ROM loop keeps them;
       indexing the members instead folds the model pointer's +0x1c into its start. */
    char *self = (char *)this;
    int i;
    char *rot = self;
    char *model = (char *)&mModelAnims[0];
    char *pos = self;
    char *mtx = self;
    for (i = 0; i < 5; i++) {
        Matrix4x3_FromRotationY(model + 0x1c, *(s16 *)(rot + 0x446));
        *(s32 *)(mtx + 0x150) = *(s32 *)(pos + 0x3cc) >> 3;
        *(s32 *)(mtx + 0x154) = *(s32 *)(pos + 0x3d0) >> 3;
        *(s32 *)(mtx + 0x158) = *(s32 *)(pos + 0x3d4) >> 3;
        rot += sizeof(Vector3s);
        model += sizeof(ModelAnim);
        pos += sizeof(Vector3);
        mtx += sizeof(ModelAnim);
    }
}

// @symbol _ZN12daHanachan_c16CleanupResourcesEv
/* Releases all five entries of each of the seven file tables. */
int daHanachan_c::CleanupResources()
{
    int i;
    for (i = 0; i < 5; i++) {
        data_ov034_02113838[i]->Release();
        data_ov034_0211384c[i]->Release();
        ((SharedFilePtr *)data_ov034_02113860[i])->Release();
        ((SharedFilePtr *)data_ov034_02113874[i])->Release();
        ((SharedFilePtr *)data_ov034_02113888[i])->Release();
        ((SharedFilePtr *)data_ov034_0211389c[i])->Release();
        ((SharedFilePtr *)data_ov034_021138b0[i])->Release();
    }
    return 1;
}

/* Animates each segment's texture and draws the segment at its scale. */
// @symbol _ZN12daHanachan_c6RenderEv
int daHanachan_c::Render()
{
    int i = 0;
    ModelAnim *model = &mModelAnims[0];
    TextureSequence *tex = &mTextureSequences[0];
    Vector3 *scale = &mSegmentScale[0];
    for (; i < 5; i++) {
        tex->Update(model->data);
        model->Render(scale);
        model++;
        tex++;
        scale++;
    }
    return 1;
}

// @symbol _ZN12daHanachan_c8BehaviorEv
/* vtable slot 6. Runs the state, advances the segment animations and the
 * stomp bounce, then moves the body, its cylinders and, while mDriveCamera is
 * set, the camera. */
int daHanachan_c::Behavior()
{
    void **animFile;
    s32 i;
    Vector3 head;
    Vector3 part;
    Vector3 camA;
    Vector3 camB;
    Vector3 scratch;
    Vector3 t0;
    Vector3 t1;

    if (mFalling != 0) {
        if (mHomePos.y + mPosY < -0x1f4000)
            MarkForDestruction();
    }

    ExecState();

    if (mVertSpeed <= 0) {
        ApproachLinear(mSegmentRot[0].y, mTargetAngleY, (s16)((mHorzSpeed / 0x1000) * 0x1e));
    }

    if (mWithMeshClsn.JustHitGround() != 0) {
        mVertAccel = -0x1000;
        LandingDust(true);
    }

    if (mModelAnims[0].file == (BCA_File *)data_ov034_02113888[0][1] ||
        mModelAnims[0].file == (BCA_File *)data_ov034_02113874[0][1] ||
        mModelAnims[0].file == (BCA_File *)data_ov034_0211389c[0][1]) {
        if (mWithMeshClsn.IsOnGround() != 0) {
            if ((((u32)mModelAnims[0].currFrame << 4) >> 16) == 6) {
                if (mHealth == 2)
                    func_0201267c(0x10a, &mCamSpacePosX);
                else
                    func_0201267c(0x11d, &mCamSpacePosX);
            }
            if (mHealth == 2) {
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(mPosX, mPosY, mPosZ);
            }
        }
    }

    animFile = data_ov034_021138b0[0];
    if (mModelAnims[0].file == (BCA_File *)animFile[1] &&
        (((u32)mModelAnims[0].currFrame << 4) >> 16) == 0xf) {
        func_0201267c(0x11e, &mCamSpacePosX);
    }

    for (i = 0; i < 5; i++) {
        u16 n = 4 - mHealth;
        bool ok;
        if (n >= 3) n = 0;
        ok = false;     /* after the clamp: the ROM keeps two separate zero slots */
        if (n <= 2) ok = true;
        if (ok)
            mTextureSequences[i].currFrame = n << 12;

        if (mFalling == 0)
            mModelAnims[i].Advance();

        if (mDefeated == 0) {
            if (DecIfAbove0_Byte(&mSegmentBounceTimer[i]) != 0) {
                mSegmentBouncePhase[i] += 0x1200;
                mSegmentScale[i].y = ((s32)(((s64)data_02082214[(((u16)mSegmentBouncePhase[i] >> 4) << 1) + 1] * 0x3000 + 0x800) >> 12) + 0xc000) / 10;
            } else {
                ApproachLinear(mSegmentScale[i].y, 0x1000, 0x199);
            }
        }
    }

    if (mHealth == 4) {
        if (mIntroDone != 0)
            mTextureSequences[0].currFrame = 0x1000;
        else
            mTextureSequences[0].currFrame = 0;
    }

    mPrevAngleY = mSegmentRot[0].y;
    UpdatePos(0);

    if (mFalling == 0)
        UpdateWMClsn(mWithMeshClsn, 0);

    mSegmentPos[0].x = mPosX;
    mSegmentPos[0].y = mPosY;
    mSegmentPos[0].z = mPosZ;
    UpdateSegments();

    mSegmentPos[0].y = mSegmentPos[1].y + 0x1000;
    head = mSegmentPos[0];
    head.x += (s32)(((s64)data_02082214[((u16)mSegmentRot[0].y >> 4) << 1] * 0x64000 + 0x800) >> 12);
    head.z += (s32)(((s64)data_02082214[(((u16)mSegmentRot[0].y >> 4) << 1) + 1] * 0x64000 + 0x800) >> 12);
    mdCc_cs1[0].pos = head;
    mdCc_cs2[0].pos = head;

    if (mModelAnims[0].file != (BCA_File *)animFile[1]) {
        u32 n = (u8)(4 - mHealth);
        if (n < 3) {
            part = mSegmentPos[0];
            part.x += (s32)(((s64)data_02082214[((u16)mSegmentRot[0].y >> 4) << 1] * 0x23000 + 0x800) >> 12);
            part.z += (s32)(((s64)data_02082214[(((u16)mSegmentRot[0].y >> 4) << 1) + 1] * 0x23000 + 0x800) >> 12);
            if (n == 0)
                part.y += 0x89000;
            else
                part.y += 0xb9000;
            if (mIntroDone != 0) {
                mParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    (u32)mParticle, data_ov034_02113828[n], part.x, part.y, part.z, 0, 0);
            }
        }
    }

    UpdateSegmentModels();

    {
        int j;
        for (j = 0; j < 5; j++) {
            mdCc_cs1[j].Clear();
            mdCc_cs1[j].Update();
            mdCc_cs2[j].Clear();
            mdCc_cs2[j].Update();
        }
    }

    if (mDriveCamera != 0) {
        void *cam = data_0209f318;
        camA.x = 0; camA.y = 0x80000; camA.z = 0;
        camB.x = 0; camB.y = 0x80000; camB.z = 0x1f4000;
        scratch.x = 0; scratch.y = 0; scratch.z = 0;
        Matrix4x3_FromRotationY(data_020a0e68, mSegmentRot[0].y);
        MulVec3Mat4x3(&camA, data_020a0e68, &scratch);
        Vec3_Add(&t0, &mPosX, &scratch);
        camA = t0;
        scratch.x = 0; scratch.y = 0; scratch.z = 0;
        MulVec3Mat4x3(&camB, data_020a0e68, &scratch);
        Vec3_Add(&t1, &mPosX, &scratch);
        camB = t1;
        func_020092c4(cam, (char *)cam + 0x80, &camA);
        func_020092c4(cam, (char *)cam + 0x8c, &camB);
    }

    return 1;
}

// @symbol _ZN12daHanachan_c13InitResourcesEv
int daHanachan_c::InitResources()
{
    char *c = (char *)this;
    s32 i;
    ModelAnim *animRow;
    TextureSequence *texSeq;
    char *posRow;
    char *rotRow;
    Vector3 *segPos;
    char *texRow;
    dCcAcPos_c *cyl1;
    dCcAcPos_c *cyl2;
    SharedFilePtr *modelFile;
    void *animFile;
    SharedFilePtr *texFile;
    s32 prev;
    s32 radius;
    s32 one;
    s32 minusOne;
    s32 zeroA;
    s32 zeroB;
    s32 zeroC;
    s32 cyl1Height;
    s32 cyl1Flags;
    s32 cyl2Height;
    s32 va[3];
    s32 vb[3];
    s32 out[3];
    s32 j;
    ModelAnim *anim;

    mStarID = (u8)param1;
    if (mStarID == 0xFF) {
        mStarID = 0;
    }
    mStarTrackID = (u8)TrackStar(mStarID, 2);

    i = 0;
    cyl1 = &mdCc_cs1[0];
    cyl2 = &mdCc_cs2[0];
    cyl1Height = 0x78000;
    cyl1Flags = 0x200000;
    cyl2Height = 0x32000;
    one = 1;
    texRow = c;
    posRow = c;
    rotRow = c;
    animRow = &mModelAnims[0];
    texSeq = &mTextureSequences[0];
    segPos = &mSegmentPos[0];
    minusOne = -1;
    zeroA = 0;
    zeroB = 0;
    zeroC = 0;
    do {
        mSegmentSpacing[i] = data_ov034_021138c4[i];
        modelFile = data_ov034_02113838[i];
        Model::LoadFile(*modelFile);
        texFile = data_ov034_0211384c[i];
        TextureSequence::LoadFile(*texFile);
        animFile = data_ov034_02113860[i];
        Animation::LoadFile(*(SharedFilePtr *)animFile);
        Animation::LoadFile(*(SharedFilePtr *)data_ov034_02113874[i]);
        Animation::LoadFile(*(SharedFilePtr *)data_ov034_02113888[i]);
        Animation::LoadFile(*(SharedFilePtr *)data_ov034_0211389c[i]);
        Animation::LoadFile(*(SharedFilePtr *)data_ov034_021138b0[i]);
        animRow->SetFile(*(BMD_File **)((char *)modelFile + 4), one, minusOne);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(animRow, *(void **)((char *)animFile + 4), zeroA, 0x1000, zeroA);
        TextureSequence::Prepare(**(BMD_File **)((char *)modelFile + 4), **(BTP_File **)((char *)texFile + 4));
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(texSeq, *(void **)((char *)texFile + 4), zeroB, 0x1000, zeroB);
        *(s32 *)(texRow + 0x374) = zeroC;
        if (i == 0) {
            *(s32 *)(posRow + 0x3cc) = mPosX;
            *(s32 *)(posRow + 0x3d0) = mPosY;
            *(s32 *)(posRow + 0x3d4) = mPosZ;
            *(s16 *)(rotRow + 0x444) = mAngleX;
            *(s16 *)(rotRow + 0x446) = mAngleY;
            *(s16 *)(rotRow + 0x448) = mAngleZ;
            mPrevAngleY = *(s16 *)(rotRow + 0x446);
        } else {
            va[2] = 0 - mSegmentSpacing[i];
            va[0] = zeroC;
            va[1] = zeroC;
            vb[0] = zeroC;
            vb[1] = zeroC;
            vb[2] = zeroC;
            Matrix4x3_FromRotationY(data_020a0e68, mSegmentRot[0].y);
            MulVec3Mat4x3(va, data_020a0e68, vb);
            prev = i - 1;
            Vec3_Add(out, &mSegmentPos[prev], vb);
            *(s32 *)(posRow + 0x3cc) = out[0];
            *(s32 *)(posRow + 0x3d0) = out[1];
            *(s32 *)(posRow + 0x3d4) = out[2];
            *(s16 *)(rotRow + 0x446) = *(s16 *)(c + prev * 6 + 0x446);
        }
        *(s32 *)(posRow + 0x408) = 0x1000;
        *(s32 *)(posRow + 0x40c) = 0x1000;
        *(s32 *)(posRow + 0x410) = 0x1000;
        radius = data_ov034_021138d8[i];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(cyl1, this, segPos, radius + 0xA000, cyl1Height, cyl1Flags, 0x26FE0);
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(cyl2, this, segPos, radius, cyl2Height, 0x200004, 0x26FE0);
        texRow += 0x14;
        animRow++;
        cyl1++;
        texSeq++;
        posRow += 0xc;
        rotRow += 6;
        segPos++;
        cyl2++;
        i += 1;
    } while (i < 5);

    mVertAccel = -0x1000;
    mTerminalVelocity = -0x64000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnims[0], data_ov034_02113874[0][1], 0, 0x1000, 0xc);
    anim = &mModelAnims[0];
    anim = anim + 1;
    j = 1;
    do {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_02113874[j][1], 0, 0x1000, (u16)(s16)((5 - j) * 3));
        j += 1;
        anim++;
    } while (j < 5);

    SetState(0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x14000, 0x14000, 0, 0);
    mHealth = 4;
    UpdateSegmentModels();
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    return 1;
}

