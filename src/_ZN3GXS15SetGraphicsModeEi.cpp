//cpp
extern "C" void _ZN3GXS15SetGraphicsModeEi(int mode);
void _ZN3GXS15SetGraphicsModeEi(int mode) {
    *(volatile unsigned*)0x4001000 = (*(volatile unsigned*)0x4001000 & ~7) | mode;
}
