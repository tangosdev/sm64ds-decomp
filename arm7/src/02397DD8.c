/* storage 0x02397DD8 runtime 0x027C9D50 linked 0x64 main ARM.
 * Reject a record whose first halfword is odd, then copy it twice. */

extern void fn_023869D8(void *dst, void *src);

int fn_02397DD8(unsigned short *src) {
    if (*src & 1)
        return 5;
    fn_023869D8(*(unsigned char **)0x0380FFF4 + 0x2E8, src);
    fn_023869D8((void *)0x04808018, src);
    *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 0x304) |= 2;
    return 0;
}
