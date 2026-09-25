//cpp
/* Castle grounds birds -- ov009/daSBird_c, profile SBIRD (BIRD 343).
 *
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / METAL_NET / FLAG / BIRD).
 * RTTI ov009:0x02113928 names daSBird_c; the debug table names SBIRD.
 * This is the flock, not the flag and not castle water.
 *
 * daSBird_c_classInit is reconstructed (RTTI daSBird_c, SBIRD registry).
 * Retail does not store that spelling. Historical alias: Bird_Spawn.
 *
 * Factory is `return new daSBird_c()`. fBase_c::operator new(unsigned long)
 * forwards `_ZN7fBase_cnwEj`. D1/D0 are leftover enrolled files -- the
 * destructor is out of line so this TU does not synthesise them.
 * Key function is ~daSBird_c(); those leftover files emit the class
 * vtable and RTTI. This TU does not.
 *
 * deslop leftovers:
 * - ModelAnim::SetAnim / DropShadowRadHeight 6az: this TU passes Fix12<int>
 *   by value; the header method form size-DIFFs.
 * - cstd::atan2 6az (Fix12<int> by value) in func_ov009_02111234.
 * - Vec3_Asr / Vec3_Sub / Vec3_HorzLen / LenVec3 / ApproachLinear: no shared
 *   header this TU can take without a campaign.
 * - data_ov009_02113c20 / 02113c28 SharedFilePtr handles: sinit constructs
 *   them as file IDs 1080 / 1081; overlay .bss owns them. symbols.txt has
 *   no recovered names, so they are not coined.
 * - data_ov009_02113c48 is the 4-state PMF table this Behavior indexes;
 *   overlay .bss owns it. Helpers stay func_ov009_* (table order: 021116ec,
 *   021115d8, 0211145c, 02111234). 02111224 is the follower-attach called
 *   from the two spawn loops.
 * - func_ov009_0211145c is a proven mwccarm 1.2/2004/b56 register-
 *   allocation wall. It lives in src/func_ov009_0211145c.c as an
 *   unenrolled draft (not in this TU's delinks span).
 *   D1/D0/02111224/02111234 stay as leftover enrolled files below
 *   that hole so this TU's complete range is 0x021115d8..0x02111a70.
 * - *(Vector3 *)&mPosX addressing shape (a Vector3 member at 0x05c is a
 *   dActor_c campaign, not this leaf).
 * - data_020a0e68 scratch matrix; data_02082214 sine table;
 *   data_0209e650 RNG; func_0201267c (sound 0x6a at mCamSpacePos).
 * - RandomIntInternal stays unsigned: signed % 400 changed codegen.
 * - S14: g_profile_SBIRD stays outside the licensed .text.
 * - common.h for Vector3_16 (Spawn rotation); included after the class
 *   header so ModelAnim's Matrix4x3 spelling wins.
 */

#include "daSBird_c.h"
#include "common.h"
#include "SharedFilePtr.h"

typedef void (daSBird_c::*BirdState)();

struct BirdMtx {
    int w[12];
};

extern "C" {
extern SharedFilePtr data_ov009_02113c20;
extern SharedFilePtr data_ov009_02113c28;
extern BirdState data_ov009_02113c48[];
extern BirdMtx data_020a0e68;
extern s16 data_02082214[];
extern unsigned int RandomIntInternal(void *rng);
extern int data_0209e650;
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Vec3_Sub(void *out, void *a, void *b);
extern s32 Vec3_HorzLen(void *v);
extern s32 LenVec3(void *v);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, s16 a);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 a);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern void _Z14ApproachLinearRsss(void *p, s16 target, s16 step);
extern int func_0201267c(unsigned int a, void *b);
extern void func_ov009_02111224(char *c, int r1);

/* ModelAnim::SetAnim -- wall 6az. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *animFile, int flags, int speed, u32 startFrame);

/* dActor_c::DropShadowRadHeight -- wall 6az. */
int _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *self, ShadowModel *sm, Matrix4x3 *m, int rad, int h, unsigned u);
}

extern int _ZTV9daSBird_c[];

// @symbol daSBird_c_classInit
extern "C" daSBird_c *daSBird_c_classInit()
{
    return new daSBird_c();
}

