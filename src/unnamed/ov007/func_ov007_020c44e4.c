#include "types.h"
// @symbol func_ov007_020c44e4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))

#include "Ov007Sprite.h"

int func_ov007_020c44e4(int count)
{
    Ov007Sprite *p = (Ov007Sprite *)func_ov007_020c3df4(0, 0x88);
    int i;
    u16 zs[3];

    p->frame = 0;
    p->frameCount = (s16)count;
    p->frames = (void **)func_ov007_020c3df4(0, count << 2);
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
        u16 *zw = (u16 *)AT(zs, 0);
        zw[0] = 0;
        zw[1] = 0;
        zw[2] = 0;
    }
    {
        u16 a = ((volatile u16 *)zs)[0];
        u16 b = ((volatile u16 *)zs)[1];
        p->shadowOffset[0] = a;
        p->shadowOffset[1] = b;
        p->shadowOffset[2] = zs[2];
    }
    p->tilemap = 0;
    return (int)p;
}
