/* storage 0x02397B28 runtime 0x027C9AA0 linked 0x64 main ARM.
 * Copy four 20-byte blocks out of a1. */

extern void fn_02385400(void *dst, void *src, int len);

int fn_02397B28(unsigned char *src) {
    fn_02385400((void *)0x04805F80, src, 20);
    fn_02385400((void *)0x04805FA0, src + 20, 20);
    fn_02385400((void *)0x04805FC0, src + 40, 20);
    fn_02385400((void *)0x04805FE0, src + 60, 20);
    return 0;
}
