//cpp
/* daMoray_c -- Jolly Roger Bay eel (MORAY).
 *
 * Five states installed by func_ov016_02111bf0 (retreat init is the
 * BookSwitch_Spawn name from the spawn-table pass). InitResources keeps the
 * variant picked by data_0209f220 and stars 1/2 of sublevel 8: variant 2
 * waits in the den, variant 1 carries a STAR (178), variant 0 swims the path.
 *
 * Text 0x021111a0..0x0211260c. Key function ~daMoray_c (out-of-line; D1/D0
 * at the start of ov016, D2 deadstripped). defer_codegen off keeps ROM order.
 * common.h stays first: the pose reads Matrix4x3 words m[9], m[10], m[11].
 *
 * leftovers:
 * - dExtBlendAnmModel_c::SetAnim, dCcAcPos_c::Init, ApproachLinear and
 *   dActor_c::SetRanges stay scalar Fix12 externs. The member forms
 *   grow the callers (02111718, InitResources, 02111758).
 * - currFrame >> 12 instead of the << 4 >> 16 shift shrinks 02111758
 *   and moves its pool off data_020a0e68.
 * - GetNode writes node 0 onto mPos, then node 1 over it. Filling a
 *   stack vector for the facing DIFFs InitResources by one word.
 * - Indexing mSegmentPos[i] shrinks 02111c40. The walk advances a
 *   daMoray_c* one Vector3 at a time, which is what the ROM does.
 *   The volatile stores into va, and the c1/c2 locals, stay.
 * - ModelComponents::UpdateBones grows Render. Render keeps
 *   func_020167a4, which is that wrapper.
 * - Sound::Play(3, 0xfa, pos) grows 02111860. func_02012694 is Play
 *   with bank 3.
 * - Deleting the dead mVariant == 0xff and mPathID < 0 tests, or the
 *   unused second PathPtr::FromID, shrinks InitResources. mwcc still
 *   emits those tests.
 * - The held actor's matrix is the word at dActor_c+0xc8, inside
 *   pad_0c5. unk_400 is zeroed by the two idle inits and read nowhere
 *   in this file. Sound 0xfa has no name here.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daMoray_c_AssetId {
    ASSET_MORAY_BMD = 0x03b5,
    ASSET_MORAY_SEARCH_BCA = 0x03b6,
    ASSET_MORAY_SWIM_BCA = 0x03b7,
    ASSET_MORAY_SWIM_START_BCA = 0x03b8
};

#include "common.h"
#include "daMoray_c.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "dExtFrameCtrl_c.h"
#include "dExtSimpleModel_c.h"
#include "Player.h"
#include "math/Approach.h"

#pragma defer_codegen off

enum {
    ACTOR_PLAYER = 191,
    ACTOR_STAR = 178,
};

/* The model handle's ctor/dtor are the cartridge's func_02017acc /
 * func_02017ab4 pair; the manifest aliases these. */
struct MorayModelFileHandle : SharedFilePtr {
    MorayModelFileHandle(u32 fileID);
    ~MorayModelFileHandle();
};

/* The three animation handles construct through SharedFilePtr::Construct
 * and register SharedFilePtr_Destruct_Anim; the manifest aliases these. */
struct MorayAnimResFileHandle : SharedFilePtr {
    MorayAnimResFileHandle(u32 fileID);
    ~MorayAnimResFileHandle();
};

extern MorayModelFileHandle data_ov016_02114d38;   /* model, file 0x3b5 */
extern MorayAnimResFileHandle data_ov016_02114d20; /* lunge anim, file 0x3b6 */
extern MorayAnimResFileHandle data_ov016_02114d30; /* path-swim anim, file 0x3b7 */
extern MorayAnimResFileHandle data_ov016_02114d28; /* den / swim-out anim, file 0x3b8 */

/* State tables filled by __sinit_d_a_moray.cpp from the PMFs at 0x02114878.
 * 02114d7c retreat (daMoray_c::BookSwitch_Spawn, then daMoray_c::func_ov016_02111534),
 * 02114d8c den wait, 02114d9c lunge, 02114dac swim out, 02114dbc path swim. */
