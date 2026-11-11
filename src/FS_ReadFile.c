/* NitroSDK FS_ReadFile: a synchronous FSi_ReadFileCore. */
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern int FSi_ReadFileCore(FSFile *p_file, void *dst, int len, int async);

int FS_ReadFile(FSFile *p_file, void *dst, int len) {
    return FSi_ReadFileCore(p_file, dst, len, 0);
}
