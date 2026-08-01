//cpp
#include "types.h"
// @symbol _ZN6Player11OpenBigDoorEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
/* Player::OpenBigDoor() at 0x020ca144 (ov002).
 * Sets a u8 flag at offset 0x70b (1803) in the Player object to 1.
 * Player.h has only `u8 unk70b` here, so keep the typed offset access.
 */
struct Player;

void Player::OpenBigDoor()
{
    mIsOpeningBigDoor = 1; /* +0x70b */
}
