#include "Ov007Model.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c9268(Ov007AnimPlayer *p);

void *func_ov007_020c92ac(void) {
    void *c = func_ov007_020c3df4(0, 0x1c);
    func_ov007_020c9268(c);
    return c;
}

/* Long-branch veneer: long_calls emits the pooled `ldr ip,[pc]; bx ip`
 * absolute tail-call; bracketed because the pragma is positional. */
#pragma long_calls on
void func_ov007_020c92a0(void *p) {
    func_ov007_020c3d1c(p);
}
#pragma long_calls off

/* The 0x1c-byte animation player func_ov007_020c92ac allocates and this
 * function resets. state, anim, frame and frameCount are named as in
 * func_ov007_020bc02c; the rest from func_ov007_020c9214 (which starts an
 * animation) and func_ov007_020c9110 (which ticks one). */
/* Resets a player: stopped (state 2), one-shot, no animation, at frame 0.
 */
void func_ov007_020c9268(Ov007AnimPlayer *p) {
    p->state = 2;
    p->loopMode = 0;
    p->flags = 1;
    p->anim = -1;
    p->prevAnim = -1;
    p->frame = 0;
    p->frameCount = 0;
    p->framePos = 0;
    p->speed = 0;
}
