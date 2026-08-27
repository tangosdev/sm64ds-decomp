/* func_0204ff48 at 0x0204ff48
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern void func_0205abb8(int a, int b, int c, int d);
extern void *func_0205afb4(void);
extern void func_0205b070(int);
extern void func_0205afe0(unsigned);

void func_0204ff48(char *p) {
    if (!((*(int *)(p + 0xc) << 30) >> 31)) return;
    func_0205abb8(*(int *)(p + 0x2c), 0, 1 << *(int *)(p + 0x28), 0);
    void *x = func_0205afb4();
    func_0205b070(1);
    func_0205afe0((unsigned)x);
}
