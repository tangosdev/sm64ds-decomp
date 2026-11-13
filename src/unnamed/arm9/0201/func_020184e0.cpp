//cpp
#include "nitro/os/cache.h"
#include "nitro/fs/file.h"

extern "C" {


extern int myFS_ReadFile(FSFile *p_file, void *dst, int len);
extern void func_02018770(void);
extern int func_02018568(int a);
}


/* Read up to n bytes of the open file into buf (cache-maintained around the
   read), close it and unload any archive the open mounted. The length read,
   or 0 if the read came up short. The LZ77-tag test on the result is made
   and dropped: this path never decompresses. */
int func_020184e0(FSFile *self, unsigned int buf, unsigned int n) {
    int r;
    unsigned int len = FS_GetLength(self);
    if (len > n)
        len = n;
    DC_FlushRange((void *)buf, len);
    r = myFS_ReadFile(self, (void *)buf, len);
    FS_CloseFile(self);
    func_02018770();
    if (len != r)
        return 0;
    func_02018568(buf);
    DC_StoreRange((void *)buf, len);
    return len;
}
