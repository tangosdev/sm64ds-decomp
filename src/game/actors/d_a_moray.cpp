//cpp
/* daMoray_c -- Jolly Roger Bay eel (MORAY).
 *
 * Five states installed by SetState. The member names are coined from the
 * bodies (StateXxxInit / StateXxxMain as in the other state-table actors);
 * the N64 unagi.inc.c has only numbered actions. InitResources keeps the
 * variant picked by gCurrentStarId and stars 1/2 of sublevel 8: variant 2
 * waits in the den, variant 1 carries a STAR (178), variant 0 swims the path.
 *
 * Text 0x021111a0..0x0211260c. Key function ~daMoray_c (out-of-line; D1/D0
 * at the start of ov016, D2 deadstripped). defer_codegen off keeps ROM order.
 *
 * leftovers:
 * - currFrame >> 12 instead of the << 4 >> 16 shift shrinks 02111758
 *   and moves its pool off gScratchMtx.
 * - GetNode writes node 0 onto mPos, then node 1 over it. Filling a
 *   stack vector for the facing DIFFs InitResources by one word.
 * - UpdatePose walks mSegmentPos with a daMoray_c* advanced one Vector3 at
 *   a time beside the pos walker; see the note there.
 * - ModelComponents::UpdateBones grows Render. Render keeps
 *   func_020167a4, which is that wrapper.
 * - Sound::Play(3, 0xfa, pos) grows 02111860. func_02012694 is Play
 *   with bank 3.
 * - Deleting the dead mVariant == 0xff and mPathID < 0 tests, or the
 *   unused second PathPtr::FromID, shrinks InitResources. mwcc still
 *   emits those tests.
 * - The held actor borrows mStarMtx through dActor_c::mCarryMatrix.
 *   unk_400 is zeroed by the two idle inits and read nowhere in this
 *   file. Sound 0xfa has no name here.
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
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "dExtFrameCtrl_c.h"
#include "dExtSimpleModel_c.h"
#include "daPly_c.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"

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
 * 02114d7c retreat (daMoray_c::StateRetreatInit, then daMoray_c::StateRetreatMain),
 * 02114d8c den wait, 02114d9c lunge, 02114dac swim out, 02114dbc path swim. */
extern daMoray_c::State data_ov016_02114d7c;
extern daMoray_c::State data_ov016_02114d8c;
extern daMoray_c::State data_ov016_02114d9c;
extern daMoray_c::State data_ov016_02114dac;
extern daMoray_c::State data_ov016_02114dbc;

/* The zero cylinder offset has three signed components and no registered
 * lifetime work in the retail initializer: a VecFx32, not a Vector3. Its
 * components are copied into the collision API's Vector3 locals below. */
extern "C" {
extern VecFx32 data_ov016_02114d4c; /* cylinder offset, bss */

extern unsigned char gCurrentStarId; /* the selected star */

int AngleDiff(int a, int b);
int SublevelToLevel(int sub);
/* Sound::Play(3, id, pos). Calling Play directly is one word larger
   (StateLungeInit 0x54 -> 0x58). */
/* UpdateBones(model.data, model.file, frame). Inlining that grows Render
   0x8c -> 0x9c. */
/* Blend the posed bones by blendWeight. No named method. */
void func_0204531c(ModelComponents *data, s32 weight);
}
int IsStarCollected(int level, int star);
void func_02012694(unsigned int id, const Vector3 *pos);
void func_020167a4(dExtAnmModel_c *model);

/* Render's per-segment bend table, one 0xc record per body bone.
 * The six ROM records are {0, 0, scale} with scales 1,1,1,1,-1,0.
 * This loop reads only angleScale. */
struct MorayRenderStep {
    s32 pad_00;
    s32 pad_04;
    s32 angleScale;
};

MorayRenderStep data_ov016_02114908[6] = {
    {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, -1}, {0, 0, 0},
};

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
daMoray_c::~daMoray_c()
{
}

/* Places both hit cylinders. On entrance 1, outside path swim, the second
   cylinder (mdCcAcPos_c2) goes to a fixed point moved along the facing (a
   (0, -0x128000, 0x7c000) offset turned by mAngle.y), with radius 0xc8000
   (200 units) and height 0xf0000 (240 units). The first cylinder
   (mdCcAcPos_c1) steps through mSegmentPos: the segment index advances by one
   per call and wraps after 6; that segment's position gets an offset added
   (0x48000 = 72 units down, plus 0x7c000 = 124 units forward on segment 5,
   turned by mAngle.y) and is copied to the cylinder. Its radius is 0x52000
   (82 units), or 0xb8000 (184 units) on segment 5, and its height 0x70000
   (112 units). The segment positions are recomputed on the next Behavior.
   Hurts the actor recorded in the first cylinder's otherOwner (set by the
   hit test since the previous Clear) when that actor is the PLAYER (actor
   191). Called from Behavior. */
