typedef unsigned short u16;

void func_ov006_0211f34c(char *o, int i)
{
    int m = i * 0x24;
    int xt = *(int *)((char *)(((int)o + 0x4660) & 0xFFFFFFFFFFFFFFFF) + m) >> 12;
    int yt = *(int *)((char *)(((int)o + 0x4664) & 0xFFFFFFFFFFFFFFFF) + m) >> 12;
    if (xt + 8 > 0xfe) {
        u16 *pa = (u16 *)((char *)(((int)o + 0x466c) & 0xFFFFFFFFFFFFFFFF) + m);
        *pa = 0x8000 - *pa;
        *(int *)((char *)(((int)o + 0x4660) & 0xFFFFFFFFFFFFFFFF) + m) = 0xf6000;
    } else if (xt - 8 < 2) {
        u16 *pa = (u16 *)((char *)(((int)o + 0x466c) & 0xFFFFFFFFFFFFFFFF) + m);
        *pa = 0x8000 - *pa;
        *(int *)((char *)(((int)o + 0x4660) & 0xFFFFFFFFFFFFFFFF) + m) = 0xa000;
    }
    if (yt + 8 > 0xbe) {
        u16 *pa = (u16 *)((char *)(((int)o + 0x466c) & 0xFFFFFFFFFFFFFFFF) + m);
        *pa = -*pa;
        *(int *)((char *)(((int)o + 0x4664) & 0xFFFFFFFFFFFFFFFF) + m) = 0xb6000;
    } else if (yt - 8 < 2) {
        u16 *pa = (u16 *)((char *)(((int)o + 0x466c) & 0xFFFFFFFFFFFFFFFF) + m);
        *pa = -*pa;
        *(int *)((char *)(((int)o + 0x4664) & 0xFFFFFFFFFFFFFFFF) + m) = 0xa000;
    }
}
