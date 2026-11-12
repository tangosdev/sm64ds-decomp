//cpp
/* Production translation unit for ov018/daObjSm_Lift_c.
 * 11 function(s), .text 0x021111a0..0x02111848. Cool, Cool Mountain's ski lift
 * (SM_LIFT): it follows a path and turns when a mega mushroom hits it.
 *
 * NAME: _ZTS14daObjSm_Lift_c is "14daObjSm_Lift_c" at ov018 0x02113894; _ZTI
 * at 0x02113888 reads [__si_class_type_info, that string, _ZTI10dBgActor_c].
 * The tree previously called the class SkiLift (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x021111a0), D0
 * (0x021111e4), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * Notes:
 * - InitResources keeps the param1 mask as an offset read, and the yaw
 *   adjust and node-index add as writes through a pointer to the member.
 *   Plain member arithmetic changed the function size.
 * - AngleDiff, Vec3_Sub, Vec3_MulScalar and Math_Function_0203b14c are arm9 C helpers no header declares yet.
 * The file ends with the mesh-collider hook HandlePlayerContact, the
 * veneer StoodOnCallback that InitResources installs, and the registry
 * factory daObjSm_Lift_c_classInit (0x02111818).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjSm_Lift_c_AssetId { ASSET_SM_LIFT_BMD = 0x06b3, ASSET_SM_LIFT_KCL = 0x06b4 };

#include "daObjSm_Lift_c.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "Sound.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"

/* Two CLPS blocks the lift hands to its colliders, back to back. */
int data_ov018_02112f48[4] = {0x53504c43, 0x00020008, 0x00000fc5, 0x000000ff};
int data_ov018_02112f58[4] = {0x01000fc5, 0x000000ff, 0xffffffff, 0xffffffff};

extern "C" {
int AngleDiff(int, int);
int Math_Function_0203b14c(void *p, int a, int b, int c, int d);
void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
void Vec3_MulScalar(Vector3 *out, const Vector3 *v, int scalar);
}

/* dExtSimpleModel_c handles construct through func_02017acc and destroy through
 * func_02017ab4; collision handles construct through func_02017b4c and destroy
 * through SharedFilePtr_Destruct_Clsn. The manifest aliases those undefined
 * member spellings onto the real ROM functions; each handle is the base plus
 * the two words that family actually stores. */
struct SmLiftModelFilePtr : SharedFilePtr {
    SmLiftModelFilePtr(u32 fileID);
    ~SmLiftModelFilePtr();
};

struct SmLiftClsnFileHandle : SharedFilePtr {
    SmLiftClsnFileHandle(u32 fileID);
    ~SmLiftClsnFileHandle();
};

extern SmLiftModelFilePtr gSmLiftModelFile;
extern SmLiftClsnFileHandle gSmLiftCollisionFile;

/* Emission order is ROM order. The destructor pair must stay first. */
#pragma defer_codegen off

daObjSm_Lift_c::~daObjSm_Lift_c() {}

void daObjSm_Lift_c::OnHitByMegaChar(daPly_c &player) {
    player.IncMegaKillCount();
    KillByMegaChar(player);
    mAngleY = mPrevAngleY + 0x4000;
}

void daObjSm_Lift_c::UpdateModelTransform() {
    Vector3 t;
    Vec3_Asr(&t, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, 0x4000);
    mModel.mat4x3 = data_020a0e68;
}

s32 daObjSm_Lift_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    gSmLiftModelFile.Release();
    gSmLiftCollisionFile.Release();
    return 1;
}

s32 daObjSm_Lift_c::Render() {
    mModel.Render(0);
    return 1;
}

