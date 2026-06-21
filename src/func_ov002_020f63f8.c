/* func_ov002_020f63f8 at 0x020f63f8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_ov002_020f5dd8(int a, unsigned char b);

int func_ov002_020f63f8(char *r0, unsigned char *r1) {
    unsigned char v = *r1;
    r0[0x100] = v;
    func_ov002_020f5dd8(*(int *)(r0 + 0xd8), *(unsigned char *)(r0 + 0x100));
    return 1;
}
