/* NitroSDK FS_LoadArchiveTables(p_arc, p_mem, max_size): if the archive's FAT
 * and FNT fit in max_size (rounded up to 32 bytes, plus 32 for alignment),
 * read each into the 32-byte-aligned buffer through a direct file
 * (func_0205d5e8 = FS_OpenFileDirect over [top, top + size)), point fat / fnt
 * at the copies, switch table reads to memory (func_0205d28c =
 * FSi_ReadMemoryCore) and set FS_ARCHIVE_FLAG_TABLE_LOAD (4). Returns the size
 * the tables need. Field names follow the SDK's FSArchive; only the fields
 * this function touches are spelled out. */
typedef struct FSArchive {
    char pad_00[0x10];
    unsigned int flag; /* 0x10 */
    char pad_14[0xc];
    int fat;      /* 0x20 */
    int fat_size; /* 0x24 */
    int fnt;      /* 0x28 */
    int fnt_size; /* 0x2c */
    char pad_30[8];
    void *load_mem; /* 0x38 */
    char pad_3c[8];
    int (*table_func)(void); /* 0x44 */
} FSArchive;

extern void FS_InitFile(int *s);
extern void func_0205d5e8(int *s, void *this, int a, int b, int e);
extern void FS_ReadFile(int *s, int x, int n);
extern void FS_CloseFile(int *s);
extern int func_0205d28c(void);

unsigned int func_0205c9b8(char *thiz, int arg1, unsigned int arg2) {
    int s[18];
    unsigned int r5;
    int r4;
    int sz;
    FSArchive *p_arc = (FSArchive *)thiz;
    sz = p_arc->fat_size + p_arc->fnt_size + 0x20;
    r5 = ((unsigned int)(sz) + 0x1f) & ~0x1f;
    if (r5 <= arg2) {
        r4 = (arg1 + 0x1f) & ~0x1f;
        FS_InitFile(s);
        func_0205d5e8(s, p_arc, p_arc->fat, p_arc->fat + p_arc->fat_size, -1);
        FS_ReadFile(s, r4, p_arc->fat_size);
        FS_CloseFile(s);
        p_arc->fat = r4;
        r4 = r4 + p_arc->fat_size;
        func_0205d5e8(s, p_arc, p_arc->fnt, p_arc->fnt + p_arc->fnt_size, -1);
        FS_ReadFile(s, r4, p_arc->fnt_size);
        FS_CloseFile(s);
        p_arc->fnt = r4;
        p_arc->load_mem = (void *)arg1;
        p_arc->table_func = func_0205d28c;
        p_arc->flag |= 4;
    }
    return r5;
}
