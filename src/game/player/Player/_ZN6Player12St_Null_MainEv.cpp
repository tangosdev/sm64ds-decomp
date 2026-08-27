//cpp
// @symbol _ZN6Player12St_Null_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
/* Player::St_Null_Main() at 0x020cac28 (ov002).
 * Main hook of the ST_NULL do-nothing player state; returns VS_FAIL (1).
 * (All ST_NULL state functions are "return 1;" per Player.h.)
 */

typedef int s32;

struct Player;

s32 Player::St_Null_Main()
{
    (void)this;
    return 1;
}
