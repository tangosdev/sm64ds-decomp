/* func_ov006_020f51f0 at 0x020f51f0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020f51f0(char *r0) {
    *(unsigned char *)(r0 + 0x533c) = 0;
    *(short *)(r0 + 0x532a) = 4;
    int v = *(int *)(r0 + 0xb4);
    if (v >= 0xa) {
        *(unsigned char *)(r0 + 0x533c) = 2;
        *(short *)(r0 + 0x532a) = 6;
        return;
    }
    if (v >= 5) {
        *(unsigned char *)(r0 + 0x533c) = 1;
        *(short *)(r0 + 0x532a) = 5;
    }
}