extern daMoray_c::State data_ov016_02114d7c;
extern daMoray_c::State data_ov016_02114d8c;
extern daMoray_c::State data_ov016_02114d9c;
extern daMoray_c::State data_ov016_02114dac;
extern daMoray_c::State data_ov016_02114dbc;

/* The zero cylinder offset has three signed components and no registered
 * lifetime work in the retail initializer. Keep its storage plain; copy the
 * components into the collision API's Vector3 locals below. */
struct MorayCylinderOffset { s32 x, y, z; };

extern "C" {
extern MorayCylinderOffset data_ov016_02114d4c;         /* cylinder offset, bss */

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
void func_020167a4(dExtBlendAnmModel_c *model);
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

namespace cstd {
int fdiv(int a, int b);
}

/* Out of line, and FIRST in this ROM-ascending file: that puts D1 and D0 at
 * 0x021111a0 / 0x02111208, the two lowest addresses in the unit.
 *
 * One array cleanup, four member destructors and the chain into
 * dEnemyBase_c, all of it in reverse declaration order out of daMoray_c.h.
 * The array at 0x448 is Vector3[7]: the ROM destroys it with
 * __cxa_vec_cleanup(ptr, 7, 0xc, _ZN7Vector3D1Ev), and 0xc is
 * sizeof(Vector3). D0 then hands the object back through dActor_c's inline
 * operator delete. mwcc also emits D2, which the ROM does not have; the
 * manifest licenses it as deadstrip. */
// @symbol _ZN9daMoray_cD1Ev
// @symbol _ZN9daMoray_cD0Ev
daMoray_c::~daMoray_c()
{
}

/* Places both hit cylinders. On entrance 1, outside path swim, the second
   cylinder (mdCcAcPos_c2) goes to a fixed point moved along the facing (a
   (0, -0x128000, 0x7c000) offset turned by mAngleY), with radius 0xc8000
   (200 units) and height 0xf0000 (240 units). The first cylinder
   (mdCcAcPos_c1) steps through mSegmentPos: the segment index advances by one
   per call and wraps after 6; that segment's position gets an offset added
   (0x48000 = 72 units down, plus 0x7c000 = 124 units forward on segment 5,
   turned by mAngleY) and is copied to the cylinder. Its radius is 0x52000
   (82 units), or 0xb8000 (184 units) on segment 5, and its height 0x70000
   (112 units). The segment positions are recomputed on the next Behavior.
   Hurts the actor recorded in the first cylinder's otherOwner (set by the
   hit test since the previous Clear) when that actor is the PLAYER (actor
   191). Called from Behavior. */
// @symbol _ZN9daMoray_c19func_ov016_02111284Ev
void daMoray_c::func_ov016_02111284()
{
    Vector3 va;
    Vector3 vb;
    Vector3 out;
    Vector3 hv;
    int r4;
    int r6;
    dActor_c *p;
    int t;
    u32 id;
    int *ctr;
    int idx;
    int *px;

    *(volatile int *)&va.x = this->mPosX;
    *(volatile int *)&va.y = this->mPosY;
    *(volatile int *)&va.z = this->mPosZ;

    vb.x = 0;
    vb.y = 0;
    vb.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;

    r4 = 0x52000;

    if (data_0209f220 == 1 && this->mState != &data_ov016_02114dbc) {
        r6 = 0x128000;
        r6 = -r6;
        vb.y = r6;
        vb.z = 0x7c000;
        va.x = 0x15e0000;
        va.y = 0xfee90000;
        va.z = 0x65e000;
        Matrix4x3_FromRotationY(&data_020a0e68, this->mAngleY);
        MulVec3Mat4x3(&vb, &data_020a0e68, &out);
        va.x = va.x + out.x;
        va.y = va.y + out.y;
        va.z = va.z + out.z;
        this->mdCcAcPos_c2.pos.x = va.x;
        {
            int c1 = 0xc8000;
            this->mdCcAcPos_c2.pos.y = va.y;
            int c2 = 0xf0000;
            this->mdCcAcPos_c2.pos.z = va.z;
            this->mdCcAcPos_c2.radius = c1;
            this->mdCcAcPos_c2.height = c2;
        }
    }

    ctr = &this->mCylSegment;
    *ctr = *ctr + 1;
    if (this->mCylSegment > 6)
        this->mCylSegment = 0;

    idx = this->mCylSegment;
    if (idx == 5)
        r4 = 0xb8000;
    vb.y = 0;
    vb.x = 0;
    vb.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    vb.y = -0x48000;
    if (this->mCylSegment == 5)
        vb.z = 0x7c000;

    Matrix4x3_FromRotationY(&data_020a0e68, this->mAngleY);
    MulVec3Mat4x3(&vb, &data_020a0e68, &out);

    {
        /* Three int bases stepped by a byte index keep the * 0xc per axis. */
        int *p448 = &this->mSegmentPos[0].x;
        int *p44c = &this->mSegmentPos[0].y;
        int *p450 = &this->mSegmentPos[0].z;
        int idx2 = this->mCylSegment;
        ((int *)((char *)p448 + idx2 * 0xc))[0] = ((int *)((char *)p448 + idx2 * 0xc))[0] + out.x;
        idx2 = this->mCylSegment;
        ((int *)((char *)p44c + idx2 * 0xc))[0] = ((int *)((char *)p44c + idx2 * 0xc))[0] + out.y;
        idx2 = this->mCylSegment;
        ((int *)((char *)p450 + idx2 * 0xc))[0] = ((int *)((char *)p450 + idx2 * 0xc))[0] + out.z;
        idx2 = this->mCylSegment;
        px = (int *)((char *)p448 + idx2 * 0xc);
        this->mdCcAcPos_c1.pos.x = px[0];
        this->mdCcAcPos_c1.pos.y = px[1];
        this->mdCcAcPos_c1.pos.z = px[2];
    }

    this->mdCcAcPos_c1.radius = r4;
    this->mdCcAcPos_c1.height = 0x70000;

    id = this->mdCcAcPos_c1.otherOwner;
    if (id == 0)
        return;

    p = dActor_c::FindWithID(id);
    t = (int)(p->actorID == ACTOR_PLAYER);
    if (t == 0)
        return;

    hv.x = this->mPosX;
    hv.y = this->mPosY;
    hv.z = this->mPosZ;
    ((Player *)p)->Hurt(hv, 3, 0xc000, 1, 0, 1);
}

/* Retreat execute. When the timer set by the retreat init runs out, stops,
   turns back to the spawn angles and returns to den wait. */
// @symbol _ZN9daMoray_c19func_ov016_02111534Ev
int daMoray_c::func_ov016_02111534()
{
    if (*(u16 *)&this->mStateTimer == 0) {
        this->mHorzSpeed = 0;
        this->mPrevAngleX = this->mInitAngleX;
        this->mPrevAngleY = this->mInitAngleY;
        this->mPrevAngleZ = this->mInitAngleZ;
        this->mAngleX = this->mPrevAngleX;
        this->mAngleY = this->mPrevAngleY;
        this->mAngleZ = this->mPrevAngleZ;
        func_ov016_02111bf0(&data_ov016_02114d8c);
    }
    return 1;
}

/* Retreat init: mStateTimer = 100 frames, mHorzSpeed = 0x14000 (20 units).
   Behavior counts the timer down each frame; daMoray_c::func_ov016_02111534 is the only
   state in this file that tests it for 0. */
// @symbol _ZN9daMoray_c16BookSwitch_SpawnEv
int daMoray_c::BookSwitch_Spawn()
{
    this->mStateTimer = 100;
    this->mHorzSpeed = 0x14000;
    return 1;
}

/* Path-swim execute. Turns toward the current node (heading and pitch each
   move at most 0x80 = 128/65536 of a turn per frame; mSegmentAngle[7], the
   body-bend target, is set to half the absolute heading difference, taken
   before this frame's step) and
   moves 0xa000 (10 units) per frame straight at it. Within 10 units of the
   node it plays sound 0xfa and steps to the next node, wrapping to 0 past
   mPathNodeCount. Variant 1 arriving at node 0 instead steps to node 1 and
   enters the retreat state, without the sound. */
// @symbol _ZN9daMoray_c19func_ov016_021115c0Ev
int daMoray_c::func_ov016_021115c0()
{
    PathPtr path;
    Vector3 node;
    Vector3 diff;
    Vector3 scaled;
    int len;
    short headingToNode;

    path.FromID(this->mPathID);
    path.GetNode(node, this->mPathNodeIndex);

    headingToNode = Vec3_HorzAngle(&this->mPos, &node);
    {
        int turn = AngleDiff(headingToNode, this->mPrevAngleY);
        this->mSegmentAngle[7] = turn / 2;
    }
    ApproachLinear(this->mPrevAngleY, headingToNode, 0x80);

    ApproachLinear(this->mPrevAngleX, Vec3_VertAngle(&this->mPos, &node), 0x80);

    Vec3_Sub(&diff, &this->mPos, &node);
    len = LenVec3(&diff);
    if (len == 0 || len <= 0xa000) {
        if (this->mVariant == 1 && this->mPathNodeIndex == 0) {
            this->mPathNodeIndex = 1;
            func_ov016_02111bf0(&data_ov016_02114d7c);
            return 1;
        }
        this->mPathNodeIndex++;
        if (this->mPathNodeIndex >= this->mPathNodeCount)
            this->mPathNodeIndex = 0;
        func_02012694(0xfa, &this->mCamSpacePos);
    } else {
        Vec3_MulScalar(&scaled, &diff, cstd::fdiv(0xa000, len));
        SubVec3(&this->mPos, &scaled, &this->mPos);
    }
    return 1;
}

/* Path-swim init: the looping swim animation. */
// @symbol _ZN9daMoray_c19func_ov016_02111718Ev
int daMoray_c::func_ov016_02111718()
{
    this->mBlendAnmModel.SetAnim(*(BCA_File *)data_ov016_02114d30.filePtr, 8, 0, 0x1000, 0);
    return 1;
}

/* Lunge execute. On animation frames 0x15..0x3c (inclusive), moves its
   position toward a point on its facing from the home point, 0x1f4000 (500
   units) out, or 0x2bc000 (700 units) when the entrance filter is 1,
   at up to 0x14000 (20 units) per frame. When the animation finishes it
   stops and enters swim-out. */
// @symbol _ZN9daMoray_c19func_ov016_02111758Ev
int daMoray_c::func_ov016_02111758()
{
    Vector3 localOffset;
    Vector3 worldTarget;
    localOffset.x = 0; localOffset.y = 0; localOffset.z = 0;
    worldTarget.x = 0; worldTarget.y = 0; worldTarget.z = 0;
    unsigned int frame = (unsigned int)this->mBlendAnmModel.currFrame << 4 >> 0x10;
    if (frame >= 0x15 && frame <= 0x3c) {
        if (data_0209f220 == 1) {
            localOffset.z = 0x2bc000;
        } else {
            localOffset.z = 0x1f4000;
        }
        Matrix4x3_FromRotationY(&data_020a0e68, this->mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, this->mAngleX);
        MulVec3Mat4x3(&localOffset, &data_020a0e68, &worldTarget);
        worldTarget.x += this->mHomePosX;
        worldTarget.y += this->mHomePosY;
        worldTarget.z += this->mHomePosZ;
        ApproachLinear(this->mPos, worldTarget, 0x14000);
    }
    if (this->mBlendAnmModel.Finished()) {
        this->mHorzSpeed = 0;
        func_ov016_02111bf0(&data_ov016_02114dac);
    }
    return 1;
}

/* Lunge init: bite animation with flag bit 30, and the lunge sound. */
// @symbol _ZN9daMoray_c19func_ov016_02111860Ev
int daMoray_c::func_ov016_02111860()
{
    this->mBlendAnmModel.SetAnim(*(BCA_File *)data_ov016_02114d20.filePtr, 8, 0x40000000, 0x1000,
                                 0);
    func_02012694(0xfa, &this->mCamSpacePos);
    return 1;
}

/* Swim-out execute. Moves toward a point 0x76c000 (1900 units) from the home
   point along its facing at up to 0x14000 (20 units) per frame, and enters
   path-swim, with the sound, once within 20 units of it. */
// @symbol _ZN9daMoray_c19func_ov016_021118b4Ev
int daMoray_c::func_ov016_021118b4()
{
    Vector3 localOffset;
    Vector3 worldTarget;
    localOffset.x = 0; localOffset.y = 0; localOffset.z = 0;
    worldTarget.x = 0; worldTarget.y = 0; worldTarget.z = 0;
    localOffset.z = 0x76c000;
    Matrix4x3_FromRotationY(&data_020a0e68, this->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, this->mAngleX);
    MulVec3Mat4x3(&localOffset, &data_020a0e68, &worldTarget);
    worldTarget.x += this->mHomePosX;
    worldTarget.y += this->mHomePosY;
    worldTarget.z += this->mHomePosZ;
    ApproachLinear(this->mPos, worldTarget, 0x14000);
    if (Vec3_Dist(&this->mPos, &worldTarget) < 0x14000) {
        func_02012694(0xfa, &this->mCamSpacePos);
        func_ov016_02111bf0(&data_ov016_02114dbc);
    }
    return 1;
}

/* Swim-out init: idle animation and the same sound. */
// @symbol _ZN9daMoray_c19func_ov016_02111994Ev
int daMoray_c::func_ov016_02111994()
{
    this->unk_400 = 0;
    func_02012694(0xfa, &this->mCamSpacePos);
    this->mBlendAnmModel.SetAnim(*(BCA_File *)data_ov016_02114d28.filePtr, 8, 0, 0x1000, 0);
    return 1;
}

/* Den-wait execute. While farther than 0xa000 (10 units) from the home
   point it moves back at up to 0x5000 (5 units) per frame and does nothing
   else. At home it snaps onto it and, with a player present, lunges when
   AngleDiff(angle from the eel to that player, mAngleY) is below 0x3300
   (about 72 degrees either side of mAngleY, AngleDiff being an absolute
   value), the horizontal distance is below 0x3e8000 (1000 units) and the
   height difference is below 0x800000 (2048 units). Entrance 1 widens the
   horizontal limit to 0x495000 (1173 units), tightens the height limit to
   0x6ee000 (1774 units), and also lunges when the player is within
   0x224000 (548 units) of a point 0x64000 (100 units) ahead of home. */
#pragma push
#pragma opt_propagation off
// @symbol _ZN9daMoray_c19func_ov016_021119ecEv
int daMoray_c::func_ov016_021119ec()
{
    Player *player;
    Vector3 playerPos;
    Vector3 local;
    Vector3 world;
    int thrHorz;
    int thrVert;
    int thrAng;
    unsigned char entrance;
    int zero;
    int dy;

    if (Vec3_Dist(&this->mPos, (const Vector3 *)&this->mHomePosX) > 0xa000) {
        ApproachLinear(this->mPos, *(const Vector3 *)&this->mHomePosX, 0x5000);
        return 1;
    }

    this->mPosX = this->mHomePosX;
    this->mPosY = this->mHomePosY;
    this->mPosZ = this->mHomePosZ;

    player = this->ClosestPlayer();
    if (player != 0) {
        /* ACDB order: x,y,z then entrance -- colors y/z as r2 and entrance as r1 */
        const Vector3 *pos = &player->mPos;
        playerPos.x = pos->x;
        playerPos.y = pos->y;
        playerPos.z = pos->z;
        entrance = data_0209f220;

        thrHorz = 0x3e8000;
        thrAng = 0x3300;
        thrVert = 0x418000;
        thrVert += 0x3e8000;

        if (entrance == 1) {
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
            Matrix4x3_FromRotationY(&data_020a0e68, this->mAngleY);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, this->mAngleX);
            MulVec3Mat4x3(&local, &data_020a0e68, &world);
            world.x += this->mHomePosX;
            world.y += this->mHomePosY;
            world.z += this->mHomePosZ;
            if (Vec3_Dist(&playerPos, &world) < 0x224000)
                func_ov016_02111bf0(&data_ov016_02114d9c);
        }

        if (AngleDiff(this->HorzAngleToCPlayer(), this->mAngleY) < thrAng) {
            if (Vec3_HorzDist(&this->mPos, &playerPos) < thrHorz) {
                dy = this->mPosY - playerPos.y;
                if (dy < 0)
                    dy = -dy;
                if (dy < thrVert)
                    func_ov016_02111bf0(&data_ov016_02114d9c);
            }
        }
    }
    return 1;
}
#pragma pop

