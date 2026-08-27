/* func_02051454 at 0x02051454
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern unsigned int _ZN3IRQ7DisableEv(void);
extern int func_02050d3c(int v);
extern int func_020509b0(unsigned int i);
extern void func_02050994(int i, int val);
extern void _ZN3IRQ7RestoreEj(unsigned int);

void func_02051454(int a, int b, unsigned int c) {
    int t;
    unsigned int irq;
    irq = _ZN3IRQ7DisableEv();
    t = func_02050d3c(b);
    if (a == func_020509b0(c)) {
        func_02050994(c, 0);
    }
    func_02050d3c(t);
    _ZN3IRQ7RestoreEj(irq);
}
