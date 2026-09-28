//cpp
/* daManta_c -- Jolly Roger Bay manta (MANTA 226).
 *
 * Swims the path in param1's low byte. Every 0x27 frames it drops a
 * WATER_RING (244) on bone 3 and remembers that ring's uniqueID.
 * Rings taken in that order play the collection jingle and count up.
 * Five of them, then 0x1e frames, spawn STAR (178) with the star index
 * from param1 bits 12..15. A cylinder on data_ov090_02134200 hurts
 * PLAYER (191).
 *
 * daManta_c_classInit abuts this run and stays in src/d_a_manta.c.
 * g_profile_MANTA stays in its own file. #pragma defer_codegen off
 * below emits .text in source order, which is the ROM order.
 *
 * deslop leftovers:
 * - func_ov090_021327e4: mRingIDs[mRingRead] != mHitRing->uniqueID is 1
 *   word (cmp r1, r0; the cartridge is cmp r0, r1). Both operand orders
 *   missed. The address dance below keeps cmp r0, r1.
 * - func_ov090_02132a58: mModelAnim.SetAnim with Fix12<int> speed is
 *   0x6c -> 0x78. The scalar _ZN9ModelAnim7SetAnim call stays.
 * - daManta_c::InitResources: mdCcAcPos_c.Init with Fix12<int> radius
 *   and height is 0x1b0 -> 0x1c0 and moves reloc destinations. The
 *   scalar _ZN10dCcAcPos_c4Init call stays.
 * - daManta_c::Behavior: dropping the unk_0ac copy after the terminal
 *   clamp is 0x1a4 -> 0x19c and moves the calls that follow it.
 */

#pragma defer_codegen off

/* decl_common.h first: Matrix4x3 must stay the flat s32[12] spelling.
 * func_ov090_02132b14 indexes .m. */
#include "decl_common.h"
#include "daManta_c.h"
#include "daWater_Ring_c.h"
#include "PathPtr.h"
#include "SharedFilePtr.h"

/* init at +0, execute at +8. Same shape as daWater_Ring_c::State.
 * The overlay image stores each as (function, this-delta) with delta 0. */
struct MantaState {
    int (daManta_c::*init)();
    int (daManta_c::*execute)();
};

/* SharedFilePtr.h has no fields. The BCA pointer SetAnim reads is the
 * word at +4, which is where Construct leaves the loaded file. */
struct MantaFileWord {
    int head;
    void *file;
};

enum {
    MANTA_STAR = 0xb2,
    MANTA_PLAYER = 0xbf,
    MANTA_WATER_RING = 0xf4,
    MANTA_RING_GOAL = 5,
    MANTA_RING_SLOTS = 0x14,
    MANTA_RING_INTERVAL = 0x27,
    MANTA_STAR_WAIT = 0x1e,
    MANTA_STAR_SPAWNED = 0xa,
    MANTA_COLLECT_JINGLE = 0x25,
    MANTA_NODE_REACH = 0x258000,
    MANTA_FORWARD = 0xa000,
    MANTA_STAR_FLAG = 0x40
};

extern "C" {
extern SharedFilePtr data_ov090_02134524;
extern SharedFilePtr data_ov002_0210da10;
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov090_0213452c;
extern unsigned char data_0209f2d8;
extern Matrix4x3 data_020a0e68;

void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    Player *, const Vector3 &, unsigned int, Fix12i, unsigned int,
    unsigned int, unsigned int);
void func_02012790(u32 a);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromTranslation(Matrix4x3 *m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void MulMat4x3Mat4x3(const int *a, const int *b, int *dst);
unsigned short DecIfAbove0_Short(unsigned short *p);
void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
int LenVec3(Vector3 *v);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
short Vec3_VertAngle(const Vector3 *v0, const Vector3 *v1);
void Matrix4x3_FromRotationY(void *m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angle);
void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *out);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *self, dActor_c *a, const Vector3 &v, int b, int c,
    unsigned int d, unsigned int e);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *anim, void *file, int a, int b, unsigned int u);

void func_ov090_02132730(daManta_c *self);
int func_ov090_021327e4(daManta_c *self);
int func_ov090_02132a58(daManta_c *self);
int func_ov090_02132ac4(daManta_c *self, MantaState *state);
void func_ov090_02132b14(daManta_c *self);
}

int ApproachLinear(short &v, short target, short step);

/* One written destructor. The compiler emits D1 then D0 from the
 * members above; D0 adds dEnemyBase_c's inline operator delete. */
// @symbol _ZN9daManta_cD1Ev
// @symbol _ZN9daManta_cD0Ev
daManta_c::~daManta_c()
{
}

/* Cylinder follows data_ov090_02134200. A PLAYER inside it is hurt
 * from this actor's position. */
