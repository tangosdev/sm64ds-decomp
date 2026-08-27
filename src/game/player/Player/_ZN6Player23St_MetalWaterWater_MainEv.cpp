//cpp
// @symbol _ZN6Player23St_MetalWaterWater_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Metal Mario walking along the bottom underwater. Losing both the metal and
 * mega caps drops back to the swim state. While mStateWork is 0 the player
 * walks and sinks -- gravity and terminal velocity depend on whether the water
 * surface (data_0209f32c) is more than 0x46000 above (0xd2000 when mega).
 * Landing plays a thud, spawns a splash particle, and either returns to the
 * ground state or starts the landing animation.
 */
#include "Player.h"

struct S18 { short a; short b; int c[5]; };

extern "C" {
extern int func_ov002_020cec2c(void *c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *c, void *s);
extern int func_ov002_020eeca8(void *a, void *b);
extern int func_ov002_020bf224(void *c, int a, int b);
extern void ApproachAngle(short *p, int target, int a, int b, int d);
extern void _Z14ApproachLinearRiii(int *v, int a, int b);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void *v);
/* Particle::System::NewSimple and SetAnim take Fix12<int> by value -- the
   mwccarm 6az wall, runbook section 7 -- so they stay extern "C". */
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *c, u32 a, int b, int d, u32 e);
extern void Player_AdvanceAnims(void *c);

extern u8 data_020a0e40;
extern struct S18 data_0209f4a0[];
extern int data_0209f32c;             /* water surface height */
extern int data_ov002_0211067c;       /* the swim state */
extern int data_ov002_021106ac;       /* the underwater-ground state */
}

int Player::St_MetalWaterWater_Main()
{
    /* volatile: the original pins these stores to memory in this order. */
    volatile Vector3 pos;
    u32 f;
    int v;
    int lim;
    int py;
    int px, py2, pz;

    if (func_ov002_020cec2c(this) != 0) return 1;
    f = mIsMetal;
    if (f == 0 && mIsMega == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211067c);
        return 1;
    }
    if (f != 0) func_ov002_020eeca8(&mMeshClsn, this);

    if (mStateWork == 0) {
        v = 0;
        if (data_0209f4a0[data_020a0e40].a != 0) {
            v = func_ov002_020bf224(this, 0x14000, 0x1000);
            ApproachAngle(&mPrevAngleY, mDesiredAngleY, 0x20, 0x1000, 0x10);
        }
        mAngleY = mPrevAngleY;
        _Z14ApproachLinearRiii(&mHorzSpeed, v, 0x400);
        py = mPosY;
    lim = py + 0x46000;
    if (mIsMega != 0) lim = py + 0xd2000;
        if (data_0209f32c > lim) {
            mVertAccel = -0x1800;
            mTerminalVelocity = -0x14000;
        } else {
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x4b000;
        }
        if (mIsAirborne == 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0xa9, &mCamSpacePosX);
            px = mPosX;
            pos.x = px;
            py2 = mPosY;
            pos.y = py2;
            pz = mPosZ;
            pos.z = pz;
            if (mIsMega != 0) {
                py2 += 0x19000;
                pos.y = py2;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5c, px, py2, pz);
            } else {
                py2 += 0x32000;
                pos.y = py2;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd7, px, py2, pz);
            }
            if (data_0209f4a0[data_020a0e40].a != 0) {
                _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_021106ac);
            } else {
                mHorzSpeed = 0;
                if (mStateStep != 0)
                    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x52, 0x40000000, 0x1000, 0);
                else
                    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x55, 0x40000000, 0x1000, 0);
                mStateWork = 1;
            }
        }
    } else {
        if (FinishedAnim() != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_021106ac);
            return 1;
        }
    }
    Player_AdvanceAnims(this);
    return 1;
}
