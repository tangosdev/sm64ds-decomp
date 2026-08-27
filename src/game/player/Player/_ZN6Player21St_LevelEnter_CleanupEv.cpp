//cpp
// @symbol _ZN6Player21St_LevelEnter_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
/* Player::St_LevelEnter_Cleanup() at 0x020c6f34 (ov002).
 * Cleanup hook of the ST_LEVEL_ENTER player state-machine state; a no-op
 * that returns VS_FAIL (1). The implicit Player* this is unused.
 */

typedef int s32;

struct Player;

s32 Player::St_LevelEnter_Cleanup()
{
    (void)this;
    return 1;
}
