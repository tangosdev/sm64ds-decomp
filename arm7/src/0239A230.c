extern int *dat_0380FFF4;
extern void fn_0239F108(void);
extern int fn_0239B3A4(void);

int fn_0239A230(void)
{
    unsigned short *q;
    unsigned int result;

    q = (unsigned short *)((int)dat_0380FFF4 + 0x300);
    if (q[0x50] != 0)
        fn_0239F108();
    q = (unsigned short *)((int)dat_0380FFF4 + 0x200);
    result = ((unsigned int)q[0x7F] << 25) >> 31;
    if (result)
        return fn_0239B3A4();
    return result;
}
