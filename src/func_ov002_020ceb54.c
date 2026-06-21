/* func_ov002_020ceb54 at 0x020ceb54
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int data_0209f32c;

int func_ov002_020ceb54(char *r0) {
    int a = *(int *)(r0 + 0x60);
    return (data_0209f32c - 0x50000) >= (a + 0x3c000);
}
