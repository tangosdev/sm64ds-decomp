#include "types.h"
#include "decl_common.h"
#include "Ov007Sprite.h"

Ov007Sprite *func_ov007_020c44e4(int count) {
    Ov007Sprite *p = func_ov007_020c3df4(0, 0x88);
    int i;

    p->frame = 0;
    p->frameCount = (s16)count;
    p->frames = func_ov007_020c3df4(0, count << 2);
    for (i = 0; i < count; i++) {
        p->frames[i] = 0;
    }

    p->drawFlags = 0;
    p->repeatV = 1;
    p->repeatU = p->repeatV;
    p->screenY = 0;
    p->screenX = p->screenY;
    p->height = 0;
    p->width = p->height;

    {
        struct Vector3 z3 = {0, 0, 0};
        p->pos = z3;

        p->worldHeight = 0;
        p->worldWidth = p->worldHeight;
        p->scaleY = 0x1000;
        p->scaleX = p->scaleY;
        p->rotZ = 0;
        p->rotY = p->rotZ;
        p->rotX = p->rotY;

        {
            struct Vector3 z3b = {0, 0, 0};
            p->posOffset = z3b;
        }
    }

    p->cornerColors[3] = 0x7fff;
    p->cornerColors[2] = p->cornerColors[3];
    p->cornerColors[1] = p->cornerColors[2];
    p->cornerColors[0] = p->cornerColors[1];
    p->color = 0x7fff;
    p->polygonId = 0x23;
    p->alpha = 0x1f000;
    p->texRepeat = 3;
    p->texFlip = 0;
    p->texColor0 = 0;
    p->texT = 0;
    p->texS = p->texT;
    p->polygonAttr = 0;
    p->shadowScaleY = 0x1000;
    p->shadowScaleX = p->shadowScaleY;
    p->shadowAlphaDrop = 0x19;
    p->shadowPolygonId = 0x24;

    {
        VecFx16 zero = {0, 0, 0};
        p->shadowOffset = zero;
    }
    p->tilemap = 0;
    return p;
}
