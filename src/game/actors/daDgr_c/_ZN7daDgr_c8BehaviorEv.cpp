//cpp
// @symbol _ZN7daDgr_c8BehaviorEv
/* daDgr_c::Behavior -- vtable slot 6, ov025 0x021113f0.
 *
 * A swinging platform. mPhaseTimer is the phase counter, mSwingStage the stage counter
 * (-1 is the "hold at the end" sentinel), mSwingDir the direction flag. The swing
 * speed comes from `10 - mSwingStage' folded to a magnitude and bucketed to 1/2/4,
 * so the platform eases in and out; mBasePosY holds the resting height that the
 * sine offset from data_02082214 is added to. Two dust particle systems
 * (mDustParticle1, mDustParticle2) trail either end, and the last frame of each swing shakes
 * the ground.
 *
 * IsClsnInRange, Earthquake and Particle::System::New all carry Fix12<int> by
 * value in their mangled names (wall 6az), so all three stay extern-C free
 * functions.
 *
 * LAUNDER() is a no-op macro the legacy file used to MARK its read-modify-write
 * sites. It is kept, with its name, so the marking survives -- it emits nothing
 * and it is not the reason those sites take an address. Every one of them now
 * names the member: `(s8 *)LAUNDER(&mSwingStage)', not `(s8 *)LAUNDER(self + 0x328)'.
 *
 * Swept greedily against build_pin.verify -- 33 substitutions, 33 kept once the
 * three real obstacles were fixed: `mAngleY' is read UNSIGNED at the table-index
 * sites (an s16 read costs four words, the same finding Trap::InitResources
 * produced), the three func_ov025_* helpers are declared `char *' in
 * decl_common.h so `this' needs a cast, and the two-step `b = self + 0x300;
 * b + 0x28' was just mSwingStage reached the long way round.
 */
#include "daDgr_c.h"
#include "decl_common.h"

typedef struct { s32 x, y, z; } Vec3;

extern "C" {
extern s16 data_02082214[];

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(daDgr_c *self, s32 a, s32 b);
void func_02012694(s32 a, void *b);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(daDgr_c *self, Vec3 *pos, s32 fix);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 effect, s32 x, s32 y, s32 z, const void *rot, void *cb);
}

#define LAUNDER(p) (p)

s32 daDgr_c::Behavior()
{
    s32 loc[6];
    s32 n;
    s32 m;

    if (mSwingStage == -1) {
        if (mPhaseTimer == 0x20) {
            mSwingStage = 0;
            mPhaseTimer = 0;
        } else {
            u16 *cnt = (u16 *)LAUNDER(&mPhaseTimer);
            (*cnt)++;
            unk_0ac = 0;
            mAngleXSpeed = 0;
            func_ov025_02111344((char *)this);
            if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0)) {
                func_ov025_021112e0((char *)this);
            }
            return 1;
        }
    }

    n = 10 - mSwingStage;
    if (n < 0) {
        s32 neg = -1;
        n = n * neg;
    }
    n = n - 6;
    if (n < 0)
        n = 0;

    if (mPhaseTimer == n + 8) {
        s8 *p;
        mPhaseTimer = 0;
        p = (s8 *)LAUNDER(&mSwingStage);
        *p = (s8)(*p + 1);
        if (mSwingStage == 0x14) {
            u8 *pd = (u8 *)LAUNDER(&mSwingDir);
            *pd = (u8)(*pd ^ 1);
            mSwingStage = -1;
        }
    }

    if ((u32)(n - 3) <= 1u) {
        n = 4;
    } else if ((u32)(n - 1) <= 1u) {
        n = 2;
    } else if (n == 0) {
        n = 1;
    }

    m = n << 3;

    if (mPhaseTimer < m) {
        if (mSwingDir == 0) {
            unk_0ac = 0x14000 / n;
            mAngleXSpeed = (s16)(0x400 / n);
        } else {
            unk_0ac = (-0x14000) / n;
            mAngleXSpeed = (s16)((-0x400) / n);
        }

        {
            s32 *pz = (s32 *)LAUNDER(&mPosZ);
            s16 *pr = (s16 *)LAUNDER(&mAngleX);
            *pz = *pz + unk_0ac;
            *pr = (s16)(*pr + mAngleXSpeed);
            if ((mAngleX & 0x1fff) < 0x320) {
                if (mAngleXSpeed != 0)
                    func_02012694(0x65, (char *)&mCamSpacePosX);
            }
        }

        {
            s16 t = mAngleX;
            s32 idx = ((u16)(s16)(t << 2) >> 4) * 2;
            s32 prod = (s32)data_02082214[idx] * 23;
            if (prod < 0) {
                s32 neg = -1;
                prod = prod * neg;
            }
            mPosY = mBasePosY + prod;
        }

        if (mPhaseTimer == m - 1) {
            loc[3] = mPosX;
            loc[4] = mPosY;
            loc[5] = mPosZ;
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, (Vec3 *)&loc[3], 0x3e8000);
        }

        loc[0] = mPosX;
        loc[1] = mPosY;
        loc[2] = mPosZ;
        {
            s32 scale = 100;
            s32 zero = 0;
            s32 c;
            c = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            loc[0] = c * scale + mPosX;
            loc[1] = mBasePosY - 0xb9000;
            c = data_02082214[((u16)mAngleY >> 4) * 2];
            loc[2] = c * scale + mPosZ;
            mDustParticle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mDustParticle1, 0x2d, loc[0], loc[1], loc[2], (void *)zero, (void *)zero);
        }
        {
            s32 scale = 100;
            s32 zero = 0;
            s32 c;
            c = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            loc[0] = mPosX - c * scale;
            c = data_02082214[((u16)mAngleY >> 4) * 2];
            loc[2] = mPosZ - c * scale;
            mDustParticle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mDustParticle2, 0x2d, loc[0], loc[1], loc[2], (void *)zero, (void *)zero);
        }
    }

    func_ov025_02111344((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) || func_ov025_0211123c((char *)this)) {
        func_ov025_021112e0((char *)this);
    }

    {
        u16 *cnt = (u16 *)LAUNDER(&mPhaseTimer);
        (*cnt)++;
    }
    return 1;
}
