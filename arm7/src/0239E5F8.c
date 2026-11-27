#pragma scheduling off
extern void fn_02385354(int dst, int value);
extern int fn_02385334(unsigned char *p);
int fn_0239E5F8(int a1) {
    unsigned char *base;
    unsigned short hw;
    int n;
    unsigned int i;
    unsigned char *p;
    n = 0;
    fn_02385354(a1, 0xDD);
    fn_02385354(a1 + 1, (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x39C) + 8) & 0xFF);
    fn_02385354(a1 + 2, n);
    fn_02385354(a1 + 3, 9);
    fn_02385354(a1 + 4, 0xBF);
    fn_02385354(a1 + 5, n);
    fn_02385354(a1 + 6, *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x300) & 0xFF);
    fn_02385354(a1 + 7, (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x300) >> 8) & 0xFF);
    hw = *(unsigned short *)0x0380FFF0;
    fn_02385354(a1 + 8, hw & 0xFF);
    fn_02385354(a1 + 9, ((unsigned)hw >> 8) & 0xFF);
    n = n + 10;
    base = *(unsigned char **)0x0380FFF4;
    if (*(unsigned short *)(base + 0x39C) != 0) {
        p = *(unsigned char **)(base + 0x398);
        if (*(unsigned short *)(base + 0x39E) & 1)
            p = p + 1;
        i = 0;
        while (i < *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x39C)) {
            fn_02385354(a1 + n, fn_02385334(p));
            n = n + 1;
            p = p + 1;
            i = i + 1;
        }
    }
    return n;
}
