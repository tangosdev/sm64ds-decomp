//cpp
#include "Memory.h"
#include "private/wm_views.h"

extern "C" {

/* MB_ReadMultiBootParentBssDesc */
extern void func_020672d0(void *thiz, int parentMaxSize, int childMaxSize, int ksFlag, int csFlag);
extern int WM_Init(void *buf, int ch);

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

/* Allocate the link's buffers. On a download-play child (the halfword at
   0x27ffc40 is 2), rebuild the parent's WMBssDesc into data_020a0f54, keep its
   channel in data_020a0f28 (and at data_020a1064 +0x30) and step its tgid. */
void func_020408b0(unsigned short arg) {
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
        WMBssDesc *desc;
        func_020672d0(data_020a0f54, 0x200, 0x20, 0, 1);
        desc = (WMBssDesc *)data_020a0f54;
        data_020a0f28 = desc->channel;
        ++desc->gameInfo.tgid;
    }
    data_020a0f74 = Memory::Allocate(0x420, 0x20);
    data_020a0f80 = Memory::Allocate(0x100, 0x20);
    data_020a0f58 = 0x220;
    data_020a0f60 = Memory::Allocate(0x220, 0x20);
    data_020a0f64 = 0x480;
    data_020a0f68 = Memory::Allocate(0x480, 0x20);
    *(unsigned short *)(data_020a1064 + 0x30) = data_020a0f28;
    WM_Init(data_020a0f4c, 1);
}
}
