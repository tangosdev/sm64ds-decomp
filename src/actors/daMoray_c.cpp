//cpp
/* daMoray_c — Jolly Roger Bay eel (MORAY).
 *
 * Five states, installed by func_ov016_02111bf0. The tables are filled by
 * __sinit_ov016_021136ec from the pointer-to-member words at 0x02114878:
 *   02114d8c  den wait     init 02111bac, execute 021119ec
 *   02114d9c  lunge        init 02111860, execute 02111758
 *   02114dac  swim out     init 02111994, execute 021118b4
 *   02114dbc  path swim    init 02111718, execute 021115c0
 *   02114d7c  retreat      init BookSwitch_Spawn, execute func_ov016_02111534
 * The retreat pair is not in this TU. BookSwitch_Spawn sets mStateTimer to
 * 100 and mHorzSpeed to 0x14000; func_ov016_02111534 restores the spawn
 * angles when that timer hits 0 and goes back to den wait.
 *
 * Which variant InitResources keeps depends on data_0209f220 (the entrance
 * filter) and whether stars 1 and 2 of sublevel 8 are collected. Variant 2
 * lowers its home and waits in the den. Variant 1 carries a held STAR
 * (actor 0xb2). Variant 0 swims the path. Anything else returns 0.
 *
 * Destructors and daMoray_c_classInit are the neighbouring TUs.
 * func_ov016_02111284 (between them and this run) places the hit cylinders.
 * `#pragma defer_codegen off` keeps this file in ROM order. It also scopes
 * opt_propagation off on the den-wait execute and opt_strength_reduction
 * off on Render. common.h stays first: the pose reads the flat Matrix4x3
 * words m[9], m[10], m[11].
 *
 * deslop leftovers:
 * - BlendModelAnim::SetAnim with a Fix12<int> speed grows
 *   func_ov016_02111718 from 0x40 to 0x48 (+2 words). A named local and a
 *   compound literal cost the same. func_ov016_02111860, 02111994 and
 *   02111bac pass that same speed and keep the scalar extern.
 * - dCcAcPos_c::Init with two Fix12 locals grows InitResources from 0x3c8
 *   to 0x3d8 (+4 words) for the first cylinder alone. Both cylinders stay
 *   on the scalar extern.
 * - ApproachLinear(Vector3 &, Vector3 const &, Fix12<int>) grows
 *   func_ov016_02111758 from 0x108 to 0x110 (+2 words). 021118b4 and
 *   021119ec keep the scalar extern.
 * - dActor_c::SetRanges with four Fix12 arguments grows InitResources from
 *   0x3c8 to 0x410 (+0x48). Adding the method to dActor_c.h did not move
 *   the other fourteen functions here; the call still does not match, and
 *   the declaration is not left on the shared header.
 * - `currFrame >> 12` instead of `<< 4 >> 0x10` shrinks func_ov016_02111758
 *   from 0x108 to 0x104 and moves its pool (data_020a0e68 read as
 *   data_0209f220).
 * - GetNode into the stack vector the facing is taken against, instead of
 *   into mPos, is one word of InitResources: `add r1, r4, #0x5c` versus
 *   `add r1, sp, #0x24`. Node 0 is stored on the actor and then overwritten
 *   by node 1.
 * - Collapsing Behavior's star gotos shrinks Behavior from 0x1b0 to 0x1ac.
 * - Deleting `mVariant == 0xff` (the value was just masked to 4 bits)
 *   shrinks InitResources from 0x3c8 to 0x3b8. Deleting `mPathID < 0`
 *   does the same. mwcc still emits both tests.
 * - Deleting the second PathPtr::FromID, whose result is unused, shrinks
 *   InitResources from 0x3c8 to 0x3b4.
 * - Indexing mSegmentPos[i] and transforms[i] shrinks func_ov016_02111c40
 *   from 0x2f8 to 0x2e8. The walk through a daMoray_c* advanced one Vector3
 *   at a time is what the ROM does.
 * - ModelComponents::UpdateBones, passing mBlendModelAnim.file and the same
 *   frame shift, grows Render from 0x8c to 0x9c. Render keeps func_020167a4,
 *   which is that wrapper.
 * - Sound::Play(3, 0xfa, pos) grows func_ov016_02111860 from 0x54 to 0x58.
 *   func_02012694 is Play with bank 3. The other call sites use it too.
 */

