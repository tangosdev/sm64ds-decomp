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
};

extern int myFS_ReadFile(struct Obj *a, void *b, unsigned int c);
extern int func_02018568(void *a);
extern unsigned int func_02018ac4(void *p);
extern void DecompressLZ16(void *src, void *dst);
extern void CpuCopy8(void *a, void *b, unsigned int c);
extern void Crash(void);
extern int FS_CloseFile(struct Obj *a);
extern void func_02018770(void);

/* Read the whole open file (more than 8 bytes, or crash) into a new heap
   block: an LZ77-tagged file is decompressed into a block of its unpacked
   size, anything else copied. Closes the file, unloads any archive the open
   mounted, and with flag set flushes the result from the data cache. */
unsigned int func_0201834c(struct Obj *self, int flag) {
    unsigned int result = 0;
    unsigned int len = self->bottom - self->top;
    if (len > 8) {
        void *buf = Memory::Allocate(len, -4);
        CP15::FlushAndInvalidateDataCache((unsigned int)buf, len);
        int r = myFS_ReadFile(self, buf, len);
        if (r != len)
            return result;
        if (func_02018568(buf) != 0) {
            len = func_02018ac4((void *)((char *)buf + 4));
            result = (unsigned int)Memory::Allocate(len);
            DecompressLZ16((void *)((char *)buf + 4), (void *)result);
        } else {
            result = (unsigned int)Memory::Allocate(len);
            CpuCopy8(buf, (void *)result, len);
        }
        Memory::Deallocate(buf);
    } else {
        Crash();
    }
    FS_CloseFile(self);
    func_02018770();
    if (flag != 0)
        CP15::FlushDataCache(result, len);
    return result;
}
}
