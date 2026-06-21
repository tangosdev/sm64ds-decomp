/* func_ov002_020efebc at 0x020efebc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020efebc(char *r0) {
    unsigned char v = (*(unsigned *)(r0 + 8) >> 8) & 3;
    return v == 2;
}
