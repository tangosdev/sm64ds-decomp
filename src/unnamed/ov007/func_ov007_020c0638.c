#include "Ov007BgLayer.h"
#include "nitro/mi/memory.h"

/* Rebuilds the layer's screen scrolled to pixel (x, y): fills it with tile,
 * copies the overlapping part of source in at the tile offset and keeps the
 * sub-tile remainder as the fine scroll. */
void func_ov007_020c0638(Ov007BgLayer *self, int x, int y, u16 tile) {
    int ty;
    char *dst;
    char *src;
    int sy;
    int sh;
    int tx;
    int rx;
    int ry;
    int sw;
    int sx;
    volatile u16 v;

    tx = x >> 3;
    ty = y >> 3;
    rx = x - (tx << 3);
    ry = y - (ty << 3);
    sw = self->width;
    sh = self->height;
    sx = 0;
    sy = 0;
    dst = self->screen;
    src = self->source;
    if (tx < 0) {
        sw += tx;
        sx = -tx;
        tx = 0;
    } else if (tx > 0) {
        sw -= tx;
    }
    if (ty < 0) {
        sh += ty;
        sy = -ty;
        ty = 0;
    } else if (ty > 0) {
        sh -= ty;
    }
    v = tile;
    MIi_CpuClear16(v, dst, self->width * self->height * 2);
    self->fineScrollX = -rx << 12;
    self->fineScrollY = -ry << 12;
    if (sw > 0 && sh > 0) {
        for (rx = ty + sh; ty < rx; ty++, sy++) {
            MIi_CpuCopy16(src + sy * self->width * 2 + sx * 2, dst + ty * self->width * 2 + tx * 2,
                          sw * 2);
        }
    }
    self->dirty = 1;
}
