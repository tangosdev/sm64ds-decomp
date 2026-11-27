//cpp
/* Crash-screen input, drawing, and tiny print implementation. mwccarm emits
 * the bodies in reverse, so the print routines come first in source. */
#include "types.h"
#include "Heap.h"
#include "fBase_c.h"
#include "fListNode.h"
#include "fManager_c.h"
#include "msl/string.h"
#include "nitro/spi/pm.h"
#include "nitro/mb/mb.h"
#include "nitro/pad/pad.h"
#include "nitro/hw/registers.h"
#include "nitro/os/reset.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/mi/memory.h"
#include "dScene_c.h"

extern "C" void func_020147bc(unsigned short *dst, int glyph);
extern "C" int vsnprintf(char *s, size_t n, const char *format, char *arg);
extern "C" u64 func_020427c4(void);
extern "C" char *GetStageDebugName(void);
extern "C" int GetActorDebugName(unsigned int i);

extern void *data_020a0ea0;
extern u8 data_0208a178[];
extern u8 gFont2DNcl[];
extern signed char gCurrentSublevel;
extern u8 gCurrentEntranceId;
extern u8 gCurrentStarId;
extern signed char gCurrentAreaId;
extern void *data_020a4b68;
extern u16 gCurrentSpawnID;
extern u8 gBaseCreationState;
extern unsigned int gLastUsedFileID;
extern int data_020a0f38;
extern char data_0208e538[], data_0208e548[], data_0208e54c[], data_0208e55c[];
extern char data_0208e56c[], data_0208e57c[], data_0208e58c[], data_0208e59c[];
extern char data_0208e5b0[], data_0208e5c0[], data_0208e5d4[], data_0208e5e4[];
extern char data_0208e5f0[], data_0208e5fc[], data_0208e608[], data_0208e610[];
extern char data_0208e624[], data_0208e634[], data_0208e648[], gBuildTimeString[];

typedef struct {
    char c[52];
} Buf52;
extern Buf52 data_0208e504;

u8 data_0209cdcc;
u8 data_0209cdd0;
char *data_0209cdd4;
int data_0209cdd8;
int *gSavedRegisterValues;
int data_0209cde0;
u32 data_0209cde4;
char data_0209cde8[0x100];

extern "C" void nds_print(unsigned short *dst, const char *str) {
    while (*str != 0) {
        func_020147bc(dst++, *str++);
    }
}

extern "C" void nds_vprintf(unsigned short *dst, const char *fmt, char *args) {
    vsnprintf(data_0209cde8, 256, fmt, args);
    nds_print(dst, data_0209cde8);
}

extern "C" void nds_printf(unsigned short *dst, const char *fmt, ...) {
    char *args = (char *)(((u32)&fmt & ~3u) + 4);
    nds_vprintf(dst, fmt, args);
}

void nds_printt(unsigned short *dest, const char *str, int value) {
    int len = strlen(str);
    int c;
    unsigned short *p = dest + len;
    while (value != 0 && len != 0) {
        int out;
        len--;
        c = str[len];
        p--;
        if (c >= 0x30 && c <= 0x39) {
            out = value % (c - 0x2f);
            value = value / (c - 0x2f);
            out += 0x30;
        } else if (c >= 0x61 && c <= 0x7a) {
            out = c;
        } else if (c >= 0x41 && c <= 0x5a) {
            out = c;
        } else {
            out = (int)(c);
        }
        func_020147bc(p, (char)out);
    }
    if (len == 0)
        return;
    {
        unsigned short *q = dest + len;
        int space = 0x20;
        do {
            q--;
            len--;
            func_020147bc(q, space);
        } while (len != 0);
    }
}

