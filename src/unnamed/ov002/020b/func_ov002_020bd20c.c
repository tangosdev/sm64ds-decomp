/* func_ov002_020bd20c at 0x020bd20c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */

extern void func_ov002_020bd06c(unsigned char *a, unsigned char val);

int func_ov002_020bd20c(unsigned char *a, unsigned char *b)
{
    if (a[0x727] != *b) {
        func_ov002_020bd06c(a, *b);
        a[0x727] = *b;
        a[0x728] = 0;
    }
    return 1;
}
