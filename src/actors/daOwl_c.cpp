//cpp
/* The owl actor, daOwl_c (registry profile OWL). It waits unrendered until the
 * player comes near, then hovers around an anchor point, talks to the player
 * (message 0xa2) and, once the talk is over, can pick the player up and carry
 * it.
 * 22 functions, .text 0x02135700..0x021367e8.
 *
 * NAME: _ZTS7daOwl_c is "7daOwl_c" at ov094 0x02136a10; _ZTI at 0x02136a28
 * reads [__si_class_type_info, that string, _ZTI12dEnemyBase_c]. The vtable's
 * address point is 0x02136a58, and slots 16/17 hold D1/D0 below. The tree
 * previously called the class HootTheOwl (coined; vtable address only).
 *
 * STATES: the actor's state is a pointer to a pair of pointers-to-member
 * (enter, main). ov094's __sinit builds five of them from constants in .data.
 * Names are read off what each main handler does:
 *
 *     0x02136b40  dormant   invisible (Render skips it); waits for the player
 *     0x02136b60  hover     bobs at the anchor, wanders, starts the talk
 *     0x02136b50  talk      faces the player until the message ends
 *     0x02136b70  carry     pinned to the rider's matrix
 *     0x02136b30  return    fades out, reappears above the anchor, fades in
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02135700), D0
 * (0x02135748), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 * daOwl_c_classInit (0x02136798..0x021367e8, historical alias
 * HootTheOwl_Spawn) now appends after InitResources, at the end of source
 * order:
 * fBase_c::operator new(size_t) forwards `return new daOwl_c();` to the same
 * _ZN7fBase_cnwEj(1016) allocator the loose factory called by hand, and
 * daOwl_c has no user-declared constructor, so the inherited dEnemyBase_c
 * ctor plus the vtable store plus the four member subobjects in field order
 * (dCcAcPos_c, dBgCh_Actr, ModelAnim, dExtShadowModel_c) come from the implicit
 * default constructor with zero mangled calls.
 *
 * The fourteen address-named functions whose first parameter was the owl are
 * non-static methods. The cartridge keeps no English spelling, so the method
 * name is the ROM address. func_ov094_02136188 still takes the State pointer.
 *
 * Leftover: the actor-id test in func_ov094_021357a4 goes through locals `t`
 *   and `eq`. Collapsing it to `other->actorID != ACTOR_PLAYER` shrank the
 *   method from ROM 0x110 to object 0x104. The locals stay.
 * Leftover: func_ov094_02136024 stores mPos into `d` and then overwrites all
 *   three fields. Dropping those three stores shrank the method from ROM
 *   0x12c to object 0x114. The stores stay.
 * Leftover: ModelAnim::SetAnim, dCcAcPos_c::Init, dBgCh_Actr::Init and
 *   dActor_c::DropShadowRadHeight take Fix12<int> by value, so they stay
 *   mangled calls. Sound::PlayLong and dBgCh_Actr::Unk_0203589c have no member
 *   declaration in a shared header.
 * Leftover: unk_3e8 is scratch that each state uses differently (fade phase,
 *   sound latch, bobbing phase). It stays unnamed.
 * Leftover: the rider matrix pointer at +0xc8, and the model matrix at +0x328
 *   and bone-matrix pointer at +0x320, stay raw offsets from the actor.
 *   Writing them as mModelAnim members hoists the base.
 * Leftover: state-timer reads use a u16 cast. mStateTimer is s16; the ROM
 *   loads it unsigned.
 * Leftover: RandomIntInternal stays unsigned so func_ov094_02135e64 keeps its
 *   logical shift. Behavior tests the carry state twice; the else is what
 *   the ROM emits.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daOwl_c.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* Resource handles the static initializer constructs. The wrapper spellings
 * are reconstructed; the constructor and destructor addresses, the file IDs
 * and the 8-byte width are the ROM's. */
struct OwlModelFilePtr : SharedFilePtr {
    u32 words[2];

    OwlModelFilePtr(u32 fileID);
    ~OwlModelFilePtr();
};

