//cpp
/* Production translation unit for ov018/daObjSm_Lift_c.
 * 8 function(s), .text 0x021111a0..0x021117e8. Cool, Cool Mountain's ski lift
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
 * Leftover: dBgW_KcMbg::SetFile, dBgActor_c::UpdateKillByMegaChar and
 *   dBgActor_c::IsClsnInRange take Fix12<int> by value, so they stay mangled;
 *   a member call homes the argument and changes the ROM ABI.
 * Leftover: the Sound::PlayLong extern is the shards' own spelling.
 *   Retyping it is declaration-changing matching work, so its banked
 *   decl-agreement entry moved to this file unchanged. The path handles
 *   are still 8-byte local views of PathPtr, constructed explicitly.
 * Leftover: InitResources keeps the param1 mask, the yaw adjust, and the
 *   node-index add as offset writes. Naming them changed the function size.
 * Leftover: func_ov018_021117e8, func_ov018_02111804 (the callback
 *   InitResources installs) and the factory daObjSm_Lift_c_classInit
 *   (0x02111818) sit past this run's right edge and stay one-function sources.
 */

#include "decl_common.h"
#include "daObjSm_Lift_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"

extern "C" {
void Vec3_Asr(Vector3 *d, Vector3 *s, int n);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angY);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, short angX);
extern Matrix4x3 data_020a0e68;
extern s16 data_02082214[];

int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(void *self, s16 a, s16 b, s16 c, int d);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *th, void *kf, void *mtx, int fx, short s, void *clps);
void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
void _ZN7PathPtrC1Ev(void *self);

u8 DecIfAbove0_Byte(u8 *p);
int Vec3_HorzDist(const void *a, const void *b);
s16 Vec3_HorzAngle(const void *a, const void *b);
void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, void *pos, unsigned int d);
void Math_Function_0203b14c(void *p, int a, int b, int c, int d);
void Vec3_Sub(void *out, void *a, void *b);
int LenVec3(void *v);
int _ZN4cstd4fdivEii(int a, int b);
void Vec3_MulScalar(void *out, void *v, int scalar);
void SubVec3(void *a, void *b, void *c);
}

struct PathBytes { char b[8]; };
struct PathPtrObj { int a, b; };

/* Emission order is ROM order. The destructor pair must stay first. */
#pragma defer_codegen off

// @symbol _ZN14daObjSm_Lift_cD1Ev
// @symbol _ZN14daObjSm_Lift_cD0Ev
daObjSm_Lift_c::~daObjSm_Lift_c()
{
}

// @symbol _ZN14daObjSm_Lift_c15OnHitByMegaCharER6Player
void daObjSm_Lift_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    KillByMegaChar(player);
    mAngleY = mPrevAngleY + 0x4000;
}

// @symbol func_ov018_02111278
extern "C" void func_ov018_02111278(void *p)
{
    daObjSm_Lift_c *self = (daObjSm_Lift_c *)p;
    Vector3 t;
    Vec3_Asr(&t, (Vector3 *)&self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, self->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, self->mAngleX);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, 0x4000);
    self->mModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN14daObjSm_Lift_c16CleanupResourcesEv
s32 daObjSm_Lift_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    reinterpret_cast<SharedFilePtr &>(data_ov018_02113bc8).Release();
    reinterpret_cast<SharedFilePtr &>(data_ov018_02113bc0).Release();
    return 1;
}

