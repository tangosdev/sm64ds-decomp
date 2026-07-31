#include "types.h"
/* StartMinigameMenu at 0x0202adf4
 * Starts the minigame menu scene. r0 controls whether to return to rec room after.
 */
extern u8 RETURN_TO_REC_ROOM;  /* 0x0209f298 */

extern void _ZN5Scene20SetAndStopColorFaderEv(void);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 actorID, u32 param, u16 fadeColor);

void StartMinigameMenu(u8 returnToRecRoom)
{
    RETURN_TO_REC_ROOM = returnToRecRoom;
    _ZN5Scene20SetAndStopColorFaderEv();
    _ZN5Scene14StartSceneFadeEjjt(5, 0, 0x7fff);
}
