//cpp

extern "C" {
void MenuGfx_StartCapture(void) {
    *(volatile unsigned int *)0x4000064 = 0x80370010;
}
}
