//cpp
#include "types.h"
#include "Heap.h"
#include "fBase_c.h"
#include "nitro/spi/pm.h"
#include "nitro/mb/mb.h"
#include "nitro/pad/pad.h"
#include "nitro/hw/registers.h"
#include "nitro/os/reset.h"

extern "C" {

extern u32 data_0209cde4;
extern u8 data_0209cdcc;
extern mHeap::Heap_t *data_020a0e9c;
extern void *data_020a0ea0;

extern void InitCrashScreen(void);
extern void ShowCrashScreen(void);

void func_02013f4c(void)
{
    u32 cur;
    u32 old;
    u8 sel;
    u16 raw;

    raw = PAD_Read();
    cur = raw & PAD_KEYPORT_MASK;
    old = data_0209cde4;
    data_0209cde4 = cur;
    old = cur & (old ^ cur);
    sel = data_0209cdcc;

    switch (sel) {
    case 0:
        if (old & PAD_BUTTON_DEBUG)
            data_0209cdcc = 5;
        if (cur == (PAD_BUTTON_L | PAD_BUTTON_R | PAD_KEY_LEFT | PAD_BUTTON_A))
            data_0209cdcc++;
        break;
    case 1:
        if (cur == 0)
            data_0209cdcc = sel + 1;
        break;
    case 2:
        if (cur & ~(PAD_KEY_DOWN | PAD_BUTTON_B)) {
            data_0209cdcc = 0;
            break;
        }
        if (cur == (PAD_KEY_DOWN | PAD_BUTTON_B))
            data_0209cdcc = sel + 1;
        break;
    case 3:
        if (cur == 0)
            data_0209cdcc = sel + 1;
        break;
    case 4:
        if (cur & ~(PAD_BUTTON_START | PAD_BUTTON_SELECT)) {
            data_0209cdcc = 0;
            break;
        }
        if (cur == (PAD_BUTTON_START | PAD_BUTTON_SELECT))
            data_0209cdcc = sel + 1;
        break;
    case 5:
        InitCrashScreen();
        data_0209cdcc++;
    case 6:
        ShowCrashScreen();
        if ((cur & (PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START | PAD_BUTTON_SELECT)) ==
            (PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START | PAD_BUTTON_SELECT)) {
            if (!MB_IsMultiBootChild()) {
                OS_ResetSystem(0);
            } else {
                PM_ForceToPowerOff();
            }
        }
        if (old & PAD_BUTTON_A) {
            if (cur & PAD_BUTTON_SELECT) {
                mHeap::Heap_t *pa = data_020a0e9c;
                mHeap::Heap_t *pb = (mHeap::Heap_t *)data_020a0ea0;
                mHeap::Heap_t *pc = data_020a0eac;
                if (cur & PAD_KEY_LEFT) {
                    pa->Rescue();
                } else if (cur & PAD_KEY_RIGHT) {
                    pc->Rescue();
                } else if (cur & PAD_KEY_UP) {
                    pb->Rescue();
                }
            }
        }
        break;
    }

    /* Wait for the next VBlank (line 192): out of this one, then into it. */
    while ((int)REG_VCOUNT >= 192) {
    }
    while ((int)REG_VCOUNT < 192) {
    }
}
}
