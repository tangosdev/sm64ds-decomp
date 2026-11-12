#include "private/arm9_archive_table.h"

#include "nitro/os/system.h"
#include "nitro/fs/file.h"

extern void func_02018598(FSFileID *c, int arg);
extern int myFS_OpenFileFast(FSFile *p_file, FSFileID id);
extern int LoadArchive(int idx);
extern int func_0204eda4(int file, char *archive, unsigned int index);

/* Open game file id into the FSFile at file. Ids below 0x8000 are FAT ids
   in the ROM archive (func_02018598 builds the FSFileID); higher ids are
   opened inside the archive whose range holds them, mounting it first if
   needed (and recording it in data_0208eb54). Any failure is fatal. */
int func_020185c0(FSFile *file, unsigned int id) {
    int result = 0;

    FS_InitFile(file);

    if (id < 0x8000) {
        FSFileID arg;
        FSFileID fileId;
        func_02018598(&fileId, id);
        arg.arc = fileId.arc;
        arg.file_id = fileId.file_id;
        result = myFS_OpenFileFast(file, arg);
    } else {
        unsigned int i = 0;
        ArchiveTableEntry *e = data_0208ecf4;
        do {
            if (e->firstFileId <= id && id < e->endFileId) {
                if (e->archive == 0) {
                    if (LoadArchive(i) != 0)
                        data_0208eb54 = i;
                    if (e->archive == 0) {
                        OS_Terminate();
                        return 0;
                    }
                }
                result = func_0204eda4((int)file, e->archive, id - e->firstFileId);
                break;
            }
            i++;
            e++;
        } while (i < ARCHIVE_TABLE_COUNT);
    }

    if (result == 0)
        OS_Terminate();
    return result;
}
