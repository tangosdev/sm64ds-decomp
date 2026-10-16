/* storage 0x0239F108 runtime 0x027D1080 size 0x120 main. */

extern void fn_02395400(char *dst, char *src, unsigned int count);
extern void fn_02395354(unsigned int addr, unsigned int val);

void fn_0239F108(void) {
    char **pp;
    char *base;
    char *cursor;
    char *dst;
    unsigned int count;
    unsigned short *pair;
    unsigned int sum;

    pp = (char **)0x0380FFF4;
    base = *pp;
    cursor = *(char **)(base + 0x464) + 0x24 + *(unsigned short *)(base + 0x300 + 0x92);
    count = *(unsigned short *)(base + 0x300 + 0x9C);
    if (count != 0) {
        if ((*(unsigned short *)(base + 0x300 + 0x9E) & 1) != 0) {
            dst = cursor + 10;
            dst = dst - 1;
            fn_02395400(dst, *(char **)(base + 0x398), count + 2);
            fn_02395354((unsigned int)(cursor + 9),
                        ((int)*(unsigned short *)0x0380FFF0 >> 8) & 0xFF);
        } else {
            fn_02395400(cursor + 10, *(char **)(base + 0x398), count + 1);
        }
    }
    base = *pp;
    sum = *(unsigned short *)(base + 0x300 + 0x92) + 0x26;
    sum = sum + *(unsigned short *)(base + 0x300 + 0x9C);
    *(unsigned short *)(*(char **)(base + 0x464) + 0xA) = (unsigned short)sum;
    fn_02395354((unsigned int)(cursor + 1),
                (*(unsigned short *)(*pp + 0x300 + 0x9C) + 8) & 0xFF);
    base = *pp;
    if ((*(unsigned short *)(base + 0x600 + 0x1C) & 4) != 0) {
        pair = (unsigned short *)(((unsigned int)(cursor + 0xD)
                                   + *(unsigned short *)(base + 0x300 + 0x9C))
                                  & ~3u);
        pair[0] = 0xB6B8;
        pair[1] = 0x1D46;
    }
    *(unsigned short *)(*pp + 0x300 + 0xA0) = 0;
}