struct daOwlAnimationFilePtr : SharedFilePtr {
    u32 words[2];

    daOwlAnimationFilePtr(u32 fileID);
    ~daOwlAnimationFilePtr();
};

/* One BSS state: two pointer-to-member constants, copied by the initializer. */
struct OwlState {
    int (daOwl_c::*enter)();
    int (daOwl_c::*main)();

    OwlState(int (daOwl_c::*enterFn)(), int (daOwl_c::*mainFn)())
        : enter(enterFn), main(mainFn) {}
};

typedef char OwlModelFilePtr_size_must_be_8[sizeof(OwlModelFilePtr) == 8 ? 1 : -1];
typedef char daOwlAnimationFilePtr_size_must_be_8[sizeof(daOwlAnimationFilePtr) == 8 ? 1 : -1];
typedef char OwlState_size_must_be_0x10[sizeof(OwlState) == 0x10 ? 1 : -1];

extern OwlModelFilePtr data_ov094_02136ae0;
extern daOwlAnimationFilePtr data_ov094_02136ae8;
extern daOwlAnimationFilePtr data_ov094_02136af0;
extern daOwlAnimationFilePtr data_ov094_02136af8;
extern OwlState data_ov094_02136b30;
extern OwlState data_ov094_02136b40;
extern OwlState data_ov094_02136b50;
extern OwlState data_ov094_02136b60;
extern OwlState data_ov094_02136b70;

/* decl_common.h includes common.h, so Matrix4x3 is the flat s32 m[12]
 * spelling. math/Matrix.h (via daOwl_c.h -> ModelAnim.h) stands down. */
typedef char Matrix4x3_size_must_be_0x30[sizeof(Matrix4x3) == 0x30 ? 1 : -1];

struct OwlVec {
    int x, y, z;
};
typedef char OwlVec_size_must_be_0x0c[sizeof(OwlVec) == 0x0c ? 1 : -1];

struct V6 {
    int v[6];
};
typedef char V6_size_must_be_0x18[sizeof(V6) == 0x18 ? 1 : -1];

enum {
    ACTOR_PLAYER = 0xbf,
    OPACITY_MAX = 0x1f,
    TALK_IDLE = 0,
    TALK_TALKING = 1,
    TALK_DONE = 2
};

#define OWL_STATE(sym)      ((daOwl_c::State *)&(sym))
#define OWL_STATE_DORMANT   OWL_STATE(data_ov094_02136b40)
#define OWL_STATE_HOVER     OWL_STATE(data_ov094_02136b60)
#define OWL_STATE_TALK      OWL_STATE(data_ov094_02136b50)
#define OWL_STATE_CARRY     OWL_STATE(data_ov094_02136b70)
#define OWL_STATE_RETURN    OWL_STATE(data_ov094_02136b30)

/* mStateTimer is s16 but every read in the ROM is an unsigned halfword load. */
#define OWL_TIMER(o)        (*(u16 *)&(o)->mStateTimer)

/* Whole animation frame of the ModelAnim, as the ROM extracts it. */
#define OWL_FRAME(o)        ((u16)((o)->mModelAnim.currFrame >> 12))

extern "C" {
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
extern short data_02082214[];
extern int data_0209e650[];
extern Matrix4x3 data_020a0e68;

int func_ov002_020df840(void *a, void *b, void *d);
void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 *a, const Vector3 *b, Fix12i f);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void *pos, unsigned int d);
int func_ov002_020df7f4(void *c);
int func_ov002_020df7ac(void *thiz);
void _ZN10dBgCh_Actr12Unk_0203589cEv(void *self);
int func_02012694(int a, void *pos);
int ApproachAngle(short *p, int target, int a, int b, int c);
int Vec3_Dist(const void *a, const void *b);
short Vec3_HorzAngle(const void *a, const void *b);
short Vec3_VertAngle(const void *a, const void *b);
unsigned int RandomIntInternal(void *seed);
void _Z14ApproachLinearRiii(int *x, int target, int step);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulVec3Mat4x3(void *dst, void *mat, void *src);
void Vec3_Sub(OwlVec *out, OwlVec *a, OwlVec *b);
int LenVec3(OwlVec *v);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromTranslation(Matrix4x3 *m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *m, Fix12i x, Fix12i y, Fix12i z);
void MulMat4x3Mat4x3(const int *a, const int *b, int *dst);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, Matrix4x3 *m, Fix12i fx, int t, unsigned int u);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *, void *, int *, int, int, unsigned int, unsigned int);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *, void *, int, int, void *, int);
int IsStarCollectedInCurLevel(int);
void DecIfAbove0_Short(void *);
}

