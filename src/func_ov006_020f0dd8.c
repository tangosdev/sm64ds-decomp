/* func_ov006_020f0dd8 at 0x020f0dd8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020f0dd8(char *r0) {
    int i;
    for (i = 0; i < 2; i++) {
        *(unsigned char *)(r0 + 0x47b4) = 1;
        *(unsigned char *)(r0 + 0x47b6) = 0;
        *(short *)(r0 + 0x47b0) = 0;
        *(short *)(r0 + 0x47b2) = 0;
        *(unsigned char *)(r0 + 0x47b5) = 0;
        *(unsigned char *)(r0 + 0x47b7) = 0;
        r0 += 0x18;
    }
}
