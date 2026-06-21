/* func_ov002_020c97f8 at 0x020c97f8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int a, int b, int fix, unsigned int e);

void func_ov002_020c97f8(char *r0) {
    char *r4 = r0;
    *(unsigned char*)(r4 + 0x6e3) = 0x1a;
    *(unsigned short*)(r4 + 0x600 + 0x9c) = 0;
    _ZN6Player7SetAnimEji5Fix12IiEj(r4, 0x47, 0, 0x1000, 0);
    *(int*)(r4 + 0x98) = 0;
    *(int*)(r4 + 0xa8) = 0;
}
