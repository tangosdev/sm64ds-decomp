/* func_ov002_020d3498 at 0x020d3498
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020d3498(char *r0) {
    *(int *)(r0 + 0x98) = -*(int *)(r0 + 0x98);
    *(short *)(r0 + 0x8e) = *(short *)(r0 + 0x8e) + 0x8000;
    *(short *)(r0 + 0x94) = *(short *)(r0 + 0x8e);
}
