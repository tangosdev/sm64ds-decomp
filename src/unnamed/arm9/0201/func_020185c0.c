#include "private/arm9_archive_table.h"

/* NitroSDK's FSFileID: the archive and the file id inside it. */
struct FSFileID {
    int arc;
    int file_id;
};

extern void FS_InitFile(int *s);
extern void func_02018598(struct FSFileID *c, int arg);
extern int myFS_OpenFileFast(int a, struct FSFileID v);
extern int LoadArchive(int idx);
extern void Crash(void);
extern int func_0204eda4(int file, char *archive, unsigned int index);

/* Open game file id into the FSFile at file. Ids below 0x8000 are FAT ids
   in the ROM archive (func_02018598 builds the FSFileID); higher ids are
   opened inside the archive whose range holds them, mounting it first if
   needed (and recording it in data_0208eb54). Any failure is fatal. */
int func_020185c0(int file, unsigned int id) {
    int result = 0;

    FS_InitFile((int *)file);

    if (id < 0x8000) {
        struct FSFileID arg;
        struct FSFileID fileId;
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
                        Crash();
                        return 0;
                    }
                }
                result = func_0204eda4(file, e->archive, id - e->firstFileId);
                break;
            }
            i++;
            e++;
        } while (i < ARCHIVE_TABLE_COUNT);
    }

    if (result == 0)
        Crash();
    return result;
}
