/* func_ov002_020efedc at 0x020efedc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020efedc(int *r0) {
    int result = 1;
    unsigned char v = ((unsigned int)r0[2] >> 8) & 3;
    if (v == 1) return result;
    if (v != 2) result = 0;
    return result;
}
