/* storage 0x02387A30 runtime 0x037FF8C8 linked 0x9C wram ARM. */

struct Ctrl {
    int pad;
    int left;
    int a;
    int b;
};

extern struct Ctrl dword_3808D84;
typedef void (*StepFn)(struct Ctrl *ctrl);

void fn_02387A30(int a, int b, int count, StepFn step) {
    volatile unsigned short *mmio = (volatile unsigned short *)0x040001A0;
    struct Ctrl *base = &dword_3808D84;
    base->a = a;
    base->b = b;
    *mmio = 0xA040;
    while (count != 0) {
        int *cursor = &base->left;
        *cursor = *cursor - 1;
        if (base->left == 0)
            *mmio = 0xA000;
        while (*mmio & 0x80)
            ;
        step(base);
        count--;
    }
    if (base->left == 0)
        *mmio = 0;
}
