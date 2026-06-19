//cpp
typedef int (*fp)(void);
extern "C" fp data_020a89a4;
extern "C" int func_020610fc(void);
extern "C" void _ZN3IRQ21GameCardIREQMCHandlerEv(void)
{
    fp cb = data_020a89a4;
    int r = 1;
    *(volatile int*)0x020a89a0 = r;
    if (cb) r = cb();
    if (r == 0) return;
    func_020610fc();
}