#include "common.h"
#include "daMoray_c.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "Animation.h"
#include "Model.h"
#include "Player.h"

#pragma defer_codegen off

/* decl_common.h types the four file handles as char[] and the state tables
 * as char / void *. This TU repeats the names it uses. SharedFilePtr.h
 * declares no fields; __sinit_ov016_021136ec constructs each handle with a
 * file id, and SetAnim reads the loaded file at +4. */
struct MorayFile {
    s32 fileId;                 /* 0x00 */
    void *file;                 /* 0x04 -- the pointer SetAnim is handed */
};

extern SharedFilePtr data_ov016_02114d38;   /* model, file 0x3b5 */
extern SharedFilePtr data_ov016_02114d20;   /* lunge anim, file 0x3b6 */
extern SharedFilePtr data_ov016_02114d30;   /* path-swim anim, file 0x3b7 */
extern SharedFilePtr data_ov016_02114d28;   /* den / swim-out anim, file 0x3b8 */

extern "C" {
/* State tables filled by __sinit_ov016_021136ec from the PMFs at 0x02114878.
 * 02114d7c retreat (BookSwitch_Spawn, then func_ov016_02111534),
 * 02114d8c den wait, 02114d9c lunge, 02114dac swim out, 02114dbc path swim. */
extern daMoray_c::State data_ov016_02114d7c;
extern daMoray_c::State data_ov016_02114d8c;
extern daMoray_c::State data_ov016_02114d9c;
extern daMoray_c::State data_ov016_02114dac;
extern daMoray_c::State data_ov016_02114dbc;
extern Vector3 data_ov016_02114d4c;         /* cylinder offset, bss */

extern unsigned char data_0209f220; /* entrance filter */
extern Matrix4x3 data_020a0e68;             /* the shared scratch matrix */

int AngleDiff(int a, int b);
int SublevelToLevel(int sub);
int IsStarCollected(int level, int star);
u16 DecIfAbove0_Short(u16 *p);
/* Sound::Play(3, id, pos). Calling Play directly is one word larger
   (func_ov016_02111860 0x54 -> 0x58). */
void func_02012694(unsigned int id, const Vector3 *pos);
/* UpdateBones(model.data, model.file, frame). Inlining that grows Render
   0x8c -> 0x9c. */
void func_020167a4(BlendModelAnim *model);
/* Blend the posed bones by blendWeight. No named method. */
void func_0204531c(ModelComponents *data, s32 weight);

short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
short Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void Vec3_Sub(void *out, void *a, void *b);
void SubVec3(void *a, void *b, void *out);
int LenVec3(void *v);
void Vec3_MulScalar(void *out, void *v, int s);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);

void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_FromRotationY(Matrix4x3 *m, int angY);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, short angX);
void Matrix4x3_ApplyInPlaceToRotationZXYExt(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void MulMat4x3Mat4x3(void *m1, void *m0, void *mF);

void func_ov016_02111284(void *actor);
int func_ov016_02111bf0(daMoray_c *self, const daMoray_c::State *state);

/* Header methods take Fix12<int> by value. Brace-init compiles and homes
   the argument (see the leftover list). These scalars are the calls that
   match. */
void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *file, int blendFrames, int flags, int speed, unsigned short startFrame);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(dActor_c *self, int offsetY, int radius, int clipDistance, int farDistance);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset, int radius, int height, u32 flags, u32 vulnFlags);
int _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 &cur, const Vector3 &target, int step);
}

/* Render's per-segment bend table, one 0xc record per body bone.
 * The six ROM records are {0, 0, scale} with scales 1,1,1,1,-1,0.
 * This loop reads only angleScale. */
