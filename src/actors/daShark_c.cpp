//cpp
/**
 * daShark_c -- the lake shark (SHARK), ov090.
 *
 * Swims a closed path. Behavior advances mPathNodeIdx once the actor is
 * within 0x258000 of the current node, turns toward that node, and moves
 * forward at 0x14000. The hurt cylinder cycles three lateral offsets.
 *
 * common.h FIRST. func_ov090_02133904 copies data_020a0e68 into
 * mModelAnim.mat4x3 as twelve words. common.h's flat s32 m[12] is that
 * copy; math/Matrix.h's nested {Matrix3x3 r; Vector3 t;} scalarizes it.
 *
 * #pragma defer_codegen off emits .text in source order. The out-of-line
 * destructor is the key function, so it emits D1 then D0. The factory
 * (daShark_c_classInit) is not in this TU. g_profile_SHARK is not this TU.
 *
 * The two state words __sinit_ov090_02134020 copies into data_ov090_021345cc
 * are pointer-to-member records (function, this-delta 0): enter is
 * func_ov090_0213387c, and the per-frame body is func_ov090_02133830.
 * Those names are the ROM symbols the state table points at.
 *
 * deslop leftovers:
 * - func_ov090_02133710: a reference through pad_380[4] differs by 3 words
 *   (ldr/str via a register). The ROM is [this, #0x384], so the phase
 *   stays *(s32 *)((char *)self + 0x384).
 * - func_ov090_02133710: `if (hit->actorID != 0xbf)` is size 0x120 to
 *   0x114. The int temporary stays.
 * - func_ov090_0213387c: ModelAnim::SetAnim with a Fix12<int> speed is
 *   size 0x38 to 0x44. The int-parameter call stays.
 * - InitResources: dCcAcPos_c::Init with Fix12<int> radius and height is
 *   size 0x108 to 0x118. The int-parameter call stays.
 * - Behavior: clamping with `speed < mTerminalVelocity` and dropping the
 *   unk_0ac copy is size 0x1c0 to 0x1b8 and shifts the calls
 *   (WRONG-DEST, func_ov090_02133904 against 0x02010d40). The
 *   compare-into-terminal and the copy stay.
 */

#pragma defer_codegen off

#include "common.h"
#include "daShark_c.h"
#include "PathPtr.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* sinit constructs 021345a4 as file 0x325 (model) and 021345ac as file
 * 0x326 (animation). SharedFilePtr has no fields; the loaded pointer is
 * the word at +4. */
extern char data_ov090_021345a4[];
extern char data_ov090_021345ac[];
extern char data_ov090_021345cc[];
extern char data_020a0e68[];

extern "C" {
extern int func_02012694(int, void *);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    char *anim, void *file, int a, int b, unsigned int u);
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(
    void *m, short rx, short ry, short rz);
extern u16 DecIfAbove0_Short(u16 *value);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern s32 LenVec3(Vector3 *value);
extern s16 Vec3_HorzAngle(Vector3 *a, Vector3 *b);
extern s16 Vec3_VertAngle(Vector3 *a, Vector3 *b);
extern void Matrix4x3_FromRotationY(void *matrix, s32 angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *matrix, s16 angle);
extern void MulVec3Mat4x3(void *a, void *matrix, void *b);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *thiz, void *actor, void *pos, int f, int g,
    unsigned int h, unsigned int i);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    void *, void *, unsigned int, int, unsigned int, unsigned int, unsigned int);
}

bool ApproachLinear(short &value, short target, short step);

struct SharkLoadedFile {
    u32 id;
    void *file;
};

typedef int (daShark_c::*SharkFn)();

struct SharkState {
    SharkFn enter;
    SharkFn update;
};

// @symbol _ZN9daShark_cD1Ev
// @symbol _ZN9daShark_cD0Ev
daShark_c::~daShark_c()
{
}

// @symbol func_ov090_02133710
extern "C" void func_ov090_02133710(daShark_c *self)
{
    Vector3 offset;
    Vector3 pos;
    /* Swing phase lives at +0x384, inside pad_380. A pointer or reference
     * to that word addresses it from a register; the ROM uses [this, #0x384]. */
    *(s32 *)((char *)self + 0x384) += 1;
    if (*(s32 *)((char *)self + 0x384) > 2)
        *(s32 *)((char *)self + 0x384) = 0;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    if (*(s32 *)((char *)self + 0x384) == 1)
        offset.z = 0x8c000;
    else if (*(s32 *)((char *)self + 0x384) == 2)
        offset.z = -0x8c000;
    Matrix4x3_FromRotationY(data_020a0e68, 0);
    MulVec3Mat4x3(&offset, data_020a0e68, &self->mClsnOffset);
    self->mdCcAcPos_c.SetPosRelativeToActor(self->mClsnOffset);
    if (self->mdCcAcPos_c.otherOwner == 0)
        return;
    dActor_c *hit = dActor_c::FindWithID(self->mdCcAcPos_c.otherOwner);
    {
        int isPlayer = (hit->actorID == 0xbf); /* PLAYER */
        if (isPlayer == 0)
            return;
    }
    if (((Player *)hit)->mIsVanish)
        return;
    pos.x = self->mPosX;
    pos.y = self->mPosY;
    pos.z = self->mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(hit, &pos, 3, 0xc000, 1, 0, 1);
}

