//cpp
/* Actor::PoofDust() at 0x0200fe3c, 0x34 bytes.
 *
 * Copies the actor's own position into a stack Vector3 and poofs there.
 * The copy is real, not an alias: PoofDustAt takes the position by reference
 * and the ROM builds a fresh three-word temporary on the stack rather than
 * passing `&mPosX`.
 */
#include "Actor.h"

void Actor::PoofDust()
{
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    PoofDustAt(pos);
}
