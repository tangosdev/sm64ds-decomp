//cpp
#include "CP15.h"
#include "Memory.h"

extern "C" {

/* func_0201834c at 0x0201834c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct Obj { char pad[0x20]; unsigned int cur; unsigned int end; };

extern int func_02018d48(struct Obj *a, void *b, unsigned int c);
extern int func_02018568(void *a);
extern unsigned int func_02018ac4(void *p);
extern void DecompressLZ16(void *src, void *dst);
extern void CpuCopy8(void *a, void *b, unsigned int c);
extern void Crash(void);
extern void FS_CloseFile(struct Obj *a);
extern void func_02018770(void);

unsigned int func_0201834c(struct Obj *self, int flag)
{
    unsigned int result = 0;
    unsigned int len = self->end - self->cur;
    if (len > 8) {
        void *buf = Memory::Allocate(len, -4);
        CP15::FlushAndInvalidateDataCache((unsigned int)buf, len);
        int r = func_02018d48(self, buf, len);
        if (r != len) return result;
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
