typedef unsigned int u32;

extern void func_02059fa8(int ch);
extern void func_02056e98(int a, void *cb, int arg6);
extern void DMAStartTransfer(int a, int src, int dst, u32 ctrl);

void func_02059fd0(int ch, int src, int dst, u32 size, void (*cb)(int), int cbarg)
{
    if (size == 0) {
        if (cb != 0)
            cb(cbarg);
        return;
    }
    func_02059fa8(ch);
    if (cb != 0) {
        func_02056e98(ch, (void *)cb, cbarg);
        DMAStartTransfer(ch, src, dst, (size >> 2) | 0xc4000000);
        return;
    }
    DMAStartTransfer(ch, src, dst, (size >> 2) | 0x84000000);
}