void ShowCrashScreen(void) {
    u16 *scr = (u16 *)G2_GetBG1ScrPtr();
    int hp;
    int rr;
    int cc;

    nds_printt(scr + 0x2f4, data_0208e538, (u32)func_020427c4());
    nds_print(scr, data_0208e548);
    nds_print(scr + 3, gBuildTimeString);
    cc = gCurrentSublevel;
    nds_printf(scr + 0x20, data_0208e54c, cc, GetStageDebugName());
    nds_printf(scr + 0x40, data_0208e55c, gCurrentEntranceId);
    nds_printf(scr + 0x60, data_0208e56c, gCurrentStarId);
    nds_printf(scr + 0x80, data_0208e57c, gCurrentAreaId);

    rr = fManager_c::m_nowLoopProc;
    if (rr != 0 && rr < 6)
        nds_printf(scr + 0xc0, data_0208e58c, rr, ProcessList::func_020441bc(rr));

    hp = (data_020a4b68 != 0) ? ((fLiNdBaPr_c *)data_020a4b68)->owner->actorID : *&gCurrentSpawnID;
    if (hp != 0xffff) {
        int bb = gBaseCreationState;
        nds_printf(scr + 0xe0, data_0208e59c, hp, bb, GetActorDebugName(hp));
    }

    if (gLastUsedFileID != 0)
        nds_printf(scr + 0x100, data_0208e5b0, gLastUsedFileID);

    {
        u32 dispb = REG_IF;
        u32 dispa = REG_IE;
        nds_printf(scr + 0x140, data_0208e5c0, dispa, dispb);
    }

    if (data_020a0f38 != 0)
        nds_printf(scr + 0x160, data_0208e5d4, data_020a0f38);

    {
        int *regs = gSavedRegisterValues;
        if (regs != 0) {
            Buf52 buf = data_0208e504;
            int i;
            int *tail;
            {
                i = 0;
                if (i < 17)
                    do {
                        nds_printf(scr + (i + 3) * 0x20 + 0x14, data_0208e5e4, &buf.c[i * 3],
                                   regs[i]);
                        i++;
                    } while (i < 17);
            }
            tail = gSavedRegisterValues + 0x19;
            nds_printf(scr + 0x294, data_0208e5f0, tail[0]);
            nds_printf(scr + 0x2b4, data_0208e5fc, tail[1]);
        }
    }

    if (data_0209cde0 != 0) {
        nds_printf(scr + 0x1e0, data_0208e608, data_0209cde0, data_0209cdd8);
        nds_print(scr + 0x200, data_0209cdd4);
    }

    if (Memory::rootHeapPtr->Intact()) {
        int mx = Memory::rootHeapPtr->MaxAllocationUnitSize();
        nds_printf(scr + 0x280, data_0208e610, ((int)Memory::rootHeapPtr->heapSize - mx) / 1024,
                   mx / 1024);
    } else {
        nds_print(scr + 0x280, data_0208e624);
    }

    if (GAME_HEAP_PTR->Intact()) {
        int mx = GAME_HEAP_PTR->MaxAllocationUnitSize();
        nds_printf(scr + 0x2a0, data_0208e634, ((int)GAME_HEAP_PTR->heapSize - mx) / 1024,
                   mx / 1024);
    } else {
        nds_print(scr + 0x280, data_0208e648);
    }

    data_0209cdd0 = 1;
}

void ResetCrashScreen(void) {
    void *ptr = G2_GetBG1ScrPtr();
    volatile unsigned short val = 0xff;
    MI_CpuFill16(ptr, val, 0x800);
}

void InitCrashScreen(void) {
    dScene_c::ResetHardwareRegisters();
    G2_BlendNone();
    G2S_BlendNone();
    GX_SetBankForBG(GX_VRAM_BG_128_D);
    GX_SetVisiblePlane(DISPLAY_LAYER_BG1);
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf000,
                     GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);
    GX_LoadBG1Char(data_0208a178, 0, 0x2000);
    *(volatile short *)0x5000000 = 0x7c00;
    GX_LoadBGPltt(gFont2DNcl, 0x1a0, 0x60);
    ResetCrashScreen();
    ShowCrashScreen();
    GX_DispOn();
    GXS_DispOff();
}

void func_02013f4c(void) {
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
                mHeap::Heap_t *pa = Memory::rootHeapPtr;
                mHeap::Heap_t *pb = (mHeap::Heap_t *)data_020a0ea0;
                mHeap::Heap_t *pc = GAME_HEAP_PTR;
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
