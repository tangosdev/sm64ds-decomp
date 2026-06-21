/* func_ov002_020f26c4 at 0x020f26c4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020f26c4(char *r0, unsigned char *r1) {
    *(unsigned char *)(r0 + 0x100) = *r1;
    return 1;
}