// @symbol _ZN7daOwl_cD1Ev
// @symbol _ZN7daOwl_cD0Ev
/* The whole body is compiler-emitted: one vptr store, then dExtShadowModel_c
 * (0x370), ModelAnim (0x30c), dBgCh_Actr (0x150) and dCcAcPos_c (0x110) in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c. D1 is the
 * evidence for the header's four member types; D0 adds dEnemyBase_c's inline
 * operator delete. */
daOwl_c::~daOwl_c()
{
}

// @symbol _ZN7daOwl_c19func_ov094_021357a4Ev
/* Hover, once the talk is over: if the player is touching the owl, grab it.
 * The contact owner is the actor id in the hit-box; only the player (0xbf)
 * counts. Attaches the player, zeroes the owl's velocity and enters the carry
 * state; if the attach is refused the rider is dropped again. */
void daOwl_c::func_ov094_021357a4()
{
    int st[3];
    OwlVec tmp;
    tmp.x = data_ov094_02136a1c[0];
    tmp.y = data_ov094_02136a1c[1];
    tmp.z = data_ov094_02136a1c[2];
    mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)&tmp);
    if (mdCcAcPos_c.otherOwner == 0)
        return;
    dActor_c *other = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
    if (other == 0)
        return;
    int t = other->actorID;
    unsigned eq = (t == ACTOR_PLAYER);
    if (!eq)
        return;
    mRider = (Player *)other;
    st[0] = 0;
    st[1] = 0;
    st[2] = 0;
    st[1] = 0x1838000;
    if (data_0209f2f8 == 0x16)
        st[1] = 0x1194000;
    unk_0a4 = 0;
    mVertSpeed = 0;
    unk_0ac = 0;
    if (func_ov002_020df840(mRider, this, st) != 1) {
        mRider = 0;
        return;
    }
    func_ov094_02136188(OWL_STATE_CARRY);
}

// @symbol _ZN7daOwl_c19func_ov094_021358b4Ev
/* Main of the return state. Phase 0 fades the owl out (with an upward speed);
 * once invisible it jumps to just above the anchor and starts phase 1, which
 * eases it onto the anchor while fading back in, then goes to hover. */
int daOwl_c::func_ov094_021358b4()
{
    if (unk_3e8 == 0) {
        if (mOpacity != 0) {
            mOpacity--;
            mPrevAngleX = 0;
            mVertSpeed = 0x14000;
        } else {
            unk_3e8 = 1;
            unk_0a4 = 0;
            mVertSpeed = 0;
            unk_0ac = 0;
            mPosX = mAnchorX;
            mPosY = mAnchorY;
            mPosZ = mAnchorZ;
            mPosY += 0x64000;
        }
    } else {
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3 *)&mPosX, (Vector3 *)&mAnchorX, 0x2000);
        if (mOpacity < OPACITY_MAX) {
            mOpacity++;
        } else {
            mOpacity = OPACITY_MAX;
            func_ov094_02136188(OWL_STATE_HOVER);
        }
    }
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_0213598cEv
/* Enter of the return state: restart the fade and play the animation
 * data_ov094_02136af8[1] at double speed. */
