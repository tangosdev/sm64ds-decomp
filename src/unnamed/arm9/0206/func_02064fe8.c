/* func_02064fe8 at 0x02064fe8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_02064fe8(void *dst, short b, char c)
{
    char *p;
    char *q;
    unsigned char one = 1;
    short s = b;
    CpuCopy8(&one, dst, 1);
    p = (char *)dst + 1;
    CpuCopy8(&s, p, 2);
    q = p + 2;
    CpuCopy8(&c, q, 1);
    return q + 1 - (char *)dst;
}
