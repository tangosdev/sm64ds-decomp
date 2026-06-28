typedef unsigned int u32;
typedef unsigned short u16;

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 saved);

extern void (*data_020a7fc8[])(u32 idx, u32 data, u32 flag);

typedef union {
    u32 raw;
    struct {
        u32 a : 5;
        u32 b : 1;
        u32 c : 26;
    } f;
} IpcCmd;

#define FIFOCNT (*(volatile u16 *)0x4000184)
#define FIFORECV (*(volatile u32 *)0x4100000)
#define FIFOSEND (*(volatile u32 *)0x4000188)

void _ZN3IRQ24IPCRxFifoNotEmptyHandlerEv(void) {
    IpcCmd cmd;
    int code;
    u32 saved;
    for (;;) {
        if (FIFOCNT & 0x4000) {
            FIFOCNT = FIFOCNT | 0xc000;
            code = -3;
        } else {
            saved = _ZN3IRQ7DisableEv();
            if (FIFOCNT & 0x100) {
                _ZN3IRQ7RestoreEj(saved);
                code = -4;
            } else {
                cmd.raw = FIFORECV;
                _ZN3IRQ7RestoreEj(saved);
                code = 0;
            }
        }
        if (code == -4)
            return;
        if (code == -3)
            continue;
        {
            void (*fn)(u32, u32, u32);
            if (cmd.f.a == 0)
                continue;
            fn = data_020a7fc8[cmd.f.a];
            if (fn != 0) {
                fn(cmd.f.a, cmd.f.c, cmd.f.b);
                continue;
            }
            if (cmd.f.b)
                continue;
            cmd.raw = cmd.raw | 0x20;
            if (FIFOCNT & 0x4000) {
                FIFOCNT = FIFOCNT | 0xc000;
                continue;
            }
            saved = _ZN3IRQ7DisableEv();
            if (FIFOCNT & 2) {
                _ZN3IRQ7RestoreEj(saved);
                continue;
            }
            FIFOSEND = cmd.raw;
            _ZN3IRQ7RestoreEj(saved);
        }
    }
}
