/* func_0206df14 at 0x0206df14
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern char data_0209a330[];
extern int func_0206e06c(void *);

int func_0206df14(void) {
    int result = 0;
    char *p = data_0209a330;
    int i = 1;
    do {
        if (((unsigned int)*(int *)(p + 4) << 22) >> 29) {
            if (func_0206e06c(p)) {
                result = -1;
            }
        }
        p = (i < 3) ? (data_0209a330 + i++ * 0x4c) : 0;
    } while (p);
    return result;
}
