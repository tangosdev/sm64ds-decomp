/* func_02064fe8 at 0x02064fe8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void func_0205a61c(const void *src, void *dst, unsigned int size);

int func_02064fe8(void *dst, short b, char c)
{
    char *p;
    char *q;
    unsigned char one = 1;
    short s = b;
    func_0205a61c(&one, dst, 1);
    p = (char *)dst + 1;
    func_0205a61c(&s, p, 2);
    q = p + 2;
    func_0205a61c(&c, q, 1);
    return q + 1 - (char *)dst;
}
