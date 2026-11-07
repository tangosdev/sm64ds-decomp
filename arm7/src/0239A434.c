/* storage 0x0239A434 runtime 0x027CC3AC linked 0xC8 main ARM.
 * Channel-work init: clear table, invalidate entries with 0xFFFF, run inits. */
extern int *dat_0380FFF4;
extern void fn_02382C80(int, void *, int);
extern int fn_0239A284(int, void *);
extern int fn_0239A98C(int, int);
extern unsigned short unk_27D680C[];
int fn_0239A434(void) {
    char *r4 = *(char **)((char *)dat_0380FFF4 + 0x2E0);
    unsigned short count = *(unsigned short *)((char *)dat_0380FFF4 + 0x2E6);
    volatile unsigned short z1 = 0;
    fn_02382C80(z1, r4, count * 28);
    {
        volatile unsigned short z2 = 0;
        fn_02382C80(z2, (char *)dat_0380FFF4 + 0x4E8, 12);
    }
    *(unsigned short *)(r4 + 0x1A) = 0xFFFF;
    {
        unsigned int ip = 1;
        while (ip < *(unsigned short *)((char *)dat_0380FFF4 + 0x2E6))
        {
            *(unsigned short *)(r4 + ip * 28 + 0x1A) = 0xFFFF;
            ip++;
        }
    }
    fn_0239A284(0, unk_27D680C);
    return fn_0239A98C(0, 0x40);
}
