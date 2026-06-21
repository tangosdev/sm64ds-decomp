/* func_ov006_020f72c0 at 0x020f72c0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020f72c0(char *r0) {
    int v;
    *(unsigned char *)(r0 + 0x540a) = 0;
    *(short *)(r0 + 0x53ea) = 8;
    v = *(int *)(r0 + 0xb4);
    if (v >= 0xa) {
        *(unsigned char *)(r0 + 0x540a) = 2;
        *(short *)(r0 + 0x53ea) = 0xa;
        return;
    }
    if (v >= 5) {
        *(unsigned char *)(r0 + 0x540a) = 1;
        *(short *)(r0 + 0x53ea) = 9;
    }
}
