//cpp
#include "nitro/gx/gxcommon.h"
#include "Ov007SpriteBank.h"
enum { FLIP_HORIZONTAL = 0x10000000, FLIP_VERTICAL = 0x20000000, FLIP_BOTH = 0x30000000 };
// Keep mode membership integer-valued; mwcc otherwise shares C++ bool tests.
static inline int HasOamFlip(int mode, int axis) {
    int singleAxis = mode == axis ? 1 : 0;
    int bothAxes = mode == FLIP_BOTH ? 1 : 0;
    return singleAxis | bothAxes;
}
// Retain the configured address-named export while its callers are reconstructed.
extern "C" {
extern unsigned short data_ov007_020d7d7c[][4];
extern unsigned short data_ov007_020d7d94[][4];

void func_ov007_020c0eb0(GXOamAttr *dst, const Ov007CellEntry *src, int xoff, int yoff,
                         unsigned char prio, int flip, int mosaic, int affineIdx) {
    unsigned short attr0;
    unsigned short attr1;
    unsigned short attr2;
    unsigned short ty;
    unsigned short tx;
    int x;
    int y;
    int size;
    int shape;
    int sy;
    int sx;

    attr0 = src->attr0;
    attr1 = src->attr1;
    tx = attr1 << 7;
    ty = attr0 << 8;
    x = tx >> 7;
    y = ty >> 8;
    size = attr1 >> 14;
    shape = attr0 >> 14;
    attr2 = src->attr2;

    if (!(attr0 & 0x100) && flip != 0) {
        if (HasOamFlip(flip, FLIP_HORIZONTAL)) {
            x = -(x + data_ov007_020d7d7c[shape][size]);
            attr1 = (attr1 & ~0x1000) | (~attr1 & 0x1000);
        }
        if (HasOamFlip(flip, FLIP_VERTICAL)) {
            y = -(y + data_ov007_020d7d94[shape][size]);
            attr1 = (attr1 & ~0x2000) | (~attr1 & 0x2000);
        }
    }

    dst->attr0 = attr0;
    dst->attr1 = attr1;
    dst->attr2 = attr2;

    if (affineIdx >= 0) {
        dst->attr01 = (dst->attr01 & 0xc1fffcff) | 0x100 | (affineIdx << 25);
    }
    sy = y + yoff;
    sx = x + xoff;
    dst->attr01 = (dst->attr01 & 0xfe00ff00) | (sy & 0xff) | ((sx & 0x1ff) << 16);
    dst->attr2 = (dst->attr2 & ~0xc00) | (prio << 10);
    if (mosaic) {
        dst->attr01 |= 0x1000;
    } else {
        dst->attr01 &= ~0x1000;
    }
}
}