// @symbol _ZN9daSBird_c13InitResourcesEv
s32 daSBird_c::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov009_02113c20);
    mModelAnim.SetFile(modelFile, 1, 1);
    BCA_File *animFile = (BCA_File *)Animation::LoadFile(data_ov009_02113c28);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, animFile, 0, 0x1000, 0);
    mShadowModel.InitCylinder();
    {
        int *p60 = &mPosY;
        int y = *p60;
        int zero = 0;
        *p60 = y + 0xa000;
        mVertAccel = zero;
        mTerminalVelocity = -0x32000;
        mIsLeader = 1;
        mOwnerID = uniqueID;
        mTargetPos.x = mPosX;
        mTargetPos.y = mPosY;
        mTargetPos.z = mPosZ;
        mState = zero;
    }
    return 1;
}

// @symbol _ZN9daSBird_c8BehaviorEv
s32 daSBird_c::Behavior()
{
    (this->*data_ov009_02113c48[mState])();
    int tmp[3];
    Vec3_Asr(tmp, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, tmp[0], tmp[1], tmp[2]);
    mAngleY = mPrevAngleY;
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    *(BirdMtx *)((char *)&mModelAnim.mat4x3) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mModelAnim.mat4x3, 0x1e000, 0x7d0000, 0xf);
    mModelAnim.Advance();
    return 1;
}

// @symbol _ZN9daSBird_c6RenderEv
s32 daSBird_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN9daSBird_c16OnPendingDestroyEv
void daSBird_c::OnPendingDestroy()
{
}

// @symbol _ZN9daSBird_c16CleanupResourcesEv
s32 daSBird_c::CleanupResources()
{
    data_ov009_02113c28.Release();
    data_ov009_02113c20.Release();
    return 1;
}

// @symbol func_ov009_021116ec
extern "C" void func_ov009_021116ec(char *c)
{
    if (*(unsigned char *)(c + 0x180)) {
        int n = *(int *)(c + 8) & 0xf;
        if (n > 1) {
            Vector3_16 rot = *(Vector3_16 *)(c + 0x92);
            for (int i = 0; i < n - 1; i++) {
                Vector3 pos;
                pos.x = *(int *)(c + 0x5c)
                    + (int)((RandomIntInternal(&data_0209e650) % 400) - 0xa0)
                        * 4096;
                pos.y = *(int *)(c + 0x60);
                pos.z = *(int *)(c + 0x64)
                    + (int)((RandomIntInternal(&data_0209e650) % 400) - 0xa0)
                        * 4096;
                rot.y = (short)(rot.y
                    + (short)(RandomIntInternal(&data_0209e650) >> 16));
                void *a = dActor_c::Spawn(
                    0x157, 0, pos, &rot, *(signed char *)(c + 0xcc), -1);
                if (a)
                    func_ov009_02111224((char *)a, *(int *)(c + 4));
            }
        }
        *(int *)(c + 0x160) = -0x14000;
        *(int *)(c + 0x168) = 0xff06a000;
    }
    *(int *)(c + 0x17c) = 1;
}

// @symbol func_ov009_021115d8
extern "C" void func_ov009_021115d8(char *c)
{
    if (*(unsigned char *)(c + 0x180) != 0) {
        char *p2 = (char *)((dActor_c *)c)->ClosestPlayer();
        if (p2 != 0) {
            struct Vector3 copy;
            struct Vector3 diff;
            int len;
            Vec3_Sub(&diff, c + 0x5c, p2 + 0x5c);
            copy.x = diff.x;
            copy.y = diff.y;
            copy.z = diff.z;
            len = Vec3_HorzLen(&copy);
            if (len > 0x7d0000)
                return;
        }
        func_0201267c(0x6a, c + 0x74);
    } else {
        char *p = (char *)dActor_c::FindWithID(*(unsigned int *)(c + 0x178));
        if (p == 0) {
            ((fBase_c *)c)->MarkForDestruction();
            return;
        }
        if (*(int *)(p + 0x17c) != 3)
            return;
    }
    *(short *)(c + 0x92) = 5000
        - (unsigned int)RandomIntInternal(&data_0209e650) % 4000;
    *(int *)(c + 0x174) = 0x28000;
    *(int *)(c + 0x17c) = 3;
    *(int *)(((int)c + 0xb0)) &= ~0x10000;
}
