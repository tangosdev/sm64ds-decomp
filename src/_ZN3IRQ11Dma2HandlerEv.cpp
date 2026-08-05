//cpp
extern "C" void _ZN3IRQ13DmaTimHandlerEv(int irq);
extern "C" void _ZN3IRQ11Dma2HandlerEv(void)
{
    _ZN3IRQ13DmaTimHandlerEv(2);
}
