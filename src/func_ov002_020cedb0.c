/* func_ov002_020cedb0 at 0x020cedb0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern unsigned short data_ov002_020ff158[];

int func_ov002_020cedb0(int *r0, int r1) {
    unsigned t = data_ov002_020ff158[r0[2]];
    return (int)(((long long)r1 * (int)t + 0x800) >> 12);
}
