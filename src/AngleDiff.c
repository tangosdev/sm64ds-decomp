/* AngleDiff at 0x0203b0e8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

int AngleDiff(int a, int b) {
    short d = (short)(a - b);
    if (d < 0) return -d;
    return d;
}
