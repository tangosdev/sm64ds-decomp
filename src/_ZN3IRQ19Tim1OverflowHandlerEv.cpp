//cpp
extern "C" void _ZN3IRQ13DmaTimHandlerEv(int irq);
extern "C" void _ZN3IRQ19Tim1OverflowHandlerEv(void)
{
    _ZN3IRQ13DmaTimHandlerEv(5);
}