void daMoray_c::UpdateHitCylinders() {
    Vector3 va;
    Vector3 vb;
    Vector3 out;
    Vector3 hv;
    int r4;
    dActor_c *p;
    int t;
    u32 id;
    int idx;
    int *px;

    va.x = this->mPos.x;
    va.y = this->mPos.y;
    va.z = this->mPos.z;

    vb.x = 0;
    vb.y = 0;
    vb.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;

    r4 = 0x52000;

    if (gCurrentStarId == 1 && this->mState != &data_ov016_02114dbc) {
        vb.y = -0x128000;
        vb.z = 0x7c000;
        va.x = 0x15e0000;
        va.y = 0xfee90000;
        va.z = 0x65e000;
        Matrix4x3_FromRotationY(&gScratchMtx, this->mAngle.y);
        MTX_MultVec43(&vb, &gScratchMtx, &out);
        va.x = va.x + out.x;
        va.y = va.y + out.y;
        va.z = va.z + out.z;
        this->mdCcAcPos_c2.pos.x = va.x;
        this->mdCcAcPos_c2.pos.y = va.y;
        this->mdCcAcPos_c2.pos.z = va.z;
        this->mdCcAcPos_c2.radius = 0xc8000;
        this->mdCcAcPos_c2.height = 0xf0000;
    }

    this->mCylSegment++;
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

    Matrix4x3_FromRotationY(&gScratchMtx, this->mAngle.y);
    MTX_MultVec43(&vb, &gScratchMtx, &out);

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
    t = (p->actorID == ACTOR_PLAYER);
    if (t == 0)
        return;

    hv.x = this->mPos.x;
    hv.y = this->mPos.y;
    hv.z = this->mPos.z;
    ((daPly_c *)p)->Hurt(hv, 3, 0xc000, 1, 0, 1);
}

/* Retreat execute. When the timer set by the retreat init runs out, stops,
   turns back to the spawn angles and returns to den wait. */
int daMoray_c::StateRetreatMain() {
    if (this->mStateTimer == 0) {
        this->mHorzSpeed = 0;
        this->mPrevAngle.x = this->mInitAngleX;
        this->mPrevAngle.y = this->mInitAngleY;
        this->mPrevAngle.z = this->mInitAngleZ;
        this->mAngle.x = this->mPrevAngle.x;
        this->mAngle.y = this->mPrevAngle.y;
        this->mAngle.z = this->mPrevAngle.z;
        SetState(&data_ov016_02114d8c);
    }
    return 1;
}

/* Retreat init: mStateTimer = 100 frames, mHorzSpeed = 0x14000 (20 units).
   Behavior counts the timer down each frame; daMoray_c::StateRetreatMain is the only
   state in this file that tests it for 0. */
int daMoray_c::StateRetreatInit() {
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
int daMoray_c::StatePathSwimMain() {
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
        int turn = AngleDiff(headingToNode, this->mPrevAngle.y);
        this->mSegmentAngle[7] = turn / 2;
    }
    ApproachLinear(this->mPrevAngle.y, headingToNode, 0x80);

    ApproachLinear(this->mPrevAngle.x, Vec3_VertAngle(&this->mPos, &node), 0x80);

    Vec3_Sub(&diff, &this->mPos, &node);
    len = VEC_Mag(&diff);
    if (len == 0 || len <= 0xa000) {
        if (this->mVariant == 1 && this->mPathNodeIndex == 0) {
            this->mPathNodeIndex = 1;
            SetState(&data_ov016_02114d7c);
            return 1;
        }
        this->mPathNodeIndex++;
        if (this->mPathNodeIndex >= this->mPathNodeCount)
            this->mPathNodeIndex = 0;
        func_02012694(0xfa, &this->mCamSpacePos);
    } else {
        Vec3_MulScalar(&scaled, &diff, FX_Div(0xa000, len));
        VEC_Subtract(&this->mPos, &scaled, &this->mPos);
    }
    return 1;
}

/* Path-swim init: the looping swim animation. */
int daMoray_c::StatePathSwimInit() {
    this->mBlendAnmModel.SetAnim(*(BCA_File *)data_ov016_02114d30.filePtr, 8, 0, 0x1000, 0);
    return 1;
}

