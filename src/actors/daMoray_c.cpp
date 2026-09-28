//cpp
/* daMoray_c, the Jolly Roger Bay eel (MORAY).
 * 15 function(s), .text 0x021115c0..0x02112588.
 *
 * NAME: _ZTS9daMoray_c is "9daMoray_c" at ov016 0x021148c8; _ZTI at
 * 0x021148d4; the vtable address point is 0x02114958. The tree previously
 * called the class Unagi.
 *
 * D1 (0x021111a0) and D0 (0x02111208) stay shards. func_ov016_02111284,
 * func_ov016_02111534 and BookSwitch_Spawn sit between those destructors
 * and this run, so the destructor is not defined here. The factory
 * daMoray_c_classInit (0x02112588) is the next TU.
 *
 * The first eight functions are the state bodies daMoray_c::State points
 * at, and func_ov016_02111c40 poses the body bones every frame. Their ROM
 * names are unrecovered, so they stay extern "C" free functions taking the
 * eel explicitly; a member spelling would mangle to a symbol that does not
 * exist.
 *
 * `#pragma defer_codegen off` lays .text down in source order, so this
 * file is ROM-ascending, and it binds the two member pragmas:
 * opt_propagation off on func_ov016_021119ec, opt_strength_reduction off
 * on Render. common.h is first so the flat Matrix4x3 (m[12]) stands;
 * func_ov016_02111c40 reads m[9], m[10] and m[11].
 */

#include "common.h"
#include "daMoray_c.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "Animation.h"
#include "Model.h"
#include "decl_PathPtr.h"
#include "Player.h"

#pragma defer_codegen off

/* decl_common.h types the four file handles as char[] and the state tables
 * as char / void *; this TU repeats the names it uses rather than include
 * that header. The state tables keep those types and are cast where they
 * are installed. A handle's loaded file is its second word; SharedFilePtr.h
 * deliberately declares no fields. */
extern SharedFilePtr data_ov016_02114d38;   /* model */
extern SharedFilePtr data_ov016_02114d20;   /* animation */
extern SharedFilePtr data_ov016_02114d30;   /* animation */
extern SharedFilePtr data_ov016_02114d28;   /* animation */

extern "C" {
extern char data_ov016_02114d7c;
extern void *data_ov016_02114d8c;
extern void *data_ov016_02114d9c;
extern void *data_ov016_02114dac;
extern void *data_ov016_02114dbc;
extern Vector3 data_ov016_02114d4c;

extern unsigned char data_0209f220;
extern Matrix4x3 data_020a0e68;             /* the shared scratch matrix */

int AngleDiff(int a, int b);
int SublevelToLevel(int sub);
int IsStarCollected(int level, int star);
u16 DecIfAbove0_Short(u16 *p);
void func_02012694(int a, void *p);
void func_020167a4(BlendModelAnim *model);
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

/* local extern: the header spellings take Fix12<int> by value, and Fix12
   has no int constructor, so a call through them cannot be written with
   these literals (measured: dCcAcPos_c::Init fails to compile). */
void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *file, int blendFrames, int flags, int speed, unsigned short startFrame);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(dActor_c *self, int offsetY, int radius, int clipDistance, int farDistance);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset, int radius, int height, u32 flags, u32 vulnFlags);
int _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 &cur, const Vector3 &target, int step);
/* local extern: include/ModelBase.h does not declare SetFile. */
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
}

/* Render's per-segment bend table, one 0xc record per body bone. */
struct MorayRenderStep {
    s32 unk_00;
    s32 unk_04;
    s32 angleScale;
};

extern "C" MorayRenderStep data_ov016_02114908[];

bool ApproachLinear(short &value, short target, short step);
namespace cstd { int fdiv(int a, int b); }

/* Swims to the current path node, turning toward it, and steps to the next
   node on arrival. */
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
            func_ov016_02111bf0(self, (const daMoray_c::State *)&data_ov016_02114d7c);
            return 1;
        }
        self->mPathNodeIndex++;
        if (self->mPathNodeIndex >= self->mPathNodeCount)
            self->mPathNodeIndex = 0;
        func_02012694(0xfa, &self->mCamSpacePosX);
    } else {
        Vec3_MulScalar(&scaled, &diff, cstd::fdiv(0xa000, len));
        SubVec3((Vector3 *)&self->mPosX, &scaled, (Vector3 *)&self->mPosX);
    }
    return 1;
}

// @symbol func_ov016_02111718
extern "C" int func_ov016_02111718(daMoray_c *self)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, ((void **)&data_ov016_02114d30)[1], 8, 0, 0x1000, 0);
    return 1;
}

