/* func_ov002_020f0918 at 0x020f0918
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f0918(char *r0, char *r1) {
    if (r1 == 0) return;
    *(int *)(r0 + 0x138) = *(int *)(r1 + 4);
    *(int *)(r0 + 0x148) = *(int *)(r0 + 0x60) - *(int *)(r1 + 0x60);
}
