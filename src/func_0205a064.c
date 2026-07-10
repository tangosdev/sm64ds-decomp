typedef unsigned int u32;

extern void func_02059fa8(int ch);
extern void func_02056e98(u32 idx, u32 handler, u32 arg);
extern void DMAStartTransfer(u32 ch, u32 reg, u32 src, u32 ctrl);

void func_0205a064(u32 ch, u32 src, u32 dst, u32 size, void (*cb)(u32), u32 cbArg)
{
    u32 *reg;
    if (size == 0)
    {
        if (cb == 0)
            return;
        cb(cbArg);
        return;
    }
    func_02059fa8(ch);
    if (cb != 0)
    {
        func_02056e98(ch, (u32)cb, cbArg);
        reg = (u32 *)(int)(((long long)(int)(0x040000e0 + ch * 4)) & 0xFFFFFFFFFFFFFFFFLL);
        *reg = dst;
        DMAStartTransfer(ch, (u32)reg, src, (size >> 2) | 0xc5000000);
    }
    else
    {
        reg = (u32 *)(int)(((long long)(int)(0x040000e0 + ch * 4)) & 0xFFFFFFFFFFFFFFFFLL);
        *reg = dst;
        DMAStartTransfer(ch, (u32)reg, src, (size >> 2) | 0x85000000);
    }
}
