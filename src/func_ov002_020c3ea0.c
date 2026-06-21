/* func_ov002_020c3ea0 at 0x020c3ea0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int a, int b, int c, unsigned int d);

void func_ov002_020c3ea0(char *self) {
    _ZN6Player7SetAnimEji5Fix12IiEj(self, 0xb3, 0x40000000, 0x1000, 0);
    *(unsigned char*)(self + 0x6e3) = 2;
}
