//cpp
#include "Memory.h"
#include "nitro/mi/memory.h"
extern "C" {
/* the asset handle table (see func_02018a24.c) and its entry count */
extern unsigned short *data_0209d3b8;
extern int data_0209d3b0;

/* Allocate a zeroed asset handle table of n u16 file ids. Coined name. */
int AllocAssetHandleTable(int n) {
    unsigned int sz = n * 2;
    unsigned short *p = (unsigned short *)Memory::Allocate(sz);
    data_0209d3b8 = p;
    MI_CpuClear8(p, sz);
    data_0209d3b0 = n;
    return 1;
}
}
