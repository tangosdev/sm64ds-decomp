//cpp
extern "C" void _ZN3IRQ13DmaTimHandlerEv(int irq);
extern "C" void _ZN3IRQ19Tim3OverflowHandlerEv(void)
{
    _ZN3IRQ13DmaTimHandlerEv(7);
}
