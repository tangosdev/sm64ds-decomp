//cpp
// @symbol _ZN6Player15JumpIntoBooCageER7Vector3
/* recovered: named members + shared header, real C++ method
 *
 * Refuses while airborne and still rising, then takes control, records the
 * cage position in the 0x744 vector and turns the player to face away from it.
 */
#include "Player.h"
extern "C" {
extern int _ZN6Player17SetNoControlStateEhih(void*, unsigned char, int, unsigned char);
extern short Vec3_HorzAngle(const Vector3*, const Vector3*);
}

int Player::JumpIntoBooCage(Vector3 & v_)
{
    Vector3 *v = &v_;
    if (mIsAirborne) {
        if (mVertSpeed < 0) return 0;
    }
    if (!_ZN6Player17SetNoControlStateEhih(this, 9, -1, 0)) return 0;
    unk_744 = v->x;
    unk_748 = v->y;
    unk_74c = v->z;
    mAngleY = Vec3_HorzAngle(v, (const Vector3 *)&mPosX) + 0x8000;
    return 1;
}
