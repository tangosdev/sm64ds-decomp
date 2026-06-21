/* func_ov002_020cd2c4 at 0x020cd2c4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int a, int b, int c, unsigned int d);

void func_ov002_020cd2c4(char *r0) {
    char *r4 = r0;
    _ZN6Player7SetAnimEji5Fix12IiEj(r0, 0xab, 0x40000000, 0x1000, 0);
    *(unsigned char*)(r4 + 0x6e3) = 1;
    *(unsigned short*)(r4 + 0x600 + 0xa4) = 0;
}
