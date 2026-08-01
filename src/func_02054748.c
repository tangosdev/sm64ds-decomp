#include "types.h"
struct VramReg { u16 w0; u16 pad[6]; u16 w0e; };
extern struct VramReg data_020a6088;
extern void Vram__Map(u16 bits);

void func_02054748(int x)
{
    data_020a6088.w0 = (u16)((data_020a6088.w0 | data_020a6088.w0e) & ~x);
    data_020a6088.w0e = (u16)x;
    switch (x) {
    case 0:
        *(volatile unsigned int *)0x4000000 &= ~0x40000000;
        break;
    case 0x10:
        *(volatile unsigned int *)0x4000000 |= 0x40000000;
        *(volatile unsigned char *)0x4000244 = 0x84;
        break;
    case 0x40:
        *(volatile unsigned int *)0x4000000 |= 0x40000000;
        *(volatile unsigned char *)0x4000246 = 0x8c;
        break;
    case 0x60:
        *(volatile unsigned char *)0x4000246 = 0x8c;
        // fallthrough
    case 0x20:
        *(volatile unsigned char *)0x4000245 = 0x84;
        *(volatile unsigned int *)0x4000000 |= 0x40000000;
        break;
    }
    Vram__Map(data_020a6088.w0);
}
