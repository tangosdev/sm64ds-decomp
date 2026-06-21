/* func_ov006_020e9318 at 0x020e9318
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_020e9318(char *r0, int r1) {
    if (*(unsigned char *)(r0 + 0x554e) == 1) {
        unsigned char *p = (unsigned char *)(r0 + 0x521c + r1 * 0x18);
        if (*p == 1) *p = 0;
    }
    if (*(unsigned char *)(r0 + 0x554e) >= 0x10) {
        *(unsigned char *)(r0 + r1 * 0x18 + 0x5219) = 1;
    }
}
