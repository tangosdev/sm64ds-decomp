//cpp
/* The planks that float on water and tip when you stand on them --
 * ov002/daObjFloatBoard_c. Abstract base of Jolly Roger Bay's raft and the
 * two Wet-Dry World boards; the leaves supply InitResources (slot 0 is pure)
 * and inherit CleanupResources, Behavior and Render from here.
 *
 * The board floats at mWaterY, sinks 0x28000 further while someone is standing
 * on it (mRider), bobs on top of that from a sine table walked by mBobPhase,
 * and tilts toward the rider. mRiderTimeout is what lets go of the rider a
 * few frames after they step off (the mesh callback reloads 5 on touch).
 *
 * deslop
 * Leftover: func_ov002_020b5ab4 / 020b5b98 keep ROM address names (neither
 *   occupies a vtable slot, nothing in the ROM names them); their (char *)
 *   signatures stay too, shared with decl_common.h and func_ov002_020b5e58.
 * Leftover: cstd::atan2 / dBgActor_c::IsClsnInRangeOnScreen stay TU-local
 *   mangled scalars (Fix12<int> by value, wall 6az; types.h).
 * Leftover: func_020393a4 is an 8-byte store into dBgW+0x0c (d_a_obj_emm_log
 *   finding); naming belongs with dBgW in arm9. Its parameter is spelled
 *   dBgW_KcMbg * because that is what dBgActor_c::mMeshCollider is declared as
 *   in include/dBgActor_c.h, so the call needs no upcast; daObjPathLift_c.cpp
 *   already spells it the same way. The three dBgW * spellings elsewhere name
 *   a base of that type, and the definition in src/func_020393a4.c still says
 *   int *.
 * Leftover: sine table data_02082214 (arm9).
 * Leftover: mBobPhase's second spelling ((char *)this + 0x300, + 0x38):
 *   collapsing it to one member access lets mwcc reuse the loaded value
 *   instead of reloading it, which the ROM does not.
 * Leftover: the rider position goes through one Vector3 * (member loads
 *   cost an instruction and DIFF).
 * Leftover: abstract class -- no factory, no g_profile row (S14).
 */

#include "daObjFloatBoard_c.h"
#include "common.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"

/* ApproachLinear's two overloads mangle back to the ROM's own
 * _Z14ApproachLinearRiii and _Z14ApproachLinearRsss, so they are declared as
 * real C++ overloads and the compiler produces the reference. */
int  ApproachLinear(int &p, int value, int speed);
void ApproachLinear(short &p, short value, short speed);

/* cstd::atan2 and dBgActor_c::IsClsnInRangeOnScreen take Fix12<int> BY VALUE,
 * which hits the documented mwccarm stack-homing wall when they are spelled
 * as real members, so they stay mangled free functions and only the call site
 * changes. See include/ShadowModel.h. Everything else here is an ordinary
 * unmangled ROM symbol. */
extern "C" {
extern s8    data_0209f2f8;      /* current stage id */
extern int   data_0209f32c;      /* the stage water's global height (written by d_a_obj_wc_mizu) */
extern short data_02082214[];    /* sin/cos table */

int  SurfaceInfo_TestFlag0x20(int *p);
void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
void func_020393a4(dBgW_KcMbg *clsn, int range);
int  Vec3_HorzDist(const void *a, const void *b);
unsigned short DecIfAbove0_Short(unsigned short *p);

int  func_ov002_020b5ab4(char *c);
void func_ov002_020b5b98(char *t);

short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
int   _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(dBgActor_c *self, int x, int z);
}

/* ROM ordinal 4 -- vtable slot 6, ov002 0x020b5c4c.
 *
 * TWO SPELLINGS OF mBobPhase ON PURPOSE: the store goes through `ctr` and the
 * reload two lines later goes through a separate `char *` base. Collapsing
 * them to one member access lets mwcc reuse the loaded value instead of
 * reloading it, which the ROM does not. */
