/* storage 0x0238ACB8 runtime 0x03802B50 size 0x54 wram. */

extern void fn_0238AAFC(void *a1);
extern int fn_0238A77C(int a1, int a2, int a3, int a4);

struct Nib {
    unsigned short lo:4;
    short hi:12;
};

int fn_0238ACB8(int a1) {
    unsigned short buf[4];
    int v;

    fn_0238AAFC(buf);
    v = ((struct Nib *)buf)->lo & 0xB;
    if (v != 1)
        return 0;
    fn_0238A77C(6, 16, a1, 1);
    return 1;
}
