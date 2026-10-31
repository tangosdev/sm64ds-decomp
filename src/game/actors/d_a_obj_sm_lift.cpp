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
 * - InitResources keeps the param1 mask, the yaw adjust, and the
 *   node-index add as offset writes. Naming them changed the function size.
 * - Vec3_Asr, AngleDiff, Vec3_Sub, LenVec3, Vec3_MulScalar, SubVec3 and
 *   Math_Function_0203b14c are arm9 C helpers no header declares yet.
 * The file ends with the mesh-collider hook HandlePlayerContact, the
 * veneer BeforeClsnCallback that InitResources installs, and the registry
 * factory daObjSm_Lift_c_classInit (0x02111818).
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjSm_Lift_c_AssetId { ASSET_SM_LIFT_BMD = 0x06b3, ASSET_SM_LIFT_KCL = 0x06b4 };

#include "daObjSm_Lift_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "Sound.h"

extern s16 data_02082214[];
extern int data_ov018_02112f48[];

extern "C" {
void Vec3_Asr(Vector3 *d, Vector3 *s, int n);
void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
int AngleDiff(int, int);
void Math_Function_0203b14c(void *p, int a, int b, int c, int d);
void Vec3_Sub(void *out, void *a, void *b);
int LenVec3(void *v);
void Vec3_MulScalar(void *out, void *v, int scalar);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
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

// @symbol _ZN14daObjSm_Lift_cD1Ev
// @symbol _ZN14daObjSm_Lift_cD0Ev
daObjSm_Lift_c::~daObjSm_Lift_c() {}

// @symbol _ZN14daObjSm_Lift_c15OnHitByMegaCharER6Player
void daObjSm_Lift_c::OnHitByMegaChar(Player &player) {
    player.IncMegaKillCount();
    KillByMegaChar(player);
    mAngleY = mPrevAngleY + 0x4000;
}

// @symbol _ZN14daObjSm_Lift_c20UpdateModelTransformEv
void daObjSm_Lift_c::UpdateModelTransform() {
    Vector3 t;
    Vec3_Asr(&t, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, 0x4000);
    mModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN14daObjSm_Lift_c16CleanupResourcesEv
s32 daObjSm_Lift_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    gSmLiftModelFile.Release();
    gSmLiftCollisionFile.Release();
    return 1;
}

// @symbol _ZN14daObjSm_Lift_c6RenderEv
s32 daObjSm_Lift_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjSm_Lift_c8BehaviorEv
int daObjSm_Lift_c::Behavior() {
    char *self = (char *)this;
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
        target = (s16)(FX_MUL(dist, data_02082214[((u16)(s16)AngleDiff(
                                                       Vec3_HorzAngle(&mPos, otherPos), mAngleY) >>
                                                   4) *
                                                      2 +
                                                  1]) /
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

        Vec3_Sub(&diff, &mPosX, &nodeA);
        {
            int len = LenVec3(&diff);
            if (len == 0 || len <= mHorzSpeed) {
                mPosX = nodeA.x;
                reached = 1;
                mPosY = nodeA.y;
                mPosZ = nodeA.z;
            } else {
                int scale = cstd::fdiv(mHorzSpeed, len);
                Vec3_MulScalar(&scaled, &diff, scale);
                SubVec3((Vector3 *)&mPosX, (Vector3 *)&scaled, (Vector3 *)&mPosX);
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
                    int *pIdx3 = (int *)((unsigned)(int)self + 0x33c);
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

// @symbol _ZN14daObjSm_Lift_c13InitResourcesEv
s32 daObjSm_Lift_c::InitResources() {
    char *c = (char *)this;
    mPathId = *(int *)(c + 8) & 0xff;
    if (mPathId == 0xff)
        return 0;
    void *f = dExtSimpleModel_c::LoadFile(gSmLiftModelFile);
    ((dExtModel_c *)(c + 0xd4))->SetFile((BMD_File *)f, 1, -1);
    UpdateModelTransform();
    UpdateClsnPosAndRot();
    void *kf = dBgW_Kc::LoadFile(gSmLiftCollisionFile);
    mMeshCollider.SetFile((KCL_File *)kf, mClsnMat, 0x1000, mAngleY,
                          *(CLPS_Block *)data_ov018_02112f48);
    func_020393d4((int *)(c + 0x124), (int)dBgW::UpdatePosWithTransform);
    func_020393c4((int *)(c + 0x124), (int)&daObjSm_Lift_c::BeforeClsnCallback);
    mPauseTimer = 0x3c;
    mBasePosX = mPosX;
    mBasePosY = mPosY;
    mBasePosZ = mPosZ;
    {
        short *ang = (short *)(c + 0x8e);
        *ang = *ang - 0x4000;
    }
    PathPtr p;
    p.FromID(mPathId);
    mNodeCount = p.NumNodes();
    mNodeStep = 1;
    {
        int *ip = (int *)(c + 0x33c);
        *ip = *ip + mNodeStep;
    }
    mTouchingPlayer = 0;
    return 1;
}

// @symbol _ZN14daObjSm_Lift_c19HandlePlayerContactEP8dActor_c
/* Remembers the player while the player touches the lift (actor ID 0xbf);
   Behavior clears mTouchingPlayer again at the end of every frame. The
   comparison is materialised into an int before the test, as the ROM does. */
void daObjSm_Lift_c::HandlePlayerContact(dActor_c *other) {
    int isPlayer = other->actorID == 0xbf;
    if (isPlayer)
        mTouchingPlayer = other;
}

// @symbol _ZN14daObjSm_Lift_c18BeforeClsnCallbackEPvPS_P8dActor_c
/* dBgW callback veneer, armed by func_020393c4 in InitResources. Drops the
   collider and forwards the touching actor to HandlePlayerContact. */
void daObjSm_Lift_c::BeforeClsnCallback(void *collider, daObjSm_Lift_c *self, dActor_c *other) {
    self->HandlePlayerContact(other);
}

/* Reconstructed source-style name: SM64DS proves daObjSm_Lift_c through RTTI,
 * allocation size, vtable identity, and the SM_LIFT registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: SkiLift_Spawn.
 *
 * `new daObjSm_Lift_c` is the whole sequence the loose factory spelled by
 * hand: fBase_c::operator new(0x354), dBgActor_c's base constructor and the
 * vptr store (the class adds no member with a constructor). */
// @symbol daObjSm_Lift_c_classInit
extern "C" daObjSm_Lift_c *daObjSm_Lift_c_classInit(void) {
    return new daObjSm_Lift_c;
}

/* The ski lift's shared file handles: the model, then the collision map. Their
 * constructors and the destructor registrations make mwcc emit this TU's
 * __sinit. */
SmLiftModelFilePtr gSmLiftModelFile(ASSET_SM_LIFT_BMD);
SmLiftClsnFileHandle gSmLiftCollisionFile(ASSET_SM_LIFT_KCL);
