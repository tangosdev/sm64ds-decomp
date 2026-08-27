//cpp
extern "C" int func_0204fce8(void *c, int b);
extern "C" void func_0205ac28(int a, int b, int c, int d);

struct IRQ {
    static unsigned int Disable();
    static void Restore(unsigned int);
};

struct C {
    char pad0[0xc];
    int fc;
    char pad10[0x10];
    int f20;
    char pad24[4];
    int f28;
    int f2c;
};

extern "C" void func_0204fed0(struct C *c) {
    if (((c->fc << 30) >> 31) == 0) return;
    while (c->f20 != 0) {
        unsigned int saved = IRQ::Disable();
        func_0204fce8(c, 1);
        IRQ::Restore(saved);
    }
    func_0205ac28(c->f2c, 0, 1 << c->f28, 0);
}
