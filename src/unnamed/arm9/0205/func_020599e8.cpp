//cpp
extern "C" void Crash(void);
extern "C" long long func_02059650(void);
extern "C" void func_02059a60(void *c, long long v);

struct IRQ {
    static unsigned int Disable();
    static void Restore(unsigned int);
};

extern "C" void func_020599e8(int *c, long long v, int d, int arg5) {
    unsigned int saved;
    if (c != 0 && *c == 0) {
        /* ok */
    } else {
        Crash();
    }
    saved = IRQ::Disable();
    c[7] = 0;
    c[8] = 0;
    c[0] = d;
    c[1] = arg5;
    func_02059a60(c, func_02059650() + v);
    IRQ::Restore(saved);
}
