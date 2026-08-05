//cpp
extern "C" void _ZN3IRQ13DmaTimHandlerEv(int irq);
extern "C" void _ZN3IRQ11Dma1HandlerEv(void)
{
    _ZN3IRQ13DmaTimHandlerEv(1);
}
