/* func_ov002_020c1e44 at 0x020c1e44
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020c1e44(char *self, int a) {
    short *h = (short*)(self + 0x94);
    *h = (short)((a << 1) - *h + 0x8000);
    *(int*)(self + 0x98) = (int)(((long long)*(int*)(self + 0x98) * 0xe00 + 0x800) >> 12);
    if (*(int*)(self + 0x98) < 0x4000) {
        *(int*)(self + 0x98) = 0x4000;
    }
    return 1;
}