/* Lunge execute. On animation frames 0x15..0x3c (inclusive), moves its
   position toward a point on its facing from the home point, 0x1f4000 (500
   units) out, or 0x2bc000 (700 units) when the entrance filter is 1,
   at up to 0x14000 (20 units) per frame. When the animation finishes it
   stops and enters swim-out. */
int daMoray_c::StateLungeMain() {
    Vector3 localOffset;
    Vector3 worldTarget;
    localOffset.x = 0; localOffset.y = 0; localOffset.z = 0;
    worldTarget.x = 0; worldTarget.y = 0; worldTarget.z = 0;
    unsigned int frame = (unsigned int)this->mBlendAnmModel.currFrame << 4 >> 16;
    if (frame >= 21 && frame <= 60) {
        if (gCurrentStarId == 1) {
            localOffset.z = 0x2bc000;
        } else {
            localOffset.z = 0x1f4000;
        }
        Matrix4x3_FromRotationY(&gScratchMtx, this->mAngle.y);
        Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, this->mAngle.x);
        MTX_MultVec43(&localOffset, &gScratchMtx, &worldTarget);
        worldTarget.x += this->mHomePos.x;
        worldTarget.y += this->mHomePos.y;
        worldTarget.z += this->mHomePos.z;
        ApproachLinear(this->mPos, worldTarget, 0x14000);
    }
    if (this->mBlendAnmModel.Finished()) {
        this->mHorzSpeed = 0;
        SetState(&data_ov016_02114dac);
    }
    return 1;
}

/* Lunge init: bite animation with flag bit 30, and the lunge sound. */
int daMoray_c::StateLungeInit() {
    this->mBlendAnmModel.SetAnim(*(BCA_File *)data_ov016_02114d20.filePtr, 8, 0x40000000, 0x1000,
                                 0);
    func_02012694(0xfa, &this->mCamSpacePos);
    return 1;
}

/* Swim-out execute. Moves toward a point 0x76c000 (1900 units) from the home
   point along its facing at up to 0x14000 (20 units) per frame, and enters
   path-swim, with the sound, once within 20 units of it. */
int daMoray_c::StateSwimOutMain() {
    Vector3 localOffset;
    Vector3 worldTarget;
    localOffset.x = 0; localOffset.y = 0; localOffset.z = 0;
    worldTarget.x = 0; worldTarget.y = 0; worldTarget.z = 0;
    localOffset.z = 0x76c000;
    Matrix4x3_FromRotationY(&gScratchMtx, this->mAngle.y);
    Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, this->mAngle.x);
    MTX_MultVec43(&localOffset, &gScratchMtx, &worldTarget);
    worldTarget.x += this->mHomePos.x;
    worldTarget.y += this->mHomePos.y;
    worldTarget.z += this->mHomePos.z;
    ApproachLinear(this->mPos, worldTarget, 0x14000);
    if (Vec3_Dist(&this->mPos, &worldTarget) < 0x14000) {
        func_02012694(0xfa, &this->mCamSpacePos);
        SetState(&data_ov016_02114dbc);
    }
    return 1;
}

/* Swim-out init: idle animation and the same sound. */
int daMoray_c::StateSwimOutInit() {
    this->unk_400 = 0;
    func_02012694(0xfa, &this->mCamSpacePos);
    this->mBlendAnmModel.SetAnim(*(BCA_File *)data_ov016_02114d28.filePtr, 8, 0, 0x1000, 0);
    return 1;
}

/* Den-wait execute. While farther than 0xa000 (10 units) from the home
   point it moves back at up to 0x5000 (5 units) per frame and does nothing
   else. At home it snaps onto it and, with a player present, lunges when
   AngleDiff(angle from the eel to that player, mAngle.y) is below 0x3300
   (about 72 degrees either side of mAngle.y, AngleDiff being an absolute
   value), the horizontal distance is below 0x3e8000 (1000 units) and the
   height difference is below 0x800000 (2048 units). Entrance 1 widens the
   horizontal limit to 0x495000 (1173 units), tightens the height limit to
   0x6ee000 (1774 units), and also lunges when the player is within
   0x224000 (548 units) of a point 0x64000 (100 units) ahead of home. */
