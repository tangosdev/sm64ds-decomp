/* func_0205ae64 at 0x0205ae64
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern void func_0205ba64(int a, void *p);
extern int func_020587dc(void);
extern int func_0205ba3c(int bit, int word);
extern void func_02059d8c(int n);
extern void func_0205aed0(void);

void func_0205ae64(void) {
    func_0205ba64(7, (void *)func_0205aed0);
    if (func_020587dc()) return;
    if (func_0205ba3c(7, 1)) return;
    do {
        func_02059d8c(0x64);
    } while (!func_0205ba3c(7, 1));
}
