//cpp
#include "CP15.h"

extern "C" {

/* FSFile, the two words this reads: top and bottom of prop.file (at 0x1c),
   the file's extent in its archive. bottom - top is its length
   (FS_GetLength). */
struct Obj {
    char pad[0x20];
    unsigned int top;    /* 0x20 */
    unsigned int bottom; /* 0x24 */
};

extern int myFS_ReadFile(void *a, int b, int c);
extern int FS_CloseFile(void *a);
extern void func_02018770(void);
extern int func_02018568(int a);

/* Read up to n bytes of the open file into buf (cache-maintained around the
   read), close it and unload any archive the open mounted. The length read,
   or 0 if the read came up short. The LZ77-tag test on the result is made
   and dropped: this path never decompresses. */
int func_020184e0(struct Obj *self, unsigned int buf, unsigned int n) {
    int r;
    unsigned int len = self->bottom - self->top;
    if (len > n)
        len = n;
    CP15::FlushAndInvalidateDataCache(buf, len);
    r = myFS_ReadFile(self, buf, len);
    FS_CloseFile(self);
    func_02018770();
    if (len != r)
        return 0;
    func_02018568(buf);
    CP15::FlushDataCache(buf, len);
    return len;
}
}
