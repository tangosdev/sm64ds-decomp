#include "types.h"
/* HitDeathPlane at 0x02029b84
 * Handles player hitting a death plane: either restarts (SetNextLevel) or
 * triggers a scene fade, then always exits with StartExitFaderWipe.
 */
extern s8 data_0209f2f4[];

extern void SetNextLevel(void);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
extern void StartExitFaderWipe(u32 a);

void HitDeathPlane(int arg)
{
    if (data_0209f2f4[0] != 0 || arg == 0)
        SetNextLevel();
    else
        _ZN5Scene14StartSceneFadeEjjt(8, 0, 0);
    StartExitFaderWipe(6);
}
