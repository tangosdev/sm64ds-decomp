//cpp
namespace GX {
    void SetBankForTex(unsigned short);
    void SetBankForTexPltt(unsigned short);
    void SetBankForOBJ(unsigned short);
}

extern "C" void func_ov007_020bd5a8(void)
{
    GX::SetBankForTex(3);
    GX::SetBankForTexPltt(0x20);
    GX::SetBankForOBJ(0x40);

    *(volatile unsigned short*)0x4000008 &= ~0x40;
    *(volatile unsigned short*)0x400000a &= ~0x40;
    *(volatile unsigned short*)0x400000c &= ~0x40;
    *(volatile unsigned short*)0x400000e &= ~0x40;
    *(volatile unsigned short*)0x400000c &= ~3;
    *(volatile unsigned int*)0x4000000 &= 0xffcfffef;
}