/* KEEPER, MEASURED (#821). Two crutches hold this body and each is load-bearing
   on its own, measured against the cartridge's 0x1c0: without the propagation
   bracket it is 0x1bc, and with `thrVert = 0x800000` folded into one store 0x1b8.
   The double store to local.z is the ROM's (dropping the first one is 0x1bc).
   Re-tried after the respellings below, not inherited. Re-measured (#821 r17):
   without the bracket still 0x1bc, and `thrVert = thrHorz + 0x418000` (the
   threshold built from the horizontal one) is 0x1b4.
   Round 64: without the bracket the only loss of substance is thrAng folded
   into `cmp r0, #0x3300` (the ROM holds it in r4). Setting thrAng = 0x3300
   again in the entrance-1 arm keeps it a variable and gives 0x1c0, but 9
   words still differ (the player-position copy and entrance take r2/r1/r0
   for the ROM's r3/r2/r1), and that second store would be a matching device
   anyway; `playerPos = player->mPos` is 0x1bc; the pragma file-wide breaks
   UpdatePose, InitResources, UpdateHitCylinders and Render. */
#pragma push
#pragma opt_propagation off
int daMoray_c::StateDenWaitMain() {
    daPly_c *player;
    Vector3 playerPos;
    Vector3 local;
    Vector3 world;
    int thrHorz;
    int thrVert;
    int thrAng;
    unsigned char entrance;
    int dy;

    if (Vec3_Dist(&this->mPos, &this->mHomePos) > 0xa000) {
        ApproachLinear(this->mPos, this->mHomePos, 0x5000);
        return 1;
    }

    this->mPos.x = this->mHomePos.x;
    this->mPos.y = this->mHomePos.y;
    this->mPos.z = this->mHomePos.z;

    player = this->ClosestPlayer();
    if (player != 0) {
        /* ACDB order: x,y,z then entrance -- colors y/z as r2 and entrance as r1 */
        const Vector3 *pos = &player->mPos;
        playerPos.x = pos->x;
        playerPos.y = pos->y;
        playerPos.z = pos->z;
        entrance = gCurrentStarId;

        thrHorz = 0x3e8000;
        thrAng = 0x3300;
        thrVert = 0x418000;
        thrVert += 0x3e8000;

        if (entrance == 1) {
            local.z = 0;
            local.z = 0x64000;
            local.x = 0;
            local.y = 0;
            world.x = 0;
            world.y = 0;
            world.z = 0;
            thrHorz = 0x495000;
            thrVert = 0x6ee000;
            Matrix4x3_FromRotationY(&gScratchMtx, this->mAngle.y);
            Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, this->mAngle.x);
            MTX_MultVec43(&local, &gScratchMtx, &world);
            world.x += this->mHomePos.x;
            world.y += this->mHomePos.y;
            world.z += this->mHomePos.z;
            if (Vec3_Dist(&playerPos, &world) < 0x224000)
                SetState(&data_ov016_02114d9c);
        }

        if (AngleDiff(this->HorzAngleToCPlayer(), this->mAngle.y) < thrAng) {
            if (Vec3_HorzDist(&this->mPos, &playerPos) < thrHorz) {
                dy = this->mPos.y - playerPos.y;
                if (dy < 0)
                    dy = -dy;
                if (dy < thrVert)
                    SetState(&data_ov016_02114d9c);
            }
        }
    }
    return 1;
}
#pragma pop

/* Den-wait init: the idle animation. */
int daMoray_c::StateDenWaitInit() {
    this->unk_400 = 0;
    this->mBlendAnmModel.SetAnim(*(BCA_File *)data_ov016_02114d28.filePtr, 8, 0, 0x1000, 0);
    return 1;
}

/* Installs a state and runs its init. */
int daMoray_c::SetState(State *state) {
    this->mState = state;
    const daMoray_c::State *installed = this->mState;
    if (installed->init == 0)
        return 1;
    return (this->*installed->init)();
}

/* Poses the body: the model matrix from position and angles, then each
   segment's world position out of its bone transform, and the carried
   star's matrix off the jaw bone. */