struct MorayRenderStep {
    s32 pad_00;
    s32 pad_04;
    s32 angleScale;
};

/* Runtime pose elem filled by UpdateBones, stride 0x34. Rotation angles
 * sit at 0x1a/0x1c/0x1e; Render bends rotZ. Not BMD_Bone (file stride 0x40). */
struct MorayBone {
    u8 pad_00[0x1a];
    s16 rotX;
    s16 rotY;
    s16 rotZ;
    u8 pad_20[0x14];
};

/* The carried star keeps its carrier matrix at dActor_c+0xc8, inside pad_0c5.
   Other actors use that word differently, so it is not a dActor_c field. */
struct MorayStarCarrier {
    u8 pad[0xc8];
    Matrix4x3 *mtx;
};

extern "C" MorayRenderStep data_ov016_02114908[];

bool ApproachLinear(short &value, short target, short step);
namespace cstd { int fdiv(int a, int b); }

/* Path-swim execute. Turns toward the current node and steps to the next
   one on arrival. Variant 1, back at node 0, enters the retreat state. */
// @symbol func_ov016_021115c0
extern "C" int func_ov016_021115c0(daMoray_c *self)
{
    PathPtr path;
    Vector3 node;
    Vector3 diff;
    Vector3 scaled;
    int len;
    short ha;

    path.FromID(self->mPathID);
    path.GetNode(node, self->mPathNodeIndex);

    ha = Vec3_HorzAngle((Vector3 *)&self->mPosX, &node);
    {
        int d = AngleDiff(ha, self->mPrevAngleY);
        self->mSegmentAngle[7] = d / 2;
    }
    ApproachLinear(self->mPrevAngleY, ha, 0x80);

    ApproachLinear(self->mPrevAngleX, Vec3_VertAngle((Vector3 *)&self->mPosX, &node), 0x80);

    Vec3_Sub(&diff, (Vector3 *)&self->mPosX, &node);
    len = LenVec3(&diff);
    if (len == 0 || len <= 0xa000) {
        if (self->mVariant == 1 && self->mPathNodeIndex == 0) {
            self->mPathNodeIndex = 1;
            func_ov016_02111bf0(self, &data_ov016_02114d7c);
            return 1;
        }
        self->mPathNodeIndex++;
        if (self->mPathNodeIndex >= self->mPathNodeCount)
            self->mPathNodeIndex = 0;
        func_02012694(0xfa, (const Vector3 *)&self->mCamSpacePosX);
    } else {
        Vec3_MulScalar(&scaled, &diff, cstd::fdiv(0xa000, len));
        SubVec3((Vector3 *)&self->mPosX, &scaled, (Vector3 *)&self->mPosX);
    }
    return 1;
}

/* Path-swim init: the looping swim animation. */
// @symbol func_ov016_02111718
extern "C" int func_ov016_02111718(daMoray_c *self)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, ((MorayFile *)&data_ov016_02114d30)->file, 8, 0, 0x1000, 0);
    return 1;
}

/* Lunge execute. On bite frames 0x15..0x3c, slides toward a point along
   its facing (farther when the entrance filter is 1), then swim-out. */
// @symbol func_ov016_02111758
extern "C" int func_ov016_02111758(daMoray_c *self)
{
    Vector3 in;
    Vector3 out;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    unsigned int frame = (unsigned int)self->mBlendModelAnim.currFrame << 4 >> 0x10;
    if (frame >= 0x15 && frame <= 0x3c) {
        if (data_0209f220 == 1) {
            in.z = 0x2bc000;
        } else {
            in.z = 0x1f4000;
        }
        Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, self->mAngleX);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        out.x += self->mHomePosX;
        out.y += self->mHomePosY;
        out.z += self->mHomePosZ;
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(*(Vector3 *)&self->mPosX, out, 0x14000);
    }
    if (self->mBlendModelAnim.Finished()) {
        self->mHorzSpeed = 0;
        func_ov016_02111bf0(self, &data_ov016_02114dac);
    }
    return 1;
}