/* Den-wait init: the idle animation. */
// @symbol _ZN9daMoray_c19func_ov016_02111bacEv
int daMoray_c::func_ov016_02111bac()
{
    this->unk_400 = 0;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)data_ov016_02114d28.filePtr, 8, 0, 0x1000, 0);
    return 1;
}

/* Installs a state and runs its init. */
// @symbol _ZN9daMoray_c19func_ov016_02111bf0EPNS_5StateE
int daMoray_c::func_ov016_02111bf0(State *state)
{
    this->mState = state;
    const daMoray_c::State *installed = this->mState;
    if (installed->init == 0)
        return 1;
    return (this->*installed->init)();
}

/* Poses the body: the model matrix from position and angles, then each
   segment's world position out of its bone transform, and the carried
   star's matrix off the jaw bone. */
// @symbol _ZN9daMoray_c19func_ov016_02111c40Ev
void daMoray_c::func_ov016_02111c40()
{
    Vector3 localOffset;
    Vector3 worldTarget;
    Vector3 asr;
    int zero[1];
    int i;
    daMoray_c *row;
    int transformOffset;
    Vector3 *pos;
    Matrix4x3 *modelMtx;
    dActor_c *star;
    u32 starUniqueID;

    Vec3_Asr(&asr, &this->mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    localOffset.z = 0;
    localOffset.z = -0x190000;
    localOffset.x = 0;
    localOffset.y = 0;
    worldTarget.x = 0;
    worldTarget.y = 0;
    worldTarget.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, this->mAngleY);
    MulVec3Mat4x3(&localOffset, &data_020a0e68, &worldTarget);
    Matrix4x3_FromTranslation(&data_020a0e68, (this->mPosX + worldTarget.x) >> 3, this->mPosY >> 3,
                              (this->mPosZ + worldTarget.z) >> 3);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, this->mAngleX, this->mAngleY, this->mAngleZ);
    this->mBlendAnmModel.mat4x3 = data_020a0e68;
    ApproachLinear(this->mSegmentAngle[6], this->mSegmentAngle[7], 0x40);
    i = 0;
    modelMtx = &this->mBlendAnmModel.mat4x3;
    this->mSegmentAngle[5] = this->mSegmentAngle[6];
    /* mSegmentPos[i] / transforms[i] is 0x10 smaller than this walk
       (func_ov016_02111c40 0x2f8 -> 0x2e8). row steps one Vector3 at a time
       so the clear and the << 3 hit mSegmentPos[i] through the actor base;
       pos receives the bone translation before that shift. */
    row = this;
    transformOffset = i;
    pos = this->mSegmentPos;
    zero[0] = i;
    do {
        Matrix4x3 *dst;
        row->mSegmentPos[0].x = zero[0];
        row->mSegmentPos[0].y = zero[0];
        row->mSegmentPos[0].z = zero[0];
        dst = &data_020a0e68;
        *dst = *modelMtx;
        MulMat4x3Mat4x3((char *)this->mBlendAnmModel.data.transforms + transformOffset, dst, dst);
        pos->x = dst->m[9];
        pos->y = dst->m[10];
        pos->z = dst->m[11];
        row->mSegmentPos[0].x = row->mSegmentPos[0].x << 3;
        row->mSegmentPos[0].y = row->mSegmentPos[0].y << 3;
        row->mSegmentPos[0].z = row->mSegmentPos[0].z << 3;
        row = (daMoray_c *)((char *)row + sizeof(Vector3));
        transformOffset += 0x30;
        pos++;
        i++;
    } while (i < 7);
    starUniqueID = this->mStarUniqueID;
    if (starUniqueID == 0) {
        return;
    }
    star = dActor_c::FindWithID(starUniqueID);
    if (star == 0) {
        return;
    }
    this->mStarPos.x = 0;
    this->mStarPos.y = 0;
    this->mStarPos.z = 0;
    MulMat4x3Mat4x3(&this->mBlendAnmModel.data.transforms[4], &this->mBlendAnmModel.mat4x3,
                    &this->mStarMtx);
    Matrix4x3_FromTranslation(&data_020a0e68, 0x28000, 0, 0);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x4000, -0x8000, this->mStarSpinAngle);
    MulMat4x3Mat4x3(&data_020a0e68, &this->mStarMtx, &this->mStarMtx);
    data_020a0e68 = this->mStarMtx;
    this->mStarPos.x = data_020a0e68.m[9];
    this->mStarPos.y = data_020a0e68.m[10];
    this->mStarPos.z = data_020a0e68.m[11];
    this->mStarPos.x <<= 3;
    this->mStarPos.y <<= 3;
    this->mStarPos.z <<= 3;
    ((MorayStarCarrier *)star)->mtx = &this->mStarMtx;
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

    func_020167a4(&mBlendAnmModel);
    bone = (MorayBone *)mBlendAnmModel.data.bones + 1;
    step = data_ov016_02114908;
    for (i = 1; i < 7; i++) {
        s16 movement = mSegmentAngle[i];
        u16 *angle = (u16 *)&bone->rotZ;
        *angle = *angle + (u16)(s16)(movement * step->angleScale);
        step++;
        bone++;
    }
    func_0204531c(&mBlendAnmModel.data, mBlendAnmModel.blendWeight);
    mBlendAnmModel.dExtSimpleModel_c::Render(0);
    return 1;
}
#pragma pop