void daMoray_c::UpdatePose() {
    Vector3 localOffset;
    Vector3 worldTarget;
    Vector3 asr;
    Matrix4x3 *modelMtx;
    dActor_c *star;
    u32 starUniqueID;

    Vec3_Asr(&asr, &this->mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, asr.x, asr.y, asr.z);
    localOffset.z = 0;
    localOffset.z = -0x190000;
    localOffset.x = 0;
    localOffset.y = 0;
    worldTarget.x = 0;
    worldTarget.y = 0;
    worldTarget.z = 0;
    Matrix4x3_FromRotationY(&gScratchMtx, this->mAngle.y);
    MTX_MultVec43(&localOffset, &gScratchMtx, &worldTarget);
    Matrix4x3_FromTranslation(&gScratchMtx, (this->mPos.x + worldTarget.x) >> 3, this->mPos.y >> 3,
                              (this->mPos.z + worldTarget.z) >> 3);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&gScratchMtx, this->mAngle.x, this->mAngle.y,
                                           this->mAngle.z);
    this->mBlendAnmModel.mat4x3 = gScratchMtx;
    ApproachLinear(this->mSegmentAngle[6], this->mSegmentAngle[7], 0x40);
    modelMtx = &this->mBlendAnmModel.mat4x3;
    this->mSegmentAngle[5] = this->mSegmentAngle[6];
    for (int i = 0; i < 7; i++) {
        Matrix4x3 *dst;
        mSegmentPos[i].x = 0;
        mSegmentPos[i].y = 0;
        mSegmentPos[i].z = 0;
        dst = &gScratchMtx;
        *dst = *modelMtx;
        MTX_Concat43(&mBlendAnmModel.data.transforms[i], dst, dst);
        /* Measured (#821 r96): pos declared at the top of the loop is
           15 words off (its address takes r6 ahead of the bone row), and
           the << 3 through pos as well is 0x2e8 of 0x2f8. */
        Vector3 &pos = mSegmentPos[i];
        pos.x = dst->t.x;
        pos.y = dst->t.y;
        pos.z = dst->t.z;
        mSegmentPos[i].x <<= 3;
        mSegmentPos[i].y <<= 3;
        mSegmentPos[i].z <<= 3;
    }
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
    MTX_Concat43(&this->mBlendAnmModel.data.transforms[4], &this->mBlendAnmModel.mat4x3,
                 &this->mStarMtx);
    Matrix4x3_FromTranslation(&gScratchMtx, 0x28000, 0, 0);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&gScratchMtx, 0x4000, -0x8000, this->mStarSpinAngle);
    MTX_Concat43(&gScratchMtx, &this->mStarMtx, &this->mStarMtx);
    gScratchMtx = this->mStarMtx;
    this->mStarPos.x = gScratchMtx.t.x;
    this->mStarPos.y = gScratchMtx.t.y;
    this->mStarPos.z = gScratchMtx.t.z;
    this->mStarPos.x <<= 3;
    this->mStarPos.y <<= 3;
    this->mStarPos.z <<= 3;
    star->mCarryMatrix = &this->mStarMtx;
}

s32 daMoray_c::CleanupResources()
{
    data_ov016_02114d38.Release();
    data_ov016_02114d20.Release();
    data_ov016_02114d30.Release();
    data_ov016_02114d28.Release();
    return 1;
}

void daMoray_c::OnPendingDestroy()
{
}

s32 daMoray_c::Render()
{
    int i;
    MorayRenderStep *step;
    ModelBone *bone;

    func_020167a4(&mBlendAnmModel);
    bone = mBlendAnmModel.data.bones + 1;
    step = data_ov016_02114908;
    for (i = 1; i < 7; i++) {
        s16 movement = mSegmentAngle[i];
        u16 *angle = &bone->rotZ;
        *angle = *angle + (u16)(s16)(movement * step->angleScale);
        step++;
        bone++;
    }
    func_0204531c(&mBlendAnmModel.data, mBlendAnmModel.blendWeight);
    mBlendAnmModel.dExtSimpleModel_c::Render(0);
    return 1;
}

