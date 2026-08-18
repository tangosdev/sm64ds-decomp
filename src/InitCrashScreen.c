extern void _ZN8dScene_c22ResetHardwareRegistersEv(void);
extern void _ZN2GX12SetBankForBGEt(unsigned short b);
extern void _ZN2GX11LoadBG1CharEPKvjj(const void *p, unsigned int a, unsigned int b);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, unsigned int a, unsigned int b);
extern void ResetCrashScreen(void);
extern void ShowCrashScreen(void);
extern void _ZN2GX6DispOnEv(void);
extern char data_0208a178;
extern char data_0208c178;

void InitCrashScreen(void)
{
    _ZN8dScene_c22ResetHardwareRegistersEv();
    *(volatile short *)0x4000050 = 0;
    *(volatile short *)0x4001050 = 0;
    _ZN2GX12SetBankForBGEt(8);
    *(volatile int *)0x4000000 = (*(volatile int *)0x4000000 & ~0x1f00) | 0x200;
    *(volatile short *)0x400000a = (unsigned short)((*(volatile unsigned short *)0x400000a & 0x43) | 0x1e10);
    _ZN2GX11LoadBG1CharEPKvjj(&data_0208a178, 0, 0x2000);
    *(volatile short *)0x5000000 = 0x7c00;
    _ZN2GX10LoadBGPlttEPKvjj(&data_0208c178, 0x1a0, 0x60);
    ResetCrashScreen();
    ShowCrashScreen();
    _ZN2GX6DispOnEv();
    *(volatile int *)0x4001000 = *(volatile int *)0x4001000 & ~0x10000;
}