/* Lunge init: bite animation with flag bit 30, and the lunge sound. */
// @symbol func_ov016_02111860
extern "C" int func_ov016_02111860(daMoray_c *self)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, ((MorayFile *)&data_ov016_02114d20)->file, 8, 0x40000000, 0x1000, 0);
    func_02012694(0xfa, (const Vector3 *)&self->mCamSpacePosX);
    return 1;
}

/* Swim-out execute. Approaches a point 0x76c000 along its facing, then
   starts path-swim. */
// @symbol func_ov016_021118b4
extern "C" int func_ov016_021118b4(daMoray_c *self)
{
    Vector3 in;
    Vector3 out;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    in.z = 0x76c000;
    Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, self->mAngleX);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    out.x += self->mHomePosX;
    out.y += self->mHomePosY;
    out.z += self->mHomePosZ;
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(*(Vector3 *)&self->mPosX, out, 0x14000);
    if (Vec3_Dist((Vector3 *)&self->mPosX, &out) < 0x14000) {
        func_02012694(0xfa, (const Vector3 *)&self->mCamSpacePosX);
        func_ov016_02111bf0(self, &data_ov016_02114dbc);
    }
    return 1;
}

/* Swim-out init: idle animation and the same sound. */
// @symbol func_ov016_02111994
extern "C" int func_ov016_02111994(daMoray_c *self)
{
    self->unk_400 = 0;
    func_02012694(0xfa, (const Vector3 *)&self->mCamSpacePosX);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, ((MorayFile *)&data_ov016_02114d28)->file, 8, 0, 0x1000, 0);
    return 1;
}

/* Den-wait execute. Returns to the home point, then lunges when a player
   is inside the cone in front. Entrance 1 uses a wider cone and an extra
   point along the facing. */
#pragma push
#pragma opt_propagation off
// @symbol func_ov016_021119ec
extern "C" int func_ov016_021119ec(daMoray_c *self)
{
    Player *player;
    Vector3 playerPos;
    Vector3 local;
    Vector3 world;
    int thrHorz;
    int thrVert;
    int thrAng;
    unsigned char stage;
    int zero;
    int dy;

    if (Vec3_Dist((const Vector3 *)&self->mPosX, (const Vector3 *)&self->mHomePosX) > 0xa000) {
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(*(Vector3 *)&self->mPosX, *(const Vector3 *)&self->mHomePosX, 0x5000);
        return 1;
    }

    self->mPosX = self->mHomePosX;
    self->mPosY = self->mHomePosY;
    self->mPosZ = self->mHomePosZ;

    player = self->ClosestPlayer();
    if (player != 0) {
        /* ACDB order: x,y,z then stage -- colors y/z as r2 and stage as r1 */
        const Vector3 *pos = (const Vector3 *)&player->mPosX;
        playerPos.x = pos->x;
        playerPos.y = pos->y;
        playerPos.z = pos->z;
        stage = data_0209f220;

        thrHorz = 0x3e8000;
        thrAng = 0x3300;
        thrVert = 0x418000;
        thrVert += 0x3e8000;

        if (stage == 1) {
            zero = 0;
            local.z = zero;
            local.z = 0x64000;
            local.x = zero;
            local.y = zero;
            world.x = zero;
            world.y = zero;
            world.z = zero;
            thrHorz = 0x495000;
            thrVert = 0x6ee000;
            Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, self->mAngleX);
            MulVec3Mat4x3(&local, &data_020a0e68, &world);
            world.x += self->mHomePosX;
            world.y += self->mHomePosY;
            world.z += self->mHomePosZ;
            if (Vec3_Dist(&playerPos, &world) < 0x224000)
                func_ov016_02111bf0(self, &data_ov016_02114d9c);
        }

        if (AngleDiff(self->HorzAngleToCPlayer(), self->mAngleY) < thrAng) {
            if (Vec3_HorzDist((Vector3 *)&self->mPosX, &playerPos) < thrHorz) {
                dy = self->mPosY - playerPos.y;
                if (dy < 0)
                    dy = -dy;
                if (dy < thrVert)
                    func_ov016_02111bf0(self, &data_ov016_02114d9c);
            }
        }
    }
    return 1;
}
#pragma pop

