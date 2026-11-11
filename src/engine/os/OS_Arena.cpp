//cpp
/* OS arenas (the Nitro SDK's os_arena.c), arm9 .text 0x02058cd0..0x02059040.
 *
 * pret/pokediamond's NitroSDK OS_arena.c defines, in this order:
 * OS_InitArena, OS_InitArenaEx, OS_GetArenaHi, OS_GetArenaLo,
 * OS_GetInitArenaHi, OS_GetInitArenaLo, OS_SetArenaHi, OS_SetArenaLo,
 * OS_AllocFromArenaLo, OS_AllocFromArenaHi. mwccarm defers C bodies and emits
 * them in reverse, so the ROM runs bottom-up from OS_AllocFromArenaLo at
 * 0x02058cd0 to OS_InitArena at 0x02058f28. Nothing calls
 * OS_AllocFromArenaHi, so the linker dropped it.
 *
 * The arena bounds live in the OSArenaInfo block shared with the ARM7:
 * lo[9] at 0x027ffda0 and hi[9] at 0x027ffdc4.
 *
 * This SDK is older than pret's. Its OS_InitArenaEx sets up the main-ex
 * arena only on an 8 MB console and opens protection region 1 over it; its
 * OS_GetInitArenaLo / OS_GetInitArenaHi return this game's fixed bounds.
 */
#include "types.h"
#include "nitro/os/protectionRegion.h"

/* OSArenaId */
#define OS_ARENA_MAIN 0
#define OS_ARENA_MAINEX 2
#define OS_ARENA_ITCM 3
#define OS_ARENA_DTCM 4
#define OS_ARENA_SHARED 5
#define OS_ARENA_WRAM_MAIN 6

#define OS_CONSOLE_SIZE_MASK 3
#define OS_CONSOLE_SIZE_8MB 2

#define OSi_ARENA_LO ((unsigned int *)0x027ffda0)
#define OSi_ARENA_HI ((unsigned int *)0x027ffdc4)

