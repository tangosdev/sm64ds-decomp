/* func_ov002_020f035c at 0x020f035c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020f035c(int r0, int r1) {
    int v;
    switch (r0) {
    case 4:
        v = 0xff0;
        break;
    case 5:
        v = 0xfff;
        break;
    case 3:
        v = 0xfc1;
        break;
    case 0:
    default:
        v = 0xf74;
        break;
    case 1:
    case 2:
        v = 0xf08;
        break;
    }
    return r1 <= v;
}
