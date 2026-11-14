#ifndef OV007_MENU_MEM_H
#define OV007_MENU_MEM_H

/* ov007's memory hooks (src/ui/menus/unit020c3ce4.cpp). The menu never
 * calls a heap directly: it allocates through func_ov007_020c3df4 and frees
 * through func_ov007_020c3d1c, which forward to hooks the init dispatcher
 * installs. The first argument of the two allocators is a heap-name tag
 * ("global data", "pad", ...) that is not forwarded. The MenuMem_* names
 * are coined; ov007 records none. */

#ifdef __cplusplus

void MenuMem_SetAllocHook(int);

void MenuMem_SetFreeHook(int);

void MenuMem_SetMaxAllocSizeHook(int);

/* C++ linkage: the objects emit the Itanium names. */
void *MenuMem_AllocClear(const char *, int);
int MenuMem_GetMaxAllocSize(void);
extern "C" {
#else
#define MenuMem_AllocClear _Z18MenuMem_AllocClearPKci
#define MenuMem_GetMaxAllocSize _Z23MenuMem_GetMaxAllocSizev
void *MenuMem_AllocClear(const char *, int);
int MenuMem_GetMaxAllocSize(void);
#endif

void *func_ov007_020c3df4(const char *, int);
void func_ov007_020c3d1c(void *);

#ifdef __cplusplus
}
#endif

#endif