// @symbol func_ov090_02132730
extern "C" void func_ov090_02132730(daManta_c *self)
{
    Vector3 offset;
    offset.x = data_ov090_02134200.x;
    offset.y = data_ov090_02134200.y;
    offset.z = data_ov090_02134200.z;
    self->mdCcAcPos_c.SetPosRelativeToActor(offset);
    unsigned int id = self->mdCcAcPos_c.otherOwner;
    if (id == 0)
        return;
    dActor_c *actor = dActor_c::FindWithID(id);
    int isPlayer = (int)(actor->actorID == MANTA_PLAYER);
    if (isPlayer == 0)
        return;
    {
        Vector3 hit;
        hit.x = self->mPosX;
        hit.y = self->mPosY;
        hit.z = self->mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
            (Player *)actor, hit, 1, 0xc000, 1, 0, 1);
    }
}

/* State execute. Advance the swim anim, hurt, drop the next ring,
 * score an in-order pass, and spawn the star once five have landed. */
// @symbol func_ov090_021327e4
extern "C" int func_ov090_021327e4(daManta_c *self)
{
    dActor_c *spawned;
    Vector3 num1;
    Vector3 num2;

    self->mModelAnim.Advance();
    func_ov090_02132730(self);

    if (self->mRingCount >= MANTA_RING_GOAL)
        goto Ldecay;

    if (*(unsigned short *)&self->mStateTimer == 0) {
        spawned = dActor_c::Spawn(
            MANTA_WATER_RING, 1, self->mRingPos, (Vector3_16 *)&self->mAngleX,
            self->mAreaId, -1);
        if (spawned != 0) {
            self->mRingIDs[self->mRingWrite] = spawned->uniqueID;
            self->mRingWrite += 1;
            if (self->mRingWrite >= MANTA_RING_SLOTS)
                self->mRingWrite = 0;
            ((daWater_Ring_c *)spawned)->unk_38c = (char *)self;
        }
        self->mStateTimer = MANTA_RING_INTERVAL;
    }

    if (self->mHitRing == 0)
        goto Ldecay;

    if (self->mRingCount == 0) {
        int i;
        int key = self->mHitRing->uniqueID;
        for (i = 0; i < MANTA_RING_SLOTS; i++) {
            int slot = self->mRingIDs[i];
            if (slot == key) {
                self->mRingRead = i;
                self->mRingCount += 1;
                func_02012790(MANTA_COLLECT_JINGLE);
                num1 = *(Vector3 *)&self->mHitRing->mPosX;
                self->SpawnNumber(num1, self->mRingCount, 0, 0, 0);
                self->mHitRing = 0;
                return 1;
            }
        }
        goto Lreset;
    } else {
        self->mRingRead += 1;
        if (self->mRingRead >= MANTA_RING_SLOTS)
            self->mRingRead = 0;
        /* A direct != here is the same loads and then `cmp r1, r0`.
         * The cartridge has `cmp r0, r1`. The two ints keep that order. */
        int slotAddr = self->mRingRead;
        int ringId = (int)self->mHitRing;
        slotAddr = (int)((char *)self + (slotAddr << 2));
        ringId = *(int *)(ringId + 4);
        slotAddr = *(int *)(slotAddr + 0x3ac);
        if (slotAddr != ringId)
            goto Lreset;
        self->mRingCount += 1;
        func_02012790(MANTA_COLLECT_JINGLE);
        num2 = *(Vector3 *)&self->mHitRing->mPosX;
        self->SpawnNumber(num2, self->mRingCount, 0, 0, 0);
        self->mHitRing = 0;
        return 1;
    }

Lreset:
    self->mRingCount = 0;
    self->mRingRead = 0;
    self->mHitRing = 0;
Ldecay:
    if (self->mRingCount == MANTA_RING_GOAL) {
        self->mStarDelay += 1;
        if (self->mStarDelay > MANTA_STAR_WAIT) {
            dActor_c::Spawn(
                MANTA_STAR, self->mStarID | MANTA_STAR_FLAG,
                *(Vector3 *)&self->mPosX, (Vector3_16 *)&self->mAngleX,
                self->mAreaId, -1);
            self->mRingCount = MANTA_STAR_SPAWNED;
        }
    }
    return 1;
}

/* State init. Swim anim at 1.0, empty ring list. */
// @symbol func_ov090_02132a58
extern "C" int func_ov090_02132a58(daManta_c *self)
{
    MantaFileWord *anim = (MantaFileWord *)&data_ov090_0213452c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &self->mModelAnim, anim->file, 0, 0x1000, 0);
    self->mHitRing = 0;
    self->mRingWrite = 0;
    self->mRingRead = 0;
    self->mModelAnim.speed = 0x1000;
    int i;
    for (i = 0; i < MANTA_RING_SLOTS; i++)
        self->mRingIDs[i] = 0;
    return 1;
}

/* Install state and run its init PMF. A null init is "do nothing". */
// @symbol func_ov090_02132ac4
extern "C" int func_ov090_02132ac4(daManta_c *self, MantaState *state)
{
    self->mState = state;
    MantaState *current = self->mState;
    if (current->init == 0)
        return 1;
    return (self->*(current->init))();
}