extern "C" {

/* local extern: no header declares OS_GetConsoleType (the SDK's
 * os_emulator.c) yet. */
extern int OS_GetConsoleType(void);

/* NitroSDK linker constants: the address is the value (the LCF assigns
 * SDK_SYS_STACKSIZE = 0 and SDK_IRQ_STACKSIZE = 0x600). */
extern void SDK_IRQ_STACKSIZE(void);
extern void SDK_SYS_STACKSIZE(void);

/* OSi_Initialized. OS_Thread.cpp takes this word's address as the top of
 * OSi_IdleThreadStack, which ends here. */
extern unsigned int data_020a6378;
extern short data_023c0000[]; /* HW_DTCM */

unsigned int OS_GetArenaHi(int id);
unsigned int OS_GetArenaLo(int id);
unsigned int OS_GetInitArenaHi(int id);
unsigned int OS_GetInitArenaLo(int id);
void OS_SetArenaHi(int id, unsigned int newHi);
void OS_SetArenaLo(int id, unsigned int newLo);

void OS_InitArena(void) {
    if (data_020a6378 != 0)
        return;
    data_020a6378 = 1;
    OS_SetArenaHi(OS_ARENA_MAIN, OS_GetInitArenaHi(OS_ARENA_MAIN));
    OS_SetArenaLo(OS_ARENA_MAIN, OS_GetInitArenaLo(OS_ARENA_MAIN));
    OS_SetArenaLo(OS_ARENA_MAINEX, 0);
    OS_SetArenaHi(OS_ARENA_MAINEX, 0);
    OS_SetArenaHi(OS_ARENA_ITCM, OS_GetInitArenaHi(OS_ARENA_ITCM));
    OS_SetArenaLo(OS_ARENA_ITCM, OS_GetInitArenaLo(OS_ARENA_ITCM));
    OS_SetArenaHi(OS_ARENA_DTCM, OS_GetInitArenaHi(OS_ARENA_DTCM));
    OS_SetArenaLo(OS_ARENA_DTCM, OS_GetInitArenaLo(OS_ARENA_DTCM));
    OS_SetArenaHi(OS_ARENA_SHARED, OS_GetInitArenaHi(OS_ARENA_SHARED));
    OS_SetArenaLo(OS_ARENA_SHARED, OS_GetInitArenaLo(OS_ARENA_SHARED));
    OS_SetArenaHi(OS_ARENA_WRAM_MAIN, OS_GetInitArenaHi(OS_ARENA_WRAM_MAIN));
    OS_SetArenaLo(OS_ARENA_WRAM_MAIN, OS_GetInitArenaLo(OS_ARENA_WRAM_MAIN));
}

void OS_InitArenaEx(void) {
    if ((OS_GetConsoleType() & OS_CONSOLE_SIZE_MASK) != OS_CONSOLE_SIZE_8MB)
        return;
    OS_SetArenaHi(OS_ARENA_MAINEX, OS_GetInitArenaHi(OS_ARENA_MAINEX));
    OS_SetArenaLo(OS_ARENA_MAINEX, OS_GetInitArenaLo(OS_ARENA_MAINEX));
    OS_SetProtectionRegion1(0x200002d); /* main RAM, 8 MB, enabled */
}

unsigned int OS_GetArenaHi(int id) {
    return OSi_ARENA_HI[id];
}

unsigned int OS_GetArenaLo(int id) {
    return OSi_ARENA_LO[id];
}

unsigned int OS_GetInitArenaHi(int id) {
    switch (id) {
    case OS_ARENA_MAIN:
        return 0x23c0000;
    case OS_ARENA_MAINEX:
        return 0x2700000;
    case OS_ARENA_ITCM:
        return 0x2000000;
    case OS_ARENA_DTCM: {
        u32 irqStackLo = (u32)data_023c0000 + 0x3f80 - (s32)SDK_IRQ_STACKSIZE;
        u32 sysStackLo;

        if (!(s32)SDK_SYS_STACKSIZE) {
            sysStackLo = (u32)data_023c0000;
            if (sysStackLo < 0x23c0020) {
                sysStackLo = 0x23c0020;
            }
        } else if ((s32)SDK_SYS_STACKSIZE < 0) {
            sysStackLo = (u32)data_023c0000 - (s32)SDK_SYS_STACKSIZE;
        } else {
            sysStackLo = irqStackLo - (s32)SDK_SYS_STACKSIZE;
        }
        return sysStackLo;
    }
    case OS_ARENA_SHARED:
        return 0x27ff800;
    case OS_ARENA_WRAM_MAIN:
        return 0x37f8000;
    default:
        return 0;
    }
}

unsigned int OS_GetInitArenaLo(int id) {
    switch (id) {
    case OS_ARENA_MAIN:
        return 0x214eaa0;
    case OS_ARENA_MAINEX:
        return 0x2400000;
    case OS_ARENA_ITCM:
        return 0x1ffdf40;
    case OS_ARENA_DTCM:
        return 0x23c0020;
    case OS_ARENA_SHARED:
        return 0x27ff000;
    case OS_ARENA_WRAM_MAIN:
        return 0x37f8000;
    }
    return 0;
}

void OS_SetArenaHi(int id, unsigned int newHi) {
    OSi_ARENA_HI[id] = newHi;
}

void OS_SetArenaLo(int id, unsigned int newLo) {
    OSi_ARENA_LO[id] = newLo;
}

void *OS_AllocFromArenaLo(int id, unsigned int size, unsigned int align) {
    unsigned int ptr = OS_GetArenaLo(id);
    unsigned int arenaLo, arenaHi;
    if (ptr == 0)
        return 0;
    arenaLo = (ptr + align - 1) & ~(align - 1);
    arenaHi = arenaLo + size;
    arenaHi = (arenaHi + align - 1) & ~(align - 1);
    if (arenaHi > OS_GetArenaHi(id))
        return 0;
    OS_SetArenaLo(id, arenaHi);
    return (void *)arenaLo;
}

} // extern "C"
