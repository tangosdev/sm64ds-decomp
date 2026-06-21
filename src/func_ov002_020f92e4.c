/* func_ov002_020f92e4 at 0x020f92e4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020f92e4(int r0) {
    unsigned char v = *(unsigned char*)(r0 + 0x36d);
    if (v != 0) {
        if (v != 4) return 5;
    }
    return 0;
}
