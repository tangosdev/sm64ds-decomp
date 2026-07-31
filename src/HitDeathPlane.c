#include "types.h"
/* HitDeathPlane at 0x02029b84
 * Handles player hitting a death plane: either restarts (SetNextLevel) or
 * triggers a scene fade, then always exits with StartExitFaderWipe.
 */
extern s8 NUM_LIVES[];

extern void SetNextLevel(void);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
extern void StartExitFaderWipe(u32 a);

void HitDeathPlane(int arg)
{
    if (NUM_LIVES[0] != 0 || arg == 0)
        SetNextLevel();
    else
        _ZN5Scene14StartSceneFadeEjjt(8, 0, 0);
    StartExitFaderWipe(6);
}
