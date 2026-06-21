/* func_020396c0 at 0x020396c0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

void func_020396c0(void *p, int v) {
    if (v < 0) v = 0;
    *(int *)((char *)p + 0x48) = v;
}
