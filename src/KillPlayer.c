#include "types.h"
/* KillPlayer at 0x02029b30
 * Kills the player: if lives remain, go to next level with reason 2,
 * otherwise trigger a game-over scene fade.  Then plays death sound.
 */
extern s8 NUM_LIVES[];

extern void SetNextLevel(u32 reason);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
extern void StartExitFaderWipe(u32 a);
extern void func_02012790(u32 soundID);

void KillPlayer(void)
{
    if (NUM_LIVES[0] != 0)
        SetNextLevel(2);
    else
        _ZN5Scene14StartSceneFadeEjjt(8, 0, 0);
    StartExitFaderWipe(4);
    func_02012790(0x18);
}
