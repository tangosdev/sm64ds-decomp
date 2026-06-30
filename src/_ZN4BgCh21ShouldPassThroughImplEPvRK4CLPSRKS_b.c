/* _ZN4BgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b at 0x02039488
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern int func_02037e78(int *p);
extern int func_0203547c(unsigned char *p);
extern int func_02037e20(int *p);
extern int func_0203543c(unsigned char *p);
extern int func_02037e2c(int *p);
extern int func_0203545c(unsigned char *p);
extern int func_02037e14(int *p);
extern int func_02037e38(int *p);
extern int func_02035408(unsigned char *p);
extern int func_020353d4(unsigned char *p);
extern int func_020354b0(unsigned char *p);

int _ZN4BgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(void *p, int *clps, unsigned char *bg, int flag)
{
    int r4 = 0;

    if (func_02037e78(clps)) {
        r4 = 1;
        if (func_0203547c(bg) == 0) return r4;
    }
    if (func_02037e20(clps)) {
        r4 = 1;
        if (func_0203543c(bg) == 0) return r4;
    }
    if (func_02037e2c(clps)) {
        r4 = 1;
        if (func_0203545c(bg) != 0) return r4;
    }
    if (func_02037e14(clps)) {
        r4 = 1;
        if (func_0203545c(bg) == 0) return r4;
    }
    if (func_02037e38(clps) == 0x11) {
        r4 = 1;
        if (func_02035408(bg) != 0) return r4;
    }
    if (flag != 0 && func_02037e38(clps) == 0x14) {
        r4 = 1;
        if (func_020353d4(bg) != 0) return r4;
    }
    if (r4 == 0) {
        if (func_020354b0(bg) == 0) return 1;
    }
    return 0;
}