// @symbol _ZN9daMoray_c8BehaviorEv
s32 daMoray_c::Behavior()
{
    dActor_c *star;
    unsigned starUniqueID;

    DecIfAbove0_Short((u16 *)&mStateTimer);
    if (mState->execute)
        (this->*mState->execute)();
    UpdatePos(&mdCcAcPos_c1);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov016_02111c40();

    starUniqueID = mStarUniqueID;
    if (starUniqueID != 0) {
        star = FindWithID(starUniqueID);
        if (star != 0) {
            if (mVariant == 1) {
                Player *player = ClosestPlayer();
                if (player != 0 && Vec3_Dist(&mStarPos, &player->mPos) < 0xfa000) {
                    star->MarkForDestruction();
                    /* 0x40 releases the STAR; InitResources spawned it held (| 0x50). */
                    Spawn(ACTOR_STAR, mStarParam | 0x40, mStarPos, 0, mAreaId, -1);
                    mStarUniqueID = 0;
                }
            }
        } else {
            mStarUniqueID = 0;
        }
        if (mStarUniqueID != 0) {
            star->mPosX = mStarPos.x;
            star->mPosY = mStarPos.y;
            star->mPosZ = mStarPos.z;
            mStarSpinAngle += 0x1000;
        }
    }

    func_ov016_02111284();
    mdCcAcPos_c1.Clear();
    mdCcAcPos_c1.Update();
    if (data_0209f220 == 1 && mState != &data_ov016_02114dbc) {
        mdCcAcPos_c2.Clear();
        mdCcAcPos_c2.Update();
    }
    mBlendAnmModel.Advance();
    return 1;
}

