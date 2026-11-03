//cpp
#include "Memory.h"
extern "C" {
extern void func_0205a588(void *, unsigned char, unsigned int);
/* the asset handle table (see func_02018a24.c) and its entry count */
extern void *data_0209d3b8;
extern int data_0209d3b0;

/* Allocate a zeroed asset handle table of n u16 file ids. */
int func_02018a58(int n) {
    unsigned int sz = n * 2;
    void *p = Memory::Allocate(sz);
    data_0209d3b8 = p;
    func_0205a588(p, 0, sz);
    data_0209d3b0 = n;
    return 1;
}
}
