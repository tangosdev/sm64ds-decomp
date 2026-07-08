extern int RENDER_DMA_CHANNEL;
void func_0205a064(int a0, int a1, int a2, int a3, int a4, int a5);
void DMASyncFillTransfer(int a0, int a1, int a2, int a3);
void MultiStore_Int(int val, int *dst, int len);

void func_020554bc(void)
{
    int i;
    if ((&RENDER_DMA_CHANNEL)[0] != -1) {
        func_0205a064((&RENDER_DMA_CHANNEL)[0], 0x4000330, 0, 0x10, 0, 0);
        DMASyncFillTransfer((&RENDER_DMA_CHANNEL)[0], 0x4000360, 0, 0x50);
    } else {
        volatile int a = 0;
        volatile int b;
        MultiStore_Int(a, (int*)0x4000330, 0x10);
        b = 0;
        MultiStore_Int(b, (int*)0x4000360, 0x50);
    }
    for (i = 0; i < 0x20; i++)
        *(volatile int*)0x40004d0 = 0;
}
