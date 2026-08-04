//cpp
typedef void (*IRQHandler)(void);
extern "C" void func_ov006_020efdf0(void *self, int x);
extern "C" int func_02053c10(int enable);
extern "C" unsigned int data_0209f608;
extern "C" unsigned char data_0209d460;
extern "C" void func_ov006_020efcf8(void);

namespace IRQ {
    void SetIRQHandler(unsigned int, IRQHandler);
    void EnableIRQs(unsigned int);
}

extern "C" void func_ov006_020f00a4(char *self)
{
    unsigned short saved;
    *(unsigned char *)(self + 0x4000 + 0x7f4) = 1;
    *(int *)(self + 0x4000 + 0x7e4) = 0x80000;
    *(int *)(self + 0x4000 + 0x7e8) = 0x60000;
    *(unsigned char *)(self + 0x4000 + 0x7f5) = 0;
    *(int *)(self + 0x4000 + 0x7ec) = 0x40000;
    *(int *)(self + 0x4000 + 0x7f0) = 0x1000;
    data_0209f608 = 0;
    func_ov006_020efdf0(self, 0);

    data_0209f608 = 1;
    func_ov006_020efdf0(self, 0);

    data_0209f608 = 0;
    saved = *(volatile unsigned short *)0x4000208;
    *(volatile unsigned short *)0x4000208 = 0;
    IRQ::SetIRQHandler(2, func_ov006_020efcf8);

    *(volatile unsigned int *)0x4000000 = (*(volatile unsigned int *)0x4000000 & ~0xe000) | 0x2000;
    data_0209d460 = 1;
    { unsigned int v = *(volatile unsigned short *)0x4000048; v = (v & ~0x3f) | 0x18; v = v | 0x20; *(volatile unsigned short *)0x4000048 = v; }
    *(volatile unsigned short *)0x400004a = (*(volatile unsigned short *)0x400004a & ~0x3f) | 0x14;
    *(volatile unsigned short *)0x4000040 = 0;
    *(volatile unsigned short *)0x4000044 = 0xc0;
    IRQ::EnableIRQs(2);
    func_02053c10(1);
    if (saved != 0) {
        unsigned short tmp = *(volatile unsigned short *)0x4000208;
        *(volatile unsigned short *)0x4000208 = 1;
    }
}
