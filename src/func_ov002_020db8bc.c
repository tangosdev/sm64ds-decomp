/* func_ov002_020db8bc at 0x020db8bc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020db8bc(char *r0, char r1) {
    r0[0x6f2] = 1;
    *(short *)(r0 + 0x6b2) = 0;
    r0[0x6f3] = r1;
}