/* Den-wait init: the idle animation. */
// @symbol func_ov016_02111bac
extern "C" int func_ov016_02111bac(daMoray_c *self)
{
    self->unk_400 = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, ((MorayFile *)&data_ov016_02114d28)->file, 8, 0, 0x1000, 0);
    return 1;
}

/* Installs a state and runs its init. */
// @symbol func_ov016_02111bf0
extern "C" int func_ov016_02111bf0(daMoray_c *self, const daMoray_c::State *state)
{
    self->mState = state;
    const daMoray_c::State *installed = self->mState;
    if (installed->init == 0)
        return 1;
    return (self->*installed->init)();
}

/* Poses the body: the model matrix from position and angles, then each
   segment's world position out of its bone transform, and the carried
   star's matrix off the jaw bone. */
// @symbol func_ov016_02111c40
extern "C" void func_ov016_02111c40(daMoray_c *self)
{
    Vector3 in;
    Vector3 out;
    Vector3 asr;
    int zero[1];
    int i;
    daMoray_c *row;
    int base;
    Vector3 *pos;
    Matrix4x3 *mat_src;
    dActor_c *star;
    u32 id;

    Vec3_Asr(&asr, (Vector3 *)&self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    in.z = 0;
    in.z = -0x190000;
    in.x = 0;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    Matrix4x3_FromTranslation(&data_020a0e68, (self->mPosX + out.x) >> 3, self->mPosY >> 3,
                              (self->mPosZ + out.z) >> 3);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mBlendModelAnim.mat4x3 = data_020a0e68;
    ApproachLinear(self->mSegmentAngle[6], self->mSegmentAngle[7], 0x40);
    i = 0;
    mat_src = &self->mBlendModelAnim.mat4x3;
    self->mSegmentAngle[5] = self->mSegmentAngle[6];
    /* mSegmentPos[i] / transforms[i] is 0x10 smaller than this walk
       (func_ov016_02111c40 0x2f8 -> 0x2e8). row steps one Vector3 at a time
       so the clear and the << 3 hit mSegmentPos[i] through the actor base;
       pos receives the bone translation before that shift. */
    row = self;
    base = i;
    pos = self->mSegmentPos;
    zero[0] = i;
    do {
        Matrix4x3 *dst;
        row->mSegmentPos[0].x = zero[0];
        row->mSegmentPos[0].y = zero[0];
        row->mSegmentPos[0].z = zero[0];
        dst = &data_020a0e68;
        *dst = *mat_src;
        MulMat4x3Mat4x3((char *)self->mBlendModelAnim.data.transforms + base, dst, dst);
        pos->x = dst->m[9];
        pos->y = dst->m[10];
        pos->z = dst->m[11];
        row->mSegmentPos[0].x = row->mSegmentPos[0].x << 3;
        row->mSegmentPos[0].y = row->mSegmentPos[0].y << 3;
        row->mSegmentPos[0].z = row->mSegmentPos[0].z << 3;
        row = (daMoray_c *)((char *)row + sizeof(Vector3));
        base += 0x30;
        pos++;
        i++;
    } while (i < 7);
    id = self->mStarUniqueID;
    if (id == 0) {
        return;
    }
    star = dActor_c::FindWithID(id);
    if (star == 0) {
        return;
    }
    self->mStarPos.x = 0;
    self->mStarPos.y = 0;
    self->mStarPos.z = 0;
    MulMat4x3Mat4x3(&self->mBlendModelAnim.data.transforms[4], &self->mBlendModelAnim.mat4x3, &self->mStarMtx);
    Matrix4x3_FromTranslation(&data_020a0e68, 0x28000, 0, 0);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x4000, -0x8000, self->mStarSpinAngle);
    MulMat4x3Mat4x3(&data_020a0e68, &self->mStarMtx, &self->mStarMtx);
    data_020a0e68 = self->mStarMtx;
    self->mStarPos.x = data_020a0e68.m[9];
    self->mStarPos.y = data_020a0e68.m[10];
    self->mStarPos.z = data_020a0e68.m[11];
    self->mStarPos.x <<= 3;
    self->mStarPos.y <<= 3;
    self->mStarPos.z <<= 3;
    ((MorayStarCarrier *)star)->mtx = &self->mStarMtx;
}

