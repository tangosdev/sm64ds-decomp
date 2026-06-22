//cpp
extern "C" void _ZN3G2x13SetBlendAlphaEPVttttt(
    volatile unsigned int *reg,
    unsigned short a, unsigned short b, unsigned short c,
    unsigned int d, unsigned short e)
{
    *reg = ((a | 0x40) | (b << 8)) | ((c | (d << 8)) << 16);
}
