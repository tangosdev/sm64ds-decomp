/* storage 0x023A0A94 runtime 0x027D2A0C linked 0x1DC main ARM. */

extern void fn_02382CB4(int zero, void *dst, int n);
extern int fn_0238D594(unsigned int val, int mode);

void fn_023A0A94(void) {
    volatile int zero = 0;
    int **pp;
    volatile unsigned short *out;
    unsigned short mode;
    unsigned short half;

    fn_02382CB4(zero, (char *)*(int **)0x0380FFF4 + 0x498, 0x50);
    *(unsigned short *)0x04808030 = 0x8000;
    pp = (int **)0x0380FFF4;
    mode = *(unsigned short *)((char *)*pp + 0x314);
    switch (mode) {
    case 0:
        *(unsigned short *)0x04808050 = 0x4794;
        *(unsigned short *)0x04808056 = 0x3CA;
        *(unsigned short *)((char *)*pp + 0x396) = 0x17CC;
        break;
    case 1:
        *(unsigned short *)0x04808050 = 0x50C4;
        *(unsigned short *)0x04808056 = 0x862;
        *(unsigned short *)((char *)*pp + 0x396) = 0xE9C;
        break;
    case 2:
        *(unsigned short *)0x04808050 = 0x4BFC;
        *(unsigned short *)0x04808056 = 0x5FE;
        *(unsigned short *)((char *)*pp + 0x396) = 0x1364;
        break;
    case 3:
        *(unsigned short *)0x04808050 = 0x4794;
        *(unsigned short *)0x04808056 = 0x3CA;
        *(unsigned short *)((char *)*pp + 0x396) = 0x17CC;
        break;
    default:
        break;
    }
    *(unsigned short *)0x04808052 = 0x5F60;
    out = (volatile unsigned short *)0x0480805A;
    *out = fn_0238D594(*(unsigned short *)0x04808050 & 0x3FFF, 2);
    half = *out;
    pp = (int **)0x0380FFF4;
    *(unsigned short *)((char *)*pp + 0x49C) = half;
    *(unsigned short *)0x04808062 = 0x5F5E;
    *(unsigned short *)0x04808030 = 0x8001;
    *(unsigned short *)((char *)*pp + 0x498) = 0xFFFF;
}
