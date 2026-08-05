//cpp
extern "C" void _ZN3IRQ13DmaTimHandlerEv(int irq);
extern "C" void _ZN3IRQ19Tim0OverflowHandlerEv(void)
{
    _ZN3IRQ13DmaTimHandlerEv(4);
}
