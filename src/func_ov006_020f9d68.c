/* func_ov006_020f9d68 at 0x020f9d68
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020f9d68(char *r0, int r1) {
    *(short *)(r0 + 0x28) = (short)((4 - r1 % 5) * 2);
    *(unsigned char *)(r0 + 0x2d) = 5;
}
