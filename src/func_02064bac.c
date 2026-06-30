/* func_02064bac at 0x02064bac
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void func_0205a61c(const void *src, void *dst, unsigned int size);

int func_02064bac(unsigned char *dst, unsigned short a1, unsigned char a2, unsigned int a3)
{
    unsigned char hdr = 8;
    unsigned short s = a1;
    unsigned char *p1, *p2, *p3;
    func_0205a61c(&hdr, dst, 1);
    p1 = dst + 1;
    func_0205a61c(&s, p1, 2);
    p2 = p1 + 2;
    func_0205a61c(&a2, p2, 1);
    p3 = p2 + 1;
    func_0205a61c(&a3, p3, 4);
    return (p3 + 4) - dst;
}