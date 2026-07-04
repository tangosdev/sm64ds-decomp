typedef unsigned short u16;

extern void _ZN3G3X13SetClearColorEtiiib(u16 color, int r, int g, int b, int alpha);

void func_ov007_020b6c54(void) {
    volatile u16 *reg = (volatile u16*)0x4000060;
    *reg = (*reg & ~0x3000) | 0x10;
    *reg = (*reg & ~0x3000) | 8;
    *reg = *reg & ~0x3002;
    *(int*)0x4000540 = 2;
    _ZN3G3X13SetClearColorEtiiib(0, 0, 0x7fff, 0x3f, 0);
}
