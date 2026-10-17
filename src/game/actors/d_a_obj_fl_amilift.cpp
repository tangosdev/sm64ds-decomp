//cpp
/* daObjFl_Amilift_c: a platform that rides a path, ov064 0x02117978..0x02118020,
 * 14 functions. It walks between the nodes of a PathPtr, sinks a little while
 * the player stands on it and bobs on a sine table.
 *
 * ROM name from _ZTS17daObjFl_Amilift_c. Vtable _ZTV17daObjFl_Amilift_c
 * at 0x0211bc68. The run ends with the touch helper func_ov064_02117fb4, the
 * collision callback func_ov064_02117fd4 and the registry factory
 * daObjFl_Amilift_c_classInit.
 *
 * STATES: mState indexes a three-entry table of pointers-to-member at
 * data_ov064_0211c750, filled by ov064's __sinit from the constants at
 * 0x0211bc0c, 0x0211bc14 and 0x0211bc1c (the ROM words are, in order,
 * 0x02117c24, 0x02117bdc, 0x02117b8c):
 *
 *     0  func_ov064_02117c24  wait for the player, 20 frames, then walk forward
 *     1  func_ov064_02117bdc  walk forward; at the last node switch to 2
 *     2  func_ov064_02117b8c  walk back after a 20-frame pause; at node 0 go
 *                             to 1 if auto-run, else back to 0
 *
 * #pragma defer_codegen off lays .text down in source order. One out-of-line
 * destructor emits D1 then D0.
 */

// leftovers:
// - The three state handlers and their helpers are daObjFl_Amilift_c members
//   that keep their ROM addresses as names. Renaming is a reviewer call;
//   the bodies already read as methods.
// - dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange and cstd::atan2 take
//   Fix12<int> by value, so they stay mangled calls.
// - The collider callbacks registered in InitResources
//   (dBgW::UpdatePosWithTransform and func_ov064_02117fd4) are passed as raw
//   addresses to func_020393d4 / func_020393c4, which have no header.
// - Behavior updates mStateFrames and mBobPhase through pointer locals and
//   re-reads mBobPhase as unsigned; plain member updates came out 8 bytes
//   short.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjFl_Amilift_c_AssetId {
    ASSET_FL_AMILIFT_BMD = 0x05ef,
    ASSET_FL_AMILIFT_KCL = 0x05f0
};

#include "daObjFl_Amilift_c.h"
#include "common.h"
#include "SharedFilePtr.h"

/* The resource handles as the ROM lays them out: {id, loaded file}. The
 * model handle constructs through func_02017acc and destroys through
 * func_02017ab4; the collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. The manifest aliases each
 * undefined member onto its ROM symbol. */
struct AmiliftModelFilePtr : SharedFilePtr {
    u32 id; void *ptr;

    AmiliftModelFilePtr(u32 fileID);
    ~AmiliftModelFilePtr();
};

struct AmiliftClsnFilePtr : SharedFilePtr {
    u32 id; void *ptr;

    AmiliftClsnFilePtr(u32 fileID);
    ~AmiliftClsnFilePtr();
};

/* Lets Behavior call the state table's entries as members of the lift. */
typedef void (daObjFl_Amilift_c::*LiftStateFn)();
struct LiftState {
    LiftStateFn pmf;
};

struct BMD_File;
struct KCL_File;
struct CLPS_Block;

