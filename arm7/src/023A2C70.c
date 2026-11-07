/* storage 0x023A2C70 runtime 0x027D4BE8 size 0x1B8 main. */

extern int fn_0239E91C(short *a1, int a2);
extern int fn_0239F778(int a1);
extern void fn_0239A98C(int a1, int a2);
extern void fn_0239B488(short *a1, unsigned short a2);
extern void fn_02382CB4(int val, void *dst, int n);
extern void fn_023A17C8(void *a1);
extern int fn_0239EDE0(int a1, int a2, unsigned short *a3);

int fn_023A2C70(unsigned short *a1) {
    volatile int zero;
    int buf[10];
    unsigned short *v3;
    unsigned int v2;
    int v4;
    int v5;
    unsigned int v6;
    int v8;
    int result;

    v2 = a1[3];
    v3 = a1 + 22;
    if (v2 > 4) {
        result = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x14);
        if (result == 1) {
            v4 = a1[1];
            if (v4 != 0) {
                v5 = 28 * v4;
                v6 = *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 736) + v5);
                if (v6 >= 0x30)
                    goto big;
            }
            result = fn_0239E91C((short *)(a1 + 15), 6);
            if (result != 0)
                result = fn_0239F778(result);
            return result;
        big:
            if (v6 == 64) {
                fn_0239A98C(v4, 0x30);
                fn_0239B488((short *)(a1 + 15), 1);
            }
            zero = 0;
            fn_02382CB4(zero, buf, 40);
            buf[0] = (int)(v3 + 2);
            ((unsigned short *)buf)[4] = (unsigned short)(v2 - 4);
            fn_023A17C8(buf);
            result = *v3;
            if ((result & 0xFFC2) != 0
                || (((unsigned int)(result << 31) >> 31) == 0)
                || (*(unsigned short *)(*(char **)0x0380FFF4 + 0x200 + 0xF8) == 0
                    && (((unsigned int)result << 27) >> 31) == 1)
                || (*(unsigned short *)(*(char **)0x0380FFF4 + 0x200 + 0xF8) != 0
                    && (((unsigned int)*(volatile unsigned short *)v3 << 27) >> 31) == 0)
                || ((((unsigned int)*(unsigned short *)(*(char **)0x0380FFF4 + 0x200 + 0xFE) << 29) >> 31) == 1
                    && (((unsigned int)*(volatile unsigned short *)v3 << 26) >> 31) == 0)) {
                v8 = 10;
            } else {
                *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 736) + v5 + 12) = (unsigned short)result;
                if ((((unsigned short *)buf)[5] & 1) == 0) {
                    v8 = 1;
                } else if ((((unsigned short *)buf)[5] & 4) == 0) {
                    v8 = 18;
                } else {
                    *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 736) + v5 + 16) = ((unsigned short *)buf)[11];
                    v8 = 0;
                }
            }
            result = fn_0239EDE0(v4, v8, (unsigned short *)buf[6]);
            if (result != 0)
                result = fn_0239F778(result);
        }
    }
    return result;
}
