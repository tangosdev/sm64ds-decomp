#include "types.h"
/* StartMinigameMenu at 0x0202adf4
 * Starts the minigame menu scene. r0 controls whether to return to rec room after.
 */
extern u8 data_0209f298;  /* 0x0209f298 */

extern void _ZN8dScene_c20SetAndStopColorFaderEv(void);
extern void _ZN8dScene_c14StartSceneFadeEjjt(u32 actorID, u32 param, u16 fadeColor);

void StartMinigameMenu(u8 returnToRecRoom)
{
    data_0209f298 = returnToRecRoom;
    _ZN8dScene_c20SetAndStopColorFaderEv();
    _ZN8dScene_c14StartSceneFadeEjjt(5, 0, 0x7fff);
}