extern "C" {
int Vec3_HorzDist(const void* a, const void* b);
int _Z14ApproachLinearRiii(int* v, int target, int step);
extern LiftState data_ov064_0211c750[];
extern short data_02082214[];
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block( void *self, KCL_File *k, Matrix4x3 *m, int fix, short s, CLPS_Block *clps);
void func_020393d4(void *p, void *v);
void func_020393c4(void *p, void *v);
extern AmiliftModelFilePtr data_ov064_0211c730;
extern AmiliftClsnFilePtr data_ov064_0211c728;
extern CLPS_Block data_ov064_0211bb6c;
extern void Vec3_Sub(Vector3* dst, Vector3* a, Vector3* b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
/* The collision callback this TU installs; defined at the end of the file. */
void func_ov064_02117fd4(void *collider, daObjFl_Amilift_c *self, dActor_c *other);
}

/* Emission order is ROM order. Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN17daObjFl_Amilift_cD1Ev
// @symbol _ZN17daObjFl_Amilift_cD0Ev
daObjFl_Amilift_c::~daObjFl_Amilift_c()
{
}

// @symbol func_ov064_02117a14
/* Turn the lift to face from node b toward node a. */
void daObjFl_Amilift_c::func_ov064_02117a14(Vector3* a, Vector3* b){
  Vector3 v;
  Vec3_Sub(&v, a, b);
  mPrevAngleY = (short)_ZN4cstd5atan2E5Fix12IiES1_(v.x, v.z);
}

// @symbol func_ov064_02117a44
/* Advance along the path by one frame. Speeds up toward 0xa000 and slows to a
 * stop within 0x69000 of the next node; once stopped there it snaps onto the
 * node, steps mNodeIndex one way or the other (per mState) and turns toward
 * the following node. Returns -1 when it has just reached either end of the
 * path (the index is then pulled back inside the path), else 0 or 1. */
int daObjFl_Amilift_c::func_ov064_02117a44() {
  Vector3 a;
  Vector3 b;
  int r;
  UpdatePos(0);
  r = (Vec3_HorzDist(&mPosX, &mToX) < 0x69000) ? 0 : 0xa000;
  if (_Z14ApproachLinearRiii(&mHorzSpeed, r, 0x800) != 0 && r == 0) {
    mFromX = mToX;
    mFromY = mToY;
    mFromZ = mToZ;
    mPosX = mFromX;
    mPosY = mFromY;
    mPosZ = mFromZ;
    r = 1;
    if (mState == daObjFl_Amilift_c::STATE_FORWARD) {
      ++mNodeIndex;
      if (mNodeIndex >= mNodeCount) {
        mNodeIndex = mNodeCount - 2;
        r = -1;
      }
    } else {
      --mNodeIndex;
      if (mNodeIndex < 0) {
        mNodeIndex = r;
        r = -1;
      }
    }
    mPathPtr.GetNode(*(Vector3 *)&mToX, mNodeIndex);
    a.x = mToX;
    a.y = mToY;
    a.z = mToZ;
    b.x = mFromX;
    b.y = mFromY;
    b.z = mFromZ;
    func_ov064_02117a14(&a, &b);
    return r;
  }
  return 0;
}

// @symbol func_ov064_02117b8c
/* State 2, walking back: pause 20 frames, then walk. On reaching node 0 go to
 * the forward state if auto-run, else wait for the player. */
void daObjFl_Amilift_c::func_ov064_02117b8c() {
    unsigned short v = mStateFrames;
    if (v < 0x14) return;
    int r = func_ov064_02117a44();
    if (r != -1) return;
    unsigned char b = mAutoRun;
    if (b == 0) {
        mState = daObjFl_Amilift_c::STATE_WAIT;
    } else {
        mState = daObjFl_Amilift_c::STATE_FORWARD;
    }
}

// @symbol func_ov064_02117bdc
/* State 1, walking forward: an auto-run lift pauses 20 frames first. On
 * reaching the last node go to the backward state. */
int daObjFl_Amilift_c::func_ov064_02117bdc()
{
    unsigned char b = mAutoRun;
    if (b == 1) {
        unsigned short v = mStateFrames;
        if (v < 0x14) return v;
    }
    int r = func_ov064_02117a44();
    if (r == -1) {
        r = 2;
        mState = r;
    }
    return r;
}

// @symbol func_ov064_02117c24
/* State 0, waiting: while the player is on the lift, count frames (Behavior
 * does the counting); once more than 20 have counted, or at once if auto-run, turn toward
 * the next node and start walking forward. With nobody on it the counter
 * is held at 0. */
void daObjFl_Amilift_c::func_ov064_02117c24()
{
    if (mRiderOn != 0) {
        if (mAutoRun != 1) {
            if (mStateFrames <= 0x14)
                return;
        }
        mState = daObjFl_Amilift_c::STATE_FORWARD;
        Vector3 v0, v1;
        v0.x = mToX;
        v0.y = mToY;
        v0.z = mToZ;
        v1.x = mFromX;
        v1.y = mFromY;
        v1.z = mFromZ;
        func_ov064_02117a14(&v0, &v1);
        return;
    }
    mStateFrames = 0;
}

// @symbol _ZN17daObjFl_Amilift_c16CleanupResourcesEv
s32 daObjFl_Amilift_c::CleanupResources() {
    mMeshCollider.Disable();
    data_ov064_0211c730.Release();
    data_ov064_0211c728.Release();
    return 1;
}

// @symbol _ZN17daObjFl_Amilift_c6RenderEv
s32 daObjFl_Amilift_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN17daObjFl_Amilift_c8BehaviorEv
/* Run the state handler, count frames (restarting the count when the state
 * changed), then set the height: the spawn height plus the sink offset (eased
 * to -0x28000 while the player is on it) plus a sine bob, applied only for
 * the model and collider update and restored afterwards. */
s32 daObjFl_Amilift_c::Behavior() {
    int idx = mState;
    (this->*data_ov064_0211c750[idx].pmf)();
    unsigned short* p338 = &mStateFrames;
    *p338 = (unsigned short)(*p338 + 1);
    if (idx != mState) {
        mStateFrames = 0;
    }
    int target = mRiderOn ? -0x28000 : 0;
    if (_Z14ApproachLinearRiii(&mSinkOffset, target, 0x5000)) {
        short* pAng = &mBobPhase;
        *pAng = (short)(*pAng + 0xa00);
        unsigned short h = *(unsigned short *)&mBobPhase;
        short s = data_02082214[(h >> 4) * 2];
        short ten = 10;
        mBobOffset = s * ten;
    }
    int t330 = mHomeY;
    int t320 = mSinkOffset;
    int t324 = mBobOffset;
    int saved = mPosY;
    mPosY = t324 + (t330 + t320);
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    mPosY = saved;
    mRiderOn = 0;
    return 1;
}

// @symbol _ZN17daObjFl_Amilift_c13InitResourcesEv
s32 daObjFl_Amilift_c::InitResources() {
    BMD_File *bmd;
    KCL_File *kcl;
    Vector3 nodeA;
    Vector3 nodeB;
    unsigned char b;

    bmd = (BMD_File *)Model::LoadFile(data_ov064_0211c730);
    mModel.SetFile(bmd, 1, -1);

    b = (param1 >> 8) & 1;
    mAutoRun = b;
    b = mAutoRun;
    if (b == 0)
        mState = STATE_WAIT;
    else
        mState = STATE_FORWARD;

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov064_0211c728);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, &data_ov064_0211bb6c);

    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, (void *)&func_ov064_02117fd4);

    mPathPtr.FromID(param1 & 0xf);
    mNodeCount = mPathPtr.NumNodes();
    mNodeIndex = 1;

    mFromX = mPosX;
    mFromY = mPosY;
    mFromZ = mPosZ;

    mPathPtr.GetNode(*(Vector3 *)&mToX, mNodeIndex);

    nodeA.x = mToX;
    nodeA.y = mToY;
    nodeA.z = mToZ;
    nodeB.x = mFromX;
    nodeB.y = mFromY;
    nodeB.z = mFromZ;

    func_ov064_02117a14(&nodeA, &nodeB);

    mHomeX = mPosX;
    mHomeY = mPosY;
    mHomeZ = mPosZ;

    return 1;
}

