//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "Heap.h"
#include "nitro/fs/file.h"

extern "C" {

extern int myFS_OpenFile(FSFile *p_file, const char *path);
extern int myFS_ReadFile(FSFile *p_file, void *dst, int len);
extern int func_0204ee40(void *a, void *b, void *c);

/* LoadArchive's loader: read the .narc at path whole into one heap block,
   after a 0x60-byte head, and mount it there under name (func_0204ee40).
   Returns the block, or 0 (freeing it) if the read or the mount fails. */
void *func_02018934(char *name, char *path, void *heap) {
    FSFile file;
    void *result = 0;
    void *ptr;
    void *p60;
    int diff, aligned;

    FS_InitFile(&file);
    myFS_OpenFile(&file, path);
    diff = FS_GetLength(&file);
    aligned = (diff + 0xf) & ~0xf;
    ptr = ((mHeap::Heap_t *)heap)->_Allocate(aligned + 0x60, -0x10);
    p60 = (char *)ptr + 0x60;
    DC_FlushRange(p60, aligned);
    if (myFS_ReadFile(&file, p60, diff) == diff && func_0204ee40(ptr, name, p60)) {
        result = ptr;
    } else {
        ((mHeap::Heap_t *)heap)->_Deallocate(ptr);
    }
    FS_CloseFile(&file);
    return result;
}
}
