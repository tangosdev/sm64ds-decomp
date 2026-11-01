/* storage 0x0239F228 runtime 0x027D11A0 size 0x43C main. */

extern unsigned int fn_023963F0(void);
extern void fn_02396958(char *dst, char *src, char *a, char *b);
extern void fn_02395354(char *addr, unsigned int val);
extern unsigned int fn_02395334(char *addr);
extern unsigned int fn_0239E784(char *p);

void fn_0239F228(void) {
    char **pp;
    char *base;
    char *hdr;
    char *body;
    char *cur;
    char *at;
    unsigned int zero;
    unsigned int bit;
    unsigned int i;
    char *src;
    unsigned int count;
    unsigned int j;
    char *end;
    unsigned short hw;
    unsigned short *pair;

    pp = (char **)0x0380FFF4;
    base = *pp;
    hdr = *(char **)(base + 0x464);
    *(unsigned short *)hdr = 0;
    *(unsigned short *)(hdr + 2) = 0;
    *(unsigned short *)(hdr + 4) = 0;
    *(unsigned short *)(hdr + 6) = 0;
    *(unsigned short *)(hdr + 8) = (unsigned short)fn_023963F0();
    *(unsigned short *)(hdr + 0xC) = 0x80;
    *(unsigned short *)(hdr + 0xE) = 0;
    base = *pp;
    fn_02396958(hdr + 0x10, (char *)0x027D64F4, base + 0x2E8, base + 0x2E8);
    zero = 0;
    *(unsigned short *)(hdr + 0x22) = (unsigned short)zero;
    body = hdr + 0x24;
    *(unsigned int *)body = zero;
    *(unsigned int *)(body + 4) = zero;
    *(unsigned short *)(body + 8) = *(unsigned short *)(*pp + 0x300 + 0x6A);
    *(unsigned short *)(body + 0xA) = *(unsigned short *)(*pp + 0x300 + 0x78);
    cur = body + 0xC;
    base = *pp;
    bit = (unsigned int)(*(unsigned short *)(base + 0x200 + 0xFE) << 31);
    bit >>= 31;
    if (bit == 0) {
        *(unsigned short *)(base + 0x300 + 0x8E) = (unsigned short)(cur - body);
        fn_02395354(cur, zero);
        at = cur + 1;
        cur = cur + 2;
        fn_02395354(at, *(unsigned short *)(*pp + 0x300 + 0x1A) & 0xFF);
        i = 0;
        while (i < *(unsigned short *)(*pp + 0x300 + 0x1A)) {
            base = *pp;
            fn_02395354(cur, fn_02395334(base + 0x31C + i));
            cur = cur + 1;
            i = i + 1;
        }
        fn_02395354(cur + (~0u - i), i & 0xFF);
    } else {
        *(unsigned short *)(base + 0x300 + 0x8E) = (unsigned short)zero;
    }
    cur = cur + fn_0239E784(cur);
    fn_02395354(cur, 3);
    fn_02395354(cur + 1, 1);
    fn_02395354(cur + 2, *(unsigned short *)(*pp + 0x300 + 0x76) & 0xFF);
    at = cur + 3;
    *(unsigned short *)(*pp + 0x300 + 0x90) = (unsigned short)(at - body);
    *(unsigned short *)0x04808084 = (unsigned short)(*(unsigned short *)(*pp + 0x300 + 0x90) + 2);
    fn_02395354(at, 5);
    fn_02395354(cur + 4, 5);
    fn_02395354(cur + 5, 0);
    fn_02395354(cur + 6, *(unsigned short *)(*pp + 0x300 + 0x70) & 0xFF);
    fn_02395354(cur + 7, 0);
    fn_02395354(cur + 8, 0);
    fn_02395354(cur + 9, 0);
    at = cur + 0xA;
    *(unsigned short *)(*pp + 0x300 + 0x92) = (unsigned short)(at - body);
    *(unsigned short *)(*pp + 0x300 + 0x9E) =
        (unsigned short)(*(unsigned short *)(*pp + 0x300 + 0x92) & 1);
    fn_02395354(at, 0xDD);
    fn_02395354(cur + 0xB, (*(unsigned short *)(*pp + 0x300 + 0x9C) + 8) & 0xFF);
    fn_02395354(cur + 0xC, 0);
    fn_02395354(cur + 0xD, 9);
    fn_02395354(cur + 0xE, 0xBF);
    fn_02395354(cur + 0xF, 0);
    base = *pp;
    bit = (unsigned int)(*(unsigned short *)(base + 0x300 + 0x16) << 31);
    bit >>= 31;
    if (bit == 1) {
        fn_02395354(cur + 0x10, *(unsigned short *)(base + 0x300) & 0xFF);
        at = cur + 0x11;
        cur = cur + 0x12;
        fn_02395354(at, ((int)*(unsigned short *)(*pp + 0x300) >> 8) & 0xFF);
    } else {
        fn_02395354(cur + 0x10, 0xFF);
        at = cur + 0x11;
        cur = cur + 0x12;
        fn_02395354(at, 0xFF);
    }
    hw = *(unsigned short *)0x0380FFF0;
    fn_02395354(cur, hw & 0xFF);
    at = cur + 1;
    cur = cur + 2;
    fn_02395354(at, ((unsigned int)hw >> 8) & 0xFF);
    src = *(char **)(*pp + 0x398);
    i = 0;
    while (i < *(unsigned short *)(*pp + 0x300 + 0x9C)) {
        fn_02395354(cur, fn_02395334(src));
        cur = cur + 1;
        src = src + 1;
        i = i + 1;
    }
    base = *pp;
    count = *(unsigned short *)(base + 0x300 + 0x9C);
    if (*(unsigned short *)(base + 0x300 + 0x9E) != 0) {
        end = *(char **)(base + 0x398) + count;
        end = end - 1;
        j = 0;
        while (j < *(unsigned short *)(*pp + 0x300 + 0x9C)) {
            fn_02395354(end + 1, fn_02395334(end));
            j = j + 1;
            end = end - 1;
        }
    }
    if ((*(unsigned short *)(*pp + 0x600 + 0x1C) & 4) != 0) {
        pair = (unsigned short *)((unsigned int)(cur + 3) & ~3u);
        pair[0] = 0xB6B8;
        pair[1] = 0x1D46;
    }
    *(unsigned short *)(*pp + 0x300 + 0xA0) = 0;
    *(unsigned short *)(hdr + 0xA) = (unsigned short)((cur + 0x1C) - body);
}
