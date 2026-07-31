//cpp
extern "C" {
void _ZN5Fader13AdvanceInterpEv(void* thiz);
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short* p, unsigned short a, int b);
}

struct FaderColor {
    void AdvanceFade();
};

void FaderColor::AdvanceFade()
{
    char* thiz = (char*)this;
int old = *(int*)(thiz + 4);
    _ZN5Fader13AdvanceInterpEv(thiz);
    if (*(int*)(thiz + 4) == old) return;
    {
        unsigned short color = *(unsigned short*)(thiz + 0xc);
        int m = color ? 0x10 : -0x10;
        int r = (*(int*)(thiz + 4) * m) >> 12;
        if (r != 0) {
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4000050, 0x3f, r);
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4001050, 0x3f, r);
        } else {
            *(unsigned short*)0x4000050 = 0;
            *(unsigned short*)0x4001050 = 0;
        }
    }

}