// @symbol _ZN14daObjSm_Lift_c6RenderEv
s32 daObjSm_Lift_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjSm_Lift_c8BehaviorEv
int daObjSm_Lift_c::Behavior()
{
    char *self = (char *)this;
    struct PathPtrObj path;
    Vector3 nodeA, nodeB, diff, scaled;
    s16 target;
    char *other;

    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(self, 0x2000, 0, 0, 0) != 0)
        return 1;

    if (DecIfAbove0_Byte(&unk_331) == 0)
        unk_330 = 1;

    other = (char *)unk_320;
    if (other != 0) {
        char *otherPos = other + 0x5c;
        int dist = Vec3_HorzDist(&mPosX, otherPos);
        target = (s16)((int)(((s64)dist * data_02082214[((u16)(s16)AngleDiff(Vec3_HorzAngle(&mPosX, otherPos), mAngleY) >> 4) * 2 + 1] + 0x800) >> 12) / 256);
    } else {
        if (mNodeStep > 0 && mNodeIndex == 1) {
            if (Vec3_HorzDist(&mPosX, &mBasePosX) < 0x10000) {
                unk_330 = 0;
                unk_331 = 0x3c;
            }
        }
        target = 0;
    }
    _Z14ApproachLinearRsss(&mAngleX, target, 0xc0);

    if (unk_330 != 0) {
        int idx2;
        int reached;

        unk_350 = _ZN5Sound8PlayLongEjjjRK7Vector3s(unk_350, 3, 0x8c, &mCamSpacePosX, 0);

        _ZN7PathPtrC1Ev(&path);
        ((PathPtr *)&path)->FromID(mPathId);

        {
            int step = mNodeStep;
            int curIdx = mNodeIndex;
            idx2 = curIdx - step;
            reached = 0;
            if (step > 0) {
                if (idx2 < 0) idx2 = mNodeCount - 1;
            } else {
                if (idx2 >= mNodeCount) idx2 = 0;
            }
            ((PathPtr *)&path)->GetNode(nodeA, curIdx);
        }
        ((PathPtr *)&path)->GetNode(nodeB, idx2);

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
                int scale = _ZN4cstd4fdivEii(mHorzSpeed, len);
                Vec3_MulScalar(&scaled, &diff, scale);
                SubVec3(&mPosX, &scaled, &mPosX);
            }
        }

        if (reached) {
            int *pIdx = &mNodeIndex;
            *pIdx = *pIdx + mNodeStep;
            if (mNodeIndex < 0) {
                if (((PathPtr *)&path)->Loops() != 0) {
                    mNodeIndex = mNodeCount - 1;
                } else {
                    unk_331 = 0x3c;
                    unk_330 = 0;
                    mNodeStep = 1;
                    int *pIdx2 = &mNodeIndex;
                    *pIdx2 = *pIdx2 + (mNodeStep * 2);
                }
            }
            if (mNodeIndex >= mNodeCount) {
                if (((PathPtr *)&path)->Loops() != 0) {
                    mNodeIndex = 0;
                } else {
                    unk_331 = 0x3c;
                    unk_330 = 0;
                    mNodeStep = -1;
                    int *pIdx3 = (int *)((unsigned)(int)self + 0x33c);
                    *pIdx3 = *pIdx3 + (mNodeStep * 2);
                }
            }
        }
    } else {
        mHorzSpeed = 0;
    }

    func_ov018_02111278(self);

    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(self, 0, 0) != 0) {
        UpdateClsnPosAndRot();
    }
    *(int *)(self + 0x320) = 0;
    return 1;
}

// @symbol _ZN14daObjSm_Lift_c13InitResourcesEv
s32 daObjSm_Lift_c::InitResources()
{
    char *c = (char *)this;
    mPathId = *(int *)(c + 8) & 0xff;
    if (mPathId == 0xff)
        return 0;
    void *f = Model::LoadFile(reinterpret_cast<SharedFilePtr &>(data_ov018_02113bc8));
    ((ModelBase *)(c + 0xd4))->SetFile((BMD_File *)f, 1, -1);
    func_ov018_02111278(c);
    UpdateClsnPosAndRot();
    void *kf = dBgW_Kc::LoadFile(reinterpret_cast<SharedFilePtr &>(data_ov018_02113bc0));
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, kf, c + 0x2ec, 0x1000, mAngleY, data_ov018_02112f48);
    func_020393d4((int *)(c + 0x124), (int)dBgW::UpdatePosWithTransform);
    func_020393c4((int *)(c + 0x124), (int)func_ov018_02111804);
    unk_331 = 0x3c;
    mBasePosX = mPosX;
    mBasePosY = mPosY;
    mBasePosZ = mPosZ;
    {
        short *ang = (short *)(c + 0x8e);
        *ang = *ang - 0x4000;
    }
    PathBytes p;
    _ZN7PathPtrC1Ev(&p);
    ((PathPtr *)&p)->FromID(mPathId);
    mNodeCount = ((PathPtr *)&p)->NumNodes();
    mNodeStep = 1;
    {
        int *ip = (int *)(c + 0x33c);
        *ip = *ip + mNodeStep;
    }
    unk_320 = 0;
    return 1;
}
