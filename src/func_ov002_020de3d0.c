/* func_ov002_020de3d0 at 0x020de3d0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020de3d0(void *unused, short *a, short *b) {
    if (*b >= 0x100) *b -= 0x20;
    if (*b <= -0x100) *b += 0x20;
    if (*a < 0) *b += 0x10;
    else *b -= 0x10;
    *a = *a + *b;
}
