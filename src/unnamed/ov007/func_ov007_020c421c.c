#include "Ov007Sprite.h"
#include "common.h"
#include "nitro/gx/g3imm_cmd.h"

/* Draws the sprite, then, when drawFlags & 4, a second black pass scaled
 * by shadowScaleX/Y and shifted by shadowOffset, its alpha lowered by
 * shadowAlphaDrop (medium-confidence "shadow", as Ov007Sprite.h says). */
void func_ov007_020c421c(Ov007Sprite *s) {
    Vector3_16 v;
    int shadowAlpha;
    v.x = v.y = v.z = 0;
    G3_PushMtx();
    func_ov007_020c42f8(s);
    func_ov007_020c4684(s, s->color, (s->alpha >> 12) & 0xff, s->polygonId, s->drawFlags, &v.x);
    if (s->drawFlags & 4) {
        shadowAlpha = (s->alpha >> 12) - s->shadowAlphaDrop;
        if (shadowAlpha < 0) {
            shadowAlpha = 0;
        }
        G3_Scale(s->shadowScaleX, s->shadowScaleY, 0x1000);
        func_ov007_020c4684(s, 0, shadowAlpha & 0xff, s->shadowPolygonId, 0, &s->shadowOffset.x);
    }
    G3_PopMtx(1);
}
