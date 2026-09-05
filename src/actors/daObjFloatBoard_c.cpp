//cpp
/* Genuine production translation unit for ov002/daObjFloatBoard_c.
 *
 * The abstract base of the planks that float on water and tip when you stand
 * on them: Jolly Roger Bay's raft and the two Wet-Dry World boards. The class
 * is declared in include/daObjFloatBoard_c.h; the cartridge's evidence for it
 * is in notes/data/class-facts/daObjFloatBoard_c.json.
 *
 * FIVE FUNCTIONS ARE WRITTEN HERE, IN REVERSE ROM ORDER, because mwccarm
 * 2004/b56 emits one .text section per function in the reverse of source
 * order. This entry licenses .text 0x020b5ab4..0x020b5e58 -- the five -- and
 * not the destructor pair below it.
 *
 * THE KEY FUNCTION IS CleanupResources, not Behavior. The key function is the
 * first virtual DECLARED out of line, and the header declares
 * CleanupResources (slot 3) before Behavior (slot 6); the inline destructor
 * above them does not count, and the pure InitResources, declared last, has no
 * body to be one. The comments the D0/D1 shards carried said Behavior; they
 * were wrong about which function it was and right about the consequence.
 * Owning the key function is what makes this TU emit the class's _ZTV/_ZTI/
 * _ZTS group and vague-linkage copies of both destructor variants.
 *
 * WHY THE DESTRUCTOR PAIR IS NOT IN THIS TU'S LICENSED RANGE. The cartridge
 * has D0 at 0x020b5a18 BELOW D1 at 0x020b5a70. No source form that keeps
 * ~daObjFloatBoard_c inline in the header can produce that order: measured
 * here, an inline-in-class destructor emits its vague pair D1-then-D0, and the
 * only lever that flips it -- an out-of-line definition, which emits D2, D0,
 * D1 -- is refused by the cartridge itself. All three descendants (ov016
 * daObjKi_Ita_c, ov029 daObjWcObj01_c and daObjWcObj06_c) INLINE this
 * destructor's vptr store: ov016 0x02112f3c/0x02112f9c/0x02112ff0 and ov029
 * 0x021111e8/0x02111248/0x02111374 all load _ZTV17daObjFloatBoard_c
 * (0x02108fdc) directly, and nothing anywhere in the image calls 0x020b5a70 or
 * 0x020b5a18 -- only vtable slots 16 and 17 name them. An out-of-line
 * destructor would make those three emit a call instead, so the header's
 * inline body is the cartridge's own form and the pair's placement is the
 * retail link's choice of vague-linkage copy, not this object's section order.
 * src/_ZN17daObjFloatBoard_cD1Ev.cpp and src/_ZN17daObjFloatBoard_cD0Ev.cpp
 * therefore keep their own delinks entries; this object's copies are licensed
 * as deadstrip-duplicate against those two ROM homes.
 *
 * NOTHING IN THE CLASS DATA GROUP LIES INSIDE THE LICENSED .text RANGE
 * 0x020b5ab4..0x020b5e58 -- _ZTS/_ZTI/_ZTV sit at ov002 0x02108fc0/0x02108fb4/
 * 0x02108fdc -- so this manifest is text-only and the emitted duplicates are
 * licensed as compiler-only output at their measured ROM homes.
 *
 * func_ov002_020b5ab4 and func_ov002_020b5b98 keep their address-derived
 * names. Neither occupies a vtable slot in any class in the image and nothing
 * in the ROM names them: 020b5ab4 is reached only from this TU's Behavior,
 * and 020b5b98 is reached from Behavior and from func_ov002_020b5e58 in the
 * next TU, so it is not even file-local. Both keep their raw-offset bodies;
 * they are the bytes that matched.
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
extern int   data_0209f32c;
extern short data_02082214[];    /* sin/cos table */

int  SurfaceInfo_TestFlag0x20(int *p);
void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
void func_020393a4(int *p, int v);
int  Vec3_HorzDist(const void *a, const void *b);
unsigned short DecIfAbove0_Short(unsigned short *p);

