//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

extern void func_ov007_020b2614(int a, int b, int c);

/* Grows the model over states[1]'s ticks: at tick 0 it starts
 * func_ov007_020b2614(0, 0, -1); over ticks 0..10 modelGrowth rises from 0
 * to 0x17d0 (quadratic ease-in), over ticks 10..30 it eases back down to
 * 0x1000, and after that states[1] goes to state 0. At tick 10 states[2] goes
 * to state 3. Figure 1 holds the growth at 0x1000. */
void func_ov007_020b1b74(void)
{
    int r4 = WORK->states[1]->stateTicks;
    if (r4 == 0)
        func_ov007_020b2614(0, 0, -1);

    if (r4 == 0xa)
        WORK->states[2]->requestedState = 3;

    if (r4 >= 0 && r4 <= 0xa) {
        int val;
        int sq;
        if (r4 <= 0)
            val = 0;
        else if (r4 >= 0xa)
            val = 0x1000;
        else
            val = FX_DivS32(r4 << 12, 0xa);
        sq = (int)(((long long)val * val) >> 12);
        WORK->modelGrowth = FX_MUL(sq, 0x17d0);
    } else if (r4 <= 0x1e) {
        int val;
        int t;
        int sq;
        if (r4 <= 0xa)
            val = 0;
        else if (r4 >= 0x1e)
            val = 0x1000;
        else
            val = FX_DivS32((r4 - 0xa) << 12, 0x14);
        t = 0x1000 - val;
        sq = (int)(((long long)t * t) >> 12);
        WORK->modelGrowth = FX_MUL(sq, 0x7d0) + 0x1000;
    } else {
        WORK->states[1]->requestedState = 0;
    }

    if (WORK->figureIndex == 1)
        WORK->modelGrowth = 0x1000;
}
}
