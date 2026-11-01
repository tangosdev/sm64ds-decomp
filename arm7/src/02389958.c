/* storage 0x02389958 runtime 0x038017F0 size 0x88 + 8 pool wram ARM.
 * Two constant pools (0x040001C0, 0x040001C2), BLs to the in-pack thunk
 * (arg 1) and fn_023899E8. Returns the 0x40001C2 address (R0 at exit).
 * First store via a named temp (double AND), second direct (single AND). */

extern void fn_02388CD8(int a1);
extern volatile unsigned short *fn_023899E8(int a1);

volatile unsigned short *fn_02389958(int a1, int a2) {
    int s1 = a1;
    int s2 = a2;
    volatile unsigned short *dp = (volatile unsigned short *)0x40001C2;
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int one = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(one);
    }
    fn_023899E8(1);
    {
        unsigned char t = s1;
        *dp = t & 0xFF;
    }
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int one = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(one);
    }
    fn_023899E8(0);
    *dp = s2 & 0xFF;
    return dp;
}
