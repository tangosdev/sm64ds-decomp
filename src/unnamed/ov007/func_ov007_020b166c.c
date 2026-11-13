#include "Ov007MenuState.h"
#include "Ov007Work.h"
#define FigureScene_SetScreenBlend _Z26FigureScene_SetScreenBlendi
#define RequestScreenFade _Z17RequestScreenFadeiiPFvvE

#define func_ov007_020c1d8c _Z19func_ov007_020c1d8ci
extern void func_ov007_020c1d8c(int i);
extern void FigureScene_SetScreenBlend(int n);
extern void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern void func_ov007_020b16f0(void);

/* On states[1]'s first tick: sets the screen quads' corner colours to their
 * end values and fades to func_ov007_020b16f0, with fade 1 / 2 when states[0]
 * came from state 6 and 2 / 3 otherwise. */
void func_ov007_020b166c(void) {
    Ov007Work *w = WORK;
    int fromState6 = (w->states[0]->previousState == 6);
    if (w->states[1]->stateTicks != 0)
        return;
    func_ov007_020c1d8c(0);
    FigureScene_SetScreenBlend(0x1000);
    if (fromState6)
        RequestScreenFade(1, 2, func_ov007_020b16f0);
    else
        RequestScreenFade(2, 3, func_ov007_020b16f0);
}
