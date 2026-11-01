//cpp
#include "CP15.h"
#include "Memory.h"

extern "C" {

/* FSFile, the two words this reads: top and bottom of prop.file (at 0x1c),
   the file's extent in its archive. bottom - top is its length
   (FS_GetLength). */
struct Obj {
    char pad[0x20];
    unsigned int top;    /* 0x20 */
    unsigned int bottom; /* 0x24 */
    char pad2[0x1c];
};

extern int func_020186c0(unsigned int val);
extern int func_02018568(int a);
extern unsigned func_02018ac4(unsigned *p);
extern void DecompressLZ16(void *src, void *dst);
extern int func_020185c0(void *buf, int x);
extern int FS_CloseFile(void *a);
extern void CpuCopy8(int a, int b, unsigned int c);
extern void func_02018770(void);
extern unsigned int func_02018a24(unsigned int i);
extern int func_0201834c(void *buf, int x);

/* Load file id (an asset handle, or an archived file id) into a new heap
   block, as SharedFilePtr::Load does: a file resident in a mounted archive
   is decompressed or copied out of it, anything else read through
   func_0201834c. flush nonzero flushes the result from the data cache. */
int func_0201818c(unsigned int id, int flush) {
    struct Obj obj;
    char buf[0x44];
    int p = func_020186c0(id);
    if (p != 0) {
        unsigned int size;
        int mem;
        if (func_02018568(p)) {
            unsigned int *q = (unsigned int *)(p + 4);
            size = func_02018ac4(q);
            mem = (int)Memory::Allocate(size);
            DecompressLZ16(q, (void *)mem);
        } else {
            func_020185c0(&obj, id);
            size = obj.bottom - obj.top;
            FS_CloseFile(&obj);
            mem = (int)Memory::Allocate(size, 0x20);
            CpuCopy8(p, mem, size);
        }
        func_02018770();
        if (flush != 0)
            CP15::FlushDataCache(mem, size);
        return mem;
    }
    func_020185c0(buf, func_02018a24(id));
    return func_0201834c(buf, flush);
}
}
