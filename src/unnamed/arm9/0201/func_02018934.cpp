//cpp
#include "types.h"
#include "CP15.h"
#include "Heap.h"

extern "C" {

/* func_02018934 at 0x02018934
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void FS_InitFile(int *s);
extern int func_02018d98(int *s, int a);
extern int func_02018d48(int *s, void *buf, int n);
extern int func_0204ee40(void *a, void *b, void *c);
extern void FS_CloseFile(int *s);

void *func_02018934(void *arg0, int arg1, void *heap)
{
    int buf[0x11];
    void *result = 0;
    void *ptr;
    void *p60;
    int diff, aligned;

    FS_InitFile(buf);
    func_02018d98(buf, arg1);
    diff = buf[9] - buf[8];
    aligned = (diff + 0xf) & ~0xf;
    ptr = ((mHeap::Heap_t *)heap)->_Allocate(aligned + 0x60, -0x10);
    p60 = (char *)ptr + 0x60;
    CP15::FlushAndInvalidateDataCache((u32)p60, aligned);
    if (func_02018d48(buf, p60, diff) == diff &&
        func_0204ee40(ptr, arg0, p60)) {
        result = ptr;
    } else {
        ((mHeap::Heap_t *)heap)->_Deallocate(ptr);
    }
    FS_CloseFile(buf);
    return result;
}
}
