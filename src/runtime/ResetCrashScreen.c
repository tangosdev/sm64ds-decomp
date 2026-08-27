extern volatile void* _ZN2G212GetBG1ScrPtrEv(void);
extern void MultiStore16(unsigned short value, volatile void* dst, int count);

void ResetCrashScreen(void)
{
    volatile void* ptr = _ZN2G212GetBG1ScrPtrEv();
    volatile unsigned short val = 0xff;
    MultiStore16(val, ptr, 0x800);
}
