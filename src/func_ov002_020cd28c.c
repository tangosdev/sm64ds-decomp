/* func_ov002_020cd28c at 0x020cd28c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

typedef int Fix12;
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int a, int b, Fix12 c, unsigned int d);

void func_ov002_020cd28c(char *r0) {
    char *r4 = r0;
    _ZN6Player7SetAnimEji5Fix12IiEj(r0, 0xaf, 0x40000000, 0x1000, 0);
    *(unsigned char*)(r4 + 0x6e3) = 7;
}