int daOwl_c::func_ov094_0213598c()
{
    unk_3e8 = 0;
    mStateTimer = 0;
    mAnimSpeed = 0x2000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov094_02136af8.words[1], 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_021359d8Ev
/* Main of the carry state. Once the 30-frame timer has run out it keeps
 * sound 0x18c playing, plays sound 0x139 once per animation loop, levels the pitch, and
 * hands the rider back (to the return state) when the rider lets go, is lost,
 * or the owl hits a wall. */
int daOwl_c::func_ov094_021359d8()
{
    Player *rider;

    if (OWL_TIMER(this) == 1) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov094_02136ae8.words[1], 0, 0x1000, 0);
        mAnimSpeed = 0x1000;
    }

    if (OWL_TIMER(this) == 0) {
        mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 3, 0x18c, &mCamSpacePosX, 0);
    }

    if (OWL_TIMER(this) != 0) {
        if (unk_3e8 == 0 && OWL_FRAME(this) <= 2) {
            func_02012694(0x139, &mCamSpacePosX);
            unk_3e8 = 1;
        } else {
            if (OWL_FRAME(this) > 2)
                unk_3e8 = 0;
        }
    }

    rider = mRider;
    if (rider != 0 && func_ov002_020df7f4(rider) == 1) {
        OWL_TIMER(this) = 0xa;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov094_02136af8.words[1], 0, 0x1000, 0);
    }

    ApproachAngle(&mPrevAngleX, 0, 0xa, 0x200, 0x100);

    if (mWithMeshClsn.IsOnWall() != 0 || func_02035638((u8 *)&mWithMeshClsn) != 0) {
        rider = mRider;
        if (rider != 0 && func_ov002_020df7ac(rider) != 0) {
            _ZN10dBgCh_Actr12Unk_0203589cEv(&mWithMeshClsn);
            mRider = 0;
            unk_3e8 = 0;
            mStateTimer = 0;
            func_ov094_02136188(OWL_STATE_RETURN);
            return 1;
        }
    }

    rider = mRider;
    if (rider == 0) {
        goto cleanup;
    }
    if (rider == 0) {
        goto end;
    }
    if (func_ov002_020df7f4(rider) < 0) {
cleanup:
        mRider = 0;
        unk_3e8 = 0;
        mStateTimer = 0;
        func_ov094_02136188(OWL_STATE_RETURN);
    }
end:
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_02135bd4Ev
/* Enter of the carry state: 30-frame timer, animation data_ov094_02136af8[1] at double speed. */
int daOwl_c::func_ov094_02135bd4()
{
    mStateTimer = 0x1e;
    unk_3e8 = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov094_02136af8.words[1], 0, 0x1000, 0);
    mAnimSpeed = 0x2000;
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_02135c28Ev
/* Main of the hover state. Until a conversation has happened it tries to
 * start one with the player (message 0xa2) and, if that takes, enters the talk
 * state. Otherwise it wanders: when it is too far from the anchor it turns
 * back toward it, else it picks a random heading and duration. It bobs on a
 * sine table around the anchor's height and moves along its heading. */