int daObjSm_Lift_c::Behavior() {
    Vector3 nodeA, nodeB, diff, scaled;
    s16 target;
    dActor_c *other;

    if (UpdateKillByMegaChar(0x2000, 0, 0, 0) != 0)
        return 1;

    if (DecIfAbove0_Byte(&mPauseTimer) == 0)
        mMoving = 1;

    other = mTouchingPlayer;
    if (other != 0) {
        Vector3 *otherPos = &other->mPos;
        int dist = Vec3_HorzDist(&mPos, otherPos);
        target =
            (s16)(FX_MUL(dist,
                         FX_CosIdx((u16)(s16)AngleDiff(Vec3_HorzAngle(&mPos, otherPos), mAngleY) >>
                                   4)) /
                  256);
    } else {
        if (mNodeStep > 0 && mNodeIndex == 1) {
            if (Vec3_HorzDist(&mPos, (Vector3 *)&mBasePosX) < 0x10000) {
                mMoving = 0;
                mPauseTimer = 0x3c;
            }
        }
        target = 0;
    }
    ApproachLinear(mAngleX, target, 0xc0);

    if (mMoving != 0) {
        int idx2;
        int reached;

        mSoundID = Sound::PlayLong(mSoundID, 3, 0x8c, mCamSpacePos, 0);

        PathPtr path;
        path.FromID(mPathId);

        {
            int step = mNodeStep;
            int curIdx = mNodeIndex;
            idx2 = curIdx - step;
            reached = 0;
            if (step > 0) {
                if (idx2 < 0)
                    idx2 = mNodeCount - 1;
            } else {
                if (idx2 >= mNodeCount)
                    idx2 = 0;
            }
            path.GetNode(nodeA, curIdx);
        }
        path.GetNode(nodeB, idx2);

        Math_Function_0203b14c(&mHorzSpeed, 0xa000, 0x200, 0x2000, 0x200);

        Vec3_Sub(&diff, &mPos, &nodeA);
        {
            int len = VEC_Mag(&diff);
            if (len == 0 || len <= mHorzSpeed) {
                mPosX = nodeA.x;
                reached = 1;
                mPosY = nodeA.y;
                mPosZ = nodeA.z;
            } else {
                int scale = FX_Div(mHorzSpeed, len);
                Vec3_MulScalar(&scaled, &diff, scale);
                VEC_Subtract(&mPos, &scaled, &mPos);
            }
        }

        if (reached) {
            int *pIdx = &mNodeIndex;
            *pIdx = *pIdx + mNodeStep;
            if (mNodeIndex < 0) {
                if (path.Loops() != 0) {
                    mNodeIndex = mNodeCount - 1;
                } else {
                    mPauseTimer = 0x3c;
                    mMoving = 0;
                    mNodeStep = 1;
                    int *pIdx2 = &mNodeIndex;
                    *pIdx2 = *pIdx2 + (mNodeStep * 2);
                }
            }
            if (mNodeIndex >= mNodeCount) {
                if (path.Loops() != 0) {
                    mNodeIndex = 0;
                } else {
                    mPauseTimer = 0x3c;
                    mMoving = 0;
                    mNodeStep = -1;
                    int *pIdx3 = (int *)((unsigned)(int)&mNodeIndex);
                    *pIdx3 = *pIdx3 + (mNodeStep * 2);
                }
            }
        }
    } else {
        mHorzSpeed = 0;
    }

    UpdateModelTransform();

    if (IsClsnInRange(0, 0) != 0) {
        UpdateClsnPosAndRot();
    }
    mTouchingPlayer = 0;
    return 1;
}

s32 daObjSm_Lift_c::InitResources() {
    char *c = (char *)this;
    mPathId = *(int *)(c + 8) & 0xff;
    if (mPathId == 0xff)
        return 0;
    BMD_File *f = dExtSimpleModel_c::LoadFile(gSmLiftModelFile);
    ((dExtModel_c *)&mModel)->SetFile(f, 1, -1);
    UpdateModelTransform();
    UpdateClsnPosAndRot();
    KCL_File *kf = dBgW_Kc::LoadFile(gSmLiftCollisionFile);
    mMeshCollider.SetFile(kf, mClsnMat, 0x1000, mAngleY, *(CLPS_Block *)data_ov018_02112f48);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    mMeshCollider.SetStoodOnCallback(&StoodOnCallback);
    mPauseTimer = 0x3c;
    mBasePosX = mPosX;
    mBasePosY = mPosY;
    mBasePosZ = mPosZ;
    {
        short *ang = (short *)&mAngleY;
        *ang = *ang - 0x4000;
    }
    PathPtr p;
    p.FromID(mPathId);
    mNodeCount = p.NumNodes();
    mNodeStep = 1;
    {
        int *ip = (int *)&mNodeIndex;
        *ip = *ip + mNodeStep;
    }
    mTouchingPlayer = 0;
    return 1;
}

/* Remembers the player while the player touches the lift (actor ID 0xbf);
   Behavior clears mTouchingPlayer again at the end of every frame. The
   comparison is materialised into an int before the test, as the ROM does. */
void daObjSm_Lift_c::HandlePlayerContact(dActor_c *other) {
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        mTouchingPlayer = other;
}

/* dBgW callback veneer, armed by dBgW::SetStoodOnCallback in InitResources. Drops the
   collider and forwards the touching actor to HandlePlayerContact. */
void daObjSm_Lift_c::StoodOnCallback(dBgW *collider, dActor_c *self, dActor_c *other) {
    static_cast<daObjSm_Lift_c *>(self)->HandlePlayerContact(other);
}

/* Reconstructed source-style name: SM64DS proves daObjSm_Lift_c through RTTI,
 * allocation size, vtable identity, and the SM_LIFT registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: SkiLift_Spawn.
 *
 * `new daObjSm_Lift_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x354), dBgActor_c's base constructor and the
 * vptr store (the class adds no member with a constructor). */
extern "C" daObjSm_Lift_c *daObjSm_Lift_c_classInit(void) {
    return new daObjSm_Lift_c;
}

/* The ski lift's shared file handles: the model, then the collision map. Their
 * constructors and the destructor registrations make mwcc emit this TU's
 * __sinit. */
SmLiftModelFilePtr gSmLiftModelFile(ASSET_SM_LIFT_BMD);
SmLiftClsnFileHandle gSmLiftCollisionFile(ASSET_SM_LIFT_KCL);
