/* func_02064b2c at 0x02064b2c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void CpuCopy8(const void *src, int off, int size);

int func_02064b2c(int off, short b, int c, int d)
{
    unsigned char tag = 9;
    short val = b;
    int c1, c2, c3;
    CpuCopy8(&tag, off, 1);
    c1 = off + 1;
    CpuCopy8(&val, c1, 2);
    c2 = c1 + 2;
    CpuCopy8(&c, c2, 1);
    c3 = c2 + 1;
    CpuCopy8(&d, c3, 4);
    return c3 + 4 - off;
}