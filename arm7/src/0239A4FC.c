/* storage 0x0239A4FC runtime 0x027CC474 linked 0x154 main ARM.
 * Channel-event poll: walk the 28-byte entry table at *(*dat+0x2E0), count
 * down per-entry timers, and fire fn_0239F9AC / fn_0239B6F0 / fn_023969F4 /
 * fn_02396A98 / fn_0239A98C as entries expire past 0x20.
 * Pool +0x148: dat_0380FFF4, 0xFFFF, 0x4E8. */
extern int dat_0380FFF4;
extern void fn_0239F9AC(int);
extern void fn_0239B6F0(void *, int);
extern void fn_023969F4(int);
extern void fn_02396A98(void);
extern int fn_0239A98C(int, int);

void fn_0239A4FC(void)
{
    unsigned short *v1;
    unsigned int i;
    unsigned int v4;
    unsigned int v3;
    int *w;
    int c32;
    int one;
    int zero;
    int si;
    w = &dat_0380FFF4;
    v1 = *(unsigned short **)(*w + 0x2E0) + 14;
    {
        unsigned int t = (unsigned int)*w + 0x400;
        v3 = *(unsigned short *)(t + 0xE8);
        if (v3 <= 1)
            return;
    }
    v4 = 0;
    i = 1;
    c32 = 32;
    one = (int)i;
    zero = (int)v4;
    si = (int)i;
    while (i < *(unsigned short *)(*(unsigned char **)w + 0x2E6)) {
        if (*v1 != 0) {
            if (v1[12] != 0 && v1[12] != 0xFFFF) {
                v1[12]--;
                if (v1[12] == 0) {
                    if (*v1 >= 0x20) {
                        fn_0239F9AC((int)i);
                        {
                            if (*(unsigned short *)(*(unsigned char **)w + 0x314) == 1) {
                                if (*v1 > 0x20)
                                    fn_0239B6F0((void *)(v1 + 2), one);
                            } else if (i == *(unsigned short *)(*(unsigned char **)w + 0x384)) {
                                fn_023969F4(c32);
                                fn_02396A98();
                                fn_0239B6F0((void *)(v1 + 2), si);
                            }
                        }
                        fn_0239A98C((unsigned short)i, c32);
                    }
                    *v1 = (unsigned short)zero;
                    {
                        int b = *w;
                        int nv = *(unsigned short *)(b + 0x4E8) - 1;
                        *(unsigned short *)(b + 0x4E8) = (unsigned short)nv;
                    }
                }
            }
            v4++;
            if (v4 >= v3)
                break;
        }
        i++;
        v1 += 14;
    }
}
