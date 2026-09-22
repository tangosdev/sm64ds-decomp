//cpp
// @symbol _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE
/* recovered: real C++ method
 *
 * What happens when a mega/invincible Mario runs an enemy over. The enemy is
 * turned to face the player, given the launch velocity the caller passed, put
 * into death state 8 with a 30-frame timer, and two particle systems are spawned
 * at its position raised by whatever OnAimedAtWithEgg (vtable slot 29) reports.
 *
 * The third parameter is genuinely unused -- every caller materialises it and no
 * body reads it. include/dEnemyBase_c.h carries the census that proves both the
 * arity and this address's overlay; this file spent a long time as
 * func_ov002_020ada40 because the mangled name was sitting on ov004's unrelated
 * function at the same address.
 */
#include "dEnemyBase_c.h"
#include "common.h"

extern "C" {
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int a, const Vector3 *b);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
void _ZN6Player16IncMegaKillCountEv(void *p);
}

void dEnemyBase_c::KillByInvincibleChar(const Vector3_16 &vel_, Player &player_, Fix12<int>)
{
    mFlags &= ~0x10000001;
    mPrevAngleY = Vec3_HorzAngle((const Vector3 *)((char *)&player_ + 0x5c),
                                 (const Vector3 *)&mPosX);
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mDeathTimer = 0x1e;
    mSpinRateX = vel_.x;
    mSpinRateY = vel_.y;
    mSpinRateZ = vel_.z;
    mDeathState = 8;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (const Vector3 *)&mCamSpacePosX);
    volatile int v[3];
    v[0] = mPosX;
    v[1] = mPosY;
    v[2] = mPosZ;
    int ret = OnAimedAtWithEgg();
    int vy = v[1];
    int vx = v[0];
    vy = vy + ret;
    v[1] = vy;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, vx, vy, v[2]);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, v[0], v[1], v[2]);
    _ZN6Player16IncMegaKillCountEv(&player_);
}
