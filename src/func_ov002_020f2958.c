/* func_ov002_020f2958 at 0x020f2958
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f2958(char *r0) {
    *(unsigned char *)(r0 + 0x1ed) = 1;
    *(int *)(r0 + 0x1e4) = 0x199;
    *(short *)(r0 + 0x1e8) = 0x199;
    *(unsigned char *)(r0 + 0x1ec) = 0;
    *(short *)(r0 + 0x1ea) = 0;
}
