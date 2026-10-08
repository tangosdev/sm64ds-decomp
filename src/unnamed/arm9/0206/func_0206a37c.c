/* func_0206a37c at 0x0206a37c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern void func_02057014(int a);
extern void _ZN3IRQ7RestoreEj(unsigned int);

void func_0206a37c(int a, int *p) {
    if (p[0] == 0) func_02057014(a);
    _ZN3IRQ7RestoreEj(p[1]);
}