// @symbol func_ov090_02133830
extern "C" int func_ov090_02133830(daShark_c *self)
{
    int frame = self->mModelAnim.currFrame;

    frame = (frame >> 12) << 16;
    frame = (unsigned int)frame >> 16;
    if (frame == 0)
        func_02012694(9, &self->mCamSpacePosX);
    self->mModelAnim.speed = 0x1000;
    static_cast<Animation &>(self->mModelAnim).Advance();
    func_ov090_02133710(self);
    return 1;
}

// @symbol func_ov090_0213387c
extern "C" int func_ov090_0213387c(daShark_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (char *)&self->mModelAnim,
        ((SharkLoadedFile *)data_ov090_021345ac)->file,
        0, 0x1000, 0);
    return 1;
}

// @symbol func_ov090_021338b4
extern "C" int func_ov090_021338b4(void *self, void *state)
{
    daShark_c *shark = (daShark_c *)self;
    SharkFn *enter = (SharkFn *)state;

    *(SharkFn **)&shark->mState = enter;
    enter = *(SharkFn **)&shark->mState;
    if (*enter == 0)
        return 1;
    return (shark->*(*enter))();
}

// @symbol func_ov090_02133904
extern "C" void func_ov090_02133904(daShark_c *self)
{
    int pos[3];

    Vec3_Asr(pos, &self->mPosX, 3);
    Matrix4x3_FromTranslation(data_020a0e68, pos[0], pos[1], pos[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(
        data_020a0e68, self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModelAnim.mat4x3 = *(Matrix4x3 *)data_020a0e68;
}

// @symbol _ZN9daShark_c16CleanupResourcesEv
int daShark_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov090_021345a4)->Release();
    ((SharedFilePtr *)data_ov090_021345ac)->Release();
    return 1;
}

// @symbol _ZN9daShark_c16OnPendingDestroyEv
void daShark_c::OnPendingDestroy()
{
}

// @symbol _ZN9daShark_c6RenderEv
int daShark_c::Render()
{
    mModelAnim.Render((const Vector3 *)0);
    return 1;
}

// @symbol _ZN9daShark_c8BehaviorEv
int daShark_c::Behavior()
{
    DecIfAbove0_Short((u16 *)&mStateTimer);
    {
        SharkState *state = *(SharkState **)&mState;
        if (state->update != 0)
            (this->*(state->update))();
    }
    {
        PathPtr path;
        Vector3 node;
        Vector3 difference;
        Vector3 velocity;
        int distance;

        path.FromID(mPathID);
        path.GetNode(node, mPathNodeIdx);
        Vec3_Sub(&difference, (Vector3 *)&mPosX, &node);
        distance = LenVec3(&difference);
        if (distance == 0 || distance <= 0x258000) {
            mPathNodeIdx++;
            if (mPathNodeIdx >= mPathNodeCount)
                mPathNodeIdx = 0;
        }
        ApproachLinear(mPrevAngleY,
            Vec3_HorzAngle((Vector3 *)&mPosX, &node), 0x180);
        ApproachLinear(mPrevAngleX,
            Vec3_VertAngle((Vector3 *)&mPosX, &node), 0x40);
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        velocity.y = velocity.x = velocity.z = 0;
        velocity.z = 0x14000;
        Matrix4x3_FromRotationY(data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&velocity, data_020a0e68, (Vector3 *)&unk_0a4);
    }
    {
        /* unk_0ac is the z word of the vector just stored at unk_0a4.
         * The copy is load-bearing. */
        int speed = mVertSpeed + mVertAccel;
        int terminalVelocity = mTerminalVelocity;
        int kept = unk_0ac;
        if (speed >= terminalVelocity)
            terminalVelocity = speed;
        mVertSpeed = terminalVelocity;
        unk_0ac = kept;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    func_ov090_02133904(this);
    mdCcAcPos_c.Clear();
    {
        Player *player = ClosestPlayer();
        if (player != 0 && player->mIsVanish == 0)
            mdCcAcPos_c.Update();
    }
    return 1;
}

// @symbol _ZN9daShark_c13InitResourcesEv
int daShark_c::InitResources()
{
    /* Each PathPtr constructs at its declaration. The first call is
     * before the cylinder init and the second is after it. */
    mModelAnim.SetFile(
        (BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov090_021345a4),
        1, -1);
    Animation::LoadFile(*(SharedFilePtr *)data_ov090_021345ac);
    mPathID = (*(s32 *)&param1) & 0xff;
    if (mPathID < 0)
        mPathID = 0;
    {
        PathPtr path;
        path.FromID(mPathID);
        mPathNodeCount = path.NumNodes();
    }
    mTerminalVelocity = -0x3c000;
    mClsnOffset.x = 0;
    mClsnOffset.y = 0;
    mClsnOffset.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, &mClsnOffset, 0x42000, 0x6e000, 0x200004, 0);
    {
        PathPtr path;
        path.FromID(mPathID);
        mPathNodeIdx = 1;
        path.GetNode(*(Vector3 *)&mPosX, mPathNodeIdx);
    }
    func_ov090_021338b4(this, data_ov090_021345cc);
    return 1;
}
