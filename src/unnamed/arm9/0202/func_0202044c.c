#include "OamAnimation.h"

/* Show frame idx: load its attribute list and duration and restart its
 * tick count. Returns 0 (and changes nothing) past the last frame. */
int func_0202044c(char *this, int idx) {
    struct OamAnimation *anim = (struct OamAnimation *)this;

    if (idx >= anim->frameCount)
        return 0;
    anim->frame = idx;
    anim->tile = anim->frames[idx].tile;
    anim->frameDuration = anim->frames[idx].duration;
    anim->elapsed = 0;
    return 1;
}
