extern unsigned short data_ov007_020d7d7c[][4];
extern unsigned short data_ov007_020d7d94[][4];

void func_ov007_020c0eb0(unsigned short *dst, unsigned short *src, int xoff, int yoff,
                         unsigned char prio, int flip, int mosaic, int affineIdx)
{
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

    attr0 = src[0];
    attr1 = src[1];
    tx = attr1 << 7;
    ty = attr0 << 8;
    x = tx >> 7;
    y = ty >> 8;
    size = attr1 >> 14;
    shape = attr0 >> 14;
    attr2 = src[2];

    if (!(attr0 & 0x100) && flip != 0) {
        if ((flip == 0x10000000) | (flip == 0x30000000)) {
            x = -(x + data_ov007_020d7d7c[shape][size]);
            attr1 = (attr1 & ~0x1000) | (~attr1 & 0x1000);
        }
        if ((flip == 0x20000000) | (flip == 0x30000000)) {
            y = -(y + data_ov007_020d7d94[shape][size]);
            attr1 = (attr1 & ~0x2000) | (~attr1 & 0x2000);
        }
    }

    dst[0] = attr0;
    dst[1] = attr1;
    dst[2] = attr2;

    if (affineIdx >= 0) {
        *(unsigned int *)dst = (*(unsigned int *)dst & 0xc1fffcff) | 0x100 | (affineIdx << 25);
    }
    sy = y + yoff;
    sx = x + xoff;
    *(unsigned int *)dst = (*(unsigned int *)dst & 0xfe00ff00) | (sy & 0xff) | ((sx & 0x1ff) << 16);
    dst[2] = (dst[2] & ~0xc00) | (prio << 10);
    if (mosaic) {
        *(unsigned int *)dst |= 0x1000;
    } else {
        *(unsigned int *)dst &= ~0x1000;
    }
}
