/* func_ov002_020ec654 at 0x020ec654
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020ec654(char *r0) {
    int x = *(unsigned char*)(r0 + 0x428);
    x = ((x & 3) - 1) & 1;
    return x ? 1 : 0;
}
