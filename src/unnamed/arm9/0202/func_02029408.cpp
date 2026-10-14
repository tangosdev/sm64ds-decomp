//cpp
// @symbol func_02029408
/* Can the current player pause? Forwards to Player::CanPause on the active
 * entry of the live player list (data_0209f394, indexed by data_0209f250).
 * dScStage_c polls it before opening the pause menu. */
#include "Player.h"

extern unsigned char data_0209f250;
extern void *data_0209f394[];

extern "C" int func_02029408(void)
{
    return ((Player *)data_0209f394[data_0209f250])->CanPause();
}