// @symbol _ZN9daMoray_c13InitResourcesEv
s32 daMoray_c::InitResources()
{
    Vector3 node;
    Vector3 cylOffset1;
    Vector3 cylOffset2;
    int i;
    void *file;
    dActor_c *spawned;

    file = dExtSimpleModel_c::LoadFile(data_ov016_02114d38);
    mBlendAnmModel.SetFile((BMD_File *)file, 1, -1);
    dExtFrameCtrl_c::LoadFile(data_ov016_02114d20);
    dExtFrameCtrl_c::LoadFile(data_ov016_02114d30);
    dExtFrameCtrl_c::LoadFile(data_ov016_02114d28);

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
    cylOffset1.x = data_ov016_02114d4c.x;
    cylOffset1.y = data_ov016_02114d4c.y;
    cylOffset1.z = data_ov016_02114d4c.z;
    mdCcAcPos_c1.Init(this, cylOffset1, 0x32000, 0x50000, 0x200004, 0);
    cylOffset2.x = data_ov016_02114d4c.x;
    cylOffset2.y = data_ov016_02114d4c.y;
    cylOffset2.z = data_ov016_02114d4c.z;
    mdCcAcPos_c2.Init(this, cylOffset2, 0x32000, 0x50000, 0x200000, 0);

    /* Second bind is unused. Dropping the constructor and FromID shrinks
       InitResources by 0x14, so the ROM really does call them. */
    {
        PathPtr path;
        path.FromID(mPathID);
    }
    mPathNodeIndex = 1;
    mStarUniqueID = 0;
    mBlendAnmModel.speed = 0x1000;

    if (data_0209f220 == 1 || IsStarCollected(SublevelToLevel(8), 1) == 0) {
        /* Entrance 1, or star 1 still uncollected: only variant 2. */
        if (mVariant == 2) {
            mHomePosY -= 0x80000;
            mPathNodeIndex = 8;
            if (mPathNodeIndex >= mPathNodeCount)
                mPathNodeIndex = 4;
            mPosY = mHomePosY;
            func_ov016_02111bf0(&data_ov016_02114d8c);
        } else {
            return 0;
        }
    } else if (data_0209f220 == 2 || IsStarCollected(SublevelToLevel(8), 2) == 0) {
        /* Entrance 2, or star 2 still uncollected: only variant 1. */
        if (mVariant == 1) {
            /* Held (| 0x50). Behavior releases it with | 0x40. */
            spawned = Spawn(ACTOR_STAR, mStarParam | 0x50, mPos, 0, mAreaId, -1);
            if (spawned != 0) {
                mStarUniqueID = spawned->uniqueID;
                spawned->mFlags = 0;
                spawned->SetRanges(0, 0x3e8000, 0x1f40000, 0x1f40000);
            }
            func_ov016_02111bf0(&data_ov016_02114d8c);
        } else {
            return 0;
        }
    } else {
        /* Otherwise both stars are collected: only variant 0. */
        if (mVariant == 0) {
            PathPtr path;
            path.FromID(mPathID);
            /* Both nodes are written to mPos; the second sticks. The facing
               below is against `node`, which this block does not fill. Sending
               node 0 there is a one-word DIFF. */
            path.GetNode(mPos, 0);
            path.GetNode(mPos, 1);
            mPrevAngleY = Vec3_HorzAngle(&mPos, &node);
            mAngleY = mPrevAngleY;
            func_ov016_02111bf0(&data_ov016_02114dbc);
        } else {
            return 0;
        }
    }

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

/* Reconstructed source-style name: SM64DS proves daMoray_c through RTTI,
 * allocation size, vtable identity, and the MORAY registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: Unagi_Spawn.
 *
 * The implicit constructor builds the base, collision cylinders, background
 * checker, model animation, and seven segment positions in declaration order.
 * Vector3's empty default constructor is the array callback at 0x0203d384. */
// @symbol daMoray_c_classInit
extern "C" daMoray_c *daMoray_c_classInit()
{
    return new daMoray_c;
}

/* Construction order is source order: the four file handles first, then the
 * five State rows in the order the initializer copies them. The registration
 * nodes and the PMF literals are compiler temporaries. */
MorayModelFileHandle data_ov016_02114d38(ASSET_MORAY_BMD);
MorayAnimResFileHandle data_ov016_02114d20(ASSET_MORAY_SEARCH_BCA);
MorayAnimResFileHandle data_ov016_02114d30(ASSET_MORAY_SWIM_BCA);
MorayAnimResFileHandle data_ov016_02114d28(ASSET_MORAY_SWIM_START_BCA);

daMoray_c::State data_ov016_02114d8c = { &daMoray_c::func_ov016_02111bac,
                                       &daMoray_c::func_ov016_021119ec };
daMoray_c::State data_ov016_02114d9c = { &daMoray_c::func_ov016_02111860,
                                       &daMoray_c::func_ov016_02111758 };
daMoray_c::State data_ov016_02114dac = { &daMoray_c::func_ov016_02111994,
                                       &daMoray_c::func_ov016_021118b4 };
daMoray_c::State data_ov016_02114dbc = { &daMoray_c::func_ov016_02111718,
                                       &daMoray_c::func_ov016_021115c0 };
daMoray_c::State data_ov016_02114d7c = { &daMoray_c::BookSwitch_Spawn,
                                       &daMoray_c::func_ov016_02111534 };

MorayCylinderOffset data_ov016_02114d4c;
