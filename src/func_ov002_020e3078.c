/* func_ov002_020e3078 at 0x020e3078
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020e3078(char *r0, int r1) {
    return *(int *)(r0 + 0x374) == r1;
}