s32 daMoray_c::Behavior()
{
    dActor_c *star;
    unsigned starUniqueID;

    DecIfAbove0_Short(&mStateTimer);
    if (mState->execute)
        (this->*mState->execute)();
    UpdatePos(&mdCcAcPos_c1);
    mAngle.x = mPrevAngle.x;
    mAngle.y = mPrevAngle.y;
    mAngle.z = mPrevAngle.z;
    UpdatePose();

    starUniqueID = mStarUniqueID;
    if (starUniqueID != 0) {
        star = FindWithID(starUniqueID);
        if (star != 0) {
            if (mVariant == 1) {
                daPly_c *player = ClosestPlayer();
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
            star->mPos.x = mStarPos.x;
            star->mPos.y = mStarPos.y;
            star->mPos.z = mStarPos.z;
            mStarSpinAngle += 0x1000;
        }
    }

    UpdateHitCylinders();
    mdCcAcPos_c1.Clear();
    mdCcAcPos_c1.Update();
    if (gCurrentStarId == 1 && mState != &data_ov016_02114dbc) {
        mdCcAcPos_c2.Clear();
        mdCcAcPos_c2.Update();
    }
    mBlendAnmModel.Advance();
    return 1;
}

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
    mVariant = (param1 >> 8) & 15;
    mStarParam = (param1 >> 12) & 15;
    if (mVariant == 0xff)
        mVariant = 0;
    if (mPathID < 0)
        mPathID = 0;

    {
        PathPtr path;
        path.FromID(mPathID);
        mPathNodeCount = path.NumNodes();
    }

    mHomePos.x = mPos.x;
    mHomePos.y = mPos.y;
    mHomePos.z = mPos.z;
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

    if (gCurrentStarId == 1 || IsStarCollected(SublevelToLevel(8), 1) == 0) {
        /* Entrance 1, or star 1 still uncollected: only variant 2. */
        if (mVariant == 2) {
            mHomePos.y -= 0x80000;
            mPathNodeIndex = 8;
            if (mPathNodeIndex >= mPathNodeCount)
                mPathNodeIndex = 4;
            mPos.y = mHomePos.y;
            SetState(&data_ov016_02114d8c);
        } else {
            return 0;
        }
    } else if (gCurrentStarId == 2 || IsStarCollected(SublevelToLevel(8), 2) == 0) {
        /* Entrance 2, or star 2 still uncollected: only variant 1. */
        if (mVariant == 1) {
            /* Held (| 0x50). Behavior releases it with | 0x40. */
            spawned = Spawn(ACTOR_STAR, mStarParam | 0x50, mPos, 0, mAreaId, -1);
            if (spawned != 0) {
                mStarUniqueID = spawned->uniqueID;
                spawned->mFlags = 0;
                spawned->SetRanges(0, 0x3e8000, 0x1f40000, 0x1f40000);
            }
            SetState(&data_ov016_02114d8c);
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
            mPrevAngle.y = Vec3_HorzAngle(&mPos, &node);
            mAngle.y = mPrevAngle.y;
            SetState(&data_ov016_02114dbc);
        } else {
            return 0;
        }
    }

    for (i = 0; i < 7; i++) {
        /* array index form -> add r0,r4,ip,lsl#1 (not strength-reduced i*2) */
        mSegmentAngle[i] = 0;
        mSegmentAngle[7] = 0;
        mSegmentPos[i].x = mPos.x;
        mSegmentPos[i].y = mPos.y;
        mSegmentPos[i].z = mPos.z;
    }
    mInitAngleX = mAngle.x;
    mInitAngleY = mAngle.y;
    mInitAngleZ = mAngle.z;
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
daMoray_c *daMoray_c_classInit() {
    return new daMoray_c;
}

extern "C" SpawnInfo<daMoray_c> g_profile_MORAY = {
    daMoray_c_classInit, 242, 98, 0x00000000, 0x00000000, 0x003e8000, 0x01f40000, 0x01f40000};

/* Twelve zero bytes between the profile and the bend table. */
#pragma explicit_zero_data on
extern "C" u32 data_ov016_021148fc[3] = {0, 0, 0};
#pragma explicit_zero_data off

/* Construction order is source order: the four file handles first, then the
 * five State rows in the order the initializer copies them. The registration
 * nodes and the PMF literals are compiler temporaries. */
MorayModelFileHandle data_ov016_02114d38(ASSET_MORAY_BMD);
MorayAnimResFileHandle data_ov016_02114d20(ASSET_MORAY_SEARCH_BCA);
MorayAnimResFileHandle data_ov016_02114d30(ASSET_MORAY_SWIM_BCA);
MorayAnimResFileHandle data_ov016_02114d28(ASSET_MORAY_SWIM_START_BCA);

daMoray_c::State data_ov016_02114d8c = {&daMoray_c::StateDenWaitInit, &daMoray_c::StateDenWaitMain};
daMoray_c::State data_ov016_02114d9c = {&daMoray_c::StateLungeInit, &daMoray_c::StateLungeMain};
daMoray_c::State data_ov016_02114dac = {&daMoray_c::StateSwimOutInit, &daMoray_c::StateSwimOutMain};
daMoray_c::State data_ov016_02114dbc = {&daMoray_c::StatePathSwimInit,
                                        &daMoray_c::StatePathSwimMain};
daMoray_c::State data_ov016_02114d7c = {&daMoray_c::StateRetreatInit, &daMoray_c::StateRetreatMain};

VecFx32 data_ov016_02114d4c;
