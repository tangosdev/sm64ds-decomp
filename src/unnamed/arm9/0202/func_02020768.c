extern int func_0202044c(char *thiz, int idx);
extern int func_0202043c(void *thiz);
#include "OamAnimation.h"

extern int data_0208ee44;

/* Advance an enabled animation by data_0208ee44 ticks: once elapsed reaches
 * frameDuration, step to the next frame (func_0202044c); past the last one,
 * restart (func_0202043c) -- forever with loops 0, otherwise until loops
 * counts down to 1, where it stops. */
void func_02020768(char *thiz)
{
    struct OamAnimation *anim = (struct OamAnimation *)thiz;

    if (anim->enabled == 0)
        return;

    anim->elapsed += data_0208ee44;

    if (anim->elapsed < anim->frameDuration)
        return;

    anim->elapsed = 0;

    if (func_0202044c(thiz, anim->frame + 1) != 0)
        return;

    if (anim->loops == 0) {
        func_0202043c(thiz);
        return;
    }

    if (anim->loops <= 1) {
        anim->enabled = 0;
        return;
    }

    anim->loops -= 1;
    func_0202043c(thiz);
}