// @symbol _ZN17daObjFloatBoard_c8BehaviorEv
int daObjFloatBoard_c::Behavior()
{
    Vector3 tpos;
    int dist;
    short atanRes;
    short angleDiff;
    int j;
    short s, co, tv;
    short val1, val2;

    func_020393a4(&mMeshCollider, 0x300000);

    if (func_ov002_020b5ab4((char *)this) != 0) {
        if (ApproachLinear(mSinkOffset, mRider != 0 ? -0x28000 : 0, 0x5000) != 0) {
            short *ctr = &mBobPhase;
            short cval = *ctr;
            char *st = (char *)this + 0x300;
            *ctr = (short)(cval + 0xa00);
            j = (*(unsigned short *)(st + 0x38) >> 4) * 2;
            tv = data_02082214[j];
            mBobOffset = 10 * tv;
        }

        if (mRider == 0) {
            val2 = 0;
            val1 = val2;
        } else {
            /* The rider's position as one address: spelling the three loads
               as rider->mPosX/Y/Z instead of [riderPos+0/4/8] costs an
               instruction and DIFFs. */
            Vector3 *riderPos = (Vector3 *)&mRider->mPosX;
            tpos.x = riderPos->x;
            tpos.y = riderPos->y;
            tpos.z = riderPos->z;
            dist = Vec3_HorzDist(&mPosX, &tpos) >> 0xc;
            atanRes = _ZN4cstd5atan2E5Fix12IiES1_(mPosX - tpos.x, mPosZ - tpos.z);
            angleDiff = atanRes - mAngleY;
            j = ((u16)angleDiff >> 4) * 2;
            s = data_02082214[j];
            co = data_02082214[j + 1];
            val1 = (short)((-dist * co) / 2200);
            val2 = (short)((dist * s) / 1500);
        }

        ApproachLinear(mAngleX, val1, 0xc0);
        ApproachLinear(mAngleZ, val2, 0xc0);

        mPosY = mWaterY + mSinkOffset + mBobOffset;
    } else {
        mPosY = mWaterY;
        mAngleZ = 0;
        mAngleX = mAngleZ;
    }

    func_ov002_020b5b98((char *)this);
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();

    if (DecIfAbove0_Short(&mRiderTimeout) == 0)
        mRider = 0;

    return 1;
}

/* ROM ordinal 3 -- vtable slot 9, ov002 0x020b5c24. Dispatches through
 * dBgActor_c's own mModel (0xd4), as every sibling in this series does. */
// @symbol _ZN17daObjFloatBoard_c6RenderEv
s32 daObjFloatBoard_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 2 -- vtable slot 3, ov002 0x020b5be0. THE KEY FUNCTION: the
 * first out-of-line virtual this class declares, so this TU emits the class's
 * _ZTV/_ZTI/_ZTS and both destructor variants.
 *
 * mFileTable is reloaded between the two Release() calls because the ROM
 * reloads it -- Release() can move it, so the second index must come from a
 * fresh read. */
// @symbol _ZN17daObjFloatBoard_c16CleanupResourcesEv
int daObjFloatBoard_c::CleanupResources()
{
    daObjFloatBoard_c_Resources *files;

    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    files = mFileTable;
    files->model->Release();
    files = mFileTable;
    files->collision->Release();
    return 1;
}

/* ROM ordinal 1 -- ov002 0x020b5b98. Writes the model matrix from the actor's
 * Euler angles and its translation row from the position >> 3. Also called
 * from func_ov002_020b5e58 in the next TU, so the (char *) signature stays. */
// @symbol func_ov002_020b5b98
extern "C" void func_ov002_020b5b98(char *t)
{
    daObjFloatBoard_c *board = (daObjFloatBoard_c *)t;

    Matrix4x3_FromRotationZXYExt(&board->mModel.mat4x3,
        board->mAngleX, board->mAngleY, board->mAngleZ);
    board->mModel.mat4x3.m[9] = board->mPosX >> 3;
    board->mModel.mat4x3.m[10] = board->mPosY >> 3;
    board->mModel.mat4x3.m[11] = board->mPosZ >> 3;
}

/* ROM ordinal 0 -- ov002 0x020b5ab4. Asks whether the board is on water and
 * writes the surface height to mWaterY: on stage 0x15 from the class's own
 * fallback level, otherwise from a downward water raycast. Reached only from
 * this TU's Behavior. */
// @symbol func_ov002_020b5ab4
extern "C" int func_ov002_020b5ab4(char *c)
{
    daObjFloatBoard_c *board = (daObjFloatBoard_c *)c;

    if (data_0209f2f8 == 0x15) {
        s32 fallback = board->mFallbackWaterY;
        board->mWaterY = fallback;
        s32 tide = data_0209f32c;
        if (tide > fallback) {
            board->mWaterY = tide;
            return 1;
        }
        return 0;
    }

    Vector3 vec;
    dBgCh_Gnd rg;
    /* Staged through locals: direct member stores schedule differently. */
    int vx = board->mPosX;
    int vz = board->mPosZ;
    int vy = board->mPosY + 0x64000;
    vec.x = vx;
    vec.y = vy;
    vec.z = vz;
    rg.StartDetectingWater();
    rg.SetObjAndPos(vec, board);
    if (rg.DetectClsn() != 0) {
        board->mWaterY = rg.clsnY;
        if (SurfaceInfo_TestFlag0x20((int *)&rg.surface) != 0) {
            return 1;
        }
    }
    return 0;
}
