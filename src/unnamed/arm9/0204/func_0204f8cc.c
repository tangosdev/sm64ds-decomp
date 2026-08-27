/* func_0204f8cc at 0x0204f8cc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void func_02052494(void *o, int a, int b);

void func_0204f8cc(char **c, int a, int b) {
    char *p = *c;
    if (p == 0) return;
    if (*(unsigned char *)(p + 0x2c) == 2) return;
    func_02052494(p + 0x1c, a << 8, b);
}
