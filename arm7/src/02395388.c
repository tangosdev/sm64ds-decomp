/* storage 0x02395388 runtime 0x027C7300 size 0x3C main. No pool. */

extern void fn_02395414(int, int, unsigned int);

void fn_02395388(int dst, int src, int src2, int count) {
    fn_02395414(dst, src, 0x12);
    if (count != 0)
        fn_02395414(dst + 0x28, src2, (unsigned int)(count + 1) >> 1);
}