// @symbol _ZN9daMoray_c16CleanupResourcesEv
s32 daMoray_c::CleanupResources()
{
    data_ov016_02114d38.Release();
    data_ov016_02114d20.Release();
    data_ov016_02114d30.Release();
    data_ov016_02114d28.Release();
    return 1;
}

// @symbol _ZN9daMoray_c16OnPendingDestroyEv
void daMoray_c::OnPendingDestroy()
{
}

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN9daMoray_c6RenderEv
s32 daMoray_c::Render()
{
    int i;
    MorayRenderStep *step;
    MorayBone *bone;

    func_020167a4(&mBlendModelAnim);
    bone = (MorayBone *)mBlendModelAnim.data.bones + 1;
    step = data_ov016_02114908;
    for (i = 1; i < 7; i++) {
        s16 movement = mSegmentAngle[i];
        u16 *angle = (u16 *)&bone->rotZ;
        *angle = *angle + (u16)(s16)(movement * step->angleScale);
        step++;
        bone++;
    }
    func_0204531c(&mBlendModelAnim.data, mBlendModelAnim.blendWeight);
    mBlendModelAnim.Model::Render(0);
    return 1;
}
#pragma pop

// @symbol _ZN9daMoray_c8BehaviorEv
s32 daMoray_c::Behavior()
{
    dActor_c *star;
    unsigned id;

    DecIfAbove0_Short((u16 *)&mStateTimer);
    if (mState->execute)
        (this->*mState->execute)();
    UpdatePos(&mdCcAcPos_c1);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov016_02111c40(this);

    id = mStarUniqueID;
    if (id != 0) {
        star = FindWithID(id);
        if (star == 0)
            goto clear_id;
        if (mVariant == 1) {
            Player *player = ClosestPlayer();
            if (player != 0) {
                if (Vec3_Dist(&mStarPos, (const Vector3 *)&player->mPosX) < 0xfa000) {
                    star->MarkForDestruction();
                    /* 0xb2 is STAR. 0x40 releases it; InitResources spawned it held (| 0x50). */
                    Spawn(0xb2, mStarParam | 0x40, mStarPos, 0, mAreaId, -1);
                    mStarUniqueID = 0;
                    goto after_first;
                }
            }
        }
        goto after_first;
    clear_id:
        mStarUniqueID = 0;
    after_first:
        if (mStarUniqueID != 0) {
            star->mPosX = mStarPos.x;
            star->mPosY = mStarPos.y;
            star->mPosZ = mStarPos.z;
            mStarSpinAngle += 0x1000;
        }
    }

    func_ov016_02111284(this);
    mdCcAcPos_c1.Clear();
    mdCcAcPos_c1.Update();
    if (data_0209f220 == 1 && mState != &data_ov016_02114dbc) {
        mdCcAcPos_c2.Clear();
        mdCcAcPos_c2.Update();
    }
    mBlendModelAnim.Advance();
    return 1;
}

