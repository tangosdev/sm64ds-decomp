extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);

typedef union {
    unsigned int raw;
    struct {
        unsigned int a : 5;
        unsigned int b : 1;
        unsigned int c : 26;
    } f;
} IpcCmd;

int IPCSend(unsigned int a, unsigned int c, unsigned int b)
{
    IpcCmd cmd;
    unsigned int saved;

    cmd.f.a = a;
    cmd.f.b = b;
    cmd.f.c = c;

    if (*(volatile unsigned short *)0x4000184 & 0x4000) {
        *(volatile unsigned short *)0x4000184 |= 0xc000;
        return -1;
    }

    saved = _ZN3IRQ7DisableEv();

    if (*(volatile unsigned short *)0x4000184 & 2) {
        _ZN3IRQ7RestoreEj(saved);
        return -2;
    }

    *(volatile unsigned int *)0x4000188 = cmd.raw;
    _ZN3IRQ7RestoreEj(saved);
    return 0;
}
