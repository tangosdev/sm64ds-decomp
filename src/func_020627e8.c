#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int func_02061548(void);
extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int savedState);
extern void _ZN4CP1519InvalidateDataCacheEjj(u32 addr, u32 len);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern int func_020623ec(int a0, int a1, int a2, int a3, u16 a4, u16 a5);
extern void func_02062d10(void *self);

#define H(o) (*(u16 *)(self + (o)))

void func_020627e8(u8 *self, int flag)
{
    int ctx;
    u32 saved;
    u16 slot;
    u16 next;
    u16 mask;
    u16 prevSlot;
    u16 newSlot;
    volatile u16 zero;
    int ret;

    ctx = func_02061548();
    saved = _ZN3IRQ7DisableEv();
    slot = H(0x408);
    if (*(u16 *)(self + ((u32)slot << 8)) != 0)
        goto early_exit;

    _ZN4CP1519InvalidateDataCacheEjj(*(u32 *)(ctx + 4) + 0x86, 2);

    prevSlot = H(0x408);
    next = (u16)((prevSlot + 1) % 4);
    mask = *(u16 *)(*(u32 *)(ctx + 4) + 0x86);
    newSlot = (H(0x418) == 1) ? (u16)((next + 1) % 4) : next;

    zero = 0;
    MultiStore16(zero, self + ((u32)newSlot << 8), 0x100);

    *(u16 *)(self + ((u32)newSlot << 8)) = (u16)(H(0x40e) & (mask | 1));
    H(0x408) = next;
    *(u16 *)(self + ((u32)prevSlot << 8)) = H(0x40e);
    if (flag == 1) {
        *(u16 *)(self + ((u32)prevSlot << 8)) &= ~1;
    }

    _ZN3IRQ7RestoreEj(saved);

    ret = func_020623ec((int)func_02062d10, (int)(self + ((u32)prevSlot << 8)),
                        H(0x414), (u16)(H(0x40e) & mask), H(0x416), 1);

    if (ret == 7) {
        *(u16 *)(self + ((u32)prevSlot << 1) + 0x400) = 0xffff;
        H(0x40a) = (u16)((H(0x40a) + 1) % 4);
        return;
    }
    if (ret == 0)
        return;
    if (ret != 2) {
        H(0x41c) = 3;
    }
    return;

early_exit:
    _ZN3IRQ7RestoreEj(saved);
    return;
}
