//cpp
#include "nitro/os/cache.h"
#include "Memory.h"
#include "nitro/mi/memory.h"
#include "nitro/os/system.h"
#include "nitro/fs/file.h"

extern "C" {

extern int myFS_ReadFile(FSFile *p_file, void *dst, int len);
extern int func_02018568(void *a);
extern unsigned int func_02018ac4(unsigned int *p);
extern void DecompressLZ16(void *src, void *dst);
extern void func_02018770(void);

/* Read the whole open file (more than 8 bytes, or crash) into a new heap
   block: an LZ77-tagged file is decompressed into a block of its unpacked
   size, anything else copied. Closes the file, unloads any archive the open
   mounted, and with flush set flushes the result from the data cache. */
void *func_0201834c(FSFile *file, int flush) {
    void *result = 0;
    unsigned int len = FS_GetLength(file);
    if (len > 8) {
        u32 *buf = (u32 *)Memory::Allocate(len, -4);
        DC_FlushRange(buf, len);
        int readLen = myFS_ReadFile(file, buf, len);
        if (readLen != len)
            return result;
        if (func_02018568(buf) != 0) {
            /* the LZ header word follows the "LZ77" tag */
            len = func_02018ac4(buf + 1);
            result = Memory::Allocate(len);
            DecompressLZ16(buf + 1, result);
        } else {
            result = Memory::Allocate(len);
            MI_CpuCopy8(buf, result, len);
        }
        Memory::Deallocate(buf);
    } else {
        OS_Terminate();
    }
    FS_CloseFile(file);
    func_02018770();
    if (flush != 0)
        DC_StoreRange(result, len);
    return result;
}
}
