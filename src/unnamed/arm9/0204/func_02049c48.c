// func_02049c48 - set active context, clear state table if changed

extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int);
extern void MultiStore16(unsigned short val, void* dest, unsigned int size);
extern void func_0205a8c4(int val);

extern int data_020a4c64;       // linked to 0x020a4c64
extern unsigned short data_020a4c70[];  // linked to 0x020a4c70

void func_02049c48(int self) {
    int currentOwner = data_020a4c64;
    volatile unsigned short tmp;
    unsigned int savedIRQ;
    if (self == currentOwner)
        return;
    if (currentOwner == 1) {
        func_0205a8c4(0);
    }
    savedIRQ = _ZN3IRQ7DisableEv();
    tmp = 0;
    MultiStore16(tmp, data_020a4c70, 0xc0);
    data_020a4c64 = self;
    _ZN3IRQ7RestoreEj(savedIRQ);
    if (self == 1) {
        func_0205a8c4(0x3000);
    }
}