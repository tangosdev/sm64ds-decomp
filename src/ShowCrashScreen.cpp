//cpp
#include "types.h"
#include "nitro/gx/gx_bgcnt.h"
#include "Heap.h"
#include "fListNode.h"
#include "fBase_c.h"
#include "nitro/hw/registers.h"

extern "C" u64 func_020427c4(void);
extern "C" {

extern char *GetStageDebugName(void);
}

#include "fManager_c.h"
extern "C" {

extern int GetActorDebugName(unsigned int i);
}

extern void nds_printt(unsigned short *dst, const char *fmt, s32 v);
extern "C" {

extern void nds_print(unsigned short *dst, const char *str);
extern void nds_printf(void *dst, const char *fmt, ...);
}

extern signed char gCurrentSublevel;
extern u8 gCurrentEntranceId;
extern u8 gCurrentStarId;
extern signed char gCurrentAreaId;
extern void *data_020a4b68;
extern u16 gCurrentSpawnID;
extern u8 gBaseCreationState;
extern unsigned int gLastUsedFileID;
extern int data_020a0f38;
extern int *gSavedRegisterValues;
extern int data_0209cde0;
extern int data_0209cdd8;
extern char *data_0209cdd4;
extern u8 data_0209cdd0;

extern char data_0208e538[], data_0208e548[], data_0208e54c[], data_0208e55c[];
extern char data_0208e56c[], data_0208e57c[], data_0208e58c[], data_0208e59c[];
extern char data_0208e5b0[], data_0208e5c0[], data_0208e5d4[], data_0208e5e4[];
extern char data_0208e5f0[], data_0208e5fc[], data_0208e608[], data_0208e610[];
extern char data_0208e624[], data_0208e634[], data_0208e648[], gBuildTimeString[];

typedef struct {
    char c[52];
} Buf52;
extern Buf52 data_0208e504;

void ShowCrashScreen(void)
{
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
