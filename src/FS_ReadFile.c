/* NitroSDK FS_ReadFile: a synchronous FSi_ReadFileCore (func_0205d688). */
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern int func_0205d688(FSFile *p_file, void *dst, int len, int async);

int FS_ReadFile(FSFile *p_file, void *dst, int len) {
    return func_0205d688(p_file, dst, len, 0);
}
