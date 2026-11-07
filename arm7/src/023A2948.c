/* storage 0x023A2948 runtime 0x027D48C0 size 0x184 main. */

extern int fn_0239E91C(short *a1, int a2);
extern int fn_0239F778(int a1);
extern void fn_0239A98C(int a1, int a2);
extern int fn_0239B488(short *a1, unsigned short a2);
extern void fn_02382CB4(int val, void *dst, int n);
extern void fn_023A17C8(void *a1);
extern int fn_0239EC48(int a1, int a2, int a3);

int fn_023A2948(unsigned short *a1) {
    volatile int zero;
    int buf[10];
    unsigned short *v3;
    unsigned int v2;
    int v4;
    int v5;
    unsigned int v6;
    int v7;
    int v8;
    int result;

    v2 = a1[3];
    v3 = a1 + 22;
    if (v2 > 10) {
        result = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x14);
        if (result == 1) {
            v4 = a1[1];
            v5 = 28 * v4;
            v6 = *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 736) + 28 * v4);
            if (v6 < 0x30) {
                result = fn_0239E91C((short *)(a1 + 15), 6);
                if (result != 0)
                    result = fn_0239F778(result);
            } else {
                if (v6 == 64) {
                    fn_0239A98C(v4, 0x30);
                    fn_0239B488((short *)(a1 + 15), 1);
                }
                zero = 0;
                fn_02382CB4(zero, buf, 40);
                buf[0] = (int)(v3 + 5);
                ((unsigned short *)buf)[4] = (unsigned short)(v2 - 10);
                ((unsigned short *)buf)[6] = 2048;
                fn_023A17C8(buf);
                v7 = *v3;
                if ((v7 & 0xFFC2) != 0
                    || (*(unsigned short *)(*(char **)0x0380FFF4 + 0x200 + 0xF8) == 0
                        && (((unsigned int)v7 << 27) >> 31) == 1)
                    || (*(unsigned short *)(*(char **)0x0380FFF4 + 0x200 + 0xF8) != 0
                        && ((((unsigned int)*(volatile unsigned short *)v3 << 27) >> 31) == 0))) {
                    v8 = 10;
                } else {
                    *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 736) + v5 + 12) = (unsigned short)v7;
                    if ((((unsigned short *)buf)[5] & 1) == 0) {
                        v8 = 1;
                    } else if ((((unsigned short *)buf)[5] & 4) == 0) {
                        v8 = 18;
                    } else {
                        *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 736) + v5 + 16) = ((unsigned short *)buf)[11];
                        v8 = 0;
                    }
                }
                result = fn_0239EC48(v4, v8, buf[6]);
                if (result != 0)
                    result = fn_0239F778(result);
            }
        }
    }
    return result;
}
