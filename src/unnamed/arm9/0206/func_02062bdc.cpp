//cpp
struct IRQ { static unsigned int Disable(); static void Restore(unsigned int); };
extern "C" void func_02062990(void *base, unsigned int a, unsigned int b);
extern "C" void func_020627e8(void *base, int x);

struct M {
    unsigned char pad0[2];
    unsigned short f2;
    unsigned short f4;
    unsigned char pad6[6];
    unsigned int fc;
    unsigned char pad10[2];
    unsigned short f12;
    unsigned char pad14[8];
    void *f1c;
};

extern "C" void func_02062bdc(struct M *m)
{
    void *base = m->f1c;
    if (m->f2 == 0) {
        if (m->f4 == 7) goto L7;
        if (m->f4 == 9) goto L9;
        if (m->f4 != 0x15) return;
        func_02062990(base, m->f12, m->fc);
        func_020627e8(base, 0);
        return;

    L7:
        func_020627e8(base, 0);
        return;

    L9:
        {
            unsigned short mask = (unsigned short)(1 << m->f12);
            unsigned int saved = IRQ::Disable();
            unsigned short b408 = *(unsigned short *)((char *)base + 0x408);
            *(unsigned short *)((char *)base + (b408 << 8)) &= ~mask;
            if (*(unsigned short *)((char *)base + 0x418) == 1) {
                int n = (*(unsigned short *)((char *)base + 0x408) + 1) % 4;
                *(unsigned short *)((char *)base + (n << 8)) &= ~mask;
            }
            IRQ::Restore(saved);
            func_020627e8(base, 0);
            if (*(unsigned short *)((char *)base + 0x418) != 1) return;
            func_020627e8(base, 0);
            return;
        }
    }
    *(unsigned short *)((char *)base + 0x41c) = 3;
}
