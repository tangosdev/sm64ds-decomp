//cpp
/* Clock painting hands, sharing one class and separate long/short profiles.
 * mwccarm 2004/b56 emits functions in reverse source order. Keep the ROM
 * ordinals below; the inline destructor supplies the final D1/D0 pair.
 * Layout, data ownership and matching probes are recorded in
 * notes/agents/handoffs/pr2490-source-review-0910.md.
 */

#include "daObjClock_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"

extern "C" {
extern void Matrix4x3_FromRotationZXYExt(void *, int, int, int);
int IsAreaShowing(int areaId);
extern signed char data_02092110[];
extern unsigned char data_0209f2c0[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daObjClock_c_classInit_CLOCK_LONG, 0x02111674, size 0x38        */
/* ROM ordinal 7 -- daObjClock_c_classInit_CLOCK_SHORT, 0x0211163c, size 0x38       */
/* -------------------------------------------------------------------------- */
/* The profile suffixes are reconstructed names. Both factories create the
 * same class; InitResources selects the hand from actorID. */
extern "C" {
// @symbol daObjClock_c_classInit_CLOCK_LONG
int *daObjClock_c_classInit_CLOCK_LONG(void)
{
    return (int *)new daObjClock_c;
}

// @symbol daObjClock_c_classInit_CLOCK_SHORT
int *daObjClock_c_classInit_CLOCK_SHORT(void)
{
    return (int *)new daObjClock_c;
}
}

/* Profiles are emitted in source order: long, then short. The priority
 * halfwords use the unsigned types read by the fBase_c constructor. */
struct ClockSpawnInfo {
    int *(*classInit)();
    u16 behaviorPriority;
    u16 renderPriority;
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;                 /* 0x1000 == 1.0 */
    s32 clipDistance;
    s32 farDistance;
};
typedef char ClockSpawnInfo_size_must_be_0x1c[sizeof(ClockSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_CLOCK_LONG
extern "C" ClockSpawnInfo g_profile_CLOCK_LONG = {
    daObjClock_c_classInit_CLOCK_LONG, 0x0125, 0x00a3, 0x00000006,
    0x00064000, 0x000fa000, 0x00c80000, 0x00640000
};

// @symbol g_profile_CLOCK_SHORT
extern "C" ClockSpawnInfo g_profile_CLOCK_SHORT = {
    daObjClock_c_classInit_CLOCK_SHORT, 0x0124, 0x00a2, 0x00000006,
    0x00064000, 0x000fa000, 0x00c80000, 0x00640000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN12daObjClock_c13InitResourcesEv, 0x021115cc, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjClock_c13InitResourcesEv
int daObjClock_c::InitResources()
{
    int isLongHand = (int)(actorID == 0x125);
    if (isLongHand != 0)
        mHandIndex = 0;
    else
        mHandIndex = 1;
    {
        unsigned char index = mHandIndex;
        SharedFilePtr &file = *(SharedFilePtr *)data_ov013_021116b0[index];
        mModel.SetFile((BMD_File *)Model::LoadFile(file), 1, -1);
    }
    func_ov013_02111430((char *)this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN12daObjClock_c8BehaviorEv, 0x021114cc, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjClock_c8BehaviorEv
int daObjClock_c::Behavior()
{
    if (data_02092110[0] <= 0) {
        mAngleZ += data_ov013_021116ac[mHandIndex];
    } else if (IsAreaShowing(mAreaId) && mHandIndex == 0) {
        unsigned short angle = (unsigned short)-mAngleZ;
        if (angle < 0x2000)
            data_0209f2c0[0] = 3;
        else if (angle < 0x6000 && angle >= 0x2000)
            data_0209f2c0[0] = 0;
        else if (angle < 0xa000 && angle >= 0x6000)
            data_0209f2c0[0] = 2;
        else if (angle < 0xe000 && angle >= 0xa000)
            data_0209f2c0[0] = 1;
        else if (angle >= 0xe000)
            data_0209f2c0[0] = 3;
    }
    func_ov013_02111430((char *)this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN12daObjClock_c6RenderEv, 0x021114a4, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjClock_c6RenderEv
int daObjClock_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN12daObjClock_c16CleanupResourcesEv, 0x02111478, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjClock_c16CleanupResourcesEv
int daObjClock_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov013_021116b0[mHandIndex])->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov013_02111430, 0x02111430, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov013_02111430
/* Rebuild the model matrix. The external helper boundary is retained;
 * its original member/free-function form remains unproven. */
extern "C" {
void func_ov013_02111430(char *t)
{
    daObjClock_c *self = (daObjClock_c *)t;
    Matrix4x3_FromRotationZXYExt(&self->mModel.mat4x3,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModel.mat4x3.t.x = self->mPosX >> 3;
    self->mModel.mat4x3.t.y = self->mPosY >> 3;
    self->mModel.mat4x3.t.z = self->mPosZ >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN12daObjClock_cD0Ev, 0x021113ec, size 0x44              */
/* ROM ordinal 0 -- _ZN12daObjClock_cD1Ev, 0x021113bc, size 0x30              */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjClock_cD1Ev
// @symbol _ZN12daObjClock_cD0Ev
/* The inline destructor destroys Model and the actor base; D0 also frees
 * the allocation. The measured out-of-line form adds an unlicensed D2 and
 * puts D0 before D1. See the handoff for the exact compiler probe. */
