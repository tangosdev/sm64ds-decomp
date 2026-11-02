#include "types.h"
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern FSArchive *FS_FindArchive(const char *name, int name_len);
extern int FS_UnloadArchive(FSArchive *p_arc);
extern void FS_SetArchiveProc(FSArchive *p_arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags);
extern int FS_LoadArchive(FSArchive *p_arc, u32 base, u32 fat, u32 fat_size, u32 fnt, u32 fnt_size,
                          FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func);
extern void func_0205db88(int target, void *ptr, u32 size);

extern char data_02099e6c[]; /* "rom" */
extern int func_02042668(FSFile *p_file, int command);
extern int func_020426f8(FSArchive *p_arc, void *dst, u32 pos, u32 size);
extern int func_020426f0(FSArchive *p_arc, const void *src, u32 pos, u32 size);

struct Pair {
    int *f0;
    int f4;
};

/* Child: mount the file-share table block (func_020424c0's layout: a
 * leading word, then the FAT, FNT, ARM9 and ARM7 overlay tables, each as its
 * size word and data rounded up to 0x20) as the "rom" archive.
 * The calls have the NitroSDK FS shapes:
 * - FS_FindArchive finds the archive by name.
 * - FS_UnloadArchive unloads it.
 * - FS_SetArchiveProc installs func_02042668 as its proc for every command.
 * - FS_LoadArchive loads it from the FAT and FNT here, at base 0.
 *   func_020426f8 copies from memory for reads, and func_020426f0 fails
 *   writes.
 * - func_0205db88 attaches the two overlay tables.
 * Returns the block's leading word. */
int func_020423dc(int *r6) {
    int r5;
    FSArchive *h;
    int r3;
    struct Pair arr[4];

    h = FS_FindArchive(data_02099e6c, 3);
    FS_UnloadArchive(h);
    FS_SetArchiveProc(h, &func_02042668, -1);
    r5 = *r6;
    r6++;
    for (r3 = 0; r3 < 4; r3++) {
        arr[r3].f0 = r6 + 1;
        arr[r3].f4 = *r6;
        r6 = (int *)((char *)arr[r3].f0 + ((arr[r3].f4 + 0x1f) & ~0x1f));
    }
    FS_LoadArchive(h, 0, (u32)arr[0].f0, arr[0].f4, (u32)arr[1].f0, arr[1].f4, &func_020426f8,
                   &func_020426f0);
    func_0205db88(0, arr[2].f0, arr[2].f4);
    func_0205db88(1, arr[3].f0, arr[3].f4);
    return r5;
}
