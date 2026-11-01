//cpp
#include "types.h"
#include "Heap.h"
#include "nitro/spi/pm.h"

extern "C" {

extern u32 data_0209cde4;
extern u8 data_0209cdcc;
extern mHeap::Heap_t *data_020a0e9c;
extern void *data_020a0ea0;
extern void *data_020a0eac;

extern void InitCrashScreen(void);
extern void ShowCrashScreen(void);
extern void OS_ResetSystem(unsigned int a);

void func_02013f4c(void)
{
    u32 cur;
    u32 old;
    u8 sel;
    u16 raw;

    raw = ((*(volatile u16*)0x4000130 | *(volatile u16*)0x27fffa8) ^ 0x2fff) & 0x2fff;
    cur = raw & 0x3ff;
    old = data_0209cde4;
    data_0209cde4 = cur;
    old = cur & (old ^ cur);
    sel = data_0209cdcc;

    switch (sel) {
    case 0:
        if (old & 0x2000)
            data_0209cdcc = 5;
        if (cur == 0x321)
            data_0209cdcc++;
        break;
    case 1:
        if (cur == 0)
            data_0209cdcc = sel + 1;
        break;
    case 2:
        if (cur & ~0x82) {
            data_0209cdcc = 0;
            break;
        }
        if (cur == 0x82)
            data_0209cdcc = sel + 1;
        break;
    case 3:
        if (cur == 0)
            data_0209cdcc = sel + 1;
        break;
    case 4:
        if (cur & ~0xc) {
            data_0209cdcc = 0;
            break;
        }
        if (cur == 0xc)
            data_0209cdcc = sel + 1;
        break;
    case 5:
        InitCrashScreen();
        data_0209cdcc++;
    case 6:
        ShowCrashScreen();
        if ((cur & 0x30c) == 0x30c) {
            int b = (*(volatile u16*)0x27ffc40 == 2);
            if (b == 0) {
                OS_ResetSystem(0);
            } else {
                PM_ForceToPowerOff();
            }
        }
        if (old & 1) {
            if (cur & 4) {
                mHeap::Heap_t *pa = data_020a0e9c;
                mHeap::Heap_t *pb = (mHeap::Heap_t *)data_020a0ea0;
                mHeap::Heap_t *pc = (mHeap::Heap_t *)data_020a0eac;
                if (cur & 0x20) {
                    pa->Rescue();
                } else if (cur & 0x10) {
                    pc->Rescue();
                } else if (cur & 0x40) {
                    pb->Rescue();
                }
            }
        }
        break;
    }

    while ((int)*(volatile u16*)0x4000006 >= 0xc0) {}
    while ((int)*(volatile u16*)0x4000006 < 0xc0) {}
}
}
