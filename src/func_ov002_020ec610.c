/* func_ov002_020ec610 at 0x020ec610
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020ec610(int r0) {
    int v = *(unsigned char*)(r0 + 0x428);
    return ((v >> 7) & 1) != 0;
}
