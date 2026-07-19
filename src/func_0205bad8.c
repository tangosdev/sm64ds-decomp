typedef unsigned int u32;
typedef unsigned short u16;

extern u32  _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(u32 irqBits, void (*handler)(void));
extern void _ZN3IRQ10EnableIRQsEj(u32 irqBits);
extern void _ZN3IRQ24IPCRxFifoNotEmptyHandlerEv(void);

extern u16 data_020a7fc4;
extern u32 data_020a7fc8[];

typedef struct {
    unsigned char pad[0x388];
    unsigned int bits;
} Bits96;

void func_0205bad8(void)
{
    u32 state;
    int i;
    int r5;
    int lr;
    Bits96 *p;

    volatile u16 *reg;
    int ip;

    state = _ZN3IRQ7DisableEv();
    if (data_020a7fc4 == 0) {
        data_020a7fc4 = 1;
        p = (Bits96 *)0x27ffc00;
        p->bits = 0;
        for (i = 0; i < 0x20; i++) {
            data_020a7fc8[i] = 0;
        }
        _ZN3IRQ13SetIRQHandlerEjPFvvE(0x40000, _ZN3IRQ24IPCRxFifoNotEmptyHandlerEv);
        _ZN3IRQ10EnableIRQsEj(0x40000);
        *(volatile u16 *)0x4000184 = 0xc408;

        ip = 0;
        reg = (volatile u16 *)0x4000180;
        for (;;) {
            lr = *reg & 0xf;
            *reg = (u16)(lr << 8);
            if (lr == 0) {
                if (ip > 4) break;
            }
            r5 = 0x3e8;
            if ((*reg & 0xf) == lr) {
                for (;;) {
                    if (r5 <= 0) { ip = 0; break; }
                    r5--;
                    if ((*reg & 0xf) != lr) break;
                }
            }
            ip++;
        }
    }
    _ZN3IRQ7RestoreEj(state);
}
