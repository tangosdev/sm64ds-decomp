#include "Ov007Sprite.h"
#include "common.h"

/* Draws the sprite, then, when drawFlags & 4, a second black pass scaled
 * by shadowScaleX/Y and shifted by shadowOffset, its alpha lowered by
 * shadowAlphaDrop (medium-confidence "shadow", as Ov007Sprite.h says). */
void func_ov007_020c421c(Ov007Sprite *s) {
    Vector3_16 v;
    int shadowAlpha;
    int scaleY;
    int scaleX;
    v.z = 0;
    v.y = v.z;
    v.x = v.y;
    *((volatile int *)0x4000444) = 0; /* MTX_PUSH */
    func_ov007_020c42f8(s);
    func_ov007_020c4684(s, s->color, (s->alpha >> 12) & 0xff, s->polygonId, s->drawFlags, &v.x);
    if (s->drawFlags & 4) {
        scaleY = s->shadowScaleY;
        scaleX = s->shadowScaleX;
        shadowAlpha = (s->alpha >> 12) - s->shadowAlphaDrop;
        if (shadowAlpha < 0) {
            shadowAlpha = 0;
        }
        *((volatile int *)0x400046c) = scaleX; /* MTX_SCALE */
        *((volatile int *)0x400046c) = scaleY;
        *((volatile int *)0x400046c) = 0x1000;
        func_ov007_020c4684(s, 0, shadowAlpha & 0xff, s->shadowPolygonId, 0, s->shadowOffset);
    }
    *((volatile int *)0x4000448) = 1; /* MTX_POP */
}