/* Lunges out of the den along its facing while the bite frames play. */
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
        func_ov016_02111bf0(self, (const daMoray_c::State *)&data_ov016_02114dac);
    }
    return 1;
}

// @symbol func_ov016_02111860
extern "C" int func_ov016_02111860(daMoray_c *self)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, ((void **)&data_ov016_02114d20)[1], 8, 0x40000000, 0x1000, 0);
    func_02012694(0xfa, &self->mCamSpacePosX);
    return 1;
}

/* Swims out to the far point along its facing, then waits there. */
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
        func_02012694(0xfa, &self->mCamSpacePosX);
        func_ov016_02111bf0(self, (const daMoray_c::State *)&data_ov016_02114dbc);
    }
    return 1;
}

// @symbol func_ov016_02111994
extern "C" int func_ov016_02111994(daMoray_c *self)
{
    self->unk_400 = 0;
    ((void (*)(int, void *, int))func_02012694)(0xfa, &self->mCamSpacePosX, 0);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, ((void **)&data_ov016_02114d28)[1], 8, 0, 0x1000, 0);
    return 1;
}

/* Waits in the den, and bites when a player swims into the cone in front. */
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
                func_ov016_02111bf0(self, (const daMoray_c::State *)&data_ov016_02114d9c);
        }

        if (AngleDiff(self->HorzAngleToCPlayer(), self->mAngleY) < thrAng) {
            if (Vec3_HorzDist((Vector3 *)&self->mPosX, &playerPos) < thrHorz) {
                dy = self->mPosY - playerPos.y;
                if (dy < 0)
                    dy = -dy;
                if (dy < thrVert)
                    func_ov016_02111bf0(self, (const daMoray_c::State *)&data_ov016_02114d9c);
            }
        }
    }
    return 1;
}
#pragma pop

// @symbol func_ov016_02111bac
extern "C" int func_ov016_02111bac(daMoray_c *self)
{
    self->unk_400 = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, ((void **)&data_ov016_02114d28)[1], 8, 0, 0x1000, 0);
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
    /* The ROM reaches each segment twice: pos for the transform result, and
       row, a base stepping one Vector3 at a time and addressed at
       mSegmentPos's own offset, for the clear and the shift. Indexing the
       array instead (mSegmentPos[i]) costs 17 words. */
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
    /* 0xc8 falls in dActor_c's unnamed pad_0c5: the star reads its carrier's matrix there. */
    *(Matrix4x3 **)((char *)star + 0xc8) = &self->mStarMtx;
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
    char *bone;

    func_020167a4(&mBlendModelAnim);
    bone = (char *)mBlendModelAnim.data.bones + 0x34;
    step = data_ov016_02114908;
    for (i = 1; i < 7; i++) {
        s16 movement = mSegmentAngle[i];
        u16 *angle = (u16 *)(bone + 0x1e);
        *angle = *angle + (u16)(s16)(movement * step->angleScale);
        step++;
        bone += 0x34;
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
    if (data_0209f220 == 1 && mState != (const State *)&data_ov016_02114dbc) {
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
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mBlendModelAnim, file, 1, -1);
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

    {
        PathPtr path;
        path.FromID(mPathID);
    }
    mPathNodeIndex = 1;
    mStarUniqueID = 0;
    mBlendModelAnim.speed = 0x1000;

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
    func_ov016_02111bf0(this, (const State *)&data_ov016_02114d8c);
    goto tail;
ret0_a:
    return 0;

stage2_path:
    if (data_0209f220 == 2)
        goto check_param1;
    if (IsStarCollected(SublevelToLevel(8), 2) != 0)
        goto check_param0;

check_param1:
    if (mVariant != 1)
        goto ret0_b;
    spawned = Spawn(0xb2, mStarParam | 0x50, *(Vector3 *)&mPosX, 0, mAreaId, -1);
    if (spawned != 0) {
        mStarUniqueID = spawned->uniqueID;
        spawned->mFlags = 0;
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(spawned, 0, 0x3e8000, 0x1f40000, 0x1f40000);
    }
    func_ov016_02111bf0(this, (const State *)&data_ov016_02114d8c);
    goto tail;
ret0_b:
    return 0;

check_param0:
    if (mVariant != 0)
        goto ret0_c;
    {
        PathPtr path;
        path.FromID(mPathID);
        path.GetNode(*(Vector3 *)&mPosX, 0);
        path.GetNode(*(Vector3 *)&mPosX, 1);
    }
    mPrevAngleY = Vec3_HorzAngle((Vector3 *)&mPosX, &node);
    mAngleY = mPrevAngleY;
    func_ov016_02111bf0(this, (const State *)&data_ov016_02114dbc);
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
