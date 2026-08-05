//cpp
extern "C" void _ZN3IRQ13DmaTimHandlerEv(int irq);
extern "C" void _ZN3IRQ11Dma3HandlerEv(void)
{
    _ZN3IRQ13DmaTimHandlerEv(3);
}
