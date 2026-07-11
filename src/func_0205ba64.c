struct Bits96 {
    unsigned char pad[0x388];
    unsigned int bits;
};
extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern void* data_020a7fc8[];
void func_0205ba64(int idx, void* val) {
    unsigned int saved = _ZN3IRQ7DisableEv();
    struct Bits96 *p = (struct Bits96 *)0x27ffc00;
    data_020a7fc8[idx] = val;
    if (val != 0) {
        *(unsigned int *)(((int)p + 0x388) & 0xFFFFFFFFFFFFFFFF) |= (1u << idx);
    } else {
        *(unsigned int *)(((int)p + 0x388) & 0xFFFFFFFFFFFFFFFF) &= ~(1u << idx);
    }
    _ZN3IRQ7RestoreEj(saved);
}
