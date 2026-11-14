//cpp
/* unit020c3ce4 -- ov007 unit 0x020c3ce4..0x020c3e7c: 9 functions, .text only.
 *
 * The title menu's memory hooks. ov007 is the title and file-select menu
 * program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or
 * vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. The menu does not call a heap directly: the init
 * dispatcher, func_ov007_020b7138, installs three hooks through
 * MenuMem_SetMaxAllocSizeHook, MenuMem_SetFreeHook and
 * MenuMem_SetAllocHook, and every module allocates through
 * func_ov007_020c3df4 (the alloc hook, data_ov007_02103258), frees through
 * func_ov007_020c3d1c (the free hook, data_ov007_02103254) and asks
 * MenuMem_GetMaxAllocSize for the largest block the caller's heap can give
 * (the third hook, data_ov007_02104c14; 0 while it is unset).
 * MenuMem_AllocClear allocates and zero-fills, picking the store routine by
 * the size's low bits. func_ov007_020c3df4's
 * first argument (the callers pass a heap-name string) is not forwarded.
 * The two .data hook words start out pointing at this unit's own defaults,
 * MenuMem_DefaultFree and MenuMem_DefaultAlloc, which go to the OS heap:
 * arena 0, the current heap.
 *
 * Edges: the two .data hook words 0x02103254 / 0x02103258 and the bss word
 * 0x02104c14 are referenced only by these functions, and the two defaults
 * are referenced only by those .data words (no code calls or loads them).
 * The next function, func_ov007_020c3e7c, frees a sprite group and only
 * calls in through func_ov007_020c3d1c. The left edge is medium:
 * func_ov007_020c3bc8 stores a magic word into the neighbouring .data word
 * 0x02103250 and may belong here, but the colour blend func_ov007_020c3be0
 * between them references no data at all.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The MenuMem_* names are coined from the bodies and their callers; ov007
 * records no names. func_ov007_020c3df4 (alloc) and
 * func_ov007_020c3d1c (free) keep address names while files another
 * branch holds call them. The third hook is word 2 of dScDSMT_c's hook
 * table data_ov007_02103290 (0x020cca80 in the ROM image), whose body is
 * Heap::MaxAllocationUnitSize on the game heap; that names
 * MenuMem_SetMaxAllocSizeHook and MenuMem_GetMaxAllocSize.
 */
#include "nitro/mi/memory.h"
#include "nitro/os/alloc.h"

typedef int (*Ov007QueryHook)(void);
typedef void (*Ov007FreeHook)(void *);
typedef void *(*Ov007AllocHook)(unsigned int size);

extern Ov007QueryHook data_ov007_02104c14;
extern Ov007FreeHook data_ov007_02103254;
extern Ov007AllocHook data_ov007_02103258;

/* The alloc hook's default (data_ov007_02103258's initial value). */
void *MenuMem_DefaultAlloc(unsigned int size) {
    return OS_AllocFromHeap(0, -1, size);
}

/* The free hook's default (data_ov007_02103254's initial value). */
void MenuMem_DefaultFree(void *ptr) {
    OS_FreeToHeap(0, -1, ptr);
}

void MenuMem_SetAllocHook(int v) {
    data_ov007_02103258 = reinterpret_cast<Ov007AllocHook>(v);
}

void MenuMem_SetFreeHook(int v) {
    data_ov007_02103254 = reinterpret_cast<Ov007FreeHook>(v);
}

void MenuMem_SetMaxAllocSizeHook(int v) {
    data_ov007_02104c14 = reinterpret_cast<Ov007QueryHook>(v);
}

/* The menu's allocator: forwards the size to the alloc hook and returns
 * the block. name tags the allocation ("global data", "pad",
 * "touch_panel", ...; most callers pass 0) and is dropped. */
/* Callers, including the .c files, declare this with C linkage. */
extern "C" void *func_ov007_020c3df4(const char *name, int size) {
    return data_ov007_02103258(size);
}

extern "C" void *MenuMem_AllocClear(const char *name, int n) {
    int *r = (int *)func_ov007_020c3df4(name, n);
    volatile unsigned short v16;
    volatile unsigned v32;
    volatile int vint;
    switch (n - (n & ~3)) {
    case 0:
        if (n - (n & ~0x1f) != 0) {
            v32 = 0;
            MI_CpuFillFast(r, v32, n);
        } else {
            vint = 0;
            MI_CpuFill32(r, vint, n);
        }
        break;
    case 2:
        v16 = 0;
        MI_CpuFill16(r, v16, n);
        break;
    case 1:
    case 3:
        MI_CpuClear8(r, n);
        break;
    }
    return r;
}

/* NOT a daPly_c method: it reads no fields, it only calls the free hook. */
extern "C" void func_ov007_020c3d1c(void *self) {
    data_ov007_02103254(self);
}

extern "C" int MenuMem_GetMaxAllocSize(void) {
    if (data_ov007_02104c14 == 0)
        return 0;
    return data_ov007_02104c14();
}