// @symbol _ZN9daMoray_c13InitResourcesEv
s32 daMoray_c::InitResources()
{
    Vector3 node;
    Vector3 v1;
    Vector3 v2;
    int i;
    void *file;
    dActor_c *spawned;

    file = Model::LoadFile(data_ov016_02114d38);
    mBlendModelAnim.SetFile((BMD_File *)file, 1, -1);
    Animation::LoadFile(data_ov016_02114d20);
    Animation::LoadFile(data_ov016_02114d30);
    Animation::LoadFile(data_ov016_02114d28);

    mPathID = param1 & 0xff;
    mVariant = (param1 >> 8) & 0xf;
    mStarParam = (param1 >> 0xc) & 0xf;
    if (mVariant == 0xff)
        mVariant = 0;
    if (mPathID < 0)
        mPathID = 0;

    {
        PathPtr path;
        path.FromID(mPathID);
        mPathNodeCount = path.NumNodes();
    }

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mTerminalVelocity = -0x1e000;
    v1 = data_ov016_02114d4c;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c1, this, &v1, 0x32000, 0x50000, 0x200004, 0);
    v2 = data_ov016_02114d4c;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c2, this, &v2, 0x32000, 0x50000, 0x200000, 0);

    /* Second bind is unused. Dropping the constructor and FromID shrinks
       InitResources by 0x14, so the ROM really does call them. */
    {
        PathPtr path;
        path.FromID(mPathID);
    }
    mPathNodeIndex = 1;
    mStarUniqueID = 0;
    mBlendModelAnim.speed = 0x1000;

    /* Entrance 1, or star 1 still uncollected: only variant 2. */
    if (data_0209f220 == 1)
        goto check_param2;
    if (IsStarCollected(SublevelToLevel(8), 1) != 0)
        goto stage2_path;

check_param2:
    if (mVariant != 2)
        goto ret0_a;
    mHomePosY -= 0x80000;
    mPathNodeIndex = 8;
    if (mPathNodeIndex >= mPathNodeCount)
        mPathNodeIndex = 4;
    mPosY = mHomePosY;
    func_ov016_02111bf0(this, &data_ov016_02114d8c);
    goto tail;
ret0_a:
    return 0;

stage2_path:
    /* Entrance 2, or star 2 still uncollected: only variant 1. */
    if (data_0209f220 == 2)
        goto check_param1;
    if (IsStarCollected(SublevelToLevel(8), 2) != 0)
        goto check_param0;

check_param1:
    if (mVariant != 1)
        goto ret0_b;
    /* STAR, held (| 0x50). Behavior releases it with | 0x40. */
    spawned = Spawn(0xb2, mStarParam | 0x50, *(Vector3 *)&mPosX, 0, mAreaId, -1);
    if (spawned != 0) {
        mStarUniqueID = spawned->uniqueID;
        spawned->mFlags = 0;
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(spawned, 0, 0x3e8000, 0x1f40000, 0x1f40000);
    }
    func_ov016_02111bf0(this, &data_ov016_02114d8c);
    goto tail;
ret0_b:
    return 0;

check_param0:
    if (mVariant != 0)
        goto ret0_c;
    {
        PathPtr path;
        path.FromID(mPathID);
        /* Both nodes are written to mPos; the second sticks. The facing
           below is against `node`, which this block does not fill. Sending
           node 0 there is the one-word DIFF in the leftover list. */
        path.GetNode(*(Vector3 *)&mPosX, 0);
        path.GetNode(*(Vector3 *)&mPosX, 1);
    }
    mPrevAngleY = Vec3_HorzAngle((Vector3 *)&mPosX, &node);
    mAngleY = mPrevAngleY;
    func_ov016_02111bf0(this, &data_ov016_02114dbc);
    goto tail;
ret0_c:
    return 0;

tail:
    for (i = 0; i < 7; i++) {
        /* array index form -> add r0,r4,ip,lsl#1 (not strength-reduced i*2) */
        mSegmentAngle[i] = 0;
        mSegmentAngle[7] = 0;
        mSegmentPos[i].x = mPosX;
        mSegmentPos[i].y = mPosY;
        mSegmentPos[i].z = mPosZ;
    }
    mInitAngleX = mAngleX;
    mInitAngleY = mAngleY;
    mInitAngleZ = mAngleZ;
    return 1;
}
