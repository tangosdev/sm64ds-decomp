extern int fn_0238AAFC(int a1);
extern int fn_0238A77C(int a1, int a2, int a3, int a4);

int fn_0238AC18(int a1) {
    unsigned short buf[4];
    unsigned int w;
    fn_0238AAFC((int)buf);
    w = (unsigned int)buf[0] << 28;
    w = w >> 28;
    if (w != 4)
        return 0;
    fn_0238A77C(6, 0x10, a1, 3);
    return 1;
}
