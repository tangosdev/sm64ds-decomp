extern void fn_02382C98(void *src, void *dst, int n);
extern unsigned short *fn_02390214(unsigned short *msg, void *src, unsigned short a3);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

void fn_02393E20(unsigned char *a1) {
    unsigned char local[8];
    unsigned short buf[0x100];
    fn_02382C98(a1 + 4, local, 6);
    unsigned short *ret = fn_02390214(buf, local, 0x13);
    unsigned short *msg = fn_0238DE4C();
    msg[0] = 0x22;
    if (ret[2] == 0) {
        msg[1] = 0;
    } else {
        msg[1] = 1;
        msg[2] = 5;
        msg[3] = ret[2];
    }
    fn_0238DEF4(msg);
}