/* The two file-scope resource handles, in retail initializer order: the
 * model file, then the collision file. */
AmiliftModelFilePtr data_ov064_0211c730(ASSET_FL_AMILIFT_BMD);
AmiliftClsnFilePtr data_ov064_0211c728(ASSET_FL_AMILIFT_KCL);

/* The three-entry state dispatch table: wait, forward, backward. mwcc cannot
 * link-time-initialize pointer-to-member records, so the initializer copies
 * the descriptor records in. */
extern "C" LiftState data_ov064_0211c750[3] = {
    {&daObjFl_Amilift_c::func_ov064_02117c24},
    {(LiftStateFn)&daObjFl_Amilift_c::func_ov064_02117bdc},
    {&daObjFl_Amilift_c::func_ov064_02117b8c},
};

// @symbol func_ov064_02117fb4
/* The player (actor 0xbf) touched the lift: raise mRiderOn, which Behavior
 * reads to sink the lift and clears every frame. */
extern "C" void func_ov064_02117fb4(daObjFl_Amilift_c *self, dActor_c *other)
{
    u8 isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        self->mRiderOn = 1;
}

/* The collision callback InitResources installs in mMeshCollider's slot. The
 * slot passes three arguments; the touch helper wants the last two.
 * long_calls keeps the pooled absolute tail call. */
#pragma push
#pragma long_calls on
// @symbol func_ov064_02117fd4
extern "C" void func_ov064_02117fd4(void *collider, daObjFl_Amilift_c *self, dActor_c *other)
{
    func_ov064_02117fb4(self, other);
}
#pragma pop

// @symbol daObjFl_Amilift_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjFl_Amilift_c through
 * RTTI, allocation size, vtable identity, and the FL_AMILIFT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MetalNetLift_Spawn.
 *
 * `new daObjFl_Amilift_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x368), dBgActor_c's base constructor, the vptr
 * store, then mPathPtr's constructor. */
extern "C" daObjFl_Amilift_c *daObjFl_Amilift_c_classInit(void)
{
    return new daObjFl_Amilift_c;
}
