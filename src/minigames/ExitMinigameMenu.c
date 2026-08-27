#include "types.h"
/* ExitMinigameMenu at 0x0202ad78
 * Exits the minigame menu, either returning to rec room or fading to main menu.
 */
extern u8 data_0209f298;  /* 0x0209f298 */
extern u8 data_0209f2f8;            /* 0x0209f2f8 */

extern void LoadLevelNoReturn(u32 actorID, u32 param, u32 arg2, u32 arg3);
extern void _ZN8dScene_c14StartSceneFadeEjjt(u32 actorID, u32 param, u16 fadeColor);

void ExitMinigameMenu(void)
{
    if (data_0209f298) {
        LoadLevelNoReturn(0x32, 4, 1, 0);
        data_0209f2f8 = 6;
        _ZN8dScene_c14StartSceneFadeEjjt(3, 0, 0x7fff);
    } else {
        _ZN8dScene_c14StartSceneFadeEjjt(1, 0, 0);
    }
}
