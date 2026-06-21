/* func_ov002_020c0688 at 0x020c0688
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

int func_ov002_020c0688(char *self) {
    int r1, r2;
    if (*(unsigned char *)(self + 0x6de) == 0) return 1;
    if (*(unsigned char *)(self + 0x70e) != 0) {
        r2 = *(int *)(self + 0x644);
        r1 = *(int *)(self + 0x60) - r2;
        if (r1 >= 0xb4000) goto ret0;
        if (r1 < 0) goto ret0;
        *(int *)(self + 0x60) = r2;
        return 1;
    }
    if (*(unsigned char *)(self + 0x6e4) == 0) goto ret0;
    r2 = *(int *)(self + 0x644);
    r1 = *(int *)(self + 0x60) - r2;
    if (r1 < 0x28000) {
        *(int *)(self + 0x60) = r2;
        return 1;
    }
ret0:
    return 0;
}
