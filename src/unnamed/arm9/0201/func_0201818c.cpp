//cpp
#include "nitro/os/cache.h"
#include "Memory.h"
#include "nitro/mi/memory.h"
#include "nitro/fs/file.h"

extern "C" {

extern int func_020186c0(unsigned int val);
extern int func_02018568(int a);
extern unsigned func_02018ac4(unsigned *p);
extern void DecompressLZ16(void *src, void *dst);
extern int func_020185c0(void *buf, unsigned int x);
extern void func_02018770(void);
extern unsigned int func_02018a24(unsigned int i);
extern void *func_0201834c(FSFile *file, int flush);

/* Load file id (an asset handle, or an archived file id) into a new heap
   block, as SharedFilePtr::Load does: a file resident in a mounted archive
   is decompressed or copied out of it, anything else read through
   func_0201834c. flush nonzero flushes the result from the data cache. */
void *func_0201818c(unsigned int id, int flush) {
    FSFile obj;
    FSFile file;
    int p = func_020186c0(id);
    if (p != 0) {
        unsigned int size;
        void *mem;
        if (func_02018568(p)) {
            unsigned int *q = (unsigned int *)(p + 4);
            size = func_02018ac4(q);
            mem = Memory::Allocate(size);
            DecompressLZ16(q, mem);
        } else {
            func_020185c0(&obj, id);
            size = FS_GetLength(&obj);
            FS_CloseFile(&obj);
            mem = Memory::Allocate(size, 0x20);
            MI_CpuCopy8((const void *)p, mem, size);
        }
        func_02018770();
        if (flush != 0)
            DC_StoreRange(mem, size);
        return mem;
    }
    func_020185c0(&file, func_02018a24(id));
    return func_0201834c(&file, flush);
}
}
