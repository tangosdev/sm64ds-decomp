/* func_ov002_020f0e54 at 0x020f0e54
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f0e54(int *r0, int *r1) {
    if (r1 == 0) return;
    r0[0x138 / 4] = r1[1];
    r0[0x148 / 4] = r0[0x60 / 4] - r1[0x60 / 4];
}
