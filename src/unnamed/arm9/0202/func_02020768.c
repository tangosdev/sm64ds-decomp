#include "OamAnimation.h"

extern int func_0202044c(struct OamAnimation *anim, int idx);
extern int func_0202043c(struct OamAnimation *anim);

extern int gFrameStep;

/* Advance an enabled animation by gFrameStep ticks: once elapsed reaches
 * frameDuration, step to the next frame (func_0202044c); past the last one,
 * restart (func_0202043c) -- forever with loops 0, otherwise until loops
 * counts down to 1, where it stops. */
void func_02020768(struct OamAnimation *anim) {
    if (anim->enabled == 0)
        return;

    anim->elapsed += gFrameStep;

    if (anim->elapsed < anim->frameDuration)
        return;

    anim->elapsed = 0;

    if (func_0202044c(anim, anim->frame + 1) != 0)
        return;

    if (anim->loops == 0) {
        func_0202043c(anim);
        return;
    }

    if (anim->loops <= 1) {
        anim->enabled = 0;
        return;
    }

    anim->loops -= 1;
    func_0202043c(anim);
}
