/* storage 0x023A26E4 runtime 0x027D465C size 0xA4 main. */

extern void fn_02382CB4(int val, void *dst, int n);
extern void fn_023A17C8(void *a1);
extern int fn_0239EA94(unsigned short *a1);
extern int fn_0239F778(int a1);

int fn_023A26E4(unsigned short *a1) {
    volatile int zero;
    int buf[10];
    int result;

    if ((a1[18] & 1) != 0 || (result = (a1[4] & 0x8000)) != 0) {
        zero = 0;
        fn_02382CB4(zero, buf, 40);
        buf[0] = (int)(a1 + 22);
        ((unsigned short *)buf)[4] = a1[3];
        if ((((unsigned int)*(unsigned short *)(*(char **)0x0380FFF4 + 0x200 + 0xFE) << 30) >> 31) == 0)
            ((unsigned short *)buf)[6] = 2048;
        fn_023A17C8(buf);
        result = ((unsigned short *)buf)[5] & 1;
        if (result == 1) {
            result = fn_0239EA94(a1 + 15);
            if (result != 0)
                result = fn_0239F778(result);
        }
    }
    return result;
}
