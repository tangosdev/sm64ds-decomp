typedef unsigned short u16;
struct VramReg { u16 w0; u16 pad[0xb]; u16 f18; };
extern struct VramReg gVramReg;
extern void Vram__Map(u16 bits);
void _ZN2GX23SetBankForSubOBJExtPlttEt(u16 x){
    gVramReg.w0 = (gVramReg.w0 | gVramReg.f18) & ~x;
    gVramReg.f18 = x;
    if (x != 0) { if (x == 0x100) { *(volatile unsigned int*)0x4001000 |= 0x80000000; *(volatile unsigned char*)0x4000249 = 0x83; } }
    else { *(volatile unsigned int*)0x4001000 &= ~0x80000000; }
    Vram__Map(gVramReg.w0);
}
