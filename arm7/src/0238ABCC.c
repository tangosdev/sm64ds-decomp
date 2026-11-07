extern int fn_0238AAFC(int a1);
extern int fn_0238A77C(int a1, int a2, int a3, int a4);

int fn_0238ABCC(int a1) {
    unsigned short buf[4];
    unsigned int w;
    fn_0238AAFC((int)buf);
    w = buf[0];
    w <<= 25;
    w >>= 31;
    if (w == 0)
        return 0;
    fn_0238A77C(0x86, 0x50, a1, 3);
    return 1;
}