int daOwl_c::func_ov094_02135c28()
{
    int result;
    int ang;
    int *p3e8;
    short tbl;
    int idx;
    V6 buf;

    if (mTalkState == TALK_IDLE) {
        if (ClosestPlayer() != 0) {
            Player *player = mTalkTarget;
            if (player != 0) {
                if ((u16)(player->mStateFlags & 0x800) == 0) {
                    if (player->StartTalk(*this, 1) != 0) {
                        if (mTalkTarget->ShowMessage(*this, 0xa2, (const Vector3 *)&mPosX, 0, 0) == 1) {
                            func_02012694(0x176, &mCamSpacePosX);
                            mTalkState = TALK_TALKING;
                            func_ov094_02136188(OWL_STATE_TALK);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    if (Vec3_Dist(&mPosX, &mAnchorX) > 0x190000) {
        mStateTimer = 0x32;
        mWanderAngleY = Vec3_HorzAngle(&mPosX, &mAnchorX);
    } else if (OWL_TIMER(this) == 0) {
        mWanderAngleY = (s16)(((u32)RandomIntInternal(data_0209e650) >> 8) << 12);
        mStateTimer = (s16)((((u32)RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0x32);
    }

    ApproachAngle(&mPrevAngleY, mWanderAngleY, 0xa, 0x200, 0x100);
    ApproachAngle(&mPrevAngleX, 0, 0xa, 0x200, 0x100);

    buf.v[0] = 0;
    buf.v[1] = 0;
    buf.v[2] = 0x5000;
    buf.v[3] = 0;
    buf.v[4] = 0;
    buf.v[5] = 0;

    p3e8 = &unk_3e8;
    *p3e8 += 0x200;
    ang = unk_3e8;
    idx = ((u16)(short)ang >> 4) * 2;
    tbl = data_02082214[idx];
    result = (int)(((long long)tbl * 0x64000 + 0x800) >> 12);
    _Z14ApproachLinearRiii(&mPosY, mAnchorY + result, 0x3000);

    Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
    MulVec3Mat4x3(&buf, &data_020a0e68, &unk_0a4);
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_02135e64Ev
/* Enter of the hover state: animation data_ov094_02136af0[1] at normal speed, random heading and a
 * 50..113 frame wander timer. */
int daOwl_c::func_ov094_02135e64()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov094_02136af0.words[1], 0, 0x1000, 0);
    mWanderAngleY = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
    mStateTimer = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x3f) + 0x32);
    mAnimSpeed = 0x1000;
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_02135ee0Ev
/* Main of the talk state: turn to face the player (yaw, then pitch toward a
 * point 200 units above its feet). When the player's talk state goes negative
 * the conversation is over; mark it done and return to hover. */
int daOwl_c::func_ov094_02135ee0()
{
    if (mTalkTarget == 0) {
        func_ov094_02136188(OWL_STATE_HOVER);
        return 1;
    }
    OwlVec v = *(OwlVec *)&mTalkTarget->mPosX;
    Vector3 w;
    w.x = v.x;
    w.y = v.y;
    w.z = v.z;
    ApproachAngle(&mPrevAngleY, Vec3_HorzAngle(&mPosX, &w), 0xa, 0x200, 0x100);
    Vector3 u;
    v.y += 0xc8000;
    u.x = v.x - 0;
    u.y = v.y;
    u.z = v.z;
    ApproachAngle(&mPrevAngleX, Vec3_VertAngle(&mPosX, &u), 0xa, 0x200, 0x100);
    if (mTalkTarget->GetTalkState() < 0) {
        mTalkState = TALK_DONE;
        func_ov094_02136188(OWL_STATE_HOVER);
    }
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_02135fe0Ev
/* Enter of the talk state: stop, and play the animation data_ov094_02136af0[1]. */
int daOwl_c::func_ov094_02135fe0()
{
    unk_0a4 = 0;
    mVertSpeed = 0;
    unk_0ac = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void *)data_ov094_02136af0.words[1], 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_02136024Ev
/* Main of the dormant state. Watches the nearest player and keeps the owl 400
 * units above that player's ground height. When the player's position is
 * within 40 units of the owl (LenVec3 < 0x28000) it sets the anchor 800 units
 * from the owl along a fixed yaw, faces it, and enters hover.
 */
int daOwl_c::func_ov094_02136024()
{
    Player *p = ClosestPlayer();
    if (p != 0 && *(int *)((char *)p + 0x37c) != 0) {
        char *ip = (char *)&p->mPosX;
        OwlVec pp;
        OwlVec d;
        OwlVec *selfpos = (OwlVec *)&mPosX;
        pp.x = *(int *)(ip + 0);
        pp.y = *(int *)(ip + 4);
        pp.z = *(int *)(ip + 8);
        mPosY = p->mGroundY + 0x190000;
        mTalkTarget = p;
        Vec3_Sub(&d, selfpos, &pp);
        if (LenVec3(&d) < 0x28000) {
            d.x = mPosX;
            d.y = mPosY;
            d.z = mPosZ;
            d.x = 0;
            d.y = 0;
            d.z = 0x320000;
            Matrix4x3_FromRotationY(&data_020a0e68, 0x4000);
            MulVec3Mat4x3(&d, &data_020a0e68, (OwlVec *)&mAnchorX);
            {
                int *px = &mAnchorX;
                int *py = &mAnchorY;
                int *pz = &mAnchorZ;
                *px = *px + mPosX;
                *py = *py + mPosY;
                *pz = *pz + mPosZ;
            }
            mPrevAngleY = Vec3_HorzAngle((OwlVec *)&mPosX, (OwlVec *)&mAnchorX);
            mAngleY = mPrevAngleY;
            func_ov094_02136188(OWL_STATE_HOVER);
        }
    }
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_02136150Ev
/* Enter of the dormant state: animation data_ov094_02136af0[1]. */
int daOwl_c::func_ov094_02136150()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)((char *)&data_ov094_02136af0 + 4), 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN7daOwl_c19func_ov094_02136188EPNS_5StateE
/* Switch state: store it, then run its enter handler if it has one. */
int daOwl_c::func_ov094_02136188(State *state)
{
    mCurrentState = state;
    State *s = mCurrentState;
    if (s->mEnter == 0)
        return 1;
    return (this->*(s->mEnter))();
}

// @symbol _ZN7daOwl_c19func_ov094_021361d8Ev
/* Per-frame model setup: world matrix from position (>> 3) and angles, the
 * model's opacity, its shadow matrix, and the drop shadow. */
void daOwl_c::func_ov094_021361d8()
{
    Matrix4x3 out;
    OwlVec v;
    Vec3_Asr((Vector3 *)&v, (Vector3 *)&mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        mAngleX, mAngleY, mAngleZ);
    mModelAnim.ApplyOpacity(mOpacity, 1);
    *(Matrix4x3 *)((char *)this + 0x328) = data_020a0e68;
    MulMat4x3Mat4x3((const int *)(*(char **)((char *)this + 0x320) + 0x30),
        (const int *)((char *)this + 0x328), out.m);
    Matrix4x3_FromTranslation(&data_020a0e68,
        mPosX >> 3,
        (mPosY - 0x38000) >> 3,
        mPosZ >> 3);
    mShadowMat = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mShadowMat, 0x64000, 0x320000, 0xf);
}

// @symbol _ZN7daOwl_c19func_ov094_021362e0Ev
/* Carry state's placement: take the rider's matrix (pointer at rider + 0xc8),
 * offset and rotate it, and use the result as the owl's own position, yaw and
 * model matrix. */
void daOwl_c::func_ov094_021362e0()
{
    OwlVec v;
    Player *m;
    int z = 0;
    if (mRider == 0)
        return;
    *(volatile Fix12i *)&v.x = z;
    *(volatile Fix12i *)&v.y = z;
    *(volatile Fix12i *)&v.z = z;
    m = mRider;
    data_020a0e68 = *(Matrix4x3 *)(*(char **)((char *)m + 0xc8));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x3000, z, z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, -0x6000, 0x1000, 0x4000);
    v.x = data_020a0e68.m[9];
    v.y = data_020a0e68.m[10];
    v.z = data_020a0e68.m[11];
    mPosX = v.x << 3;
    mPosY = v.y << 3;
    mPosZ = v.z << 3;
    mPrevAngleY = mRider->mAngleY;
    mPrevAngleX = (s16)z;
    *(Matrix4x3 *)((char *)this + 0x328) = data_020a0e68;
}

// @symbol _ZN7daOwl_c16CleanupResourcesEv
int daOwl_c::CleanupResources()
{
    data_ov094_02136ae0.Release();
    data_ov094_02136af8.Release();
    data_ov094_02136ae8.Release();
    data_ov094_02136af0.Release();
    return 1;
}

// @symbol _ZN7daOwl_c16OnPendingDestroyEv
void daOwl_c::OnPendingDestroy()
{
}

// @symbol _ZN7daOwl_c6RenderEv
int daOwl_c::Render()
{
    if (mCurrentState == OWL_STATE_DORMANT)
        return 1;
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN7daOwl_c8BehaviorEv
int daOwl_c::Behavior()
{
    DecIfAbove0_Short(&mStateTimer);
    {
        State *o = mCurrentState;
        /* Read the handler word. `&o->mMain` materialises the whole pmf. */
        if (*(int *)((char *)o + 8) != 0) {
            (this->*(o->mMain))();
        }
    }
    if (mCurrentState == OWL_STATE_DORMANT)
        return 1;
    mModelAnim.speed = mAnimSpeed;
    mModelAnim.Advance();
    {
        State *m = mCurrentState;
        if ((m == OWL_STATE_TALK || m == OWL_STATE_HOVER ||
             m == OWL_STATE_RETURN) &&
            (unsigned short)(mModelAnim.currFrame >> 0xc) == 0) {
            func_02012694(0x139, &mCamSpacePosX);
        }
    }
    /* The ROM tests this state twice; the else is unreachable. */
    if (mCurrentState == OWL_STATE_CARRY) {
        if (mCurrentState == OWL_STATE_CARRY) {
            func_ov094_021362e0();
            mAngleX = mPrevAngleX;
            mAngleY = mPrevAngleY;
            mAngleZ = mPrevAngleZ;
            UpdateWMClsn(mWithMeshClsn, 0);
        } else {
            func_ov094_021361d8();
        }
        return 1;
    }
    {
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        int keep = unk_0ac;
        if (fallSpeed >= clamped)
            clamped = fallSpeed;
        mVertSpeed = clamped;
        unk_0ac = keep;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov094_021361d8();
    if (mCurrentState == OWL_STATE_HOVER && mTalkState == TALK_DONE) {
        func_ov094_021357a4();
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN7daOwl_c13InitResourcesEv
int daOwl_c::InitResources()
{
    int v0[3];
    void *f;

    f = Model::LoadFile(data_ov094_02136ae0);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(data_ov094_02136af8);
    dExtFrameCtrl_c::LoadFile(data_ov094_02136ae8);
    dExtFrameCtrl_c::LoadFile(data_ov094_02136af0);
    v0[0] = data_ov094_02136a1c[0];
    v0[1] = data_ov094_02136a1c[1];
    v0[2] = data_ov094_02136a1c[2];
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, v0, 0x64000, 0x64000, 0x800004, 0);

    mRider = 0;
    mdCcAcPos_c.flags |= 2;
    mTerminalVelocity = -0x1e000;
    mAnimSpeed = 0x1000;
    mOpacity = OPACITY_MAX;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x50000, 0x64000, (void *)0, 0);
    func_ov094_02136188(OWL_STATE_DORMANT);

    if (data_0209f2f8 != 7)
        goto ret1;
    if (data_0209f220 != 1) {
        if (IsStarCollectedInCurLevel(1) != 0)
            goto ret1;
    }
    MarkForDestruction();
    return 0;
ret1:
    return 1;
}

// @symbol daOwl_c_classInit
extern "C" daOwl_c *daOwl_c_classInit()
{
    return new daOwl_c();
}

/* Static-resource ownership, formerly the handwritten __sinit_ov094_021367e8.
 * Definition order is the retail initializer's construction order. The
 * registration nodes and the pointer-to-member descriptors are compiler
 * temporaries. */
OwlModelFilePtr data_ov094_02136ae0(0x3cb);
daOwlAnimationFilePtr data_ov094_02136af8(0x3cc);
daOwlAnimationFilePtr data_ov094_02136ae8(0x3cd);
daOwlAnimationFilePtr data_ov094_02136af0(0x3ce);

OwlState data_ov094_02136b40(&daOwl_c::func_ov094_02136150, &daOwl_c::func_ov094_02136024);
OwlState data_ov094_02136b50(&daOwl_c::func_ov094_02135fe0, &daOwl_c::func_ov094_02135ee0);
OwlState data_ov094_02136b60(&daOwl_c::func_ov094_02135e64, &daOwl_c::func_ov094_02135c28);
OwlState data_ov094_02136b70(&daOwl_c::func_ov094_02135bd4, &daOwl_c::func_ov094_021359d8);
OwlState data_ov094_02136b30(&daOwl_c::func_ov094_0213598c, &daOwl_c::func_ov094_021358b4);
