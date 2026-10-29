//cpp
#include "Memory.h"

extern "C" {

extern void func_020672d0(void *thiz, int a1, int a2, int a3, int a4);
extern void func_020616e8(void *p, int a1);

extern unsigned short data_020a0f14;
extern int data_020a0f94;
extern short data_020a0f24;
extern short data_020a0f2c;
extern void *data_020a0f4c;
extern void *data_020a0f44;
extern void *data_020a0f48;
extern void *data_020a0f54;
extern unsigned short data_020a0f28;
extern void *data_020a0f74;
extern void *data_020a0f80;
extern int data_020a0f58;
extern void *data_020a0f60;
extern int data_020a0f64;
extern void *data_020a0f68;
extern char data_020a1064[];

void func_020408b0(unsigned short arg)
{
    int b;
    data_020a0f14 = arg;
    data_020a0f94 = 0;
    data_020a0f24 = 0;
    data_020a0f2c = 1;
    data_020a0f4c = Memory::Allocate(0x1300, 0x20);
    data_020a0f44 = Memory::Allocate(0x40, 0x20);
    data_020a0f48 = Memory::Allocate(0x20, 0x20);
    data_020a0f54 = Memory::Allocate(0xc0, 0x20);
    b = (*(volatile unsigned short *)0x27ffc40 == 2);
    if (b != 0) {
        char *p;
        func_020672d0(data_020a0f54, 0x200, 0x20, 0, 1);
        p = (char *)data_020a0f54;
        data_020a0f28 = *(unsigned short *)(p + 0x36);
        p += 0x48;
        ++*(unsigned short *)(int)(p);
    }
    data_020a0f74 = Memory::Allocate(0x420, 0x20);
    data_020a0f80 = Memory::Allocate(0x100, 0x20);
    data_020a0f58 = 0x220;
    data_020a0f60 = Memory::Allocate(0x220, 0x20);
    data_020a0f64 = 0x480;
    data_020a0f68 = Memory::Allocate(0x480, 0x20);
    *(unsigned short *)(data_020a1064 + 0x30) = data_020a0f28;
    func_020616e8(data_020a0f4c, 1);
}
}
