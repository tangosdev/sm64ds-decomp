typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern short data_02082214[];

#define B ((char *)c + i * 0x40 + 0x4000)

#pragma opt_common_subs off

void func_ov006_020d7c4c(char *c, int i)
{
    int t;
    int x, y;

    t = data_02082214[(*(u16 *)(B + 0x68c) >> 4) * 2 + 1];
    *(int *)(c + 0x4660 + (i << 6)) +=
        (int)(((long long)t * *(int *)(B + 0x670) + 0x800) >> 12);
    t = data_02082214[(*(u16 *)(B + 0x68c) >> 4) * 2];
    *(int *)(c + 0x4664 + (i << 6)) +=
        (int)(((long long)t * *(int *)(B + 0x670) + 0x800) >> 12);

    x = *(int *)(B + 0x660) >> 12;
    y = *(int *)(B + 0x664) >> 12;

    if (*(u8 *)(B + 0x696) != 0) {
        if (x + 12 > 0x100) {
            *(u16 *)(B + 0x68c) = 0x8000 - *(u16 *)(B + 0x68c);
            *(int *)(B + 0x660) = 0xf4000;
        } else if (x - 12 < 0xc0) {
            *(u16 *)(B + 0x68c) = 0x8000 - *(u16 *)(B + 0x68c);
            *(int *)(B + 0x660) = 0xcc000;
        }
        if (y + 12 > 0x80) {
            *(u16 *)(B + 0x68c) = -*(u16 *)(B + 0x68c);
            *(int *)(B + 0x664) = 0x74000;
        } else if (y - 12 < 0x40) {
            *(u16 *)(B + 0x68c) = -*(u16 *)(B + 0x68c);
            *(int *)(B + 0x664) = 0x4c000;
        }
    } else {
        if (x + 12 > 0x40) {
            *(u16 *)(B + 0x68c) = 0x8000 - *(u16 *)(B + 0x68c);
            *(int *)(B + 0x660) = 0x34000;
        } else if (x - 12 < 0) {
            *(u16 *)(B + 0x68c) = 0x8000 - *(u16 *)(B + 0x68c);
            *(int *)(B + 0x660) = 0xc000;
        }
        if (y + 12 > 0x80) {
            *(u16 *)(B + 0x68c) = -*(u16 *)(B + 0x68c);
            *(int *)(B + 0x664) = 0x74000;
        } else if (y - 12 < 0x40) {
            *(u16 *)(B + 0x68c) = -*(u16 *)(B + 0x68c);
            *(int *)(B + 0x664) = 0x4c000;
        }
    }
}