/* Position >> 3 and the angle triple become the model matrix. Bone 3's
 * transform is then folded in, and its translation << 3 is where the
 * next ring spawns. */
// @symbol func_ov090_02132b14
extern "C" void func_ov090_02132b14(daManta_c *self)
{
    Vector3 scaled;
    Vec3_Asr(&scaled, (Vector3 *)&self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, scaled.x, scaled.y, scaled.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(
        &data_020a0e68, self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModelAnim.mat4x3 = data_020a0e68;
    self->mRingPos.x = 0;
    self->mRingPos.y = 0;
    self->mRingPos.z = 0;
    data_020a0e68 = self->mModelAnim.mat4x3;
    MulMat4x3Mat4x3(
        (const int *)(self->mModelAnim.data.transforms + 3),
        data_020a0e68.m, data_020a0e68.m);
    self->mRingPos.x = data_020a0e68.m[9];
    self->mRingPos.y = data_020a0e68.m[10];
    self->mRingPos.z = data_020a0e68.m[11];
    self->mRingPos.x <<= 3;
    self->mRingPos.y <<= 3;
    self->mRingPos.z <<= 3;
}

// @symbol _ZN9daManta_c16CleanupResourcesEv
int daManta_c::CleanupResources()
{
    data_ov090_02134524.Release();
    data_ov002_0210da10.Release();
    data_ov002_0210d9a8.Release();
    data_ov090_0213452c.Release();
    return 1;
}

// @symbol _ZN9daManta_c16OnPendingDestroyEv
void daManta_c::OnPendingDestroy()
{
}

// @symbol _ZN9daManta_c6RenderEv
int daManta_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN9daManta_c8BehaviorEv
int daManta_c::Behavior()
{
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    {
        MantaState *state = mState;
        /* The execute word, not &state->execute: taking the address of a
         * pointer-to-member materialises the whole 8-byte PMF. */
        if (*(int *)((char *)state + 8) != 0)
            (this->*(state->execute))();
    }
    {
        PathPtr path;
        Vector3 node;
        Vector3 diff;
        Vector3 forward;
        int len;

        path.FromID(*(unsigned int *)&mPathID);
        path.GetNode(node, *(unsigned int *)&mPathNode);
        Vec3_Sub(&diff, (Vector3 *)&mPosX, &node);
        len = LenVec3(&diff);
        if (len == 0 || len <= MANTA_NODE_REACH) {
            mPathNode++;
            if (mPathNode >= mNumNodes)
                mPathNode = 0;
        }
        ApproachLinear(mPrevAngleY, Vec3_HorzAngle((Vector3 *)&mPosX, &node), 0x60);
        ApproachLinear(mPrevAngleX, Vec3_VertAngle((Vector3 *)&mPosX, &node), 0x40);
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        forward.y = forward.x = forward.z = 0;
        forward.z = MANTA_FORWARD;
        Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&forward, &data_020a0e68, (Vector3 *)&unk_0a4);
    }
    {
        int s = mVertSpeed + mVertAccel;
        int m2 = mTerminalVelocity;
        int ac = unk_0ac;
        if (s >= m2)
            m2 = s;
        mVertSpeed = m2;
        unk_0ac = ac;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    func_ov090_02132b14(this);
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN9daManta_c13InitResourcesEv
int daManta_c::InitResources()
{
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov090_02134524), 1, -1);
    Model::LoadFile(data_ov002_0210da10);
    Model::LoadFile(data_ov002_0210d9a8);
    Animation::LoadFile(data_ov090_0213452c);

    mPathID = param1 & 0xff;
    mStarID = (param1 >> 12) & 0xf;
    if (mPathID < 0)
        mPathID = 0;

    {
        PathPtr path;
        path.FromID(*(unsigned int *)&mPathID);
        mNumNodes = path.NumNodes();
    }

    mTerminalVelocity = -0x3c000;

    {
        Vector3 offset;
        offset.x = data_ov090_02134200.x;
        offset.y = data_ov090_02134200.y;
        offset.z = data_ov090_02134200.z;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mdCcAcPos_c, this, offset, 0x150000, 0xc8000, 0x200004, 0);
    }

    {
        PathPtr path;
        path.FromID(*(unsigned int *)&mPathID);
        mPathNode = 1;
        path.GetNode(*(Vector3 *)&mPosX, *(unsigned int *)&mPathNode);
    }

    {
        /* ContinueKuppaScriptIfNecessary stores 2 here. That visit snaps
         * the manta onto node 3 at a fixed pose and clears mFlags. */
        int kuppa = (int)(data_0209f2d8 == 2);
        if (kuppa != 0) {
            mPathNode = 3;
            mPrevAngleX = (short)0xf303;
            mPrevAngleY = 0xb50;
            mPrevAngleZ = 0;
            mPosX = (int)0xfdfb8000;
            mPosY = (int)0xff8f8000;
            mPosZ = 0x29a000;
            mFlags = 0;
        }
    }

    func_ov090_02132ac4(this, (MantaState *)&data_ov090_0213454c);
    return 1;
}
