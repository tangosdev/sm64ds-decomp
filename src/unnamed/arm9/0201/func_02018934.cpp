//cpp
#include "types.h"
#include "CP15.h"
#include "Heap.h"

extern "C" {

extern void FS_InitFile(int *s);
extern int myFS_OpenFile(void *file, const char *path);
extern int myFS_ReadFile(int *s, void *buf, int n);
extern int func_0204ee40(void *a, void *b, void *c);
extern int FS_CloseFile(int *s);

/* LoadArchive's loader: read the .narc at path whole into one heap block,
   after a 0x60-byte head, and mount it there under name (func_0204ee40).
   Returns the block, or 0 (freeing it) if the read or the mount fails.
   buf is an FSFile; words 8 and 9 are prop.file.top and bottom. */
void *func_02018934(char *name, char *path, void *heap) {
    int buf[0x11];
    void *result = 0;
    void *ptr;
    void *p60;
    int diff, aligned;

    FS_InitFile(buf);
    myFS_OpenFile(buf, path);
    diff = buf[9] - buf[8];
    aligned = (diff + 0xf) & ~0xf;
    ptr = ((mHeap::Heap_t *)heap)->_Allocate(aligned + 0x60, -0x10);
    p60 = (char *)ptr + 0x60;
    CP15::FlushAndInvalidateDataCache((u32)p60, aligned);
    if (myFS_ReadFile(buf, p60, diff) == diff && func_0204ee40(ptr, name, p60)) {
        result = ptr;
    } else {
        ((mHeap::Heap_t *)heap)->_Deallocate(ptr);
    }
    FS_CloseFile(buf);
    return result;
}
}
