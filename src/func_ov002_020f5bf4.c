/* func_ov002_020f5bf4 at 0x020f5bf4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void func_ov002_020f5bf4(void *this) {
    char *p = (char *)this;
    if (*(unsigned char *)(p + 0x128) == 0) return;
    *(int *)(p + 0xe4) = 0x80000;
    *(int *)(p + 0xe8) = 0x60000;
    *(short *)(p + 0x114) = 0;
    *(unsigned char *)(p + 0x12a) = 1;
    *(int *)(p + 0x10c) = 0xccc;
    *(short *)(p + 0x110) = 0x155;
}
