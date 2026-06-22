//cpp
extern "C" void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short *p, unsigned short val, short amt);
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short *p, unsigned short val, short amt)
{
    if (amt < 0) {
        p[0] = val | 0xc0;
        p[2] = -amt;
    } else {
        p[0] = val | 0x80;
        p[2] = amt;
    }
}
