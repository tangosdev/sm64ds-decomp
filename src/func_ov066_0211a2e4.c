extern void* _ZN5Actor13ClosestPlayerEv(void* c);
extern unsigned char data_ov066_0211ae04;
extern unsigned char data_ov066_0211ae08;

void func_ov066_0211a2e4(void* a, int b)
{
    volatile int dummy[3];
    (void)dummy;
    void* p;
    if (data_ov066_0211ae04 != 1) return;
    if (b == 0) return;
    p = _ZN5Actor13ClosestPlayerEv(a);
    if (p == 0) return;
    if (*(int*)((char*)p + 0x64) < (int)0xff387000) {
        data_ov066_0211ae08++;
    }
}
