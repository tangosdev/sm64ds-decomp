//cpp
// @symbol _ZN6Player15St_Null_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
/* Player::St_Null_Cleanup() at 0x020cac20 (ov002).
 * Cleanup hook of the ST_NULL do-nothing player state; returns VS_FAIL (1).
 * The implicit Player* this is unused.
 */

typedef int s32;

struct Player;

s32 Player::St_Null_Cleanup()
{
    (void)this;
    return 1;
}
