//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"

extern "C" {

extern void func_ov007_020b2614(int a, int b, int c);

/* Grows the model over states[1]'s ticks: at tick 0 it starts
 * func_ov007_020b2614(0, 0, -1); over ticks 0..10 modelGrowth rises from 0
 * to 0x17d0 (quadratic ease-in), over ticks 10..30 it eases back down to
 * 0x1000, and after that states[1] goes to state 0. At tick 10 states[2] goes
 * to state 3. Figure 1 holds the growth at 0x1000. */
void func_ov007_020b1b74(void)
{
    int ticks = gMenuWork->states[1]->stateTicks;
    if (ticks == 0)
        func_ov007_020b2614(0, 0, -1);

    if (ticks == 10)
        gMenuWork->states[2]->requestedState = 3;

    if (ticks >= 0 && ticks <= 10) {
        int rise = ProgressRatio(ticks, 10);
        int sq = (fx32)(((fx64)rise * rise) >> FX32_SHIFT);
        gMenuWork->modelGrowth = FX_MUL(sq, 0x17d0);
    } else if (ticks <= 30) {
        int fall = 0x1000 - RangeRatio(ticks, 10, 30);
        int sq = (fx32)(((fx64)fall * fall) >> FX32_SHIFT);
        gMenuWork->modelGrowth = FX_MUL(sq, 0x7d0) + 0x1000;
    } else {
        gMenuWork->states[1]->requestedState = 0;
    }

    if (gMenuWork->figureIndex == 1)
        gMenuWork->modelGrowth = 0x1000;
}
}
