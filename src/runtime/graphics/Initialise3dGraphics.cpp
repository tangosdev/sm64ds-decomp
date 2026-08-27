//cpp
extern "C" {
extern void func_0205583c(void);
extern void func_020554bc(void);
extern void func_020556d0(void);
extern void _ZN3G3X13SetClearColorEtiiib(unsigned short t, int a, int b, int c, int d);
extern void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
    int a, int b, int c, int d, int e, int f, int g, void *h);

void Initialise3dGraphics(int arg) {
    func_0205583c();
    func_020554bc();
    func_020556d0();
    *(volatile unsigned short*)0x4000060 &= ~0x3002;
    *(volatile unsigned short*)0x4000060 = (*(volatile unsigned short*)0x4000060 & ~0x3000) | 0x10;
    *(volatile unsigned short*)0x4000060 = (*(volatile unsigned short*)0x4000060 & ~0x3000) | 8;
    *(volatile unsigned short*)0x4000060 &= 0xcfdf;
    _ZN3G3X13SetClearColorEtiiib(0, arg, 0x7fff, 0x3f, 0);
    *(int*)0x4000580 = 0xbfff0000;
    _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
        0x579, 0xf09, 0x1555, 0x1000, 0x3e8000, 0x1000, 1, 0);
    *(int*)0x400044c = 0;
    *(int*)0x4000540 = 3;
}
}
