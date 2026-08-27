/* func_02066fb0 at 0x02066fb0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern int data_020a9db8;

int func_02066fb0(int a, int b, int c) {
    int (*fn)(int, int, int) = *(int (**)(int, int, int))(data_020a9db8 + 0x14e4);
    if (fn) return fn(a, b, c);
}
