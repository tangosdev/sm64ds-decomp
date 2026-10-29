//cpp
#include "CP15.h"

extern "C" {

struct Obj { char pad[0x20]; unsigned int cur; unsigned int end; };

extern int func_02018d48(void *a, unsigned int b, unsigned int c);
extern void FS_CloseFile(void *a);
extern void func_02018770(void);
extern int func_02018568(unsigned int a);

int func_020184e0(struct Obj *self, unsigned int buf, unsigned int n)
{
    int r;
    unsigned int len = self->end - self->cur;
    if (len > n) len = n;
    CP15::FlushAndInvalidateDataCache(buf, len);
    r = func_02018d48(self, buf, len);
    FS_CloseFile(self);
    func_02018770();
    if (len != r) return 0;
    func_02018568(buf);
    CP15::FlushDataCache(buf, len);
    return len;
}
}