int  func_ov002_020b5ab4(char *c);
void func_ov002_020b5b98(char *t);

short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
int   _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(dBgActor_c *self, int x, int z);
}

/* Not called, and not in the ROM. Forces the DELETING destructor to be asked
 * for first. */
/* ROM ordinal 6 -- vtable slot 6, ov002 0x020b5c4c.
 *
 * The board floats at mWaterY, sinks 0x28000 further while someone is standing
 * on it (mRider), bobs on top of that from a sine table walked by mBobPhase,
 * and tilts toward the rider -- val1/val2 are the pitch and roll it eases into
 * with ApproachLinear. mRiderTimeout is what lets go of the rider a few frames
 * after they step off.
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

    func_020393a4((int *)&mMeshCollider, 0x300000);

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
            Vector3 *tp = (Vector3 *)((char *)mRider + 0x5c);
            tpos.x = tp->x;
            tpos.y = tp->y;
            tpos.z = tp->z;
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

/* ROM ordinal 5 -- vtable slot 9, ov002 0x020b5c24. Dispatches through
 * dBgActor_c's own mModel (0xd4), as every sibling in this series does. */
// @symbol _ZN17daObjFloatBoard_c6RenderEv
s32 daObjFloatBoard_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 4 -- vtable slot 3, ov002 0x020b5be0. THE KEY FUNCTION: the
 * first out-of-line virtual this class declares, so this TU emits the class's
 * _ZTV/_ZTI/_ZTS and both destructor variants.
 *
 * mFileTable is reloaded between the two Release() calls because the ROM
 * reloads it -- Release() can move it, so the second index must come from a
 * fresh read. */
// @symbol _ZN17daObjFloatBoard_c16CleanupResourcesEv
int daObjFloatBoard_c::CleanupResources()
{
    SharedFilePtr **fp;

    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    fp = (SharedFilePtr **)mFileTable;
    fp[0]->Release();
    fp = (SharedFilePtr **)mFileTable;
    fp[1]->Release();
    return 1;
}

/* ROM ordinal 3 -- ov002 0x020b5b98. Writes the model matrix at 0xf0 from the
 * actor's Euler angles and its translation row at 0x114/0x118/0x11c from the
 * position >> 3. Also called from func_ov002_020b5e58 in the next TU, so it is
 * not file-local. Raw offsets are the bytes that matched. */
// @symbol func_ov002_020b5b98
extern "C" void func_ov002_020b5b98(char *t)
{
    Matrix4x3_FromRotationZXYExt(t + 0xf0, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}

/* ROM ordinal 2 -- ov002 0x020b5ab4. Asks whether the board is on water and
 * writes the surface height to this+0x324 (mWaterY): on stage 0x15 from the
 * class's own fallback level at 0x344, otherwise from a downward water
 * raycast. Reached only from this TU's Behavior. */
// @symbol func_ov002_020b5ab4
extern "C" int func_ov002_020b5ab4(char *c)
{
    if (data_0209f2f8 == 0x15) {
        int x = *(int *)(c + 0x344);
        int y;
        *(int *)(c + 0x324) = x;
        y = data_0209f32c;
        if (y > x) {
            *(int *)(c + 0x324) = y;
            return 1;
        }
        return 0;
    }
    {
        Vector3 vec;
        dBgCh_Gnd rg;
        {
            int vx = *(int *)(c + 0x5c);
            int vz = *(int *)(c + 0x64);
            int vy = *(int *)(c + 0x60) + 0x64000;
            vec.x = vx;
            vec.y = vy;
            vec.z = vz;
        }
        rg.StartDetectingWater();
        rg.SetObjAndPos(vec, (dActor_c *)c);
        if (rg.DetectClsn() != 0) {
            *(int *)(c + 0x324) = rg.clsnY;
            if (SurfaceInfo_TestFlag0x20((int *)&rg.surface) != 0) {
                return 1;
            }
        }
    }
    return 0;
}
